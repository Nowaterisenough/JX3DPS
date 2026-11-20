"""Compare the standalone Rust engines with the project's GCC Release native engine."""
import argparse
import filecmp
import hashlib
import json
import statistics
import subprocess
import tempfile
from pathlib import Path

root = Path(__file__).resolve().parents[3]
parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument("native", type=Path)
parser.add_argument("rust", type=Path)
parser.add_argument("output", type=Path)
args = parser.parse_args()
commands = {"cpp": [args.native.resolve(), "native"], "rust": [args.rust.resolve()]}
output = args.output.resolve()
output.mkdir(parents=True, exist_ok=True)
scenes = {"tx": ("chain","sequence"), "tx_buffs": ("chain","sequence","dot"),
          "mw": ("chain", "sequence", "interrupt", "dot"),
          "mw_equipped": ("chain", "sequence", "interrupt", "dot")}
checks, traces, samples = {}, {}, []

def save(name, data):
    (output / (name + ".json")).write_text(json.dumps(data, indent=1), encoding="utf8")

def run(engine, scene, n, threads, trace=None, macro=""):
    cmd = [*commands[engine], scene, str(n), str(threads)]
    if trace is not None:
        cmd.append(trace)
    if macro:
        cmd.append(root / "tests/performance/pgo_holdout" / f"{scene[:2]}_{macro}.txt")
    elapsed, count, *signature = subprocess.check_output(cmd, text=True).split()
    if int(count) != n or checks.setdefault(f"{scene}/{n}/{macro}", signature) != signature:
        raise ValueError(f"stats mismatch: {engine}/{scene}/{macro}/{n}")
    return float(elapsed)

with tempfile.TemporaryDirectory(dir=output) as scratch:
    paths = [Path(scratch) / f"{e}.bin" for e in commands]
    for scene in scenes:
        for macro in ("", *scenes[scene]):
            n = 512 if not macro else 64
            for engine, path in zip(commands, paths):
                run(engine, scene, n, 1, path, macro)
            if not filecmp.cmp(*paths, shallow=False):
                raise ValueError(f"event mismatch: {scene}/{macro}")
            with paths[0].open("rb") as stream:
                traces[f"{scene}/{macro}"] = dict(n=n, size=paths[0].stat().st_size,
                                                 sha256=hashlib.file_digest(stream, "sha256").hexdigest())
save("traces", traces)
for threads, n in ((1, 8192), (20, 100000)):
    for scene in scenes:
        for engine in commands:
            run(engine, scene, 1024, threads)
    for block in range(10):
        for j in range(4):
            scene = list(scenes)[(j+block) % 4]
            order = list(commands)
            if (block + list(scenes).index(scene)) % 2:
                order.reverse()
            for engine in order:
                samples.append(dict(engine=engine, scene=scene, threads=threads, block=block, n=n,
                                    seconds=run(engine, scene, n, threads)))
        save("times", samples)
        save("checks", checks)
    for scene in scenes:
        times = [[r["seconds"] for r in samples if (r["scene"],r["threads"],r["engine"]) ==
                  (scene,threads,e)] for e in commands]
        a,b=times
        print(threads,scene,statistics.median(a),statistics.median(b),
              statistics.median([100*(1-y/x) for x,y in zip(a,b)]),sum(y<x for x,y in zip(a,b)),flush=True)
