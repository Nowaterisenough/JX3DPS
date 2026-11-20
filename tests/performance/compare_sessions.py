"""Compare two shared engines through the C ABI used by the Web worker.

On Windows place compiler runtime DLLs beside the engines or pass --dll-dir.
"""

import argparse
import ctypes
import json
import os
import statistics
import time
from contextlib import ExitStack, contextmanager
from pathlib import Path


class Engine:
    def __init__(self, path):
        self.lib = ctypes.CDLL(str(path.resolve()))
        self.lib.jx3dps_create.restype = ctypes.c_void_p
        self.lib.jx3dps_destroy.argtypes = [ctypes.c_void_p]
        self.lib.jx3dps_destroy.restype = None
        self.lib.jx3dps_call.argtypes = [ctypes.c_void_p, ctypes.c_char_p]
        self.lib.jx3dps_call.restype = ctypes.c_char_p

    @contextmanager
    def session(self):
        handle = self.lib.jx3dps_create()
        if not handle:
            raise MemoryError("jx3dps_create")

        def call(request):
            response = json.loads(self.lib.jx3dps_call(handle, request))
            if "error" in response:
                raise ValueError(response["error"])
            return response

        try:
            yield call
        finally:
            self.lib.jx3dps_destroy(handle)


def run(engine, request):
    start = time.perf_counter()
    with engine.session() as call:
        result = call(request)
        while "checksum" not in result:
            result = call(b'{"op":"next"}')
    return time.perf_counter() - start, result


def main(resources):
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("before", type=Path)
    parser.add_argument("after", type=Path)
    if os.name == "nt":
        parser.add_argument("--dll-dir", type=Path, help="Directory containing the compiler runtime DLLs")
    parser.add_argument("--iterations", type=int, default=8192)
    parser.add_argument("--repeats", type=int, default=8)
    args = parser.parse_args()
    if not 1 <= args.iterations <= 100000 or args.repeats < 1:
        parser.error("iterations must be 1..100000 and repeats must be positive")
    if os.name == "nt" and args.dll_dir:
        resources.enter_context(os.add_dll_directory(args.dll_dir.resolve()))
    engines = [Engine(args.before), Engine(args.after)]
    defaults = []
    for engine in engines:
        with engine.session() as call:
            defaults.append(call(b'{"op":"defaults"}'))
    if defaults[0] != defaults[1]:
        raise ValueError("Different default macros")
    report = {}
    for scene in ("tx", "tx_buffs", "mw", "mw_equipped"):
        request = dict(op="start", scenario=scene, seconds=300, seed="42", macro=defaults[0][scene[:2]])
        for engine in engines:
            run(engine, json.dumps(dict(request, iterations=256)).encode())
        encoded = json.dumps(dict(request, iterations=args.iterations), ensure_ascii=False).encode()
        expected = None
        samples = [[], []]
        for block in range(args.repeats):
            for i in ((0, 1) if block % 2 == 0 else (1, 0)):
                elapsed, result = run(engines[i], encoded)
                if expected is None:
                    expected = result
                if result != expected or result["completed"] != args.iterations:
                    raise ValueError(f"Battle result mismatch for {scene}, block {block}, engine {i}")
                samples[i].append(elapsed)
        savings = [100 * (1 - b / a) for a, b in zip(*samples)]
        report[scene] = dict(before_s=samples[0], after_s=samples[1], median_saved_pct=statistics.median(savings),
                             checksum=expected["checksum"], damage=sum(int(row["damage"]) for row in expected["skills"]))
    print(json.dumps(report, indent=2))


if __name__ == "__main__":
    with ExitStack() as resources:
        main(resources)
