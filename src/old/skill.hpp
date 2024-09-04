#ifndef JX3DPS_SKILL_HPP
#define JX3DPS_SKILL_HPP

#include <algorithm>
#include <concepts>
#include <functional>

#include "common.h"
#include "concepts.hpp"
#include "type.h"

namespace JX3DPS {

template <typename T>
class SkillImpl
{
public:
    using skill_effects_t = array_t<vector_t<func_t<void()>>, COUNT>;

    PROPERTY frame_t* global_cooldown[32] = { nullptr };

    struct Self
    {
        enum Roll roll   = HIT;
        int       damage = 0;
        int       qidian = 0;
    };

    Self self;

    inline void Init()
    {
        for (auto &&[id, isActive] : talents) {
            if (!isActive) {
                continue;
            }
            auto &&effects = effects_pool_[id];
            for (auto &&[event, effect] : effects) {
                effects_[event].insert(effects_[event].end(), effect.begin(), effect.end());
            }
        }
        for (auto &&[id, isActive] : recipes) {
            if (!isActive) {
                continue;
            }
            auto &&effects = effects_pool_[id];
            for (auto &&[event, effect] : effects) {
                effects_[event].insert(effects_[event].end(), effect.begin(), effect.end());
            }
        }
    }

    inline void Register(auto effect_id, EventType event, std::function<void()> &&effect)
    {
        effects_pool_[effect_id][event].emplace_back(std::move(effect));
    }

    inline void Register(EventType event, std::function<void()> &&effect)
    {
        effects_[event].emplace_back(std::move(effect));
    }

    inline void Property(auto effect_id, std::function<void()> &&effect)
    {
        effects_pool_[effect_id][PERMANENT].emplace_back(std::move(effect));
    }

    inline void Property(std::function<void()> &&effect) { effect(); }

    void Cast()
    {
        if constexpr (HasPrepare<T>) {
            PreCast();
        }

        if constexpr (!HasCasting<T>) {
            if constexpr (HasPhysicsAP<T>) {
                Roll();
                PostRoll();
                Damage();
                PostDamage();
            }

            PostCast();
        }
    }

    void Tick()
    {
        if constexpr (HasCasting<T>) {
            Casting();

            if constexpr (HasPhysicsAP<T>) {
                Roll();
                PostRoll();
                Damage();
                PostDamage();
            }

            PostCast();
        }
    }

    inline void PreCast()
    {
        std::for_each(effects_[PRE_CAST].begin(), effects_[PRE_CAST].end(), [](auto &&effect) {
            effect();
        });
    }

    inline void Casting()
    {
        std::for_each(effects_[CASTING].begin(), effects_[CASTING].end(), [](auto &&effect) {
            effect();
        });
    }

    inline void Roll() { self.roll = Roll(); }

    inline void PostRoll()
    {
        std::for_each(effects_[POST_ROLL].begin(), effects_[POST_ROLL].end(), [](auto &&effect) {
            effect();
        });
    }

    inline void Damage() { self.damage = Damage(); }

    inline void PostDamage()
    {
        std::for_each(effects_[POST_DAMAGE].begin(), effects_[POST_DAMAGE].end(), [](auto &&effect) {
            effect();
        });
    }

    inline void PostCast()
    {
        std::for_each(effects_[POST_CAST].begin(), effects_[POST_CAST].end(), [](auto &&effect) {
            effect();
        });
    }

protected:

    skill_effects_t               effects_;
    hash_t<jx3id_t, skill_effects_t> effects_pool_;
};

class Skill
{
public:
    virtual ~Skill()    = default;
    virtual void Cast() = 0;
};

template <typename T>
class SkillWrapper : public Skill
{
    T _skill;

public:
    SkillWrapper(T &&skill) : _skill(std::move(skill)) { }

    void Cast() final { _skill.Cast(); }
};

}; // namespace JX3DPS

#endif // JX3DPS_SKILL_HPP