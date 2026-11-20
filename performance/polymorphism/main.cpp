#include <array>
#include <memory>
#include <tuple>

#include <benchmark/benchmark.h>
#include <proxy/proxy.h>

namespace {

template <typename Derived>
struct Operation
{
    int Calculate(int x) const { return static_cast<const Derived *>(this)->CalculateImpl(x); }
};

struct Double : Operation<Double>
{
    int CalculateImpl(int x) const { return x * 2; }
};

struct Increment : Operation<Increment>
{
    int CalculateImpl(int x) const { return x + 1; }
};

constexpr auto OPERATIONS = std::tuple{ Double{}, Increment{}, Increment{}, Double{} };

PRO_DEF_MEM_DISPATCH(MemCalculate, Calculate);
using Calculatable = pro::facade_builder::add_convention<MemCalculate, int(int) const>::build;

void MeasureProxy(benchmark::State &state, const auto &store)
{
    const auto objects = std::apply([&](auto... op) { return std::array{ store(op)... }; }, OPERATIONS);
    int        input   = 42;
    for (auto _ : state) {
        benchmark::DoNotOptimize(input);
        for (const auto &object : objects) {
            benchmark::DoNotOptimize(object->Calculate(input));
        }
    }
}

void BM_HeapProxy(benchmark::State &state)
{
    MeasureProxy(state, []<class T>(T op) -> pro::proxy<Calculatable> { return std::make_unique<T>(op); });
}

void BM_Proxy(benchmark::State &state)
{
    MeasureProxy(state, [](auto op) { return pro::make_proxy_inplace<Calculatable>(op); });
}

void BM_CRTP(benchmark::State &state)
{
    int input = 42;
    for (auto _ : state) {
        benchmark::DoNotOptimize(input);
        std::apply([&](const auto &...op) { (benchmark::DoNotOptimize(op.Calculate(input)), ...); }, OPERATIONS);
    }
}

BENCHMARK(BM_CRTP);
BENCHMARK(BM_HeapProxy);
BENCHMARK(BM_Proxy);

} // namespace

BENCHMARK_MAIN();
