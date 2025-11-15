#ifndef JX3DPS_CLASS_TAI_XU_JIAN_YI_H
#define JX3DPS_CLASS_TAI_XU_JIAN_YI_H

#include "src/core/context.h"
#include "src/global/concepts.h"
#include "src/global/jx3.h"
#include "src/global/types.h"
#include "src/player/player.hpp"
#include "tai_xu_jian_yi_ids.h"
#include <list>

namespace JX3DPS {
namespace 太虚剑意 {

/**
 * @brief 太虚剑意玩家类
 *
 * 特色系统：
 * - 气点机制 (0-10点)
 * - 剑气场管理
 * - 三柴剑法特殊GCD
 */
class Player : public JX3DPS::Player<Player> {
public:
    Player() = default;
    ~Player() = default;

    // ========== 气点系统 ==========

    int GetQidian() const { return m_qidian; }
    void SetQidian(int qidian) { m_qidian = std::clamp(qidian, 0, 10); }
    void AddQidian(int qidian) { SetQidian(m_qidian + qidian); }

    // ========== GCD 管理 ==========

    tick_t cooldownSanChaiJianFaCurrent = 0;

    tick_t GetNextGlobalCooldown() const override {
        tick_t gcd = global_cooldown_current > 0 ? global_cooldown_current : JX3DPS_INVALID_FRAMES;
        if (cooldownSanChaiJianFaCurrent > 0) {
            gcd = std::min(gcd, cooldownSanChaiJianFaCurrent);
        }
        return gcd;
    }

    void UpdateGlobalCooldown(tick_t next) override {
        global_cooldown_current = std::max(global_cooldown_current - next, 0);
        cooldownSanChaiJianFaCurrent = std::max(cooldownSanChaiJianFaCurrent - next, 0);
    }

    // ========== 剑气场管理 ==========

    std::list<jx3id_t> fields;   // 已生成的剑气场列表
    jx3id_t nextFieldId = 1;     // 下一个剑气场ID

    void AddField(jx3id_t fieldId) {
        fields.push_back(fieldId);
    }

    void RemoveField(jx3id_t fieldId, int count = 1) {
        int removed = 0;
        for (auto it = fields.begin(); it != fields.end() && removed < count;) {
            if (*it == fieldId) {
                it = fields.erase(it);
                ++removed;
            } else {
                ++it;
            }
        }
    }

private:
    int m_qidian = 10; // 初始10点气点
};

} // namespace 太虚剑意
} // namespace JX3DPS

// ========== 包含BUFF和技能定义 ==========
#include "tai_xu_jian_yi_buff.h"
#include "tai_xu_jian_yi_skill.h"

#endif // JX3DPS_CLASS_TAI_XU_JIAN_YI_H
