/**
 * Project: JX3DPS
 * File: Json.h
 * Description:
 * Created Date: 2023-06-18 18:59:53
 * Author: 难为水
 * -----
 * Last Modified: 2023-09-13 04:15:03
 * Modified By: 难为水
 * -----
 * HISTORY:
 * Date      	By   	Comments
 * ----------	-----	----------------------------------------------------------
 */

#ifndef __JX3DPS_JSON_PARSE_H__
#define __JX3DPS_JSON_PARSE_H__

#include <unordered_set>

#include <nlohmann/json.hpp>

#include "Global/Defs.h"
#include "TimeLine.hpp"
namespace JX3DPS {

class Attribute;

Error_t ParseJsonToClassType(const nlohmann::ordered_json &json, ClassType &classType);

Error_t ParseJsonToOptions(const nlohmann::ordered_json &json, Options &options);

Error_t ParseJsonToSkills(const nlohmann::ordered_json &json,
                          std::list<std::pair<std::string, std::list<std::string>>> &skills);

Error_t ParseJsonToEvents(const nlohmann::ordered_json &json, std::list<std::string> &events);

Error_t ParseJsonToTotalFrames(const nlohmann::ordered_json &json, Frame_t &totalFrames);

Error_t ParseJsonToTalents(const nlohmann::ordered_json &json, Talents &talents);

Error_t ParseJsonToRecipes(const nlohmann::ordered_json &json, Recipes &recipes);

Error_t ParseJsonToEquipEffects(const nlohmann::ordered_json &json, EquipEffects &equipEffects);

Error_t ParseJsonToAttribute(const nlohmann::ordered_json &json, Attribute &attribute);

Error_t ParseJsonToPermanents(const nlohmann::ordered_json &json, Attribute &attribute);

Error_t ParseJsonToTeamCore(const nlohmann::ordered_json &json, Attribute &attribute, ClassType &teamCore);

Error_t ParseJsonToBuff3rds(const nlohmann::ordered_json &json, std::unordered_set<Id_t> &buff3rds);

Error_t StatsToJson(const Stats &stats, nlohmann::ordered_json &json);

Error_t DamageStatsToJson(const DamageStats &damageStats, nlohmann::ordered_json &json);

Error_t TimeLineToJson(const TimeLine::InfosList &infosList, nlohmann::ordered_json &json);

} // namespace JX3DPS

#endif // __JX3DPS_JSON_PARSE_H__