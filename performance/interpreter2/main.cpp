#include <functional>
#include <random>
#include <vector>

#include <benchmark/benchmark.h>
#include <proxy/proxy.h>

// 模拟的上下文类
class Context
{
private:
    mutable std::mt19937                    gen{ 42 }; // 固定种子确保可重复性
    mutable std::uniform_int_distribution<> dist{ 0, 99 };

public:
    bool GetSkillCooldown(int skill_id) const { return skill_id % 2 == 0; }

    bool GetBuffDuration(int buff_id) const { return buff_id % 3 == 0; }

    int GetMana() const { return dist(gen); }

    int GetLife() const { return dist(gen); }
};

Context context;

// Proxy 实现
struct SkillNotinCdProxy
{
    int skill_id;

    bool operator()() const { return context.GetSkillCooldown(skill_id); }
};

struct BuffDurationProxy
{
    int buff_id;

    bool operator()() const { return context.GetBuffDuration(buff_id); }
};

struct ManaLevelProxy
{
    int threshold;

    bool operator()() const { return context.GetMana() >= threshold; }
};

struct LifeLevelProxy
{
    int threshold;

    bool operator()() const { return context.GetLife() > threshold; }
};

using Evaluator = pro::facade_builder::add_convention<pro::operator_dispatch<"()">, bool() const>::build;

// 用于 std::bind 的函数
bool SkillNotinCd(int skill_id)
{
    return context.GetSkillCooldown(skill_id);
}

bool BuffDuration(int buff_id)
{
    return context.GetBuffDuration(buff_id);
}

bool ManaLevel(int mana_threshold)
{
    return context.GetMana() >= mana_threshold;
}

bool LifeLevel(int life_threshold)
{
    return context.GetLife() > life_threshold;
}

// 创建代理条件
auto CreateProxyCondition(int kind)
{
    switch (kind) {
        case 0: return pro::make_proxy_inplace<Evaluator, SkillNotinCdProxy>(50);
        case 1: return pro::make_proxy_inplace<Evaluator, BuffDurationProxy>(20);
        case 2: return pro::make_proxy_inplace<Evaluator, ManaLevelProxy>(100);
        case 3: return pro::make_proxy_inplace<Evaluator, LifeLevelProxy>(80);
    }
    return pro::make_proxy_inplace<Evaluator, SkillNotinCdProxy>(50); // 默认返回
}

// 创建绑定条件
auto CreateBindCondition(int kind)
{
    switch (kind) {
        case 0: return std::bind(SkillNotinCd, 50);
        case 1: return std::bind(BuffDuration, 20);
        case 2: return std::bind(ManaLevel, 100);
        case 3: return std::bind(LifeLevel, 80);
    }
    return std::bind(SkillNotinCd, 50); // 默认返回
}

// Benchmark for Proxy
static void BM_Proxy(benchmark::State &state)
{
    context = Context{};
    std::vector<pro::proxy<Evaluator>> conditions;
    for (int i = 0; i < 4; ++i) {
        conditions.push_back(CreateProxyCondition(i));
    }

    for (const auto _ : state) {
        bool result = true;
        for (const auto &condition : conditions) {
            result &= (*condition)();
        }
        benchmark::DoNotOptimize(result);
    }
}

// Benchmark for std::bind
static void BM_Bind(benchmark::State &state)
{
    context = Context{};
    std::vector<std::function<bool()>> conditions;
    for (int i = 0; i < 4; ++i) {
        conditions.emplace_back(CreateBindCondition(i));
    }

    for (const auto _ : state) {
        bool result = true;
        for (const auto &condition : conditions) {
            result &= condition();
        }
        benchmark::DoNotOptimize(result);
    }
}

BENCHMARK(BM_Proxy);
BENCHMARK(BM_Bind);

BENCHMARK_MAIN();