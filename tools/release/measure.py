"""Measure the shipped desktop or browser entry point, with the same workload."""
import argparse
import functools
import json
import os
import platform
import math
import shutil
import subprocess
import tempfile
import threading
from http.server import SimpleHTTPRequestHandler, ThreadingHTTPServer
from pathlib import Path

FIGHTS, SECONDS, SEED, REPEATS = 100000, 300, "42", 3


def native(exe, output, scene, count):
    subprocess.run([exe, "--pgo-workload", scene, "batch", str(count), "1", SEED, output],
                   check=True, timeout=360)
    r = json.loads(output.read_text(encoding="utf8"))
    return {"elapsed": r["seconds"], "count": int(r["primary"]["count"]),
            "checksum": r["primary"]["checksum"], "damage": r["totalDamage"]}


def desktop_schools(exe):
    return json.loads(subprocess.check_output([exe, "--pgo-workload", "--list-schools"], encoding="utf8"))


def collect(schools, run, baseline=None, fights=FIGHTS, repeats=REPEATS):
    if not schools:
        raise ValueError("empty school catalog")
    rows = {}
    for index, (scene, name) in enumerate(schools.items()):
        run(scene, 256)
        if baseline:
            baseline(scene, 256)
        row = {"name": name, "seconds": []}
        if baseline:
            row["baseline_seconds"] = []
        reference = None
        for repeat in range(repeats):
            variants = [("seconds", run)]
            if baseline:
                variants.append(("baseline_seconds", baseline))
                if (repeat + index) % 2:
                    variants.reverse()
            for key, sample in variants:
                result = sample(scene, fights)
                signature = (result["damage"], result["checksum"])
                if reference is None:
                    reference = signature
                if (result["count"] != fights or signature != reference or
                        not math.isfinite(result["elapsed"]) or result["elapsed"] <= 0):
                    raise ValueError(f"inconsistent {key} sample: {scene}, round {repeat + 1}")
                row[key].append(result["elapsed"])
        row["damage"], row["checksum"] = reference
        rows[scene] = row
        print(f"Measured {scene} ({index + 1}/{len(schools)})", flush=True)
    return rows


def train_release_gcc(build_dir, exe, output, schools):
    """Train the exact CI build directory; keep its selected compiler, Qt and package identity."""
    def rebuild(mode):
        subprocess.run(["cmake", "-S", str(Path(__file__).resolve().parents[2]), "-B", build_dir,
                        f"-DJX3DPS_DESKTOP_PGO={mode}"], check=True)
        subprocess.run(["cmake", "--build", build_dir, "--target", "JX3DPSDesktop"], check=True)

    rebuild("GENERATE")
    # GCC otherwise merges stale execution counts on a repeated build.
    profiles = [build_dir / "src/CMakeFiles/JX3DPS.dir/engine/engine.cpp.gcda",
                build_dir / "apps/desktop/CMakeFiles/DesktopWorkbench.dir/batch_worker.cpp.gcda"]
    for path in profiles:
        path.unlink(missing_ok=True)
    for scene in schools:
        for mode, count in (("batch", 512), ("gains", 64)):
            subprocess.run([exe, "--pgo-workload", scene, mode, str(count), "1", "713", output], check=True, timeout=360)
    for path in profiles:
        if not path.is_file() or not path.stat().st_size:
            raise ValueError(f"missing GCC training data: {path}")
    rebuild("USE")


def browser(site, channel):
    from playwright.sync_api import sync_playwright
    class Static(SimpleHTTPRequestHandler):
        def log_message(self, *args): pass
    handler = functools.partial(Static, directory=str(site))
    with ThreadingHTTPServer(("127.0.0.1", 0), handler) as server, sync_playwright() as pw:
        thread = threading.Thread(target=server.serve_forever, daemon=True)
        thread.start()
        try:
            client = pw.chromium.launch(channel=channel, headless=True)
            page = client.new_page()
            # A same-origin blank asset avoids starting the UI's automatic first run.
            page.goto(f"http://127.0.0.1:{server.server_port}/icons.json")
            schools = page.evaluate("""async () => {
                const w = new Worker('./ui/worker.js', {type:'module'});
                window.simWorker = w;
                const ready = await new Promise((resolve,reject)=>{
                    const timeout=setTimeout(()=>reject(Error("WASM initialization timeout")),60000);
                    w.onerror=e=>{clearTimeout(timeout);reject(Error(e.message));};
                    w.onmessage=e=>{clearTimeout(timeout);if(e.data.type==='ready')resolve(e.data);else reject(Error(e.data.error));};
                });
                window.macros = ready.defaults;
                return Object.fromEntries(ready.schools.map(s => [s.code, s.name]));
            }""")
            def once(scene, count):
                return page.evaluate("""async args => {
                    const [scene,iterations,seconds,seed] = args;
                    const worker = window.simWorker;
                    return new Promise((resolve,reject)=>{
                        const timer=setTimeout(()=>reject(Error('WASM benchmark timeout')),360000);
                        worker.onerror=e=>{clearTimeout(timer);reject(Error(e.message));};
                        worker.onmessage=({data})=>{
                            if(data.type==='progress')return;
                            clearTimeout(timer);
                            if(data.type!=='result'){reject(Error(data.error || data.type));return;}
                            const r=data.result;
                            resolve({elapsed:data.elapsed,count:r.completed,checksum:r.checksum,
                                damage:r.skills.reduce((n,s)=>n+BigInt(s.damage),0n).toString()});
                        };
                        worker.postMessage({type:'run',request:{scenario:scene,iterations,seconds,seed,
                            attributes:{},macro:window.macros[scene]}});
                    });
                }""", [scene,count,SECONDS,SEED])
            result = collect(schools, once)
            return result, client.version
        finally:
            server.shutdown()
            thread.join()


def cpu_model():
    if Path("/proc/cpuinfo").is_file():
        for line in Path("/proc/cpuinfo").read_text().splitlines():
            if line.startswith("model name"): return line.split(":",1)[1].strip()
    if platform.system()=="Darwin":
        return subprocess.check_output(["sysctl","-n","machdep.cpu.brand_string"],text=True).strip()
    return platform.processor() or platform.machine()


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    group=parser.add_mutually_exclusive_group(required=True)
    group.add_argument("--desktop",type=Path)
    group.add_argument("--site",type=Path)
    parser.add_argument("--train-gcc-build", type=Path, help="Train GCC and compare with the original static Release executable")
    parser.add_argument("--channel", help="Installed Chromium channel, e.g. msedge")
    for name in ("build","compiler","runner","sha","output"): parser.add_argument("--"+name,required=True)
    args=parser.parse_args()
    if args.train_gcc_build and (not args.desktop or not args.build.endswith("-gcc")):
        parser.error("--train-gcc-build requires a GCC desktop build")
    if args.site:
        rows,version=browser(args.site.resolve(),args.channel)
    else:
        with tempfile.TemporaryDirectory() as tmp:
            exe = args.desktop.resolve()
            output = Path(tmp) / "run.json"
            schools = desktop_schools(exe)
            def timed(scene, count):
                return native(exe, output, scene, count)
            if args.train_gcc_build:
                build_dir = args.train_gcc_build.resolve()
                cache = (build_dir / "CMakeCache.txt").read_text(encoding="utf8").splitlines()
                if "JX3DPS_SHARED_ENGINE:BOOL=OFF" not in cache or "JX3DPS_DESKTOP_PGO:STRING=OFF" not in cache:
                    raise ValueError("paired GCC benchmark requires the initial static, non-PGO Release build")
                # Use the same loader search paths as the shipped executable. Remove the
                # temporary baseline before build/bin is archived for release.
                fd, path = tempfile.mkstemp(prefix="benchmark-off-", suffix=exe.suffix, dir=exe.parent)
                os.close(fd)
                baseline = Path(path)
                try:
                    shutil.copy2(exe, baseline)
                    train_release_gcc(build_dir, exe, output, schools)
                    rows = collect(schools, timed, lambda scene, n: native(baseline, output, scene, n))
                finally:
                    baseline.unlink(missing_ok=True)
            else:
                rows = collect(schools, timed)
        version=None
    report=dict(build=args.build,compiler=args.compiler,runner=args.runner,sha=args.sha,cpu=cpu_model(),
                arch=platform.machine(),browser=version,workers=1,iterations=FIGHTS,duration=SECONDS,seed=SEED,
                warmup=256,repeats=REPEATS,scenes=rows,
                optimization="GCC PGO" if args.train_gcc_build else "Release")
    output=Path(args.output)
    output.parent.mkdir(parents=True,exist_ok=True)
    output.write_text(json.dumps(report,ensure_ascii=False,indent=2)+"\n",encoding="utf8")
