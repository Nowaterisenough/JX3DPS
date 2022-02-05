/**
 * @Description : 角色，包括技能、buff、状态等
 * @Author      : NoWats
 * @Date        : 2022-02-04 12:07:37
 * @Update      : NoWats
 * @LastTime    : 2022-02-04 20:46:15
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
    Player(Class classType);
    virtual ~Player();

    /* 心法 */
    Class GetClass();

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

    /* 属性 */
    Attr &Attr();

    /* 技能 */
    SkillHash &Skill();

    /* Buff */
    BuffHash &Buff();

    /* 橙武效果 */
    virtual void SetGearSetCW(bool gearSetCW = false) = 0;
    bool         GetGearSetCW() const;

    /* 门派套装技能效果 */
    virtual void SetGearSetClass(bool gearSetClass = false) = 0;

    /* 门派套装攻击效果 */
    virtual void SetGearSetAttack(bool gearSetAttack = false) = 0;
    bool         GetGearSetAttack() const;

protected:
    /* 心法 */
    Class m_classType;

    /* 读条 */
    bool m_isCast;

    /* 倒读条 */
    bool m_isReCast;

    /* 历史技能 */
    Id_t m_lastSkill;

    /* 停手 */
    bool m_stop;

    /* 属性 */
    class Attr *m_attr;

    /* 橙武效果 */
    bool m_gearSetCW;

    /* 门派套装攻击效果 */
    bool m_gearSetAttack;

    /* 技能 */
    SkillHash m_skillHash;

    /* Buff */
    BuffHash m_buffHash;

    /* 公共冷却 */
    Frame_t m_publicCooldown;
};

} // namespace JX3DPS

#endif // PLAYER_H
