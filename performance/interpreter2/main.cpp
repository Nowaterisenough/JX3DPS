#include <proxy/v4/proxy.h>

#include <functional>
#include <random>
#include <vector>
#include <memory>

#include <benchmark/benchmark.h>

// 模拟的上下文类
class Context
{
public:
    bool GetSkillCooldown(int skill_id) const { return skill_id % 2 == 0; }
    bool GetBuffDuration(int buff_id) const { return buff_id % 3 == 0; }
    int GetMana() const { return rand() % 100; }
    int GetLife() const { return rand() % 100; }
};

Context context;

// Proxy 实现
template <typename Ret, typename... Args>
class ConditionProxy
{
private:
    std::tuple<Args...> args;

public:
    explicit ConditionProxy(Args... args) : args(std::forward<Args>(args)...) { }

    Ret operator()() const
    {
        return std::apply([this](const Args &...args) { return this->evaluate(args...); }, args);
    }

    virtual Ret evaluate(const Args &...args) const = 0;
};

class SkillNotinCdProxy : public ConditionProxy<bool, int>
{
public:
    explicit SkillNotinCdProxy(int skill_id) : ConditionProxy(skill_id) { }

    bool evaluate(const int &skill_id) const override { return context.GetSkillCooldown(skill_id); }
};

class BuffDurationProxy : public ConditionProxy<bool, int>
{
public:
    explicit BuffDurationProxy(int buff_id) : ConditionProxy(buff_id) { }

    bool evaluate(const int &buff_id) const override { return context.GetBuffDuration(buff_id); }
};

class ManaLevelProxy : public ConditionProxy<bool, int>
{
public:
    explicit ManaLevelProxy(int mana_threshold) : ConditionProxy(mana_threshold) { }

    bool evaluate(const int &mana_threshold) const override { return context.GetMana() >= mana_threshold; }
};

class LifeLevelProxy : public ConditionProxy<bool, int>
{
public:
    explicit LifeLevelProxy(int life_threshold) : ConditionProxy(life_threshold) { }

    bool evaluate(const int &life_threshold) const override { return context.GetLife() > life_threshold; }
};

namespace spec {
PRO_DEF_MEM_DISPATCH(MemEvaluate, operator());

template <typename Ret, typename... Args>
struct Evaluator : pro::facade_builder::add_convention<MemEvaluate, Ret() const>::build
{
};
} // namespace spec

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

// 虚函数方法
class Condition
{
public:
    virtual bool evaluate() const = 0;
    virtual ~Condition() = default;
};

class SkillNotinCdCondition : public Condition
{
private:
    int skill_id;

public:
    explicit SkillNotinCdCondition(int id) : skill_id(id) {}
    bool evaluate() const override { return context.GetSkillCooldown(skill_id); }
};

class BuffDurationCondition : public Condition
{
private:
    int buff_id;

public:
    explicit BuffDurationCondition(int id) : buff_id(id) {}
    bool evaluate() const override { return context.GetBuffDuration(buff_id); }
};

class ManaLevelCondition : public Condition
{
private:
    int mana_threshold;

public:
    explicit ManaLevelCondition(int threshold) : mana_threshold(threshold) {}
    bool evaluate() const override { return context.GetMana() >= mana_threshold; }
};

class LifeLevelCondition : public Condition
{
private:
    int life_threshold;

public:
    explicit LifeLevelCondition(int threshold) : life_threshold(threshold) {}
    bool evaluate() const override { return context.GetLife() > life_threshold; }
};

// 创建随机代理条件
auto CreateRandomProxyCondition()
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis(0, 3);

    switch (dis(gen)) {
        case 0: return pro::make_proxy<spec::Evaluator<bool>>(SkillNotinCdProxy(50));
        case 1: return pro::make_proxy<spec::Evaluator<bool>>(BuffDurationProxy(20));
        case 2: return pro::make_proxy<spec::Evaluator<bool>>(ManaLevelProxy(100));
        case 3: return pro::make_proxy<spec::Evaluator<bool>>(LifeLevelProxy(80));
    }
    return pro::make_proxy<spec::Evaluator<bool>>(SkillNotinCdProxy(rand() % 100)); // 默认返回
}

// 创建随机绑定条件
auto CreateRandomBindCondition()
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis(0, 3);

    switch (dis(gen)) {
        case 0: return std::bind(SkillNotinCd, 50);
        case 1: return std::bind(BuffDuration, 20);
        case 2: return std::bind(ManaLevel, 100);
        case 3: return std::bind(LifeLevel, 80);
    }
    return std::bind(SkillNotinCd, rand() % 100); // 默认返回
}

// 创建随机虚函数条件
std::unique_ptr<Condition> CreateRandomVirtualCondition()
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis(0, 3);

    switch (dis(gen)) {
        case 0: return std::make_unique<SkillNotinCdCondition>(50);
        case 1: return std::make_unique<BuffDurationCondition>(20);
        case 2: return std::make_unique<ManaLevelCondition>(100);
        case 3: return std::make_unique<LifeLevelCondition>(80);
    }
    return std::make_unique<SkillNotinCdCondition>(rand() % 100); // 默认返回
}

// Benchmark for Proxy
static void BM_Proxy(benchmark::State &state)
{
    std::vector<decltype(CreateRandomProxyCondition())> conditions;
    for (int i = 0; i < 4; ++i) {
        conditions.push_back(CreateRandomProxyCondition());
    }

    for (auto _ : state) {
        bool result = true;
        for (const auto &condition : conditions) {
            result &= condition->operator()();
        }
        benchmark::DoNotOptimize(result);
    }
}

// Benchmark for std::bind
static void BM_Bind(benchmark::State &state)
{
    std::vector<std::function<bool()>> conditions;
    for (int i = 0; i < 4; ++i) {
        conditions.push_back(CreateRandomBindCondition());
    }

    for (auto _ : state) {
        bool result = true;
        for (const auto &condition : conditions) {
            result &= condition();
        }
        benchmark::DoNotOptimize(result);
    }
}

// Benchmark for Virtual Function
static void BM_Virtual(benchmark::State &state)
{
    std::vector<std::unique_ptr<Condition>> conditions;
    for (int i = 0; i < 4; ++i) {
        conditions.push_back(CreateRandomVirtualCondition());
    }

    for (auto _ : state) {
        bool result = true;
        for (const auto &condition : conditions) {
            result &= condition->evaluate();
        }
        benchmark::DoNotOptimize(result);
    }
}

BENCHMARK(BM_Proxy);
BENCHMARK(BM_Bind);
BENCHMARK(BM_Virtual);

BENCHMARK_MAIN();