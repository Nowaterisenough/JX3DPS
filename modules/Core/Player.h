/**
 * @Description : 角色，包括技能、buff、状态等
 * @Author      : NoWats
 * @Date        : 2022-02-04 12:07:37
 * @Update      : NoWats
 * @LastTime    : 2022-02-06 13:47:30
 * @FilePath    : \JX3DPS\modules\Core\Player.h
 */

#ifndef PLAYER_H
#define PLAYER_H

#include <unordered_map>
#include <list>

#include "Common/ConstVal.h"

namespace JX3DPS {

class Attr;
class Skill;
class Buff;

class Player
{
public:
    Player(JX3Class classType);
    virtual ~Player();

    /* 心法 */
    JX3Class GetClass();

    /* 刷新时间 */
    void UpdateTime(Frame_t frames);

    /* 读条状态 */
    void SetCast(bool isCast);
    bool IsCast() const;

    /* 倒读条状态 */
    void SetReCast(bool isReCast);
    bool IsReCast() const;

    /* 历史技能 */
    void SetLastSkill(Id_t skillId);
    Id_t GetLastSkill() const;

    /* 停手 */
    void SetStop(bool stop);
    bool GetStop() const;

    /* 橙武效果 */
    virtual void SetGearSetCW(bool gearSetCW = false);
    bool         GetGearSetCW() const;

    /* 门派套装技能效果 */
    virtual void SetGearSetClass(bool gearSetClass = false);

    /* 门派套装攻击效果 */
    virtual void SetGearSetAttack(bool gearSetAttack = false);
    bool         GetGearSetAttack() const;

    /* 属性 */
    Attr *attr = nullptr;

    /* 技能 */
    Skills skills;

    /* Buff */
    Buffs buffs;

    /* 奇穴 */
    Talents talents;

    /* 公共冷却 */
    Frame_t publicCooldown;

protected:
    /* 心法 */
    JX3Class m_classType;

    /* 读条 */
    bool m_isCast;

    /* 倒读条 */
    bool m_isReCast;

    /* 历史技能 */
    Id_t m_lastSkill;

    /* 停手 */
    bool m_stop;

    /* 橙武效果 */
    bool m_gearSetCW;

    /* 门派套装攻击效果 */
    bool m_gearSetAttack;
};

} // namespace JX3DPS

#endif // PLAYER_H
