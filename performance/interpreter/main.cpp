#include <proxy.h>

#include <functional>
#include <memory>
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

ConditionFunc CreateBuffCondition(int duration)
{
    return std::bind(CheckBuffDuration, std::placeholders::_1, std::placeholders::_2, duration);
}

ConditionFunc CreateCooldownCondition(int cooldown)
{
    return std::bind(CheckSkillCooldown, std::placeholders::_1, std::placeholders::_2, cooldown);
}

ConditionFunc CreateManaCondition(int mana)
{
    return std::bind(CheckManaLevel, std::placeholders::_1, std::placeholders::_2, mana);
}

ConditionFunc CreateLifeCondition(int life)
{
    return std::bind(CheckLifeLevel, std::placeholders::_1, std::placeholders::_2, life);
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
    int _cooldown;

public:
    CooldownConditionProxy(int cooldown) : _cooldown(cooldown) { }

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
    pro::facade_builder ::add_convention<MemEvaluate, bool(const Player *, const Targets *) const>::build
{
};

} // namespace spec

static void BM_Bind(benchmark::State &state)
{
    Player  player;
    Targets targets;
    auto    buff_condition     = CreateBuffCondition(50);
    auto    cooldown_condition = CreateCooldownCondition(0);
    auto    mana_condition     = CreateManaCondition(30);
    auto    life_condition     = CreateLifeCondition(20);

    for (auto _ : state) {
        bool result = buff_condition(&player, &targets) && cooldown_condition(&player, &targets) &&
                      mana_condition(&player, &targets) && life_condition(&player, &targets);
        benchmark::DoNotOptimize(result);
    }
}

static void BM_Virtual(benchmark::State &state)
{
    Player            player;
    Targets           targets;
    BuffCondition     buff_condition(50);
    CooldownCondition cooldown_condition(0);
    ManaCondition     mana_condition(30);
    LifeCondition     life_condition(20);

    for (auto _ : state) {
        bool result =
            buff_condition.Evaluate(&player, &targets) && cooldown_condition.Evaluate(&player, &targets) &&
            mana_condition.Evaluate(&player, &targets) && life_condition.Evaluate(&player, &targets);
        benchmark::DoNotOptimize(result);
    }
}

static void BM_Proxy(benchmark::State &state)
{
    Player  player;
    Targets targets;
    auto    buff_condition     = pro::make_proxy<spec::Evaluatable>(BuffConditionProxy(50));
    auto    cooldown_condition = pro::make_proxy<spec::Evaluatable>(CooldownConditionProxy(0));
    auto    mana_condition     = pro::make_proxy<spec::Evaluatable>(ManaConditionProxy(30));
    auto    life_condition     = pro::make_proxy<spec::Evaluatable>(LifeConditionProxy(20));

    for (auto _ : state) {
        bool result =
            life_condition->Evaluate(&player, &targets) && mana_condition->Evaluate(&player, &targets) &&
            cooldown_condition->Evaluate(&player, &targets) && buff_condition->Evaluate(&player, &targets);
        benchmark::DoNotOptimize(result);
    }
}

BENCHMARK(BM_Bind);
BENCHMARK(BM_Virtual);
BENCHMARK(BM_Proxy);

BENCHMARK_MAIN();