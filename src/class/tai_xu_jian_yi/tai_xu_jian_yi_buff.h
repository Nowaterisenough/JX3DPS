#ifndef JX3DPS_CLASS_TAI_XU_JIAN_YI_BUFF_H
#define JX3DPS_CLASS_TAI_XU_JIAN_YI_BUFF_H

#include "src/buff/buff.hpp"
#include "src/core/context.h"
#include "tai_xu_jian_yi_ids.h"

namespace JX3DPS {

namespace 太虚剑意 {


// ============= 叠刃 (DieRen) - 核心DOT BUFF =============

class 叠刃 : public BuffImpl<叠刃> {
public:
    static inline constexpr Tag TAG = Tag::TARGET | Tag::DOT | Tag::PHYSICS;

    // BUFF基础属性
    PROPERTY tick_t interval = JX3_FRAMES_PER_SECOND * 3;  // 3秒跳一次
    PROPERTY int max_stacks = 5;                           // 最大5层
    PROPERTY int effect_count = 8;                         // 持续8跳

    // 伤害系数 [层数]
    PROPERTY int base_damage[7] = {
        static_cast<int>(58 * 1.15 * 1.1 * 1.1) * 1,
        static_cast<int>(58 * 1.15 * 1.1 * 1.1) * 2,
        static_cast<int>(58 * 1.15 * 1.1 * 1.1) * 3,
        static_cast<int>(58 * 1.15 * 1.1 * 1.1) * 4,
        static_cast<int>(58 * 1.15 * 1.1 * 1.1) * 5,
        static_cast<int>(58 * 1.15 * 1.1 * 1.1) * 6,
        static_cast<int>(58 * 1.15 * 1.1 * 1.1) * 7
    };

    PROPERTY cof_t physics_ap[7] = {0.0};  // 无AP加成

    叠刃() {
        // 裂云奇穴：最大层数+2
        Register(TalentId::裂云, EventType::POST_INIT, [this]() {
            叠刃::max_stacks = 7;
        });

        // 虚极奇穴：伤害提高20%
        Register(TalentId::虚极, EventType::POST_INIT, [this]() {
            for (int i = 0; i < 7; ++i) {
                叠刃::base_damage[i] = static_cast<int>(叠刃::base_damage[i] * 1.2);
            }
        });

        // 叠刃添加时触发裂云BUFF
        Register(EventType::POST_ADD, [this](jx3id_t targetId) {
            // TODO: Add 裂云 buff if talent enabled
        });

        // 叠刃移除时触发裂云BUFF
        Register(EventType::POST_CLEAR, [this](jx3id_t targetId) {
            // TODO: Add 裂云 buff if talent enabled
        });
    }

    // 切玉技能提前引爆叠刃
    void TriggerQieYu(jx3id_t targetId) {
        // TODO: Calculate remaining ticks and deal damage all at once
    }

    // 环月技能立即触发一跳
    void TriggerHuanYue(jx3id_t targetId) {
        // TODO: Trigger one tick immediately
    }
};

// ============= 万象归元 - AOE DOT BUFF =============

class 万象归元 : public BuffImpl<万象归元> {
public:
    static inline constexpr Tag TAG = Tag::TARGET | Tag::DOT | Tag::PHYSICS;

    PROPERTY tick_t interval = JX3_FRAMES_PER_SECOND * 3;
    PROPERTY int max_stacks = 3;
    PROPERTY int effect_count = 10;

    PROPERTY int base_damage[3] = {
        static_cast<int>(205 * 1 * 1.8),
        static_cast<int>(205 * 2 * 1.8),
        static_cast<int>(205 * 3 * 1.8)
    };

    PROPERTY cof_t physics_ap[3] = {0.0};
};

// ============= 人剑合一 - 消耗剑气场 =============

class 人剑合一Buff : public BuffImpl<人剑合一Buff> {
public:
    static inline constexpr Tag TAG = Tag::PLAYER | Tag::BUFF;

    PROPERTY tick_t duration = JX3_FRAMES_PER_SECOND * 8;  // 持续8秒

    // 提高10%伤害
    PROPERTY pctf_t damage_add = 0.1;

    人剑合一Buff() {
        Register(EventType::POST_ADD, [this]() {
            // TODO: Apply damage bonus
        });

        Register(EventType::POST_CLEAR, [this]() {
            // TODO: Remove damage bonus
        });
    }
};

// ============= 紫气东来 - 气点回复 =============

class 紫气东来Buff : public BuffImpl<紫气东来Buff> {
public:
    static inline constexpr Tag TAG = Tag::PLAYER | Tag::BUFF;

    PROPERTY tick_t duration = JX3_FRAMES_PER_SECOND * 20;  // 持续20秒

    紫气东来Buff() {
        Register(EventType::POST_ADD, [this]() {
            auto *player = static_cast<::JX3DPS::太虚剑意::Player *>(context.player);
            player->AddQidian(5);
        });

        Register(EventType::POST_CLEAR, [this]() {
            // No cleanup needed
        });
    }
};

// ============= 玄门 - 增伤BUFF =============

class 玄门 : public BuffImpl<玄门> {
public:
    static inline constexpr Tag TAG = Tag::PLAYER | Tag::BUFF;

    PROPERTY tick_t duration = JX3_FRAMES_PER_SECOND * 8;
    PROPERTY int max_stacks = 1;

    // 提高10%伤害
    PROPERTY pctf_t damage_add = 0.1;

    玄门() {
        Register(EventType::POST_ADD, [this]() {
            // TODO: Apply damage bonus
        });

        Register(EventType::POST_CLEAR, [this]() {
            // TODO: Remove damage bonus
        });
    }
};

// ============= 剑气场 - 碎星辰 =============

class 剑气场_碎星辰 : public BuffImpl<剑气场_碎星辰> {
public:
    static inline constexpr Tag TAG = Tag::TARGET | Tag::DOT | Tag::PHYSICS | Tag::FIELD;

    PROPERTY tick_t interval = JX3_FRAMES_PER_SECOND * 2;  // 2秒跳一次
    PROPERTY tick_t duration = JX3_FRAMES_PER_SECOND * 20; // 持续20秒
    PROPERTY int effect_count = 10;

    PROPERTY int base_damage = 85;
    PROPERTY cof_t physics_ap = 1.85;

    剑气场_碎星辰() {
        Register(EventType::POST_ADD, [this]() {
            // 气盛奇穴：添加气盛BUFF
            // TODO: Add 气盛 buff if talent enabled
        });

        Register(EventType::TICK, [this]() {
            // 每跳触发伤害
            // TODO: Deal damage
        });
    }
};

// ============= 剑气场 - 生太极 =============

class 剑气场_生太极 : public BuffImpl<剑气场_生太极> {
public:
    static inline constexpr Tag TAG = Tag::TARGET | Tag::DOT | Tag::PHYSICS | Tag::FIELD;

    PROPERTY tick_t interval = JX3_FRAMES_PER_SECOND * 2;
    PROPERTY tick_t duration = JX3_FRAMES_PER_SECOND * 18;
    PROPERTY int effect_count = 9;

    PROPERTY int base_damage = 39;
    PROPERTY cof_t physics_ap = 0.25;

    剑气场_生太极() {
        Register(EventType::TICK, [this]() {
            // TODO: Deal damage
        });
    }
};

// ============= 剑气场 - 吞日月 =============

class 剑气场_吞日月 : public BuffImpl<剑气场_吞日月> {
public:
    static inline constexpr Tag TAG = Tag::TARGET | Tag::DOT | Tag::PHYSICS | Tag::FIELD;

    PROPERTY tick_t interval = JX3_FRAMES_PER_SECOND * 2;
    PROPERTY tick_t duration = JX3_FRAMES_PER_SECOND * 18;
    PROPERTY int effect_count = 9;

    PROPERTY int base_damage = 11;
    PROPERTY cof_t physics_ap = 1.3;

    剑气场_吞日月() {
        Register(EventType::POST_ADD, [this](jx3id_t targetId) {
            // 添加吞日月易伤BUFF
            // TODO: Add vulnerability buff to target
        });

        Register(EventType::TICK, [this]() {
            // TODO: Deal damage
        });
    }
};

// ============= 气盛 - 剑气场增伤 =============

class 气盛 : public BuffImpl<气盛> {
public:
    static inline constexpr Tag TAG = Tag::PLAYER | Tag::BUFF;

    PROPERTY tick_t duration = JX3_FRAMES_PER_SECOND * 20;

    // 提高剑气场伤害30%
    PROPERTY pctf_t field_damage_add = 0.3;

    气盛() {
        Register(EventType::POST_ADD, [this]() {
            // TODO: Apply field damage bonus
        });

        Register(EventType::POST_CLEAR, [this]() {
            // TODO: Remove field damage bonus
        });
    }
};

// ============= 剑入 - 剑入形BUFF =============

class 剑入 : public BuffImpl<剑入> {
public:
    static inline constexpr Tag TAG = Tag::PLAYER | Tag::BUFF;

    PROPERTY tick_t duration = JX3_FRAMES_PER_SECOND * 30;
    PROPERTY int max_stacks = 10;

    // 每层提高1%伤害
    PROPERTY pctf_t damage_add_per_stack = 0.01;

    剑入() {
        Register(EventType::POST_ADD, [this]() {
            // TODO: Apply damage bonus based on stacks
        });

        Register(EventType::POST_STACK_CHANGE, [this]() {
            // TODO: Update damage bonus
        });
    }
};

// ============= 风势 - 加速BUFF =============

class 风势 : public BuffImpl<风势> {
public:
    static inline constexpr Tag TAG = Tag::PLAYER | Tag::BUFF;

    PROPERTY tick_t duration = JX3_FRAMES_PER_SECOND * 10;

    // 提高加速10%
    PROPERTY value_t haste_add = 1024;  // 102.4%基准值的10%

    风势() {
        Register(EventType::POST_ADD, [this]() {
            // TODO: Apply haste bonus
        });

        Register(EventType::POST_CLEAR, [this]() {
            // TODO: Remove haste bonus
        });
    }
};

// ============= 裂云 - 叠刃触发 =============

class 裂云 : public BuffImpl<裂云> {
public:
    static inline constexpr Tag TAG = Tag::PLAYER | Tag::BUFF;

    PROPERTY tick_t duration = JX3_FRAMES_PER_SECOND * 8;
    PROPERTY int max_stacks = 5;

    // 每层提高会心2%
    PROPERTY value_t critical_strike_per_stack = 200;  // 基于万分比

    裂云() {
        Register(EventType::POST_ADD, [this]() {
            // TODO: Apply critical strike bonus
        });

        Register(EventType::POST_STACK_CHANGE, [this]() {
            // TODO: Update critical strike bonus
        });
    }
};

// ============= 镜花影 - 镜像技能触发 =============

class 镜花影 : public BuffImpl<镜花影> {
public:
    static inline constexpr Tag TAG = Tag::PLAYER | Tag::BUFF;

    PROPERTY tick_t duration = JX3_FRAMES_PER_SECOND * 6;

    // 下一个无我无剑造成额外50%伤害
    PROPERTY pctf_t damage_add = 0.5;

    镜花影() {
        Register(EventType::POST_ADD, [this]() {
            // TODO: Mark for damage bonus
        });

        Register(EventType::POST_CONSUME, [this]() {
            // TODO: Consume buff after use
        });
    }
};

// ============= 持盈 - 奇穴BUFF =============

class 持盈 : public BuffImpl<持盈> {
public:
    static inline constexpr Tag TAG = Tag::PLAYER | Tag::BUFF;

    PROPERTY tick_t duration = JX3_FRAMES_PER_SECOND * 12;
    PROPERTY int max_stacks = 10;

    // 每层提高0.5%会心
    PROPERTY value_t critical_strike_per_stack = 50;

    持盈() {
        Register(EventType::POST_ADD, [this]() {
            // TODO: Apply critical strike bonus
        });

        Register(EventType::POST_STACK_CHANGE, [this]() {
            // TODO: Update critical strike bonus
        });

        // 10层时触发伤害
        Register(EventType::POST_STACK_CHANGE, [this]() {
            auto *player = static_cast<::JX3DPS::太虚剑意::Player *>(context.player);
            if (player->GetBuffStack(static_cast<jx3id_t>(BuffId::持盈)) >= 10) {
                // TODO: Deal damage
            }
        });
    }
};

// ============= 云中剑系列 - 奇穴触发 =============

class 云中剑_碎星辰 : public BuffImpl<云中剑_碎星辰> {
public:
    static inline constexpr Tag TAG = Tag::PLAYER | Tag::BUFF;

    PROPERTY tick_t duration = JX3_FRAMES_PER_SECOND * 15;

    // 碎星辰冷却缩短50%
    PROPERTY pctf_t cooldown_reduce = 0.5;
};

class 云中剑_生太极 : public BuffImpl<云中剑_生太极> {
public:
    static inline constexpr Tag TAG = Tag::PLAYER | Tag::BUFF;

    PROPERTY tick_t duration = JX3_FRAMES_PER_SECOND * 15;

    // 生太极冷却缩短50%
    PROPERTY pctf_t cooldown_reduce = 0.5;
};

class 云中剑_吞日月 : public BuffImpl<云中剑_吞日月> {
public:
    static inline constexpr Tag TAG = Tag::PLAYER | Tag::BUFF;

    PROPERTY tick_t duration = JX3_FRAMES_PER_SECOND * 15;

    // 吞日月冷却缩短50%
    PROPERTY pctf_t cooldown_reduce = 0.5;
};

// ============= 门派特性 =============

class 门派特性 : public BuffImpl<门派特性> {
public:
    static inline constexpr Tag TAG = Tag::PLAYER | Tag::BUFF | Tag::PERMANENT;

    // 永久增伤10%
    PROPERTY pctf_t damage_add = 0.1;

    门派特性() {
        Register(EventType::POST_INIT, [this]() {
            // TODO: Apply permanent damage bonus
        });
    }
};

// ============= 武器特效CW =============

class 武器特效CW : public BuffImpl<武器特效CW> {
public:
    static inline constexpr Tag TAG = Tag::PLAYER | Tag::BUFF;

    PROPERTY tick_t duration = JX3_FRAMES_PER_SECOND * 10;
    PROPERTY int max_stacks = 5;

    // 每层提高2%伤害
    PROPERTY pctf_t damage_add_per_stack = 0.02;

    武器特效CW() {
        Register(EventType::POST_ADD, [this]() {
            // TODO: Apply damage bonus
        });

        Register(EventType::POST_STACK_CHANGE, [this]() {
            // TODO: Update damage bonus
        });
    }
};

// ============= 套装特效 =============

class 套装特效 : public BuffImpl<套装特效> {
public:
    static inline constexpr Tag TAG = Tag::PLAYER | Tag::BUFF;

    PROPERTY tick_t duration = JX3_FRAMES_PER_SECOND * 15;

    // 提高15%伤害
    PROPERTY pctf_t damage_add = 0.15;

    套装特效() {
        Register(EventType::POST_ADD, [this]() {
            // TODO: Apply damage bonus
        });

        Register(EventType::POST_CLEAR, [this]() {
            // TODO: Remove damage bonus
        });
    }
};

// ============= 团队增益 - 游刃 =============

class 团队核心_太虚剑意_游刃 : public BuffImpl<团队核心_太虚剑意_游刃> {
public:
    static inline constexpr Tag TAG = Tag::TEAM | Tag::BUFF;

    PROPERTY tick_t duration = JX3_FRAMES_PER_SECOND * 20;
    PROPERTY int max_stacks = 1;

    // 提高2%伤害
    PROPERTY pctf_t damage_add = 0.02;

    团队核心_太虚剑意_游刃() {
        Register(EventType::POST_ADD, [this]() {
            // TODO: Apply team damage bonus
        });
    }
};

// ============= 团队增益 - 精妙 =============

class 团队核心_太虚剑意_精妙 : public BuffImpl<团队核心_太虚剑意_精妙> {
public:
    static inline constexpr Tag TAG = Tag::TEAM | Tag::BUFF;

    // 永久增加会心5%
    PROPERTY value_t critical_strike_add = 500;

    团队核心_太虚剑意_精妙() {
        Register(EventType::POST_INIT, [this]() {
            // TODO: Apply permanent critical strike bonus
        });
    }
};

} // namespace 太虚剑意

} // namespace JX3DPS

#endif // JX3DPS_CLASS_TAI_XU_JIAN_YI_BUFF_H
