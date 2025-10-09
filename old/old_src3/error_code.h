#ifndef JX3DPS_ERROR_CODE_H
#define JX3DPS_ERROR_CODE_H

#include <string_view>

#include "type.h"

namespace JX3DPS {

enum ErrorCode
{
    SUCCESS = 0,       // 成功
    INVALID_PARAMETER, // 无效参数
    INVALID_JSON,      // 无效JSON
    INVALID_VERSION,   // 无效版本
    INVALID_SKILL,     // 无效技能
    INVALID_BUFF,      // 无效BUFF
    INVALID_EQUIPMENT, // 无效装备
    INVALID_TALENT,    // 无效天赋
    INVALID_ENCHANT,   // 无效附魔

    COUNT,
};

using err_t = ErrorCode;

inline constexpr static array_t ERROR_CODE_MSG{
    "SUCCESS",           "INVALID_PARAMETER", "INVALID_JSON",
    "INVALID_VERSION",   "INVALID_SKILL",     "INVALID_BUFF",
    "INVALID_EQUIPMENT", "INVALID_TALENT",    "INVALID_ENCHANT"
};

} // namespace JX3DPS

#endif // JX3DPS_ERROR_CODE_H