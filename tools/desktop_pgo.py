"""Build a GCC Release desktop with trained batch workers and optionally benchmark it."""

import argparse
import hashlib
import json
import os
from pathlib import Path
import shutil
import statistics
import subprocess
import tempfile
import time


ROOT = Path(__file__).resolve().parents[1]
SCENES = ("tx", "mw", "gfj", "yly")
MODES = ("batch", "gains")
HOLDOUT_MACROS = {
    "legacy_tx": ("tx_chain", "tx_sequence"),
    "tx_buffs": ("tx_chain", "tx_sequence", "tx_dot"),
    "legacy_mw": ("mw_chain", "mw_sequence", "mw_interrupt", "mw_dot"),
    "mw_equipped": ("mw_chain", "mw_sequence", "mw_interrupt", "mw_dot"),
}


def build(build_dir, mode, jobs, cmake_args):
    subprocess.run([
        "cmake", "-S", str(ROOT), "-B", str(build_dir), "-G", "Ninja", *cmake_args,
        "-DCMAKE_CXX_COMPILER=g++", "-DCMAKE_BUILD_TYPE=Release", "-DBUILD_APP=ON",
        "-DJX3DPS_RUNTIME_ONLY=OFF", "-DJX3DPS_DESKTOP_WORKLOAD=ON", f"-DJX3DPS_DESKTOP_PGO={mode}",
    ], check=True)
    subprocess.run(["cmake", "--build", str(build_dir), "--target", "JX3DPSDesktop", "--parallel", str(jobs)], check=True)
    return Path((build_dir / "desktop-workload-Release.txt").read_text(encoding="utf-8").strip())


def workload(executable, scene, mode, iterations, workers, seed, directory, macro=None):
    output = directory / "batch.json"
    output.unlink(missing_ok=True)
    command = [str(executable), "--pgo-workload", scene, mode, str(iterations), str(workers), str(seed), str(output)]
    if macro is not None:
        command.append(str(macro))
    subprocess.run(command, check=True)
    result = json.loads(output.read_text(encoding="utf-8"))
    if result["primary"]["count"] != str(iterations) or len(result["gains"]) != (8 if mode == "gains" else 0):
        raise RuntimeError(f"Incomplete batch for {scene}/{mode}")
    return result


def signature(result):
    return result["totalDamage"], result["primary"], result["gains"]


def compare(build_dir, binaries, rounds, iterations, workers, directory, profile=None):
    runs, reference, primary = [], {}, {}
    cases = [(scene, None) for scene in SCENES]
    metadata = {}
    filename = "desktop-pgo-results.json"
    if profile is not None:
        filename = "desktop-pgo-holdout.json"
        profile_file, digest = profile
        cases = []
        hashes = {}
        for scene, names in HOLDOUT_MACROS.items():
            for name in names:
                macro = directory / f"{name}.txt"
                if name not in hashes:
                    shutil.copyfile(ROOT / "tests/performance/pgo_holdout" / macro.name, macro)
                    hashes[name] = hashlib.sha256(macro.read_bytes()).hexdigest()
                cases.append((scene, macro))
        metadata = {"profile_sha256": digest, "macro_sha256": hashes}
    for repeat in range(rounds):
        for i in range(len(cases)):
            index = (repeat + i) % len(cases)
            scene, macro = cases[index]
            case = scene if macro is None else f"{scene}/{macro.stem}"
            for mode in MODES:
                order = ("OFF", "USE") if (repeat + index) % 2 == 0 else ("USE", "OFF")
                for build_mode in order:
                    result = workload(binaries[build_mode], scene, mode, iterations, workers, 42, directory, macro)
                    observed = signature(result)
                    if (reference.setdefault((case, mode), observed) != observed or
                            primary.setdefault(case, observed[:2]) != observed[:2]):
                        raise RuntimeError(f"Damage or attribute gain mismatch for {case}/{mode}, round {repeat + 1}: {result}")
                    if macro is not None:
                        result["macro"] = macro.stem
                    runs.append(dict(result, build=build_mode, round=repeat + 1))
        print(f"Completed comparison round {repeat + 1}/{rounds}", flush=True)

    medians = {}
    for scene, macro in cases:
        case = scene if macro is None else f"{scene}/{macro.stem}"
        medians[case] = {}
        for mode in MODES:
            sample = [r for r in runs if r["scene"] == scene and r["mode"] == mode and
                      (macro is None or r["macro"] == macro.stem)]
            values = {v: statistics.median(r["seconds"] for r in sample if r["build"] == v) for v in binaries}
            medians[case][mode] = values
            print(f"{case:24} {mode:5} OFF {values['OFF']:.4f}s  USE {values['USE']:.4f}s")
    if profile is not None and hashlib.sha256(profile_file.read_bytes()).hexdigest() != digest:
        raise RuntimeError("GCC profile changed during holdout acceptance")
    report = {"compiler": subprocess.check_output(["g++", "--version"], text=True).splitlines()[0],
              "binary_sha256": {k: hashlib.sha256(path.read_bytes()).hexdigest() for k, path in binaries.items()},
              "fight_seconds": 300, "gain_increment": 100, "medians": medians, "runs": runs, **metadata}
    path = build_dir / filename
    pending = path.with_suffix(".tmp")
    pending.write_text(json.dumps(report, ensure_ascii=False, indent=2), encoding="utf-8")
    pending.replace(path)
    print(f"Verified benchmark results: {path}")


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--build-dir", type=Path, default=ROOT / ".cache" / "desktop-pgo-release")
    mode = parser.add_mutually_exclusive_group()
    mode.add_argument("--benchmark", action="store_true", help="also build OFF and compare both desktop binaries")
    mode.add_argument("--holdout", action="store_true", help="compare OFF/USE on held-out macros with the existing profile")
    parser.add_argument("--rounds", type=int, default=4)
    parser.add_argument("--iterations", type=int, default=100_000, help="measured fights per scenario and mode")
    parser.add_argument("--workers", type=int, default=20, help="simulation workers during measurement")
    parser.add_argument("--jobs", type=int, default=4, help="parallel compiler jobs")
    parser.add_argument("--cmake-arg", action="append", default=[], help="extra CMake argument, e.g. --cmake-arg=-DQt6_DIR=...")
    args = parser.parse_args()
    if min(args.rounds, args.iterations, args.workers, args.jobs) < 1:
        parser.error("rounds, iterations, workers and jobs must be positive")

    build_dir = args.build_dir.resolve()
    build_dir.mkdir(parents=True, exist_ok=True)
    # The three configurations share object paths and GCC's training profile.
    with (build_dir / "desktop-pgo.lock").open("a+b") as lock:
        lock.seek(0)
        try:
            if os.name == "nt":
                import msvcrt
                msvcrt.locking(lock.fileno(), msvcrt.LK_NBLCK, 1)
            else:
                import fcntl
                fcntl.flock(lock, fcntl.LOCK_EX | fcntl.LOCK_NB)
        except OSError as error:
            raise RuntimeError(f"Cannot lock PGO build directory {build_dir}: {error}") from error
        train_and_build(build_dir, args)


def train_and_build(build_dir, args):
    # Version.cmake uses the configure timestamp. Give all three stages the same build identity.
    os.environ.setdefault("SOURCE_DATE_EPOCH", str(int(time.time())))
    binaries = {}
    profile = build_dir / "apps/desktop/CMakeFiles/DesktopWorkbench.dir/batch_worker.cpp.gcda"
    if args.holdout:
        if not profile.is_file() or not profile.stat().st_size:
            raise RuntimeError(f"Holdout acceptance needs an existing trained profile at {profile}")
        digest = hashlib.sha256(profile.read_bytes()).hexdigest()
    if args.benchmark or args.holdout:
        binary = build(build_dir, "OFF", args.jobs, args.cmake_arg)
        baseline = binary.with_name(binary.stem + "-baseline" + binary.suffix)
        shutil.copy2(binary, baseline)
        binaries["OFF"] = baseline

    if args.holdout:
        binaries["USE"] = build(build_dir, "USE", args.jobs, args.cmake_arg)
        with tempfile.TemporaryDirectory(dir=build_dir) as temporary:
            compare(build_dir, binaries, args.rounds, args.iterations, args.workers, Path(temporary), (profile, digest))
        return

    binary = build(build_dir, "GENERATE", args.jobs, args.cmake_arg)
    profile.unlink(missing_ok=True)
    with tempfile.TemporaryDirectory(dir=build_dir) as temporary:
        directory = Path(temporary)
        training = {}
        for scene in SCENES:
            for mode in MODES:
                training[scene, mode] = signature(workload(binary, scene, mode, 1024, 4, 713, directory))
        if not profile.is_file() or profile.stat().st_size == 0:
            raise RuntimeError(f"GCC did not write the batch worker profile to {profile}")
        print(f"Trained both desktop paths; profile size {profile.stat().st_size} bytes", flush=True)
        binary = build(build_dir, "USE", args.jobs, args.cmake_arg)
        for (scene, mode), expected in training.items():
            if signature(workload(binary, scene, mode, 1024, 4, 713, directory)) != expected:
                raise RuntimeError(f"Trained binary failed replay for {scene}/{mode}")
        if args.benchmark:
            binaries["USE"] = binary
            compare(build_dir, binaries, args.rounds, args.iterations, args.workers, directory)
    print(f"PGO desktop: {binary}")


if __name__ == "__main__":
    main()
