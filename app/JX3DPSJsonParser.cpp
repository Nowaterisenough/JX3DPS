/**
 * Project: JX3DPS
 * File: JX3DPSJsonParser.cpp
 * Description:
 * Created Date: 2023-08-10 00:05:57
 * Author: 难为水
 * -----
 * Last Modified: 2023-08-14 04:48:06
 * Modified By: 难为水
 * -----
 * HISTORY:
 * Date      	By   	Comments
 * ----------	-----	----------------------------------------------------------
 */

#include "JX3DPSJsonParser.h"

#include <fstream>

QString JX3IconPath(int id)
{
    return QString(":/resources/pics/JX3/Icons/%1.png").arg(id);
}

void JX3DPS::Simulator::JsonParser::LoadConfig(const std::string &config, nlohmann::ordered_json &json)
{
    std::ifstream ifs(config);
    if (!ifs.is_open()) {
        throw std::runtime_error("Failed to open config file.");
    }
    ifs >> json;
    ifs.close();
}

void JX3DPS::Simulator::JsonParser::ParseJsonToTalentItemInfos(
    const nlohmann::ordered_json               &json,
    JX3DPS::ClassType                           classType,
    std::vector<std::list<ComboBox::ItemInfo>> &talents,
    std::list<std::string>                     &defaults)
{
    for (auto &item : json["ClassType"]) {
        if (item["Name"].get<std::string>().c_str() == JX3DPS::CLASS_NAME[static_cast<int>(classType)])
        {
            for (auto &talent : item["Talents"].items()) {
                std::list<ComboBox::ItemInfo> talentItemInfos;
                for (auto &itemInfo : talent.value()) {
                    ComboBox::ItemInfo itemInfo_;
                    itemInfo_.name     = itemInfo["Name"].get<std::string>().c_str();
                    itemInfo_.iconPath = JX3IconPath(itemInfo["Icon"].get<int>());
                    itemInfo_.description = itemInfo["Description"].get<std::string>().c_str();
                    talentItemInfos.push_back(itemInfo_);
                }
                talents.push_back(talentItemInfos);
            }

            for (auto &item : item["Default"].items()) {
                if (item.key() == "Talents") {
                    for (auto &iter : item.value()) {
                        defaults.push_back(iter.get<std::string>());
                    }
                }
            }
        }
    }
}

void JX3DPS::Simulator::JsonParser::ParseJsonToRecipeItemInfos(
    const nlohmann::ordered_json                                   &json,
    JX3DPS::ClassType                                               classType,
    std::unordered_map<std::string, std::list<CheckBox::ItemInfo>> &recipes,
    std::list<std::string>                                         &defaults)
{
    for (auto &item : json["ClassType"]) {
        if (item["Name"].get<std::string>().c_str() == JX3DPS::CLASS_NAME[static_cast<int>(classType)])
        {
            for (auto &recipe : item["Recipes"].items()) {
                std::list<CheckBox::ItemInfo> recipeItemInfos;
                for (auto &itemInfo : recipe.value()) {
                    CheckBox::ItemInfo itemInfo_;
                    itemInfo_.name     = itemInfo["Name"].get<std::string>().c_str();
                    itemInfo_.iconPath = JX3IconPath(itemInfo["Icon"].get<int>());
                    itemInfo_.description = itemInfo["Description"].get<std::string>().c_str();
                    recipeItemInfos.push_back(itemInfo_);
                }
                recipes.emplace(recipe.key(), recipeItemInfos);
            }

            for (auto &item : item["Default"].items()) {
                if (item.key() == "Recipes") {
                    for (auto &it : item.value().items()) {
                        for (auto &iter : it.value()) {
                            defaults.push_back(iter.get<std::string>());
                        }
                    }
                }
            }
        }
    }
}

void JX3DPS::Simulator::JsonParser::ParseJsonToSkills(
    const nlohmann::ordered_json                              &json,
    JX3DPS::ClassType                                          classType,
    std::list<std::pair<std::string, std::list<std::string>>> &skills)
{
    for (auto &item : json["ClassType"]) {
        if (item["Name"].get<std::string>().c_str() == JX3DPS::CLASS_NAME[static_cast<int>(classType)])
        {
            for (auto &item : item["Default"].items()) {
                if (item.key() == "SkillsExpression") {
                    for (auto &[skillName, exprs] : item.value().items()) {
                        std::list<std::string> exprs_ = exprs.get<std::list<std::string>>();
                        skills.emplace_back(skillName, exprs_);
                    }
                }
            }
        }
    }
}

void JX3DPS::Simulator::JsonParser::ParseJsonToEvents(const nlohmann::ordered_json &json,
                                                      JX3DPS::ClassType       classType,
                                                      std::list<std::string> &events)
{
    for (auto &item : json["ClassType"]) {
        if (item["Name"].get<std::string>().c_str() == JX3DPS::CLASS_NAME[static_cast<int>(classType)])
        {
            for (auto &item : item["Default"].items()) {
                if (item.key() == "EventsExpression") {
                    events = item.value().get<std::list<std::string>>();
                }
            }
        }
    }
}

void JX3DPS::Simulator::JsonParser::ParseJsonToPermanents(
    const nlohmann::ordered_json                                   &json,
    JX3DPS::ClassType                                               classType,
    std::unordered_map<std::string, CheckBox::ItemInfo>            &permanents1,
    std::unordered_map<std::string, std::list<ComboBox::ItemInfo>> &permanents2)
{
    for (auto &item : json["ClassType"]) {
        if (item["Name"].get<std::string>().c_str() == JX3DPS::CLASS_NAME[static_cast<int>(classType)])
        {
            for (auto &[key, value] : item["Permanents"].items()) {
                if (key == "Others") {
                    for (auto &permanent : value) {
                        CheckBox::ItemInfo itemInfo;
                        itemInfo.name     = permanent["Name"].get<std::string>().c_str();
                        itemInfo.iconPath = JX3IconPath(permanent["Icon"].get<int>());
                        itemInfo.description = permanent["Description"].get<std::string>().c_str();
                        permanents1.emplace(itemInfo.name.toStdString(), itemInfo);
                    }
                } else {
                    std::list<ComboBox::ItemInfo> itemInfos;
                    for (auto &permanent : value) {
                        ComboBox::ItemInfo itemInfo;
                        itemInfo.name     = permanent["Name"].get<std::string>().c_str();
                        itemInfo.iconPath = JX3IconPath(permanent["Icon"].get<int>());
                        itemInfo.description = permanent["Description"].get<std::string>().c_str();
                        itemInfos.emplace_back(itemInfo);
                    }
                    permanents2.emplace(key, itemInfos);
                }
            }
        }
    }
}

void JX3DPS::Simulator::JsonParser::LoadJsonPermanent(const nlohmann::ordered_json &json,
                                                      JX3DPS::ClassType       classType,
                                                      const std::string      &name,
                                                      nlohmann::ordered_json &out)
{
    for (auto &item : json["ClassType"]) {
        if (item["Name"].get<std::string>().c_str() == JX3DPS::CLASS_NAME[static_cast<int>(classType)])
        {
            for (auto &[key, value] : item["Permanents"].items()) {
                for (auto &permanent : value) {
                    if (name == permanent["Name"].get<std::string>().c_str()) {
                        out = permanent;
                        return;
                    }
                }
            }
        }
    }
}

void JX3DPS::Simulator::JsonParser::ParseJsonToClassTypeItemInfos(const nlohmann::ordered_json &json,
                                                                  std::list<ComboBox::ItemInfo> &itemInfos)
{
    for (auto &item : json["ClassType"]) {
        ComboBox::ItemInfo itemInfo;
        itemInfo.name        = item["Name"].get<std::string>().c_str();
        itemInfo.iconPath    = JX3IconPath(item["Icon"].get<int>());
        itemInfo.description = item["Description"].get<std::string>().c_str();
        itemInfos.push_back(itemInfo);
    }
}

void JX3DPS::Simulator::JsonParser::ParseJsonToDefaultAttribute(
    const nlohmann::ordered_json         &json,
    JX3DPS::ClassType                     classType,
    std::unordered_map<std::string, int> &attributes)
{
    for (auto &item : json["ClassType"]) {
        if (item["Name"].get<std::string>().c_str() == JX3DPS::CLASS_NAME[static_cast<int>(classType)])
        {
            for (auto &[key, value] : item["Default"].items()) {
                if (key == "Attribute") {
                    for (auto &[key, value] : value.items()) {
                        attributes.emplace(key, value.get<int>());
                    }
                }
            }
        }
    }
}

long long JX3DPS::Simulator::JsonParser::GetTotalDamage(const nlohmann::ordered_json &json)
{
    long long sum = 0;
    for (auto &[targetName, targetDamage] : json.items()) {
        for (auto &[effectName, effectDamage] : targetDamage.items()) {
            for (auto &[subName, subDamage] : effectDamage.items()) {
                for (auto &[levelName, levelDamage] : subDamage.items()) {
                    for (auto &[rollResult, info] : levelDamage.items()) {
                        sum += info["伤害"].get<long long>();
                        sum += info["破招伤害"].get<long long>();
                    }
                }
            }
        }
    }
    return sum;
}

std::list<std::tuple<std::string, long long>> JX3DPS::Simulator::JsonParser::GetTargetDamages(
    const nlohmann::ordered_json &json)
{
    std::list<std::tuple<std::string, long long>> targetDamages;
    long long                                     total = 0;
    targetDamages.emplace_back("总计", total);
    for (auto &[targetName, targetDamage] : json.items()) {
        long long sum = 0;
        for (auto &[effectName, effectDamage] : targetDamage.items()) {
            for (auto &[subName, subDamage] : effectDamage.items()) {
                for (auto &[levelName, levelDamage] : subDamage.items()) {
                    for (auto &[rollResult, info] : levelDamage.items()) {
                        sum += info["伤害"].get<long long>();
                        sum += info["破招伤害"].get<long long>();
                    }
                }
            }
        }
        total += sum;
        targetDamages.emplace_back(targetName, sum);
    }
    std::get<1>(targetDamages.front()) += total;
    return targetDamages;
}

std::unordered_map<std::string, std::tuple<int, long long>> JX3DPS::Simulator::JsonParser::GetTargetEffectDamages(
    const nlohmann::ordered_json &json,
    const std::string            &target)
{
    std::unordered_map<std::string, std::tuple<int, long long>> targetEffectDamages;
    for (auto &[targetName, targetDamage] : json.items()) {
        if (target != targetName && target != "总计") {
            continue;
        }
        for (auto &[effectName, effectDamage] : targetDamage.items()) {
            int       count = 0;
            long long sum   = 0;
            for (auto &[subName, subDamage] : effectDamage.items()) {
                for (auto &[levelName, levelDamage] : subDamage.items()) {
                    for (auto &[rollResult, info] : levelDamage.items()) {
                        sum   += info["伤害"].get<long long>();
                        sum   += info["破招伤害"].get<long long>();
                        count += info["数目"].get<int>();
                    }
                }
            }
            if (targetEffectDamages.find(effectName) != targetEffectDamages.end()) {
                std::get<0>(targetEffectDamages[effectName]) += count;
                std::get<1>(targetEffectDamages[effectName]) += sum;
            } else {
                targetEffectDamages.emplace(effectName, std::tuple<int, long long>(count, sum));
            }
        }
    }
    return targetEffectDamages;
}

std::unordered_map<std::string, std::tuple<int, long long>> JX3DPS::Simulator::JsonParser::GetTargetEffectDamages(
    const nlohmann::ordered_json &json,
    const std::string            &target,
    const std::string            &effect)
{
    std::unordered_map<std::string, std::tuple<int, long long>> targetEffectRollDamages;
    for (auto &[targetName, targetDamage] : json.items()) {
        if (target != targetName && target != "总计") {
            continue;
        }
        for (auto &[effectName, effectDamage] : targetDamage.items()) {
            if (effect != effectName) {
                continue;
            }
            for (auto &[subName, subDamage] : effectDamage.items()) {
                for (auto &[levelName, levelDamage] : subDamage.items()) {
                    for (auto &[rollResult, info] : levelDamage.items()) {
                        int       count  = 0;
                        long long sum    = 0;
                        sum             += info["伤害"].get<long long>();
                        sum             += info["破招伤害"].get<long long>();
                        count           += info["数目"].get<int>();

                        if (targetEffectRollDamages.find(rollResult) !=
                            targetEffectRollDamages.end())
                        {
                            std::get<0>(targetEffectRollDamages[rollResult]) += count;
                            std::get<1>(targetEffectRollDamages[rollResult]) += sum;
                        } else {
                            targetEffectRollDamages.emplace(rollResult,
                                                            std::tuple<int, long long>(count, sum));
                        }
                    }
                }
            }
        }
    }
    return targetEffectRollDamages;
}
