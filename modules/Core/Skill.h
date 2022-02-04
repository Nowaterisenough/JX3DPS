#ifndef SKILL_H
#define SKILL_H


#include <map>
#include <list>
#include <vector>

#include "Core/Global.h"

namespace JX3DPS {

class Player;
class Target;

using TargetList = std::list<Target *>;

class Skill
{
public:
    Skill(Player *player);
    virtual ~Skill();

    /* 执行 */
    virtual void Cast(TargetList &targetList,
                      Stats &stats,
                      SIM_MODE &mode) = 0;

    /* 中断技能 */
    virtual void Interrupt(TargetList &targetList,
                           Stats &stats,
                           SIM_MODE &mode) = 0;
    /* 公共调息就绪 */
    bool IsReady();

    /* 下次执行时间 */
    Frame_t GetNextTime();

    /* 刷新时间 */
    void UpdateTime(Frame_t frames);

    /* ID */
    Id_t GetId();

    /* 名称 */
    const std::string &GetName();

    /* 后缀字段 */
    const std::string &GetSubName(int subNum);

    /* 强度字段 */
    const std::string &GetLevelName(int levelNum);

    /* 词缀数 */
    int GetSubNum();

    /* 强度层数 */
    int GetLevelNum();

    /* 充能数 */
    int GetEnergyNum();

    /* 冷却时间 */
    Frame_t GetCooldown();

    /* 公共冷却技能 */
    bool IsPublicSkill();

    /* 冷却加成 */
    void UpdateSkillCooldown(Frame_t frames);

    /* 会心加成 */
    void UpdateSkillCriticalStrikePercent(Pct_t percent);

    /* 会心效果加成 */
    void UpdateSkillCriticalStrikePowerPercent(Pct_t percent);

    /* 伤害加成 */
    void UpdateSkillDamageBinPercent(BinPct_t binPercent);

    /* 命中加成 */
    void UpdateSkillHitValuePercent(Pct_t percent);

    /* 设置延迟 */
    static void SetPing(int minPing, int maxPing);

protected:
    /* 判定 */
    TableRes Roll(Pct_t playerHitValuePercent,
                  Pct_t playerCriticalStrikePercent,
                  Pct_t playerStrainPercent,
                  Pct_t targetMissPercent,
                  Pct_t targetSightPercent,
                  RollType rollType = RollType::COMMON);

    TableRes GetRollResult(Target &target);

    /* 外功伤害 */
    DamageStats GetPhysicsDamage(Target &target,
                                 TableRes tableRes,
                                 std::string &subName,
                                 int level);

    /* 外功统计 */
    void UpdatePhysicsStats(Target &target,
                            Stats &stats,
                            SIM_MODE mode,
                            TableRes tableRes,
                            std::string &subName,
                            int level);
    /* 内功伤害 */
    DamageStats GetMagicDamage(Target &target,
                               TableRes tableRes,
                               std::string &subName,
                               int level);

    /* 内功统计 */
    void UpdateMagicStats(Target &target,
                          Stats &stats,
                          SIM_MODE mode,
                          TableRes tableRes,
                          std::string &subName,
                          int level);

    /* 重置公共冷却 */
    void ResetPublicCooldown();

protected_var:
    /* 角色 */
    Player *m_player;

    /* ping范围 */
    static int s_minPing;
    static int s_maxPing;

    /* 公共冷却 */
    static Frame_t s_publicCooldown;
    Frame_t *m_publicCooldown;
    int m_isPublicCooldown;

    /* ID */
    Id_t m_id;

    /* 名称 */
    std::string m_name;

    /* 词缀字段 */
    std::vector<std::string> m_subNameVec;

    /* 强度字段 */
    std::vector<std::string> m_levelNameVec;

    /* 技能CD */
    Frame_t m_cooldown;

    /* 吟唱时间 */
    Frame_t m_prepareFrames;

    /* 作用间隔 */
    Frame_t m_intervalFrames;

    /* 作用次数 */
    int m_effectNum;

    /* 充能数 */
    int m_energyNum;

    /* 冷却加成 */
    Frame_t m_skillCooldownAdd;

    /* 技能会心加成 */
    Pct_t m_skillCriticalStrikePercentAdd;

    /* 技能会心效果加成 */
    Pct_t m_skillCriticalStrikePowerPercentAdd;

    /* 技能伤害加成 */
    Pct_t m_skillDamageBinPercentAdd;

    /* 技能命中加成 */
    Pct_t m_skillHitValuePercentAdd;

    /* 伤害参数 */
    std::map<std::string, std::vector<DamageParam>> m_damageParam;
};

}

#endif // SKILL_H
