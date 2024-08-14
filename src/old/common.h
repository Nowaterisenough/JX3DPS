#ifndef JX3DPS_COMMON_H
#define JX3DPS_COMMON_H

#include "type.h"

namespace JX3DPS {

#define PROPERTY inline thread_local static
#define CONSTANT constexpr static

enum EventType
{
    ROLL,
    POST_ROLL,
    // DAMAGE,
    POST_DAMAGE, // 伤害后

    ADD,     // 添加buff或dot
    TICKING, // buff或dot的持续效果
    TRIGGER, // 引爆dot或激活某些特殊buff的效果
    EXPIRE,  // buff或dot到期
    REMOVE,  // 手动移除buff或dot

    PERMANENT, // buff或dot的永久效果（如果有的话）

    PRE_CAST,  // 预处理
    POST_CAST, // 结束后

    // CASTING, // 施法中

    COUNT, // 用于计数BuffEvent的数量
};

enum Roll
{
    HIT,
    DOUBLE,
    MISS,
    INSIGHT,
};

enum Tag
{

    SINGLE = 1 << 4,
    AOE    = 1 << 5,

    INSTANT    = 1 << 6, // 瞬发
    CHANNELING = 1 << 7, // 读条
    CASTING    = 1 << 8, // 施法

    SELF   = 1 << 9,  // 自身
    TARGET = 1 << 10, // 目标
    THIRD  = 1 << 11, // 第三者

    DAMAGE = 1 << 11, // 伤害
    HEAL   = 1 << 12, // 治疗

    RANGE = 1 << 12, // 远程
    MELEE = 1 << 13, // 近战

    DOT = TARGET | DAMAGE,
    HOT = SELF | HEAL,

    YUN_ZHONG_JIAN = SELF | AOE | DAMAGE,

    CUSTOM = 1 << 14,
};

thread_local array_t<std::tuple<id_t, bool>, 100> talents;
thread_local array_t<std::tuple<id_t, bool>, 100> recipes;

constexpr pctn_t PCT_5   = 51;
constexpr pctn_t PCT_10  = 102;
constexpr pctn_t PCT_15  = 153;
constexpr pctn_t PCT_20  = 205;
constexpr pctn_t PCT_25  = 256;
constexpr pctn_t PCT_30  = 307;
constexpr pctn_t PCT_35  = 358;
constexpr pctn_t PCT_40  = 410;
constexpr pctn_t PCT_45  = 461;
constexpr pctn_t PCT_50  = 512;
constexpr pctn_t PCT_55  = 563;
constexpr pctn_t PCT_60  = 615;
constexpr pctn_t PCT_65  = 666;
constexpr pctn_t PCT_70  = 717;
constexpr pctn_t PCT_75  = 769;
constexpr pctn_t PCT_80  = 820;
constexpr pctn_t PCT_85  = 871;
constexpr pctn_t PCT_90  = 923;
constexpr pctn_t PCT_95  = 974;
constexpr pctn_t PCT_100 = 1024;

} // namespace JX3DPS

#endif // JX3DPS_COMMON_H