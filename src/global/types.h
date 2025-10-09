#ifndef JX3DPS_GLOBAL_TYPES_H
#define JX3DPS_GLOBAL_TYPES_H

#include <array>
#include <climits>
#include <cstdint>
#include <functional>
#include <map>
#include <unordered_map>
#include <vector>

namespace JX3DPS {

// ============= 基础类型别名 =============

// 百分比类型
using pctf_t = double; // 浮点百分比 (1.0 = 100%)
using pctn_t = int;    // 整数百分比 (1024 = 100%)
using bpn_t  = int;    // 基点 (10000 = 100%)

// 数值类型
using tick_t  = int;       // 帧数/Tick
using value_t = long long; // 属性值
using cof_t   = double;    // 系数

// ID类型
using jx3id_t = int; // 技能/BUFF/目标 ID

// ============= 容器类型模板 =============

template <typename T, int COUNT>
using array_t = std::array<T, COUNT>;

template <typename T>
using vector_t = std::vector<T>;

template <typename Key, typename Value>
using hash_t = std::unordered_map<Key, Value>;

template <typename Key, typename Value>
using rbtree_t = std::map<Key, Value>;

template <typename T>
using func_t = std::function<T>;

// ============= 标签系统 =============

enum class Tag : uint32_t {
    NONE = 0,

    // 技能/BUFF标签
    TARGET  = 1 << 0,  // 目标技能
    SELF    = 1 << 1,  // 自身技能
    PLAYER  = 1 << 2,  // 玩家BUFF
    AOE     = 1 << 3,  // 范围技能
    DAMAGE  = 1 << 4,  // 伤害技能
    HEAL    = 1 << 5,  // 治疗技能
    MELEE   = 1 << 6,  // 近战技能
    RANGE   = 1 << 7,  // 远程技能
    INSTANT = 1 << 8,  // 即时技能
    DOT     = 1 << 9,  // 持续伤害
    HOT     = 1 << 10, // 持续治疗
    BUFF    = 1 << 11, // BUFF标记
    HASTED  = 1 << 12, // 受加速影响
    THIRD   = 1 << 13, // 第三方BUFF(团队增益)
    NO_GCD  = 1 << 14, // 无GCD
    FIELD   = 1 << 15, // 剑气场
    TEAM    = 1 << 16, // 团队BUFF
    PERMANENT = 1 << 17, // 永久BUFF

    // 物理/魔法
    PHYSICS = 1 << 20, // 外功
    MAGIC   = 1 << 21, // 内功
};

// 标签位运算
inline constexpr Tag operator|(Tag a, Tag b) {
    return static_cast<Tag>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
}

inline constexpr Tag operator&(Tag a, Tag b) {
    return static_cast<Tag>(static_cast<uint32_t>(a) & static_cast<uint32_t>(b));
}

inline constexpr bool operator==(Tag a, Tag b) {
    return static_cast<uint32_t>(a) == static_cast<uint32_t>(b);
}

// ============= 内功属性分类 =============

enum class MagicType : int {
    NEUTRAL = 0, // 混元性内功 (紫霞功、离经易道、花间游等)
    SOLAR   = 1, // 阳性内功 (少林、明教阳性等)
    LUNAR   = 2, // 阴性内功 (七秀、明教阴性等)
    POISON  = 3, // 毒性内功 (五毒等)
};

// ============= 圆桌判定 =============

enum class RollResult : int {
    HIT     = 0,   // 命中
    DOUBLE  = 1,   // 会心
    INSIGHT = 4,   // 识破
    MISS    = 126, // 偏离
};

// ============= 事件类型 =============

enum class EventType : int {
    PERMANENT = 0, // 永久效果

    // 技能事件
    CAST_CONDITION, // 施放条件检查
    PRE_CAST,       // 施放前
    POST_CAST,      // 施放后
    PRE_PREPARE,    // 前摇前
    POST_PREPARE,   // 前摇后
    PRE_CASTING,    // 读条前
    POST_CASTING,   // 读条后
    PRE_ROLL,       // 判定前
    POST_ROLL,      // 判定后
    PRE_DAMAGE,     // 伤害前
    POST_DAMAGE,    // 伤害后

    // BUFF事件
    POST_INIT,         // 初始化后
    ADD,               // BUFF添加
    POST_ADD,          // BUFF添加后
    EXPIRE,            // BUFF过期
    REFRESH,           // BUFF刷新
    CLEAR,             // BUFF清除
    POST_CLEAR,        // BUFF清除后
    TICK,              // BUFF跳动 (别名)
    TICKING = TICK,    // BUFF跳动
    POST_STACK_CHANGE, // BUFF层数变化后
    POST_CONSUME,      // BUFF消耗后

    COUNT
};

// ============= 伤害结构 =============

struct Damage {
    long long damage        = 0; // 普通伤害
    long long surplusDamage = 0; // 破招伤害

    long long SumDamage() const { return damage + surplusDamage; }

    Damage &operator+=(const Damage &other) {
        damage += other.damage;
        surplusDamage += other.surplusDamage;
        return *this;
    }
};

// 收益伤害
struct GainsDamage {
    Damage damage;                     // 总伤害
    Damage attackPowerGain;            // 攻击收益
    Damage criticalStrikePowerGain;    // 会效收益
    Damage overcomeGain;               // 破防收益
    Damage strainBaseGain;             // 无双收益
};

// ============= 宏定义 =============

// 常量定义 (编译期 + 线程本地)
#define CONSTANT inline static thread_local constinit

// 属性定义 (线程本地)
#define PROPERTY inline static thread_local

// 技能静态数据 (非thread_local，共享只读数据，constexpr解决MSVC静态成员初始化问题)
#define SKILL_DATA inline static constexpr

// 百分比常量
constexpr pctn_t PCT_100 = 1024;
constexpr pctn_t PCT_10  = 102;
constexpr pctn_t PCT_20  = 205;
constexpr pctn_t PCT_30  = 307;
constexpr pctn_t PCT_40  = 410;
constexpr pctn_t PCT_50  = 512;

constexpr pctn_t PCT_105 = 1075;
constexpr pctn_t PCT_110 = 1126;
constexpr pctn_t PCT_115 = 1178;
constexpr pctn_t PCT_120 = 1229;
constexpr pctn_t PCT_125 = 1280;
constexpr pctn_t PCT_130 = 1331;
constexpr pctn_t PCT_135 = 1382;
constexpr pctn_t PCT_140 = 1434;
constexpr pctn_t PCT_145 = 1485;

// 无效值
constexpr tick_t JX3DPS_INVALID_FRAMES = INT_MAX;

} // namespace JX3DPS

#endif // JX3DPS_GLOBAL_TYPES_H
