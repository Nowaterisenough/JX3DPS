/**
 * Project: JX3DPS
 * File: Skill.h
 * Description:
 * Created Date: 2023-05-29 17:22:39
 * Author: 难为水
 * -----
 * Last Modified: 2023-07-03 05:49:13
 * Modified By: 难为水
 * -----
 * HISTORY:
 * Date      	By     	Comments
 * ----------	-------	----------------------------------------------------------
 */

#ifndef JX3DPS_SKILL_H_
#define JX3DPS_SKILL_H_

#include "Globals.h"

#define SKILL_DEFAULT_FUNCTION(class_name)                 \
                                                           \
public:                                                    \
    class_name(JX3DPS::Player *player, Targets *targets);  \
    class_name(const class_name &other) : Skill(other) { } \
    ~class_name() { }                                      \
    class_name &operator=(const class_name &other)         \
    {                                                      \
        if (this == &other) {                              \
            return *this;                                  \
        }                                                  \
        JX3DPS::Skill::operator=(other);                   \
        return *this;                                      \
    }                                                      \
    class_name *Clone() const                              \
    {                                                      \
        return new class_name(*this);                      \
    }                                                      \
    void Trigger() override;                               \
    void Cast() override;

namespace JX3DPS {
class Skill
{
public:
    static constexpr Id_t SAN_CHAI_JIAN_FA  = 1000;
    static constexpr Id_t WU_WO_WU_JIAN     = 1001;
    static constexpr Id_t BA_HUANG_GUI_YUAN = 1002;
    static constexpr Id_t SAN_HUAN_TAO_YUE  = 1003;
    static constexpr Id_t WAN_JIAN_GUI_ZONG = 1004;
    static constexpr Id_t REN_JIAN_HE_YI    = 1005;
    static constexpr Id_t SUI_XING_CHEN     = 1006;
    static constexpr Id_t SHENG_TAI_JI      = 1007;
    static constexpr Id_t TUN_RI_YUE        = 1008;
    static constexpr Id_t ZI_QI_DONG_LAI    = 1009;
    static constexpr Id_t JING_HUA_YING     = 1010;

    inline static const std::unordered_map<Id_t, std::string> &SKILL_NAME = {
        {SAN_CHAI_JIAN_FA,   "三柴剑法"},
        { WU_WO_WU_JIAN,     "无我无剑"},
        { BA_HUANG_GUI_YUAN, "八荒归元"},
        { SAN_HUAN_TAO_YUE,  "三环套月"},
        { WAN_JIAN_GUI_ZONG, "万剑归宗"},
        { REN_JIAN_HE_YI,    "人剑合一"},
        { SUI_XING_CHEN,     "碎星辰"   },
        { SHENG_TAI_JI,      "生太极"   },
        { TUN_RI_YUE,        "吞日月"   },
        { ZI_QI_DONG_LAI,    "紫气东来"},
        { JING_HUA_YING,     "镜花影"   },
    };

public:
    Skill(Player *player, Targets *targets);
    virtual ~Skill();

    Skill(const Skill &other);
    Skill         &operator=(const Skill &other);
    virtual Skill *Clone() const = 0;

    virtual void Cast()    = 0;
    virtual void Trigger() = 0;

    Frame_t GetNextKeyFrame() const;
    void    UpdateKeyFrame(Frame_t frame);

    RollResult GetPhysicsRollResult() const;
    RollResult GetMagicRollResult() const;

    Damage CalcPhysicsDamage(Id_t targetId, RollResult rollResult, int sub = 0, int level = 0) const;

    Damage CalcMagicDamage(Id_t targetId, RollResult rollResult, int sub = 0, int level = 0) const;

    void Record(Id_t          targetId   = 1,
                RollResult    rollResult = RollResult::HIT,
                const Damage &damage     = Damage(),
                int           sub        = 0,
                int           level      = 0);

    void SetPlayer(Player *player);
    void SetTargets(Targets *targets);

    Frame_t GetCooldown() const;
    float   GetRange() const;
    int     GetEnergyCount() const;

    void SetCooldownReduction(Frame_t cooldown);

    DamageStats &GetDamageStats();

    const std::string &GetName() const;

    bool IsEnergy() const;

protected:
    Player  *m_player  = nullptr;
    Targets *m_targets = nullptr;

    /* ID */
    Id_t m_id = 0;

    /* 名称 */
    std::string m_name = "";

    /* 施法距离 */
    float m_range = JX3DPS_UNLIMITED_RANGE;

    /* 词缀字段 */
    std::vector<std::string> m_subNames;

    /* 强度字段 */
    std::vector<std::string> m_levelNames;

    /* 公共冷却 */
    Frame_t *m_globalCooldown = nullptr;

    /* 技能冷却 */
    Frame_t m_cooldown = 0;

    Frame_t m_cooldownFixed = 0;

    /* 蓄力时间 */
    Frame_t m_prepareFrames = JX3DPS_INVALID_FRAMES_SET;

    Frame_t m_prepareFramesFixed = JX3DPS_INVALID_FRAMES_SET;

    /* 充能数 */
    int m_energyCount = -1;

    int m_energyCountFixed = JX3DPS_INVALID_ENERGY_SET;

    int m_targetCountFixed = 0;

    /* 技能会心加成 */
    PctFloat_t m_skillCriticalStrikeAddPercent = 0.0;

    /* 技能会心效果加成 */
    PctFloat_t m_skillCriticalStrikePowerAddPercent = 0.0;

    /* 技能伤害加成 */
    PctInt_t m_skillDamageAddPercentInt = 0;

    /* 技能气点加成 */
    int m_skillQidianAdd = 0;

    /* 伤害参数 */
    DamageParams m_damageParams;

    /* 记录 */
    DamageStats m_damageStats;
};
} // namespace JX3DPS
#endif // JX3DPS_SKILL_H_