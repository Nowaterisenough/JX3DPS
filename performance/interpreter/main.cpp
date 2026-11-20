#include <functional>
#include <memory>
#include <vector>

#include <benchmark/benchmark.h>
#include <proxy/proxy.h>

// 基础类定义
class Player
{
public:
    int buff_duration  = 100;
    int skill_cooldown = 0;
    int mana           = 50;
    int life           = 100;
};

class Targets
{
};

// std::bind 方法
using ConditionFunc = std::function<bool(const Player *, const Targets *)>;

bool CheckBuffDuration(const Player *player, const Targets *, int duration)
{
    return player->buff_duration > duration;
}

bool CheckSkillCooldown(const Player *player, const Targets *, int cooldown)
{
    return player->skill_cooldown <= cooldown;
}

bool CheckManaLevel(const Player *player, const Targets *, int mana)
{
    return player->mana >= mana;
}

bool CheckLifeLevel(const Player *player, const Targets *, int life)
{
    return player->life > life;
}

ConditionFunc CreateBindCondition(int kind)
{
    switch (kind) {
        case 0: return std::bind(CheckBuffDuration, std::placeholders::_1, std::placeholders::_2, 50);
        case 1: return std::bind(CheckSkillCooldown, std::placeholders::_1, std::placeholders::_2, 0);
        case 2: return std::bind(CheckManaLevel, std::placeholders::_1, std::placeholders::_2, 30);
        case 3: return std::bind(CheckLifeLevel, std::placeholders::_1, std::placeholders::_2, 20);
    }
    return nullptr;
}

// Proxy方法
class BuffConditionProxy
{
private:
    int _duration;

public:
    BuffConditionProxy(int duration) : _duration(duration) { }

    bool Evaluate(const Player *player, const Targets *) const { return player->buff_duration > _duration; }
};

class CooldownConditionProxy
{
private:
    int _cooldown;

public:
    explicit CooldownConditionProxy(int cooldown) : _cooldown(cooldown) { }

    bool Evaluate(const Player *player, const Targets *) const { return player->skill_cooldown <= _cooldown; }
};

class ManaConditionProxy
{
private:
    int _mana;

public:
    ManaConditionProxy(int mana) : _mana(mana) { }

    bool Evaluate(const Player *player, const Targets *) const { return player->mana >= _mana; }
};

class LifeConditionProxy
{
private:
    int _life;

public:
    LifeConditionProxy(int life) : _life(life) { }

    bool Evaluate(const Player *player, const Targets *) const { return player->life > _life; }
};

// Proxy 定义
namespace spec {

PRO_DEF_MEM_DISPATCH(MemEvaluate, Evaluate);

struct Evaluatable : pro::facade_builder::add_convention<MemEvaluate, bool(const Player *, const Targets *) const>::build
{
};

} // namespace spec

// Heap-owned virtual controls use the same four predicates as the inline proxies.
class VirtualCondition
{
public:
    virtual ~VirtualCondition()                                  = default;
    virtual bool Evaluate(const Player *, const Targets *) const = 0;
};

template <class T>
class VirtualPredicate final : public VirtualCondition
{
public:
    explicit VirtualPredicate(T value) : value_(value) { }

    bool Evaluate(const Player *player, const Targets *targets) const override
    {
        return value_.Evaluate(player, targets);
    }

private:
    T value_;
};

auto CreateCondition(int kind, const auto &store)
{
    switch (kind) {
        case 1: return store(CooldownConditionProxy(0));
        case 2: return store(ManaConditionProxy(30));
        case 3: return store(LifeConditionProxy(20));
        default: return store(BuffConditionProxy(50));
    }
}

static void BM_Bind(benchmark::State &state)
{
    const Player               PLAYER;
    const Targets              TARGETS;
    std::vector<ConditionFunc> conditions;
    for (int i = 0; i < 4; ++i) {
        conditions.push_back(CreateBindCondition(i));
    }

    for (const auto _ : state) {
        bool result = true;
        for (const auto &condition : conditions) {
            result &= condition(&PLAYER, &TARGETS);
        }
        benchmark::DoNotOptimize(result);
    }
}

static void MeasureConditions(benchmark::State &state, const auto &store)
{
    const Player                                     PLAYER;
    const Targets                                    TARGETS;
    std::vector<decltype(CreateCondition(0, store))> conditions;
    conditions.reserve(4);
    for (int i = 0; i < 4; ++i) {
        conditions.push_back(CreateCondition(i, store));
    }

    for (const auto _ : state) {
        bool result = true;
        for (const auto &condition : conditions) {
            result &= condition->Evaluate(&PLAYER, &TARGETS);
        }
        benchmark::DoNotOptimize(result);
    }
}

static void BM_Proxy(benchmark::State &state)
{
    MeasureConditions(state, [](auto predicate) { return pro::make_proxy_inplace<spec::Evaluatable>(predicate); });
}

static void BM_Virtual(benchmark::State &state)
{
    MeasureConditions(state, []<class T>(T predicate) -> std::unique_ptr<VirtualCondition> {
        return std::make_unique<VirtualPredicate<T>>(predicate);
    });
}

BENCHMARK(BM_Bind);
BENCHMARK(BM_Virtual);
BENCHMARK(BM_Proxy);

BENCHMARK_MAIN();