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

bool 无我无剑::CheckResource() const {
    auto *player = static_cast<Player *>(context.player);
    return player != nullptr && player->GetQidian() > 0;
}

无我无剑::无我无剑() {
    // 施放前 - 记录气点并清零
    Register(EventType::PRE_CAST, [this]() {
        auto *player = static_cast<Player *>(context.player);
        self.level = player->GetQidian() - 1; // 0-9
        if (self.level < 0) self.level = 0;
        player->SetQidian(0);
    });

    // 新版本技能描述：消耗3格及以上气时额外造成一次破招伤害。
    Register(EventType::POST_ROLL, [this]() {
        int qidian = self.level + 1;
        if (qidian >= 3) {
            int pozhao_level = qidian - 3;
            // TODO: CastSkill<破招>(pozhao_level);
        }
    });

    // 奇穴: 无意 - 消耗3格气以上增加会心和会效
    Register(TalentId::无意, EventType::PRE_DAMAGE, [this]() {
        int qidian = self.level + 1;
        if (qidian >= 3) {
            self.damage_cof += 0.1; // 会心+10%，会效+30%由属性层处理
        }
    });

    // 奇穴: 叠刃 - 命中后添加一层叠刃（会心时再添加一层）
    Register(TalentId::叠刃, EventType::POST_CAST, [this]() {
        auto *player = static_cast<Player *>(context.player);
        // 添加叠刃BUFF，层数根据气点
        int qidian = self.level + 1;
        // TODO: player->AddBuff<叠刃>(context.player->GetTargetId(), qidian);
    });
}

// ========== 三环套月 ==========

三环套月::三环套月() {
    // 基础技能效果：命中后续气一格。玄门只修改人剑合一的增益，
    // 不应再把该效果错误绑定到三环套月。
    Register(EventType::POST_DAMAGE, [this]() {
        auto *player = static_cast<Player *>(context.player);
        player->AddQidian(1);
    });
}

// ========== 人剑合一 ==========

人剑合一::人剑合一() {
    // 施放后添加人剑合一BUFF
    Register(EventType::POST_CAST, [this]() {
        auto *player = static_cast<Player *>(context.player);
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
        auto *player = static_cast<Player *>(context.player);
        player->cooldownSanChaiJianFaCurrent = 48; // 3秒GCD
    });
}

// ========== 碎星辰 ==========

碎星辰::碎星辰() {
    // 读条结束后生成剑气场
    Register(EventType::POST_CASTING, [this]() {
        auto *player = static_cast<Player *>(context.player);
        jx3id_t fieldId = player->nextFieldId++;
        player->AddField(fieldId);
        // TODO: target->AddBuff<剑气场_碎星辰>(targetId, 1);
    });
}

// ========== 生太极 ==========

生太极::生太极() {
    Register(EventType::POST_CASTING, [this]() {
        auto *player = static_cast<Player *>(context.player);
        jx3id_t fieldId = player->nextFieldId++;
        player->AddField(fieldId);
        // TODO: target->AddBuff<剑气场_生太极>(targetId, 1);
    });
}

// ========== 吞日月 ==========

吞日月::吞日月() {
    Register(EventType::POST_CASTING, [this]() {
        auto *player = static_cast<Player *>(context.player);
        jx3id_t fieldId = player->nextFieldId++;
        player->AddField(fieldId);
        // TODO: target->AddBuff<剑气场_吞日月>(targetId, 1);
    });
}

// ========== 紫气东来 ==========

紫气东来::紫气东来() {
    // 施放后立即聚5格气；当前资源上限为10格，后续每秒回复由运行时资源
    // 事件处理器补齐，这里先保证宏模拟的即时状态与官方描述一致。
    Register(EventType::POST_CAST, [this]() {
        auto *player = static_cast<Player *>(context.player);
        player->AddQidian(5);
        // TODO: player->AddBuff<紫气东来>(1);
    });
}

} // namespace 太虚剑意
} // namespace JX3DPS
