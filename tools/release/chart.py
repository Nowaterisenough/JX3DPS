"""Validate all six release reports and produce a PNG and release body."""
import argparse
import json
import math
import statistics
from pathlib import Path
from urllib.parse import quote
from measure import FIGHTS, REPEATS, SCENES, SECONDS, SEED

BUILDS = {"windows-x86_64-gcc":"Windows GCC","windows-x86_64-msvc":"Windows MSVC",
          "windows-x86_64-clang":"Windows Clang","linux-x86_64-gcc":"Linux GCC",
          "macos-arm64-appleclang":"macOS AppleClang","web-wasm32-emscripten":"WebAssembly"}
LABELS = ["Tai Xu / Default","Tai Xu / Team","Mo Wen / Default","Mo Wen / Equipment"]


def render(reports, out, sha, repo, tag, preview=False):
    expected = set(BUILDS)
    by_id = {r["build"]:r for r in reports}
    if len(by_id)!=len(reports) or (not preview and set(by_id)!=expected) or not by_id or set(by_id)-expected:
        raise ValueError("missing, duplicate or unexpected release build")
    signatures={}
    for r in reports:
        if (r["sha"],r["workers"],r["iterations"],r["duration"],r["seed"],r["warmup"],r["repeats"]) != (sha,1,FIGHTS,SECONDS,SEED,256,REPEATS):
            raise ValueError(f"workload mismatch: {r['build']}")
        if set(r["scenes"])!=set(SCENES):raise ValueError("incomplete scenario set")
        for scene, row in r["scenes"].items():
            if len(row["seconds"])!=REPEATS or not all(math.isfinite(t) and t>0 for t in row["seconds"]):
                raise ValueError("invalid timing")
            sig=(int(row["checksum"]),int(row["damage"]))
            if signatures.setdefault(scene,sig)!=sig:raise ValueError(f"damage mismatch: {r['build']} / {scene}")
    import matplotlib
    matplotlib.use("Agg")
    import matplotlib.pyplot as plt
    out.mkdir(parents=True,exist_ok=True)
    ids=[k for k in BUILDS if k in by_id]
    fig,axes=plt.subplots(2,2,figsize=(12,7.8),layout="constrained")
    fig.suptitle("JX3DPS  |  "+("Local benchmark" if preview else "Release benchmark")+"\n100,000 fights   ·   300 s per fight   ·   1 worker   ·   median of 3 runs",fontsize=15)
    maximum=max(statistics.median(result["seconds"]) for r in reports for result in r["scenes"].values())
    for ax,scene,title in zip(axes.flat,SCENES,LABELS):
        times=[statistics.median(by_id[k]["scenes"][scene]["seconds"]) for k in ids]
        bars=ax.barh([BUILDS[k] for k in ids],times,color=["#cc7849" if k.startswith("web-") else "#27735a" for k in ids])
        ax.bar_label(bars,fmt="%.3f",padding=3,fontsize=9)
        ax.set_xlim(0,maximum*1.22);ax.invert_yaxis();ax.set_title(title,loc="left");ax.set_xlabel("Wall time (s)")
        ax.grid(axis="x",alpha=.18);ax.set_axisbelow(True);ax.spines[["top","right"]].set_visible(False)
    fig.savefig(out/"performance.png",dpi=150);plt.close(fig)
    (out/"performance.json").write_text(json.dumps([by_id[k] for k in ids],ensure_ascii=False,indent=2)+"\n",encoding="utf8")
    url=f"https://github.com/{repo}/releases/download/{quote(tag,safe='')}/"
    lines=["## 各构建版本性能对照","",f"![四种配置的批跑耗时]({url}performance.png)","",
           "每组 10 万场、每场 300 秒、单工作线程，预热 256 场后取三轮耗时中位数。WASM 使用真实 Chromium Worker。耗时包含批跑与结果汇总。不同平台使用各自的运行器，硬件差异也会影响结果。","",
           "| 构建 | 编译器 / 浏览器 | 运行器及 CPU | 太虚默认 | 太虚团队 | 莫问默认 | 莫问多装备 |",
           "|---|---|---|---:|---:|---:|---:|"]
    for k in ids:
        r=by_id[k];compiler=r['compiler']+(f" / Chromium {r['browser']}" if r['browser'] else "")
        lines.append(f"| {BUILDS[k]} | {compiler} | {r['runner']} · {r['cpu']} | "+" | ".join(f"{statistics.median(r['scenes'][s]['seconds']):.3f} s" for s in SCENES)+" |")
    lines+=["",f"四组总伤害与校验值在所有参测构建间一致。[逐轮耗时与校验值]({url}performance.json)"]
    (out/"performance.md").write_text("\n".join(lines)+"\n",encoding="utf8")


if __name__=="__main__":
    p=argparse.ArgumentParser(description=__doc__)
    for k in ("input","output","sha","repo","tag"):p.add_argument("--"+k,required=True)
    a=p.parse_args()
    render([json.loads(f.read_text(encoding="utf8")) for f in Path(a.input).glob("bench-*.json")],Path(a.output),a.sha,a.repo,a.tag)
