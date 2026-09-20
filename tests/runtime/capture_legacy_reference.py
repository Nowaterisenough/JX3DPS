"""Refresh arithmetic fixtures; verify the checkout's commit with git first."""
import hashlib
from pathlib import Path
import re
import shutil
import sys

COMMIT = "ba960f776dad2b30c8d43b529cd06d4fcaed5c2c"
root = Path(sys.argv[1]).resolve()
out = Path(__file__).resolve().parent / "legacy_reference"
manifest_path = out / "SHA256.txt"
if manifest_path.exists():
    for line in manifest_path.read_text(encoding="ascii").splitlines()[1:]:
        digest, path = line.split("  ", 1)
        if hashlib.sha256((root / path).read_bytes()).hexdigest() != digest:
            raise SystemExit("reference source differs from the pinned fixture: " + path)
(out / "Global").mkdir(parents=True, exist_ok=True)
paths = ["Damage/Damage.hpp", "Global/JX3.h", "Global/Types.h", "Global/Version.h"]
for path in paths:
    shutil.copyfile(root / "src" / path, out / ("Damage.hpp" if path.startswith("Damage/") else path))

skill_path = root / "src/Class/TaiXuJianYi/TaiXuJianYiSkill.cpp"
buff_path = root / "src/Class/TaiXuJianYi/TaiXuJianYiBuff.cpp"
skill = skill_path.read_text(encoding="utf-8")
buff = buff_path.read_text(encoding="utf-8")
specs = [(skill, "WuWoWuJian", 0, 10, "wuwu"),
         (skill, "BaHuangGuiYuan", 0, 1, "bahuang"),
         (skill, "BaHuangGuiYuan", 1, 10, "bahuang_extra"),
         (skill, "SanHuanTaoYue", 0, 1, "sanhuan"),
         (skill, "SanChaiJianFa", 0, 1, "sanchai"),
         (skill, "PoZhao", 0, 3, "pozhao"),
         (buff, "DieRen", 0, 7, "dieren"),
         (buff, "ChiYing", 0, 1, "chiying"),
         (skill, "WanJianGuiZong", 0, 1, "wanjian"),
         (skill, "RenJianHeYi", 0, 1, "renjian"),
         (skill, "RenJianHeYiSuiXingChen", 0, 1, "renjian_suixing"),
         (buff, "RenJianHeYi", 0, 1, "renjian_dot"),
         (buff, "JianRu", 0, 1, "jianru"),
         (buff, "JianRu", 1, 1, "jianru_active"),
         (skill, "JingHuaYing", 0, 1, "jinghua"),
         (skill, "JingHuaYing", 1, 1, "jinghua_sui"),
         (buff, "YunZhongJianSuiXingChen", 0, 1, "yunzhong_suixing"),
         (buff, "YunZhongJianShengTaiJi", 0, 1, "yunzhong_shengtaiji"),
         (buff, "YunZhongJianTunRiYue", 0, 1, "yunzhong_tunriyue")]
lines = ["// Generated from " + COMMIT + "; do not hand edit.",
         "struct LegacyParam { int fixed, weapon, channel;",
         "    LegacyParam(int f, int w, int c) : fixed(f), weapon(w), channel(c) {} };", ""]
for source, name, sub, count, alias in specs:
    constructor = source.split(name + "::" + name + "(", 1)[1].split("\nvoid ", 1)[0]
    expressions = re.findall(r"m_damageParams\[" + str(sub) + r"\]\.emplace_back\((.*?)\);", constructor, re.DOTALL)
    if len(expressions) < count:
        raise SystemExit("missing reference parameters for " + name)
    lines.append("const std::array<LegacyParam, " + str(count) + "> legacy_" + alias + "{{")
    lines.extend("    LegacyParam(" + x.replace("JX3_PERCENT_INT_BASE", "1024") + ")," for x in expressions[:count])
    lines.append("}};\n")
(out / "skill_parameters.inc").write_text("\n".join(lines), encoding="ascii")
manifest = ["commit " + COMMIT]
for path in paths + ["Class/TaiXuJianYi/TaiXuJianYiSkill.cpp", "Class/TaiXuJianYi/TaiXuJianYiBuff.cpp"]:
    manifest.append(hashlib.sha256((root / "src" / path).read_bytes()).hexdigest() + "  src/" + path)
(out / "SHA256.txt").write_text("\n".join(manifest) + "\n", encoding="ascii")
