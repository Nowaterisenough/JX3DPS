/**
 * @Description : 宏解析
 * @Author      : NoWats
 * @Date        : 2022-02-02 16:33:58
 * @Update      : NoWats
 * @LastTime    : 2022-02-06 13:43:51
 * @FilePath    : \JX3DPS\modules\Core\Macro.cpp
 */

#pragma execution_character_set("utf-8")

#include "Macro.h"

#include <regex>

#include "Common/ConstVal.h"
#include "Player.h"
#include "Target.h"

using namespace std;

namespace JX3DPS {

const std::unordered_map<std::string, Id_t> &BUFF_ID_HASH{
    {"紫气东来", BUF_ZI_QI_DONG_LAI},
    {"玄门", BUF_XUAN_MEN},
    {"碎星辰", BUF_SUI_XING_CHEN_STD},
    {"生太极", BUF_SHENG_TAI_JI_STD},
    {"吞日月", BUF_TUN_RI_YUE_STD},
    {"云中剑-碎", BUF_YUN_ZHONG_JIAN_C},
    {"云中剑-生", BUF_YUN_ZHONG_JIAN_J},
    {"云中剑-吞", BUF_YUN_ZHONG_JIAN_Y},
    {"持盈", BUF_CHI_YING},
    {"期声", BUF_QI_SHENG},
    {"风逝", BUF_FENG_SHI},
    {"碎星辰buff", BUF_SUI_XING_CHEN_BUFF},
    {"叠刃", BUF_DIE_REN},
    {"人剑合一buff", BUF_REN_JIAN_HE_YI_BUFF},
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

        std::list<std::list<Condition>> conditions;
        for (auto &orConditionStr : orConditionStrList) {
            std::list<std::string> andConditionStrList;
            while (orConditionStr.find("&") != std::string::npos) {
                andConditionStrList.emplace_back(orConditionStr.substr(0, orConditionStr.find("&")));
                orConditionStr = orConditionStr.substr(orConditionStr.find("&") + 1);
            }

            std::list<Condition> conditionsAnd;
            if (castMode == "/cast") {
                conditionsAnd.emplace_back(Condition(&Macro::IsNotReCast, Params()));
            } else if (castMode == "/fcast") {
                conditionsAnd.emplace_back(Condition(&Macro::IsNotCast, Params()));
            }
            andConditionStrList.emplace_back(orConditionStr);
            for (const auto &singleCondition : andConditionStrList) {
                Condition con;
                ParseCondition(singleCondition, con);
                conditionsAnd.emplace_back(con);
            }
            conditions.emplace_back(conditionsAnd);
        }
        std::pair<std::list<std::list<Condition>>, Id_t> macro{conditions, skillId};

        if (m_player->skills[skillId].IsPublicSkill()) {
            m_publicMacros.emplace_back(macro);
        } else {
            m_otherMacros.emplace_back(macro);
        }
        return true;
    }
    return false;
}

bool Macro::ParseCondition(const std::string &str, Condition &condition)
{
    std::regex  reg1("([a-z_]+):(.+)(>=|<=|~=|<|>|=)([0-9]+)");
    std::regex  reg2("([a-z_]+):(.+)");
    std::regex  reg3("([a-z_]+)(>=|<=|~=|<|>|=)([0-9]+)");
    std::smatch mat;
    if (regex_search(str, mat, reg1)) {
        std::string conditionType = mat[1];
        std::string effectName    = mat[2];
        std::string than          = mat[3];
        std::string numStr        = mat[4];
        if (conditionType == "buff") {
            Id_t id = BUFF_ID_HASH.at(effectName);
            condition.params.push_back(id);
            int num = stoi(numStr);
            condition.params.push_back(num);
            if (than == "<") {
                condition.func = &Macro::BuffStackCountLt;
            } else if (than == "<=") {
                condition.func = &Macro::BuffStackCountLe;
            } else if (than == "=") {
                condition.func = &Macro::BuffStackCountEq;
            } else if (than == "~=") {
                condition.func = &Macro::BuffStackCountNe;
            } else if (than == ">=") {
                condition.func = &Macro::BuffStackCountGe;
            } else if (than == ">") {
                condition.func = &Macro::BuffStackCountGt;
            }
        } else if (conditionType == "tbuff") {
            Id_t id = BUFF_ID_HASH.at(effectName);
            condition.params.push_back(id);
            int num = stoi(numStr);
            condition.params.push_back(num);
            if (than == "<") {
                condition.func = &Macro::TBuffStackCountLt;
            } else if (than == "<=") {
                condition.func = &Macro::TBuffStackCountLe;
            } else if (than == "=") {
                condition.func = &Macro::TBuffStackCountEq;
            } else if (than == "~=") {
                condition.func = &Macro::TBuffStackCountNe;
            } else if (than == ">=") {
                condition.func = &Macro::TBuffStackCountGe;
            } else if (than == ">") {
                condition.func = &Macro::TBuffStackCountGt;
            }
        } else if (conditionType == "bufftime") {
            Id_t id = BUFF_ID_HASH.at(effectName);
            condition.params.push_back(id);
            double time = stod(numStr);
            condition.params.push_back(static_cast<int>(time * JX3DPS_FRAMES_PER_SECOND));
            if (than == "<") {
                condition.func = &Macro::BuffTimeLt;
            } else if (than == "<=") {
                condition.func = &Macro::BuffTimeLe;
            } else if (than == "=") {
                condition.func = &Macro::BuffTimeEq;
            } else if (than == "~=") {
                condition.func = &Macro::BuffTimeNe;
            } else if (than == ">=") {
                condition.func = &Macro::BuffTimeGe;
            } else if (than == ">") {
                condition.func = &Macro::BuffTimeGt;
            }
        } else if (conditionType == "tbufftime") {
            Id_t id = BUFF_ID_HASH.at(effectName);
            condition.params.push_back(id);
            double time = stod(numStr);
            condition.params.push_back(static_cast<int>(time * JX3DPS_FRAMES_PER_SECOND));
            if (than == "<") {
                condition.func = &Macro::TBuffTimeLt;
            } else if (than == "<=") {
                condition.func = &Macro::TBuffTimeLe;
            } else if (than == "=") {
                condition.func = &Macro::TBuffTimeEq;
            } else if (than == "~=") {
                condition.func = &Macro::TBuffTimeNe;
            } else if (than == ">=") {
                condition.func = &Macro::TBuffTimeGe;
            } else if (than == ">") {
                condition.func = &Macro::TBuffTimeGt;
            }
        } else if (conditionType == "cd") {
            Id_t id = SKILL_ID_HASH.at(effectName);
            condition.params.push_back(id);
            double time = stod(numStr);
            condition.params.push_back(static_cast<int>(time * JX3DPS_FRAMES_PER_SECOND));
            if (than == "<") {
                condition.func = &Macro::SkillCooldownLt;
            } else if (than == "<=") {
                condition.func = &Macro::SkillCooldownLe;
            } else if (than == "=") {
                condition.func = &Macro::SkillCooldownEq;
            } else if (than == "~=") {
                condition.func = &Macro::SkillCooldownNe;
            } else if (than == ">=") {
                condition.func = &Macro::SkillCooldownGe;
            } else if (than == ">") {
                condition.func = &Macro::SkillCooldownGt;
            }
        } else if (conditionType == "skill_energy") {
            Id_t id = SKILL_ID_HASH.at(effectName);
            condition.params.push_back(id);
            int num = stoi(numStr);
            condition.params.push_back(num);
            if (than == "<") {
                condition.func = &Macro::SkillEnergyLt;
            } else if (than == "<=") {
                condition.func = &Macro::SkillEnergyLe;
            } else if (than == "=") {
                condition.func = &Macro::SkillEnergyEq;
            } else if (than == "~=") {
                condition.func = &Macro::SkillEnergyNe;
            } else if (than == ">=") {
                condition.func = &Macro::SkillEnergyGe;
            } else if (than == ">") {
                condition.func = &Macro::SkillEnergyGt;
            }
        }
    } else if (regex_search(str, mat, reg2)) {
        std::string conditionType = mat[1];
        std::string effectName    = mat[2];
        Id_t        id            = SKILL_ID_HASH.at(effectName);
        condition.params.push_back(id);
        if (conditionType == "buff") {
            condition.func = &Macro::BuffExist;
        } else if (conditionType == "nobuff") {
            condition.func = &Macro::NoBuffExist;
        } else if (conditionType == "tbuff") {
            condition.func = &Macro::TBuffExist;
        } else if (conditionType == "tnobuff") {
            condition.func = &Macro::TNoBuffExist;
        } else if (conditionType == "last_skill") {
            condition.func = &Macro::LastSkill;
        }
    } else if (regex_search(str, mat, reg3)) {
        std::string conditionType = mat[1];
        std::string than          = mat[2];
        std::string numStr        = mat[3];
        if (conditionType == "tlife") {
            double percent = stod(numStr);
            condition.params.push_back(percent);
            if (than == "<") {
                condition.func = &Macro::TLifeLt;
            } else if (than == "<=") {
                condition.func = &Macro::TLifeLe;
            } else if (than == "=") {
                condition.func = &Macro::TLifeEq;
            } else if (than == "~=") {
                condition.func = &Macro::TLifeNe;
            } else if (than == ">=") {
                condition.func = &Macro::TLifeGe;
            } else if (than == ">") {
                condition.func = &Macro::TLifeGt;
            }
        } else if (conditionType == "mana") {
            double percent = stod(numStr);
            condition.params.push_back(percent);
            if (than == "<") {
                condition.func = &Macro::ManaLt;
            } else if (than == "<=") {
                condition.func = &Macro::ManaLe;
            } else if (than == "=") {
                condition.func = &Macro::ManaEq;
            } else if (than == "~=") {
                condition.func = &Macro::ManaNe;
            } else if (than == ">=") {
                condition.func = &Macro::ManaGe;
            } else if (than == ">") {
                condition.func = &Macro::ManaGt;
            }
        } else if (conditionType == "nearby_enemy") {
            int num = stoi(numStr);
            condition.params.push_back(num);
            if (than == "<") {
                condition.func = &Macro::ManaLt;
            } else if (than == "<=") {
                condition.func = &Macro::ManaLe;
            } else if (than == "=") {
                condition.func = &Macro::ManaEq;
            } else if (than == "~=") {
                condition.func = &Macro::ManaNe;
            } else if (than == ">=") {
                condition.func = &Macro::ManaGe;
            } else if (than == ">") {
                condition.func = &Macro::ManaGt;
            }
        } else if (conditionType == "qidian") {
            int num = stoi(numStr);
            condition.params.push_back(num);
            if (than == "<") {
                condition.func = &Macro::ManaLt;
            } else if (than == "<=") {
                condition.func = &Macro::ManaLe;
            } else if (than == "=") {
                condition.func = &Macro::ManaEq;
            } else if (than == "~=") {
                condition.func = &Macro::ManaNe;
            } else if (than == ">=") {
                condition.func = &Macro::ManaGe;
            } else if (than == ">") {
                condition.func = &Macro::ManaGt;
            }
        } else if (conditionType == "rage") {
            int num = stoi(numStr);
            condition.params.push_back(num);
            if (than == "<") {
                condition.func = &Macro::ManaLt;
            } else if (than == "<=") {
                condition.func = &Macro::ManaLe;
            } else if (than == "=") {
                condition.func = &Macro::ManaEq;
            } else if (than == "~=") {
                condition.func = &Macro::ManaNe;
            } else if (than == ">=") {
                condition.func = &Macro::ManaGe;
            } else if (than == ">") {
                condition.func = &Macro::ManaGt;
            }
        } else if (conditionType == "energy") {
            int num = stoi(numStr);
            condition.params.push_back(num);
            if (than == "<") {
                condition.func = &Macro::ManaLt;
            } else if (than == "<=") {
                condition.func = &Macro::ManaLe;
            } else if (than == "=") {
                condition.func = &Macro::ManaEq;
            } else if (than == "~=") {
                condition.func = &Macro::ManaNe;
            } else if (than == ">=") {
                condition.func = &Macro::ManaGe;
            } else if (than == ">") {
                condition.func = &Macro::ManaGt;
            }
        } else if (conditionType == "sun") {
            int num = stoi(numStr);
            condition.params.push_back(num);
            if (than == "<") {
                condition.func = &Macro::ManaLt;
            } else if (than == "<=") {
                condition.func = &Macro::ManaLe;
            } else if (than == "=") {
                condition.func = &Macro::ManaEq;
            } else if (than == "~=") {
                condition.func = &Macro::ManaNe;
            } else if (than == ">=") {
                condition.func = &Macro::ManaGe;
            } else if (than == ">") {
                condition.func = &Macro::ManaGt;
            }
        } else if (conditionType == "moon") {
            int num = stoi(numStr);
            condition.params.push_back(num);
            if (than == "<") {
                condition.func = &Macro::ManaLt;
            } else if (than == "<=") {
                condition.func = &Macro::ManaLe;
            } else if (than == "=") {
                condition.func = &Macro::ManaEq;
            } else if (than == "~=") {
                condition.func = &Macro::ManaNe;
            } else if (than == ">=") {
                condition.func = &Macro::ManaGe;
            } else if (than == ">") {
                condition.func = &Macro::ManaGt;
            }
        }
    }
}

bool Macro::IsReady(const Params &params)
{
    Id_t skillId = params.at(0).paramInt;
    return !m_player->skills[skillId]->GetEnableTime();
}

bool Macro::IsNotCast(const Params &params)
{
    return !m_player->IsCast();
}

bool Macro::IsNotReCast(const Params &params)
{
    return !m_player->IsReCast();
}

bool Macro::BuffStackCountLt(const Params &params)
{
    Id_t buffId     = params.at(0).paramInt;
    int  stackCount = params.at(1).paramInt;
    return m_player->buffs[buffId]->GetStackCount() < stackCount;
}

bool Macro::BuffStackCountLe(const Params &params)
{
    Id_t buffId     = params.at(0).paramInt;
    int  stackCount = params.at(1).paramInt;
    return m_player->buffs[buffId]->GetStackCount() <= stackCount;
}

bool Macro::BuffStackCountEq(const Params &params)
{
    Id_t buffId     = params.at(0).paramInt;
    int  stackCount = params.at(1).paramInt;
    return m_player->buffs[buffId]->GetStackCount() == stackCount;
}

bool Macro::BuffStackCountNe(const Params &params)
{
    Id_t buffId     = params.at(0).paramInt;
    int  stackCount = params.at(1).paramInt;
    return m_player->buffs[buffId]->GetStackCount() != stackCount;
}

bool Macro::BuffStackCountGe(const Params &params)
{
    Id_t buffId     = params.at(0).paramInt;
    int  stackCount = params.at(1).paramInt;
    return m_player->buffs[buffId]->GetStackCount() >= stackCount;
}

bool Macro::BuffStackCountGt(const Params &params)
{
    Id_t buffId     = params.at(0).paramInt;
    int  stackCount = params.at(1).paramInt;
    return m_player->buffs[buffId]->GetStackCount() > stackCount;
}

bool Macro::TBuffStackCountLt(const Params &params)
{
    Id_t buffId     = params.at(0).paramInt;
    int  stackCount = params.at(1).paramInt;
    return m_targets->at(NORMAL).front()->buffs[buffId]->GetStackCount() < stackCount;
}

bool Macro::TBuffStackCountLe(const Params &params)
{
    Id_t buffId     = params.at(0).paramInt;
    int  stackCount = params.at(1).paramInt;
    return m_targets->at(NORMAL).front()->buffs[buffId]->GetStackCount() <= stackCount;
}

bool Macro::TBuffStackCountEq(const Params &params)
{
    Id_t buffId     = params.at(0).paramInt;
    int  stackCount = params.at(1).paramInt;
    return m_targets->at(NORMAL).front()->buffs[buffId]->GetStackCount() == stackCount;
}

bool Macro::TBuffStackCountNe(const Params &params)
{
    Id_t buffId     = params.at(0).paramInt;
    int  stackCount = params.at(1).paramInt;
    return m_targets->at(NORMAL).front()->buffs[buffId]->GetStackCount() != stackCount;
}

bool Macro::TBuffStackCountGe(const Params &params)
{
    Id_t buffId     = params.at(0).paramInt;
    int  stackCount = params.at(1).paramInt;
    return m_targets->at(NORMAL).front()->buffs[buffId]->GetStackCount() >= stackCount;
}

bool Macro::TBuffStackCountGt(const Params &params)
{
    Id_t buffId     = params.at(0).paramInt;
    int  stackCount = params.at(1).paramInt;
    return m_targets->at(NORMAL).front()->buffs[buffId]->GetStackCount() > stackCount;
}

bool Macro::BuffEffectCountLt(const Params &params)
{
    Id_t buffId      = params.at(0).paramInt;
    int  effectCount = params.at(1).paramInt;
    return m_player->buffs[buffId]->GetEffectCount() < effectCount;
}

bool Macro::BuffEffectCountLe(const Params &params)
{
    Id_t buffId      = params.at(0).paramInt;
    int  effectCount = params.at(1).paramInt;
    return m_player->buffs[buffId]->GetEffectCount() <= effectCount;
}

bool Macro::BuffEffectCountEq(const Params &params)
{
    Id_t buffId      = params.at(0).paramInt;
    int  effectCount = params.at(1).paramInt;
    return m_player->buffs[buffId]->GetEffectCount() == effectCount;
}

bool Macro::BuffEffectCountNe(const Params &params)
{
    Id_t buffId      = params.at(0).paramInt;
    int  effectCount = params.at(1).paramInt;
    return m_player->buffs[buffId]->GetEffectCount() != effectCount;
}

bool Macro::BuffEffectCountGe(const Params &params)
{
    Id_t buffId      = params.at(0).paramInt;
    int  effectCount = params.at(1).paramInt;
    return m_player->buffs[buffId]->GetEffectCount() >= effectCount;
}

bool Macro::BuffEffectCountGt(const Params &params)
{
    Id_t buffId      = params.at(0).paramInt;
    int  effectCount = params.at(1).paramInt;
    return m_player->buffs[buffId]->GetEffectCount() > effectCount;
}

bool Macro::TBuffEffectCountLt(const Params &params)
{
    Id_t buffId      = params.at(0).paramInt;
    int  effectCount = params.at(1).paramInt;
    return m_targets->at(NORMAL).front()->buffs[buffId]->GetEffectCount() < effectCount;
}

bool Macro::TBuffEffectCountLe(const Params &params)
{
    Id_t buffId      = params.at(0).paramInt;
    int  effectCount = params.at(1).paramInt;
    return m_targets->at(NORMAL).front()->buffs[buffId]->GetEffectCount() <= effectCount;
}

bool Macro::TBuffEffectCountEq(const Params &params)
{
    Id_t buffId      = params.at(0).paramInt;
    int  effectCount = params.at(1).paramInt;
    return m_targets->at(NORMAL).front()->buffs[buffId]->GetEffectCount() == effectCount;
}

bool Macro::TBuffEffectCountNe(const Params &params)
{
    Id_t buffId      = params.at(0).paramInt;
    int  effectCount = params.at(1).paramInt;
    return m_targets->at(NORMAL).front()->buffs[buffId]->GetEffectCount() != effectCount;
}

bool Macro::TBuffEffectCountGe(const Params &params)
{
    Id_t buffId      = params.at(0).paramInt;
    int  effectCount = params.at(1).paramInt;
    return m_targets->at(NORMAL).front()->buffs[buffId]->GetEffectCount() >= effectCount;
}

bool Macro::TBuffEffectCountGt(const Params &params)
{
    Id_t buffId      = params.at(0).paramInt;
    int  effectCount = params.at(1).paramInt;
    return m_targets->at(NORMAL).front()->buffs[buffId]->GetEffectCount() > effectCount;
}

bool Macro::BuffTimeLt(const Params &params)
{
    Id_t    buffId = params.at(0).paramInt;
    Frame_t frames = params.at(1).paramInt;
    return m_player->buffs[buffId]->GetRestTime() < frames;
}

bool Macro::BuffTimeLe(const Params &params)
{
    Id_t    buffId = params.at(0).paramInt;
    Frame_t frames = params.at(1).paramInt;
    return m_player->buffs[buffId]->GetRestTime() <= frames;
}

bool Macro::BuffTimeEq(const Params &params)
{
    Id_t    buffId = params.at(0).paramInt;
    Frame_t frames = params.at(1).paramInt;
    return m_player->buffs[buffId]->GetRestTime() == frames;
}

bool Macro::BuffTimeNe(const Params &params)
{
    Id_t    buffId = params.at(0).paramInt;
    Frame_t frames = params.at(1).paramInt;
    return m_player->buffs[buffId]->GetRestTime() != frames;
}

bool Macro::BuffTimeGe(const Params &params)
{
    Id_t    buffId = params.at(0).paramInt;
    Frame_t frames = params.at(1).paramInt;
    return m_player->buffs[buffId]->GetRestTime() >= frames;
}

bool Macro::BuffTimeGt(const Params &params)
{
    Id_t    buffId = params.at(0).paramInt;
    Frame_t frames = params.at(1).paramInt;
    return m_player->buffs[buffId]->GetRestTime() > frames;
}

bool Macro::TBuffTimeLt(const Params &params)
{
    Id_t    buffId = params.at(0).paramInt;
    Frame_t frames = params.at(1).paramInt;
    return m_targets->at(NORMAL).front()->buffs[buffId]->GetRestTime() < frames;
}

bool Macro::TBuffTimeLe(const Params &params)
{
    Id_t    buffId = params.at(0).paramInt;
    Frame_t frames = params.at(1).paramInt;
    return m_targets->at(NORMAL).front()->buffs[buffId]->GetRestTime() <= frames;
}

bool Macro::TBuffTimeEq(const Params &params)
{
    Id_t    buffId = params.at(0).paramInt;
    Frame_t frames = params.at(1).paramInt;
    return m_targets->at(NORMAL).front()->buffs[buffId]->GetRestTime() == frames;
}

bool Macro::TBuffTimeNe(const Params &params)
{
    Id_t    buffId = params.at(0).paramInt;
    Frame_t frames = params.at(1).paramInt;
    return m_targets->at(NORMAL).front()->buffs[buffId]->GetRestTime() != frames;
}

bool Macro::TBuffTimeGe(const Params &params)
{
    Id_t    buffId = params.at(0).paramInt;
    Frame_t frames = params.at(1).paramInt;
    return m_targets->at(NORMAL).front()->buffs[buffId]->GetRestTime() >= frames;
}

bool Macro::TBuffTimeGt(const Params &params)
{
    Id_t    buffId = params.at(0).paramInt;
    Frame_t frames = params.at(1).paramInt;
    return m_targets->at(NORMAL).front()->buffs[buffId]->GetRestTime() > frames;
}

bool Macro::SkillCooldownLt(const Params &params)
{
    Id_t    skillId = params.at(0).paramInt;
    Frame_t frames  = params.at(1).paramInt;
    return m_player->skills[skillId]->GetCooldown() < frames;
}

bool Macro::SkillCooldownLe(const Params &params)
{
    Id_t    skillId = params.at(0).paramInt;
    Frame_t frames  = params.at(1).paramInt;
    return m_player->skills[skillId]->GetCooldown() <= frames;
}

bool Macro::SkillCooldownEq(const Params &params)
{
    Id_t    skillId = params.at(0).paramInt;
    Frame_t frames  = params.at(1).paramInt;
    return m_player->skills[skillId]->GetCooldown() == frames;
}

bool Macro::SkillCooldownNe(const Params &params)
{
    Id_t    skillId = params.at(0).paramInt;
    Frame_t frames  = params.at(1).paramInt;
    return m_player->skills[skillId]->GetCooldown() != frames;
}

bool Macro::SkillCooldownGe(const Params &params)
{
    Id_t    skillId = params.at(0).paramInt;
    Frame_t frames  = params.at(1).paramInt;
    return m_player->skills[skillId]->GetCooldown() >= frames;
}

bool Macro::SkillCooldownGt(const Params &params)
{
    Id_t    skillId = params.at(0).paramInt;
    Frame_t frames  = params.at(1).paramInt;
    return m_player->skills[skillId]->GetCooldown() > frames;
}

bool Macro::SkillEnergyLt(const Params &params)
{
    Id_t skillId     = params.at(0).paramInt;
    int  energyCount = params.at(1).paramInt;
    return m_player->skills[skillId]->GetEnergyCount() < energyCount;
}

bool Macro::SkillEnergyLe(const Params &params)
{
    Id_t skillId     = params.at(0).paramInt;
    int  energyCount = params.at(1).paramInt;
    return m_player->skills[skillId]->GetEnergyCount() <= energyCount;
}

bool Macro::SkillEnergyEq(const Params &params)
{
    Id_t skillId     = params.at(0).paramInt;
    int  energyCount = params.at(1).paramInt;
    return m_player->skills[skillId]->GetEnergyCount() == energyCount;
}

bool Macro::SkillEnergyNe(const Params &params)
{
    Id_t skillId     = params.at(0).paramInt;
    int  energyCount = params.at(1).paramInt;
    return m_player->skills[skillId]->GetEnergyCount() != energyCount;
}

bool Macro::SkillEnergyGe(const Params &params)
{
    Id_t skillId     = params.at(0).paramInt;
    int  energyCount = params.at(1).paramInt;
    return m_player->skills[skillId]->GetEnergyCount() >= energyCount;
}

bool Macro::SkillEnergyGt(const Params &params)
{
    Id_t skillId     = params.at(0).paramInt;
    int  energyCount = params.at(1).paramInt;
    return m_player->skills[skillId]->GetEnergyCount() > energyCount;
}

bool Macro::TLifeLt(const Params &params)
{
    float lifePercent = params.at(0).paramFloat;
    return m_targets->at(NORMAL).front()->GetLifePercent() < lifePercent;
}

bool Macro::TLifeLe(const Params &params)
{
    float lifePercent = params.at(0).paramFloat;
    return m_targets->at(NORMAL).front()->GetLifePercent() <= lifePercent;
}

bool Macro::TLifeEq(const Params &params)
{
    float lifePercent = params.at(0).paramFloat;
    return m_targets->at(NORMAL).front()->GetLifePercent() == lifePercent;
}

bool Macro::TLifeNe(const Params &params)
{
    float lifePercent = params.at(0).paramFloat;
    return m_targets->at(NORMAL).front()->GetLifePercent() != lifePercent;
}

bool Macro::TLifeGe(const Params &params)
{
    float lifePercent = params.at(0).paramFloat;
    return m_targets->at(NORMAL).front()->GetLifePercent() >= lifePercent;
}

bool Macro::TLifeGt(const Params &params)
{
    float lifePercent = params.at(0).paramFloat;
    return m_targets->at(NORMAL).front()->GetLifePercent() > lifePercent;
}

bool Macro::ManaLt(const Params &params)
{
    return true;
}

bool Macro::ManaLe(const Params &params)
{
    return true;
}

bool Macro::ManaEq(const Params &params)
{
    return true;
}

bool Macro::ManaNe(const Params &params)
{
    return true;
}

bool Macro::ManaGe(const Params &params)
{
    return true;
}

bool Macro::ManaGt(const Params &params)
{
    return true;
}

bool Macro::NearbyEnemyLt(const Params &params)
{
    int count = params.at(0).paramInt;
    return m_targets->at(NORMAL).size() < static_cast<unsigned>(count);
}

bool Macro::NearbyEnemyLe(const Params &params)
{
    int count = params.at(0).paramInt;
    return m_targets->at(NORMAL).size() <= static_cast<unsigned>(count);
}

bool Macro::NearbyEnemyEq(const Params &params)
{
    int count = params.at(0).paramInt;
    return m_targets->at(NORMAL).size() == static_cast<unsigned>(count);
}

bool Macro::NearbyEnemyNe(const Params &params)
{
    int count = params.at(0).paramInt;
    return m_targets->at(NORMAL).size() != static_cast<unsigned>(count);
}

bool Macro::NearbyEnemyGe(const Params &params)
{
    int count = params.at(0).paramInt;
    return m_targets->at(NORMAL).size() >= static_cast<unsigned>(count);
}

bool Macro::NearbyEnemyGt(const Params &params)
{
    int count = params.at(0).paramInt;
    return m_targets->at(NORMAL).size() > static_cast<unsigned>(count);
}

bool Macro::QidianLt(const Params &params)
{
    int count = params.at(0).paramInt;
    return m_player->GetQidian() < count;
}

bool Macro::QidianLe(const Params &params)
{
    int count = params.at(0).paramInt;
    return m_player->GetQidian() <= count;
}

bool Macro::QidianEq(const Params &params)
{
    int count = params.at(0).paramInt;
    return m_player->GetQidian() == count;
}

bool Macro::QidianNe(const Params &params)
{
    int count = params.at(0).paramInt;
    return m_player->GetQidian() != count;
}

bool Macro::QidianGe(const Params &params)
{
    int count = params.at(0).paramInt;
    return m_player->GetQidian() >= count;
}

bool Macro::QidianGt(const Params &params)
{
    int count = params.at(0).paramInt;
    return m_player->GetQidian() > count;
}

bool Macro::RageLt(const Params &params)
{
    return true;
}

bool Macro::RageLe(const Params &params)
{
    return true;
}

bool Macro::RageEq(const Params &params)
{
    return true;
}

bool Macro::RageNe(const Params &params)
{
    return true;
}

bool Macro::RageGe(const Params &params)
{
    return true;
}

bool Macro::RageGt(const Params &params)
{
    return true;
}

bool Macro::EnergyLt(const Params &params)
{
    return true;
}

bool Macro::EnergyLe(const Params &params)
{
    return true;
}

bool Macro::EnergyEq(const Params &params)
{
    return true;
}

bool Macro::EnergyNe(const Params &params)
{
    return true;
}

bool Macro::EnergyGe(const Params &params)
{
    return true;
}

bool Macro::EnergyGt(const Params &params)
{
    return true;
}

bool Macro::SunLt(const Params &params)
{
    return true;
}

bool Macro::SunLe(const Params &params)
{
    return true;
}

bool Macro::SunEq(const Params &params)
{
    return true;
}

bool Macro::SunNe(const Params &params)
{
    return true;
}

bool Macro::SunGe(const Params &params)
{
    return true;
}

bool Macro::SunGt(const Params &params)
{
    return true;
}

bool Macro::MoonLt(const Params &params)
{
    return true;
}

bool Macro::MoonLe(const Params &params)
{
    return true;
}

bool Macro::MoonEq(const Params &params)
{
    return true;
}

bool Macro::MoonNe(const Params &params)
{
    return true;
}

bool Macro::MoonGe(const Params &params)
{
    return true;
}

bool Macro::MoonGt(const Params &params)
{
    return true;
}

bool Macro::SunPower(const Params &params)
{
    return true;
}

bool Macro::MoonPower(const Params &params)
{
    return true;
}

bool Macro::BuffExist(const Params &params)
{
    Id_t buffId = params.at(0).paramInt;
    return m_player->buffs[buffId]->IsExist();
}

bool Macro::NoBuffExist(const Params &params)
{
    Id_t buffId = params.at(0).paramInt;
    return !m_player->buffs[buffId]->IsExist();
}

bool Macro::TBuffExist(const Params &params)
{
    Id_t buffId = params.at(0).paramInt;
    return m_targets->at(NORMAL).front()->buffs[buffId]->IsExist();
}

bool Macro::TNoBuffExist(const Params &params)
{
    Id_t buffId = params.at(0).paramInt;
    return !m_targets->at(NORMAL).front()->buffs[buffId]->IsExist();
}

bool Macro::LastSkill(const Params &params)
{
    Id_t skillId = params.at(0).paramInt;
    return m_player->GetLastSkill() == skillId;
}

} // namespace JX3DPS
