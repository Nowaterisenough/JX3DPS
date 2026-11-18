"""Compare complete legacy damage traces and random draws for team transitions."""
import argparse
import csv
import json
import pathlib
import subprocess


def events(path):
    with path.open(encoding="utf-8-sig", newline="") as stream:
        rows = list(csv.DictReader(stream))
    result = []
    for row in rows:
        if int(row["damage"]) <= 0:
            continue
        name = row["name"].removeprefix("Dot\u00b7").removesuffix("\u00b7\u795e\u5175")
        if name == "\u7834":
            name += "\u62db"
        result.append((int(row["frame"]), name, int(row["damage"]), int(row["outcome"])))
    return sorted(result)


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--build", type=pathlib.Path, required=True)
    parser.add_argument("--output", type=pathlib.Path, required=True)
    args = parser.parse_args()
    args.output.mkdir(parents=True, exist_ok=False)
    catalog = [("han_ru_lei", 1), ("po_feng", 1), ("jing_feng", 1), ("jie_huo", 1),
               ("chao_sheng", 24), ("sheng_yu_ming_xin", 24), ("zhen_fen", 125),
               ("han_xiao_qian_jun", 1), ("shu_kuang", 1), ("han_chang_lin_li", 1),
               ("ji_lei", 1), ("nong_mei", 1), ("she_shen_hong_fa", 36)]
    cases = []
    for name, stacks in catalog:
        cases.append((name, f"{name}:0:57:1;{name}:23:43:{stacks};{name}:97:0:1;{name}:131:0:0"))
    mixed = []
    for cycle in range(5):
        for i, (name, stacks) in enumerate(catalog):
            mixed.append(f"{name}:{cycle*901+i*3}:137:{stacks}")
    cases.append(("mixed", ";".join(mixed)))
    cases.append(("hao_ling_san_jun", "hao_ling_san_jun:0:0:48;hao_ling_san_jun:57:0:24;hao_ling_san_jun:977:0:2"))
    # Forced events at DOT ticks are deterministic in the old scheduler as well.
    cases.append(("same_frame_apply", "ji_lei:48:43:1;nong_mei:96:43:1;jie_huo:144:43:1"))
    results = []
    for case, schedule in cases:
        for phase in (0, 7, 15):
            stem = f"{case}-p{phase}"
            tape = args.output / f"{stem}-draws.json"
            base = ["--macro", str(pathlib.Path(__file__).parent / "macros/rotation.txt"),
                    "--iterations", "1", "--seconds", "300", "--warmup", "0", "--crit", "20000",
                    "--precision", "1", "--phase", str(phase), "--initial-qidian", "0", "--dot", "1",
                    "--draws", str(tape), "--team-buffs", schedule]
            runs = []
            for engine in ("legacy_record", "runtime_replay"):
                trace = args.output / f"{stem}-{engine}.csv"
                run = subprocess.run([str(args.build / f"{engine}.exe"), *base, "--trace", str(trace)],
                                     capture_output=True, encoding="utf-8", check=True)
                result = json.loads(run.stdout)
                (args.output / f"{stem}-{engine}.json").write_text(run.stdout, encoding="utf-8")
                damage_events = events(trace)
                assert sum(row[2] for row in damage_events) == result["damage_sum"], stem
                runs.append((result, damage_events))
            old, new = runs
            if old[1] != new[1]:
                differences = list(zip(old[1], new[1]))
                first = next(((a, b) for a, b in differences if a != b), None)
                raise AssertionError(f"{stem}: {len(old[1])}/{len(new[1])} events; first difference {first}")
            assert old[0]["damage_sum"] == new[0]["damage_sum"], stem
            assert old[0]["draws"] == new[0]["draws"] and new[0]["unused_draws"] == 0, stem
            results.append({"case": stem, "events": len(old[1]), "draws": new[0]["draws"], "matched": True})
    (args.output / "summary.json").write_text(json.dumps(results, indent=2), encoding="utf-8")
    print(f"Matched {len(results)} fights, {sum(r['events'] for r in results)} damage events and all random draws")


if __name__ == "__main__":
    main()
