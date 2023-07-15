/**
 * Project: JX3DPS
 * File: Macro.cpp
 * Description:
 * Created Date: 2023-06-01 15:41:10
 * Author: 难为水
 * -----
 * Last Modified: 2023-07-15 10:50:30
 * Modified By: 难为水
 * -----
 * HISTORY:
 * Date      	By   	Comments
 * ----------	-----	----------------------------------------------------------
 */

#include "Macro.h"

#include <regex>
#include <sstream>

#include <spdlog/spdlog.h>

#include "Buff.h"
#include "JX3Params.h"
#include "Player.h"
#include "Skill.h"
#include "Target.h"

const int JX3DPS_SECOND_PER_MINUTE = 60;

const char *const JX3DPS_MACRO_SYMBOL_LEFT_BRACKET = "[";

const char *const JX3DPS_MACRO_SYMBOL_RIGHT_BRACKET = "]";

const char *const JX3DPS_MACRO_SYMBOL_AND = "&";

const char *const JX3DPS_MACRO_SYMBOL_OR = "|";

const char *const JX3DPS_MACRO_TYPE_CAST = "cast";

const char *const JX3DPS_MACRO_TYPE_FCAST = "fcast";

const char *const JX3DPS_MACRO_TYPE_ADD_TARGET = "add_target";

const char *const JX3DPS_MACRO_TYPE_SET_TARGET = "set_target";

const char *const JX3DPS_MACRO_TYPE_ADD_BUFF = "add_buff";

const char *const JX3DPS_MACRO_TYPE_CLEAR_BUFF = "clear_buff";

const char *const JX3DPS_MACRO_TYPE_SET_LIFE = "life";

const char *const JX3DPS_MACRO_TYPE_SET_MANA = "mana";

const char *const JX3DPS_MACRO_TYPE_SET_TARGET_DISTANCE = "distance";

const char *const JX3DPS_MACRO_TYPE_SET_TARGET_DEAD = "dead";

const char *const JX3DPS_MACRO_TYPE_STOP = "stop";

const char *const JX3DPS_MACRO_TYPE_CONTINUE = "continue";

const char *const JX3DPS_MACRO_TYPE_END = "end";

const char *const JX3DPS_MACRO_REGEX = "([\\d{1,2}:\\d{1,2}.\\d{1,2}]*)[ ]?\\/([a-z0-9_]*)[ ]?(.*)";

const char *const JX3DPS_MACRO_REGEX_CAST =
    "\\/(f?cast)\\s?[\\[]?(.*?)[\\]]?\\s+([\u4e00-\u9fa5·0-9a-z]+)";

const char *const JX3DPS_MACRO_REGEX_ADD_TARGET =
    "([\\d{1,2}:\\d{1,2}.\\d{1,2}]*) \\/add_target id=(\\d{1,2}) level=(\\d{3}) shield=(\\d{1,}) "
    "distance=(\\d{1,})";

const char *const JX3DPS_MACRO_REGEX_SET_TARGET =
    "([\\d{1,2}:\\d{1,2}.\\d{1,2}]*) \\/set_target id=(\\d{1,2})([ ]?(.*?)=(\\d+(?:\\.\\d+)?))?";

const char *const JX3DPS_MACRO_REGEX_SET_TARGET_DEAD =
    "([\\d{1,2}:\\d{1,2}.\\d{1,2}]*) \\/set_target id=(\\d{1,2})[ ]dead";

const char *const JX3DPS_MACRO_REGEX_ADD_BUFF =
    "([\\d{1,2}:\\d{1,2}.\\d{1,2}]*) \\/add_buff id=(\\d{1,2}) name=([\u4e00-\u9fa5·0-9a-z]+)( "
    "stack_num=(\\d{1,}))?( duration=(\\d{1,}))?";

const char *const JX3DPS_MACRO_REGEX_CLEAR_BUFF =
    "([\\d{1,2}:\\d{1,2}.\\d{1,2}]*) \\/clear_buff id=(\\d{1,2}) name=([\u4e00-\u9fa5·0-9a-z]+) "
    "stack_num=(\\d{1,})";

const char *const JX3DPS_MACRO_REGEX_STOP = "([\\d{1,2}:\\d{1,2}.\\d{1,2}]*) \\/stop";

const char *const JX3DPS_MACRO_REGEX_CONTINUE = "([\\d{1,2}:\\d{1,2}.\\d{1,2}]*) \\/continue";

const char *const JX3DPS_MACRO_REGEX_END = "([\\d{1,2}:\\d{1,2}.\\d{1,2}]*) \\/end";

const char *const JX3DPS_MACRO_REGEX_CONDITION_QIDIAN = "qidian([=~<>]+)(\\d+)";

const char *const JX3DPS_MACRO_REGEX_CONDITION_BUFF_LAYERS =
    "buff:([\u4e00-\u9fa5·0-9a-z]+)([=~<>]+)(\\d+)";

const char *const JX3DPS_MACRO_REGEX_CONDITION_BUFF = "buff:([\u4e00-\u9fa5·0-9a-z]+)";

const char *const JX3DPS_MACRO_REGEX_CONDITION_NOBUFF = "nobuff:([\u4e00-\u9fa5·0-9a-z]+)";

const char *const JX3DPS_MACRO_REGEX_CONDITION_BUFFTIME =
    "bufftime:([\u4e00-\u9fa5·0-9a-z]+)([=~<>]+)(\\d+)";

const char *const JX3DPS_MACRO_REGEX_CONDITION_TBUFF_LAYERS =
    "tbuff:([\u4e00-\u9fa5·0-9a-z]+)([=~<>]+)(\\d+)";

const char *const JX3DPS_MACRO_REGEX_CONDITION_TBUFF = "tbuff:([\u4e00-\u9fa5·0-9a-z]+)";

const char *const JX3DPS_MACRO_REGEX_CONDITION_TNOBUFF = "tnobuff:([\u4e00-\u9fa5·0-9a-z]+)";

const char *const JX3DPS_MACRO_REGEX_CONDITION_TBUFFTIME =
    "tbufftime:([\u4e00-\u9fa5·0-9a-z]+)(.*?)(\\d+)";

const char *const JX3DPS_MACRO_REGEX_CONDITION_LAST_CAST_SKILL =
    "last_cast_skill(.*?)([\u4e00-\u9fa5·0-9a-z]+)";

const char *const JX3DPS_MACRO_REGEX_CONDITION_SKILL_CD =
    "skill_cd:([\u4e00-\u9fa5·0-9a-z]+)([=~<>]+)(\\d+)";

const char *const JX3DPS_MACRO_REGEX_FRAME = "(\\d{1,2}):(\\d{1,2}).?(\\d{0,2})";

JX3DPS::Id_t GetSkillId(const std::string &skillName)
{
    for (auto &skill : JX3DPS::Skill::SKILL_NAME) {
        if (skill.second == skillName) {
            return skill.first;
        }
    }
    return JX3DPS::JX3DPS_ERROR_INVALID_MACRO;
}

JX3DPS::Id_t GetBuffId(const std::string &buffName)
{
    for (auto &skill : JX3DPS::Buff::BUFF_NAME) {
        if (skill.second == buffName) {
            return skill.first;
        }
    }
    return JX3DPS::JX3DPS_ERROR_INVALID_MACRO;
}

void JX3DPS::CastSkill(Player *player, Targets *targets, ExprSkills &exprSkills)
{
    for (auto &exprSkill : exprSkills) {
        // 先判断冷却、距离和读条条件
        bool precondition = false;
        for (auto &exprIf : exprSkill.first.front()) {
            precondition = exprIf(player, targets);
            if (precondition == false) {
                break;
            }
        }
        if (precondition == false) {
            continue; // 前置条件不满足，跳过此技能
        }

        // 判断技能施放条件
        for (auto it = std::next(exprSkill.first.begin()); it != exprSkill.first.end(); ++it) {
            bool flag = false;
            if (it->empty()) {
                flag = true;
            }
            for (auto &exprIf : *it) {
                flag = exprIf(player, targets);
                if (flag == false) {
                    break;
                }
            }
            if (flag) {
                spdlog::debug("Cast\t\tskill\t{}", player->skills[exprSkill.second]->GetName());
                player->skills[exprSkill.second]->Cast();
                break;
            }
        }
    }
}

JX3DPS::Error_t Parse2ExprIf(const std::string &str, JX3DPS::ExprIf &exprIf)
{
    std::regex regQidian(JX3DPS_MACRO_REGEX_CONDITION_QIDIAN);
    std::regex regBuff(JX3DPS_MACRO_REGEX_CONDITION_BUFF);
    std::regex regNoBuff(JX3DPS_MACRO_REGEX_CONDITION_NOBUFF);
    std::regex regBuffStackNum(JX3DPS_MACRO_REGEX_CONDITION_BUFF_LAYERS);
    std::regex regBuffTime(JX3DPS_MACRO_REGEX_CONDITION_BUFFTIME);
    std::regex regTbuff(JX3DPS_MACRO_REGEX_CONDITION_TBUFF);
    std::regex regTNoBuff(JX3DPS_MACRO_REGEX_CONDITION_TNOBUFF);
    std::regex regTbuffStackNum(JX3DPS_MACRO_REGEX_CONDITION_TBUFF_LAYERS);
    std::regex regTbuffTime(JX3DPS_MACRO_REGEX_CONDITION_TBUFFTIME);
    std::regex regLastCastSkill(JX3DPS_MACRO_REGEX_CONDITION_LAST_CAST_SKILL);
    std::regex regSkillCd(JX3DPS_MACRO_REGEX_CONDITION_SKILL_CD);

    std::smatch mat;
    if (std::regex_match(str, mat, regQidian)) {
        if (mat[1].str() == "<") {
            exprIf = std::bind(&JX3DPS::Macro::QidianLt,
                               std::placeholders::_1,
                               std::placeholders::_2,
                               std::stoi(mat[2].str()));
        } else if (mat[1].str() == "<=") {
            exprIf = std::bind(&JX3DPS::Macro::QidianLe,
                               std::placeholders::_1,
                               std::placeholders::_2,
                               std::stoi(mat[2].str()));
        } else if (mat[1].str() == "=") {
            exprIf = std::bind(&JX3DPS::Macro::QidianEq,
                               std::placeholders::_1,
                               std::placeholders::_2,
                               std::stoi(mat[2].str()));
        } else if (mat[1].str() == "~=") {
            exprIf = std::bind(&JX3DPS::Macro::QidianNe,
                               std::placeholders::_1,
                               std::placeholders::_2,
                               std::stoi(mat[2].str()));
        } else if (mat[1].str() == ">=") {
            exprIf = std::bind(&JX3DPS::Macro::QidianGe,
                               std::placeholders::_1,
                               std::placeholders::_2,
                               std::stoi(mat[2].str()));
        } else if (mat[1].str() == ">") {
            exprIf = std::bind(&JX3DPS::Macro::QidianGt,
                               std::placeholders::_1,
                               std::placeholders::_2,
                               std::stoi(mat[2].str()));
        }
    } else if (std::regex_match(str, mat, regBuff)) {
        JX3DPS::Id_t id = GetBuffId(mat[1].str());
        exprIf = std::bind(&JX3DPS::Macro::BuffExist, std::placeholders::_1, std::placeholders::_2, id);
    } else if (std::regex_match(str, mat, regNoBuff)) {
        JX3DPS::Id_t id = GetBuffId(mat[1].str());
        exprIf = std::bind(&JX3DPS::Macro::NoBuffExist, std::placeholders::_1, std::placeholders::_2, id);
    } else if (std::regex_match(str, mat, regBuffStackNum)) {
        JX3DPS::Id_t id = GetBuffId(mat[1].str());
        if (mat[2].str() == "<") {
            exprIf = std::bind(&JX3DPS::Macro::BuffStackNumLt,
                               std::placeholders::_1,
                               std::placeholders::_2,
                               id,
                               std::stoi(mat[3].str()));
        } else if (mat[2].str() == "<=") {
            exprIf = std::bind(&JX3DPS::Macro::BuffStackNumLe,
                               std::placeholders::_1,
                               std::placeholders::_2,
                               id,
                               std::stoi(mat[3].str()));
        } else if (mat[2].str() == "=") {
            exprIf = std::bind(&JX3DPS::Macro::BuffStackNumEq,
                               std::placeholders::_1,
                               std::placeholders::_2,
                               id,
                               std::stoi(mat[3].str()));
        } else if (mat[2].str() == "~=") {
            exprIf = std::bind(&JX3DPS::Macro::BuffStackNumNe,
                               std::placeholders::_1,
                               std::placeholders::_2,
                               id,
                               std::stoi(mat[3].str()));
        } else if (mat[2].str() == ">=") {
            exprIf = std::bind(&JX3DPS::Macro::BuffStackNumGe,
                               std::placeholders::_1,
                               std::placeholders::_2,
                               id,
                               std::stoi(mat[3].str()));
        } else if (mat[2].str() == ">") {
            exprIf = std::bind(&JX3DPS::Macro::BuffStackNumGt,
                               std::placeholders::_1,
                               std::placeholders::_2,
                               id,
                               std::stoi(mat[3].str()));
        }
    } else if (std::regex_match(str, mat, regBuffTime)) {
        JX3DPS::Id_t id = GetBuffId(mat[1].str());
        if (mat[2].str() == "<") {
            exprIf = std::bind(&JX3DPS::Macro::BuffTimeLt,
                               std::placeholders::_1,
                               std::placeholders::_2,
                               id,
                               std::stoi(mat[3].str()) * JX3DPS::JX3_FRAMES_PER_SECOND);
        } else if (mat[2].str() == "<=") {
            exprIf = std::bind(&JX3DPS::Macro::BuffTimeLe,
                               std::placeholders::_1,
                               std::placeholders::_2,
                               id,
                               std::stoi(mat[3].str()) * JX3DPS::JX3_FRAMES_PER_SECOND);
        } else if (mat[2].str() == "=") {
            exprIf = std::bind(&JX3DPS::Macro::BuffTimeEq,
                               std::placeholders::_1,
                               std::placeholders::_2,
                               id,
                               std::stoi(mat[3].str()) * JX3DPS::JX3_FRAMES_PER_SECOND);
        } else if (mat[2].str() == "~=") {
            exprIf = std::bind(&JX3DPS::Macro::BuffTimeNe,
                               std::placeholders::_1,
                               std::placeholders::_2,
                               id,
                               std::stoi(mat[3].str()) * JX3DPS::JX3_FRAMES_PER_SECOND);
        } else if (mat[2].str() == ">=") {
            exprIf = std::bind(&JX3DPS::Macro::BuffTimeGe,
                               std::placeholders::_1,
                               std::placeholders::_2,
                               id,
                               std::stoi(mat[3].str()) * JX3DPS::JX3_FRAMES_PER_SECOND);
        } else if (mat[2].str() == ">") {
            exprIf = std::bind(&JX3DPS::Macro::BuffTimeGt,
                               std::placeholders::_1,
                               std::placeholders::_2,
                               id,
                               std::stoi(mat[3].str()) * JX3DPS::JX3_FRAMES_PER_SECOND);
        }
    } else if (std::regex_match(str, mat, regTbuff)) {
        JX3DPS::Id_t id = GetBuffId(mat[1].str());
        exprIf = std::bind(&JX3DPS::Macro::TBuffExist, std::placeholders::_1, std::placeholders::_2, id);
    } else if (std::regex_match(str, mat, regTNoBuff)) {
        JX3DPS::Id_t id = GetBuffId(mat[1].str());
        exprIf = std::bind(&JX3DPS::Macro::TNoBuffExist, std::placeholders::_1, std::placeholders::_2, id);
    } else if (std::regex_match(str, mat, regTbuffStackNum)) {
        JX3DPS::Id_t id = GetBuffId(mat[1].str());
        if (mat[2].str() == "<") {
            exprIf = std::bind(&JX3DPS::Macro::TBuffStackNumLt,
                               std::placeholders::_1,
                               std::placeholders::_2,
                               id,
                               std::stoi(mat[3].str()));
        } else if (mat[2].str() == "<=") {
            exprIf = std::bind(&JX3DPS::Macro::TBuffStackNumLe,
                               std::placeholders::_1,
                               std::placeholders::_2,
                               id,
                               std::stoi(mat[3].str()));
        } else if (mat[2].str() == "=") {
            exprIf = std::bind(&JX3DPS::Macro::TBuffStackNumEq,
                               std::placeholders::_1,
                               std::placeholders::_2,
                               id,
                               std::stoi(mat[3].str()));
        } else if (mat[2].str() == "~=") {
            exprIf = std::bind(&JX3DPS::Macro::TBuffStackNumNe,
                               std::placeholders::_1,
                               std::placeholders::_2,
                               id,
                               std::stoi(mat[3].str()));
        } else if (mat[2].str() == ">=") {
            exprIf = std::bind(&JX3DPS::Macro::TBuffStackNumGe,
                               std::placeholders::_1,
                               std::placeholders::_2,
                               id,
                               std::stoi(mat[3].str()));
        } else if (mat[2].str() == ">") {
            exprIf = std::bind(&JX3DPS::Macro::TBuffStackNumGt,
                               std::placeholders::_1,
                               std::placeholders::_2,
                               id,
                               std::stoi(mat[3].str()));
        }
    } else if (std::regex_match(str, mat, regTbuffTime)) {
        JX3DPS::Id_t id = GetBuffId(mat[1].str());
        if (mat[2].str() == "<") {
            exprIf = std::bind(&JX3DPS::Macro::TBuffTimeLt,
                               std::placeholders::_1,
                               std::placeholders::_2,
                               id,
                               std::stoi(mat[3].str()) * JX3DPS::JX3_FRAMES_PER_SECOND);
        } else if (mat[2].str() == "<=") {
            exprIf = std::bind(&JX3DPS::Macro::TBuffTimeLe,
                               std::placeholders::_1,
                               std::placeholders::_2,
                               id,
                               std::stoi(mat[3].str()) * JX3DPS::JX3_FRAMES_PER_SECOND);
        } else if (mat[2].str() == "=") {
            exprIf = std::bind(&JX3DPS::Macro::TBuffTimeEq,
                               std::placeholders::_1,
                               std::placeholders::_2,
                               id,
                               std::stoi(mat[3].str()) * JX3DPS::JX3_FRAMES_PER_SECOND);
        } else if (mat[2].str() == "~=") {
            exprIf = std::bind(&JX3DPS::Macro::TBuffTimeNe,
                               std::placeholders::_1,
                               std::placeholders::_2,
                               id,
                               std::stoi(mat[3].str()) * JX3DPS::JX3_FRAMES_PER_SECOND);
        } else if (mat[2].str() == ">=") {
            exprIf = std::bind(&JX3DPS::Macro::TBuffTimeGe,
                               std::placeholders::_1,
                               std::placeholders::_2,
                               id,
                               std::stoi(mat[3].str()) * JX3DPS::JX3_FRAMES_PER_SECOND);
        } else if (mat[2].str() == ">") {
            exprIf = std::bind(&JX3DPS::Macro::TBuffTimeGt,
                               std::placeholders::_1,
                               std::placeholders::_2,
                               id,
                               std::stoi(mat[3].str()) * JX3DPS::JX3_FRAMES_PER_SECOND);
        }
    } else if (std::regex_match(str, mat, regLastCastSkill)) {
        JX3DPS::Id_t id = GetSkillId(mat[1].str());
        if (mat[1].str() == "=") {
            exprIf = std::bind(&JX3DPS::Macro::LastCastSkill, std::placeholders::_1, std::placeholders::_2, id);
        } else if (mat[1].str() == "~=") {
            exprIf = std::bind(&JX3DPS::Macro::NotLastCastSkill, std::placeholders::_1, std::placeholders::_2, id);
        }
    } else if (std::regex_match(str, mat, regSkillCd)) {
        JX3DPS::Id_t id = GetSkillId(mat[1].str());
        if (mat[2].str() == "<") {
            exprIf = std::bind(&JX3DPS::Macro::SkillCooldownLt,
                               std::placeholders::_1,
                               std::placeholders::_2,
                               id,
                               std::stoi(mat[3].str()));
        } else if (mat[2].str() == "<=") {
            exprIf = std::bind(&JX3DPS::Macro::SkillCooldownLe,
                               std::placeholders::_1,
                               std::placeholders::_2,
                               id,
                               std::stoi(mat[3].str()));
        } else if (mat[2].str() == "=") {
            exprIf = std::bind(&JX3DPS::Macro::SkillCooldownEq,
                               std::placeholders::_1,
                               std::placeholders::_2,
                               id,
                               std::stoi(mat[3].str()));
        } else if (mat[2].str() == "~=") {
            exprIf = std::bind(&JX3DPS::Macro::SkillCooldownNe,
                               std::placeholders::_1,
                               std::placeholders::_2,
                               id,
                               std::stoi(mat[3].str()));
        } else if (mat[2].str() == ">=") {
            exprIf = std::bind(&JX3DPS::Macro::SkillCooldownGe,
                               std::placeholders::_1,
                               std::placeholders::_2,
                               id,
                               std::stoi(mat[3].str()));
        } else if (mat[2].str() == ">") {
            exprIf = std::bind(&JX3DPS::Macro::SkillCooldownGt,
                               std::placeholders::_1,
                               std::placeholders::_2,
                               id,
                               std::stoi(mat[3].str()));
        }
    } else {
        return JX3DPS::JX3DPS_ERROR_INVALID_MACRO;
    }
    return JX3DPS::JX3DPS_SUCCESS;
}

JX3DPS::Error_t Parse2Cast(const std::string &str, JX3DPS::ExprSkill &exprSkill)
{
    std::regex  reg(JX3DPS_MACRO_REGEX_CAST);
    std::smatch mat;
    if (!std::regex_match(str, mat, reg)) {
        return JX3DPS::JX3DPS_ERROR_INVALID_MACRO;
    }

    std::string cast      = mat[1].str();
    std::string condition = mat[2].str();
    std::string skillName = mat[3].str();

    std::list<JX3DPS::ExprIf> exprIfList;

    // 添加冷却判断
    exprIfList.emplace_back(
        std::bind(&JX3DPS::Macro::IsCooldown, std::placeholders::_1, std::placeholders::_2, GetSkillId(skillName)));
    // 添加是否在范围内判断
    exprIfList.emplace_back(
        std::bind(&JX3DPS::Macro::IsTargetWithinRange, std::placeholders::_1, std::placeholders::_2, GetSkillId(skillName)));

    // 添加是否在读条判断
    if (cast == std::string(JX3DPS_MACRO_TYPE_CAST)) {
        exprIfList.emplace_back(std::bind(&JX3DPS::Macro::IsNotReCast, std::placeholders::_1, std::placeholders::_2));
    } else {
        exprIfList.emplace_back(std::bind(&JX3DPS::Macro::IsNotCast, std::placeholders::_1, std::placeholders::_2));
    }

    if (!condition.empty()) {
        std::string    temp;
        std::string    temp2;
        JX3DPS::ExprIf exprIf;
        while (condition.find(JX3DPS_MACRO_SYMBOL_OR) != std::string::npos) {
            temp = condition.substr(0, condition.find(JX3DPS_MACRO_SYMBOL_OR));
            std::list<JX3DPS::ExprIf> exprIfs;
            while (temp.find(JX3DPS_MACRO_SYMBOL_AND) != std::string::npos) {
                temp2 = temp.substr(0, temp.find(JX3DPS_MACRO_SYMBOL_AND));
                if (JX3DPS::Error_t err = Parse2ExprIf(temp2, exprIf) != JX3DPS::JX3DPS_SUCCESS) {
                    return err;
                }
                exprIfs.emplace_back(exprIf);
                temp = temp.substr(temp.find(JX3DPS_MACRO_SYMBOL_AND) + 1);
            }
            if (JX3DPS::Error_t err = Parse2ExprIf(temp, exprIf) != JX3DPS::JX3DPS_SUCCESS) {
                return err;
            }
            exprIfs.emplace_back(exprIf);
            exprSkill.first.emplace_back(exprIfs);
            condition = condition.substr(condition.find(JX3DPS_MACRO_SYMBOL_OR) + 1);
        }
        std::list<JX3DPS::ExprIf> exprIfs;
        while (condition.find(JX3DPS_MACRO_SYMBOL_AND) != std::string::npos) {
            temp2 = condition.substr(0, condition.find(JX3DPS_MACRO_SYMBOL_AND));
            if (JX3DPS::Error_t err = Parse2ExprIf(temp2, exprIf) != JX3DPS::JX3DPS_SUCCESS) {
                return err;
            }
            exprIfs.emplace_back(exprIf);
            condition = condition.substr(condition.find(JX3DPS_MACRO_SYMBOL_AND) + 1);
        }
        if (JX3DPS::Error_t err = Parse2ExprIf(condition, exprIf) != JX3DPS::JX3DPS_SUCCESS) {
            return err;
        }
        exprIfs.emplace_back(exprIf);
        exprSkill.first.emplace_back(exprIfs);
    } else {
        exprSkill.first.push_back(std::list<JX3DPS::ExprIf>{});
    }
    exprSkill.first.emplace_front(exprIfList);
    if (GetSkillId(skillName) == JX3DPS::JX3DPS_ERROR_INVALID_MACRO) {
        return JX3DPS::JX3DPS_ERROR_INVALID_MACRO;
    }
    exprSkill.second = GetSkillId(skillName);

    return JX3DPS::JX3DPS_SUCCESS_SKILL;
}

JX3DPS::Frame_t Parse2Frame(const std::string &str)
{
    std::regex  reg(JX3DPS_MACRO_REGEX_FRAME);
    std::smatch mat;
    if (!std::regex_match(str, mat, reg)) {
        return JX3DPS::JX3DPS_ERROR_INVALID_MACRO;
    }
    int frame = 0;
    if (!mat[3].str().empty()) {
        frame = std::stoi(mat[3].str());
    }
    return (std::stoi(mat[1].str()) * JX3DPS_SECOND_PER_MINUTE + std::stoi(mat[2].str())) * JX3DPS::JX3_FRAMES_PER_SECOND +
           frame;
}

JX3DPS::Error_t Parse2EventCast(const std::string &str, JX3DPS::ExprEvent &exprEvent, JX3DPS::ExprSkill &exprSkill)
{

    std::regex  reg(JX3DPS_MACRO_REGEX);
    std::smatch mat;
    if (!std::regex_match(str, mat, reg)) {
        return JX3DPS::JX3DPS_ERROR_INVALID_MACRO;
    }

    std::string       strSkill = mat[3].str();
    JX3DPS::ExprSkill exprEventSkill;
    JX3DPS::Error_t   err = Parse2Cast(strSkill, exprEventSkill);
    if (JX3DPS::JX3DPS_SUCCESS_SKILL != err) {
        return err;
    }
    exprEvent.first = Parse2Frame(mat[1].str());

    exprEvent.second =
        std::bind(&JX3DPS::Macro::CastSkill, std::placeholders::_1, std::placeholders::_2, exprEventSkill);

    exprSkill.second = exprEventSkill.second;
    exprSkill.first.emplace_back(std::list<JX3DPS::ExprIf>{
        std::bind(&JX3DPS::Macro::NeverCast, std::placeholders::_1, std::placeholders::_2) });

    return JX3DPS::JX3DPS_SUCCESS_EVENT_SKILL;
}

JX3DPS::Error_t Parse2SetTarget(const std::string &str, JX3DPS::ExprEvent &exprEvent)
{
    std::regex  reg(JX3DPS_MACRO_REGEX_SET_TARGET);
    std::smatch mat;
    if (!std::regex_match(str, mat, reg)) {
        std::regex reg(JX3DPS_MACRO_REGEX_SET_TARGET_DEAD);
        if (!std::regex_match(str, mat, reg)) {
            return JX3DPS::JX3DPS_ERROR_INVALID_MACRO;
        } else {
            exprEvent.first       = Parse2Frame(mat[1].str());
            JX3DPS::Id_t targetId = std::stoi(mat[2].str());
            exprEvent.second =
                std::bind(&JX3DPS::Macro::SetTargetDead, std::placeholders::_1, std::placeholders::_2, targetId);
            return JX3DPS::JX3DPS_SUCCESS_EVENT;
        }
    }

    exprEvent.first       = Parse2Frame(mat[1].str());
    JX3DPS::Id_t targetId = std::stoi(mat[2].str());
    if (mat[4].str().empty()) {
        exprEvent.second =
            std::bind(&JX3DPS::Macro::SetTarget, std::placeholders::_1, std::placeholders::_2, targetId);
        return JX3DPS::JX3DPS_SUCCESS_EVENT;
    } else {
        if (mat[4].str() == "life") {
            exprEvent.second =
                std::bind(&JX3DPS::Macro::SetTargetLife,
                          std::placeholders::_1,
                          std::placeholders::_2,
                          targetId,
                          std::stof(mat[5].str()));
        } else if (mat[4].str() == "mana") {
            exprEvent.second =
                std::bind(&JX3DPS::Macro::SetTargetMana,
                          std::placeholders::_1,
                          std::placeholders::_2,
                          targetId,
                          std::stof(mat[5].str()));
        } else if (mat[4].str() == "distance") {
            exprEvent.second =
                std::bind(&JX3DPS::Macro::SetTargetDistance,
                          std::placeholders::_1,
                          std::placeholders::_2,
                          targetId,
                          std::stof(mat[5].str()));
        } else {
            return JX3DPS::JX3DPS_ERROR_INVALID_MACRO;
        }
    }

    return JX3DPS::JX3DPS_SUCCESS_EVENT;
}

JX3DPS::Error_t Parse2AddBuff(const std::string &str, JX3DPS::ExprEvent &exprEvent)
{
    std::regex  reg(JX3DPS_MACRO_REGEX_ADD_BUFF);
    std::smatch mat;
    if (!std::regex_match(str, mat, reg)) {
        return JX3DPS::JX3DPS_ERROR_INVALID_MACRO;
    }
    exprEvent.first = Parse2Frame(mat[1].str());

    JX3DPS::Id_t id     = std::stoi(mat[2].str());
    JX3DPS::Id_t buffId = GetBuffId(mat[3].str());

    int stackNum = 1;
    if (!mat[5].str().empty()) {
        stackNum = std::stoi(mat[5].str());
    }

    JX3DPS::Frame_t duration = JX3DPS::JX3DPS_DEFAULT_DURATION_FRAMES;
    if (!mat[7].str().empty()) {
        duration = std::stoi(mat[7].str()) * JX3DPS::JX3_FRAMES_PER_SECOND;
    }

    exprEvent.second =
        std::bind(&JX3DPS::Macro::AddBuff, std::placeholders::_1, std::placeholders::_2, id, buffId, stackNum, duration);

    return JX3DPS::JX3DPS_SUCCESS_EVENT;
}

JX3DPS::Error_t Parse2ClearBuff(const std::string &str, JX3DPS::ExprEvent &exprEvent)
{
    std::regex  reg(JX3DPS_MACRO_REGEX_CLEAR_BUFF);
    std::smatch mat;
    if (!std::regex_match(str, mat, reg)) {
        return JX3DPS::JX3DPS_ERROR_INVALID_MACRO;
    }
    exprEvent.first = Parse2Frame(mat[1].str());

    JX3DPS::Id_t id       = std::stoi(mat[2].str());
    JX3DPS::Id_t buffId   = GetBuffId(mat[3].str());
    int          stackNum = std::stoi(mat[5].str());

    exprEvent.second =
        std::bind(&JX3DPS::Macro::ClearBuff, std::placeholders::_1, std::placeholders::_2, id, buffId, stackNum);

    return JX3DPS::JX3DPS_SUCCESS_EVENT;
}

JX3DPS::Error_t Parse2Stop(const std::string &str, JX3DPS::ExprEvent &exprEvent)
{
    std::regex  reg(JX3DPS_MACRO_REGEX_STOP);
    std::smatch mat;
    if (!std::regex_match(str, mat, reg)) {
        return JX3DPS::JX3DPS_ERROR_INVALID_MACRO;
    }
    exprEvent.first = Parse2Frame(mat[1].str());
    exprEvent.second =
        std::bind(&JX3DPS::Macro::SetPlayerStop, std::placeholders::_1, std::placeholders::_2, true);
    return JX3DPS::JX3DPS_SUCCESS_EVENT;
}

JX3DPS::Error_t Parse2Continue(const std::string &str, JX3DPS::ExprEvent &exprEvent)
{
    std::regex  reg(JX3DPS_MACRO_REGEX_CONTINUE);
    std::smatch mat;
    if (!std::regex_match(str, mat, reg)) {
        return JX3DPS::JX3DPS_ERROR_INVALID_MACRO;
    }
    exprEvent.first = Parse2Frame(mat[1].str());
    exprEvent.second =
        std::bind(&JX3DPS::Macro::SetPlayerStop, std::placeholders::_1, std::placeholders::_2, false);
    return JX3DPS::JX3DPS_SUCCESS_EVENT;
}

JX3DPS::Error_t Parse2End(const std::string &str, JX3DPS::ExprEvent &exprEvent)
{
    std::regex  reg(JX3DPS_MACRO_REGEX_END);
    std::smatch mat;
    if (!std::regex_match(str, mat, reg)) {
        return JX3DPS::JX3DPS_ERROR_INVALID_MACRO;
    }
    exprEvent.first = Parse2Frame(mat[1].str());
    exprEvent.second = std::bind(&JX3DPS::Macro::SetEnd, std::placeholders::_1, std::placeholders::_2);
    return JX3DPS::JX3DPS_SUCCESS_EVENT;
}

JX3DPS::Error_t Parse2AddTarget(const std::string &str, JX3DPS::ExprEvent &exprEvent)
{
    std::regex  reg(JX3DPS_MACRO_REGEX_ADD_TARGET);
    std::smatch mat;
    if (!std::regex_match(str, mat, reg)) {
        return JX3DPS::JX3DPS_ERROR_INVALID_MACRO;
    }

    exprEvent.first          = Parse2Frame(mat[1].str());
    JX3DPS::Id_t    id       = std::stoi(mat[2].str());
    int             level    = std::stoi(mat[3].str());
    JX3DPS::Value_t shield   = std::stoi(mat[4].str());
    float           distance = std::stof(mat[5].str());

    exprEvent.second =
        std::bind(&JX3DPS::Macro::AddTarget, std::placeholders::_1, std::placeholders::_2, id, level, shield, distance);
    return JX3DPS::JX3DPS_SUCCESS_EVENT;
}

JX3DPS::Error_t JX3DPS::ParseMacro(const std::string &str, ExprEvent &exprEvent, ExprSkill &exprSkill)
{
    std::regex  reg(JX3DPS_MACRO_REGEX);
    std::smatch mat;
    if (!std::regex_match(str, mat, reg)) {
        return JX3DPS_ERROR_INVALID_MACRO;
    }
    if (mat[2].str() == JX3DPS_MACRO_TYPE_CAST || mat[2].str() == JX3DPS_MACRO_TYPE_FCAST) {
        if (mat[1].str().empty()) { // 技能宏
            return Parse2Cast(str, exprSkill);
        } else {                    // 事件里的技能宏
            return Parse2EventCast(str, exprEvent, exprSkill);
        }
    } else if (mat[2].str() == JX3DPS_MACRO_TYPE_ADD_TARGET) {
        return Parse2AddTarget(str, exprEvent);
    } else if (mat[2].str() == JX3DPS_MACRO_TYPE_SET_TARGET) {
        return Parse2SetTarget(str, exprEvent);
    } else if (mat[2].str() == JX3DPS_MACRO_TYPE_ADD_BUFF) {
        return Parse2AddBuff(str, exprEvent);
    } else if (mat[2].str() == JX3DPS_MACRO_TYPE_CLEAR_BUFF) {
        return Parse2ClearBuff(str, exprEvent);
    } else if (mat[2].str() == JX3DPS_MACRO_TYPE_STOP) {
        return Parse2Stop(str, exprEvent);
    } else if (mat[2].str() == JX3DPS_MACRO_TYPE_CONTINUE) {
        return Parse2Continue(str, exprEvent);
    } else if (mat[2].str() == JX3DPS_MACRO_TYPE_END) {
        return Parse2End(str, exprEvent);
    }
    return JX3DPS_ERROR_INVALID_MACRO_TYPE;
}

bool JX3DPS::Macro::CastSkill(Player *player, Targets *targets, ExprSkill &exprSkill)
{
    // 先判断cd、距离和读条条件
    for (auto &exprIf : exprSkill.first.front()) {
        if (exprIf(player, targets) == false) {
            return false;
        }
    }

    for (auto it = std::next(exprSkill.first.begin()); it != exprSkill.first.end(); ++it) {
        bool flag = false;
        if (it->empty()) {
            flag = true;
        }
        for (auto &exprIf : *it) {
            flag = exprIf(player, targets);
            if (flag == false) {
                break;
            }
        }
        if (flag) {
            player->skills.at(exprSkill.second)->Cast();
            return true;
        }
    }
    return false;
}

bool JX3DPS::Macro::NeverCast(Player *player, Targets *targets)
{
    return false;
}

bool JX3DPS::Macro::AddTarget(Player *player, Targets *targets, Id_t targetId, int level, Value_t shield, float distance)
{
    std::unique_ptr<Target> target = std::make_unique<Target>();
    target->SetId(targetId);
    target->SetLevel(level);
    target->SetShieldIgnorePercentInt(player->attr->GetShieldIgnorePercentInt());
    target->SetPhysicsShield(shield);
    target->SetMagicShield(shield);
    target->SetDistance(distance);
    targets->insert(std::pair<Id_t, std::unique_ptr<Target>>(targetId, std::move(target)));
    return true;
}

bool JX3DPS::Macro::SetTarget(Player *player, Targets *targets, Id_t targetId)
{
    player->targetId = targetId;
    return true;
}

bool JX3DPS::Macro::SetTargetLife(Player *player, Targets *targets, Id_t targetId, PctFloat_t life)
{
    targets->at(targetId)->SetLifePercent(life);
    return true;
}

bool JX3DPS::Macro::SetTargetMana(Player *player, Targets *targets, Id_t targetId, PctFloat_t mana)
{
    targets->at(targetId)->SetManaPercent(mana);
    return true;
}

bool JX3DPS::Macro::SetTargetDistance(Player *player, Targets *targets, Id_t targetId, float distance)
{
    targets->at(targetId)->SetDistance(distance);
    return true;
}

bool JX3DPS::Macro::SetTargetDead(Player *player, Targets *targets, Id_t targetId)
{
    targets->erase(targetId);
    for (auto &buff : player->buffs) {
        buff.second->RemoveTarget(targetId);
    }
    return true;
}

bool JX3DPS::Macro::SetPlayerLife(Player *player, Targets *targets, Id_t targetId, PctFloat_t life)
{
    player->life = life;
    return true;
}

bool JX3DPS::Macro::SetPlayerMana(Player *player, Targets *targets, Id_t targetId, PctFloat_t mana)
{
    player->mana = mana;
    return true;
}

bool JX3DPS::Macro::SetPlayerStop(Player *player, Targets *targets, bool stop)
{
    player->stop = stop;
    return true;
}

bool JX3DPS::Macro::SetEnd(Player *player, Targets *targets)
{
    player->end = true;
    return true;
}

bool JX3DPS::Macro::AddBuff(Player *player, Targets *targets, Id_t id, Id_t buffId, int stackNum, Frame_t duration)
{
    player->buffs[buffId]->Add(id, stackNum, duration);
    return true;
}

bool JX3DPS::Macro::ClearBuff(Player *player, Targets *targets, Id_t id, Id_t buffId, int stackNum)
{
    player->buffs[buffId]->Clear(id, stackNum);
    return true;
}

bool JX3DPS::Macro::IsTargetWithinRange(Player *player, Targets *targets, Id_t skillId)
{
    float range = player->skills[skillId]->GetRange();
    return targets->at(player->targetId)->GetDistance() <= range;
}

bool JX3DPS::Macro::IsNotReCast(Player *player, Targets *targets)
{
    if (player->isReCast) {

        
        return false;
    }
    return !player->isReCast;
}

bool JX3DPS::Macro::IsNotCast(Player *player, Targets *targets)
{
    return !player->isCast;
}

bool JX3DPS::Macro::IsCooldown(Player *player, Targets *targets, Id_t skillId)
{
    return player->skills[skillId]->GetCooldown() == 0;
}

bool JX3DPS::Macro::BuffExist(Player *player, Targets *targets, Id_t id)
{
    return player->buffs.at(id)->GetTimeLeft() != 0;
}

bool JX3DPS::Macro::NoBuffExist(Player *player, Targets *targets, Id_t id)
{
    return player->buffs.at(id)->GetTimeLeft() == 0;
}

bool JX3DPS::Macro::TBuffExist(Player *player, Targets *targets, Id_t id)
{
    return player->buffs.at(id)->GetTimeLeft(player->targetId) != 0;
}

bool JX3DPS::Macro::TNoBuffExist(Player *player, Targets *targets, Id_t id)
{
    return player->buffs.at(id)->GetTimeLeft(player->targetId) == 0;
}

bool JX3DPS::Macro::LastCastSkill(Player *player, Targets *targets, Id_t id)
{
    return player->lastCastSkill == id;
}

bool JX3DPS::Macro::NotLastCastSkill(Player *player, Targets *targets, Id_t id)
{
    return player->lastCastSkill != id;
}

bool JX3DPS::Macro::BuffStackNumLt(Player *player, Targets *targets, Id_t id, int stackNum)
{
    return player->buffs.at(id)->GetStackNum() < stackNum;
}

bool JX3DPS::Macro::BuffStackNumLe(Player *player, Targets *targets, Id_t id, int stackNum)
{
    return player->buffs.at(id)->GetStackNum() <= stackNum;
}

bool JX3DPS::Macro::BuffStackNumEq(Player *player, Targets *targets, Id_t id, int stackNum)
{
    return player->buffs.at(id)->GetStackNum() == stackNum;
}

bool JX3DPS::Macro::BuffStackNumNe(Player *player, Targets *targets, Id_t id, int stackNum)
{
    return player->buffs.at(id)->GetStackNum() != stackNum;
}

bool JX3DPS::Macro::BuffStackNumGe(Player *player, Targets *targets, Id_t id, int stackNum)
{
    return player->buffs.at(id)->GetStackNum() >= stackNum;
}

bool JX3DPS::Macro::BuffStackNumGt(Player *player, Targets *targets, Id_t id, int stackNum)
{
    return player->buffs.at(id)->GetStackNum() > stackNum;
}

bool JX3DPS::Macro::TBuffStackNumLt(Player *player, Targets *targets, Id_t id, int stackNum)
{
    return player->buffs.at(id)->GetStackNum(player->targetId) < stackNum;
}

bool JX3DPS::Macro::TBuffStackNumLe(Player *player, Targets *targets, Id_t id, int stackNum)
{
    return player->buffs.at(id)->GetStackNum(player->targetId) <= stackNum;
}

bool JX3DPS::Macro::TBuffStackNumEq(Player *player, Targets *targets, Id_t id, int stackNum)
{
    return player->buffs.at(id)->GetStackNum(player->targetId) == stackNum;
}

bool JX3DPS::Macro::TBuffStackNumNe(Player *player, Targets *targets, Id_t id, int stackNum)
{
    return player->buffs.at(id)->GetStackNum(player->targetId) != stackNum;
}

bool JX3DPS::Macro::TBuffStackNumGe(Player *player, Targets *targets, Id_t id, int stackNum)
{
    return player->buffs.at(id)->GetStackNum(player->targetId) >= stackNum;
}

bool JX3DPS::Macro::TBuffStackNumGt(Player *player, Targets *targets, Id_t id, int stackNum)
{
    return player->buffs.at(id)->GetStackNum(player->targetId) > stackNum;
}

bool JX3DPS::Macro::BuffTimeLt(Player *player, Targets *targets, Id_t id, Frame_t time)
{
    return player->buffs.at(id)->GetTimeLeft() < time;
}

bool JX3DPS::Macro::BuffTimeLe(Player *player, Targets *targets, Id_t id, Frame_t time)
{
    return player->buffs.at(id)->GetTimeLeft() <= time;
}

bool JX3DPS::Macro::BuffTimeEq(Player *player, Targets *targets, Id_t id, Frame_t time)
{
    return player->buffs.at(id)->GetTimeLeft() == time;
}

bool JX3DPS::Macro::BuffTimeNe(Player *player, Targets *targets, Id_t id, Frame_t time)
{
    return player->buffs.at(id)->GetTimeLeft() != time;
}

bool JX3DPS::Macro::BuffTimeGe(Player *player, Targets *targets, Id_t id, Frame_t time)
{
    return player->buffs.at(id)->GetTimeLeft() >= time;
}

bool JX3DPS::Macro::BuffTimeGt(Player *player, Targets *targets, Id_t id, Frame_t time)
{
    return player->buffs.at(id)->GetTimeLeft() > time;
}

bool JX3DPS::Macro::TBuffTimeLt(Player *player, Targets *targets, Id_t id, Frame_t time)
{
    return player->buffs.at(id)->GetTimeLeft(player->targetId) < time;
}

bool JX3DPS::Macro::TBuffTimeLe(Player *player, Targets *targets, Id_t id, Frame_t time)
{
    return player->buffs.at(id)->GetTimeLeft(player->targetId) <= time;
}

bool JX3DPS::Macro::TBuffTimeEq(Player *player, Targets *targets, Id_t id, Frame_t time)
{
    return player->buffs.at(id)->GetTimeLeft(player->targetId) == time;
}

bool JX3DPS::Macro::TBuffTimeNe(Player *player, Targets *targets, Id_t id, Frame_t time)
{
    return player->buffs.at(id)->GetTimeLeft(player->targetId) != time;
}

bool JX3DPS::Macro::TBuffTimeGe(Player *player, Targets *targets, Id_t id, Frame_t time)
{
    return player->buffs.at(id)->GetTimeLeft(player->targetId) >= time;
}

bool JX3DPS::Macro::TBuffTimeGt(Player *player, Targets *targets, Id_t id, Frame_t time)
{
    return player->buffs.at(id)->GetTimeLeft(player->targetId) > time;
}

bool JX3DPS::Macro::SkillCooldownLt(Player *player, Targets *targets, Id_t id, Frame_t time)
{
    return player->skills.at(id)->GetCooldown() < time;
}

bool JX3DPS::Macro::SkillCooldownLe(Player *player, Targets *targets, Id_t id, Frame_t time)
{
    return player->skills.at(id)->GetCooldown() <= time;
}

bool JX3DPS::Macro::SkillCooldownEq(Player *player, Targets *targets, Id_t id, Frame_t time)
{
    return player->skills.at(id)->GetCooldown() == time;
}

bool JX3DPS::Macro::SkillCooldownNe(Player *player, Targets *targets, Id_t id, Frame_t time)
{
    return player->skills.at(id)->GetCooldown() != time;
}

bool JX3DPS::Macro::SkillCooldownGe(Player *player, Targets *targets, Id_t id, Frame_t time)
{
    return player->skills.at(id)->GetCooldown() >= time;
}

bool JX3DPS::Macro::SkillCooldownGt(Player *player, Targets *targets, Id_t id, Frame_t time)
{
    return player->skills.at(id)->GetCooldown() > time;
}

bool JX3DPS::Macro::SkillEnergyLt(Player *player, Targets *targets, Id_t id, int energy)
{
    return player->skills.at(id)->GetEnergyCount() < energy;
}

bool JX3DPS::Macro::SkillEnergyLe(Player *player, Targets *targets, Id_t id, int energy)
{
    return player->skills.at(id)->GetEnergyCount() <= energy;
}

bool JX3DPS::Macro::SkillEnergyEq(Player *player, Targets *targets, Id_t id, int energy)
{
    return player->skills.at(id)->GetEnergyCount() == energy;
}

bool JX3DPS::Macro::SkillEnergyNe(Player *player, Targets *targets, Id_t id, int energy)
{
    return player->skills.at(id)->GetEnergyCount() != energy;
}

bool JX3DPS::Macro::SkillEnergyGe(Player *player, Targets *targets, Id_t id, int energy)
{
    return player->skills.at(id)->GetEnergyCount() >= energy;
}

bool JX3DPS::Macro::SkillEnergyGt(Player *player, Targets *targets, Id_t id, int energy)
{
    return player->skills.at(id)->GetEnergyCount() > energy;
}

bool JX3DPS::Macro::TLifeLt(Player *player, Targets *targets, Id_t id, double ratio)
{
    return (*targets)[player->targetId]->GetLifePercent() < ratio;
}

bool JX3DPS::Macro::TLifeLe(Player *player, Targets *targets, Id_t id, double ratio)
{
    return (*targets)[player->targetId]->GetLifePercent() <= ratio;
}

bool JX3DPS::Macro::TLifeEq(Player *player, Targets *targets, Id_t id, double ratio)
{
    return (*targets)[player->targetId]->GetLifePercent() == ratio;
}

bool JX3DPS::Macro::TLifeNe(Player *player, Targets *targets, Id_t id, double ratio)
{
    return (*targets)[player->targetId]->GetLifePercent() != ratio;
}

bool JX3DPS::Macro::TLifeGe(Player *player, Targets *targets, Id_t id, double ratio)
{
    return (*targets)[player->targetId]->GetLifePercent() >= ratio;
}

bool JX3DPS::Macro::TLifeGt(Player *player, Targets *targets, Id_t id, double ratio)
{
    return (*targets)[player->targetId]->GetLifePercent() > ratio;
}

bool JX3DPS::Macro::ManaLt(Player *player, Targets *targets, double ratio)
{
    return player->mana < ratio;
}

bool JX3DPS::Macro::ManaLe(Player *player, Targets *targets, double ratio)
{
    return player->mana <= ratio;
}

bool JX3DPS::Macro::ManaEq(Player *player, Targets *targets, double ratio)
{
    return player->mana == ratio;
}

bool JX3DPS::Macro::ManaNe(Player *player, Targets *targets, double ratio)
{
    return player->mana != ratio;
}

bool JX3DPS::Macro::ManaGe(Player *player, Targets *targets, double ratio)
{
    return player->mana >= ratio;
}

bool JX3DPS::Macro::ManaGt(Player *player, Targets *targets, double ratio)
{
    return player->mana > ratio;
}

bool JX3DPS::Macro::NearbyEnemyLt(Player *player, Targets *targets, double distance, int count)
{
    int temp = 0;
    for (auto &target : *targets) {
        if (target.second->GetDistance() < distance) {
            temp++;
        }
    }
    return temp < count;
}

bool JX3DPS::Macro::NearbyEnemyLe(Player *player, Targets *targets, double distance, int count)
{
    int temp = 0;
    for (auto &target : *targets) {
        if (target.second->GetDistance() < distance) {
            temp++;
        }
    }
    return temp <= count;
}

bool JX3DPS::Macro::NearbyEnemyEq(Player *player, Targets *targets, double distance, int count)
{
    int temp = 0;
    for (auto &target : *targets) {
        if (target.second->GetDistance() < distance) {
            temp++;
        }
    }
    return temp == count;
}

bool JX3DPS::Macro::NearbyEnemyNe(Player *player, Targets *targets, double distance, int count)
{
    int temp = 0;
    for (auto &target : *targets) {
        if (target.second->GetDistance() < distance) {
            temp++;
        }
    }
    return temp != count;
}

bool JX3DPS::Macro::NearbyEnemyGe(Player *player, Targets *targets, double distance, int count)
{
    int temp = 0;
    for (auto &target : *targets) {
        if (target.second->GetDistance() < distance) {
            temp++;
        }
    }
    return temp >= count;
}

bool JX3DPS::Macro::NearbyEnemyGt(Player *player, Targets *targets, double distance, int count)
{
    int temp = 0;
    for (auto &target : *targets) {
        if (target.second->GetDistance() < distance) {
            temp++;
        }
    }
    return temp > count;
}

bool JX3DPS::Macro::QidianLt(Player *player, Targets *targets, int count)
{
    return player->qidian < count;
}

bool JX3DPS::Macro::QidianLe(Player *player, Targets *targets, int count)
{
    return player->qidian <= count;
}

bool JX3DPS::Macro::QidianEq(Player *player, Targets *targets, int count)
{
    return player->qidian == count;
}

bool JX3DPS::Macro::QidianNe(Player *player, Targets *targets, int count)
{
    return player->qidian != count;
}

bool JX3DPS::Macro::QidianGe(Player *player, Targets *targets, int count)
{
    return player->qidian >= count;
}

bool JX3DPS::Macro::QidianGt(Player *player, Targets *targets, int count)
{
    return player->qidian > count;
}
