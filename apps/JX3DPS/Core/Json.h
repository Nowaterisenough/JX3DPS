/**
 * Project: JX3DPS
 * File: Json.h
 * Description:
 * Created Date: 2023-06-18 18:59:53
 * Author: 难为水
 * -----
 * Last Modified: 2023-07-18 02:22:54
 * Modified By: 难为水
 * -----
 * HISTORY:
 * Date      	By   	Comments
 * ----------	-----	----------------------------------------------------------
 */

#ifndef JX3DPS_JSON_H_
#define JX3DPS_JSON_H_

#include <nlohmann/json.hpp>

#include "Attr.h"
#include "Defs.h"

namespace JX3DPS {

Error_t LoadConfig(const std::string &path, nlohmann::json &json);

Error_t ParseJson2Skills(const nlohmann::json &json, std::list<std::string> &skills);

Error_t ParseJson2Events(const nlohmann::json &json, std::list<std::string> &events);

Error_t ParseJson2SimIterations(const nlohmann::json &json, int &simIterations);

Error_t ParseJson2Talents(const nlohmann::json &json, Talents &talents);

Error_t ParseJson2Secrets(const nlohmann::json &json, Secrets &secrets);

Error_t ParseJson2Attr(const nlohmann::json &json, Attr &attr);

Error_t ParseJson2Permanent(const nlohmann::json &json, Attr &attr);

Error_t Stats2Json(const DamageStats &damageStats, nlohmann::json &json);

Error_t Gains2Json(const std::vector<float> &gains, nlohmann::json &json);

} // namespace JX3DPS

#endif // JX3DPS_JSON_H_