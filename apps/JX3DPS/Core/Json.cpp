/**
 * Project: JX3DPS
 * File: Json.cpp
 * Description:
 * Created Date: 2023-06-18 19:02:20
 * Author: 难为水
 * -----
 * Last Modified: 2023-07-04 04:22:30
 * Modified By: 难为水
 * -----
 * HISTORY:
 * Date      	By   	Comments
 * ----------	-----	----------------------------------------------------------
 */

#include "Json.h"

#include <fstream>

#include <spdlog/spdlog.h>

#include "Globals.h"
#include "Skill.h"

JX3DPS::Error_t JX3DPS::LoadConfig(const std::string &path, nlohmann::json &json)
{
    std::ifstream file(path);
    if (!file.is_open()) {
        spdlog::error("配置文件打开失败 {}", path);
        return 1;
    }

    // 从文件中读取JSON数据
    try {
        file >> json;
    } catch (const std::exception &e) {
        spdlog::error("配置文件读取失败 {}", e.what());
        return JX3DPS_ERROR_INVALID_JSON;
    }

    return JX3DPS_SUCCESS;
}

JX3DPS::Error_t JX3DPS::ParseJson2Skills(const nlohmann::json &json, std::list<std::string> &skills)
{
    try {
        json.at("skills").get_to(skills);
    } catch (const std::exception &e) {
        spdlog::error("宏解析失败 {}", e.what());
        return JX3DPS_ERROR_INVALID_JSON;
    }
    return JX3DPS_SUCCESS;
}

JX3DPS::Error_t JX3DPS::ParseJson2Events(const nlohmann::json &json, std::list<std::string> &events)
{
    try {
        json.at("events").get_to(events);
    } catch (const std::exception &e) {
        spdlog::error("事件表达式解析失败 {}", e.what());
        return JX3DPS_ERROR_INVALID_JSON;
    }
    return JX3DPS_SUCCESS;
}

JX3DPS::Error_t JX3DPS::ParseJson2SimIterations(const nlohmann::json &json, int &simIterations)
{
    try {
        json.at("sim_iterations").get_to(simIterations);
    } catch (const std::exception &e) {
        spdlog::error("事件表达式解析失败 {}", e.what());
        return JX3DPS_ERROR_INVALID_JSON;
    }
    return JX3DPS_SUCCESS;
}

JX3DPS::Error_t JX3DPS::ParseJson2Talents(const nlohmann::json &json, Talents &talents)
{
    std::list<int> talentsList;
    try {
        json.at("talents").get_to(talentsList);
    } catch (const std::exception &e) {
        spdlog::error("事件表达式解析失败 {}", e.what());
        return JX3DPS_ERROR_INVALID_JSON;
    }

    for (auto &talent : talentsList) {
        talents[talent] = true;
    }

    return JX3DPS_SUCCESS;
}

JX3DPS::Id_t GetId(const std::string &skillName)
{
    for (auto &skill : JX3DPS::Skill::SKILL_NAME) {
        if (skill.second == skillName) {
            return skill.first;
        }
    }
    return JX3DPS::JX3DPS_ERROR_INVALID_MACRO;
}

JX3DPS::Error_t JX3DPS::ParseJson2Secrets(const nlohmann::json &json, Secrets &secrets)
{
    std::vector<bool> secretsList;
    try {
        for (auto it = json.at("secrets").begin(); it != json.at("secrets").end(); ++it) {
            std::string name = it.key();
            Id_t        id   = GetId(name);

            std::vector<bool> buffer = it.value().get<std::vector<bool>>();

            secrets[id] = buffer;
        }
    } catch (const std::exception &e) {
        spdlog::error("秘籍解析失败 {}", e.what());
        return JX3DPS_ERROR_INVALID_JSON;
    }

    return JX3DPS_SUCCESS;
}

JX3DPS::Error_t JX3DPS::ParseJson2Attr(const nlohmann::json &json, Attr &attr)
{
    try {
        nlohmann::json j = json.at("attr");
        attr.SetAgilityBase(j.at("Agility").get<int>());
        attr.SetSpiritBase(j.at("Spirit").get<int>());
        attr.SetStrengthBase(j.at("Strength").get<int>());
        attr.SetSpunkBase(j.at("Spunk").get<int>());

        attr.SetPhysicsAttackBase(j.at("PhysicsAttackPowerBase").get<int>() - attr.GetPhysicsAttackBaseFromMajor());

        attr.SetBoxPhysicsCriticalStrikePercent(j.at("PhysicsCriticalStrikeRate").get<double>());
        attr.SetBoxPhysicsCriticalStrikePowerPercent(j.at("PhysicsCriticalDamagePowerPercent").get<double>());

        attr.SetPhysicsOvercomeBase(j.at("PhysicsOvercome").get<int>() - attr.GetPhysicsOvercomeBaseFromMajor());
        attr.SetHaste(j.at("Haste").get<int>());
        attr.SetStrain(j.at("Strain").get<int>());
        attr.SetSurplusBase(j.at("SurplusValue").get<int>());
        attr.SetWeaponAttack(j.at("MeleeWeaponDamage").get<int>(),
                             j.at("MeleeWeaponDamage").get<int>() + j.at("MeleeWeaponDamageRand").get<int>());

    } catch (const std::exception &e) {
        spdlog::error("属性解析失败 {}", e.what());
        return JX3DPS_ERROR_INVALID_JSON;
    }
    return JX3DPS_SUCCESS;
}
