"""Validate all six release reports and produce a PNG and release body."""
import argparse
import json
import math
import statistics
from pathlib import Path
from urllib.parse import quote
from measure import FIGHTS, REPEATS, SECONDS, SEED

BUILDS = {"windows-x86_64-gcc":"Windows GCC","windows-x86_64-msvc":"Windows MSVC",
          "windows-x86_64-clang":"Windows Clang","linux-x86_64-gcc":"Linux GCC",
          "macos-arm64-appleclang":"macOS AppleClang","web-wasm32-emscripten":"WebAssembly"}
def validate_samples(values):
    if len(values) != REPEATS or not all(math.isfinite(t) and t > 0 for t in values):
        raise ValueError("invalid timing")


def render(reports, out, sha, repo, tag, preview=False):
    expected = set(BUILDS)
    by_id = {r["build"]:r for r in reports}
    if len(by_id)!=len(reports) or (not preview and set(by_id)!=expected) or not by_id or set(by_id)-expected:
        raise ValueError("missing, duplicate or unexpected release build")
    names = {p.stem for p in (Path(__file__).resolve().parents[2] / "src/class").glob("*.cpp")}
    catalog = {code: row["name"] for code, row in reports[0]["scenes"].items()}
    if set(catalog.values()) != names or len(catalog) != len(names):
        raise ValueError("benchmark catalog differs from the shipped schools")
    iterations = reports[0]["iterations"] if preview else FIGHTS
    signatures = {}
    for r in reports:
        if (r["sha"],r["workers"],r["iterations"],r["duration"],r["seed"],r["warmup"],r["repeats"]) != (sha,1,iterations,SECONDS,SEED,256,REPEATS):
            raise ValueError(f"workload mismatch: {r['build']}")
        if {k: v["name"] for k, v in r["scenes"].items()} != catalog:
            raise ValueError(f"incomplete school set: {r['build']}")
        for scene, row in r["scenes"].items():
            validate_samples(row["seconds"])
            if r["build"].endswith("-gcc"):
                if r.get("optimization") != "GCC PGO":
                    raise ValueError(f"expected trained GCC build: {r['build']}")
                validate_samples(row["baseline_seconds"])
            sig=(int(row["checksum"]),int(row["damage"]))
            if signatures.setdefault(scene,sig)!=sig:raise ValueError(f"damage mismatch: {r['build']} / {scene}")
    import matplotlib
    matplotlib.use("Agg")
    import matplotlib.pyplot as plt
    from matplotlib import font_manager
    from matplotlib.colors import LinearSegmentedColormap
    out.mkdir(parents=True,exist_ok=True)
    ids=[k for k in BUILDS if k in by_id]
    schools = sorted(catalog)
    times = [[statistics.median(by_id[k]["scenes"][s]["seconds"]) for k in ids] for s in schools]
    ratios = [[t/min(row) for t in row] for row in times]
    fig, ax = plt.subplots(figsize=(max(7, len(ids)*1.65), 2+len(schools)*.41), layout="constrained")
    cmap = LinearSegmentedColormap.from_list("time", ["#e2f3ec", "#fff7e7", "#e7ad82"])
    ax.imshow(ratios, cmap=cmap, vmin=1, vmax=max(1.01,max(map(max,ratios))), aspect="auto")
    for i, row in enumerate(times):
        for j, t in enumerate(row):
            ax.text(j, i, f"{t:.3f}", ha="center", va="center", fontsize=9, weight="bold" if t==min(row) else "normal")
    fonts = {f.name for f in font_manager.fontManager.ttflist}
    cjk = next((f for f in ("Noto Sans CJK JP", "Noto Sans CJK SC", "Microsoft YaHei", "SimHei") if f in fonts), None)
    ax.set_yticks(range(len(schools)), [catalog[s] if cjk else s.upper() for s in schools], fontfamily=cjk or "sans-serif")
    ax.set_xticks(range(len(ids)), [BUILDS[k].replace(" ", "\n") for k in ids]); ax.xaxis.tick_top()
    ax.tick_params(length=0, pad=7)
    ax.set_title(f"JX3DPS · wall time in seconds\n{iterations:,} fights × 300 s · 1 worker · median of 3 runs", pad=15)
    ax.set_xlabel("Bold = fastest in row. Color = time relative to the fastest build for that school.", labelpad=10)
    fig.savefig(out/"performance.png", dpi=150); plt.close(fig)
    (out/"performance.json").write_text(json.dumps([by_id[k] for k in ids],ensure_ascii=False,indent=2)+"\n",encoding="utf8")
    url=f"https://github.com/{repo}/releases/download/{quote(tag,safe='')}/"
    lines = ["## 各构建版本性能对照", "", f"![全部心法的模拟耗时]({url}performance.png)", "",
             f"各心法使用自己的默认宏和配置，每组 {iterations:,} 场、每场 300 秒、单工作线程，种子 42，取三轮耗时中位数（秒）。每个构建先预热 256 场。桌面计时含批跑和统计汇总，Web 使用真实 Chromium Worker。", "",
             "| 心法 | " + " | ".join(BUILDS[k] for k in ids) + " |", "|---|" + "---:|"*len(ids)]
    for s, row in zip(schools, times, strict=True):
        lines.append("| " + catalog[s] + " | " + " | ".join(f"{t:.3f}" for t in row) + " |")
    gcc = [k for k in ids if k.endswith("-gcc")]
    if gcc:
        lines += ["", "### GCC 优化前后", "", "同一台 runner 交替测试普通 Release 和 PGO 发行程序。降幅按 `1 − PGO 耗时 / 普通耗时` 计算。", "",
                  "| 心法 | " + " | ".join(BUILDS[k] + "（普通 → PGO，耗时降幅）" for k in gcc) + " |", "|---|" + "---:|"*len(gcc)]
        for s in schools:
            cells = []
            for k in gcc:
                row = by_id[k]["scenes"][s]
                before, after = statistics.median(row["baseline_seconds"]), statistics.median(row["seconds"])
                cells.append(f"{before:.3f} → {after:.3f}，{1-after/before:.1%}")
            lines.append(f"| {catalog[s]} | " + " | ".join(cells) + " |")
    lines += ["", "| 构建 | 编译器 / 浏览器 | 运行器及 CPU |", "|---|---|---|"]
    for k in ids:
        r = by_id[k]
        compiler = r['compiler'] + (f" / Chromium {r['browser']}" if r['browser'] else "")
        lines.append(f"| {BUILDS[k]} | {compiler} | {r['runner']} · {r['cpu']} |")
    lines += ["", f"{len(schools)} 个心法已分别校验总伤害与批次校验值。跨 runner 的绝对耗时同时受 CPU、系统与工具链影响。[逐轮耗时与校验值]({url}performance.json)"]
    (out/"performance.md").write_text("\n".join(lines)+"\n",encoding="utf8")


if __name__=="__main__":
    p=argparse.ArgumentParser(description=__doc__)
    for k in ("input","output","sha","repo","tag"):p.add_argument("--"+k,required=True)
    a=p.parse_args()
    render([json.loads(f.read_text(encoding="utf8")) for f in Path(a.input).glob("bench-*.json")],Path(a.output),a.sha,a.repo,a.tag)
