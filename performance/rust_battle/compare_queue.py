"""Time each queue on operations captured from 64 complete fights per scenario."""
import json
import statistics
import subprocess
import sys
import tempfile
from pathlib import Path

p = Path(sys.argv[1]).resolve()
out = Path(sys.argv[2]).resolve()
out.mkdir(parents=True, exist_ok=True)
rows = []
with tempfile.TemporaryDirectory(dir=out) as scratch:
    path = Path(scratch)
    for scene in ('tx', 'tx_buffs', 'mw', 'mw_equipped'):
        subprocess.run([p/'capture.exe', 'control', scene, '64', '1'], check=True,
                       cwd=path, stdout=subprocess.DEVNULL)
        checks = set()
        for i in range(8):
            order = ('cpp', 'rust') if i%2 == 0 else ('rust', 'cpp')
            for engine in order:
                line = subprocess.check_output([p/f'queue-{engine}.exe', path/'queue.bin', '64'],
                                               text=True)
                seconds, check = line.split()
                checks.add(check)
                if len(checks) != 1:
                    raise ValueError(f'queue order mismatch: {scene}')
                rows.append(dict(scene=scene, engine=engine, block=i, seconds=float(seconds), checksum=check))
        print(scene, [statistics.median(r['seconds'] for r in rows if r['scene']==scene and
                                        r['engine']==eng) for eng in ('cpp', 'rust')], flush=True)
        (out/'queues.json').write_text(json.dumps(rows, indent=1), encoding='utf8')
