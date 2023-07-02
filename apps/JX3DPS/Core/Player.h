/**
 * Project: JX3DPS
 * File: Player.h
 * Description:
 * Created Date: 2023-05-29 17:22:39
 * Author: 难为水
 * -----
 * Last Modified: 2023-07-03 03:51:49
 * Modified By: 难为水
 * -----
 * HISTORY:
 * Date      	By     	Comments
 * ----------	-------	----------------------------------------------------------
 */

#ifndef JX3DPS_PLAYER_H_
#define JX3DPS_PLAYER_H_

#include "Attr.h"
#include "Buff.h"

namespace JX3DPS {

class Player
{
public:
    static std::shared_ptr<Player> PlayerFactoryGenerate(Class classType);

public:
    Player();
    virtual ~Player();

    Player(const Player &other);

    virtual Player &operator=(const Player &other);

    virtual Player *Clone() const = 0;

    virtual void Init() = 0;

    void SetTargets(Targets *targets);

    void UpdateGlobalCooldown(Frame_t next);

    void AddQidian(int qidian);

public:

    Class classType;

    int qidian = 10; // 气点
    int mana   = 0;  // 内力
    int rage   = 0;  // 怒气
    int energy = 0;  // 元气
    int sun    = 0;  // 日精
    int moon   = 0;  // 月精

    PctFloat_t life = 1.0;
    PctFloat_t mane = 1.0;

    Frame_t        globalCooldown      = 0;  // 当前冷却
    Frame_t        globalCooldownFixed = 24; // 当前冷却
    static Frame_t s_globalCooldown;         // 公共冷却

    Id_t targetId = 0;                       // 目标ID

    Id_t lastCastSkill = 0;                  // 上次施放技能ID

    bool isCast   = false;                   // 是否正在蓄力
    bool isReCast = false;                   // 是否正在倒读条

    std::unique_ptr<Attr> attr = nullptr;    // 属性

    Talents talents;                         // 奇穴列表
    Secrets secrets;                         // 秘籍列表
    Skills  skills;                          // 技能列表
    Buffs   buffs;                           // BUFF列表

    bool end  = false;                       // 模拟结束的标志
    bool stop = false;                       // 是否停手

    int delayMin = 30;
    int delayMax = 65;

    bool weaponCW      = false;
    bool classSetBuff  = false;
    bool classSetSkill = false;

    bool enchantShoes  = false;
    bool enchantWrist  = false;
    bool enchantBelt   = false;
    bool enchantJacket = false;
    bool enchantHat    = false;

    HasBuff3rd hasBuff3rd;
    

    PctInt_t damageAddPercentInt = 0;
};

} // namespace JX3DPS

#endif // JX3DPS_PLAYER_H_