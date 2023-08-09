/**
 * Project: JX3DPS
 * File: JX3DPSJsonParser.h
 * Description:
 * Created Date: 2023-08-09 23:56:49
 * Author: 难为水
 * -----
 * Last Modified: 2023-08-10 05:22:36
 * Modified By: 难为水
 * -----
 * HISTORY:
 * Date      	By   	Comments
 * ----------	-----	----------------------------------------------------------
 */

#pragma once

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

} // namespace JsonParser

} // namespace Simulator

} // namespace JX3DPS
