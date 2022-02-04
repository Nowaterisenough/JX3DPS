/**
 * @Description :
 * @Author      : NoWats
 * @Date        : 2022-02-04 12:07:37
 * @Update      : NoWats
 * @LastTime    : 2022-02-04 13:15:56
 * @FilePath    : \JX3DPS\modules\Core\Player.h
 */

#ifndef PLAYER_H
#define PLAYER_H

#include <unordered_map>
#include <list>

#include "Core/Global.h"
#include "Core/Attr.h"

namespace JX3DPS {

class Skill;
class Buff;
class Macro;

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
    bool SetCast(bool isCast);
    bool IsCast();

    /* 倒读条状态 */
    bool SetReCast(bool isReCast);
    bool IsReCast();

    /* 历史技能 */
    Id_t SetLastSkill(Id_t skillId);
    Id_t GetLastSkill();

    /* 停手 */
    bool SetStop(bool stop);
    bool GetStop();

    /* 属性 */
    Attr &Attr();

    /* 橙武效果 */
    virtual bool SetGearSetCW(bool gearSetCW = false) = 0;
    bool         GetGearSetCW();

    /* 门派套装技能效果 */
    virtual bool SetGearSetClass(bool gearSetClass = false) = 0;

    /* 门派套装攻击效果 */
    virtual bool SetGearSetAttack(bool gearSetAttack = false) = 0;
    bool         GetGearSetAttack();

    public_var :
        /* 技能 */
        std::unordered_map<Id_t, Skill *>
            m_skillHash;

    /* Buff */
    std::unordered_map<Id_t, Buff *> m_buffHash;

    /* 公共冷却 */
    Frame_t m_publicCooldown;

    protected_var :
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
};

} // namespace JX3DPS

#endif // PLAYER_H
