#ifndef JX3DPS_CLASS_TAI_XU_JIAN_YI_IDS_H
#define JX3DPS_CLASS_TAI_XU_JIAN_YI_IDS_H

#include "src/global/types.h"
#include <string_view>

namespace JX3DPS {

namespace 太虚剑意 {

// ========== ID 定义 ==========

enum class SkillId : jx3id_t
{
    破招     = 32897,
    无我无剑 = 365,   // JX3BOX 技能数据库
    八荒归元 = 2699,  // JX3BOX 技能数据库
    三环套月 = 364,   // JX3BOX 技能数据库
    万剑归宗 = 311,   // JX3BOX 技能数据库
    人剑合一 = 588,   // JX3BOX 技能数据库
    三柴剑法 = 32704,
    生太极   = 358,
    碎星辰   = 362,
    吞日月   = 363,
    紫气东来 = 2681,
    镜花影   = 32898,
};

enum class BuffId : jx3id_t
{
    叠刃     = 9745,
    万象归元 = 18931,
    人剑合一 = 9732,
    紫气东来 = 19267,
    玄门     = 23367,
    风势     = 17932,
    裂云     = 25711, // Changed from duplicate 18931
    气盛     = 23393,
    剑入     = 25766,
    镜花影   = 28598,
    持盈     = 28626,
};

enum class TalentId
{
    朝玄 = 5801,
    心固 = 5807,
    不善 = 18701,
    环月 = 32407,
    深埋 = 5811,
    吐故纳新 = 17747,
    白虹 = 5800,
    不移 = 5830,
    玄德 = 14041,
    云中剑 = 21812,
    无意 = 5818,
    镜花影 = 26700,
    独笑 = 5809,
    风逝 = 17742,
    合虚 = 24969,
    瑞氛 = 36106,
    解牛 = 5802,
    湛存 = 30831,
    叠刃 = 5821,
    开兑 = 14986,
    切玉 = 6758,
    雾外江山 = 6481,
    长生 = 21725,
    列星 = 32400,
    负阴 = 14829,
    无我 = 5813,
    若水 = 14598,
    斗转 = 32403,
    裂云 = 24962,
    同根 = 5810,
    实腹 = 6466,
    故长 = 18799,
    随物 = 6744,
    无为 = 24961,
    期声 = 6472,
    物我两忘 = 17743,
    剑势 = 20782,
    剑入 = 34656,
    至柔 = 24958,
    不争 = 22704,
    狂歌 = 5812,
    虚极 = 14832,
    无欲 = 17731,
    凶年 = 14835,
    匣中 = 6757,
    玄门 = 14833,
};

enum class RecipeId
{
    无我无剑_会心_2 = 1704,
    无我无剑_会心_3 = 1705,
    无我无剑_会心_4 = 1706,
    无我无剑_伤害_3 = 2256,
    无我无剑_伤害_4 = 2257,
    无我无剑_伤害_5 = 2258,
    无我无剑_气点   = 4731,

    碎星辰_伤害_4 = 4724,
    碎星辰_伤害_5 = 4725,
    碎星辰_气点   = 4726,

    云中剑_碎星辰 = 17395,
    云中剑_生太极 = 17396,
    云中剑_吞日月 = 17397,
};

inline jx3id_t SkillIdFromName(std::string_view name)
{
    if (name == "破招") return static_cast<jx3id_t>(SkillId::破招);
    if (name == "无我无剑") return static_cast<jx3id_t>(SkillId::无我无剑);
    if (name == "八荒归元") return static_cast<jx3id_t>(SkillId::八荒归元);
    if (name == "三环套月") return static_cast<jx3id_t>(SkillId::三环套月);
    if (name == "万剑归宗") return static_cast<jx3id_t>(SkillId::万剑归宗);
    if (name == "人剑合一") return static_cast<jx3id_t>(SkillId::人剑合一);
    if (name == "三柴剑法") return static_cast<jx3id_t>(SkillId::三柴剑法);
    if (name == "生太极") return static_cast<jx3id_t>(SkillId::生太极);
    if (name == "碎星辰") return static_cast<jx3id_t>(SkillId::碎星辰);
    if (name == "吞日月") return static_cast<jx3id_t>(SkillId::吞日月);
    if (name == "紫气东来") return static_cast<jx3id_t>(SkillId::紫气东来);
    if (name == "镜花影" || name == "镜花 影") return static_cast<jx3id_t>(SkillId::镜花影);
    return 0;
}

inline jx3id_t BuffIdFromName(std::string_view name)
{
    if (name == "叠刃") return static_cast<jx3id_t>(BuffId::叠刃);
    if (name == "万象归元") return static_cast<jx3id_t>(BuffId::万象归元);
    if (name == "人剑合一") return static_cast<jx3id_t>(BuffId::人剑合一);
    if (name == "紫气东来") return static_cast<jx3id_t>(BuffId::紫气东来);
    if (name == "玄门") return static_cast<jx3id_t>(BuffId::玄门);
    if (name == "风势" || name == "风逝") return static_cast<jx3id_t>(BuffId::风势);
    if (name == "裂云") return static_cast<jx3id_t>(BuffId::裂云);
    if (name == "气盛") return static_cast<jx3id_t>(BuffId::气盛);
    if (name == "剑入") return static_cast<jx3id_t>(BuffId::剑入);
    if (name == "镜花影") return static_cast<jx3id_t>(BuffId::镜花影);
    if (name == "持盈") return static_cast<jx3id_t>(BuffId::持盈);
    return 0;
}

inline jx3id_t TalentIdFromName(std::string_view name)
{
    if (name == "朝玄") return static_cast<jx3id_t>(TalentId::朝玄);
    if (name == "心固") return static_cast<jx3id_t>(TalentId::心固);
    if (name == "不善") return static_cast<jx3id_t>(TalentId::不善);
    if (name == "环月") return static_cast<jx3id_t>(TalentId::环月);
    if (name == "深埋") return static_cast<jx3id_t>(TalentId::深埋);
    if (name == "吐故纳新") return static_cast<jx3id_t>(TalentId::吐故纳新);
    if (name == "白虹") return static_cast<jx3id_t>(TalentId::白虹);
    if (name == "不移") return static_cast<jx3id_t>(TalentId::不移);
    if (name == "玄德") return static_cast<jx3id_t>(TalentId::玄德);
    if (name == "云中剑") return static_cast<jx3id_t>(TalentId::云中剑);
    if (name == "无意") return static_cast<jx3id_t>(TalentId::无意);
    if (name == "镜花影") return static_cast<jx3id_t>(TalentId::镜花影);
    if (name == "独笑") return static_cast<jx3id_t>(TalentId::独笑);
    if (name == "风逝") return static_cast<jx3id_t>(TalentId::风逝);
    if (name == "合虚") return static_cast<jx3id_t>(TalentId::合虚);
    if (name == "瑞氛") return static_cast<jx3id_t>(TalentId::瑞氛);
    if (name == "解牛") return static_cast<jx3id_t>(TalentId::解牛);
    if (name == "湛存") return static_cast<jx3id_t>(TalentId::湛存);
    if (name == "叠刃") return static_cast<jx3id_t>(TalentId::叠刃);
    if (name == "开兑") return static_cast<jx3id_t>(TalentId::开兑);
    if (name == "切玉") return static_cast<jx3id_t>(TalentId::切玉);
    if (name == "雾外江山") return static_cast<jx3id_t>(TalentId::雾外江山);
    if (name == "长生") return static_cast<jx3id_t>(TalentId::长生);
    if (name == "列星") return static_cast<jx3id_t>(TalentId::列星);
    if (name == "负阴") return static_cast<jx3id_t>(TalentId::负阴);
    if (name == "无我") return static_cast<jx3id_t>(TalentId::无我);
    if (name == "若水") return static_cast<jx3id_t>(TalentId::若水);
    if (name == "斗转") return static_cast<jx3id_t>(TalentId::斗转);
    if (name == "裂云") return static_cast<jx3id_t>(TalentId::裂云);
    if (name == "同根") return static_cast<jx3id_t>(TalentId::同根);
    if (name == "实腹") return static_cast<jx3id_t>(TalentId::实腹);
    if (name == "故长") return static_cast<jx3id_t>(TalentId::故长);
    if (name == "随物") return static_cast<jx3id_t>(TalentId::随物);
    if (name == "无为") return static_cast<jx3id_t>(TalentId::无为);
    if (name == "期声") return static_cast<jx3id_t>(TalentId::期声);
    if (name == "物我两忘") return static_cast<jx3id_t>(TalentId::物我两忘);
    if (name == "剑势") return static_cast<jx3id_t>(TalentId::剑势);
    if (name == "剑入") return static_cast<jx3id_t>(TalentId::剑入);
    if (name == "至柔") return static_cast<jx3id_t>(TalentId::至柔);
    if (name == "不争") return static_cast<jx3id_t>(TalentId::不争);
    if (name == "狂歌") return static_cast<jx3id_t>(TalentId::狂歌);
    if (name == "虚极") return static_cast<jx3id_t>(TalentId::虚极);
    if (name == "无欲") return static_cast<jx3id_t>(TalentId::无欲);
    if (name == "凶年") return static_cast<jx3id_t>(TalentId::凶年);
    if (name == "匣中") return static_cast<jx3id_t>(TalentId::匣中);
    if (name == "玄门") return static_cast<jx3id_t>(TalentId::玄门);
    return 0;
}

inline jx3id_t RecipeIdFromName(std::string_view name)
{
    // Recipe IDs are optional in profiles. Keep a deterministic compact ID
    // for newly published 魔盒 manuals so they can still be activated before
    // a class adds a dedicated event handler.
    if (name == "") return 0;
    return static_cast<jx3id_t>(std::hash<std::string_view>{}(name) & 0x7fffffff);
}

} // namespace 太虚剑意

} // namespace JX3DPS

#endif // JX3DPS_CLASS_TAI_XU_JIAN_YI_IDS_H
