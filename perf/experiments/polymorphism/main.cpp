#include <proxy/v4/proxy.h>

#include <memory>
#include <random>
#include <vector>

#include <benchmark/benchmark.h>

// 虚函数基类
class Base
{
public:
    virtual int Calculate(int x) const = 0;
    virtual ~Base()                    = default;
};

// 具体实现类
class Derived : public Base
{
public:
    int Calculate(int x) const override final { return x * 2; }
};

class Derived2 : public Base
{
public:
    int Calculate(int x) const override final { return x * 2; }
};

class DerivedProxy
{
public:
    int Calculate(int x) const { return x * 2; }
};

// CRTP基类
template <typename Derived>
class CRTPBase
{
public:
    int Calculate(int x) const { return static_cast<const Derived *>(this)->CalculateImpl(x); }
};

// CRTP派生类
class CRTPDerived : public CRTPBase<CRTPDerived>
{
public:
    int CalculateImpl(int x) const { return x * 2; }
};

// Proxy 定义
namespace spec {

PRO_DEF_MEM_DISPATCH(MemCalculate, Calculate);

struct Calculatable : pro::facade_builder::add_convention<MemCalculate, int(int)>::build
{
};

} // namespace spec

// 随机数生成器
std::random_device              rd;
std::mt19937                    gen(rd());
std::uniform_int_distribution<> dis(1, 100);

// 创建随机对象的函数
std::unique_ptr<Base> CreateRandomVirtual()
{
    return std::make_unique<Derived>();
}

CRTPDerived CreateRandomCRTP()
{
    return CRTPDerived();
}

auto CreateRandomProxy()
{
    return pro::make_proxy<spec::Calculatable>(DerivedProxy());
}

// Benchmark 函数
static void BM_Virtual(benchmark::State &state)
{
    std::vector<std::unique_ptr<Base>> objects;
    for (int i = 0; i < 4; ++i) {
        objects.push_back(CreateRandomVirtual());
    }

    for (auto _ : state) {
        int x = dis(gen);
        for (auto &obj : objects) {
            benchmark::DoNotOptimize(obj->Calculate(x));
        }
    }
}

// CRTP基准测试
static void BM_CRTP(benchmark::State &state)
{
    std::vector<CRTPDerived> objects;
    for (int i = 0; i < 4; ++i) {
        objects.push_back(CreateRandomCRTP());
    }

    for (auto _ : state) {
        int x = dis(gen);
        for (auto &obj : objects) {
            benchmark::DoNotOptimize(obj.Calculate(x));
        }
    }
}

static void BM_Proxy(benchmark::State &state)
{
    std::vector<decltype(CreateRandomProxy())> objects;
    for (int i = 0; i < 4; ++i) {
        objects.emplace_back(CreateRandomProxy());
    }

    for (auto _ : state) {
        int x = dis(gen);
        for (auto &obj : objects) {
            benchmark::DoNotOptimize(obj->Calculate(x));
        }
    }
}

BENCHMARK(BM_CRTP);
BENCHMARK(BM_Virtual);
BENCHMARK(BM_Proxy);

BENCHMARK_MAIN();