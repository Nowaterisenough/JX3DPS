#ifndef JX3DPS_SKILL_SKILL_IMPL_HPP
#define JX3DPS_SKILL_SKILL_IMPL_HPP

#include "src/core/context.h"
#include "src/global/concepts.h"
#include "src/global/types.h"
#include <random>

namespace JX3DPS {

/**
 * @brief 技能实现基类 - 使用CRTP + Concepts实现编译期分发
 *
 * @tparam DerivedSkill 派生技能类型
 *
 * 核心机制:
 * 1. 通过Concepts检测派生类是否有特定字段
 * 2. 使用if constexpr根据字段存在与否选择不同执行路径
 * 3. 零运行时开销，所有分支在编译期决定
 */
template <typename DerivedSkill>
class SkillImpl {
public:
    // ===== 技能执行上下文 =====
    struct Self {
        RollResult roll   = RollResult::HIT;
        int damage        = 0;
        int qidian        = 0;
        int level         = 0;
        double damage_cof = 0.0;  // 伤害系数加成
        // 其他临时数据
    };

    PROPERTY Self self;

    // ===== 事件系统 =====
    using EventEffects = array_t<vector_t<func_t<void()>>, static_cast<size_t>(EventType::COUNT)>;

    PROPERTY EventEffects effects_;
    PROPERTY hash_t<int, EventEffects> effects_pool_; // 奇穴/秘籍效果池

    /**
     * @brief 注册通用事件
     */
    inline void Register(EventType event, func_t<void()> &&effect) {
        effects_[static_cast<size_t>(event)].emplace_back(std::move(effect));
    }

    /**
     * @brief 注册奇穴/秘籍事件
     */
    template <typename IdType>
    inline void Register(IdType effectId, EventType event, func_t<void()> &&effect) {
        effects_pool_[static_cast<int>(effectId)][static_cast<size_t>(event)].emplace_back(std::move(effect));
    }

    /**
     * @brief 初始化 - 合并奇穴和秘籍效果
     */
    inline void Init(const hash_t<int, bool> &talents, const hash_t<int, bool> &recipes) {
        // 合并奇穴效果
        for (auto &&[id, isActive] : talents) {
            if (!isActive) continue;
            auto &&effects = effects_pool_[id];
            for (size_t i = 0; i < effects.size(); ++i) {
                effects_[i].insert(effects_[i].end(), effects[i].begin(), effects[i].end());
            }
        }
        // 合并秘籍效果
        for (auto &&[id, isActive] : recipes) {
            if (!isActive) continue;
            auto &&effects = effects_pool_[id];
            for (size_t i = 0; i < effects.size(); ++i) {
                effects_[i].insert(effects_[i].end(), effects[i].begin(), effects[i].end());
            }
        }
    }

    /**
     * @brief 触发事件
     */
    inline void TriggerEvents(EventType event) {
        for (auto &&effect : effects_[static_cast<size_t>(event)]) {
            effect();
        }
    }

    // ===== 主要接口 =====

    /**
     * @brief 施放技能
     *
     * 根据Concepts自动选择执行路径:
     * - 有前摇: 设置前摇，return
     * - 有读条: 设置读条，return
     * - 即时技能: 直接Roll -> Damage -> PostCast
     */
    void Cast() {
        // 1. 检查前摇
        if constexpr (HasPrepare<DerivedSkill>) {
            PreCast();
            return;
        }

        // 2. 检查读条
        if constexpr (HasCasting<DerivedSkill>) {
            StartCasting();
            return;
        }

        // 3. 即时技能 - 直接结算
        if constexpr (HasPhysicsAP<DerivedSkill> || HasMagicAP<DerivedSkill>) {
            Roll();
            PostRoll();
            Damage();
            PostDamage();
        }

        PostCast();
    }

    /**
     * @brief 每帧更新
     *
     * 处理前摇/读条倒计时
     */
    void Tick() {
        // 1. 更新前摇
        if constexpr (HasPrepare<DerivedSkill>) {
            if (*DerivedSkill::current_prepare > 0) {
                --(*DerivedSkill::current_prepare);
                if (*DerivedSkill::current_prepare == 0) {
                    // 前摇结束，继续施放
                    Cast();
                }
                return;
            }
        }

        // 2. 更新读条
        if constexpr (HasCasting<DerivedSkill>) {
            if (*DerivedSkill::current_casting > 0) {
                --(*DerivedSkill::current_casting);
                if (*DerivedSkill::current_casting == 0) {
                    // 读条结束，结算
                    Casting();
                }
                return;
            }
        }

        // 3. 更新冷却
        if constexpr (HasCooldown<DerivedSkill> && !HasEnergy<DerivedSkill>) {
            if (*DerivedSkill::current_cooldown > 0) {
                --(*DerivedSkill::current_cooldown);
            }
        }
    }

    /**
     * @brief 检查技能是否就绪
     */
    bool IsReady() const {
        // 1. 检查GCD
        if constexpr (!HasNoGCD<DerivedSkill>) {
            if (context.GetGlobalCooldown() > 0) {
                return false;
            }
        }

        // 2. 检查冷却/充能
        if constexpr (HasEnergy<DerivedSkill>) {
            if (DerivedSkill::energy_count <= 0) {
                return false;
            }
        } else if constexpr (HasCooldown<DerivedSkill>) {
            if (*DerivedSkill::current_cooldown > 0) {
                return false;
            }
        }

        // 3. 检查前摇/读条状态
        if constexpr (HasPrepare<DerivedSkill>) {
            if (*DerivedSkill::current_prepare > 0) {
                return false;
            }
        }

        if constexpr (HasCasting<DerivedSkill>) {
            if (*DerivedSkill::current_casting > 0) {
                return false;
            }
        }

        return true;
    }

private:
    // ===== 内部方法 =====

    void PreCast() {
        TriggerEvents(EventType::PRE_PREPARE);

        if constexpr (HasPrepare<DerivedSkill>) {
            *DerivedSkill::current_prepare = DerivedSkill::prepare;
        }

        TriggerEvents(EventType::POST_PREPARE);
    }

    void StartCasting() {
        TriggerEvents(EventType::PRE_CASTING);

        if constexpr (HasCasting<DerivedSkill>) {
            *DerivedSkill::current_casting = DerivedSkill::casting;
        }

        TriggerEvents(EventType::POST_CASTING);
    }

    void Casting() {
        // 读条结束，触发伤害
        if constexpr (HasPhysicsAP<DerivedSkill> || HasMagicAP<DerivedSkill>) {
            Roll();
            PostRoll();
            Damage();
            PostDamage();
        }

        PostCast();
    }

    void Roll() {
        TriggerEvents(EventType::PRE_ROLL);

        // 判定
        if constexpr (HasPhysicsAP<DerivedSkill>) {
            self.roll = RollPhysics();
        } else if constexpr (HasMagicAP<DerivedSkill>) {
            self.roll = RollMagic();
        }
    }

    void PostRoll() { TriggerEvents(EventType::POST_ROLL); }

    void Damage() {
        TriggerEvents(EventType::PRE_DAMAGE);

        // 计算伤害
        if constexpr (HasMultiSub<DerivedSkill>) {
            // 多段伤害
            for (int sub = 0; sub < DerivedSkill::SUB; ++sub) {
                self.damage = CalcDamage(sub);
                ApplyDamage(sub);
            }
        } else {
            // 单段伤害
            self.damage = CalcDamage(0);
            ApplyDamage(0);
        }
    }

    void PostDamage() { TriggerEvents(EventType::POST_DAMAGE); }

    void PostCast() {
        TriggerEvents(EventType::POST_CAST);

        ApplyCooldown();
    }

    void ApplyCooldown() {
        if constexpr (HasEnergy<DerivedSkill>) {
            // 充能技能
            if (DerivedSkill::energy_count > 0) {
                --DerivedSkill::energy_count;
            }
        } else if constexpr (HasCooldown<DerivedSkill>) {
            // 普通冷却
            *DerivedSkill::current_cooldown = DerivedSkill::cooldown;
        }

        // 应用GCD
        if constexpr (!HasNoGCD<DerivedSkill>) {
            context.SetGlobalCooldown(JX3_GLOBAL_COOLDOWN);
        }
    }

    // ===== 伤害计算 =====

    RollResult RollPhysics() const {
        // TODO: 实现物理圆桌判定
        return RollResult::HIT;
    }

    RollResult RollMagic() const {
        // TODO: 实现魔法圆桌判定
        return RollResult::HIT;
    }

    int CalcDamage(int sub) {
        int damage = 0;

        // 1. 基础伤害
        if constexpr (HasMultiSub<DerivedSkill>) {
            damage = DerivedSkill::base_damage[sub][self.level];
            damage += Random(0, DerivedSkill::rand_damage[sub][self.level]);
        } else {
            if constexpr (std::is_array_v<decltype(DerivedSkill::base_damage)>) {
                damage = DerivedSkill::base_damage[self.level];
                damage += Random(0, DerivedSkill::rand_damage[self.level]);
            } else {
                damage = DerivedSkill::base_damage;
                damage += Random(0, DerivedSkill::rand_damage);
            }
        }

        // 2. 攻击力加成
        // TODO: 完整的伤害计算公式

        return damage;
    }

    void ApplyDamage(int sub) {
        // TODO: 应用伤害到目标
    }

    int Random(int min, int max) {
        static thread_local std::mt19937 rng(std::random_device{}());
        std::uniform_int_distribution<int> dist(min, max);
        return dist(rng);
    }
};

} // namespace JX3DPS

#endif // JX3DPS_SKILL_SKILL_IMPL_HPP
