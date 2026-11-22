"""Compare native fights with the published flagship dummy cycles (16 frames/second)."""
import argparse
import ctypes
import json
import math
import os
from contextlib import ExitStack
from pathlib import Path

REFERENCE = Path(__file__).resolve().parents[1] / "data/jx3box/season50_rotation_reference.json"


def compare(call, code, reference, iterations, seed, config=None):
    seconds = reference["seconds"]
    frames = round(seconds * 16)
    if frames != seconds * 16:
        raise ValueError(f"{code}: reference duration is not frame-aligned")
    result = call({"op": "batch", "scenario": code, "iterations": iterations, "seed": str(seed),
                   "workers": 1, "frames": frames, "config": config or {}})
    measured = {row["name"]: row for row in result["skills"]}
    rows = []
    for entry in reference["skills"]:
        parts = [measured[n] for n in entry.get("local_names", [entry["name"]]) if n in measured]
        hits = sum(part["count"] for part in parts)
        damage = sum(int(part["damage"]) for part in parts)
        rows.append({"skill": entry["name"], "online_count": entry["count"], "local_count": hits / iterations,
                     "online_per_hit": entry["total_damage"] / entry["count"], "local_per_hit": damage / hits if hits else None})
    return {"school": code, "frames": frames, "online_dps": reference["online_dps"],
            "local_dps": result["stats"]["mean_damage"] / seconds, "skills": rows}


def main():
    references = json.loads(REFERENCE.read_text(encoding="utf-8"))["schools"]
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--library", required=True, type=Path, help="Native shared JX3DPS library")
    parser.add_argument("--school", choices=references, action="append", help="Repeat to compare several schools")
    parser.add_argument("--iterations", default=128, type=int)
    parser.add_argument("--seed", default=42, type=int)
    parser.add_argument("--distance", type=float, default=4, help="Distance from the dummy, in game yards")
    parser.add_argument("--fsj-ablation", action="store_true", help="Repeat the Fen Shan fight, changing one rule at a time")
    args = parser.parse_args()
    if not math.isfinite(args.distance) or args.distance < 0:
        parser.error("distance must be finite and nonnegative")
    if args.iterations < 1 or not 0 <= args.seed < 2**64:
        parser.error("expected positive iterations and unsigned 64-bit seed")
    path = args.library.resolve(strict=True)
    with ExitStack() as stack:
        if os.name == "nt":
            stack.enter_context(os.add_dll_directory(str(path.parent)))
            for directory in os.get_exec_path():
                candidate = Path(directory.strip('"'))
                if (candidate / "libstdc++-6.dll").is_file():
                    stack.enter_context(os.add_dll_directory(str(candidate.resolve())))
                    break
        lib = ctypes.CDLL(str(path))
        lib.jx3dps_create.restype = ctypes.c_void_p
        lib.jx3dps_destroy.argtypes = [ctypes.c_void_p]
        lib.jx3dps_destroy.restype = None
        lib.jx3dps_call.argtypes = [ctypes.c_void_p, ctypes.c_char_p]
        lib.jx3dps_call.restype = ctypes.c_char_p
        handle = lib.jx3dps_create()
        if not handle:
            raise RuntimeError("could not create engine")
        stack.callback(lib.jx3dps_destroy, handle)

        def call(request):
            result = json.loads(lib.jx3dps_call(handle, json.dumps(request).encode("utf-8")))
            if "error" in result:
                raise RuntimeError(result["error"])
            return result

        target = {"target": {"distance": args.distance}}
        results = [compare(call, code, references[code], args.iterations, args.seed, target) for code in args.school or references]
        output = {"iterations": args.iterations, "seed": args.seed, "distance": args.distance, "results": results}
        if args.fsj_ablation:
            fsj = next(book for book in call({"op": "schools"}) if book["code"] == "fsj")
            trials = {"默认": {}, "无绝刀减耗": {"recipes": [x for x in fsj["defaultRecipes"] if x != "绝刀·怒气消耗-15"]},
                      "无预铺": {"precombat": False}, "延迟一帧": {"delay": 1}}
            for talent in ("残裂", "锋鸣"):
                trials["无" + talent] = {"talents": [x for x in fsj["defaultTalents"] if x != talent]}
            output["fsj_ablation"] = {}
            for label, config in trials.items():
                outcome = compare(call, "fsj", references["fsj"], args.iterations, args.seed, target | config)
                blade = next(row for row in outcome["skills"] if row["skill"] == "绝刀")
                output["fsj_ablation"][label] = {"dps": outcome["local_dps"], **blade}
        print(json.dumps(output, ensure_ascii=False, indent=2))


if __name__ == "__main__":
    main()
