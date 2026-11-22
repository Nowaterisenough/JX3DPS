"""Measure the shipped desktop or browser entry point, with the same workload."""
import argparse
import functools
import json
import platform
import subprocess
import tempfile
import threading
from http.server import SimpleHTTPRequestHandler, ThreadingHTTPServer
from pathlib import Path

SCENES = ("tx", "mw", "gfj", "yly")
FIGHTS, SECONDS, SEED, REPEATS = 100000, 300, "42", 3


def native(exe, output, scene, count):
    subprocess.run([exe, "--pgo-workload", scene, "batch", str(count), "1", SEED, output],
                   check=True, timeout=360)
    r = json.loads(output.read_text(encoding="utf8"))
    return {"elapsed": r["seconds"], "count": int(r["primary"]["count"]),
            "checksum": r["primary"]["checksum"], "damage": r["totalDamage"]}


def collect(run):
    rows = {}
    for scene in SCENES:
        run(scene, 256)
        samples = [run(scene, FIGHTS) for _ in range(REPEATS)]
        first = samples[0]
        if any(s["count"] != FIGHTS or (s["checksum"], s["damage"]) !=
               (first["checksum"], first["damage"]) for s in samples):
            raise ValueError(f"inconsistent simulation: {scene}")
        rows[scene] = {"seconds": [s["elapsed"] for s in samples],
                       "damage": first["damage"], "checksum": first["checksum"]}
    return rows


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
            page.evaluate("""async () => {
                const w = new Worker('./ui/worker.js', {type:'module'});
                window.simWorker = w;
                window.macros = await new Promise((resolve,reject)=>{
                    const timeout=setTimeout(()=>reject(Error("WASM initialization timeout")),60000);
                    w.onerror=e=>{clearTimeout(timeout);reject(Error(e.message));};
                    w.onmessage=e=>{clearTimeout(timeout);if(e.data.type==='ready')resolve(e.data.defaults);else reject(Error(e.data.error));};
                });
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
            result = collect(once)
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
    parser.add_argument("--channel", help="Installed Chromium channel, e.g. msedge")
    for name in ("build","compiler","runner","sha","output"): parser.add_argument("--"+name,required=True)
    args=parser.parse_args()
    if args.site:
        rows,version=browser(args.site.resolve(),args.channel)
    else:
        with tempfile.TemporaryDirectory() as tmp:
            rows=collect(lambda scene,n:native(args.desktop.resolve(),Path(tmp)/"run.json",scene,n))
        version=None
    report=dict(build=args.build,compiler=args.compiler,runner=args.runner,sha=args.sha,cpu=cpu_model(),
                arch=platform.machine(),browser=version,workers=1,iterations=FIGHTS,duration=SECONDS,seed=SEED,
                warmup=256,repeats=REPEATS,scenes=rows)
    output=Path(args.output)
    output.parent.mkdir(parents=True,exist_ok=True)
    output.write_text(json.dumps(report,ensure_ascii=False,indent=2)+"\n",encoding="utf8")
