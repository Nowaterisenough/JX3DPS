#include "batch_worker.h"
#include <QCoreApplication>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSaveFile>
#include <cstdio>
#include <limits>
#include <optional>
#include <stdexcept>

namespace {
desktop::Config Scenario(const QString &name)
{
    namespace tx = JX3DPS::runtime::tai_xu;
    namespace mw = JX3DPS::runtime::mo_wen;
    if (name == "tx") return tx::Config{};
    if (name == "mw") return mw::Config{};
    if (name == "tx_buffs") {
        tx::Config config;
        config.talents = tx::DieRenTalent | tx::WuYi | tx::ShenMai | tx::XuanMen | tx::LieYun | tx::GuChang;
        config.equipment = tx::WeaponCW | tx::SetAttribute;
        config.attributes.overcome_base = 20000;
        config.attributes.overcome_by_class = 5000;
        config.team_buffs = {{tx::team::JiLei, 0, 96, 1}, {tx::team::NongMei, 48, 96, 1},
                             {tx::team::HaoLingSanJun, 16, 0, 48}, {tx::team::PoFeng, 32, 160, 1}};
        return config;
    }
    if (name == "mw_equipped") {
        mw::Config config;
        config.talents = mw::HaoZhong | mw::FeiFan | mw::ShiXiang | mw::ZhengLvHeMing | mw::LiuZhaoTalent | mw::KeMeng;
        config.equipment = mw::EnchantShoes | mw::EnchantWrist | mw::EnchantBelt | mw::SetAttribute |
                           mw::SetSkill | mw::WeaponCW | mw::WeaponWater;
        config.team_effects = mw::YouRen | mw::JingMiao;
        return config;
    }
    throw std::invalid_argument("unknown scenario");
}

QJsonObject Stats(const desktop::rt::BatchStats &stats)
{
    return {{"count", QString::number(stats.count)}, {"checksum", QString::number(stats.checksum)},
            {"minDamage", QString::number(stats.min_damage)}, {"maxDamage", QString::number(stats.max_damage)}};
}
} // namespace

int RunDesktopWorkload(int argc, char *argv[]) try {
    QCoreApplication app(argc, argv);
    const auto args = app.arguments();
    if (args.size() != 8)
        throw std::invalid_argument("expected scene, batch|gains, iterations, workers, seed, result file");
    const bool gains = args[3] == "gains";
    if (!gains && args[3] != "batch") throw std::invalid_argument("unknown workload");
    desktop::rt::BatchOptions options;
    bool countOk, workersOk, seedOk;
    options.iterations = args[4].toULongLong(&countOk);
    options.workers = args[5].toUInt(&workersOk);
    options.seed = args[6].toULongLong(&seedOk);
    options.duration = 300 * 16;
    if (!countOk || !options.iterations || !workersOk || !options.workers || !seedOk)
        throw std::invalid_argument("invalid iterations, workers or seed");

    auto config = Scenario(args[2]);
    const auto spec = desktop::ClassOf(config);
    auto source = desktop::DefaultMacro(spec).toUtf8();
    auto [macro, errors] = desktop::rt::MacroCompiler::Compile(source.toStdString(), desktop::MacroOptions(spec));
    if (!errors.empty()) throw std::runtime_error("macro line " + std::to_string(errors.front().line) + ": " + errors.front().message);
    if (macro.program.lines.empty()) throw std::runtime_error("empty desktop workload macro");
    BatchWorker worker(std::move(macro.program), options, std::move(config), nullptr, false, gains);
    std::optional<SimulationResult> result;
    QString failure;
    QObject::connect(&worker, &BatchWorker::StatisticsReady, &app,
                     [&](const SimulationResult &value) { result = value; }, Qt::DirectConnection);
    QObject::connect(&worker, &BatchWorker::Failed, &app, [&](const QString &error) { failure = error; }, Qt::DirectConnection);
    worker.start();
    worker.wait();
    if (!failure.isEmpty()) throw std::runtime_error(failure.toStdString());
    if (!result || !result->Complete() || !result->skillsCollected || result->gains.size() != (gains ? 8 : 0) ||
        worker.Completed() != worker.Requested()) throw std::runtime_error("incomplete desktop batch");

    quint64 totalDamage = 0;
    for (const auto &skill : result->skills) {
        if (skill.damage < 0 || quint64(skill.damage) > std::numeric_limits<quint64>::max() - totalDamage)
            throw std::overflow_error("total damage overflow");
        totalDamage += quint64(skill.damage);
    }
    QJsonArray gainStats;
    for (const auto &gain : result->gains) {
        if (gain.stats.count != options.iterations) throw std::runtime_error("incomplete attribute gain");
        auto item = Stats(gain.stats);
        item.insert("attribute", gain.attribute);
        item.insert("increment", gain.increment);
        gainStats.append(item);
    }
    QJsonObject output{{"scene", args[2]}, {"mode", args[3]}, {"iterations", args[4]}, {"workers", args[5]},
                       {"seed", args[6]}, {"seconds", result->elapsedSeconds}, {"primary", Stats(result->stats)},
                       {"totalDamage", QString::number(totalDamage)}, {"gains", gainStats}};
    const auto json = QJsonDocument(output).toJson();
    QSaveFile file(args[7]);
    if (!file.open(QIODevice::WriteOnly) || file.write(json) != json.size() || !file.commit())
        throw std::runtime_error(file.errorString().toStdString());
    return 0;
} catch (const std::exception &e) {
    std::fprintf(stderr, "Desktop workload failed: %s\n", e.what());
    return 1;
}
