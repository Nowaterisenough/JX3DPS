#pragma once

#include <array>
#include <atomic>
#include <chrono>
#include <exception>
#include <cstdint>
#include <vector>
#include <string_view>

namespace JX3DPS::runtime {

enum class FightPhase { Macro, Availability, Queue, TimerEffect, CastEffect, Record, Advance, Damage, Other, GainReplay, Count };
// Workers own their stacks and counters. A shared sampling tick is the only
// value read across threads. Intern the marked path only when a tick arrives.
struct ProfileFrame {
    const ProfileFrame *parent;
    const char *function;
    FightPhase phase;
};
struct CallTree {
    struct Node {
        const char *function;
        std::uint32_t parent, firstChild, nextSibling;
        std::uint64_t self = 0;
    };
    std::vector<Node> nodes{{"整批模拟", 0, 0, 0}};
    std::uint32_t Child(std::uint32_t parent, const char *function) {
        for (auto i = nodes[parent].firstChild; i; i = nodes[i].nextSibling)
            if (nodes[i].function == function || std::string_view(nodes[i].function) == function) return i;
        const auto id = static_cast<std::uint32_t>(nodes.size());
        nodes.push_back({function, parent, 0, nodes[parent].firstChild});
        nodes[parent].firstChild = id;
        return id;
    }
    std::uint32_t Intern(const ProfileFrame *frame) {
        if (!frame) return 0;
        const auto parent = Intern(frame->parent);
        return frame->function ? Child(parent, frame->function) : parent;
    }
};
struct alignas(64) WorkerProfile {
    const std::atomic<std::uint64_t> *tick = nullptr;
    std::uint64_t lastTick = 0, fights = 0;
    std::chrono::steady_clock::duration elapsed{};
    std::array<std::uint64_t, static_cast<std::size_t>(FightPhase::Count)> phases{};
    CallTree calls;
    const ProfileFrame *top = nullptr;
    std::exception_ptr error;
    inline static thread_local WorkerProfile *active = nullptr;

    void Poll() noexcept {
        const auto now = tick->load(std::memory_order_relaxed);
        if (now == lastTick) return;
        const auto count = now - lastTick;
        lastTick = now;
        // At a scope boundary all intervening ticks belong to the old stack.
        try {
            calls.nodes[calls.Intern(top)].self += count;
            phases[static_cast<std::size_t>(top ? top->phase : FightPhase::Other)] += count;
        } catch (...) { error = std::current_exception(); }
    }
};
template<bool Measure> struct PhaseScope {
    explicit PhaseScope(FightPhase, const char * = nullptr) {}
    explicit PhaseScope(const char *) {}
};

template<> class PhaseScope<true> {
public:
    explicit PhaseScope(FightPhase phase, const char *function = nullptr)
        : worker_(WorkerProfile::active), frame_{worker_->top, function, phase} {
        worker_->Poll();
        worker_->top = &frame_;
    }
    explicit PhaseScope(const char *function)
        : PhaseScope(WorkerProfile::active->top ? WorkerProfile::active->top->phase : FightPhase::Other, function) {}
    ~PhaseScope() {
        worker_->Poll();
        worker_->top = frame_.parent;
    }
    PhaseScope(const PhaseScope &) = delete;
private:
    WorkerProfile *worker_;
    ProfileFrame frame_;
};

template<class Engine> auto RuleFunctionScope(const char *function) {
    if constexpr (requires { typename Engine::CastEffectScope; }) return typename Engine::CastEffectScope(function);
    else return PhaseScope<false>(function);
}

// Rules are also used by standalone test engines without timing support.
template<class Engine> auto CastEffectTimer() {
    if constexpr (requires { typename Engine::CastEffectScope; })
        return typename Engine::CastEffectScope(FightPhase::CastEffect);
    else
        return PhaseScope<false>(FightPhase::CastEffect);
}

} // namespace JX3DPS::runtime
