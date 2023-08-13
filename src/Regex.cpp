/**
 * Project: JX3DPS
 * File: Regex.cpp
 * Description:
 * Created Date: 2023-07-23 15:44:52
 * Author: 难为水
 * -----
 * Last Modified: 2023-08-12 10:54:13
 * Modified By: 难为水
 * -----
 * CHANGELOG:
 * Date      	By     	Comments
 * ----------	-------	----------------------------------------------------------
 */

#include "Regex.h"

#include <regex>

JX3DPS::Error_t JX3DPS::Regex::ParseToExprSkillsHash(
    const std::list<std::pair<std::string, std::list<std::string>>> &strs,
    ExprSkillsHash                                                  &exprSkillsHash)
{
    std::unordered_map<Id_t, std::string> ids;
    Id_t                                  id = EXPRESSION_SKILL_PLACE_HOLDERS_1;
    for (const auto &[name, macro] : strs) {
        ids.emplace(id, name);
        id = static_cast<Id_t>(id + 1);
    }
    for (const auto &[name, macro] : strs) {
        ExprSkills exprSkills;
        Id_t       ii = EXPRESSION_SKILL_PLACE_HOLDERS_1;
        for (const auto &str : macro) {
            // 替换字符串中的name为id
            std::string temp = str;

            for (const auto &[i, n] : ids) {
                std::regex reg(n);
                temp = std::regex_replace(temp, reg, std::to_string(i - EXPRESSION_SKILL_PLACE_HOLDERS_DEFAULT));
                if (name == n) {
                    ii = i;
                }
            }
            ExprSkill exprSkill;
            ParseToExprSkill(temp, exprSkill);
            exprSkills.emplace_back(exprSkill);
        }
        exprSkillsHash[ii] = exprSkills;
    }

    return JX3DPS_SUCCESS;
}

const char *const JX3DPS_REGEX_EXPRESSION_SKILL_TYPE_CAST = "cast";

const char *const JX3DPS_REGEX_EXPRESSION_SKILL_TYPE_FCAST = "fcast";

const char *const JX3DPS_REGEX_EXPRESSION_SKILL_TYPE_SCAST = "scast";

const char *const JX3DPS_REGEX_EXPRESSION_SKILL_TYPE_SFCAST = "sfcast";

const char *const JX3DPS_REGEX_EXPRESSION_SKILL_SYMBOL_AND = "&";

const char *const JX3DPS_REGEX_EXPRESSION_SKILL_SYMBOL_OR = "|";

const char *const JX3DPS_REGEX_EXPRESSION_SKILL_CAST_SKILL =
    "\\/(s?f?cast)\\s?[\\[]?(.*?)[\\]]?\\s+([\u4e00-\u9fa5·0-9a-zA-Z]+)";

const char *const JX3DPS_REGEX_EXPRESSION_SKILL_CHANGE_TARGET =
    "\\/(s?f?cast)\\s?[\\[]?(.*?)[\\]]?\\s+转火·([\u4e00-\u9fa5·0-9a-zA-Z]+)";

const char *const JX3DPS_REGEX_EXPRESSION_SKILL_CHANGE_EXPRESSION =
    "\\/(s?f?cast)\\s?[\\[]?(.*?)[\\]]?\\s+宏·([\u4e00-\u9fa5·0-9a-zA-Z]+)";

JX3DPS::Error_t JX3DPS::Regex::ParseToExprSkill(const std::string &str, ExprSkill &exprSkill)
{
    std::smatch mat;
    std::regex  reg(JX3DPS_REGEX_EXPRESSION_SKILL_CAST_SKILL);
    if (!std::regex_match(str, mat, reg)) {
        return JX3DPS_ERROR_INVALID_EXPRESSION;
    }

    std::string cast      = mat[1].str();
    std::string condition = mat[2].str();
    std::string name      = mat[3].str();

    // scast sfcast处理
    std::list<ExprIf> scast;
    if (cast == JX3DPS_REGEX_EXPRESSION_SKILL_TYPE_SCAST || cast == JX3DPS_REGEX_EXPRESSION_SKILL_TYPE_SFCAST)
    {
        scast.emplace_back(std::bind(&JX3DPS::Expression::IsScast, std::placeholders::_1, std::placeholders::_2, true));
    } else {
        scast.emplace_back(std::bind(&JX3DPS::Expression::IsScast, std::placeholders::_1, std::placeholders::_2, false));
    }
    exprSkill.first.emplace_back(scast);

    // 前置条件处理
    std::list<ExprIf> pre;
    std::regex        regChangeTarget(JX3DPS_REGEX_EXPRESSION_SKILL_CHANGE_TARGET);
    std::regex regChangeExpression(JX3DPS_REGEX_EXPRESSION_SKILL_CHANGE_EXPRESSION);
    if (std::regex_match(str, mat, regChangeTarget)) {
        Id_t id = static_cast<Id_t>(std::stoi(mat[3].str()) + TARGET_PLACE_HOLDERS_DEFAULT);
        exprSkill.second = id;
    } else if (std::regex_match(str, mat, regChangeExpression)) {
        Id_t id = static_cast<Id_t>(std::stoi(mat[3].str()) + EXPRESSION_SKILL_PLACE_HOLDERS_DEFAULT);
        exprSkill.second = id;
    } else {
        Id_t id = SkillId(name);
        if (id == JX3DPS_ID_DEFAULT) {
            return JX3DPS_ERROR_INVALID_EXPRESSION_SKILL;
        }
        exprSkill.second = id;
        // 添加冷却判断
        pre.emplace_back(std::bind(&JX3DPS::Expression::IsCooldown, std::placeholders::_1, std::placeholders::_2, id));
        // 添加是否在范围内判断
        pre.emplace_back(std::bind(&JX3DPS::Expression::IsTargetWithinRange,
                                   std::placeholders::_1,
                                   std::placeholders::_2,
                                   id));

        // 添加读条判断
        bool fcast = false;
        if (cast == JX3DPS_REGEX_EXPRESSION_SKILL_TYPE_FCAST || cast == JX3DPS_REGEX_EXPRESSION_SKILL_TYPE_SFCAST)
        {
            fcast = true;
            exprSkill.first.front().emplace_back(
                std::bind(&JX3DPS::Expression::StopReCastSkill, std::placeholders::_1, std::placeholders::_2, id));
        }
        pre.emplace_back(std::bind(&JX3DPS::Expression::IsReady, std::placeholders::_1, std::placeholders::_2, id, fcast));
    }
    exprSkill.first.emplace_back(pre);

    // 条件处理
    std::list<ExprIf> exprIfs;
    if (condition.empty()) {
        exprSkill.first.push_back(std::list<ExprIf>{});
        return JX3DPS_SUCCESS;
    }
    std::string temp;
    std::string temp2;
    ExprIf      exprIf;
    Error_t     err = JX3DPS_SUCCESS;
    while (condition.find(JX3DPS_REGEX_EXPRESSION_SKILL_SYMBOL_OR) != std::string::npos) {
        temp = condition.substr(0, condition.find(JX3DPS_REGEX_EXPRESSION_SKILL_SYMBOL_OR));
        std::list<ExprIf> exprIfs;
        while (temp.find(JX3DPS_REGEX_EXPRESSION_SKILL_SYMBOL_AND) != std::string::npos) {
            temp2 = temp.substr(0, temp.find(JX3DPS_REGEX_EXPRESSION_SKILL_SYMBOL_AND));
            err   = ParseToExprIf(temp2, exprIf);
            if (err != JX3DPS_SUCCESS) {
                return err;
            }
            exprIfs.emplace_back(exprIf);
            temp = temp.substr(temp.find(JX3DPS_REGEX_EXPRESSION_SKILL_SYMBOL_AND) + 1);
        }
        err = ParseToExprIf(temp, exprIf);
        if (err != JX3DPS_SUCCESS) {
            return err;
        }
        exprIfs.emplace_back(exprIf);
        exprSkill.first.emplace_back(exprIfs);
        condition = condition.substr(condition.find(JX3DPS_REGEX_EXPRESSION_SKILL_SYMBOL_OR) + 1);
    }

    while (condition.find(JX3DPS_REGEX_EXPRESSION_SKILL_SYMBOL_AND) != std::string::npos) {
        temp2 = condition.substr(0, condition.find(JX3DPS_REGEX_EXPRESSION_SKILL_SYMBOL_AND));
        err = ParseToExprIf(temp2, exprIf);
        if (err != JX3DPS_SUCCESS) {
            return err;
        }
        exprIfs.emplace_back(exprIf);
        condition = condition.substr(condition.find(JX3DPS_REGEX_EXPRESSION_SKILL_SYMBOL_AND) + 1);
    }
    err = ParseToExprIf(condition, exprIf);
    if (err != JX3DPS_SUCCESS) {
        return err;
    }
    exprIfs.emplace_back(exprIf);
    exprSkill.first.emplace_back(exprIfs);

    return JX3DPS_SUCCESS;
}

const char *const JX3DPS_REGEX_EXPRESSION_EVENT =
    "([\\d{1,2}:\\d{1,2}.\\d{1,2}]*)[ ]?\\/([a-z0-9_]*)[ ]?(.*)";

const char *const JX3DPS_REGEX_EXPRESSION_EVENT_TYPE_ADD_TARGET = "add_target";

const char *const JX3DPS_REGEX_EXPRESSION_EVENT_TYPE_CHANGE_TARGET = "change_target";

const char *const JX3DPS_REGEX_EXPRESSION_EVENT_TYPE_SET_TARGET = "set_target";

const char *const JX3DPS_REGEX_EXPRESSION_EVENT_TYPE_ADD_BUFF = "add_buff";

const char *const JX3DPS_REGEX_EXPRESSION_EVENT_TYPE_CLEAR_BUFF = "clear_buff";

const char *const JX3DPS_REGEX_EXPRESSION_EVENT_TYPE_STOP = "stop";

const char *const JX3DPS_REGEX_EXPRESSION_EVENT_TYPE_CONTINUE = "continue";

const char *const JX3DPS_REGEX_EXPRESSION_EVENT_TYPE_END = "end";

JX3DPS::Error_t JX3DPS::Regex::ParseToExprEvents(const std::list<std::string> &strs, ExprEvents &exprEvents)
{
    for (const auto &str : strs) {
        std::regex  reg(JX3DPS_REGEX_EXPRESSION_EVENT);
        std::smatch mat;
        if (!std::regex_match(str, mat, reg)) {
            return JX3DPS_ERROR_INVALID_EXPRESSION_EVENT;
        }

        ExprEvent exprEvent;
        Error_t   err = ParseToFrame(mat[1].str(), exprEvent.first);
        if (err != JX3DPS_SUCCESS) {
            return err;
        }

        std::string temp = std::string("/") + mat[2].str() + " " + mat[3].str();

        if (mat[2].str() == JX3DPS_REGEX_EXPRESSION_EVENT_TYPE_ADD_TARGET) {
            ExprEvents exprs;
            exprs.emplace_back(exprEvent);
            err = ParseToAddTarget(temp, exprs);
            exprEvents.insert(exprEvents.end(), exprs.begin(), exprs.end());
        } else if (mat[2].str() == JX3DPS_REGEX_EXPRESSION_EVENT_TYPE_CHANGE_TARGET) {
            err = ParseToChangeTarget(temp, exprEvent);
            exprEvents.emplace_back(exprEvent);
        } else if (mat[2].str() == JX3DPS_REGEX_EXPRESSION_EVENT_TYPE_SET_TARGET) {
            err = ParseToSetTarget(temp, exprEvent);
            exprEvents.emplace_back(exprEvent);
        } else if (mat[2].str() == JX3DPS_REGEX_EXPRESSION_EVENT_TYPE_ADD_BUFF) {
            err = ParseToAddBuff(temp, exprEvent);
            exprEvents.emplace_back(exprEvent);
        } else if (mat[2].str() == JX3DPS_REGEX_EXPRESSION_EVENT_TYPE_CLEAR_BUFF) {
            err = ParseToClearBuff(temp, exprEvent);
            exprEvents.emplace_back(exprEvent);
        } else if (mat[2].str() == JX3DPS_REGEX_EXPRESSION_EVENT_TYPE_STOP) {
            err = ParseToStop(temp, exprEvent);
            exprEvents.emplace_back(exprEvent);
        } else if (mat[2].str() == JX3DPS_REGEX_EXPRESSION_EVENT_TYPE_CONTINUE) {
            err = ParseToContinue(temp, exprEvent);
            exprEvents.emplace_back(exprEvent);
        } else if (mat[2].str() == JX3DPS_REGEX_EXPRESSION_EVENT_TYPE_END) {
            err = ParseToEnd(temp, exprEvent);
            exprEvents.emplace_back(exprEvent);
        } else {
            return JX3DPS_ERROR_INVALID_EXPRESSION_EVENT;
        }
    }

    std::stable_sort(exprEvents.begin(), exprEvents.end(), [](const ExprEvent &a, const ExprEvent &b) {
        return a.first < b.first;
    });

    return JX3DPS_SUCCESS;
}

const char *const JX3DPS_REGEX_EXPRESSION_SKILL_CONDITION_QIDIAN =
    "qidian([=~<>]+)(\\d+)";

const char *const JX3DPS_REGEX_EXPRESSION_SKILL_CONDITION_BUFF =
    "buff:([\u4e00-\u9fa5·0-9a-zA-Z]+)";

const char *const JX3DPS_REGEX_EXPRESSION_SKILL_CONDITION_NO_BUFF =
    "nobuff:([\u4e00-\u9fa5·0-9a-zA-Z]+)";

const char *const JX3DPS_REGEX_EXPRESSION_SKILL_CONDITION_BUFF_STACK_NUM =
    "buff_stacknum:([\u4e00-\u9fa5·0-9a-zA-Z]+)([=~<>]+)(\\d+)";

const char *const JX3DPS_REGEX_EXPRESSION_SKILL_CONDITION_BUFF_TIME =
    "bufftime:([\u4e00-\u9fa5·0-9a-zA-Z]+)([=~<>]+)([\\d.]+)";

const char *const JX3DPS_REGEX_EXPRESSION_SKILL_CONDITION_TARGET_BUFF =
    "tbuff:([\u4e00-\u9fa5·0-9a-zA-Z]+)";

const char *const JX3DPS_REGEX_EXPRESSION_SKILL_CONDITION_TARGET_NO_BUFF =
    "tnobuff:([\u4e00-\u9fa5·0-9a-zA-Z]+)";

const char *const JX3DPS_REGEX_EXPRESSION_SKILL_CONDITION_TARGET_BUFF_STACK_NUM =
    "tbuff_stacknum:([\u4e00-\u9fa5·0-9a-zA-Z]+)([=~<>]+)(\\d+)";

const char *const JX3DPS_REGEX_EXPRESSION_SKILL_CONDITION_TARGET_BUFF_TIME =
    "tbufftime:([\u4e00-\u9fa5·0-9a-zA-Z]+)(.*?)([\\d.]+)";

const char *const JX3DPS_REGEX_EXPRESSION_SKILL_CONDITION_LAST_CAST_SKILL =
    "last_cast_skill(.*?)([\u4e00-\u9fa5·0-9a-zA-Z]+)";

const char *const JX3DPS_REGEX_EXPRESSION_SKILL_CONDITION_SKILL_COOLDONW =
    "skill_cd:([\u4e00-\u9fa5·0-9a-zA-Z]+)([=~<>]+)([\\d.]+)";

JX3DPS::Error_t JX3DPS::Regex::ParseToExprIf(const std::string &str, JX3DPS::ExprIf &exprIf)
{
    std::regex regQidian(JX3DPS_REGEX_EXPRESSION_SKILL_CONDITION_QIDIAN);
    std::regex regBuff(JX3DPS_REGEX_EXPRESSION_SKILL_CONDITION_BUFF);
    std::regex regNoBuff(JX3DPS_REGEX_EXPRESSION_SKILL_CONDITION_NO_BUFF);
    std::regex regBuffStackNum(JX3DPS_REGEX_EXPRESSION_SKILL_CONDITION_BUFF_STACK_NUM);
    std::regex regBuffTime(JX3DPS_REGEX_EXPRESSION_SKILL_CONDITION_BUFF_TIME);
    std::regex regTbuff(JX3DPS_REGEX_EXPRESSION_SKILL_CONDITION_TARGET_BUFF);
    std::regex regTNoBuff(JX3DPS_REGEX_EXPRESSION_SKILL_CONDITION_TARGET_NO_BUFF);
    std::regex regTbuffStackNum(JX3DPS_REGEX_EXPRESSION_SKILL_CONDITION_TARGET_BUFF_STACK_NUM);
    std::regex regTbuffTime(JX3DPS_REGEX_EXPRESSION_SKILL_CONDITION_TARGET_BUFF_TIME);
    std::regex regLastCastSkill(JX3DPS_REGEX_EXPRESSION_SKILL_CONDITION_LAST_CAST_SKILL);
    std::regex regSkillCd(JX3DPS_REGEX_EXPRESSION_SKILL_CONDITION_SKILL_COOLDONW);

    std::smatch mat;
    if (std::regex_match(str, mat, regQidian)) {
        if (mat[1].str() == "<") {
            exprIf = std::bind(&JX3DPS::Expression::QidianLt,
                               std::placeholders::_1,
                               std::placeholders::_2,
                               std::stoi(mat[2].str()));
        } else if (mat[1].str() == "<=") {
            exprIf = std::bind(&JX3DPS::Expression::QidianLe,
                               std::placeholders::_1,
                               std::placeholders::_2,
                               std::stoi(mat[2].str()));
        } else if (mat[1].str() == "=") {
            exprIf = std::bind(&JX3DPS::Expression::QidianEq,
                               std::placeholders::_1,
                               std::placeholders::_2,
                               std::stoi(mat[2].str()));
        } else if (mat[1].str() == "~=") {
            exprIf = std::bind(&JX3DPS::Expression::QidianNe,
                               std::placeholders::_1,
                               std::placeholders::_2,
                               std::stoi(mat[2].str()));
        } else if (mat[1].str() == ">=") {
            exprIf = std::bind(&JX3DPS::Expression::QidianGe,
                               std::placeholders::_1,
                               std::placeholders::_2,
                               std::stoi(mat[2].str()));
        } else if (mat[1].str() == ">") {
            exprIf = std::bind(&JX3DPS::Expression::QidianGt,
                               std::placeholders::_1,
                               std::placeholders::_2,
                               std::stoi(mat[2].str()));
        }
    } else if (std::regex_match(str, mat, regBuff)) {
        JX3DPS::Id_t id = BuffId(mat[1].str());
        exprIf = std::bind(&JX3DPS::Expression::BuffExist, std::placeholders::_1, std::placeholders::_2, id);
    } else if (std::regex_match(str, mat, regNoBuff)) {
        JX3DPS::Id_t id = BuffId(mat[1].str());
        exprIf = std::bind(&JX3DPS::Expression::NoBuffExist, std::placeholders::_1, std::placeholders::_2, id);
    } else if (std::regex_match(str, mat, regBuffStackNum)) {
        JX3DPS::Id_t id = BuffId(mat[1].str());
        if (mat[2].str() == "<") {
            exprIf = std::bind(&JX3DPS::Expression::BuffStackNumLt,
                               std::placeholders::_1,
                               std::placeholders::_2,
                               id,
                               std::stoi(mat[3].str()));
        } else if (mat[2].str() == "<=") {
            exprIf = std::bind(&JX3DPS::Expression::BuffStackNumLe,
                               std::placeholders::_1,
                               std::placeholders::_2,
                               id,
                               std::stoi(mat[3].str()));
        } else if (mat[2].str() == "=") {
            exprIf = std::bind(&JX3DPS::Expression::BuffStackNumEq,
                               std::placeholders::_1,
                               std::placeholders::_2,
                               id,
                               std::stoi(mat[3].str()));
        } else if (mat[2].str() == "~=") {
            exprIf = std::bind(&JX3DPS::Expression::BuffStackNumNe,
                               std::placeholders::_1,
                               std::placeholders::_2,
                               id,
                               std::stoi(mat[3].str()));
        } else if (mat[2].str() == ">=") {
            exprIf = std::bind(&JX3DPS::Expression::BuffStackNumGe,
                               std::placeholders::_1,
                               std::placeholders::_2,
                               id,
                               std::stoi(mat[3].str()));
        } else if (mat[2].str() == ">") {
            exprIf = std::bind(&JX3DPS::Expression::BuffStackNumGt,
                               std::placeholders::_1,
                               std::placeholders::_2,
                               id,
                               std::stoi(mat[3].str()));
        }
    } else if (std::regex_match(str, mat, regBuffTime)) {
        JX3DPS::Id_t id = BuffId(mat[1].str());
        if (mat[2].str() == "<") {
            exprIf = std::bind(&JX3DPS::Expression::BuffTimeLt,
                               std::placeholders::_1,
                               std::placeholders::_2,
                               id,
                               std::stod(mat[3].str()) * JX3DPS::JX3_FRAMES_PER_SECOND);
        } else if (mat[2].str() == "<=") {
            exprIf = std::bind(&JX3DPS::Expression::BuffTimeLe,
                               std::placeholders::_1,
                               std::placeholders::_2,
                               id,
                               std::stod(mat[3].str()) * JX3DPS::JX3_FRAMES_PER_SECOND);
        } else if (mat[2].str() == "=") {
            exprIf = std::bind(&JX3DPS::Expression::BuffTimeEq,
                               std::placeholders::_1,
                               std::placeholders::_2,
                               id,
                               std::stod(mat[3].str()) * JX3DPS::JX3_FRAMES_PER_SECOND);
        } else if (mat[2].str() == "~=") {
            exprIf = std::bind(&JX3DPS::Expression::BuffTimeNe,
                               std::placeholders::_1,
                               std::placeholders::_2,
                               id,
                               std::stod(mat[3].str()) * JX3DPS::JX3_FRAMES_PER_SECOND);
        } else if (mat[2].str() == ">=") {
            exprIf = std::bind(&JX3DPS::Expression::BuffTimeGe,
                               std::placeholders::_1,
                               std::placeholders::_2,
                               id,
                               std::stod(mat[3].str()) * JX3DPS::JX3_FRAMES_PER_SECOND);
        } else if (mat[2].str() == ">") {
            exprIf = std::bind(&JX3DPS::Expression::BuffTimeGt,
                               std::placeholders::_1,
                               std::placeholders::_2,
                               id,
                               std::stod(mat[3].str()) * JX3DPS::JX3_FRAMES_PER_SECOND);
        }
    } else if (std::regex_match(str, mat, regTbuff)) {
        JX3DPS::Id_t id = BuffId(mat[1].str());
        exprIf = std::bind(&JX3DPS::Expression::TBuffExist, std::placeholders::_1, std::placeholders::_2, id);
    } else if (std::regex_match(str, mat, regTNoBuff)) {
        JX3DPS::Id_t id = BuffId(mat[1].str());
        exprIf = std::bind(&JX3DPS::Expression::TNoBuffExist, std::placeholders::_1, std::placeholders::_2, id);
    } else if (std::regex_match(str, mat, regTbuffStackNum)) {
        JX3DPS::Id_t id = BuffId(mat[1].str());
        if (mat[2].str() == "<") {
            exprIf = std::bind(&JX3DPS::Expression::TBuffStackNumLt,
                               std::placeholders::_1,
                               std::placeholders::_2,
                               id,
                               std::stoi(mat[3].str()));
        } else if (mat[2].str() == "<=") {
            exprIf = std::bind(&JX3DPS::Expression::TBuffStackNumLe,
                               std::placeholders::_1,
                               std::placeholders::_2,
                               id,
                               std::stoi(mat[3].str()));
        } else if (mat[2].str() == "=") {
            exprIf = std::bind(&JX3DPS::Expression::TBuffStackNumEq,
                               std::placeholders::_1,
                               std::placeholders::_2,
                               id,
                               std::stoi(mat[3].str()));
        } else if (mat[2].str() == "~=") {
            exprIf = std::bind(&JX3DPS::Expression::TBuffStackNumNe,
                               std::placeholders::_1,
                               std::placeholders::_2,
                               id,
                               std::stoi(mat[3].str()));
        } else if (mat[2].str() == ">=") {
            exprIf = std::bind(&JX3DPS::Expression::TBuffStackNumGe,
                               std::placeholders::_1,
                               std::placeholders::_2,
                               id,
                               std::stoi(mat[3].str()));
        } else if (mat[2].str() == ">") {
            exprIf = std::bind(&JX3DPS::Expression::TBuffStackNumGt,
                               std::placeholders::_1,
                               std::placeholders::_2,
                               id,
                               std::stoi(mat[3].str()));
        }
    } else if (std::regex_match(str, mat, regTbuffTime)) {
        JX3DPS::Id_t id = BuffId(mat[1].str());
        if (mat[2].str() == "<") {
            exprIf = std::bind(&JX3DPS::Expression::TBuffTimeLt,
                               std::placeholders::_1,
                               std::placeholders::_2,
                               id,
                               std::stod(mat[3].str()) * JX3DPS::JX3_FRAMES_PER_SECOND);
        } else if (mat[2].str() == "<=") {
            exprIf = std::bind(&JX3DPS::Expression::TBuffTimeLe,
                               std::placeholders::_1,
                               std::placeholders::_2,
                               id,
                               std::stod(mat[3].str()) * JX3DPS::JX3_FRAMES_PER_SECOND);
        } else if (mat[2].str() == "=") {
            exprIf = std::bind(&JX3DPS::Expression::TBuffTimeEq,
                               std::placeholders::_1,
                               std::placeholders::_2,
                               id,
                               std::stod(mat[3].str()) * JX3DPS::JX3_FRAMES_PER_SECOND);
        } else if (mat[2].str() == "~=") {
            exprIf = std::bind(&JX3DPS::Expression::TBuffTimeNe,
                               std::placeholders::_1,
                               std::placeholders::_2,
                               id,
                               std::stod(mat[3].str()) * JX3DPS::JX3_FRAMES_PER_SECOND);
        } else if (mat[2].str() == ">=") {
            exprIf = std::bind(&JX3DPS::Expression::TBuffTimeGe,
                               std::placeholders::_1,
                               std::placeholders::_2,
                               id,
                               std::stod(mat[3].str()) * JX3DPS::JX3_FRAMES_PER_SECOND);
        } else if (mat[2].str() == ">") {
            exprIf = std::bind(&JX3DPS::Expression::TBuffTimeGt,
                               std::placeholders::_1,
                               std::placeholders::_2,
                               id,
                               std::stod(mat[3].str()) * JX3DPS::JX3_FRAMES_PER_SECOND);
        }
    } else if (std::regex_match(str, mat, regLastCastSkill)) {
        JX3DPS::Id_t id = SkillId(mat[1].str());
        if (mat[1].str() == "=") {
            exprIf =
                std::bind(&JX3DPS::Expression::LastCastSkill, std::placeholders::_1, std::placeholders::_2, id);
        } else if (mat[1].str() == "~=") {
            exprIf =
                std::bind(&JX3DPS::Expression::NotLastCastSkill, std::placeholders::_1, std::placeholders::_2, id);
        }
    } else if (std::regex_match(str, mat, regSkillCd)) {
        JX3DPS::Id_t id = SkillId(mat[1].str());
        if (mat[2].str() == "<") {
            exprIf = std::bind(&JX3DPS::Expression::SkillCooldownLt,
                               std::placeholders::_1,
                               std::placeholders::_2,
                               id,
                               std::stod(mat[3].str()));
        } else if (mat[2].str() == "<=") {
            exprIf = std::bind(&JX3DPS::Expression::SkillCooldownLe,
                               std::placeholders::_1,
                               std::placeholders::_2,
                               id,
                               std::stod(mat[3].str()));
        } else if (mat[2].str() == "=") {
            exprIf = std::bind(&JX3DPS::Expression::SkillCooldownEq,
                               std::placeholders::_1,
                               std::placeholders::_2,
                               id,
                               std::stod(mat[3].str()));
        } else if (mat[2].str() == "~=") {
            exprIf = std::bind(&JX3DPS::Expression::SkillCooldownNe,
                               std::placeholders::_1,
                               std::placeholders::_2,
                               id,
                               std::stod(mat[3].str()));
        } else if (mat[2].str() == ">=") {
            exprIf = std::bind(&JX3DPS::Expression::SkillCooldownGe,
                               std::placeholders::_1,
                               std::placeholders::_2,
                               id,
                               std::stod(mat[3].str()));
        } else if (mat[2].str() == ">") {
            exprIf = std::bind(&JX3DPS::Expression::SkillCooldownGt,
                               std::placeholders::_1,
                               std::placeholders::_2,
                               id,
                               std::stod(mat[3].str()));
        }
    } else {
        return JX3DPS_ERROR_INVALID_EXPRESSION;
    }
    return JX3DPS_SUCCESS;
}

const char *const JX3DPS_REGEX_EXPRESSION_EVENT_TIME =
    "(\\d{1,2}):(\\d{1,2}).?(\\d{0,2})";

JX3DPS::Error_t JX3DPS::Regex::ParseToFrame(const std::string &str, Frame_t &frame)
{
    std::regex  reg(JX3DPS_REGEX_EXPRESSION_EVENT_TIME);
    std::smatch mat;
    if (!std::regex_match(str, mat, reg)) {
        return JX3DPS_ERROR_INVALID_EXPRESSION_EVENT_TIME;
    }
    int temp = 0;
    if (!mat[3].str().empty()) {
        temp = std::stoi(mat[3].str());
    }
    frame = (std::stoi(mat[1].str()) * JX3_SECONDS_PER_MINUTE + std::stoi(mat[2].str())) *
                JX3DPS::JX3_FRAMES_PER_SECOND +
            temp;
    return JX3DPS_SUCCESS;
}

const char *const JX3DPS_REGEX_EXPRESSION_EVENT_ADD_TARGET =
    "\\/add_target id=(\\d+) level=(\\d+) shield=(\\d+) distance=([\\d.]+)( "
    "lifetime=(\\d+))?";

JX3DPS::Error_t JX3DPS::Regex::ParseToAddTarget(const std::string &str, ExprEvents &exprEvents)
{
    std::regex  reg(JX3DPS_REGEX_EXPRESSION_EVENT_ADD_TARGET);
    std::smatch mat;
    if (!std::regex_match(str, mat, reg)) {
        return JX3DPS_ERROR_INVALID_EXPRESSION_EVENT_ADD_TARGET;
    }

    Id_t id = static_cast<Id_t>(std::stoi(mat[1].str()) + TARGET_PLACE_HOLDERS_DEFAULT);

    Frame_t frame = 0;
    if (mat[5].str().empty()) {
        frame = JX3DPS_INVALID_FRAMES_SET;
    } else {
        frame = std::stoi(mat[6].str()) * JX3DPS::JX3_FRAMES_PER_SECOND;
    }

    exprEvents.front().second =
        std::bind(&JX3DPS::Expression::AddTarget,
                  std::placeholders::_1,
                  std::placeholders::_2,
                  id,
                  std::stoi(mat[2].str()),
                  std::stoi(mat[3].str()),
                  std::stod(mat[4].str()),
                  frame);

    ExprEvent exprEvent;
    exprEvent.first = exprEvents.front().first + frame;
    exprEvent.second =
        std::bind(&JX3DPS::Expression::SetTargetDead, std::placeholders::_1, std::placeholders::_2, id);
    exprEvents.emplace_back(exprEvent);

    return JX3DPS_SUCCESS;
}

const char *const JX3DPS_REGEX_EXPRESSION_EVENT_CHANGE_TARGET =
    "\\/change_target id=(\\d+)";

JX3DPS::Error_t JX3DPS::Regex::ParseToChangeTarget(const std::string &str, ExprEvent &exprEvent)
{
    std::regex  reg(JX3DPS_REGEX_EXPRESSION_EVENT_CHANGE_TARGET);
    std::smatch mat;
    if (!std::regex_match(str, mat, reg)) {
        return JX3DPS_ERROR_INVALID_EXPRESSION_EVENT_CHANGE_TARGET;
    }

    Id_t id = static_cast<Id_t>(std::stoi(mat[1].str()) + TARGET_PLACE_HOLDERS_DEFAULT);

    exprEvent.second =
        std::bind(&JX3DPS::Expression::ChangeTarget, std::placeholders::_1, std::placeholders::_2, id);

    return JX3DPS_SUCCESS;
}

const char *const JX3DPS_REGEX_EXPRESSION_EVENT_SET_TARGET =
    "\\/set_target id=(\\d+) (distance|life|mana|dead)(=(\\d+(?:\\.\\d+)?))?";

const char *const JX3DPS_REGEX_EXPRESSION_EVENT_TYPE_SET_TARGET_LIFE = "life";

const char *const JX3DPS_REGEX_EXPRESSION_EVENT_TYPE_SET_TARGET_MANA = "mana";

const char *const JX3DPS_REGEX_EXPRESSION_EVENT_TYPE_SET_TARGET_DISTANCE = "distance";

const char *const JX3DPS_REGEX_EXPRESSION_EVENT_TYPE_SET_TARGET_DEAD = "dead";

JX3DPS::Error_t JX3DPS::Regex::ParseToSetTarget(const std::string &str, ExprEvent &exprEvent)
{
    std::regex  reg(JX3DPS_REGEX_EXPRESSION_EVENT_SET_TARGET);
    std::smatch mat;
    if (!std::regex_match(str, mat, reg)) {
        return JX3DPS_ERROR_INVALID_EXPRESSION_EVENT_SET_TARGET;
    }

    Id_t id = static_cast<Id_t>(std::stoi(mat[1].str()) + TARGET_PLACE_HOLDERS_DEFAULT);

    if (mat[2].str() == JX3DPS_REGEX_EXPRESSION_EVENT_TYPE_SET_TARGET_LIFE) {
        exprEvent.second =
            std::bind(&JX3DPS::Expression::SetTargetLife,
                      std::placeholders::_1,
                      std::placeholders::_2,
                      id,
                      std::stod(mat[3].str()));
    } else if (mat[2].str() == JX3DPS_REGEX_EXPRESSION_EVENT_TYPE_SET_TARGET_MANA) {
        exprEvent.second =
            std::bind(&JX3DPS::Expression::SetTargetMana,
                      std::placeholders::_1,
                      std::placeholders::_2,
                      id,
                      std::stod(mat[3].str()));
    } else if (mat[2].str() == JX3DPS_REGEX_EXPRESSION_EVENT_TYPE_SET_TARGET_DISTANCE) {
        exprEvent.second =
            std::bind(&JX3DPS::Expression::SetTargetDistance,
                      std::placeholders::_1,
                      std::placeholders::_2,
                      id,
                      std::stod(mat[3].str()));
    } else if (mat[2].str() == JX3DPS_REGEX_EXPRESSION_EVENT_TYPE_SET_TARGET_DEAD) {
        exprEvent.second =
            std::bind(&JX3DPS::Expression::SetTargetDead, std::placeholders::_1, std::placeholders::_2, id);
    }
    return JX3DPS_SUCCESS;
}

const char *const JX3DPS_REGEX_EXPRESSION_EVENT_ADD_BUFF =
    "\\/add_buff id=(\\d{1,2}) name=([\u4e00-\u9fa5·0-9a-zA-Z]+)( stack_num=(\\d{1,}))?( "
    "duration=(\\d{1,})-(\\d{1,}))?";

JX3DPS::Error_t JX3DPS::Regex::ParseToAddBuff(const std::string &str, ExprEvent &exprEvent)
{
    std::regex  reg(JX3DPS_REGEX_EXPRESSION_EVENT_ADD_BUFF);
    std::smatch mat;
    if (!std::regex_match(str, mat, reg)) {
        return JX3DPS_ERROR_INVALID_EXPRESSION_EVENT_ADD_BUFF;
    }

    Id_t id = TARGET_PLACE_HOLDERS_DEFAULT;
    if (std::stoi(mat[1].str()) == 0) {
        id = PLAYER_ID;
    } else {
        id = static_cast<Id_t>(std::stoi(mat[2].str()) + TARGET_PLACE_HOLDERS_DEFAULT);
    }

    Id_t buffId = BuffId(mat[2].str());

    int stackNum = 1;
    if (!mat[4].str().empty()) {
        stackNum = std::stoi(mat[4].str());
    }

    JX3DPS::Frame_t durationMin = JX3DPS::JX3DPS_DEFAULT_DURATION_FRAMES;
    JX3DPS::Frame_t durationMax = JX3DPS::JX3DPS_DEFAULT_DURATION_FRAMES;
    if (!mat[6].str().empty()) {
        durationMin = std::stoi(mat[6].str()) * JX3DPS::JX3_FRAMES_PER_SECOND;
        durationMax = std::stoi(mat[7].str()) * JX3DPS::JX3_FRAMES_PER_SECOND;
    }

    exprEvent.second =
        std::bind(&JX3DPS::Expression::AddBuff, std::placeholders::_1, std::placeholders::_2, id, buffId, stackNum, durationMin, durationMax);
    return JX3DPS_SUCCESS;
}

JX3DPS::Error_t JX3DPS::Regex::ParseToBuff3rds(const std::list<std::string> &strs, std::list<Id_t> &buff3rds)
{
    for (const auto &str : strs) {
        std::regex  reg(JX3DPS_REGEX_EXPRESSION_EVENT);
        std::smatch mat;
        if (!std::regex_match(str, mat, reg)) {
            return JX3DPS_ERROR_INVALID_EXPRESSION_EVENT;
        }

        std::string temp = std::string("/") + mat[2].str() + " " + mat[3].str();

        std::regex  regAddbuff(JX3DPS_REGEX_EXPRESSION_EVENT_ADD_BUFF);
        std::smatch matAddbuff;
        if (!std::regex_match(temp, matAddbuff, regAddbuff)) {
            continue;
        }

        Id_t buffId = BuffId(mat[2].str());

        if (buffId > BUFF_3RD_DEFAULT && buffId < BUFF_3RD_END) {
            buff3rds.emplace_back(buffId);
        }
    }
    return JX3DPS_SUCCESS;
}

const char *const JX3DPS_REGEX_EXPRESSION_EVENT_CLEAR_BUFF =
    "\\/clear_buff id=(\\d{1,2}) name=([\u4e00-\u9fa5·0-9a-zA-Z]+) stack_num=(\\d{1,})";

JX3DPS::Error_t JX3DPS::Regex::ParseToClearBuff(const std::string &str, ExprEvent &exprEvent)
{
    std::regex  reg(JX3DPS_REGEX_EXPRESSION_EVENT_CLEAR_BUFF);
    std::smatch mat;
    if (!std::regex_match(str, mat, reg)) {
        return JX3DPS_ERROR_INVALID_EXPRESSION_EVENT_CLEAR_BUFF;
    }

    Id_t id = TARGET_PLACE_HOLDERS_DEFAULT;
    if (std::stoi(mat[1].str()) == 0) {
        id = PLAYER_ID;
    } else {
        id = static_cast<Id_t>(std::stoi(mat[1].str()) + TARGET_PLACE_HOLDERS_DEFAULT);
    }

    Id_t buffId = BuffId(mat[2].str());

    int stackNum = std::stoi(mat[3].str());

    exprEvent.second =
        std::bind(&JX3DPS::Expression::ClearBuff, std::placeholders::_1, std::placeholders::_2, id, buffId, stackNum);

    return JX3DPS_SUCCESS;
}

const char *const JX3DPS_REGEX_EXPRESSION_EVENT_STOP = "\\/stop";

JX3DPS::Error_t JX3DPS::Regex::ParseToStop(const std::string &str, ExprEvent &exprEvent)
{
    exprEvent.second =
        std::bind(&JX3DPS::Expression::SetPlayerStop, std::placeholders::_1, std::placeholders::_2, true);
    return JX3DPS_SUCCESS;
}

const char *const JX3DPS_REGEX_EXPRESSION_EVENT_CONTINUE = "\\/continue";

JX3DPS::Error_t JX3DPS::Regex::ParseToContinue(const std::string &str, ExprEvent &exprEvent)
{
    exprEvent.second =
        std::bind(&JX3DPS::Expression::SetPlayerStop, std::placeholders::_1, std::placeholders::_2, false);
    return JX3DPS_SUCCESS;
}

const char *const JX3DPS_REGEX_EXPRESSION_EVENT_END = "\\/end";

JX3DPS::Error_t JX3DPS::Regex::ParseToEnd(const std::string &str, ExprEvent &exprEvent)
{
    exprEvent.second =
        std::bind(&JX3DPS::Expression::SetEnd, std::placeholders::_1, std::placeholders::_2);
    return JX3DPS_SUCCESS;
}

JX3DPS::Error_t JX3DPS::Regex::ParseToTotalFrames(const std::list<std::string> &strs, Frame_t &totalFrames)
{
    for (const auto &str : strs) {
        std::regex  reg(JX3DPS_REGEX_EXPRESSION_EVENT);
        std::smatch mat;
        if (!std::regex_match(str, mat, reg)) {
            return JX3DPS_ERROR_INVALID_EXPRESSION_EVENT;
        }

        std::string temp = std::string("/") + mat[2].str();

        std::regex  regEnd(JX3DPS_REGEX_EXPRESSION_EVENT_END);
        std::smatch matEnd;
        if (!std::regex_match(temp, matEnd, regEnd)) {
            continue;
        }

        Error_t err = ParseToFrame(mat[1].str(), totalFrames);
        if (err != JX3DPS_SUCCESS) {
            return err;
        }
    }
    return JX3DPS_SUCCESS;
}
