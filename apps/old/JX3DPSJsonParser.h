/**
 * Project: JX3DPS
 * File: JX3DPSJsonParser.h
 * Description:
 * Created Date: 2023-08-09 23:56:49
 * Author: 难为水
 * -----
 * Last Modified: 2023-09-13 01:52:06
 * Modified By: 难为水
 * -----
 * HISTORY:
 * Date      	By   	Comments
 * ----------	-----	----------------------------------------------------------
 */

#pragma once

#include <nlohmann/json.hpp>

#include "CheckBox/CheckBox.h"
#include "ComboBox/ComboBox.h"
#include "Global/JX3.h"

#include "TimeLineWidget.h"

namespace JX3DPS {

namespace Simulator {

namespace JsonParser {

QString JX3IconPath(int id);

void LoadConfig(const std::string &config, nlohmann::ordered_json &json);

void ParseJsonToClassTypeItemInfos(const nlohmann::ordered_json  &json,
                                   std::list<ComboBox::ItemInfo> &itemInfos);

void ParseJsonToDefaultAttribute(const nlohmann::ordered_json         &json,
                                 JX3DPS::ClassType                     classType,
                                 std::unordered_map<std::string, int> &attributes);

void ParseJsonToTalentItemInfos(const nlohmann::ordered_json               &json,
                                JX3DPS::ClassType                           classType,
                                std::vector<std::list<ComboBox::ItemInfo>> &talents,
                                std::list<std::string>                     &defaults);

void ParseJsonToRecipeItemInfos(const nlohmann::ordered_json &json,
                                JX3DPS::ClassType             classType,
                                std::list<std::pair<std::string, std::list<CheckBox::ItemInfo>>> &recipes,
                                std::list<std::string> &defaults);

void ParseJsonToSkills(const nlohmann::ordered_json &json,
                       JX3DPS::ClassType             classType,
                       std::list<std::pair<std::string, std::list<std::string>>> &skills);

void ParseJsonToEvents(const nlohmann::ordered_json &json,
                       JX3DPS::ClassType             classType,
                       std::list<std::string>       &events);

void ParseJsonToPermanents(const nlohmann::ordered_json &json,
                           JX3DPS::ClassType             classType,
                           std::unordered_map<std::string, CheckBox::ItemInfo> &permanents1,
                           std::unordered_map<std::string, std::list<ComboBox::ItemInfo>> &permanents2);

void LoadJsonPermanent(const nlohmann::ordered_json &json,
                       JX3DPS::ClassType             classType,
                       const std::string            &name,
                       nlohmann::ordered_json       &out);

bool LoadJsonTimeLine(const nlohmann::ordered_json &json,
                      const nlohmann::ordered_json &config,
                      std::list<SkillItem::Info>   &skillInfos);

long long GetTotalDamage(const nlohmann::ordered_json &json);

std::list<std::tuple<std::string, long long>> GetTargetDamages(const nlohmann::ordered_json &json);

std::unordered_map<std::string, std::tuple<int, long long>> GetTargetEffectDamages(
    const nlohmann::ordered_json &json,
    const std::string            &target);

std::unordered_map<std::string, std::tuple<int, long long>> GetTargetEffectDamages(
    const nlohmann::ordered_json &json,
    const std::string            &target,
    const std::string            &effect);

} // namespace JsonParser

} // namespace Simulator

} // namespace JX3DPS
