/**
 * Project: JX3DPS
 * File: Json.cpp
 * Description:
 * Created Date: 2023-06-18 19:02:20
 * Author: 难为水
 * -----
 * Last Modified: 2023-07-07 05:16:51
 * Modified By: 难为水
 * -----
 * HISTORY:
 * Date      	By   	Comments
 * ----------	-----	----------------------------------------------------------
 */

#include "Json.h"

#include <fstream>

#include <spdlog/spdlog.h>

#include "Buff.h"
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
        attr.SetPhysicsAttackBase(j.at("PhysicsAttackBase").get<int>());
        attr.SetPhysicsCriticalStrike(j.at("PhysicsCriticalStrike").get<int>());
        attr.SetPhysicsCriticalStrikePower(j.at("PhysicsCriticalStrikePower").get<int>());
        attr.SetPhysicsOvercomeBase(j.at("PhysicsOvercomeBase").get<int>());
        attr.SetMagicAttackBase(j.at("MagicAttackBase").get<int>());
        attr.SetMagicCriticalStrike(j.at("MagicCriticalStrike").get<int>());
        attr.SetMagicCriticalStrikePower(j.at("MagicCriticalStrikePower").get<int>());
        attr.SetMagicOvercomeBase(j.at("MagicOvercomeBase").get<int>());
        attr.SetHaste(j.at("Haste").get<int>());
        attr.SetStrain(j.at("Strain").get<int>());
        attr.SetSurplusBase(j.at("Surplus").get<int>());
        attr.SetWeaponAttack(j.at("WeaponAttack").get<int>());

    } catch (const std::exception &e) {
        spdlog::error("属性解析失败 {}", e.what());
        return JX3DPS_ERROR_INVALID_JSON;
    }
    return JX3DPS_SUCCESS;
}

JX3DPS::Error_t JX3DPS::Stats2Json(const DamageStats &damageStats, nlohmann::json &json)
{

    std::vector<std::string> rollResult = { "命中", "会心" };
    int                      index      = 0;

    for (auto &targetStats : damageStats) {             // TargetStats
        for (auto &effectStats : targetStats.second) {  // EffectStats
            for (auto &subStats : effectStats.second) { // SubStats
                for (auto &levelStats : subStats.second) {
                    for (auto &rollStats : levelStats.second) {
                        std::string  name;
                        JX3DPS::Id_t id = effectStats.first;
                        if ((id / 100) % 10) {
                            name = JX3DPS::Buff::BUFF_NAME.at(id);
                        } else {
                            name = JX3DPS::Skill::SKILL_NAME.at(id);
                        }

                        json[std::string("目标").append(std::to_string(targetStats.first))][name]
                            [std::string("词缀").append(std::to_string(subStats.first))]
                            [std::string("强度").append(std::to_string(levelStats.first))]
                            [rollResult[static_cast<int>(rollStats.first)]]["数目"] =
                                rollStats.second.first;

                        json[std::string("目标").append(std::to_string(targetStats.first))][name]
                            [std::string("词缀").append(std::to_string(subStats.first))]
                            [std::string("强度").append(std::to_string(levelStats.first))]
                            [rollResult[static_cast<int>(rollStats.first)]]["固定伤害"] =
                                rollStats.second.second.fixedDamage;

                        json[std::string("目标").append(std::to_string(targetStats.first))][name]
                            [std::string("词缀").append(std::to_string(subStats.first))]
                            [std::string("强度").append(std::to_string(levelStats.first))]
                            [rollResult[static_cast<int>(rollStats.first)]]["武器伤害"] =
                                rollStats.second.second.weaponDamage;

                        json[std::string("目标").append(std::to_string(targetStats.first))][name]
                            [std::string("词缀").append(std::to_string(subStats.first))]
                            [std::string("强度").append(std::to_string(levelStats.first))]
                            [rollResult[static_cast<int>(rollStats.first)]]["基础攻击伤害"] =
                                rollStats.second.second.attackBaseDamage;

                        json[std::string("目标").append(std::to_string(targetStats.first))][name]
                            [std::string("词缀").append(std::to_string(subStats.first))]
                            [std::string("强度").append(std::to_string(levelStats.first))]
                            [rollResult[static_cast<int>(rollStats.first)]]["面板攻击伤害"] =
                                rollStats.second.second.attackMainDamage;

                        json[std::string("目标").append(std::to_string(targetStats.first))][name]
                            [std::string("词缀").append(std::to_string(subStats.first))]
                            [std::string("强度").append(std::to_string(levelStats.first))]
                            [rollResult[static_cast<int>(rollStats.first)]]["破招伤害"] =
                                rollStats.second.second.surplusDamage;
                    }
                }
            }
        }
    }
    return JX3DPS_SUCCESS;
}

JX3DPS::Error_t JX3DPS::Gains2Json(const std::vector<float> &gains, nlohmann::json &json)
{
    for (int i = 0; i < gains.size(); ++i) {
        json[ATTRIBUTE_NAME.at(static_cast<AttributeType>(i))] = gains[i];
    }
    return JX3DPS_SUCCESS;
}
