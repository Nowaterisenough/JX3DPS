/**
 * Project: JX3DPS
 * File: Buff.h
 * Description:
 * Created Date: 2023-05-29 17:22:39
 * Author: 难为水
 * -----
 * Last Modified: 2023-06-28 12:31:57
 * Modified By: 难为水
 * -----
 * HISTORY:
 * Date      	By     	Comments
 * ----------	-------	----------------------------------------------------------
 */

#ifndef JX3DPS_BUFF_H_
#define JX3DPS_BUFF_H_

#include <unordered_map>

#include "Globals.h"
#include "JX3Params.h"

#define BUFF_DEFAULT_FUNCTION(class_name)                                                                        \
                                                                                                                 \
public:                                                                                                          \
    class_name(JX3DPS::Player *player, Targets *targets);                                                        \
    class_name(const class_name &other) : Buff(other) { }                                                        \
    ~class_name() { }                                                                                            \
    class_name &operator=(const class_name &other)                                                               \
    {                                                                                                            \
        if (this == &other) {                                                                                    \
            return *this;                                                                                        \
        }                                                                                                        \
        JX3DPS::Buff::operator=(other);                                                                          \
        return *this;                                                                                            \
    }                                                                                                            \
    class_name *Clone() const                                                                                    \
    {                                                                                                            \
        return new class_name(*this);                                                                            \
    }                                                                                                            \
    void Trigger() override;                                                                                     \
    void Clear(Id_t targetId = JX3DPS_PLAYER, int stackNum = 1) override;                                        \
    void Add(Id_t targetId = JX3DPS_PLAYER, int stackNum = 1, Frame_t duration = JX3DPS_DEFAULT_DURATION_FRAMES) \
        override;

namespace JX3DPS {

struct TargetSnapshot
{
    /* 持续时间 */
    Frame_t duration = JX3DPS_INVALID_FRAMES_SET;

    /* 作用间隔 内置cd */
    Frame_t interval = JX3DPS_INVALID_FRAMES_SET;

    /* 层数 */
    int stackNum = 0;

    /* 快照攻击 */
    Value_t attackBase = 0;

    /* 快照攻击 */
    Value_t attackMain = 0;

    /* 快照会心 */
    PctFloat_t criticalStrikePercent = 0.0;

    /* 快照会效 */
    PctFloat_t criticalStrikePowerPercent = 0.0;

    /* 快照加速 */
    PctFloat_t hastePercent = 0.0;

    /* 快照无双 */
    PctFloat_t strainPercent = 0.0;
};

using TargetSnapshots = std::unordered_map<Id_t, TargetSnapshot>;

class Buff
{

public:
    // 心法特性
    static constexpr Id_t CLASS_FEATURE = 100;

    // 武器buff
    static constexpr Id_t WEAPON_EFFECT_CW_1      = 110;
    static constexpr Id_t WEAPON_EFFECT_CW_2      = 111;
    static constexpr Id_t WEAPON_EFFECT_CW_3      = 112;
    static constexpr Id_t WEAPON_EFFECT_CLASS     = 113;
    static constexpr Id_t WEAPON_EFFECT_SPECIAL   = 114;
    static constexpr Id_t WEAPON_EFFECT_LIGHTNING = 115;
    static constexpr Id_t WEAPON_EFFECT_WATER     = 116;

    // 腰坠buff
    static constexpr Id_t PENDANT_OVERCOME = 117;

    // 套装buff
    static constexpr Id_t CLASS_SET_BUFF = 118;

    // 大附魔buff
    static constexpr Id_t ENCHANT_SHOES  = 119;
    static constexpr Id_t ENCHANT_JACKET = 120;
    static constexpr Id_t ENCHANT_HAT    = 121;
    static constexpr Id_t ENCHANT_BELT   = 122;
    static constexpr Id_t ENCHANT_WRIST  = 123;

    // 阵眼buff
    static constexpr Id_t CORE_TAI_XU_JIAN_YI = 130;

    // 三方buff
    static constexpr Id_t THIRD_SUI_XING_CHEN     = 170;
    static constexpr Id_t THIRD_ZHUANG_ZHOU_MENG  = 171;
    static constexpr Id_t THIRD_ZUO_XUAN_YOU_XUAN = 172;
    static constexpr Id_t THIRD_HAO_LING_SAN_JUN  = 173;
    static constexpr Id_t THIRD_SHE_SHEN_HONG_FA  = 174;
    static constexpr Id_t THIRD_ZHEN_FEN          = 175;
    static constexpr Id_t THIRD_CHAO_SHENG        = 176;
    static constexpr Id_t THIRD_LI_DI_CHENG_FO    = 177;
    static constexpr Id_t THIRD_MEI_HUA_SAN_NONG  = 178;
    static constexpr Id_t THIRD_SHU_KUANG         = 179;
    static constexpr Id_t THIRD_HAN_CHANG_LIN_LI  = 180;
    static constexpr Id_t THIRD_HAN_XIAO_QIAN_JUN = 181;
    static constexpr Id_t THIRD_FEN_LAN           = 182;
    static constexpr Id_t THIRD_PIAO_HUANG        = 183;
    static constexpr Id_t THIRD_XIAN_WANG_GU_DING = 184;
    static constexpr Id_t THIRD_QING_JUAN         = 185;
    static constexpr Id_t THIRD_XIU_QI            = 186;
    static constexpr Id_t THIRD_HAN_RU_LEI        = 187;

    // 三方tbuff
    static constexpr Id_t THIRD_JIE_HUO_ZHAN = 200;
    static constexpr Id_t THIRD_QIU_SU       = 201;
    static constexpr Id_t THIRD_PO_FENG      = 202;
    static constexpr Id_t THIRD_JIN_FENG     = 203;
    static constexpr Id_t THIRD_XU_RUO       = 204;
    static constexpr Id_t THIRD_PO_JIA       = 205;

    // 太虚剑意buff
    static constexpr Id_t ZI_QI_DONG_LAI = 1100;
    static constexpr Id_t XUAN_MEN       = 1101;
    static constexpr Id_t SUI_XING_CHEN  = 1102;
    static constexpr Id_t FIELD_SUI_XING_CHEN = 1103; // 气场和云中剑序号排序不能变，影响实现
    static constexpr Id_t YUN_ZHONG_JIAN_SUI_XING_CHEN = 1104;
    static constexpr Id_t FIELD_SHENG_TAI_JI           = 1105;
    static constexpr Id_t YUN_ZHONG_JIAN_SHENG_TAI_JI  = 1106;
    static constexpr Id_t FIELD_TUN_RI_YUE             = 1107;
    static constexpr Id_t YUN_ZHONG_JIAN_TUN_RI_YUE    = 1108;
    static constexpr Id_t FIELD_SUI_XING_CHEN_QI_SHENG = 1109;
    static constexpr Id_t CHI_YING                     = 1110;
    static constexpr Id_t QI_SHENG                     = 1111;
    static constexpr Id_t FENG_SHI                     = 1112;
    static constexpr Id_t JING_HUA_YING                = 1113;
    static constexpr Id_t FIELD_LIE_YUN                = 1114;
    static constexpr Id_t LIE_YUN                      = 1115;

    // 太虚剑意tbuff
    static constexpr Id_t DIE_REN        = 1200;
    static constexpr Id_t REN_JIAN_HE_YI = 1201;
    static constexpr Id_t TUN_RI_YUE     = 1202;

    inline static const std::unordered_map<Id_t, std::string> &BUFF_NAME = {
        {CLASS_FEATURE,                 "心法特效"             },
        { WEAPON_EFFECT_CW_1,           "武器·橙武特效1"    },
        { WEAPON_EFFECT_CW_2,           "武器·橙武特效2"    },
        { WEAPON_EFFECT_CW_3,           "武器·小橙武特效"  },
        { WEAPON_EFFECT_CLASS,          "武器·心法特效"     },
        { WEAPON_EFFECT_SPECIAL,        "武器·龙门特效"     },
        { WEAPON_EFFECT_LIGHTNING,      "武器·雷特效"        },
        { WEAPON_EFFECT_WATER,          "武器·水特效"        },
        { PENDANT_OVERCOME,             "腰坠·破防"           },
        { CLASS_SET_BUFF,               "套装·心法"           },
        { ENCHANT_SHOES,                "大附魔·鞋"           },
        { ENCHANT_JACKET,               "大附魔·衣"           },
        { ENCHANT_HAT,                  "大附魔·头"           },
        { ENCHANT_BELT,                 "大附魔·腰"           },
        { ENCHANT_WRIST,                "大附魔·腕"           },
        { CORE_TAI_XU_JIAN_YI,          "阵眼·北斗七星阵"  },
        { THIRD_SUI_XING_CHEN,          "3rd·碎星辰"           },
        { THIRD_ZHUANG_ZHOU_MENG,       "3rd·庄周梦"           },
        { THIRD_ZUO_XUAN_YOU_XUAN,      "3rd·左旋右旋"        },
        { THIRD_HAO_LING_SAN_JUN,       "3rd·号令三军"        },
        { XUAN_MEN,                     "玄门"                   },
        { ZI_QI_DONG_LAI,               "紫气东来"             },
        { SUI_XING_CHEN,                "碎星辰"                },
        { FIELD_SUI_XING_CHEN,          "气场·碎星辰"        },
        { FIELD_SUI_XING_CHEN_QI_SHENG, "气场·碎星辰·期声"},
        { FIELD_SHENG_TAI_JI,           "气场·生太极"        },
        { FIELD_TUN_RI_YUE,             "气场·吞日月"        },
        { YUN_ZHONG_JIAN_SUI_XING_CHEN, "云中剑·碎星辰"     },
        { YUN_ZHONG_JIAN_SHENG_TAI_JI,  "云中剑·生太极"     },
        { YUN_ZHONG_JIAN_TUN_RI_YUE,    "云中剑·吞日月"     },
        { CHI_YING,                     "持盈"                   },
        { QI_SHENG,                     "期声"                   },
        { FENG_SHI,                     "风逝"                   },
        { DIE_REN,                      "叠刃"                   },
        { REN_JIAN_HE_YI,               "人剑合一"             },
        { TUN_RI_YUE,                   "吞日月"                },
        { JING_HUA_YING,                "镜花影"                },
    };

public:
    Buff(Player *player, Targets *targets);
    virtual ~Buff();

    Buff(const Buff &other);
    Buff         &operator=(const Buff &other);
    virtual Buff *Clone() const = 0;

    virtual void Trigger()                                              = 0;
    virtual void Clear(Id_t targetId = JX3DPS_PLAYER, int stackNum = 1) = 0;
    virtual void Add(Id_t targetId = JX3DPS_PLAYER, int stackNum = 1, Frame_t duration = JX3DPS_DEFAULT_DURATION_FRAMES) = 0;

    void RemoveTarget(Id_t targetId);

    void SetPlayer(Player *player);

    void SetTargets(Targets *targets);

    int     GetStackNum(Id_t targetId = JX3DPS_PLAYER) const;
    Frame_t GetTimeLeft(Id_t targetId = JX3DPS_PLAYER) const;

    Frame_t GetNextKeyFrame() const;
    void    UpdateKeyFrame(Frame_t frame);

    RollResult GetPhysicsRollResult(Id_t targetId) const;
    RollResult GetMagicRollResult(Id_t targetId) const;

    Damage CalcPhysicsDamage(Id_t       targetId,
                             RollResult rollResult,
                             int        sub         = 0,
                             int        level       = 0,
                             int        effectCount = 1,
                             int        type        = JX3_NORMAL_SKILL_DAMAGE_PARAM) const;

    Damage CalcMagicDamage(Id_t       targetId,
                           RollResult rollResult,
                           int        sub         = 0,
                           int        level       = 0,
                           int        effectCount = 1,
                           int        type        = JX3_NORMAL_SKILL_DAMAGE_PARAM) const;

    void Record(Id_t          targetId   = 1,
                RollResult    rollResult = RollResult::HIT,
                const Damage &damage     = Damage(),
                int           sub        = 0,
                int           level      = 0);

    DamageStats &GetDamageStats();

    const std::string &GetName() const;

protected:
    Player  *m_player  = nullptr;
    Targets *m_targets = nullptr;

    /* ID */
    Id_t m_id = 0;

    /* 名称 */
    std::string m_name = "";

    float m_range = JX3DPS_UNLIMITED_RANGE;

    bool m_tbuff = false;

    /* 词缀字段 */
    std::vector<std::string> m_subNames;

    /* 强度字段 */
    std::vector<std::string> m_levelNames;

    /* 快照 */
    TargetSnapshots m_targetSnapshots;

    Frame_t m_durationFixed = 0;

    Frame_t m_intervalFixed = 0;

    int m_stackNumFixed = 0;

    int m_effectCountFixed = 0;

    /* 技能会心加成 */
    PctFloat_t m_skillCriticalStrikePercentAdd = 0;

    /* 技能会心效果加成 */
    PctFloat_t m_skillCriticalStrikePowerPercentAdd = 0;

    /* 技能伤害加成 */
    PctFloat_t m_skillDamageBinPercentAdd = 0;

    /* 伤害参数 */
    DamageParams m_damageParams;

    DamageStats m_damageStats;
};

namespace Buff3rd {

class EnchantShoesPhysics : public Buff
{
    BUFF_DEFAULT_FUNCTION(EnchantShoesPhysics)

public:
    void TriggerDamage();
    void SubEffect();
};

class EnchantWristPhysics : public Buff
{
    BUFF_DEFAULT_FUNCTION(EnchantWristPhysics)

public:
    void TriggerDamage();
    void SubEffect();
};

class EnchantBelt: public Buff
{
    BUFF_DEFAULT_FUNCTION(EnchantBelt)

public:
    void TriggerAdd();
    void SubEffectAdd();
    void SubEffectClear();
};

} // namespace Buff3rd

} // namespace JX3DPS

#endif // JX3DPS_BUFF_H_