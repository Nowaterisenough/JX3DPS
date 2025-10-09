/**
 * @file tai_xu_jian_yi_skill.cpp
 * @brief 太虚剑意技能实现
 */

#include "tai_xu_jian_yi_skill.h"
#include "tai_xu_jian_yi.h"
#include "src/core/context.h"

namespace JX3DPS {
namespace 太虚剑意 {

// ========== 无我无剑 ==========

无我无剑::无我无剑() {
    // 施放前 - 记录气点并清零
    Register(EventType::PRE_CAST, [this]() {
        auto *player = static_cast<Player *>(tls_sim.player);
        self.level = player->GetQidian() - 1; // 0-9
        if (self.level < 0) self.level = 0;
        player->SetQidian(0);
    });

    // 判定后 - 如果气点>=6，触发破招
    Register(EventType::POST_ROLL, [this]() {
        int qidian = self.level + 1;
        if (qidian >= 6) {
            int pozhao_level = (qidian - 6) / 2; // 0, 1, 2
            // TODO: CastSkill<破招>(pozhao_level);
        }
    });

    // 奇穴: 无意 - 6气以上增加会心和会效
    Register(TalentId::无意, EventType::PRE_DAMAGE, [this]() {
        int qidian = self.level + 1;
        if (qidian >= 6) {
            self.damage_cof += 0.1; // +10%会心, +30%会效
        }
    });

    // 奇穴: 叠刃 - 施放后添加叠刃BUFF
    Register(TalentId::叠刃, EventType::POST_CAST, [this]() {
        auto *player = static_cast<Player *>(tls_sim.player);
        // 添加叠刃BUFF，层数根据气点
        int qidian = self.level + 1;
        // TODO: player->AddBuff<叠刃>(tls_sim.player->GetTargetId(), qidian);
    });
}

// ========== 三环套月 ==========

三环套月::三环套月() {
    // 奇穴: 玄门 - 叠加气点
    Register(TalentId::玄门, EventType::POST_DAMAGE, [this]() {
        auto *player = static_cast<Player *>(tls_sim.player);
        player->AddQidian(1);
        // TODO: player->AddBuff<玄门>(1);
    });
}

// ========== 人剑合一 ==========

人剑合一::人剑合一() {
    // 施放后添加人剑合一BUFF
    Register(EventType::POST_CAST, [this]() {
        auto *player = static_cast<Player *>(tls_sim.player);
        // TODO: player->AddBuff<人剑合一>(1);

        // 消耗一个剑气场
        if (!player->fields.empty()) {
            jx3id_t fieldId = player->fields.front();
            player->RemoveField(fieldId, 1);
        }
    });
}

// ========== 三柴剑法 ==========

三柴剑法::三柴剑法() {
    // 施放后设置特殊GCD
    Register(EventType::POST_CAST, [this]() {
        auto *player = static_cast<Player *>(tls_sim.player);
        player->cooldownSanChaiJianFaCurrent = 48; // 3秒GCD
    });
}

// ========== 碎星辰 ==========

碎星辰::碎星辰() {
    // 读条结束后生成剑气场
    Register(EventType::POST_CASTING, [this]() {
        auto *player = static_cast<Player *>(tls_sim.player);
        jx3id_t fieldId = player->nextFieldId++;
        player->AddField(fieldId);
        // TODO: target->AddBuff<剑气场_碎星辰>(targetId, 1);
    });
}

// ========== 生太极 ==========

生太极::生太极() {
    Register(EventType::POST_CASTING, [this]() {
        auto *player = static_cast<Player *>(tls_sim.player);
        jx3id_t fieldId = player->nextFieldId++;
        player->AddField(fieldId);
        // TODO: target->AddBuff<剑气场_生太极>(targetId, 1);
    });
}

// ========== 吞日月 ==========

吞日月::吞日月() {
    Register(EventType::POST_CASTING, [this]() {
        auto *player = static_cast<Player *>(tls_sim.player);
        jx3id_t fieldId = player->nextFieldId++;
        player->AddField(fieldId);
        // TODO: target->AddBuff<剑气场_吞日月>(targetId, 1);
    });
}

// ========== 紫气东来 ==========

紫气东来::紫气东来() {
    // 施放后恢复10点气点
    Register(EventType::POST_CAST, [this]() {
        auto *player = static_cast<Player *>(tls_sim.player);
        player->SetQidian(10);
        // TODO: player->AddBuff<紫气东来>(1);
    });
}

} // namespace 太虚剑意
} // namespace JX3DPS
