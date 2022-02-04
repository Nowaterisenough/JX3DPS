/**
 * @Description : 宏解析
 * @Author      : NoWats
 * @Date        : 2022-02-02 16:33:58
 * @Update      : NoWats
 * @LastTime    : 2022-02-04 12:09:59
 * @FilePath    : \JX3DPS\modules\Core\Macro.cpp
 */

#pragma execution_character_set("utf-8")

#include "Macro.h"

#include <regex>
#include <iostream>

#include "Common/ConstVal.h"

using namespace std;

namespace JX3DPS {

const std::unordered_map<std::string, Id_t> &BUFF_ID_HASH{
    {"紫气东来", BUF_ZI_QI_DONG_LAI},
    {"玄门", BUF_XUAN_MEN},
    {"碎星辰", 2103},
    {"生太极", 2104},
    {"吞日月", 2105},
    {"云中剑-碎", 2106},
    {"云中剑-生", 2107},
    {"云中剑-吞", 2108},
    {"持盈", 2109},
    {"期声", 2110},
    {"风逝", 2111},
    {"碎星辰buff", 2112},
    {"叠刃", 2201},
    {"人剑合一", 2202},
};

const std::unordered_map<std::string, Id_t> &SKILL_ID_HASH{
    {"无我无剑", 2301},
    {"八荒归元", 2302},
    {"三环套月", 2303},
    {"人剑合一", 2304},
    {"碎星辰", 2305},
    {"生太极", 2306},
    {"吞日月", 2307},
    {"紫气东来", 2308},
    {"三柴剑法", 2309},
};

const std::unordered_map<std::string, Id_t> &TALENT_ID_HASH{
    {"挫锐", 2301},
    {"心固", 2302},
    {"同根", 2303},
    {"深埋", 2304},
    {"昆吾", 2305},
    {"白虹", 2306},
    {"化三清", 2307},
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
    std::regex  reg("([a-z_]+):(.+)(>=|<=|~=|<|>|=)([0-9]+)");
    std::smatch mat;
    if (std::regex_search(condition, mat, reg)) {
        std::string conditionType = mat[1];
        std::string effectName    = mat[2];
        std::string than          = mat[3];
        std::string numStr        = mat[4];
        std::cout << "conditionType:" << conditionType << std::endl;
        std::cout << than << std::endl;
        std::cout << numStr << std::endl;
        if (conditionType == "tbuff") {
            Id_t id = BUFF_ID_HASH.at(effectName);
            // std::cout << id;
        } else if (conditionType == "bufftime") {
            Id_t id = BUFF_ID_HASH.at(effectName);
            // std::cout << id;
        }
        return true;
    }
    return false;
}

bool Macro::IsReady(const Param &param)
{
    return !m_player->skills[param.int1st]->GetEnableTime();
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
