#include "debug_simulator.h"

#include <algorithm>
#include <array>
#include <deque>
#include <exception>
#include <memory>
#include <optional>
#include <utility>
#include <vector>

#include <QDebug>
#include <QStringList>

#include "src/core/runtime/batch.hpp"
#include "src/core/runtime/simulation.hpp"

#include "diagnostics.h"
#include "runtime_backend.h"
#include "traced_rules.h"

namespace {
using Simulator = JX3DPS::runtime::Simulation<desktop::TracedRules<JX3DPS::runtime::tai_xu::Rules>, true>;
namespace tx    = JX3DPS::runtime::tai_xu;
namespace rt    = JX3DPS::runtime;

QString ActionDetails(const rt::ActionDiagnostic &action, bool success, desktop::Specialization specialization)
{
    using Reason = rt::ActionBlockReason;
    QString text;
    switch (action.reason) {
        case Reason::NONE:
            text = success ? QStringLiteral("施放成功（伤害可能延后结算）。") : QStringLiteral("施放失败。");
            break;
        case Reason::PREPARING:
            text = QStringLiteral("正在运功 %1：剩余 %2 帧；强制施放不能跳过运功。")
                       .arg(desktop::SkillName(specialization, action.subject))
                       .arg(action.actual);
            break;
        case Reason::CHANNELING:
            text = QStringLiteral("正在引导 %1：剩余 %2 帧。")
                       .arg(desktop::SkillName(specialization, action.subject))
                       .arg(action.actual);
            break;
        case Reason::SKILL_COOLDOWN: text = QStringLiteral("技能冷却：剩余 %1 帧。").arg(action.actual); break;
        case Reason::GLOBAL_COOLDOWN: text = QStringLiteral("公共冷却：剩余 %1 帧。").arg(action.actual); break;
        case Reason::QIDIAN:
            text = QStringLiteral("当前气点 %1；至少需要 %2。").arg(action.actual).arg(action.required);
            break;
        case Reason::CHARGES:
            text = QStringLiteral("当前充能 %1；至少需要 %2。").arg(action.actual).arg(action.required);
            break;
        case Reason::DISTANCE:
            text = QStringLiteral("目标距离 %1；最大距离 %2。").arg(action.actual).arg(action.required);
            break;
        case Reason::MISSING_TALENT: text = QStringLiteral("未选择该技能所需奇穴。"); break;
        case Reason::MISSING_BUFF: text = QStringLiteral("缺少该技能所需增益。"); break;
    }
    if (action.interrupted != rt::INVALID_SLOT) {
        text.prepend(QStringLiteral("先打断 %1。").arg(desktop::SkillName(specialization, action.interrupted)));
    }
    return text;
}

MacroStep DescribeStep(const rt::MacroObservation &observation, const rt::CompiledMacro &macro, desktop::Specialization specialization)
{
    const auto &source = macro.source[observation.instruction];
    const auto &line   = macro.program.lines[observation.instruction];
    MacroStep   step;
    step.ordinal    = observation.ordinal;
    step.frame      = observation.frame;
    step.line       = static_cast<int>(source.line);
    step.passed     = observation.result;
    step.expression = QString::fromStdString(source.text);
    if (observation.skipped) {
        step.kind    = MacroStepKind::SKIPPED;
        step.outcome = QStringLiteral("跳过");
        step.details = QStringLiteral("顺序指令已在本场战斗中成功执行。");
    } else if (observation.phase == rt::DebugPhase::LINE) {
        step.kind    = MacroStepKind::INTERRUPTION;
        step.passed  = true;
        step.outcome = QStringLiteral("引导已打断");
        step.details = QStringLiteral("进入强制施放行时打断 %1，接下来判定本行条件。")
                           .arg(desktop::SkillName(specialization, observation.action.interrupted));
    } else if (observation.phase == rt::DebugPhase::CONDITION) {
        step.kind                      = MacroStepKind::CONDITION;
        step.expression                = QString::fromStdString(source.conditions[observation.condition]);
        step.outcome                   = observation.result ? QStringLiteral("成立") : QStringLiteral("不成立");
        const auto          &condition = macro.program.conditions[line.condition_begin + observation.condition];
        constexpr std::array COMPARISONS{ "<", "<=", "=", "!=", ">", ">=" };
        const bool TIME = condition.kind == rt::ValueKind::BUFF_DURATION || condition.kind == rt::ValueKind::SKILL_COOLDOWN;
        step.details =
            QStringLiteral("%1 %2 %3%4. ")
                .arg(observation.actual)
                .arg(QString::fromLatin1(COMPARISONS[static_cast<unsigned>(condition.comparator)]))
                .arg(condition.value)
                .arg(TIME ? QStringLiteral("（帧）") : QString{});
        if (observation.next == rt::ACCEPT) {
            step.details += QStringLiteral("条件组成立，尝试施放。");
        } else if (observation.next == rt::REJECT) {
            step.details += rt::IsSequential(line.command) ? QStringLiteral("条件不成立，结束本轮宏。") : QStringLiteral("条件不成立，进入下一行。");
        } else {
            step.details +=
                QStringLiteral("下一个条件：%1%2。")
                    .arg(observation.next + 1)
                    .arg(observation.next > observation.condition + 1 ? QStringLiteral("（短路求值）") : QString{});
        }
    } else {
        step.kind    = MacroStepKind::ACTION;
        step.outcome = observation.result ? QStringLiteral("成功") : QStringLiteral("受阻");
        step.details = ActionDetails(observation.action, observation.result, specialization);
        step.details +=
            observation.result || rt::IsSequential(line.command) ? QStringLiteral(" 结束本轮宏。") : QStringLiteral(" 进入下一行。");
    }
    return step;
}

QString CompileErrors(const std::vector<JX3DPS::runtime::CompileError> &errors)
{
    QStringList lines;
    for (const auto &error : errors) {
        lines << QStringLiteral("第 %1 行：%2").arg(error.line).arg(desktop::Diagnostic(desktop::Text(error.message)));
    }
    return lines.join('\n');
}
} // namespace

struct DebugSimulator::Impl
{
    JX3DPS::runtime::CompiledMacro macro;
    using MoWenSimulator = rt::Simulation<desktop::TracedRules<rt::mo_wen::Rules>, true>;
    std::variant<std::unique_ptr<Simulator>, std::unique_ptr<MoWenSimulator>> simulation;

    template <class F>
    decltype(auto) Visit(F &&function)
    {
        return std::visit(std::forward<F>(function), simulation);
    }

    desktop::Specialization Class() const { return desktop::ClassOf(config); }

    JX3DPS::tick_t                      duration = 16 * 300;
    std::uint64_t                       seed     = 0;
    desktop::Config                     config;
    std::size_t                         eventCursor    = 0;
    std::size_t                         mutationCursor = 0;
    BuffEvents                          initialBuffs;
    std::optional<rt::State>            initialState;
    std::size_t                         historyCursor = 0, attributeCursor = 0;
    std::deque<rt::MacroObservation>    pendingSteps;
    std::optional<rt::MacroObservation> lastStep;

    static void Observe(void *context, const rt::MacroObservation &step)
    {
        auto &self = *static_cast<Impl *>(context);
        if (self.pendingSteps.size() == MacroStepHistoryLimit) {
            self.pendingSteps.pop_front();
        }
        self.pendingSteps.push_back(step);
        self.lastStep = step;
    }
};

DebugSimulator::DebugSimulator(QObject *parent) : QObject(parent), d(std::make_unique<Impl>()) { }

DebugSimulator::~DebugSimulator() = default;

void DebugSimulator::SetOptions(const desktop::Config &config, int duration_frames, std::uint64_t seed)
{
    d->config   = config;
    d->duration = std::max(1, duration_frames);
    d->seed     = JX3DPS::runtime::IterationSeed(seed, 0);
}

bool DebugSimulator::Initialize(const QString &macro_text, QString &error_message)
{
    Reset();
    error_message.clear();

    const auto UTF8 = macro_text.toUtf8();
    auto [compiled, errors] =
        JX3DPS::runtime::MacroCompiler::Compile(std::string_view(UTF8.constData(), static_cast<std::size_t>(UTF8.size())),
                                                desktop::MacroOptions(d->Class(), true));
    if (!errors.empty()) {
        error_message = CompileErrors(errors);
        return false;
    }

    try {
        d->macro = std::move(compiled);
        std::visit(
            [&](const auto &config) {
                auto rules              = desktop::TracedRules(desktop::MakeRules(config));
                using ConcreteSimulator = rt::Simulation<decltype(rules), true>;
                auto simulation =
                    std::make_unique<ConcreteSimulator>(d->macro.program,
                                                        std::move(rules),
                                                        (static_cast<std::size_t>(d->duration) * 16) + 64,
                                                        (static_cast<std::size_t>(d->duration) * 40) + 128);
                simulation->SetDebugObserver(&Impl::Observe, d.get());
                simulation->Start(d->duration, d->seed);
                d->initialBuffs = InitialBuffEvents(simulation->GetState(), d->Class());
                d->initialState = simulation->GetState();
                d->simulation   = std::move(simulation);
            },
            d->config);
    } catch (const std::exception &error) {
        error_message = desktop::Diagnostic(QString::fromUtf8(error.what()));
        d->Visit([](auto &simulation) { simulation.reset(); });
        return false;
    }
    return true;
}

bool DebugSimulator::StepOne()
{
    return StepInto();
}

bool DebugSimulator::StepInto()
{
    return d->Visit([&](auto &simulation) -> bool { return simulation && simulation->StepInto(); });
}

bool DebugSimulator::StepOver()
{
    return d->Visit([&](auto &simulation) -> bool { return simulation && simulation->StepOver(); });
}

bool DebugSimulator::StepOut()
{
    return d->Visit([&](auto &simulation) -> bool {
        if (!simulation || simulation->Finished()) {
            return false;
        }
        do {
            simulation->StepInto();
        } while (!simulation->Finished() && !simulation->DebugControl().new_pass);
        return true;
    });
}

bool DebugSimulator::Continue(std::size_t max_steps)
{
    return d->Visit([&](auto &simulation) -> bool { return simulation && simulation->Continue(max_steps); });
}

bool DebugSimulator::IsPaused() const
{
    return d->Visit([&](auto &simulation) -> bool { return simulation && simulation->DebugControl().paused; });
}

bool DebugSimulator::BudgetExhausted() const
{
    return d->Visit([&](auto &simulation) -> bool { return simulation && simulation->DebugControl().budget_exhausted; });
}

bool DebugSimulator::SetBreakpoint(int source_line, bool enabled)
{
    return d->Visit([&](auto &simulation) -> bool {
        if (!simulation) {
            return false;
        }
        bool found = false;
        for (JX3DPS::runtime::Pc pc = 0; pc < d->macro.source.size(); ++pc) {
            if (std::cmp_equal(d->macro.source[pc].line, source_line)) {
                simulation->SetBreakpoint(pc, enabled);
                found = true;
            }
        }
        return found;
    });
}

DebugSimulator::PlayerState DebugSimulator::GetPlayerState() const
{
    return d->Visit([&](auto &simulation) -> PlayerState {
        PlayerState result{};
        result.currentMacroLine = -1;
        if (!simulation) {
            return result;
        }

        const auto &state = simulation->GetState();
        const auto &debug = simulation->MacroDebugState();
        if (d->lastStep) {
            const auto STEP = DescribeStep(*d->lastStep, d->macro, d->Class());
            result.lastStep =
                QStringLiteral("#%1 | 第 %2 帧 | 第 %3 行\n%4 -> %5\n%6")
                    .arg(STEP.ordinal)
                    .arg(STEP.frame)
                    .arg(STEP.line)
                    .arg(STEP.expression, STEP.outcome, STEP.details);
        }
        result.lifePercent       = state.life;
        result.manaPercent       = state.mana;
        result.qidian            = state.qidian;
        result.rage              = state.rage;
        result.energy            = state.energy;
        result.targetId          = 1;
        result.targetLifePercent = state.target_life;
        result.currentFrame      = state.now;
        result.currentSeconds    = JX3DPS::FramesToSeconds(state.now);
        result.currentMacro      = desktop::ClassName(d->Class());
        const auto CATALOG       = desktop::Describe(d->Class());
        result.details << QStringLiteral("目标气血 %1% · 距离 %2").arg(state.target_life * 100, 0, 'f', 2).arg(state.target_distance, 0, 'f', 2);

        if (state.last_skill != JX3DPS::runtime::INVALID_SLOT && state.last_skill < CATALOG.skills.size()) {
            result.lastSkill = QString::fromUtf8(CATALOG.skills[state.last_skill].name.data(),
                                                 static_cast<int>(CATALOG.skills[state.last_skill].name.size()));
        }
        if (!simulation->Finished() && debug.instruction < d->macro.source.size()) {
            result.currentMacroLine = static_cast<int>(d->macro.source[debug.instruction].line);
        } else {
            result.currentMacroLine = -1;
        }
        using namespace JX3DPS::runtime;
        constexpr std::array PHASES{ "宏行", "条件", "动作", "已完成" };
        result.phase =
            simulation->Finished() ? QStringLiteral("已完成") : QString::fromUtf8(PHASES[static_cast<unsigned>(debug.phase)]);
        if (!simulation->Finished() && debug.phase == DebugPhase::CONDITION &&
            debug.instruction < d->macro.program.lines.size())
        {
            const auto &line = d->macro.program.lines[debug.instruction];
            if (debug.condition < line.condition_count) {
                const auto          &condition = d->macro.program.conditions[line.condition_begin + debug.condition];
                constexpr std::array COMPARISONS{ "<", "<=", "=", "!=", ">", ">=" };
                result.condition =
                    QStringLiteral("下一条件预览（时间单位为帧）：\n%1\n%2 %3 %4 -> %5")
                        .arg(QString::fromStdString(d->macro.source[debug.instruction].conditions[debug.condition]))
                        .arg(ReadValue(state, condition))
                        .arg(QString::fromLatin1(COMPARISONS[static_cast<unsigned>(condition.comparator)]))
                        .arg(condition.value)
                        .arg(EvaluateCondition(state, condition) ? QStringLiteral("成立") : QStringLiteral("不成立"));
            }
        }
        const auto NAME = [](std::string_view text) {
            return desktop::DisplayName(text);
        };
        const auto REMAINING = [&](int deadline) {
            return JX3DPS::FramesToSeconds(std::max(0, deadline - state.now));
        };
        if (state.preparing_skill != INVALID_SLOT) {
            result.details << QStringLiteral("运功：%1（%2 秒）")
                                  .arg(NAME(CATALOG.skills[state.preparing_skill].name))
                                  .arg(REMAINING(state.skill_prepare_at[state.preparing_skill]), 0, 'f', 4);
        }
        if (state.casting_skill != INVALID_SLOT) {
            result.details << QStringLiteral("引导：%1（%2 秒）")
                                  .arg(NAME(CATALOG.skills[state.casting_skill].name))
                                  .arg(REMAINING(state.skill_casting_at[state.casting_skill]), 0, 'f', 4);
        }
        for (Slot buff = 0; buff < CATALOG.buffs.size(); ++buff) {
            for (const bool target : { false, true }) {
                const auto STACKS = (target ? state.target_buff_stacks : state.self_buff_stacks)[buff];
                if (!STACKS) {
                    continue;
                }
                const auto EXPIRES = (target ? state.target_buff_expires_at : state.self_buff_expires_at)[buff];
                result.details << QStringLiteral("%1 %2：%3 层，%4")
                                      .arg(target ? QStringLiteral("目标增益") : QStringLiteral("自身增益"))
                                      .arg(NAME(CATALOG.buffs[buff].name))
                                      .arg(STACKS)
                                      .arg(EXPIRES && EXPIRES != std::numeric_limits<JX3DPS::tick_t>::max()
                                               ? QStringLiteral("剩余 %1 秒").arg(REMAINING(EXPIRES), 0, 'f', 4)
                                               : QStringLiteral("常驻"));
            }
        }
        for (const Slot SKILL : CATALOG.castable) {
            if (state.SkillReadyAt(SKILL) == std::numeric_limits<JX3DPS::tick_t>::max()) {
                result.details << QStringLiteral("%1：当前不可用").arg(NAME(CATALOG.skills[SKILL].name));
                continue;
            }
            result.details << QStringLiteral("%1：冷却 %2 秒，充能 %3")
                                  .arg(NAME(CATALOG.skills[SKILL].name))
                                  .arg(REMAINING(state.SkillReadyAt(SKILL)), 0, 'f', 4)
                                  .arg(state.skill_energy[SKILL]);
        }
        if constexpr (std::is_base_of_v<rt::mo_wen::Rules, std::decay_t<decltype(simulation->GetRules())>>) {
            const auto &rules = simulation->GetRules();
            result.details.prepend(QStringLiteral("曲目：%1 | 影子：%2")
                                       .arg(rules.CurrentStyle() == rt::mo_wen::Style::GAO_SHAN_LIU_SHUI
                                                ? QStringLiteral("高山流水")
                                                : QStringLiteral("阳春白雪"))
                                       .arg(rules.ShadowCount()));
            for (const auto &shadow : rules.Shadows()) {
                if (shadow.expires_at) {
                    result.details << QStringLiteral("影子 #%1：剩余 %2 秒，下次攻击 %3 秒；增益快照 %4，额外会效 %5")
                                          .arg(shadow.order + 1)
                                          .arg(REMAINING(shadow.expires_at), 0, 'f', 4)
                                          .arg(REMAINING(shadow.tick_at), 0, 'f', 4)
                                          .arg(shadow.snapshot.profile)
                                          .arg(shadow.snapshot.extra_power);
                }
            }
        }
        return result;
    });
}

CombatEvents DebugSimulator::TakeEvents()
{
    return d->Visit([&](auto &simulation) -> CombatEvents {
        CombatEvents events;
        if (!simulation) {
            return events;
        }
        const auto &intents = simulation->Log().Intents();
        for (; d->eventCursor < intents.size(); ++d->eventCursor) {
            const auto &hit  = intents[d->eventCursor];
            const auto  NAME = desktop::SkillName(d->Class(), hit.skill);
            events.push_back({ hit.frame, hit.skill, NAME, simulation->GetRules().Reduce(hit), static_cast<int>(hit.outcome), hit.sequence });
        }
        return events;
    });
}

BuffEvents DebugSimulator::TakeBuffEvents()
{
    return d->Visit([&](auto &simulation) -> BuffEvents {
        if (!simulation) {
            return {};
        }
        auto events  = std::exchange(d->initialBuffs, {});
        events      += ReadBuffEvents(simulation->Log().Mutations(), d->mutationCursor, d->Class());
        return events;
    });
}

qint64 DebugSimulator::TotalDamage()
{
    return d->Visit([&](auto &simulation) -> qint64 { return simulation ? simulation->TotalDamage() : 0; });
}

desktop::HistoryChunk DebugSimulator::TakeHistory()
{
    return d->Visit([&](auto &simulation) -> desktop::HistoryChunk {
        if (!simulation) {
            return {};
        }
        desktop::HistoryChunk result;
        result.specialization = d->Class();
        result.initial        = std::exchange(d->initialState, {});
        const auto &mutations = simulation->Log().Mutations();
        result.mutations.assign(mutations.begin() + d->historyCursor, mutations.end());
        d->historyCursor       = mutations.size();
        const auto &attributes = simulation->GetRules().AttributeHistory();
        result.attributes.assign(attributes.begin() + d->attributeCursor, attributes.end());
        d->attributeCursor   = attributes.size();
        result.observedFrame = simulation->GetState().now;
        result.finished      = simulation->Finished();
        return result;
    });
}

MacroSteps DebugSimulator::TakeMacroSteps()
{
    MacroSteps steps;
    steps.reserve(static_cast<qsizetype>(d->pendingSteps.size()));
    for (const auto &step : d->pendingSteps) {
        steps.push_back(DescribeStep(step, d->macro, d->Class()));
    }
    d->pendingSteps.clear();
    return steps;
}

bool DebugSimulator::IsFinished() const
{
    return d->Visit([&](auto &simulation) -> bool { return !simulation || simulation->Finished(); });
}

void DebugSimulator::Reset()
{
    d->Visit([](auto &simulation) { simulation.reset(); });
    d->macro          = {};
    d->eventCursor    = 0;
    d->mutationCursor = 0;
    d->initialBuffs.clear();
    d->initialState.reset();
    d->historyCursor = d->attributeCursor = 0;
    d->pendingSteps.clear();
    d->lastStep.reset();
}
