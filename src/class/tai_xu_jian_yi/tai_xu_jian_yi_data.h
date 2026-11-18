#ifndef JX3DPS_CLASS_TAI_XU_JIAN_YI_DATA_H
#define JX3DPS_CLASS_TAI_XU_JIAN_YI_DATA_H

#include <array>

#include "src/global/types.h"
#include "tai_xu_jian_yi_202604_data.h"
#include "tai_xu_jian_yi_live_data.h"

namespace JX3DPS::太虚剑意::Data {

// This header contains the small, compile-time hot-path table. The complete
// 魔盒 response (including descriptions, recipes and equipment records) is
// kept in data/jx3box/tai_xu_jian_yi_latest.json and refreshed by the sync
// script; no network request is made inside a simulation.
inline constexpr const char *SOURCE_SKILL =
    "https://data.jx3box.com/bps/std/4/skill.json";
inline constexpr const char *SOURCE_GUIDE =
    "https://www.jx3box.com/bps/109689";
inline constexpr const char *SOURCE_TALENT =
    "https://data.jx3box.com/bps/std/4/talent.json";
inline constexpr const char *SOURCE_RECIPE =
    "https://data.jx3box.com/bps/std/4/skill.json#cheasts";
inline constexpr const char *SOURCE_EQUIPMENT =
    "https://node.jx3box.com/api/node/item/search?keyword=太虚&client=std";
inline constexpr const char *SOURCE_ICON =
    "https://icon.jx3box.com/icon/{id}.png";
inline constexpr const char *SOURCE_CHANGELOG =
    "https://cms.jx3box.com/api/cms/pve/skill/changelog/98";
inline constexpr const char *DATA_VERSION = "2026-09-19";
inline constexpr int LEVEL = 50;
inline constexpr int LIVE_TALENT_COUNT = 46;
inline constexpr int LIVE_SKILL_COUNT = 15;

// The April archive is exposed beside the current table so callers can
// select a historical ruleset explicitly without changing the live baseline.
inline constexpr const char *APRIL_2026_DATA_VERSION = April2026::DATA_VERSION;
inline constexpr int APRIL_2026_TALENT_VERSION = April2026::TALENT_VERSION;

struct AttributeScale {
    // The handbook reports the amount required for one percent at level 50.
    double critical_strike = 95.1291;
    double critical_strike_power = 35.046;
    double overcome = 103.7817;
    double strain = 70.4583;
    double haste = 101.079;
};

inline constexpr AttributeScale ATTRIBUTE_SCALE{};

struct SkillCoefficient {
    jx3id_t id;
    const char *name;
    double attack;
    int weapon_damage;
};

// Coefficients are the compressed level-50 values from the coefficient table.
inline constexpr std::array<SkillCoefficient, 12> SKILL_COEFFICIENTS = {{
    { 364, "三环套月", 0.307199, 1 },
    { 365, "无我无剑", 0.053517, 2 }, // Per consumed qi point.
    { 2699, "八荒归元", 0.718652, 2 }, // Highest damage channel.
    { 588, "人剑合一", 0.027801, 0 },
    { 37453, "人剑合一(DOT)", 0.078190, 0 },
    { 21979, "云中剑", 0.114678, 0 },
    { 25771, "八荒归元·神兵", 0.045176, 0 },
    { 600, "叠刃", 0.021893, 0 }, // Per stack and per tick.
    { 34693, "剑入", 0.531691, 0 },
    { 34694, "剑入·爆炸", 0.590768, 0 },
    { 18121, "三柴剑法", 0.0, 1 },
    { 40752, "无我无剑·玄门", 0.180705, 0 },
}};

struct TalentInfo {
    int tier;
    jx3id_t id;
    const char *name;
    const char *description;
};

// 魔盒 talent.json 当前太虚剑意 12 层奇穴（同步生成，图标和描述见 LiveData）。
inline constexpr std::array<TalentInfo, 46> TALENTS = {{
    { 1, 5801, "朝玄", "“万剑归宗”对定身锁足的目标造成的额外伤害提高10%。" },
    { 1, 5807, "心固", "“三环套月”会心提高10%，会心效果提高10%。" },
    { 1, 18701, "不善", "“三环套月”附带60%减速效果，持续5秒。" },
    { 2, 32407, "环月", "“三环套月”调息时间增加4秒，每剑都可以造成伤害和破招伤害，续气一格并使目标身上属于自己的“叠刃”效果立即生效一跳。" },
    { 2, 5811, "深埋", "外功招式会心后，自身立即回复1格气。" },
    { 2, 17747, "吐故纳新", "处于自身“生太极”气场当中，自身受到的伤害降低10%，受到攻击有15%几率令攻击者锁足3秒，并恢复自身10％的内力，该效果每20秒只能触发一次。" },
    { 3, 14041, "玄德", "施展“凭虚御风”后使自身受到的内功伤害降低40%，持续5秒。" },
    { 3, 5800, "白虹", "“无我无剑”命中目标后，会对目标周围6尺内最多5个目标造成外功伤害，消耗的气越多，伤害越高。该效果不受到“无我无剑”秘籍的影响。" },
    { 3, 5830, "不移", "“凭虚御风”闪避效果额外提高15%，持续时间增加3秒。" },
    { 4, 21812, "云中剑", "引爆自身“生太极”气场后会使“气剑”分别环绕自身6尺运转8秒，“气剑”运转期间会对4个敌对目标造成40-57（+0.48*攻击力）外功伤害。" },
    { 4, 5818, "无意", "消耗3格气以上施展无我无剑，会心几率提高10%，会心效果提高30%。" },
    { 4, 26700, "镜花影", "“万剑归宗”命中拥有自身“吞日月”效果的目标，可在8秒内施展二段招式，对目标造成100%武器伤害附加外功伤害。二段招式命中目标后会标记目标，3秒后对目标额外造成外功伤害。" },
    { 5, 5809, "独笑", "“转乾坤”调息时间降低5秒，伤害降低效果提高10%，不受招式控制的效果提高2秒，每秒恢复自身8%气血最大值，并在期间免疫推拉效果。" },
    { 5, 17742, "风逝", "“三环套月”“八荒归元”施展距离增加2尺，成功命中目标使下一个“无我无剑”伤害提高30%，施展距离提高2尺，并为目标添加一层叠刃效果，持续5秒。" },
    { 5, 36106, "瑞氛", "施展“紫气东来”时会将四道剑散落在20尺内自身最近的“生太极”气场上，并且剑会在3秒后回到自身并对路径目标造成伤害。" },
    { 5, 24969, "合虚", "气场不再附带“玄一”效果，但“玄一”效果被卸除时只消失1层，“无我无剑”命中自身“玄一”效果的目标，伤害提高10%。" },
    { 6, 5802, "解牛", "对被定身、锁足、眩晕的目标伤害提高25%。每次命中被定身、锁足、眩晕的目标会使自身会心几率提高2%，持续3秒，最多提高20%。" },
    { 6, 30831, "湛存", "被自身“吞日月”效果作用的敌方侠士目标每秒获得1层“湛存”效果，最多5层，持续4秒。自身施展“无我无剑”命中带有“湛存”效果的目标，最少1层对目标造成200%武器伤害附加2400-2653（+1.03*攻击力）外功伤害，最多5层对目标造成200%武器伤害附加2696-2982（+2.52*攻击力）外功伤害。" },
    { 6, 5821, "叠刃", "“无我无剑”命中目标后附带1层叠刃效果，若会心则额外附带1层叠刃效果，最多叠加5层。" },
    { 6, 14986, "开兑", "“吞日月”可额外解除友方目标阳性，阴性，混元性和毒性不利效果各一个。" },
    { 7, 6758, "切玉", "“八荒归元”命中40%气血以下目标，能够使“叠刃”的持续伤害立即生效。自身“紫气东来”期间生效气血值提高到60%以下。" },
    { 7, 6481, "雾外江山", "除“镇山河”外所有气场招式的调息时间降低1秒，施展气场后额外回复1格气，成功运功施展气场，则使下一个气场无需运功" },
    { 7, 21725, "长生", "每次成功施展气场或剑阵，则使气场或剑阵中心4尺内3个侠士目标锁足1秒，且自身下一个伤害招式附带“持盈”效果，对目标造成40-57（+0.79*攻击力）外功伤害，效果持续15秒，可叠加3层。" },
    { 7, 32400, "列星", "施展“剑冲阴阳”后可施展二段招式“剑斩列星”，对10尺半径内最多5个目标造成伤害，若目标在自己的三个气场之中则会被禁锢3秒，并返还“剑冲阴阳”2秒调息时间。" },
    { 8, 14829, "负阴", "“碎星辰”气场持续时间增加12秒，对自身的增益效果提高100%。" },
    { 8, 5813, "无我", "“坐忘无我”每点根骨或身法化解的伤害提高10%，效果期间自身受到的伤害降低50%，每秒恢复自身0.4%最大气血值，受到“锁足”“定身”“眩晕”“击倒”控制效果持续时间缩短30%。" },
    { 8, 14598, "若水", "“紫气东来”充能时间降低15秒，充能层数提高1层。" },
    { 8, 32403, "斗转", "“剑冲阴阳”使命中目标后会为目标添加2层“叠刃”效果并将把距离目标30尺内最近的自身创建的“吞日月”气场吸引至目标脚下，并强化该“吞日月”5秒，使自身伤害命中受该次“吞日月”效果的侠士会造成0.25秒眩晕效果。" },
    { 8, 24962, "裂云", "自身造成的\"叠刃\"效果最高层数提高至7层，处于自身4层“叠刃\"及以上的目标15尺内将提高自身25%的移动速度和15%的外功会心效果。" },
    { 9, 5810, "同根", "万剑归宗”目标数量提高2个，“无我无剑”“万剑归宗”的伤害提高10%。" },
    { 9, 6466, "实腹", "“大道无术”“人剑合一”“剑冲阴阳”控制时间延长1秒，并使自身续气一格。" },
    { 9, 18799, "故长", "“碎星辰”只对自己生效，处于“碎星辰”期间自身无视目标60%外功防御。非战斗状态下施展“碎星辰”返还5秒调息时间。" },
    { 9, 6744, "随物", "“剑冲阴阳”可直接对敌方目标施展，剑灵持续攻击目标对其造成1233-1450（+0.575*攻击力）外功伤害，若短时间内连续命中同一个目标，则降低为369-434（+0.15*攻击力）外功伤害，持续6秒。" },
    { 9, 24961, "无为", "“大道无术”成功控制目标获得5格气，使自身立刻获得4秒“紫气东来”效果，且4秒内可额外施展一次“八荒归元”。" },
    { 10, 6472, "期声", "拥有增益气场气劲时，自身混元内功和外功基础攻击提高10%。" },
    { 10, 17743, "物我两忘", "“人剑合一”引爆及定身效果范围提高3尺，每成功定身一个目标“人剑合一”调息时间降低1秒。" },
    { 10, 20782, "剑势", "“剑冲阴阳”成功眩晕目标后，在目标区域生成“生太极”气场。" },
    { 10, 34656, "剑入", "“紫气东来”期间自身“八荒归元”命中目标，则会在目标位置落下一把持续6秒的灵剑，灵剑受到自身范围攻击后将立刻激活并每秒对6尺范围内至多3个敌方目标造成外功伤害，命中非侠士目标伤害提高80%。若施展“八荒归元”时已经有灵剑则会引爆灵剑造成6尺外功伤害并眩晕3秒。" },
    { 10, 24958, "至柔", "“人剑合一”成功引爆自身气场后，运功不会被打退、打断，不受无法施展内功招式和缴械效果的影响，持续3秒。" },
    { 11, 17731, "无欲", "“无我无剑”命中有自身“叠刃”效果的目标则减少“八荒归元”调息时间，每消耗1格气则减少1秒。" },
    { 11, 14835, "凶年", "“生太极”作用范围提高5尺，减速效果提高到45%。" },
    { 11, 22704, "不争", "“坐忘无我”调息时间降低3秒，可以对队友使用，对队友使用时，使其化解自身身法*5点伤害且受到的伤害降低50%，持续5秒。" },
    { 11, 14832, "虚极", "自身“叠刃”伤害提高20%。“万剑归宗”会心提高10%，会心效果提高10%，调息时间降低2秒，命中目标将使其获得一层“叠刃”效果。" },
    { 11, 5812, "狂歌", "“人剑合一”每命中一个气场，自身回复2格气，5%内力和1%气血，为小队成员回复2%内力和1%气血。" },
    { 12, 6757, "匣中", "“剑冲阴阳”伤害提高100%，“剑冲阴阳”施展后立即重置“八荒归元”调息时间。" },
    { 12, 14833, "玄门", "“生太极”调息时间增加15秒，“人剑合一”每引爆一个自身的气场，破防等级提高20%，外功会心几率提高3%，持续40秒，可叠加3层。" },
}};

inline constexpr std::array<const char *, 12> RECOMMENDED_TALENTS = {{
    "心固", "环月", "白虹", "无意", "风逝", "叠刃",
    "长生", "裂云", "故长", "期声", "虚极", "玄门"
}};

// The four entries in each row are the currently recommended secret manuals.
inline constexpr std::array<std::array<const char *, 4>, 4> RECOMMENDED_RECIPES = {{
    {{ "人偶图残页", "参悟绝章", "参悟断篇", "秘诀绝章" }},
    {{ "人偶图残页", "参悟绝章", "参悟断篇", "参悟残页" }},
    {{ "穴位图残页", "秘诀绝章", "参悟绝章", "参悟断篇" }},
    {{ "穴位图残页", "人偶图断篇", "人偶图残页", "参悟断篇" }}
}};

struct EquipmentRecommendation {
    const char *name;
    const char *effect;
    int icon;
};

// Current consumables and the 50-level refinement note from the handbook.
inline constexpr std::array<EquipmentRecommendation, 8> EQUIPMENT_RECOMMENDATIONS = {{
    { "断浪·太后饼", "基础攻击 +696", 7531 },
    { "断浪·白肉血肠", "破招 +1545", 549 },
    { "断浪·红烧排骨", "破防 +1545", 9999 },
    { "断浪·酸菜鱼", "会心 +1545", 8909 },
    { "断浪·杂锦鱼球粥", "身法 +347", 9989 },
    { "断浪·上品亢龙散", "基础攻击 +895", 4302 },
    { "女儿红·旬又三", "加速 +1144", 10134 },
    { "断浪·瀑沙熔锭", "基础攻击 +597", 7595 },
}};

inline constexpr double REFINEMENT_6_TO_8_GAIN_PERCENT = 2.44;

constexpr const SkillCoefficient *FindSkillCoefficient(const char *name)
{
    for (const auto &skill : SKILL_COEFFICIENTS) {
        const char *lhs = skill.name;
        const char *rhs = name;
        while (*lhs && *rhs && *lhs == *rhs) {
            ++lhs;
            ++rhs;
        }
        if (*lhs == *rhs) {
            return &skill;
        }
    }
    return nullptr;
}

} // namespace JX3DPS::太虚剑意::Data

#endif // JX3DPS_CLASS_TAI_XU_JIAN_YI_DATA_H
