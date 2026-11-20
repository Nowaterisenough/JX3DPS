// Matched C++ control for the standalone Rust port: the same fixed macro,
// active rules, two attribute profiles, deferred damage and indexed timer heap.
#include <windows.h>

#include <array>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <vector>

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
            const auto H = descriptors[i].handle;
            if (H.kind == kind && H.slot == slot && !H.target) {
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
        gcd_timer(m.find(TimerKind::MACRO_WAKE)), regen(m.find(TimerKind::RESOURCE_REGEN)),
        tick(m.find(TimerKind::BUFF_INTERVAL, tx::PURPLE)), expire(m.find(TimerKind::BUFF_DURATION, tx::PURPLE))
    {
        for (const auto DESC : m.descriptors) {
            queue.Create(DESC.handle, DESC.priority);
        }
        for (Slot i = 0; i < 5; ++i) {
            cd_timer[i] = m.find(TimerKind::SKILL_COOLDOWN, i);
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
        const auto PROFILE = uint16_t(purple_end != 0);
        if (profiles.empty() || profiles.back() != PROFILE) {
            if (profiles.size() == 4096) {
                throw std::length_error("attribute versions");
            }
            profiles.push_back(PROFILE);
        }
        const auto  VERSION = static_cast<uint32_t>(profiles.size() - 1);
        const auto &f       = formulas[effect];
        const bool  ZERO    = f.kind == static_cast<uint8_t>(tx::FormulaKind::ZERO);
        const auto  KEY     = ZERO ? 0 : rng.Next();
        const bool  crit    = !ZERO && static_cast<double>(KEY >> 11) * 0x1p-53 < attrs[PROFILE].critical_chance;
        log.Record(queue.Now(), f.skill, ZERO ? INVALID_SLOT : 0, f.level, KEY, 0, 0, crit ? RollResult::DOUBLE : RollResult::HIT, effect, PROFILE, f.sub, VERSION, VERSION);
    }

    value_t Reduce(const DamageIntent &h)
    {
        const auto ROLL = (2u * h.effect) + static_cast<unsigned int>(h.outcome == RollResult::DOUBLE);
        auto &e = cache[((h.snapshot_version * 0x9e3779b1u) ^ (h.live_version * 0x85ebca6bu) ^ (ROLL * 17u)) & 1023];
        if (e.generation != generation || e.version != h.snapshot_version || e.roll != ROLL) {
            const auto &a = attrs[profiles[h.snapshot_version]];
            const auto &f = formulas[h.effect];
            int64_t     raw =
                f.kind == 1 ? f.surplus : f.fixed + static_cast<int64_t>(a.attack * f.coefficient) + (a.weapon * f.weapon / 1024);
            raw = raw * (1024 + a.bonus + f.bonus) / 1024 * f.count * a.mitigation / 1024;
            if (h.outcome == RollResult::DOUBLE) {
                raw = static_cast<int64_t>(raw + (raw * .75) + (raw * (a.power + f.power) / 1024));
            }
            raw = static_cast<int64_t>(raw * level_multiplier);
            const auto DAMAGE = f.kind == 3 ? 0 : raw * a.strain / 1024 * (1024 + a.pve) / 1024 * (1024 + a.vulnerable) / 1024;
            e = { generation, h.snapshot_version, ROLL, DAMAGE };
        }
        return e.damage;
    }

    void on_timer(uint32_t id)
    {
        const auto NOW = queue.Now();
        if (id == regen) {
            if (points < 10) {
                qidian(points + 1);
            }
            queue.ArmAt(id, NOW + 16);
        } else if (id == tick) {
            qidian(points + 2);
            log.RecordBuff(NOW, false, tx::PURPLE, purple_end, NOW + 16, 1);
            queue.ArmAt(id, NOW + 16);
        } else if (id == expire) {
            purple_end = 0;
            log.RecordBuff(NOW, false, tx::PURPLE, 0, 0, 0);
            queue.Cancel(tick);
        } else if (id == cd_timer[tx::ZI_QI]) {
            log.RecordSkillEnergy(NOW, tx::ZI_QI, ++charges);
            if (charges < 2) {
                recharge = NOW + 1200;
                queue.ArmAt(id, recharge);
            } else {
                recharge = 0;
            }
            ready[tx::ZI_QI] = 0;
            log.RecordSkillCooldown(NOW, tx::ZI_QI, 0);
        }
    }

    bool cast(uint32_t line, Slot skill)
    {
        const auto NOW = queue.Now();
        if (ready[skill] > NOW || (skill <= tx::SAN_HUAN && gcd > NOW)) {
            return false;
        }
        if ((line == 0 && (points >= 6 || (purple_end != 0) || (charges == 0))) || (line == 1 && points < 8) ||
            (line == 5 && points < 1))
        {
            return false;
        }
        if (skill <= tx::SAN_HUAN) {
            gcd = NOW + 24;
            log.RecordGcd(NOW, 0, gcd);
            queue.ArmAt(gcd_timer, gcd);
        }
        switch (skill) {
            case tx::ZI_QI:
                if (charges == 2) {
                    recharge = NOW + 1200;
                    queue.ArmAt(cd_timer[skill], recharge);
                }
                log.RecordSkillEnergy(NOW, skill, --charges);
                ready[skill] = (charges != 0) ? 0 : recharge;
                log.RecordSkillCooldown(NOW, skill, ready[skill]);
                qidian(10);
                purple_end = NOW + 160;
                log.RecordBuff(NOW, false, tx::PURPLE, purple_end, NOW + 16, 1);
                queue.ArmAt(tick, NOW + 16);
                queue.ArmAt(expire, purple_end);
                hit(tx::ZeroEffect);
                break;
            case tx::WU_WO:
            {
                const auto QI = points;
                qidian(0);
                hit(QI - 1);
                if (QI >= 6) {
                    hit(tx::PoZhaoEffect + ((QI - 6) / 2));
                }
                break;
            }
            case tx::BA_HUANG:
                cooldown(skill, NOW + 192);
                qidian(points + 2);
                hit(tx::BaHuangEffect);
                hit(tx::BaHuangExtraEffect);
                break;
            case tx::SAN_HUAN:
                cooldown(skill, NOW + 32);
                qidian(points + 2);
                for (int j = 0; j < 3; ++j) {
                    hit(tx::SanHuanEffect);
                }
                break;
            case tx::SAN_CHAI:
                cooldown(skill, NOW + 22);
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
            constexpr std::array<Slot, 6> SKILLS{ tx::ZI_QI,    tx::WU_WO,    tx::BA_HUANG,
                                                  tx::SAN_HUAN, tx::SAN_CHAI, tx::WU_WO };
            bool                          success = false;
            for (uint32_t j = 0; j < SKILLS.size(); ++j) {
                if (cast(j, SKILLS[j])) {
                    if constexpr (Trace) {
                        a.write('C', queue.Now(), j, SKILLS[j], Command::CAST);
                    }
                    success = true;
                    break;
                }
            }
            if (success) {
                continue;
            }
            const auto DELTA = queue.NextDelta();
            if (!DELTA || *DELTA >= 4800 - queue.Now()) {
                break;
            }
            queue.Advance(*DELTA);
        }
        int64_t total = 0;
        for (const auto &h : log.Intents()) {
            const auto DAMAGE = Reduce(h);
            result.skills[h.skill].add(DAMAGE, h.outcome);
            total += DAMAGE;
        }
        result.batch.Add(total, i);
        if constexpr (Trace) {
            dump(*this, a);
        }
    }
};

int main(int argc, char *const *argv)
{
    if (argc < 3 || argc > 4) {
        return 2;
    }
    const uint32_t N       = std::stoul(argv[1]);
    const uint32_t WORKERS = std::stoul(argv[2]);
    if ((WORKERS == 0u) || WORKERS > N || (argc == 4 && WORKERS != 1)) {
        return 2;
    }
    if (WORKERS == 1 && (SetThreadAffinityMask(GetCurrentThread(), 4) == 0u)) {
        return 3;
    }
    Audit audit;
    if (argc == 4) {
        audit.stream.exceptions(std::ios::failbit | std::ios::badbit);
        audit.stream.open(argv[3], std::ios::binary);
    }
    const auto       PREPARED = tx::Prepare();
    TimerDescription meta;
    meta.state.Resize(tx::SKILL_COUNT, tx::BUFF_COUNT);
    tx::Rules(PREPARED).Setup(meta);
    std::vector<Result> results(WORKERS);
    const auto          start  = std::chrono::steady_clock::now();
    const auto          WORKER = [&](unsigned w) {
        Fixed fixed(meta);
        auto &r = results[w];
        r.skills.resize(tx::SKILL_COUNT);
        for (uint64_t i = uint64_t(N) * w / WORKERS; i < uint64_t(N) * (w + 1) / WORKERS; ++i) {
            if (argc == 4) {
                fixed.fight<true>(i, r, audit);
            } else {
                fixed.fight<false>(i, r, audit);
            }
        }
    };
    {
        std::vector<std::jthread> pool;
        for (unsigned w = 1; w < WORKERS; ++w) {
            pool.emplace_back(WORKER, w);
        }
        WORKER(0);
    }
    std::array<Skill, tx::SKILL_COUNT> skills{};
    BatchStats                         stats;
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
        for (const auto X : s.outcomes) {
            std::cout << ',' << X;
        }
    }
    std::cout << '\n';
}
