#pragma once
#include <array>
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <map>
#include <mutex>
#include <thread>
#include <nlohmann/json.hpp>

namespace JX3DPS::profile {
enum Stage {
    Fight,
    Clone,
    Prepare,
    Advance,
    Update,
    Event,
    SkillTrigger,
    BuffTrigger,
    Macro,
    Cast,
    Rebuild,
    Summary,
    Cleanup,
    Count
};
struct Label {
    const char *name;
    Stage parent;
};
inline constexpr std::array<Label, Count> labels{{{"Simulate", Fight},
                                                  {"Player::Clone / 复制宏", Fight},
                                                  {"GenerateKeyFrameSequence", Fight},
                                                  {"KeyFrameAdvance", Fight},
                                                  {"UpdateKeyFrameSequence", Advance},
                                                  {"执行强制事件", Advance},
                                                  {"Skill::Trigger", Advance},
                                                  {"Buff::Trigger", Advance},
                                                  {"CastSkills", Advance},
                                                  {"Skill::Cast", Macro},
                                                  {"重查和重排关键帧链表", Advance},
                                                  {"SummarizeStats", Fight},
                                                  {"释放角色和目标", Fight}}};
struct alignas(64) Worker {
    std::atomic<int> current{-1};
    std::array<std::uint64_t, 2 * Count> hits{}; // sampler is the sole writer
    std::uint64_t fights = 0;
    std::chrono::steady_clock::duration elapsed{};
};
class Session {
    using Clock = std::chrono::steady_clock;

  public:
    Session()
        : sampler_([this] {
              unsigned random = 73;
              std::unique_lock lock(mutex_);
              while (!done_) {
                  random = 1664525u * random + 1013904223u;
                  auto due = Clock::now() + std::chrono::microseconds(1000 + (random >> 22));
                  if (wake_.wait_until(lock, due, [this] { return done_; }))
                      break;
                  for (auto &[id, worker] : workers_) {
                      const int slot = worker.current.load(std::memory_order_relaxed);
                      if (slot >= 0)
                          ++worker.hits[slot];
                  }
              }
          }) {}
    ~Session() { Stop(); }
    Worker &Register() {
        std::lock_guard lock(mutex_);
        return workers_[std::this_thread::get_id()];
    }
    nlohmann::ordered_json Report() {
        Stop();
        std::array<std::uint64_t, 2 * Count> hits{};
        nlohmann::ordered_json report;
        std::uint64_t fights = 0;
        double ms = 0;
        for (const auto &[id, w] : workers_) {
            fights += w.fights;
            ms += std::chrono::duration<double, std::milli>(w.elapsed).count();
            for (unsigned i = 0; i < hits.size(); ++i)
                hits[i] += w.hits[i];
        }
        std::uint64_t total = 0;
        for (auto n : hits)
            total += n;
        report["fights"] = fights;
        report["workers"] = workers_.size();
        report["samplePoints"] = total;
        report["workerMs"] = ms;
        report["wallMs"] = std::chrono::duration<double, std::milli>(Clock::now() - start_).count();
        auto &tree = report["calls"];
        tree.push_back({{"function", "整批模拟"}, {"parent", 0}, {"inclusive", total}, {"self", 0}});
        for (int pass = 0; pass < 2; ++pass) {
            std::array<std::uint64_t, Count> inclusive{};
            for (int i = Count - 1; i >= 0; --i) {
                inclusive[i] += hits[pass * Count + i];
                if (i)
                    inclusive[labels[i].parent] += inclusive[i];
            }
            if (!inclusive[0])
                continue;
            const auto base = tree.size();
            for (int i = 0; i < Count; ++i)
                tree.push_back(
                    {{"function", i ? labels[i].name : (pass ? "Simulate · 属性重跑" : "Simulate · 基准战斗")},
                     {"parent", i ? base + labels[i].parent : 0},
                     {"inclusive", inclusive[i]},
                     {"self", hits[pass * Count + i]}});
        }
        return report;
    }

  private:
    void Stop() {
        if (!sampler_.joinable())
            return;
        {
            std::lock_guard lock(mutex_);
            done_ = true;
        }
        wake_.notify_one();
        sampler_.join();
    }
    Clock::time_point start_ = Clock::now();
    std::map<std::thread::id, Worker> workers_;
    std::mutex mutex_;
    std::condition_variable wake_;
    bool done_ = false;
    std::thread sampler_;
};
template <bool Enabled> struct Scope {
    Scope(Worker *, Stage) {}
};
template <> struct Scope<true> {
    Worker *worker;
    int previous;
    Scope(Worker *w, Stage stage) : worker(w), previous(w->current.load(std::memory_order_relaxed)) {
        worker->current.store((previous >= Count ? Count : 0) + stage, std::memory_order_relaxed);
    }
    ~Scope() { worker->current.store(previous, std::memory_order_relaxed); }
    Scope(const Scope &) = delete;
};
} // namespace JX3DPS::profile
