/**
 * Project: JX3DPS
 * File: TaiXuJianYi.h
 * Description:
 * Created Date: 2023-05-29 17:22:39
 * Author: 难为水
 * -----
 * Last Modified: 2023-07-15 03:44:52
 * Modified By: 难为水
 * -----
 * HISTORY:
 * Date      	By     	Comments
 * ----------	-------	----------------------------------------------------------
 */

#ifndef JX3DPS_TAIXUJIANYI_H_
#define JX3DPS_TAIXUJIANYI_H_

#include <spdlog/spdlog.h>

#include "Buff.h"
#include "Player.h"
#include "Skill.h"

namespace JX3DPS {

namespace TaiXuJianYi {

namespace Talent {

const Id_t BEGIN = 1400;

const Id_t CHAO_XUAN = 1400;
const Id_t XIN_GU    = 1401;
const Id_t BU_SHAN   = 1402;

const Id_t HUAN_YUE     = 1403;
const Id_t SHEN_MAI     = 1404;
const Id_t TU_GU_NA_XIN = 1405;

const Id_t XUAN_DE      = 1406;
const Id_t BAI_HONG     = 1407;
const Id_t BU_YI        = 1408;
const Id_t HUA_SAN_QING = 1409;

const Id_t YUN_ZHONG_JIAN     = 1410;
const Id_t WU_YI              = 1411;
const Id_t JING_HUA_YING      = 1412;
const Id_t JIAN_FEI_JING_TIAN = 1413;

const Id_t DU_XIAO  = 1414;
const Id_t FENG_SHI = 1415;
const Id_t KUANG_GE = 1416;
const Id_t HE_XU    = 1417;

const Id_t JIE_NIU  = 1418;
const Id_t ZHAN_CUN = 1419;
const Id_t DIE_REN  = 1420;
const Id_t KAI_DUI  = 1421;

const Id_t QIE_YU            = 1422;
const Id_t WU_WAI_JIANG_SHAN = 1423;
const Id_t CHANG_SHENG       = 1424;
const Id_t LIE_XING          = 1425;

const Id_t FU_YIN    = 1426;
const Id_t WU_WO     = 1427;
const Id_t RUO_SHUI  = 1428;
const Id_t DOU_ZHUAN = 1429;
const Id_t LIE_YUN   = 1430;

const Id_t TONG_GEN = 1431;
const Id_t SHI_FU   = 1432;
const Id_t GU_CHANG = 1433;
const Id_t SUI_WU   = 1434;
const Id_t WU_WEI   = 1435;

const Id_t QI_SHENG         = 1436;
const Id_t WU_WO_LIANG_WANG = 1437;
const Id_t JIAN_SHI         = 1438;
const Id_t JIAN_RU          = 1439;
const Id_t ZHI_ROU          = 1440;

const Id_t WU_YU      = 1441;
const Id_t XIONG_NIAN = 1442;
const Id_t BU_ZHENG   = 1443;
const Id_t XU_JI      = 1444;
const Id_t BEI_CHEN   = 1445;

const Id_t XIA_ZHONG          = 1446;
const Id_t JIAN_DAO_WAN_XIANG = 1447;
const Id_t XUAN_MEN           = 1448;
const Id_t XING_TIAN_DAO      = 1449;
const Id_t LING_YUN_JIAN      = 1450;

const Id_t END = 1451;
} // namespace Talent

namespace Skill {

class PoZhao : public Skill
{
    SKILL_DEFAULT_FUNCTION(PoZhao)

public:
    void TriggerDamage(Id_t targetId, int sub, int level);
};

class WuWoWuJian : public Skill
{
    SKILL_DEFAULT_FUNCTION(WuWoWuJian)

public:
    void TriggerFengShiAdd();
    void TriggerFengShiClear();

    void SubEffect();
    void SubEffectBaiHong(Id_t targetId, int qidian);
};

class BaHuangGuiYuan : public Skill
{
    SKILL_DEFAULT_FUNCTION(BaHuangGuiYuan)

public:
    void SubEffect();

    void ResetCooldown();
    void ClearCooldown();
};

class SanHuanTaoYue : public Skill
{
    SKILL_DEFAULT_FUNCTION(SanHuanTaoYue)

public:
    void SubEffect();

    int m_index = 0;
};

class WanJianGuiZong : public Skill
{
    SKILL_DEFAULT_FUNCTION(WanJianGuiZong)

public:
    void SubEffect();
};

class RenJianHeYi : public Skill
{
    SKILL_DEFAULT_FUNCTION(RenJianHeYi)

public:
    void SubEffect();
};

class RenJianHeYiSuiXingChen : public Skill
{
    SKILL_DEFAULT_FUNCTION(RenJianHeYiSuiXingChen)

public:
    void SubEffect();
};

class RenJianHeYiTunRiYue : public Skill
{
    SKILL_DEFAULT_FUNCTION(RenJianHeYiTunRiYue)

public:
    void SubEffect();
};

class SanChaiJianFa : public Skill
{
    SKILL_DEFAULT_FUNCTION(SanChaiJianFa)

public:
    void SubEffect();
};

class SuiXingChen : public Skill
{
    SKILL_DEFAULT_FUNCTION(SuiXingChen)

public:
    void SubEffect();
};

class ShengTaiJi : public Skill
{
    SKILL_DEFAULT_FUNCTION(ShengTaiJi)

public:
    void SubEffect();
};

class TunRiYue : public Skill
{
    SKILL_DEFAULT_FUNCTION(TunRiYue)

public:
    void SubEffect();
};

class ZiQiDongLai : public Skill
{
    SKILL_DEFAULT_FUNCTION(ZiQiDongLai)

public:
    void SubEffect();
};

class JingHuaYing : public Skill
{
    SKILL_DEFAULT_FUNCTION(JingHuaYing)

public:
    void TriggerAddJingHuaYing();
    void TriggerClearJingHuaYing();

    void SubEffect(int sub);
};

} // namespace Skill

namespace Buff {

class DieRen : public Buff
{
    BUFF_DEFAULT_FUNCTION(DieRen)

public:
    void TriggerQieYu(Id_t targetId);
    void TriggerHuanYue(Id_t targetId);

    void SubEffect(Id_t targetId, int stackNum);
    void SubEffectQieYun(Id_t targetId, int stackNum, int effectCount);
    void SubEffectAdd(Id_t targetId, int stackNum);
    void SubEffectClear(Id_t targetId, int stackNum);
};

class WanXiangGuiYuan : public Buff
{
    BUFF_DEFAULT_FUNCTION(WanXiangGuiYuan)

public:
    void SubEffect(Id_t targetId, int stackNum);
};

class RenJianHeYi : public Buff
{
    BUFF_DEFAULT_FUNCTION(RenJianHeYi)
public:
    void SubEffect(Id_t targetId);
};    

class ZiQiDongLai : public Buff
{
    BUFF_DEFAULT_FUNCTION(ZiQiDongLai)

public:
    void SubEffectAdd();
    void SubEffectClear();
};

class XuanMen : public Buff
{
    BUFF_DEFAULT_FUNCTION(XuanMen)

public:
    void SubEffectAdd(int stackNum);
    void SubEffectClear(int stackNum);
};

class FieldSuiXingChen : public Buff
{
    BUFF_DEFAULT_FUNCTION(FieldSuiXingChen)

public:
    void SubEffect();
    void SubEffectAdd(int stackNum = 1);
    void SubEffectClear(int stackNum = 1);
};

class FieldShengTaiJi : public Buff
{
    BUFF_DEFAULT_FUNCTION(FieldShengTaiJi)

public:
    void SubEffect();
    void SubEffectAdd(int stackNum = 1);
};

class FieldTunRiYue : public Buff
{
    BUFF_DEFAULT_FUNCTION(FieldTunRiYue)

public:
    void SubEffect();
    void SubEffectAdd(int stackNum = 1);
    void SubEffectClear(int stackNum = 1);
};

class FieldSuiXingChenQiSheng : public Buff
{
    BUFF_DEFAULT_FUNCTION(FieldSuiXingChenQiSheng)

public:
    void SubEffect();
    void SubEffectClear(int stackNum = 1);
};

class SuiXingChen : public Buff
{
    BUFF_DEFAULT_FUNCTION(SuiXingChen)

public:
    void SubEffectAdd();
    void SubEffectClear();
};

class QiSheng : public Buff
{
    BUFF_DEFAULT_FUNCTION(QiSheng)

public:
    void SubEffectAdd();
    void SubEffectClear();
};

class FengShi : public Buff
{
    BUFF_DEFAULT_FUNCTION(FengShi)

public:
    void SubEffectAdd();
    void SubEffectClear();
};

class FieldLieYun : public Buff
{
    BUFF_DEFAULT_FUNCTION(FieldLieYun)

public:
    void SubEffectAdd(Id_t targetId);
};

class LieYun : public Buff
{
    BUFF_DEFAULT_FUNCTION(LieYun)

public:
    void SubEffectAdd();
    void SubEffectClear();
};

class TunRiYue : public Buff
{
    BUFF_DEFAULT_FUNCTION(TunRiYue)

public:
    void SubEffectAdd();
};

class JingHuaYing : public Buff
{
    BUFF_DEFAULT_FUNCTION(JingHuaYing)

public:
    void SubEffectAdd();
    void SubEffectClear();
};

class ChiYing : public Buff
{
    BUFF_DEFAULT_FUNCTION(ChiYing)

public:
    void TriggerDamage();
};

class YunZhongJianSuiXingChen : public Buff
{
    BUFF_DEFAULT_FUNCTION(YunZhongJianSuiXingChen)

public:
    void SubEffect();
};

class YunZhongJianShengTaiJi : public Buff
{
    BUFF_DEFAULT_FUNCTION(YunZhongJianShengTaiJi)

public:
    void SubEffect();
};

class YunZhongJianTunRiYue : public Buff
{
    BUFF_DEFAULT_FUNCTION(YunZhongJianTunRiYue)

public:
    void SubEffect();
};

class ClassFeatureRongJin : public Buff
{
    BUFF_DEFAULT_FUNCTION(ClassFeatureRongJin)

public:
    void SubEffectAdd();
};

class WeaponEffectCW1 : public Buff
{
    BUFF_DEFAULT_FUNCTION(WeaponEffectCW1)

public:
    void TriggerAdd();

    void SubEffectAdd();
    void SubEffectClear();
};

class ClassSetBuffJianMing : public Buff
{
    BUFF_DEFAULT_FUNCTION(ClassSetBuffJianMing)

public:
    void TriggerAdd();

    void SubEffectAdd();
    void SubEffectClear();
};

} // namespace Buff

class Player : public JX3DPS::Player
{
public:
    Player();
    ~Player();

    Player::Player(const Player &other) : JX3DPS::Player(other)
    {

        // 添加心法气点随机帧数分布
        this->buffs[JX3DPS::Buff::CLASS_FEATURE]->Add();
    }

    Player &Player::operator=(const JX3DPS::Player &other) override
    {
        if (this == &other) {
            return *this;
        }

        JX3DPS::Player::operator=(other);

        return *this;
    }

    Player *Player::Clone() const override { return new Player(*this); }

    void Init() override;

    std::list<Id_t> fields;

    void RemoveField(Id_t fieldId, int stackNum = 1)
    {
        int stack = 0;
        for (auto it = fields.begin(); it != fields.end();) {
            if (*it == fieldId) {
                it = fields.erase(it);
                ++stack;
                if (stack == stackNum) {
                    break;
                }
            } else {
                ++it;
            }
        }
    }
};

} // namespace TaiXuJianYi

} // namespace JX3DPS

#endif // JX3DPS_TAIXUJIANYI_H_
