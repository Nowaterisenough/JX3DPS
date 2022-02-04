/**
 * @Description : 
 * @Author      : NoWats
 * @Date        : 2022-02-04 12:07:17
 * @Update      : NoWats
 * @LastTime    : 2022-02-04 13:15:27
 * @FilePath    : \JX3DPS\modules\Core\Buff.h
 */

#ifndef BUFF_H
#define BUFF_H

#include <map>
#include <list>
#include <vector>

#include "Core/Global.h"
#include "Core/Attr.h"

namespace JX3DPS {

class Player;
class Target;

using TargetList = std::list<Target *>;

class Buff
{
public:
    Buff(Player *player, Target *target = nullptr);
    virtual ~Buff();

    /* 执行 */
    virtual void Cast(TargetList &targetList, Stats &stats, SIM_MODE &mode) = 0;

    /* 刷新Buff */
    virtual void Refresh() = 0;

    /* 判断是否存在 */
    bool IsExist();

    /* 下次作用时间 */
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

    /* 叠加层数 */
    int GetStackNum();

    /* 剩余作用次数 */
    int GetEffectNum();

    /* 内置CD */
    Frame_t GetCooldown();

    /* 剩余时间 */
    Frame_t GetRestTime();

protected:
    /* 判定 */
    TableRes Roll(Pct_t    playerHitValuePercent,
                  Pct_t    playerCriticalStrikePercent,
                  Pct_t    playerStrainPercent,
                  Pct_t    targetMissPercent,
                  Pct_t    targetSightPercent,
                  RollType rollType = RollType::COMMON);

    TableRes GetRollResult(Attr &attr, Target &target);

    /* 外功伤害 */
    DamageStats
    GetPhysicsDamage(Attr &attr, Target &target, TableRes tableRes, std::string &subName, int level);

    /* 外功统计 */
    void UpdatePhysicsStats(Attr        &attr,
                            Target      &target,
                            Stats       &stats,
                            SIM_MODE     mode,
                            TableRes     tableRes,
                            std::string &subName,
                            int          level);
    /* 内功伤害 */
    DamageStats
    GetMagicDamage(Attr &attr, Target &target, TableRes tableRes, std::string &subName, int level);

    /* 内功统计 */
    void UpdateMagicStats(Attr        &attr,
                          Target      &target,
                          Stats       &stats,
                          SIM_MODE     mode,
                          TableRes     tableRes,
                          std::string &subName,
                          int          level);

    protected_var :
        /* 角色 */
        Player *m_player;

    /* 角色 */
    Target *m_target;

    /* ID */
    Id_t m_id;

    /* 名称 */
    std::string m_name;

    /* 词缀字段 */
    std::vector<std::string> m_subNameVec;

    /* 强度字段 */
    std::vector<std::string> m_levelNameVec;

    /* 第三方冷却 */
    Frame_t m_3rdCooldown;

    /* 内置CD */
    Frame_t m_cooldown;

    /* 持续时间 */
    Frame_t m_lastFrames;

    /* 作用间隔 */
    Frame_t m_intervalFrames;

    /* 作用次数 */
    int m_effectNum;

    /* 叠加层数 */
    int m_stackNum;

    /* 伤害参数 */
    std::map<std::string, std::vector<DamageParam>> m_damageParam;

    /* 快照属性 */
    Attr *m_snapAttr;
};

} // namespace JX3DPS

#endif // BUFF_H
