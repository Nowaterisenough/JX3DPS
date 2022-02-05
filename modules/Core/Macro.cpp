/**
 * @Description : 宏解析
 * @Author      : NoWats
 * @Date        : 2022-02-02 16:33:58
 * @Update      : NoWats
 * @LastTime    : 2022-02-05 00:03:04
 * @FilePath    : \JX3DPS\modules\Core\Macro.cpp
 */

#pragma execution_character_set("utf-8")

#include "Macro.h"

#include <regex>
#include <iostream>
#include <tuple>

#include "Common/ConstVal.h"
#include "Player.h"

using namespace std;

namespace JX3DPS {

template <Id_t, typename ...Args>
struct Condition {
    std::tuple<Id_t, Args...> params;
};

const std::unordered_map<std::string, Id_t> &BUFF_ID_HASH{
    {"紫气东来", BUF_ZI_QI_DONG_LAI},
    {"玄门", BUF_XUAN_MEN},
    {"碎星辰", BUF_SUI_XING_CHEN},
    {"生太极", BUF_SHENG_TAI_JI},
    {"吞日月", BUF_TUN_RI_YUE},
    {"云中剑-碎", BUF_YUN_ZHONG_JIAN_C},
    {"云中剑-生", BUF_YUN_ZHONG_JIAN_J},
    {"云中剑-吞", BUF_YUN_ZHONG_JIAN_Y},
    {"持盈", BUF_CHI_YING},
    {"期声", BUF_QI_SHENG},
    {"风逝", BUF_FENG_SHI},
    {"碎星辰buff", BUF_SUI_XING_CHEN_BUFF},
    {"叠刃", BUF_DIE_REN},
    {"人剑合一", BUF_REN_JIAN_HE_YI},
};

const std::unordered_map<std::string, Id_t> &SKILL_ID_HASH{
    {"无我无剑", SKI_WU_WO_WU_JIAN},
    {"八荒归元", SKI_BA_HUANG_GUI_YUAN},
    {"三环套月", SKI_SAN_HUAN_TAO_YUE},
    {"人剑合一", SKI_REN_JIAN_HE_YI},
    {"碎星辰", SKI_SUI_XING_CHEN},
    {"生太极", SKI_SHENG_TAI_JI},
    {"吞日月", SKI_TUN_RI_YUE},
    {"紫气东来", SKI_ZI_QI_DONG_LAI},
    {"三柴剑法", SKI_SAN_CHAI_JIAN_FA},
};

const std::unordered_map<std::string, Id_t> &TALENT_ID_HASH{
    {"挫锐", TAL_CUO_RUI},
    {"心固", TAL_XIN_GU},
    {"同根", TAL_TONG_GEN},
    {"深埋", TAL_SHEN_MAI},
    {"昆吾", TAL_KUN_WU},
    {"白虹", TAL_BAI_HONG},
    {"化三清", TAL_HUA_SAN_QING},
};

Macro::Macro() {}

Macro::~Macro() {}

bool Macro::Parse(const std::list<std::string> &lines)
{
    int sum = 0;
    for (const auto &line : lines) {
        sum += Parse(line);
    }
    return true;
}

bool Macro::Parse(const std::string &line)
{
    std::regex  reg("/([f]*cast)(.*)( .*)");
    std::smatch mat;
    if (std::regex_search(line, mat, reg)) {
        std::string castMode = mat[1];
        std::string condition =
            mat[2].str().substr(mat[2].str().find("[") + 1,
                                mat[2].str().find("]") - mat[2].str().find("[") - 1);

        std::string skillName =
            mat[3].str().substr(mat[3].str().find(" ") + 1, mat[3].str().length() - 1);

        Id_t skillId = SKILL_ID_HASH.at(skillName);

        std::list<std::string> orConditionStrList;
        while (condition.find("|") != std::string::npos) {
            orConditionStrList.emplace_back(condition.substr(0, condition.find("|")));
            condition = condition.substr(condition.find("|") + 1);
        }
        orConditionStrList.emplace_back(condition);

        for (auto &orConditionStr : orConditionStrList) {
            std::list<std::string> andConditionStrList;
            while (orConditionStr.find("&") != std::string::npos) {
                andConditionStrList.emplace_back(orConditionStr.substr(0, orConditionStr.find("&")));
                orConditionStr = orConditionStr.substr(orConditionStr.find("&") + 1);
            }
            andConditionStrList.emplace_back(orConditionStr);
            for (const auto &singleCondition : andConditionStrList) {
                ParseCondition(singleCondition);
            }
        }
        return true;
    }
    return false;
}

bool Macro::ParseCondition(const std::string &condition)
{
    std::regex  reg1("([a-z_]+):(.+)(>=|<=|~=|<|>|=)([0-9]+)");
    std::regex  reg2("([a-z_]+):(.+)");
    std::regex  reg3("([a-z_]+)(>=|<=|~=|<|>|=)([0-9]+)");
    std::smatch mat;
    if (regex_search(condition, mat, reg1)) {
        std::string conditionType = mat[1];
        std::string effectName    = mat[2];
        std::string than          = mat[3];
        std::string numStr        = mat[4];
        if (conditionType == "buff") {
            Id_t id                = BUFF_ID_HASH.at(effectName);
            macroFunc.param.int1st = id;
            int num                = stoi(numStr);
            macroFunc.param.int2nd = num;
            if (than == "<") {
                macroFunc.macroFuncPtr = &Macro::BuffStackNumLt;
            } else if (than == "<=") {
                macroFunc.macroFuncPtr = &Macro::BuffStackNumLe;
            } else if (than == "=") {
                macroFunc.macroFuncPtr = &Macro::BuffStackNumEq;
            } else if (than == "~=") {
                macroFunc.macroFuncPtr = &Macro::BuffStackNumNe;
            } else if (than == ">=") {
                macroFunc.macroFuncPtr = &Macro::BuffStackNumGe;
            } else if (than == ">") {
                macroFunc.macroFuncPtr = &Macro::BuffStackNumGt;
            }
        } else if (conditionType == "tbuff") {
            Id_t id                = GetBuffId(effectName);
            macroFunc.param.int1st = id;
            int num                = stoi(numStr);
            macroFunc.param.int2nd = num;
            if (than == "<") {
                macroFunc.macroFuncPtr = &Macro::TBuffStackNumLt;
            } else if (than == "<=") {
                macroFunc.macroFuncPtr = &Macro::TBuffStackNumLe;
            } else if (than == "=") {
                macroFunc.macroFuncPtr = &Macro::TBuffStackNumEq;
            } else if (than == "~=") {
                macroFunc.macroFuncPtr = &Macro::TBuffStackNumNe;
            } else if (than == ">=") {
                macroFunc.macroFuncPtr = &Macro::TBuffStackNumGe;
            } else if (than == ">") {
                macroFunc.macroFuncPtr = &Macro::TBuffStackNumGt;
            }
        } else if (conditionType == "bufftime") {
            Id_t id                = GetBuffId(effectName);
            macroFunc.param.int1st = id;
            double time            = stod(numStr);
            macroFunc.param.int2nd = static_cast<int>(time * FRAMES_PER_SECOND);
            if (than == "<") {
                macroFunc.macroFuncPtr = &Macro::BuffTimeLt;
            } else if (than == "<=") {
                macroFunc.macroFuncPtr = &Macro::BuffTimeLe;
            } else if (than == "=") {
                macroFunc.macroFuncPtr = &Macro::BuffTimeEq;
            } else if (than == "~=") {
                macroFunc.macroFuncPtr = &Macro::BuffTimeNe;
            } else if (than == ">=") {
                macroFunc.macroFuncPtr = &Macro::BuffTimeGe;
            } else if (than == ">") {
                macroFunc.macroFuncPtr = &Macro::BuffTimeGt;
            }
        } else if (conditionType == "tbufftime") {
            Id_t id                = GetBuffId(effectName);
            macroFunc.param.int1st = id;
            double time            = stod(numStr);
            macroFunc.param.int2nd = static_cast<int>(time * FRAMES_PER_SECOND);
            if (than == "<") {
                macroFunc.macroFuncPtr = &Macro::TBuffTimeLt;
            } else if (than == "<=") {
                macroFunc.macroFuncPtr = &Macro::TBuffTimeLe;
            } else if (than == "=") {
                macroFunc.macroFuncPtr = &Macro::TBuffTimeEq;
            } else if (than == "~=") {
                macroFunc.macroFuncPtr = &Macro::TBuffTimeNe;
            } else if (than == ">=") {
                macroFunc.macroFuncPtr = &Macro::TBuffTimeGe;
            } else if (than == ">") {
                macroFunc.macroFuncPtr = &Macro::TBuffTimeGt;
            }
        } else if (conditionType == "cd") {
            Id_t id                = GetSkillId(effectName);
            macroFunc.param.int1st = id;
            double time            = stod(numStr);
            macroFunc.param.int2nd = static_cast<int>(time * FRAMES_PER_SECOND);
            if (than == "<") {
                macroFunc.macroFuncPtr = &Macro::SkillCooldownLt;
            } else if (than == "<=") {
                macroFunc.macroFuncPtr = &Macro::SkillCooldownLe;
            } else if (than == "=") {
                macroFunc.macroFuncPtr = &Macro::SkillCooldownEq;
            } else if (than == "~=") {
                macroFunc.macroFuncPtr = &Macro::SkillCooldownNe;
            } else if (than == ">=") {
                macroFunc.macroFuncPtr = &Macro::SkillCooldownGe;
            } else if (than == ">") {
                macroFunc.macroFuncPtr = &Macro::SkillCooldownGt;
            }
        } else if (conditionType == "skill_energy") {
            Id_t id                = GetSkillId(effectName);
            macroFunc.param.int1st = id;
            int num                = stoi(numStr);
            macroFunc.param.int2nd = num;
            if (than == "<") {
                macroFunc.macroFuncPtr = &Macro::SkillEnergyLt;
            } else if (than == "<=") {
                macroFunc.macroFuncPtr = &Macro::SkillEnergyLe;
            } else if (than == "=") {
                macroFunc.macroFuncPtr = &Macro::SkillEnergyEq;
            } else if (than == "~=") {
                macroFunc.macroFuncPtr = &Macro::SkillEnergyNe;
            } else if (than == ">=") {
                macroFunc.macroFuncPtr = &Macro::SkillEnergyGe;
            } else if (than == ">") {
                macroFunc.macroFuncPtr = &Macro::SkillEnergyGt;
            }
        }
    } else if (regex_search(condition, mat, reg2)) {
        std::string conditionType = mat[1];
        std::string effectName    = mat[2];
        Id_t        id            = GetSkillId(effectName);
        macroFunc.param.int1st    = id;
        if (conditionType == "buff") {
            macroFunc.macroFuncPtr = &Macro::BuffExist;
        } else if (conditionType == "nobuff") {
            macroFunc.macroFuncPtr = &Macro::NoBuffExist;
        } else if (conditionType == "tbuff") {
            macroFunc.macroFuncPtr = &Macro::TBuffExist;
        } else if (conditionType == "tnobuff") {
            macroFunc.macroFuncPtr = &Macro::TNoBuffExist;
        } else if (conditionType == "last_skill") {
            macroFunc.macroFuncPtr = &Macro::LastSkill;
        }
    } else if (regex_search(condition, mat, reg3)) {
        std::string conditionType = mat[1];
        std::string than          = mat[2];
        std::string numStr        = mat[3];
        if (conditionType == "tlife") {
            double percent            = stod(numStr);
            macroFunc.param.double4th = percent;
            if (than == "<") {
                macroFunc.macroFuncPtr = &Macro::TLifeLt;
            } else if (than == "<=") {
                macroFunc.macroFuncPtr = &Macro::TLifeLe;
            } else if (than == "=") {
                macroFunc.macroFuncPtr = &Macro::TLifeEq;
            } else if (than == "~=") {
                macroFunc.macroFuncPtr = &Macro::TLifeNe;
            } else if (than == ">=") {
                macroFunc.macroFuncPtr = &Macro::TLifeGe;
            } else if (than == ">") {
                macroFunc.macroFuncPtr = &Macro::TLifeGt;
            }
        } else if (conditionType == "mana") {
            double percent            = stod(numStr);
            macroFunc.param.double4th = percent;
            if (than == "<") {
                macroFunc.macroFuncPtr = &Macro::ManaLt;
            } else if (than == "<=") {
                macroFunc.macroFuncPtr = &Macro::ManaLe;
            } else if (than == "=") {
                macroFunc.macroFuncPtr = &Macro::ManaEq;
            } else if (than == "~=") {
                macroFunc.macroFuncPtr = &Macro::ManaNe;
            } else if (than == ">=") {
                macroFunc.macroFuncPtr = &Macro::ManaGe;
            } else if (than == ">") {
                macroFunc.macroFuncPtr = &Macro::ManaGt;
            }
        } else if (conditionType == "nearby_enemy") {
            int num                = stoi(numStr);
            macroFunc.param.int1st = num;
            if (than == "<") {
                macroFunc.macroFuncPtr = &Macro::ManaLt;
            } else if (than == "<=") {
                macroFunc.macroFuncPtr = &Macro::ManaLe;
            } else if (than == "=") {
                macroFunc.macroFuncPtr = &Macro::ManaEq;
            } else if (than == "~=") {
                macroFunc.macroFuncPtr = &Macro::ManaNe;
            } else if (than == ">=") {
                macroFunc.macroFuncPtr = &Macro::ManaGe;
            } else if (than == ">") {
                macroFunc.macroFuncPtr = &Macro::ManaGt;
            }
        } else if (conditionType == "qidian") {
            int num                = stoi(numStr);
            macroFunc.param.int1st = num;
            if (than == "<") {
                macroFunc.macroFuncPtr = &Macro::ManaLt;
            } else if (than == "<=") {
                macroFunc.macroFuncPtr = &Macro::ManaLe;
            } else if (than == "=") {
                macroFunc.macroFuncPtr = &Macro::ManaEq;
            } else if (than == "~=") {
                macroFunc.macroFuncPtr = &Macro::ManaNe;
            } else if (than == ">=") {
                macroFunc.macroFuncPtr = &Macro::ManaGe;
            } else if (than == ">") {
                macroFunc.macroFuncPtr = &Macro::ManaGt;
            }
        } else if (conditionType == "rage") {
            int num                = stoi(numStr);
            macroFunc.param.int1st = num;
            if (than == "<") {
                macroFunc.macroFuncPtr = &Macro::ManaLt;
            } else if (than == "<=") {
                macroFunc.macroFuncPtr = &Macro::ManaLe;
            } else if (than == "=") {
                macroFunc.macroFuncPtr = &Macro::ManaEq;
            } else if (than == "~=") {
                macroFunc.macroFuncPtr = &Macro::ManaNe;
            } else if (than == ">=") {
                macroFunc.macroFuncPtr = &Macro::ManaGe;
            } else if (than == ">") {
                macroFunc.macroFuncPtr = &Macro::ManaGt;
            }
        } else if (conditionType == "energy") {
            int num                = stoi(numStr);
            macroFunc.param.int1st = num;
            if (than == "<") {
                macroFunc.macroFuncPtr = &Macro::ManaLt;
            } else if (than == "<=") {
                macroFunc.macroFuncPtr = &Macro::ManaLe;
            } else if (than == "=") {
                macroFunc.macroFuncPtr = &Macro::ManaEq;
            } else if (than == "~=") {
                macroFunc.macroFuncPtr = &Macro::ManaNe;
            } else if (than == ">=") {
                macroFunc.macroFuncPtr = &Macro::ManaGe;
            } else if (than == ">") {
                macroFunc.macroFuncPtr = &Macro::ManaGt;
            }
        } else if (conditionType == "sun") {
            int num                = stoi(numStr);
            macroFunc.param.int1st = num;
            if (than == "<") {
                macroFunc.macroFuncPtr = &Macro::ManaLt;
            } else if (than == "<=") {
                macroFunc.macroFuncPtr = &Macro::ManaLe;
            } else if (than == "=") {
                macroFunc.macroFuncPtr = &Macro::ManaEq;
            } else if (than == "~=") {
                macroFunc.macroFuncPtr = &Macro::ManaNe;
            } else if (than == ">=") {
                macroFunc.macroFuncPtr = &Macro::ManaGe;
            } else if (than == ">") {
                macroFunc.macroFuncPtr = &Macro::ManaGt;
            }
        } else if (conditionType == "moon") {
            int num                = stoi(numStr);
            macroFunc.param.int1st = num;
            if (than == "<") {
                macroFunc.macroFuncPtr = &Macro::ManaLt;
            } else if (than == "<=") {
                macroFunc.macroFuncPtr = &Macro::ManaLe;
            } else if (than == "=") {
                macroFunc.macroFuncPtr = &Macro::ManaEq;
            } else if (than == "~=") {
                macroFunc.macroFuncPtr = &Macro::ManaNe;
            } else if (than == ">=") {
                macroFunc.macroFuncPtr = &Macro::ManaGe;
            } else if (than == ">") {
                macroFunc.macroFuncPtr = &Macro::ManaGt;
            }
        }
    }
}

bool Macro::IsReady(const Param &param, Player &player)
{
    return !player.Skill().at(param.int1st)->GetEnableTime();
}

bool Macro::IsNotCast(const Param &param)
{
    return !m_player->IsCast();
}

bool Macro::IsNotReCast(const Param &param)
{
    return !m_player->IsReCast();
}

bool Macro::BuffStackNumLt(const Param &param)
{
    return m_player->buffs[param.int1st]->GetStackCount() < param.int2nd;
}

bool Macro::BuffStackNumLe(const Param &param)
{
    return m_player->buffs[param.int1st]->GetStackCount() <= param.int2nd;
}

bool Macro::BuffStackNumEq(const Param &param)
{
    return m_player->buffs[param.int1st]->GetStackCount() == param.int2nd;
}

bool Macro::BuffStackNumNe(const Param &param)
{
    return m_player->buffs[param.int1st]->GetStackCount() != param.int2nd;
}

bool Macro::BuffStackNumGe(const Param &param)
{
    return m_player->buffs[param.int1st]->GetStackCount() >= param.int2nd;
}

bool Macro::BuffStackNumGt(const Param &param)
{
    return m_player->buffs[param.int1st]->GetStackCount() > param.int2nd;
}

bool Macro::TBuffStackNumLt(const Param &param)
{
    return (*m_targetsMap)[NORMAL].front()->buffs[param.int1st]->GetStackCount() < param.int2nd;
}

bool Macro::TBuffStackNumLe(const Param &param)
{
    return (*m_targetsMap)[NORMAL].front()->buffs[param.int1st]->GetStackCount() <= param.int2nd;
}

bool Macro::TBuffStackNumEq(const Param &param)
{
    return (*m_targetsMap)[NORMAL].front()->buffs[param.int1st]->GetStackCount() == param.int2nd;
}

bool Macro::TBuffStackNumNe(const Param &param)
{
    return (*m_targetsMap)[NORMAL].front()->buffs[param.int1st]->GetStackCount() != param.int2nd;
}

bool Macro::TBuffStackNumGe(const Param &param)
{
    return (*m_targetsMap)[NORMAL].front()->buffs[param.int1st]->GetStackCount() >= param.int2nd;
}

bool Macro::TBuffStackNumGt(const Param &param)
{
    return (*m_targetsMap)[NORMAL].front()->buffs[param.int1st]->GetStackCount() > param.int2nd;
}

bool Macro::BuffEffectNumLt(const Param &param)
{
    return m_player->buffs[param.int1st]->GetEffectCount() < param.int2nd;
}

bool Macro::BuffEffectNumLe(const Param &param)
{
    return m_player->buffs[param.int1st]->GetEffectCount() <= param.int2nd;
}

bool Macro::BuffEffectNumEq(const Param &param)
{
    return m_player->buffs[param.int1st]->GetEffectCount() == param.int2nd;
}

bool Macro::BuffEffectNumNe(const Param &param)
{
    return m_player->buffs[param.int1st]->GetEffectCount() != param.int2nd;
}

bool Macro::BuffEffectNumGe(const Param &param)
{
    return m_player->buffs[param.int1st]->GetEffectCount() >= param.int2nd;
}

bool Macro::BuffEffectNumGt(const Param &param)
{
    return m_player->buffs[param.int1st]->GetEffectCount() > param.int2nd;
}

bool Macro::TBuffEffectNumLt(const Param &param)
{
    return (*m_targetsMap)[NORMAL].front()->buffs[param.int1st]->GetEffectCount() < param.int2nd;
}

bool Macro::TBuffEffectNumLe(const Param &param)
{
    return (*m_targetsMap)[NORMAL].front()->buffs[param.int1st]->GetEffectCount() <= param.int2nd;
}

bool Macro::TBuffEffectNumEq(const Param &param)
{
    return (*m_targetsMap)[NORMAL].front()->buffs[param.int1st]->GetEffectCount() == param.int2nd;
}

bool Macro::TBuffEffectNumNe(const Param &param)
{
    return (*m_targetsMap)[NORMAL].front()->buffs[param.int1st]->GetEffectCount() != param.int2nd;
}

bool Macro::TBuffEffectNumGe(const Param &param)
{
    return (*m_targetsMap)[NORMAL].front()->buffs[param.int1st]->GetEffectCount() >= param.int2nd;
}

bool Macro::TBuffEffectNumGt(const Param &param)
{
    return (*m_targetsMap)[NORMAL].front()->buffs[param.int1st]->GetEffectCount() > param.int2nd;
}

bool Macro::BuffTimeLt(const Param &param)
{
    return m_player->buffs[param.int1st]->GetRestTime() < param.int2nd;
}

bool Macro::BuffTimeLe(const Param &param)
{
    return m_player->buffs[param.int1st]->GetRestTime() <= param.int2nd;
}

bool Macro::BuffTimeEq(const Param &param)
{
    return m_player->buffs[param.int1st]->GetRestTime() == param.int2nd;
}

bool Macro::BuffTimeNe(const Param &param)
{
    return m_player->buffs[param.int1st]->GetRestTime() != param.int2nd;
}

bool Macro::BuffTimeGe(const Param &param)
{
    return m_player->buffs[param.int1st]->GetRestTime() >= param.int2nd;
}

bool Macro::BuffTimeGt(const Param &param)
{
    return m_player->buffs[param.int1st]->GetRestTime() > param.int2nd;
}

bool Macro::TBuffTimeLt(const Param &param)
{
    return (*m_targetsMap)[NORMAL].front()->buffs[param.int1st]->GetRestTime() < param.int1st;
}

bool Macro::TBuffTimeLe(const Param &param)
{
    return (*m_targetsMap)[NORMAL].front()->buffs[param.int1st]->GetRestTime() <= param.int1st;
}

bool Macro::TBuffTimeEq(const Param &param)
{
    return (*m_targetsMap)[NORMAL].front()->buffs[param.int1st]->GetRestTime() == param.int1st;
}

bool Macro::TBuffTimeNe(const Param &param)
{
    return (*m_targetsMap)[NORMAL].front()->buffs[param.int1st]->GetRestTime() != param.int1st;
}

bool Macro::TBuffTimeGe(const Param &param)
{
    return (*m_targetsMap)[NORMAL].front()->buffs[param.int1st]->GetRestTime() >= param.int1st;
}

bool Macro::TBuffTimeGt(const Param &param)
{
    return (*m_targetsMap)[NORMAL].front()->buffs[param.int1st]->GetRestTime() > param.int1st;
}

bool Macro::SkillCooldownLt(const Param &param)
{
    return m_player->skills[param.int1st]->GetCooldown() < param.int2nd;
}

bool Macro::SkillCooldownLe(const Param &param)
{
    return m_player->skills[param.int1st]->GetCooldown() <= param.int2nd;
}

bool Macro::SkillCooldownEq(const Param &param)
{
    return m_player->skills[param.int1st]->GetCooldown() == param.int2nd;
}

bool Macro::SkillCooldownNe(const Param &param)
{
    return m_player->skills[param.int1st]->GetCooldown() != param.int2nd;
}

bool Macro::SkillCooldownGe(const Param &param)
{
    return m_player->skills[param.int1st]->GetCooldown() >= param.int2nd;
}

bool Macro::SkillCooldownGt(const Param &param)
{
    return m_player->skills[param.int1st]->GetCooldown() > param.int2nd;
}

bool Macro::SkillEnergyLt(const Param &param)
{
    return m_player->skills[param.int1st]->GetEnergyCount() < param.int2nd;
}

bool Macro::SkillEnergyLe(const Param &param)
{
    return m_player->skills[param.int1st]->GetEnergyCount() <= param.int2nd;
}

bool Macro::SkillEnergyEq(const Param &param)
{
    return m_player->skills[param.int1st]->GetEnergyCount() == param.int2nd;
}

bool Macro::SkillEnergyNe(const Param &param)
{
    return m_player->skills[param.int1st]->GetEnergyCount() != param.int2nd;
}

bool Macro::SkillEnergyGe(const Param &param)
{
    return m_player->skills[param.int1st]->GetEnergyCount() >= param.int2nd;
}

bool Macro::SkillEnergyGt(const Param &param)
{
    return m_player->skills[param.int1st]->GetEnergyCount() > param.int2nd;
}

bool Macro::TLifeLt(const Param &param)
{
    return (*m_targetsMap)[NORMAL].front()->GetLifePercent() < param.int1st;
}

bool Macro::TLifeLe(const Param &param)
{
    return (*m_targetsMap)[NORMAL].front()->GetLifePercent() <= param.int1st;
}

bool Macro::TLifeEq(const Param &param)
{
    return (*m_targetsMap)[NORMAL].front()->GetLifePercent() == param.int1st;
}

bool Macro::TLifeNe(const Param &param)
{
    return (*m_targetsMap)[NORMAL].front()->GetLifePercent() != param.int1st;
}

bool Macro::TLifeGe(const Param &param)
{
    return (*m_targetsMap)[NORMAL].front()->GetLifePercent() >= param.int1st;
}

bool Macro::TLifeGt(const Param &param)
{
    return (*m_targetsMap)[NORMAL].front()->GetLifePercent() > param.int1st;
}

bool Macro::ManaLt(const Param &param)
{
    return true;
}

bool Macro::ManaLe(const Param &param)
{
    return true;
}

bool Macro::ManaEq(const Param &param)
{
    return true;
}

bool Macro::ManaNe(const Param &param)
{
    return true;
}

bool Macro::ManaGe(const Param &param)
{
    return true;
}

bool Macro::ManaGt(const Param &param)
{
    return true;
}

bool Macro::NearbyEnemyLt(const Param &param)
{
    return (*m_targetsMap)[NORMAL].size() < static_cast<unsigned>(param.int1st);
}

bool Macro::NearbyEnemyLe(const Param &param)
{
    return (*m_targetsMap)[NORMAL].size() <= static_cast<unsigned>(param.int1st);
}

bool Macro::NearbyEnemyEq(const Param &param)
{
    return (*m_targetsMap)[NORMAL].size() == static_cast<unsigned>(param.int1st);
}

bool Macro::NearbyEnemyNe(const Param &param)
{
    return (*m_targetsMap)[NORMAL].size() != static_cast<unsigned>(param.int1st);
}

bool Macro::NearbyEnemyGe(const Param &param)
{
    return (*m_targetsMap)[NORMAL].size() >= static_cast<unsigned>(param.int1st);
}

bool Macro::NearbyEnemyGt(const Param &param)
{
    return (*m_targetsMap)[NORMAL].size() > static_cast<unsigned>(param.int1st);
}

bool Macro::QidianLt(const Param &param)
{
    return static_cast<TaiXuJianYi::TaiXuJianYi *>(m_player)->GetQidian() < param.int1st;
}

bool Macro::QidianLe(const Param &param)
{
    return static_cast<TaiXuJianYi::TaiXuJianYi *>(m_player)->GetQidian() <= param.int1st;
}

bool Macro::QidianEq(const Param &param)
{
    return static_cast<TaiXuJianYi::TaiXuJianYi *>(m_player)->GetQidian() == param.int1st;
}

bool Macro::QidianNe(const Param &param)
{
    return static_cast<TaiXuJianYi::TaiXuJianYi *>(m_player)->GetQidian() != param.int1st;
}

bool Macro::QidianGe(const Param &param)
{
    return static_cast<TaiXuJianYi::TaiXuJianYi *>(m_player)->GetQidian() >= param.int1st;
}

bool Macro::QidianGt(const Param &param)
{
    return static_cast<TaiXuJianYi::TaiXuJianYi *>(m_player)->GetQidian() > param.int1st;
}

bool Macro::RageLt(const Param &param)
{
    return true;
}

bool Macro::RageLe(const Param &param)
{
    return true;
}

bool Macro::RageEq(const Param &param)
{
    return true;
}

bool Macro::RageNe(const Param &param)
{
    return true;
}

bool Macro::RageGe(const Param &param)
{
    return true;
}

bool Macro::RageGt(const Param &param)
{
    return true;
}

bool Macro::EnergyLt(const Param &param)
{
    return true;
}

bool Macro::EnergyLe(const Param &param)
{
    return true;
}

bool Macro::EnergyEq(const Param &param)
{
    return true;
}

bool Macro::EnergyNe(const Param &param)
{
    return true;
}

bool Macro::EnergyGe(const Param &param)
{
    return true;
}

bool Macro::EnergyGt(const Param &param)
{
    return true;
}

bool Macro::SunLt(const Param &param)
{
    return true;
}

bool Macro::SunLe(const Param &param)
{
    return true;
}

bool Macro::SunEq(const Param &param)
{
    return true;
}

bool Macro::SunNe(const Param &param)
{
    return true;
}

bool Macro::SunGe(const Param &param)
{
    return true;
}

bool Macro::SunGt(const Param &param)
{
    return true;
}

bool Macro::MoonLt(const Param &param)
{
    return true;
}

bool Macro::MoonLe(const Param &param)
{
    return true;
}

bool Macro::MoonEq(const Param &param)
{
    return true;
}

bool Macro::MoonNe(const Param &param)
{
    return true;
}

bool Macro::MoonGe(const Param &param)
{
    return true;
}

bool Macro::MoonGt(const Param &param)
{
    return true;
}

bool Macro::SunPower(const Param &param)
{
    return true;
}

bool Macro::MoonPower(const Param &param)
{
    return true;
}

bool Macro::BuffExist(const Param &param)
{
    return m_player->buffs[param.int1st]->IsExist();
}

bool Macro::NoBuffExist(const Param &param)
{
    return !m_player->buffs[param.int1st]->IsExist();
}

bool Macro::TBuffExist(const Param &param)
{
    return (*m_targetsMap)[NORMAL].front()->buffs[param.int1st]->IsExist();
}

bool Macro::TNoBuffExist(const Param &param)
{
    return !(*m_targetsMap)[NORMAL].front()->buffs[param.int1st]->IsExist();
}

bool Macro::LastSkill(const Param &param)
{
    return m_player->GetLastSkill() == static_cast<Id_t>(param.int1st);
}

} // namespace JX3DPS
