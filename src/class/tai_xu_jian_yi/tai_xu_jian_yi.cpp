/**
 * @file tai_xu_jian_yi.cpp
 * @brief 太虚剑意动态库实现
 *
 * 提供C++和C API接口用于技能模拟
 */

#include "tai_xu_jian_yi.h"
#include "tai_xu_jian_yi_buff.h"
#include "tai_xu_jian_yi_export.h"

#include <memory>
#include <string>
#include <cstring>
#include <stdexcept>

namespace JX3DPS {
namespace 太虚剑意 {

// ============================================================================
// C++ API Implementation
// ============================================================================

/**
 * @brief 获取版本信息
 */
TXJY_API const char* GetVersion() {
    return "1.0.0";
}

/**
 * @brief 获取心法名称
 */
TXJY_API const char* GetClassName() {
    return "太虚剑意";
}

/**
 * @brief 获取技能数量
 */
TXJY_API int GetSkillCount() {
    return 11; // 11个技能
}

/**
 * @brief 获取BUFF数量
 */
TXJY_API int GetBuffCount() {
    return 25; // 25个BUFF
}

/**
 * @brief 创建玩家实例
 */
TXJY_API Player* CreatePlayer() {
    try {
        return new Player();
    } catch (...) {
        return nullptr;
    }
}

/**
 * @brief 销毁玩家实例
 */
TXJY_API void DestroyPlayer(Player* player) {
    if (player) {
        delete player;
    }
}

/**
 * @brief 获取气点
 */
TXJY_API int GetQidian(Player* player) {
    if (!player) return -1;
    return player->GetQidian();
}

/**
 * @brief 设置气点
 */
TXJY_API void SetQidian(Player* player, int qidian) {
    if (player) {
        player->SetQidian(qidian);
    }
}

/**
 * @brief 添加气点
 */
TXJY_API void AddQidian(Player* player, int qidian) {
    if (player) {
        player->AddQidian(qidian);
    }
}

/**
 * @brief 获取剑气场数量
 */
TXJY_API int GetFieldCount(Player* player) {
    if (!player) return -1;
    return static_cast<int>(player->fields.size());
}

} // namespace 太虚剑意
} // namespace JX3DPS

// ============================================================================
// C API Implementation
// ============================================================================

TXJY_EXTERN_C_BEGIN

/**
 * @brief 获取库版本
 */
TXJY_API const char* TXJY_CALL txjy_get_version() {
    return JX3DPS::太虚剑意::GetVersion();
}

/**
 * @brief 获取心法名称
 */
TXJY_API const char* TXJY_CALL txjy_get_class_name() {
    return JX3DPS::太虚剑意::GetClassName();
}

/**
 * @brief 获取技能数量
 */
TXJY_API int TXJY_CALL txjy_get_skill_count() {
    return JX3DPS::太虚剑意::GetSkillCount();
}

/**
 * @brief 获取BUFF数量
 */
TXJY_API int TXJY_CALL txjy_get_buff_count() {
    return JX3DPS::太虚剑意::GetBuffCount();
}

/**
 * @brief 创建玩家实例句柄
 * @return 玩家实例句柄，失败返回NULL
 */
TXJY_API void* TXJY_CALL txjy_create_player() {
    return JX3DPS::太虚剑意::CreatePlayer();
}

/**
 * @brief 销毁玩家实例
 * @param handle 玩家实例句柄
 */
TXJY_API void TXJY_CALL txjy_destroy_player(void* handle) {
    if (handle) {
        auto* player = static_cast<JX3DPS::太虚剑意::Player*>(handle);
        JX3DPS::太虚剑意::DestroyPlayer(player);
    }
}

/**
 * @brief 获取气点
 * @param handle 玩家实例句柄
 * @return 当前气点值，失败返回-1
 */
TXJY_API int TXJY_CALL txjy_get_qidian(void* handle) {
    if (!handle) return -1;
    auto* player = static_cast<JX3DPS::太虚剑意::Player*>(handle);
    return JX3DPS::太虚剑意::GetQidian(player);
}

/**
 * @brief 设置气点
 * @param handle 玩家实例句柄
 * @param qidian 气点值 (0-10)
 */
TXJY_API void TXJY_CALL txjy_set_qidian(void* handle, int qidian) {
    if (handle) {
        auto* player = static_cast<JX3DPS::太虚剑意::Player*>(handle);
        JX3DPS::太虚剑意::SetQidian(player, qidian);
    }
}

/**
 * @brief 添加气点
 * @param handle 玩家实例句柄
 * @param qidian 增加的气点数
 */
TXJY_API void TXJY_CALL txjy_add_qidian(void* handle, int qidian) {
    if (handle) {
        auto* player = static_cast<JX3DPS::太虚剑意::Player*>(handle);
        JX3DPS::太虚剑意::AddQidian(player, qidian);
    }
}

/**
 * @brief 获取剑气场数量
 * @param handle 玩家实例句柄
 * @return 剑气场数量，失败返回-1
 */
TXJY_API int TXJY_CALL txjy_get_field_count(void* handle) {
    if (!handle) return -1;
    auto* player = static_cast<JX3DPS::太虚剑意::Player*>(handle);
    return JX3DPS::太虚剑意::GetFieldCount(player);
}

/**
 * @brief 添加剑气场
 * @param handle 玩家实例句柄
 * @param field_id 剑气场ID
 */
TXJY_API void TXJY_CALL txjy_add_field(void* handle, unsigned int field_id) {
    if (handle) {
        auto* player = static_cast<JX3DPS::太虚剑意::Player*>(handle);
        player->AddField(field_id);
    }
}

/**
 * @brief 移除剑气场
 * @param handle 玩家实例句柄
 * @param field_id 剑气场ID
 * @param count 移除数量
 */
TXJY_API void TXJY_CALL txjy_remove_field(void* handle, unsigned int field_id, int count) {
    if (handle) {
        auto* player = static_cast<JX3DPS::太虚剑意::Player*>(handle);
        player->RemoveField(field_id, count);
    }
}

/**
 * @brief 获取技能名称
 * @param skill_id 技能ID
 * @return 技能名称，未知技能返回"Unknown"
 */
TXJY_API const char* TXJY_CALL txjy_get_skill_name(unsigned int skill_id) {
    using namespace JX3DPS::太虚剑意;

    switch (static_cast<SkillId>(skill_id)) {
        case SkillId::破招: return "破招";
        case SkillId::无我无剑: return "无我无剑";
        case SkillId::八荒归元: return "八荒归元";
        case SkillId::三环套月: return "三环套月";
        case SkillId::万剑归宗: return "万剑归宗";
        case SkillId::人剑合一: return "人剑合一";
        case SkillId::三柴剑法: return "三柴剑法";
        case SkillId::生太极: return "生太极";
        case SkillId::碎星辰: return "碎星辰";
        case SkillId::吞日月: return "吞日月";
        case SkillId::紫气东来: return "紫气东来";
        case SkillId::镜花影: return "镜花影";
        default: return "Unknown";
    }
}

/**
 * @brief 获取BUFF名称
 * @param buff_id BUFF ID
 * @return BUFF名称，未知BUFF返回"Unknown"
 */
TXJY_API const char* TXJY_CALL txjy_get_buff_name(unsigned int buff_id) {
    using namespace JX3DPS::太虚剑意;

    switch (static_cast<BuffId>(buff_id)) {
        case BuffId::叠刃: return "叠刃";
        case BuffId::万象归元: return "万象归元";
        case BuffId::人剑合一: return "人剑合一";
        case BuffId::紫气东来: return "紫气东来";
        case BuffId::玄门: return "玄门";
        case BuffId::风势: return "风势";
        case BuffId::裂云: return "裂云";
        case BuffId::气盛: return "气盛";
        case BuffId::剑入: return "剑入";
        case BuffId::镜花影: return "镜花影";
        case BuffId::持盈: return "持盈";
        default: return "Unknown";
    }
}

/**
 * @brief 获取错误信息
 * @return 最后一次错误信息
 */
TXJY_API const char* TXJY_CALL txjy_get_last_error() {
    // TODO: Implement thread-local error storage
    return "No error";
}

TXJY_EXTERN_C_END
