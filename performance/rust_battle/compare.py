"""Run from any directory after building battle.exe. Use an empty results directory."""
import argparse
import hashlib
import itertools
import json
from pathlib import Path
import statistics
import subprocess
import tempfile

root = Path(__file__).resolve().parents[2]
scenes = ('tx', 'tx_buffs', 'mw', 'mw_equipped')
engines = ('native', 'control', 'rust')
holdout = {
    'tx': ('chain', 'sequence'), 'tx_buffs': ('chain', 'sequence', 'dot'),
    'mw': ('chain', 'sequence', 'interrupt', 'dot'),
    'mw_equipped': ('chain', 'sequence', 'interrupt', 'dot'),
}
parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument('executable', type=Path)
parser.add_argument('output', type=Path)
args = parser.parse_args()
exe = args.executable.resolve()
p = args.output.resolve()
p.mkdir(parents=True, exist_ok=True)
scratch = tempfile.TemporaryDirectory(dir=p)
checks, traces, rows = {}, {}, []

def save(name, obj):
    (p / (name + '.json')).write_text(json.dumps(obj, indent=1), encoding='utf8')

def run(scene, impl, n, workers, macro='', audit=False):
    tracefile = Path(scratch.name) / 'current.bin'
    cmd = [str(exe), impl, scene, str(n), str(workers)]
    if audit:
        cmd += [str(tracefile)]
        if macro:
            cmd += [str(root / 'tests/performance/pgo_holdout' / f'{scene[:2]}_{macro}.txt')]
    t, count, *values = subprocess.check_output(cmd, text=True).split()
    key = f'{scene}/{macro}/{n}'
    if int(count) != n or checks.setdefault(key, values) != values:
        raise ValueError(f'result mismatch: {impl} {key}')
    if audit:
        with tracefile.open('rb') as f:
            record = [tracefile.stat().st_size, hashlib.file_digest(f, 'sha256').hexdigest()]
        if traces.setdefault(key, record) != record:
            raise ValueError(f'trace mismatch: {impl} {key}')
        tracefile.unlink()
    return float(t)

for scene in scenes:
    for macro in ('', *holdout[scene]):
        for impl in engines:
            run(scene, impl, 128 if not macro else 16, 1, macro, True)
save('traces', traces)
for workers, n in [(1, 8192), (20, 100000)]:
    for scene in scenes:
        for impl in engines:
            run(scene, impl, 1024, workers)
    for i, order in enumerate(itertools.permutations(engines)):
        for j in range(len(scenes)):
            scene = scenes[(j+i) % len(scenes)]
            for impl in order:
                rows.append(dict(workers=workers, count=n, block=i, scene=scene, engine=impl,
                                 seconds=run(scene, impl, n, workers)))
        save('times', rows)
        save('checks', checks)
    for scene in scenes:
        times = [[r['seconds'] for r in rows if (r['workers'], r['scene'], r['engine']) ==
                  (workers, scene, impl)] for impl in engines]
        print(workers, scene, [round(statistics.median(ts), 5) for ts in times], flush=True)
