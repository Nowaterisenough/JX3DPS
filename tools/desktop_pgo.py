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
SCENES = ("tx", "tx_buffs", "mw", "mw_equipped")
MODES = ("batch", "gains")


def build(build_dir, mode, jobs, cmake_args):
    subprocess.run([
        "cmake", "-S", str(ROOT), "-B", str(build_dir), "-G", "Ninja", *cmake_args,
        "-DCMAKE_CXX_COMPILER=g++", "-DCMAKE_BUILD_TYPE=Release", "-DBUILD_APP=ON",
        "-DJX3DPS_RUNTIME_ONLY=OFF", "-DJX3DPS_DESKTOP_WORKLOAD=ON", f"-DJX3DPS_DESKTOP_PGO={mode}",
    ], check=True)
    subprocess.run(["cmake", "--build", str(build_dir), "--target", "JX3DPSDesktop", "--parallel", str(jobs)], check=True)
    return Path((build_dir / "desktop-workload-Release.txt").read_text(encoding="utf-8").strip())


def workload(executable, scene, mode, iterations, workers, seed, directory):
    output = directory / "batch.json"
    output.unlink(missing_ok=True)
    command = [str(executable), "--pgo-workload", scene, mode, str(iterations), str(workers), str(seed), str(output)]
    subprocess.run(command, check=True)
    result = json.loads(output.read_text(encoding="utf-8"))
    if result["primary"]["count"] != str(iterations) or len(result["gains"]) != (8 if mode == "gains" else 0):
        raise RuntimeError(f"Incomplete batch for {scene}/{mode}")
    return result


def signature(result):
    return result["totalDamage"], result["primary"], result["gains"]


def compare(build_dir, binaries, rounds, iterations, workers, directory):
    runs, reference, primary = [], {}, {}
    for repeat in range(rounds):
        for i in range(len(SCENES)):
            index = (repeat + i) % len(SCENES)
            scene = SCENES[index]
            for mode in MODES:
                order = ("OFF", "USE") if (repeat + index) % 2 == 0 else ("USE", "OFF")
                for build_mode in order:
                    result = workload(binaries[build_mode], scene, mode, iterations, workers, 42, directory)
                    observed = signature(result)
                    if (reference.setdefault((scene, mode), observed) != observed or
                            primary.setdefault(scene, observed[:2]) != observed[:2]):
                        raise RuntimeError(f"Damage or attribute gain mismatch for {scene}/{mode}, round {repeat + 1}: {result}")
                    runs.append(dict(result, build=build_mode, round=repeat + 1))
        print(f"Completed comparison round {repeat + 1}/{rounds}", flush=True)

    medians = {}
    for scene in SCENES:
        medians[scene] = {}
        for mode in MODES:
            sample = [r for r in runs if r["scene"] == scene and r["mode"] == mode]
            values = {v: statistics.median(r["seconds"] for r in sample if r["build"] == v) for v in binaries}
            medians[scene][mode] = values
            print(f"{scene:12} {mode:5} OFF {values['OFF']:.4f}s  USE {values['USE']:.4f}s")
    report = {"compiler": subprocess.check_output(["g++", "--version"], text=True).splitlines()[0],
              "binary_sha256": {k: hashlib.sha256(path.read_bytes()).hexdigest() for k, path in binaries.items()},
              "fight_seconds": 300, "gain_increment": 100, "medians": medians, "runs": runs}
    path = build_dir / "desktop-pgo-results.json"
    pending = path.with_suffix(".tmp")
    pending.write_text(json.dumps(report, ensure_ascii=False, indent=2), encoding="utf-8")
    pending.replace(path)
    print(f"Verified benchmark results: {path}")


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--build-dir", type=Path, default=ROOT / ".cache" / "desktop-pgo-release")
    parser.add_argument("--benchmark", action="store_true", help="also build OFF and compare both desktop binaries")
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
    if args.benchmark:
        binary = build(build_dir, "OFF", args.jobs, args.cmake_arg)
        baseline = binary.with_name(binary.stem + "-baseline" + binary.suffix)
        shutil.copy2(binary, baseline)
        binaries["OFF"] = baseline

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
