#include <cstdio>
#include <exception>
#include <limits>
#include <optional>
#include <stdexcept>
#include <string>

#include <QCoreApplication>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSaveFile>

#include "src/engine/scenarios.hpp"

#include "batch_worker.h"

namespace {
QJsonObject Stats(const desktop::rt::BatchStats &stats)
{
    return {
        { "count",     QString::number(stats.count)      },
        { "checksum",  QString::number(stats.checksum)   },
        { "minDamage", QString::number(stats.min_damage) },
        { "maxDamage", QString::number(stats.max_damage) }
    };
}
} // namespace

int RunDesktopWorkload(int argc, char *argv[])
try {
    QCoreApplication app(argc, argv);
    const auto       ARGS = QCoreApplication::arguments();
    if (ARGS.size() == 3 && ARGS[2] == "--list-schools") {
        QJsonObject schools;
        for (const auto &book : desktop::rt::season50::Books())
            schools.insert(QString::fromUtf8(book.code), QString::fromUtf8(book.name));
        std::puts(QJsonDocument(schools).toJson(QJsonDocument::Compact).constData());
        return 0;
    }
    if (ARGS.size() != 8 && ARGS.size() != 9) {
        throw std::invalid_argument(
            "expected scene, batch|gains, iterations, workers, seed, result file and optional macro file");
    }
    const bool GAINS = ARGS[3] == "gains";
    if (!GAINS && ARGS[3] != "batch") {
        throw std::invalid_argument("unknown workload");
    }
    desktop::rt::BatchOptions options;
    bool                      count_ok;
    bool                      workers_ok;
    bool                      seed_ok;
    options.iterations = ARGS[4].toULongLong(&count_ok);
    options.workers    = ARGS[5].toUInt(&workers_ok);
    options.seed       = ARGS[6].toULongLong(&seed_ok);
    options.duration   = 300 * 16;
    if (!count_ok || (options.iterations == 0u) || !workers_ok || (options.workers == 0u) || !seed_ok) {
        throw std::invalid_argument("invalid iterations, workers or seed");
    }

    auto       config = JX3DPS::engine::Scenario(ARGS[2].toStdString());
    const auto SPEC   = desktop::ClassOf(config);
    auto       source = desktop::DefaultMacro(SPEC).toUtf8();
    if (ARGS.size() == 9) {
        QFile file(ARGS[8]);
        if (!file.open(QIODevice::ReadOnly)) {
            throw std::runtime_error(file.errorString().toStdString());
        }
        source = file.readAll();
    }
    auto [macro, errors] = desktop::rt::MacroCompiler::Compile(source.toStdString(), desktop::MacroOptions(SPEC));
    if (!errors.empty()) {
        throw std::runtime_error("macro line " + std::to_string(errors.front().line) + ": " + errors.front().message);
    }
    if (macro.program.lines.empty()) {
        throw std::runtime_error("empty desktop workload macro");
    }
    BatchWorker                     worker(std::move(macro.program), options, std::move(config), nullptr, false, GAINS);
    std::optional<SimulationResult> result;
    QString                         failure;
    QObject::connect(&worker, &BatchWorker::StatisticsReady, &app, [&](const SimulationResult &value) { result = value; }, Qt::DirectConnection);
    QObject::connect(&worker, &BatchWorker::Failed, &app, [&](const QString &error) { failure = error; }, Qt::DirectConnection);
    worker.start();
    worker.wait();
    if (!failure.isEmpty()) {
        throw std::runtime_error(failure.toStdString());
    }
    if (!result || !result->Complete() || !result->skillsCollected || result->gains.size() != (GAINS ? 8 : 0) ||
        worker.Completed() != worker.Requested())
    {
        throw std::runtime_error("incomplete desktop batch");
    }

    quint64 total_damage = 0;
    for (const auto &skill : result->skills) {
        if (skill.damage < 0 || quint64(skill.damage) > std::numeric_limits<quint64>::max() - total_damage) {
            throw std::overflow_error("total damage overflow");
        }
        total_damage += quint64(skill.damage);
    }
    QJsonArray gain_stats;
    for (const auto &gain : result->gains) {
        if (gain.stats.count != options.iterations) {
            throw std::runtime_error("incomplete attribute gain");
        }
        auto item = Stats(gain.stats);
        item.insert("attribute", gain.attribute);
        item.insert("increment", gain.increment);
        gain_stats.append(item);
    }
    const QJsonObject output{
        { "scene",       ARGS[2]                       },
        { "mode",        ARGS[3]                       },
        { "iterations",  ARGS[4]                       },
        { "workers",     ARGS[5]                       },
        { "seed",        ARGS[6]                       },
        { "seconds",     result->elapsedSeconds        },
        { "primary",     Stats(result->stats)          },
        { "totalDamage", QString::number(total_damage) },
        { "gains",       gain_stats                    }
    };
    const auto JSON = QJsonDocument(output).toJson();
    QSaveFile  file(ARGS[7]);
    if (!file.open(QIODevice::WriteOnly) || file.write(JSON) != JSON.size() || !file.commit()) {
        throw std::runtime_error(file.errorString().toStdString());
    }
    return 0;
} catch (const std::exception &e) {
    std::fprintf(stderr, "Desktop workload failed: %s\n", e.what());
    return 1;
}
