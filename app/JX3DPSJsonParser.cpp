/**
 * Project: JX3DPS
 * File: JX3DPSJsonParser.cpp
 * Description:
 * Created Date: 2023-08-10 00:05:57
 * Author: 难为水
 * -----
 * Last Modified: 2023-08-10 05:22:42
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
    std::vector<std::list<ComboBox::ItemInfo>> &talents)
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
