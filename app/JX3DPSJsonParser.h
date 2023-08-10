/**
 * Project: JX3DPS
 * File: JX3DPSJsonParser.h
 * Description:
 * Created Date: 2023-08-09 23:56:49
 * Author: 难为水
 * -----
 * Last Modified: 2023-08-11 03:14:10
 * Modified By: 难为水
 * -----
 * HISTORY:
 * Date      	By   	Comments
 * ----------	-----	----------------------------------------------------------
 */

#pragma once

#include "CheckBox/CheckBox.h"
#include "ComboBox/ComboBox.h"
#include "Global/JX3.h"
#include "nlohmann/json.hpp"

namespace JX3DPS {

namespace Simulator {

namespace JsonParser {

void LoadConfig(const std::string &config, nlohmann::ordered_json &json);

void ParseJsonToClassTypeItemInfos(const nlohmann::ordered_json  &json,
                                   std::list<ComboBox::ItemInfo> &itemInfos);

void ParseJsonToTalentItemInfos(const nlohmann::ordered_json               &json,
                                JX3DPS::ClassType                           classType,
                                std::vector<std::list<ComboBox::ItemInfo>> &talents);

void ParseJsonToRecipeItemInfos(const nlohmann::ordered_json &json,
                                JX3DPS::ClassType             classType,
                                std::unordered_map<std::string, std::list<CheckBox::ItemInfo>> &recipes);

void ParseJsonToSkills(const nlohmann::ordered_json &json,
                       JX3DPS::ClassType             classType,
                       std::list<std::pair<std::string, std::list<std::string>>> &skills);

void ParseJsonToEvents(const nlohmann::ordered_json &json,
                       JX3DPS::ClassType             classType,
                       std::list<std::string> &events);

} // namespace JsonParser

} // namespace Simulator

} // namespace JX3DPS
