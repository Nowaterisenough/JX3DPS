﻿/**
 * Project: JX3DPS
 * File: Json.cpp
 * Description:
 * Created Date: 2023-06-18 19:02:20
 * Author: 难为水
 * -----
 * Last Modified: 2023-09-13 04:19:37
 * Modified By: 难为水
 * -----
 * HISTORY:
 * Date      	By   	Comments
 * ----------	-----	----------------------------------------------------------
 */

#include "JsonParse.h"

#include <spdlog/spdlog.h>

#include "Buff.h"
#include "Regex.h"
#include "Skill.h"

JX3DPS::Error_t JX3DPS::ParseJsonToClassType(const nlohmann::ordered_json &json, ClassType &classType)
{
    try {
        std::string classTypeStr = json.at("ClassType").get<std::string>();
        classType                = GetClassType(classTypeStr);
    } catch (const std::exception &e) {
        spdlog::error("心法解析失败 {}", e.what());
        return JX3DPS_ERROR_INVALID_JSON;
    }
    return JX3DPS_SUCCESS;
}

JX3DPS::Error_t JX3DPS::ParseJsonToOptions(const nlohmann::ordered_json &json, Options &options)
{
    try {
        json.at("Options").at("FramePrecision").get_to(options.framePrecision);
        json.at("Options").at("SimIterations").get_to(options.simIterations);
        json.at("Options").at("DelayMin").get_to(options.delayMin);
        json.at("Options").at("DelayMax").get_to(options.delayMax);
        for (auto &[attributeName, s] : json.at("Options").at("GainSwitch").items()) {
            Attribute::Type type         = Attribute::AttributeType(attributeName);
            std::string     classTypeStr = json.at("ClassType").get<std::string>();
            ClassType       classType    = GetClassType(classTypeStr);
            if (type == Attribute::Type::DEFAULT) {
                if (Attribute::MAJOR[static_cast<int>(classType)][static_cast<int>(Attribute::MajorType::STRENGTH)])
                {
                    type = Attribute::Type::STRENGTH_BASE;
                } else if (Attribute::MAJOR[static_cast<int>(classType)][static_cast<int>(Attribute::MajorType::SPUNK)])
                {
                    type = Attribute::Type::SPUNK_BASE;
                } else if (Attribute::MAJOR[static_cast<int>(classType)][static_cast<int>(Attribute::MajorType::SPIRIT)])
                {
                    type = Attribute::Type::SPIRIT_BASE;
                } else {
                    type = Attribute::Type::AGILITY_BASE;
                }
            }
            options.gainSwitch.emplace(type, s.get<bool>());
        }
        std::string mode = json.at("Options").at("Mode").get<std::string>();
        if (mode == "default") {
            options.mode = Options::Mode::DEFAULT;
        } else if (mode == "debug") {
            options.mode = Options::Mode::DEBUG;
        } else {
            spdlog::error("事件表达式解析失败 {}");
            return JX3DPS_ERROR_INVALID_JSON;
        }
    } catch (const std::exception &e) {
        spdlog::error("事件表达式解析失败 {}", e.what());
        return JX3DPS_ERROR_INVALID_JSON;
    }
    return ParseJsonToTotalFrames(json, options.totalFrames);
}

JX3DPS::Error_t JX3DPS::ParseJsonToSkills(const nlohmann::ordered_json &json,
                                          std::list<std::pair<std::string, std::list<std::string>>> &skills)
{
    try {
        for (auto &[key, value] : json.at("SkillsExpression").items()) {
            std::list<std::string> skillList;
            value.get_to(skillList);
            skills.emplace_back(key, skillList);
        }
    } catch (const std::exception &e) {
        spdlog::error("宏解析失败 {}", e.what());
        return JX3DPS_ERROR_INVALID_JSON;
    }
    return JX3DPS_SUCCESS;
}

JX3DPS::Error_t JX3DPS::ParseJsonToEvents(const nlohmann::ordered_json &json, std::list<std::string> &events)
{
    try {
        json.at("EventsExpression").get_to(events);
    } catch (const std::exception &e) {
        spdlog::error("事件表达式解析失败 {}", e.what());
        return JX3DPS_ERROR_INVALID_JSON;
    }
    return JX3DPS_SUCCESS;
}

JX3DPS::Error_t JX3DPS::ParseJsonToTotalFrames(const nlohmann::ordered_json &json, Frame_t &totalFrames)
{
    std::list<std::string> events;
    try {
        json.at("EventsExpression").get_to(events);
    } catch (const std::exception &e) {
        spdlog::error("事件表达式解析失败 {}", e.what());
        return JX3DPS_ERROR_INVALID_JSON;
    }
    return Regex::ParseToTotalFrames(events, totalFrames);
}

JX3DPS::Error_t JX3DPS::ParseJsonToTalents(const nlohmann::ordered_json &json, Talents &talents)
{
    try {
        if (json.find("Talents") == json.end()) {
            return JX3DPS_SUCCESS;
        }
        for (auto &item : json.at("Talents").items()) {
            std::string talentName        = item.value().get<std::string>();
            talents[TalentId(talentName)] = true;
        }
    } catch (const std::exception &e) {
        spdlog::error("事件表达式解析失败 {}", e.what());
        return JX3DPS_ERROR_INVALID_JSON;
    }
    return JX3DPS_SUCCESS;
}

JX3DPS::Error_t JX3DPS::ParseJsonToRecipes(const nlohmann::ordered_json &json, Recipes &recipes)
{
    try {
        if (json.find("Recipes") == json.end()) {
            return JX3DPS_SUCCESS;
        }
        for (auto &[key, value] : json.at("Recipes").items()) {
            for (auto &secret : value) {
                std::string secretName        = secret.get<std::string>();
                recipes[RecipeId(secretName)] = true;
            }
        }
    } catch (const std::exception &e) {
        spdlog::error("秘籍解析失败 {}", e.what());
        return JX3DPS_ERROR_INVALID_JSON;
    }
    return JX3DPS_SUCCESS;
}

JX3DPS::Error_t JX3DPS::ParseJsonToEquipEffects(const nlohmann::ordered_json &json, EquipEffects &equipEffects)
{
    try {
        if (json.find("EquipEffects") == json.end()) {
            return JX3DPS_SUCCESS;
        }
        for (auto &item : json.at("EquipEffects")) {
            Id_t equipEffectId = EquipEffectId(item.get<std::string>());
            equipEffects.emplace(equipEffectId, true);
        }
    } catch (const std::exception &e) {
        spdlog::error("秘籍解析失败 {}", e.what());
        return JX3DPS_ERROR_INVALID_JSON;
    }
    return JX3DPS_SUCCESS;
}

JX3DPS::Error_t JX3DPS::ParseJsonToAttribute(const nlohmann::ordered_json &json, Attribute &attribute)
{
    try {
        for (auto &[attributeName, value] : json.at("Attribute").items()) {
            Attribute::Type type = Attribute::AttributeType(attributeName);
            if (type <= Attribute::Type::SPUNK_BASE) { // 先初始化主属性
                attribute.SetAttributeInitial(type, value.get<int>());
            }
        }
        for (auto &[attributeName, value] : json.at("Attribute").items()) {
            Attribute::Type type = Attribute::AttributeType(attributeName);
            if (type > Attribute::Type::SPUNK_BASE) {
                attribute.SetAttributeInitial(type, value.get<int>());
            }
        }
    } catch (const std::exception &e) {
        spdlog::error("属性解析失败 {}", e.what());
        return JX3DPS_ERROR_INVALID_JSON;
    }
    return JX3DPS_SUCCESS;
}

JX3DPS::Error_t JX3DPS::ParseJsonToPermanents(const nlohmann::ordered_json &json, Attribute &attribute)
{
    try {
        if (json.find("Permanents") == json.end()) {
            return JX3DPS_SUCCESS;
        }
        for (auto &items : json.at("Permanents")) {
            for (auto &item : items.at("AttributeAdditional")) {
                std::string     name = item.at("Type").get<std::string>();
                Attribute::Type type = Attribute::AttributeType(name);
                attribute.AddAttribute(type, item.at("Value").get<int>());
            }
        }
    } catch (const std::exception &e) {
        spdlog::error("常驻增益解析失败 {}", e.what());
        return JX3DPS_ERROR_INVALID_JSON;
    }
    return JX3DPS_SUCCESS;
}

JX3DPS::Error_t JX3DPS::ParseJsonToTeamCore(const nlohmann::ordered_json &json, Attribute &attribute, ClassType &teamCore)
{
    try {
        if (json.find("TeamCore") == json.end()) {
            return JX3DPS_SUCCESS;
        }
        for (auto &item : json.at("TeamCore").at("AttributeAdditional")) {
            std::string     name = item.at("Type").get<std::string>();
            Attribute::Type type = Attribute::AttributeType(name);
            attribute.AddAttribute(type, item.at("Value").get<int>());
        }
        std::string classTypeStr = json.at("TeamCore").at("Name").get<std::string>();
        teamCore                 = GetClassTeamCoreType(classTypeStr);
    } catch (const std::exception &e) {
        spdlog::error("秘籍解析失败 {}", e.what());
        return JX3DPS_ERROR_INVALID_JSON;
    }
    return JX3DPS_SUCCESS;
}

JX3DPS::Error_t JX3DPS::ParseJsonToBuff3rds(const nlohmann::ordered_json &json, std::unordered_set<Id_t> &buff3rds)
{
    std::list<std::string> events;
    try {
        json.at("EventsExpression").get_to(events);
    } catch (const std::exception &e) {
        spdlog::error("事件表达式解析失败 {}", e.what());
        return JX3DPS_ERROR_INVALID_JSON;
    }
    return Regex::ParseToBuff3rds(events, buff3rds);
}

JX3DPS::Error_t JX3DPS::StatsToJson(const Stats &stats, nlohmann::ordered_json &json)
{
    for (auto &[type, damageStats] : stats.gainStats) {
        nlohmann::ordered_json j;
        DamageStatsToJson(damageStats, j);
        json["Stats"][Attribute::ATTRIBUTE_NAME.at(static_cast<int>(type))] = j;
    }
    for (auto &damage : stats.damageList) {
        json["DamageList"].push_back(damage);
    }
    return JX3DPS_SUCCESS;
}

JX3DPS::Error_t JX3DPS::DamageStatsToJson(const DamageStats &damageStats, nlohmann::ordered_json &json)
{
    constexpr std::array<std::string_view, 2> ROLL_NAME = { "命中", "会心" };

    for (auto &[targetId, targetStats] : damageStats) {
        for (auto &[effectId, effectStats] : targetStats) {
            for (auto &[sub, subStats] : effectStats) {
                for (auto &[level, levelStats] : subStats) {
                    for (auto &[rollResult, item] : levelStats) {

                        auto &[count, damage] = item;

                        std::string_view name = JX3DPS_NAME.at(effectId);
                        std::string targetName = std::string("目标") + std::to_string(targetId - TARGET_PLACE_HOLDERS_DEFAULT);
                        std::string subName = std::string("词缀").append(std::to_string(sub));
                        std::string levelName = std::string("强度").append(std::to_string(level));
                        std::string_view rollName = ROLL_NAME.at(static_cast<int>(rollResult));

                        json[targetName][name][subName][levelName][rollName]["数目"] = count;
                        json[targetName][name][subName][levelName][rollName]["伤害"] =
                            damage.damage;
                        json[targetName][name][subName][levelName][rollName]["破招伤害"] =
                            damage.surplusDamage;
                    }
                }
            }
        }
    }
    return JX3DPS_SUCCESS;
}

JX3DPS::Error_t JX3DPS::TimeLineToJson(const TimeLine::InfosList &infosList, nlohmann::ordered_json &json)
{
    for (auto &[frame, infos] : infosList) {
        nlohmann::ordered_json j;
        j["TimeStamp"] = frame;
        for (auto &info : infos) {
            nlohmann::ordered_json temp;
            auto &name       = info.name;
            auto &rollResult = info.rollResult;
            auto &damage     = info.damage;
            auto &type       = info.type;
            temp["Name"] = name;
            temp["RollResult"] = rollResult;
            temp["Damage"]     = damage;
            temp["Type"]       = type;
            j["Skills"].push_back(temp);
        }
        json.push_back(j);
    }
    return JX3DPS_SUCCESS;
}
