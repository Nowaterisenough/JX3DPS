/**
 * @Description : 宏解析
 * @Author      : NoWats
 * @Date        : 2022-02-02 16:33:58
 * @Update      : NoWats
 * @LastTime    : 2022-02-03 10:51:12
 * @FilePath    : \JX3DPS\modules\Core\Macro.cpp
 */

#include "Macro.h"

#include <sstream>
#include <regex>

using namespace std;

namespace JX3DPS {

Macro::~Macro() {}

Macro *Macro::Instance()
{
    static Macro instance;
    return &instance;
}

int Macro::Parse(const std::list<std::string> &lines)
{
    int sum = 0;
    for (const auto &line : lines) {
        sum += Parse(line);
    }
    return sum;
}

int Macro::Parse(const std::string &line)
{
    std::stringstream buf(line);
    std::string       castModeStr;
    std::string       conditionStr;
    std::string       skillName;
    
    buf >> castModeStr >> conditionStr >> skillName;
    // Id_t skillId = GetSkillId(skillName);

    std::list<std::string> orConditionStrList;
    while (conditionStr.find("|") != std::string::npos) {
        orConditionStrList.emplace_back(conditionStr.substr(0, conditionStr.find("|")));
        conditionStr = conditionStr.substr(conditionStr.find("|") + 1);
    }
    std::list<std::string> andConditionStrList;
    for (auto &orConditionStr : orConditionStrList) {
        while (orConditionStr.find("&") != std::string::npos) {
            andConditionStrList.emplace_back(orConditionStr.substr(0, orConditionStr.find("&")));
            orConditionStr = orConditionStr.substr(orConditionStr.find("&") + 1);
        }
    }
    return 0;
}

bool Macro::MacroStrToFuncPtr(const std::string &str)
{
    std::regex  reg("([a-z_]+):(.+)(>=|<=|~=|<|>|=)([0-9]+)");
    std::smatch mat;
    if (std::regex_search(str, mat, reg)) {
        std::string conditionType = mat[1];
        std::string effectName    = mat[2];
        std::string than          = mat[3];
        std::string numStr        = mat[4];
        // std
        // if (conditionType == "buff") {
        //     Id_t id                = GetBuffId(effectName);
        //     macroFunc.param.int1st = id;
        //     int num                = stoi(numStr);
        //     macroFunc.param.int2nd = num;
        //     if (than == "<") {
        //         macroFunc.macroFuncPtr = &Macro::BuffStackNumLt;
        //     } else if (than == "<=") {
        //         macroFunc.macroFuncPtr = &Macro::BuffStackNumLe;
        //     } else if (than == "=") {
        //         macroFunc.macroFuncPtr = &Macro::BuffStackNumEq;
        //     } else if (than == "~=") {
        //         macroFunc.macroFuncPtr = &Macro::BuffStackNumNe;
        //     } else if (than == ">=") {
        //         macroFunc.macroFuncPtr = &Macro::BuffStackNumGe;
        //     } else if (than == ">") {
        //         macroFunc.macroFuncPtr = &Macro::BuffStackNumGt;
        //     }
        // }
        return true;
    }
    return false;
}

Macro::Macro() {}

} // namespace JX3DPS
