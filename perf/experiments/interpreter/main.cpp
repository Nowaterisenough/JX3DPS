#include <proxy/v4/proxy.h>

#include <functional>
#include <memory>
#include <random>
#include <vector>

#include <benchmark/benchmark.h>

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

ConditionFunc CreateRandomBindCondition()
{
    switch (rand() % 4) {
        case 0:
            return std::bind(CheckBuffDuration, std::placeholders::_1, std::placeholders::_2, 50);
        case 1:
            return std::bind(CheckSkillCooldown, std::placeholders::_1, std::placeholders::_2, 0);
        case 2: return std::bind(CheckManaLevel, std::placeholders::_1, std::placeholders::_2, 30);
        case 3: return std::bind(CheckLifeLevel, std::placeholders::_1, std::placeholders::_2, 20);
    }
    return nullptr;
}

// 虚函数方法
class Condition
{
public:
    virtual bool Evaluate(const Player *player, const Targets *targets) const = 0;
    virtual ~Condition()                                                      = default;
};

class BuffCondition : public Condition
{
private:
    int _duration;

public:
    BuffCondition(int duration) : _duration(duration) { }

    bool Evaluate(const Player *player, const Targets *) const override final
    {
        return player->buff_duration > _duration;
    }
};

class CooldownCondition : public Condition
{
private:
    int _cooldown;

public:
    CooldownCondition(int cooldown) : _cooldown(cooldown) { }

    bool Evaluate(const Player *player, const Targets *) const override final
    {
        return player->skill_cooldown <= _cooldown;
    }
};

class ManaCondition : public Condition
{
private:
    int _mana;

public:
    ManaCondition(int mana) : _mana(mana) { }

    bool Evaluate(const Player *player, const Targets *) const override final
    {
        return player->mana >= _mana;
    }
};

class LifeCondition : public Condition
{
private:
    int _life;

public:
    LifeCondition(int life) : _life(life) { }

    bool Evaluate(const Player *player, const Targets *) const override final
    {
        return player->life > _life;
    }
};

std::unique_ptr<Condition> CreateRandomCondition()
{
    switch (rand() % 4) {
        case 0: return std::make_unique<BuffCondition>(50);
        case 1: return std::make_unique<CooldownCondition>(0);
        case 2: return std::make_unique<ManaCondition>(30);
        case 3: return std::make_unique<LifeCondition>(20);
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

    bool Evaluate(const Player *player, const Targets *) const
    {
        return player->buff_duration > _duration;
    }
};

class CooldownConditionProxy
{
private:
    int _id;
    int _cooldown;

public:
    CooldownConditionProxy(int id, int cooldown) : _id(id), _cooldown(cooldown) { }

    bool Evaluate(const Player *player, const Targets *) const
    {
        return player->skill_cooldown <= _cooldown;
    }
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

struct Evaluatable :
    pro::facade_builder::add_convention<MemEvaluate, bool(const Player *, const Targets *) const>::build
{
};

} // namespace spec

auto CreateRandomProxyCondition()
{
    switch (rand() % 4) {
        case 0: return pro::make_proxy<spec::Evaluatable>(BuffConditionProxy(50));
        case 1: return pro::make_proxy<spec::Evaluatable>(CooldownConditionProxy(50, 0));
        case 2: return pro::make_proxy<spec::Evaluatable>(ManaConditionProxy(30));
        case 3: return pro::make_proxy<spec::Evaluatable>(LifeConditionProxy(20));
    }
    return pro::make_proxy<spec::Evaluatable>(BuffConditionProxy(50)); // 默认返回
}

static void BM_Bind(benchmark::State &state)
{
    Player                     player;
    Targets                    targets;
    std::vector<ConditionFunc> conditions;
    for (int i = 0; i < 4; ++i) {
        conditions.push_back(CreateRandomBindCondition());
    }

    for (auto _ : state) {
        bool result = true;
        for (const auto &condition : conditions) {
            result &= condition(&player, &targets);
        }
        benchmark::DoNotOptimize(result);
    }
}

static void BM_Virtual(benchmark::State &state)
{
    Player                                  player;
    Targets                                 targets;
    std::vector<std::unique_ptr<Condition>> conditions;
    for (int i = 0; i < 4; ++i) {
        conditions.push_back(CreateRandomCondition());
    }

    for (auto _ : state) {
        bool result = true;
        for (const auto &condition : conditions) {
            result &= condition->Evaluate(&player, &targets);
        }
        benchmark::DoNotOptimize(result);
    }
}

static void BM_Proxy(benchmark::State &state)
{
    Player                                              player;
    Targets                                             targets;
    std::vector<decltype(CreateRandomProxyCondition())> conditions;
    for (int i = 0; i < 4; ++i) {
        conditions.push_back(CreateRandomProxyCondition());
    }

    for (auto _ : state) {
        bool result = true;
        for (const auto &condition : conditions) {
            result &= condition->Evaluate(&player, &targets);
        }
        benchmark::DoNotOptimize(result);
    }
}

BENCHMARK(BM_Bind);
BENCHMARK(BM_Virtual);
BENCHMARK(BM_Proxy);

BENCHMARK_MAIN();