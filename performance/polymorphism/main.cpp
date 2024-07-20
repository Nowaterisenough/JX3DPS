#include <proxy.h>

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

struct Calculatable : pro::facade_builder ::add_convention<MemCalculate, int(int)>::build
{
};

} // namespace spec

// Benchmark 函数
static void BM_Virtual(benchmark::State &state)
{
    Derived d;
    Base   *b = &d;
    for (auto _ : state) {
        benchmark::DoNotOptimize(b->Calculate(42));
    }
}

// CRTP基准测试
static void BM_CRTP(benchmark::State &state)
{
    CRTPDerived d;
    for (auto _ : state) {
        benchmark::DoNotOptimize(d.Calculate(42));
    }
}

static void BM_Proxy(benchmark::State &state)
{
    auto p = pro::make_proxy<spec::Calculatable>(DerivedProxy());
    for (auto _ : state) {
        benchmark::DoNotOptimize(p->Calculate(42));
    }
}

BENCHMARK(BM_CRTP);
BENCHMARK(BM_Virtual);
BENCHMARK(BM_Proxy);

BENCHMARK_MAIN();