#pragma once

#include <string_view>

namespace JX3DPS::engine {
constexpr std::string_view DefaultMacro(bool mo_wen)
{
    if (mo_wen) {
        return "# 莫问：持续伤害、影子与曲风循环\n/cast [tnobuff:商] 商\n/cast [tnobuff:角] 角\n/cast "
               "疏影横斜\n/cast 高山流水\n/cast [buff:曲风=4] 羽\n/cast 变徵\n/cast 变宫\n";
    }
    return "# 太虚剑意：中文技能名与拼音别名均可使用\n/cast [nobuff:紫气东来&qidian<6] 紫气东来\n/cast [qidian>=8] "
           "无我无剑\n/cast 八荒归元\n/cast 三环套月\n/cast 三柴剑法\n/cast [qidian>=1] 无我无剑\n";
}
} // namespace JX3DPS::engine
