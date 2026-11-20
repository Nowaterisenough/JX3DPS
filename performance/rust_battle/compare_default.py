"""Compare the standalone, fixed Tai Xu rotation to the existing native simulator."""
from pathlib import Path
import itertools, json, statistics, subprocess, sys, tempfile

p = Path(sys.argv[1]).resolve()
out = Path(sys.argv[2]).resolve()
out.mkdir(parents=True, exist_ok=True)
engines = {'native': [p/'battle.exe', 'native', 'tx'], 'fixed-cpp': [p/'tx-cpp.exe'], 'rust': [p/'tx-rust.exe']}
checks, rows = {}, []
def run(impl, n, threads, *extra):
    line = subprocess.check_output([*engines[impl], str(n), str(threads), *extra], text=True)
    time, count, *sig = line.split()
    if int(count) != n or checks.setdefault(n, sig) != sig:
        raise ValueError(f'default Tai Xu results differ: {impl}/{n}')
    return float(time)
with tempfile.TemporaryDirectory(dir=out) as temp:
    paths = [Path(temp)/f'{impl}.bin' for impl in engines]
    for impl, path in zip(engines, paths):
        run(impl, 512, 1, path)
    import filecmp, hashlib
    if not all(filecmp.cmp(paths[0], path, shallow=False) for path in paths[1:]):
        raise ValueError('default Tai Xu traces differ')
    with paths[0].open('rb') as f:
        trace = dict(count=512, size=paths[0].stat().st_size, sha256=hashlib.file_digest(f,'sha256').hexdigest())
    (out/'default-trace.json').write_text(json.dumps(trace, indent=1))
for threads, n in [(1,8192),(20,100000)]:
    for impl in engines: run(impl, 1024, threads)
    for block, order in enumerate(list(itertools.permutations(engines))*2):
        for impl in order:
            rows.append(dict(engine=impl,threads=threads,block=block,count=n,seconds=run(impl,n,threads)))
        (out/'default-times.json').write_text(json.dumps(rows,indent=1))
    print(threads, [(impl,statistics.median(r['seconds'] for r in rows if r['engine']==impl and r['threads']==threads))
                    for impl in engines],flush=True)
(out/'default-checks.json').write_text(json.dumps(checks,indent=1))
