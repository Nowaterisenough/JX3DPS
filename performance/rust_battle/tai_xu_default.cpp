// Matched C++ control for the standalone Rust port: the same fixed macro,
// active rules, two attribute profiles, deferred damage and indexed timer heap.
#include <windows.h>

#include <chrono>
#include <iostream>

#include "src/class/tai_xu_jian_yi/runtime_rules.hpp"

#include "observation.hpp"
using namespace battle_bench;
namespace tx = JX3DPS::runtime::tai_xu;

struct FixedFormula
{
    uint16_t skill, sub, level;
    uint8_t  kind;
    int64_t  fixed, weapon, count;
    double   coefficient;
    int64_t  surplus, bonus, power;
};

struct FixedAttributes
{
    int64_t attack, weapon, bonus, mitigation, power, strain, pve, vulnerable;
    double  critical_chance;
};

#include "tx_data.hpp"

struct TimerDescription
{
    using Id = DeadlineQueue::TimerId;

    struct Timer
    {
        TimerHandle handle;
        uint16_t    priority;
    };

    std::vector<Timer> descriptors;
    State              state;
    DamageIntentLog    log;

    State &MutableState() { return state; }

    DamageIntentLog &Log() { return log; }

    TimerDescription &Timers() { return *this; }

    Id Create(TimerHandle h, uint16_t priority = 0)
    {
        descriptors.push_back({ h, priority });
        return descriptors.size() - 1;
    }

    Id find(TimerKind kind, Slot slot = INVALID_SLOT) const
    {
        for (Id i = 0; i < descriptors.size(); ++i) {
            const auto h = descriptors[i].handle;
            if (h.kind == kind && h.slot == slot && !h.target) {
                return i;
            }
        }
        throw std::logic_error("missing timer");
    }
};

struct Fixed
{
    DeadlineQueue           queue;
    DamageIntentLog         log{ 4096, 8192 };
    std::array<uint32_t, 5> cd_timer{};
    std::array<int32_t, 5>  ready{};
    uint32_t                gcd_timer, regen, tick, expire, generation = 0;
    int                     gcd = 0, points = 10, charges = 2, recharge = 0, purple_end = 0;
    std::vector<uint16_t>   profiles;
    tx::SplitMixRolls       rng;

    struct Entry
    {
        uint32_t generation, version, roll;
        int64_t  damage;
    };

    std::array<Entry, 1024> cache{};

    explicit Fixed(const TimerDescription &m) :
        gcd_timer(m.find(TimerKind::MacroWake)), regen(m.find(TimerKind::ResourceRegen)),
        tick(m.find(TimerKind::BuffInterval, tx::Purple)), expire(m.find(TimerKind::BuffDuration, tx::Purple))
    {
        for (auto desc : m.descriptors) {
            queue.Create(desc.handle, desc.priority);
        }
        for (Slot i = 0; i < 5; ++i) {
            cd_timer[i] = m.find(TimerKind::SkillCooldown, i);
        }
        profiles.reserve(4096);
    }

    DamageIntentLog &Log() { return log; }

    Fixed &GetRules() { return *this; }

    void qidian(int n)
    {
        points = std::clamp(n, 0, 10);
        log.RecordResources(queue.Now(), points, 0, 0);
    }

    void cooldown(Slot s, int at)
    {
        ready[s] = at;
        log.RecordSkillCooldown(queue.Now(), s, at);
        queue.ArmAt(cd_timer[s], at);
    }

    void hit(uint16_t effect)
    {
        const auto profile = uint16_t(purple_end != 0);
        if (profiles.empty() || profiles.back() != profile) {
            if (profiles.size() == 4096) {
                throw std::length_error("attribute versions");
            }
            profiles.push_back(profile);
        }
        const auto  version = static_cast<uint32_t>(profiles.size() - 1);
        const auto &f       = formulas[effect];
        const bool  zero    = f.kind == static_cast<uint8_t>(tx::FormulaKind::Zero);
        const auto  key     = zero ? 0 : rng.Next();
        const bool  crit    = !zero && static_cast<double>(key >> 11) * 0x1p-53 < attrs[profile].critical_chance;
        log.Record(queue.Now(), f.skill, zero ? INVALID_SLOT : 0, f.level, key, 0, 0, crit ? RollResult::DOUBLE : RollResult::HIT, effect, profile, f.sub, version, version);
    }

    value_t Reduce(const DamageIntent &h)
    {
        const auto roll = 2u * h.effect + (h.outcome == RollResult::DOUBLE);
        auto      &e    = cache[(h.snapshot_version * 0x9e3779b1u ^ h.live_version * 0x85ebca6bu ^ roll * 17u) & 1023];
        if (e.generation != generation || e.version != h.snapshot_version || e.roll != roll) {
            const auto &a = attrs[profiles[h.snapshot_version]];
            const auto &f = formulas[h.effect];
            int64_t     raw =
                f.kind == 1 ? f.surplus : f.fixed + static_cast<int64_t>(a.attack * f.coefficient) + a.weapon * f.weapon / 1024;
            raw = raw * (1024 + a.bonus + f.bonus) / 1024 * f.count * a.mitigation / 1024;
            if (h.outcome == RollResult::DOUBLE) {
                raw = static_cast<int64_t>(raw + raw * .75 + raw * (a.power + f.power) / 1024);
            }
            raw = static_cast<int64_t>(raw * level_multiplier);
            const auto damage = f.kind == 3 ? 0 : raw * a.strain / 1024 * (1024 + a.pve) / 1024 * (1024 + a.vulnerable) / 1024;
            e = { generation, h.snapshot_version, roll, damage };
        }
        return e.damage;
    }

    void on_timer(uint32_t id)
    {
        const auto now = queue.Now();
        if (id == regen) {
            if (points < 10) {
                qidian(points + 1);
            }
            queue.ArmAt(id, now + 16);
        } else if (id == tick) {
            qidian(points + 2);
            log.RecordBuff(now, false, tx::Purple, purple_end, now + 16, 1);
            queue.ArmAt(id, now + 16);
        } else if (id == expire) {
            purple_end = 0;
            log.RecordBuff(now, false, tx::Purple, 0, 0, 0);
            queue.Cancel(tick);
        } else if (id == cd_timer[tx::ZiQi]) {
            log.RecordSkillEnergy(now, tx::ZiQi, ++charges);
            if (charges < 2) {
                recharge = now + 1200;
                queue.ArmAt(id, recharge);
            } else {
                recharge = 0;
            }
            ready[tx::ZiQi] = 0;
            log.RecordSkillCooldown(now, tx::ZiQi, 0);
        }
    }

    bool cast(uint32_t line, Slot skill)
    {
        const auto now = queue.Now();
        if (ready[skill] > now || (skill <= tx::SanHuan && gcd > now)) {
            return false;
        }
        if ((line == 0 && (points >= 6 || purple_end || !charges)) || (line == 1 && points < 8) || (line == 5 && points < 1))
        {
            return false;
        }
        if (skill <= tx::SanHuan) {
            gcd = now + 24;
            log.RecordGcd(now, 0, gcd);
            queue.ArmAt(gcd_timer, gcd);
        }
        switch (skill) {
            case tx::ZiQi:
                if (charges == 2) {
                    recharge = now + 1200;
                    queue.ArmAt(cd_timer[skill], recharge);
                }
                log.RecordSkillEnergy(now, skill, --charges);
                ready[skill] = charges ? 0 : recharge;
                log.RecordSkillCooldown(now, skill, ready[skill]);
                qidian(10);
                purple_end = now + 160;
                log.RecordBuff(now, false, tx::Purple, purple_end, now + 16, 1);
                queue.ArmAt(tick, now + 16);
                queue.ArmAt(expire, purple_end);
                hit(tx::ZeroEffect);
                break;
            case tx::WuWo:
            {
                const auto qi = points;
                qidian(0);
                hit(qi - 1);
                if (qi >= 6) {
                    hit(tx::PoZhaoEffect + (qi - 6) / 2);
                }
                break;
            }
            case tx::BaHuang:
                cooldown(skill, now + 192);
                qidian(points + 2);
                hit(tx::BaHuangEffect);
                hit(tx::BaHuangExtraEffect);
                break;
            case tx::SanHuan:
                cooldown(skill, now + 32);
                qidian(points + 2);
                for (int j = 0; j < 3; ++j) {
                    hit(tx::SanHuanEffect);
                }
                break;
            case tx::SanChai:
                cooldown(skill, now + 22);
                hit(tx::SanChaiEffect);
                break;
            default: throw std::logic_error("unsupported skill");
        }
        return true;
    }

    template <bool Trace>
    void fight(uint64_t i, Result &result, Audit &a)
    {
        ready.fill(0);
        queue.Reset();
        log.Clear();
        profiles.clear();
        ++generation;
        gcd = recharge = purple_end = 0;
        points                      = 10;
        charges                     = 2;
        auto seed                   = IterationSeed(42, i);
        rng.Reset(seed);
        queue.ArmAt(regen, tx::NaturalQidianPhase{}(seed));
        if constexpr (Trace) {
            a.write('F', i);
        }
        for (;;) {
            queue.DispatchDue([&](TimerHandle h, uint32_t id) {
                if constexpr (Trace) {
                    a.write('T', queue.Now(), id, h.kind, h.slot, h.target);
                }
                on_timer(id);
            });
            constexpr std::array<Slot, 6> skills{ tx::ZiQi, tx::WuWo, tx::BaHuang, tx::SanHuan, tx::SanChai, tx::WuWo };
            bool                          success = false;
            for (uint32_t j = 0; j < skills.size(); ++j) {
                if (cast(j, skills[j])) {
                    if constexpr (Trace) {
                        a.write('C', queue.Now(), j, skills[j], Command::Cast);
                    }
                    success = true;
                    break;
                }
            }
            if (success) {
                continue;
            }
            const auto delta = queue.NextDelta();
            if (!delta || *delta >= 4800 - queue.Now()) {
                break;
            }
            queue.Advance(*delta);
        }
        int64_t total = 0;
        for (const auto &h : log.Intents()) {
            const auto damage = Reduce(h);
            result.skills[h.skill].add(damage, h.outcome);
            total += damage;
        }
        result.batch.Add(total, i);
        if constexpr (Trace) {
            dump(*this, a);
        }
    }
};

int main(int argc, char **argv)
{
    if (argc < 3 || argc > 4) {
        return 2;
    }
    const uint32_t n = std::stoul(argv[1]), workers = std::stoul(argv[2]);
    if (!workers || workers > n || (argc == 4 && workers != 1)) {
        return 2;
    }
    if (workers == 1 && !SetThreadAffinityMask(GetCurrentThread(), 4)) {
        return 3;
    }
    Audit audit;
    if (argc == 4) {
        audit.stream.exceptions(std::ios::failbit | std::ios::badbit);
        audit.stream.open(argv[3], std::ios::binary);
    }
    auto             prepared = tx::Prepare();
    TimerDescription meta;
    meta.state.Resize(tx::SkillCount, tx::BuffCount);
    tx::Rules(prepared).Setup(meta);
    std::vector<Result> results(workers);
    auto                start  = std::chrono::steady_clock::now();
    auto                worker = [&](unsigned w) {
        Fixed fixed(meta);
        auto &r = results[w];
        r.skills.resize(tx::SkillCount);
        for (uint64_t i = uint64_t(n) * w / workers; i < uint64_t(n) * (w + 1) / workers; ++i) {
            if (argc == 4) {
                fixed.fight<true>(i, r, audit);
            } else {
                fixed.fight<false>(i, r, audit);
            }
        }
    };
    {
        std::vector<std::jthread> pool;
        for (unsigned w = 1; w < workers; ++w) {
            pool.emplace_back(worker, w);
        }
        worker(0);
    }
    std::array<Skill, tx::SkillCount> skills{};
    BatchStats                        stats;
    for (const auto &r : results) {
        stats.Merge(r.batch);
        for (size_t i = 0; i < skills.size(); ++i) {
            skills[i].damage += r.skills[i].damage;
            for (int j = 0; j < 4; ++j) {
                skills[i].outcomes[j] += r.skills[i].outcomes[j];
            }
        }
    }
    std::cout << std::chrono::duration<double>(std::chrono::steady_clock::now() - start).count() << ' ' << stats.count
              << ' ' << stats.checksum;
    for (auto &s : skills) {
        std::cout << ' ' << s.damage;
        for (auto x : s.outcomes) {
            std::cout << ',' << x;
        }
    }
    std::cout << '\n';
}
