#ifndef JX3DPS_BUFF_H
#define JX3DPS_BUFF_H

#include <algorithm>
#include <concepts>
#include <functional>

#include "common.h"
#include "concepts.hpp"
#include "type.h"

namespace JX3DPS {

template <typename T>
class BuffImpl
{
public:
    struct Self
    {
        enum Roll roll      = HIT;
        int       damage    = 0;
        int       stack_num = 0;
    };

    Self self;

    inline void Init()
    {
        for (auto &&[id, is_active] : talents) {
            if (!is_active) {
                continue;
            }
            auto &&effects = effects_pool_[id];
            for (auto &&[event, effect] : effects) {
                effects_[event].insert(effects_[event].end(), effect.begin(), effect.end());
            }
        }
        for (auto &&[id, is_active] : recipes) {
            if (!is_active) {
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

    struct Snapshot
    {
        int current_duration = 0;
        int current_interval = 0;

        int stack_num = 0;
    };

    using Snapshots = vector_t<Snapshot>;

    inline void ResetInterval(Snapshot &snapshot)
    {
        if constexpr (IsHasted<T>) {
            snapshot.current_interval = T::interval * T::HastePercent();
        } else {
            snapshot.current_interval = T::interval;
        }
    }

    inline void ResetInterval()
    {
        if constexpr (IsHasted<T>) {
            T::current_interval = T::interval * T::HastePercent();
        } else {
            T::current_interval = T::interval;
        }
    }

    /**
     * @brief 每次tick时调用
     */
    inline void Tick()
    {
        if constexpr (HasInterval<T>) { // 如果有每跳属性
            if constexpr (IsDebuff<T>) {
                for_each(T::snapshots.begin(), T::snapshots.end(), [this](auto &&snapshot) {
                    if (snapshot.current_interval == 0) {
                        ResetInterval(snapshot);
                        snapshot.Ticking();
                    }
                });
            } else {
                if (T::current_interval == 0) {
                    ResetInterval();
                    Ticking();
                }
            }
        }
        if constexpr (IsDebuff<T>) {
            for (auto it = T::snapshots.begin(); it != T::snapshots.end();) {
                if (it->current_duration == 0) {
                    Expire(*it);
                    it = T::snapshots.erase(it);
                } else {
                    ++it;
                }
            }
        } else {
            if (T::current_duration == 0) {
                Expire();
            }
        }
    }

    inline void Add()
    {
        std::for_each(effects_[ADD].begin(), effects_[ADD].end(), [](auto &&effect) {
            effect();
        });
    }

    inline void Ticking()
    {
        if constexpr (HasDamage<T>) {
            if constexpr (IsAoe<T>) {
                for (int i = 0; i < T::limit; ++i) {
                    Roll();
                    PostRoll();
                    Damage();
                    PostDamage();
                }
            } else {
                Roll();
                PostRoll();
                Damage();
                PostDamage();
            }
        } else {
            std::for_each(effects_[TICKING].begin(), effects_[TICKING].end(), [](auto &&effect) {
                effect();
            });
        }
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

    inline void Expire(Snapshot &snapshot) { self = snapshot.stack_num; }

    inline void Expire() { self.stack_num = T::stack_num; }

protected:
    using effects_t = array_t<vector_t<func_t<void()>>, COUNT>;

    effects_t               effects_;
    hash_t<jx3id_t, effects_t> effects_pool_;
};

class Buff
{
public:
    virtual ~Buff()        = default;
    virtual void Trigger() = 0;
};

template <typename T>
class BuffWrapper : public Buff
{
    T _buff;

public:
    BuffWrapper(T &&buff) : _buff(std::move(buff)) { }

    void Trigger() final { _buff.Trigger(); }
};

} // namespace JX3DPS

#endif // JX3DPS_BUFF_H