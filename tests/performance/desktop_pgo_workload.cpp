#include "batch_worker.h"
#include "src/engine/scenarios.hpp"
#include <QCoreApplication>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSaveFile>
#include <cstdio>
#include <limits>
#include <optional>
#include <stdexcept>

namespace {
QJsonObject Stats(const desktop::rt::BatchStats &stats)
{
    return {{"count", QString::number(stats.count)}, {"checksum", QString::number(stats.checksum)},
            {"minDamage", QString::number(stats.min_damage)}, {"maxDamage", QString::number(stats.max_damage)}};
}
} // namespace

int RunDesktopWorkload(int argc, char *argv[]) try {
    QCoreApplication app(argc, argv);
    const auto args = app.arguments();
    if (args.size() != 8 && args.size() != 9)
        throw std::invalid_argument("expected scene, batch|gains, iterations, workers, seed, result file and optional macro file");
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

    auto config = JX3DPS::engine::Scenario(args[2].toStdString());
    const auto spec = desktop::ClassOf(config);
    auto source = desktop::DefaultMacro(spec).toUtf8();
    if (args.size() == 9) {
        QFile file(args[8]);
        if (!file.open(QIODevice::ReadOnly)) throw std::runtime_error(file.errorString().toStdString());
        source = file.readAll();
    }
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
