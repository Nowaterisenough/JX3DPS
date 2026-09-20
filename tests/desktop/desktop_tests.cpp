#include <QtTest>
#include <QAction>
#include <QCheckBox>
#include <QComboBox>
#include <QCompleter>
#include <QDoubleSpinBox>
#include <QDialog>
#include <QDir>
#include <QFile>
#include <QJsonArray>
#include <QLabel>
#include <QLineEdit>
#include <QJsonDocument>
#include <QListWidget>
#include "src/class/mo_wen/runtime_rules.hpp"
#include <QInputMethodEvent>
#include <QPainter>
#include <QPushButton>
#include <QScrollArea>
#include <QScrollBar>
#include <QSettings>
#include <QSpinBox>
#include <QTableWidget>
#include <QTableView>
#include <QTabWidget>
#include <QTextBrowser>
#include <QTemporaryDir>
#include <QToolButton>
#include "apps/desktop/workbench.h"
#include "apps/desktop/batch_worker.h"
#include "apps/desktop/results_panel.h"
#include "apps/desktop/simulation_options.h"
#include "apps/desktop/buff_panel.h"
#include "apps/desktop/macro_trace_panel.h"
#include "apps/desktop/macro_editor.h"
#include "apps/desktop/macro_search_bar.h"
#include "apps/desktop/debug_simulator.h"
#include "apps/desktop/debug_session.h"
#include "apps/desktop/game_icons.h"
#include "apps/desktop/history_panel.h"
#include "apps/desktop/traced_rules.h"
#include "apps/desktop/controls/code_editor/code_editor.h"
#include "apps/desktop/controls/code_editor/simulation_toolbar.h"
#include "apps/desktop/controls/theme/dark_style.h"
#include "apps/desktop/controls/timeline/timeline.h"
#include "apps/desktop/controls/equipment_panel/equipment_panel.h"
#include "resources.h"
#include "src/class/tai_xu_jian_yi/runtime_rules.hpp"

namespace rt = JX3DPS::runtime;
namespace tx = JX3DPS::runtime::tai_xu;

class DesktopTests : public QObject {
    Q_OBJECT
    QTemporaryDir settingsDirectory;

private slots:
    void initTestCase()
    {
        QVERIFY(settingsDirectory.isValid());
        QApplication::setQuitOnLastWindowClosed(false);
        QCoreApplication::setOrganizationName(QStringLiteral("JX3DPS-tests"));
        QCoreApplication::setApplicationName(QStringLiteral("Workbench"));
        QSettings::setDefaultFormat(QSettings::IniFormat);
        QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, settingsDirectory.path());
        Resources::InitResources();
        QApplication::setFont(Resources::Font());
        QApplication::setStyle(new DarkStyle());
    }

    void init() { QSettings().clear(); }

    void equipmentPreviewPreservesPausedSimulation()
    {
        QTemporaryDir fixture;
        QVERIFY(fixture.isValid());
        struct RestoreDirectory {
            QString previous = QDir::currentPath();
            ~RestoreDirectory() { QDir::setCurrent(previous); }
        } restoreDirectory;
        QVERIFY(QDir().mkpath(fixture.path() + "/data/jx3box"));
        QFile snapshot(fixture.path() + "/data/jx3box/tai_xu_jian_yi_latest.json");
        QVERIFY(snapshot.open(QIODevice::WriteOnly));
        const QJsonObject candidate{{"id", 1}, {"name", QStringLiteral("测试帽子")},
                                    {"type", QStringLiteral("帽子")}, {"icon_id", 0}};
        const auto bytes = QJsonDocument(QJsonObject{{"data_version", "merge-test"},
            {"equipment_catalog", QJsonArray{candidate}}}).toJson();
        QCOMPARE(snapshot.write(bytes), bytes.size());
        snapshot.close();
        QVERIFY(QDir::setCurrent(fixture.path()));

        Workbench window;
        window.show();
        auto *options = window.findChild<SimulationOptions *>();
        auto *editor = window.findChild<MacroEditor *>("macroEditor");
        auto *session = window.findChild<DebugSession *>();
        window.findChild<QAction *>("debugStart")->trigger();
        QCOMPARE(session->GetState(), DebugSession::Paused);
        const auto configuration = options->Snapshot();
        const auto macro = editor->toPlainText();
        const auto line = editor->GetCurrentDebugLine();
        auto *openEquipment = window.findChild<QToolButton *>("openEquipment");
        QVERIFY(openEquipment);
        openEquipment->click();
        auto *dialog = window.findChild<QDialog *>("equipmentDialog");
        QVERIFY(dialog);
        QTRY_VERIFY(dialog->isVisible());
        auto *panel = dialog->findChild<EquipmentPanel *>();
        QVERIFY(panel);
        QListWidget *catalog = nullptr;
        for (auto *list : panel->findChildren<QListWidget *>()) {
            if (list->count() && list->item(0)->data(Qt::UserRole).toJsonObject() == candidate)
                catalog = list;
        }
        QVERIFY(catalog);
        QSignalSpy changed(panel, &EquipmentPanel::LoadoutChanged);
        QVERIFY(changed.isValid());
        QTest::mouseClick(catalog->viewport(), Qt::LeftButton, Qt::NoModifier,
                          catalog->visualItemRect(catalog->item(0)).center());
        QCOMPARE(changed.count(), 1);
        QCOMPARE(panel->Loadout().value("items").toObject().value(QStringLiteral("帽子")).toObject(), candidate);
        QCOMPARE(options->Snapshot(), configuration);
        QCOMPARE(editor->toPlainText(), macro);
        QCOMPARE(editor->GetCurrentDebugLine(), line);
        QCOMPARE(session->GetState(), DebugSession::Paused);
        QVERIFY(editor->isReadOnly());
        QCOMPARE(window.findChild<QTabWidget *>("workspaceTabs")->count(), 3);
        dialog->close();
        openEquipment->click();
        QCOMPARE(dialog->findChild<EquipmentPanel *>(), panel);
        QCOMPARE(panel->Loadout().value("items").toObject().size(), 1);
        if (qEnvironmentVariableIsSet("JX3DPS_TEST_SCREENSHOT"))
            QVERIFY(dialog->grab().save(qEnvironmentVariable("JX3DPS_TEST_SCREENSHOT") + "-equipment.png"));
        session->Stop();
    }

    void moWenDesktopParity()
    {
        namespace mw = rt::mo_wen;
        const auto source = desktop::DefaultMacro(desktop::Specialization::MoWen);
        const auto bytes = source.toUtf8();
        auto compiled = rt::MacroCompiler::Compile(std::string_view(bytes.constData(), bytes.size()), mw::MacroOptions(false));
        QVERIFY(compiled.second.empty());
        mw::Config config;
        config.talents = mw::LiuZhaoTalent | mw::YunHanTalent | mw::KeMeng | mw::MingJinTalent | mw::CanLianTalent;
        config.equipment = mw::WeaponCW | mw::EnchantBelt;
        config.initial_yingzi = 2;
        rt::BatchOptions options{1, 913, 16 * 45, 1, 16 * 45 * 16 + 64, 16 * 45 * 40 + 128};
        BatchWorker worker(compiled.first.program, options, config, nullptr, true);
        QSignalSpy trace(&worker, &BatchWorker::TraceReady), buffs(&worker, &BatchWorker::BuffTraceReady);
        QSignalSpy historySignal(&worker, &BatchWorker::HistoryReady);
        QSignalSpy stats(&worker, &BatchWorker::StatisticsReady), failure(&worker, &BatchWorker::Failed);
        worker.start();
        QVERIFY(worker.wait(10000));
        QTRY_COMPARE(stats.count(), 1);
        QCOMPARE(failure.count(), 0);
        DebugSimulator debugger;
        debugger.SetOptions(config, options.duration, options.seed);
        QString error;
        QVERIFY2(debugger.Initialize(source, error), qPrintable(error));
        QVERIFY(debugger.GetPlayerState().details.join('\n').contains(QStringLiteral("影子：2")));
        QVERIFY(debugger.SetBreakpoint(3));
        debugger.Continue();
        QVERIFY(debugger.IsPaused());
        QCOMPARE(debugger.GetPlayerState().currentMacroLine, 3);
        debugger.SetBreakpoint(3, false);
        debugger.StepInto();
        while (!debugger.IsFinished()) debugger.Continue();
        const auto debugEvents = debugger.TakeEvents();
        const auto traceEvents = qvariant_cast<CombatEvents>(trace[0][0]);
        QTRY_COMPARE(historySignal.count(), 1);
        const auto traceHistory = qvariant_cast<desktop::HistoryChunk>(historySignal[0][0]);
        const auto debugHistory = debugger.TakeHistory();
        QVERIFY(traceHistory.initial.has_value());
        QCOMPARE(traceHistory.attributes, debugHistory.attributes);
        QCOMPARE(traceHistory.mutations, debugHistory.mutations);
        desktop::CombatHistory recorded;
        recorded.Append(traceHistory);
        QVERIFY(recorded.At(options.duration).has_value());
        QCOMPARE(recorded.At(options.duration)->attributes, debugHistory.attributes.back().values);
        QCOMPARE(debugEvents.size(), traceEvents.size());
        for (int index = 0; index < debugEvents.size(); ++index) {
            QCOMPARE(debugEvents[index].frame, traceEvents[index].frame);
            QCOMPARE(debugEvents[index].skill, traceEvents[index].skill);
            QCOMPARE(debugEvents[index].damage, traceEvents[index].damage);
            QCOMPARE(debugEvents[index].outcome, traceEvents[index].outcome);
            QCOMPARE(debugEvents[index].name, traceEvents[index].name);
        }
        QCOMPARE(debugger.TakeBuffEvents(), qvariant_cast<BuffEvents>(buffs[0][0]));
        QCOMPARE(debugger.TotalDamage(), qvariant_cast<SimulationResult>(stats[0][0]).stats.min_damage);
        QVERIFY(debugger.Initialize(QStringLiteral("/cast 变徵\n/fcast [buff:曲风>99] 羽\n/cast 变宫"), error));
        bool interruption = false;
        for (int step = 0; step < 200 && !debugger.IsFinished(); ++step) {
            debugger.StepInto();
            for (const auto &observation : debugger.TakeMacroSteps())
                if (observation.kind == MacroStepKind::Interruption) {
                    interruption = true;
                    QVERIFY(observation.details.contains(QStringLiteral("判定本行条件")));
                }
        }
        QVERIFY(interruption);
    }

    void moWenOptionsAndImports()
    {
        namespace mw = rt::mo_wen;
        SimulationOptions options;
        options.findChild<QComboBox *>("specialization")->setCurrentIndex(1);
        options.findChild<QComboBox *>("mwStyle")->setCurrentIndex(0);
        options.findChild<QSpinBox *>("mwShadows")->setValue(3);
        options.findChild<QCheckBox *>(QStringLiteral("mwTalent%1").arg(mw::LiuZhaoTalent))->setChecked(true);
        auto before = options.Snapshot();
        QString error;
        SimulationOptions restored;
        QVERIFY2(restored.RestoreSnapshot(before, error), qPrintable(error));
        QCOMPARE(restored.Snapshot(), before);
        const auto config = std::get<mw::Config>(restored.ActiveConfig());
        QCOMPARE(config.initial_style, mw::Style::YangChunBaiXue);
        QCOMPARE(config.initial_yingzi, 3);
        QVERIFY(config.talents & mw::LiuZhaoTalent);
        auto invalid = before;
        invalid.insert("specialization", 1.5);
        QVERIFY(!options.RestoreSnapshot(invalid, error));
        QCOMPARE(options.Snapshot(), before);
        auto values = before.value("values").toObject();
        for (auto flag : {mw::GongCrit2, mw::GongCrit3, mw::GongCrit4, mw::GongDamage3, mw::GongDamage4})
            values.insert(QStringLiteral("mwGong%1").arg(flag), true);
        invalid = before;
        invalid.insert("values", values);
        QVERIFY(!options.RestoreSnapshot(invalid, error));
        QVERIFY(error.contains(QStringLiteral("最多四本")));
        QCOMPARE(options.Snapshot(), before);
        QJsonObject imported{{"LunarAttackPowerBase", 26522}, {"LunarCriticalStrike", 24177},
            {"LunarCriticalDamagePower", 1170}, {"LunarOvercome", 27071}, {"SurplusValue", 26098},
            {"Strain", 12584}, {"Haste", 7029}, {"Spirit", 8328}, {"Strength", 41},
            {"MeleeWeaponDamage", 1609}, {"MeleeWeaponDamageRand", 965}};
        QVERIFY2(options.ImportAttributes(imported, error), qPrintable(error));
        const auto importedConfig = std::get<mw::Config>(options.ActiveConfig());
        QCOMPARE(importedConfig.attributes.attack_base, JX3DPS::value_t(26522));
        QCOMPARE(importedConfig.attributes.attack_by_class, JX3DPS::value_t(8328 * 1895 / 1024));
        QCOMPARE(importedConfig.attributes.weapon, JX3DPS::value_t(2091));
        before = options.Snapshot();
        imported.insert("Haste", -1);
        QVERIFY(!options.ImportAttributes(imported, error));
        QCOMPARE(options.Snapshot(), before);
        options.findChild<QComboBox *>("permanent1FoodEnhance")->setCurrentIndex(1);
        QCOMPARE(std::get<mw::Config>(options.ActiveConfig()).attributes.attack_base, importedConfig.attributes.attack_base + 831);
        options.findChild<QComboBox *>("permanent1FoodSupport")->setCurrentIndex(1);
        const auto enhanced = std::get<mw::Config>(options.ActiveConfig());
        QCOMPARE(enhanced.attributes.attack_by_class, JX3DPS::value_t((8328 + 347) * 1895 / 1024));
        QCOMPARE(enhanced.attributes.crit, JX3DPS::value_t(24177 + (8328 + 347) * 389 / 1024 - 8328 * 389 / 1024
            + (8328 + 347) * 655 / 1024 - 8328 * 655 / 1024));
        auto legacyValues = before.value("values").toObject();
        for (const auto &key : legacyValues.keys())
            if (key.startsWith("mw") || key.startsWith("permanent") || key.startsWith("imported")) legacyValues.remove(key);
        const QJsonObject legacy{{"format", "jx3dps.tai-xu.config"}, {"version", 1}, {"values", legacyValues}};
        QVERIFY2(restored.RestoreSnapshot(legacy, error), qPrintable(error));
        QCOMPARE(restored.Specialization(), desktop::Specialization::TaiXu);
        QCOMPARE(restored.Config().attributes.attack_base, JX3DPS::value_t(26522));
    }

    void moWenWorkbenchFlow()
    {
        Workbench window;
        window.show();
        auto *editor = window.findChild<MacroEditor *>("macroEditor");
        auto *selection = window.findChild<QComboBox *>("specialization");
        editor->setPlainText(QStringLiteral("# 保留太虚草稿\n/cast 三环套月"));
        editor->SetBreakpoint(2, true);
        selection->setCurrentIndex(1);
        QVERIFY(editor->toPlainText().contains(QStringLiteral("变徵")));
        window.findChild<QAction *>("checkMacro")->trigger();
        QCOMPARE(window.findChild<QTableWidget *>("macroProblems")->rowCount(), 0);
        editor->SetBreakpoint(3, true);
        window.findChild<QDoubleSpinBox *>("seconds")->setValue(15);
        window.findChild<QSpinBox *>("mwShadows")->setValue(2);
        if (qEnvironmentVariableIsSet("JX3DPS_TEST_SCREENSHOT")) {
            QTest::qWait(30);
            QVERIFY(window.grab().save(qEnvironmentVariable("JX3DPS_TEST_SCREENSHOT") + "-mowen-configuration.png"));
        }
        window.findChild<QToolButton *>("debugButton")->click();
        window.findChild<QToolButton *>("startButton")->click();
        auto *session = window.findChild<DebugSession *>();
        QCOMPARE(session->GetState(), DebugSession::Paused);
        QCOMPARE(session->GetDebugInfo().currentMacro, QStringLiteral("莫问"));
        QVERIFY(!selection->isEnabled());
        window.findChild<QAction *>("debugContinue")->trigger();
        QTRY_COMPARE(session->GetState(), DebugSession::Paused);
        QCOMPARE(session->GetDebugInfo().lineNumber, 3);
        QCOMPARE(window.findChild<QListWidget *>("breakpointList")->count(), 1);
        QVERIFY(window.findChild<QTextBrowser *>("debugInspector")->toPlainText().contains(QStringLiteral("影子")));
        QVERIFY(window.findChild<QTextBrowser *>("debugInspector")->toPlainText().contains(QStringLiteral("常驻")));
        if (qEnvironmentVariableIsSet("JX3DPS_TEST_SCREENSHOT")) {
            QTest::qWait(30);
            QVERIFY(window.grab().save(qEnvironmentVariable("JX3DPS_TEST_SCREENSHOT") + "-mowen-debug.png"));
        }
        window.findChild<QAction *>("debugStepOver")->trigger();
        window.findChild<QAction *>("debugStop")->trigger();
        selection->setCurrentIndex(0);
        QCOMPARE(editor->toPlainText(), QStringLiteral("# 保留太虚草稿\n/cast 三环套月"));
        QVERIFY(editor->GetBreakpoints().contains(2));
        selection->setCurrentIndex(1);
        QVERIFY(editor->GetBreakpoints().contains(3));
        window.findChild<QToolButton *>("debugButton")->click();
        window.findChild<QAction *>("singleTrace")->trigger();
        QTRY_COMPARE(window.findChild<QTableWidget *>("resultHistory")->rowCount(), 1);
        QTRY_VERIFY(selection->isEnabled());
        QVERIFY(window.findChild<QTableWidget *>("damageEvents")->rowCount() > 0);
        window.close();
        Workbench reopened;
        QCOMPARE(reopened.findChild<QComboBox *>("specialization")->currentIndex(), 1);
        reopened.findChild<QComboBox *>("specialization")->setCurrentIndex(0);
        QCOMPARE(reopened.findChild<MacroEditor *>("macroEditor")->toPlainText(), QStringLiteral("# 保留太虚草稿\n/cast 三环套月"));
    }

    void workbenchWorkspaceNavigation()
    {
        Workbench window;
        window.resize(1366, 900);
        window.show();
        auto *workspace = window.findChild<QTabWidget *>("workspaceTabs");
        auto *home = window.findChild<QWidget *>("simulationPage");
        auto *debug = window.findChild<QWidget *>("debugPage");
        auto *editor = window.findChild<MacroEditor *>("macroEditor");
        auto *document = editor->document();
        auto *options = window.findChild<SimulationOptions *>();
        auto *session = window.findChild<DebugSession *>();
        auto *start = window.findChild<QToolButton *>("startButton");
        auto *gains = window.findChild<QAction *>("attributeGains");
        editor->setPlainText(desktop::DefaultMacro(desktop::Specialization::TaiXu));
        editor->appendPlainText(QStringLiteral("# 页面切换保留撤销与断点"));
        editor->SetBreakpoint(3, true);
        window.findChild<QDoubleSpinBox *>("seconds")->setValue(20);
        window.findChild<QSpinBox *>("iterations")->setValue(20);
        const auto snapshot = options->Snapshot();
        const auto source = editor->toPlainText();
        workspace->setCurrentWidget(debug);
        QCOMPARE(editor->document(), document);
        QVERIFY(editor->isVisible());
        window.findChild<QAction *>("debugStart")->trigger();
        QCOMPARE(session->GetState(), DebugSession::Paused);
        window.findChild<QAction *>("debugStepOver")->trigger();
        const auto phase = session->GetDebugInfo().phase;
        const auto line = session->GetDebugInfo().lineNumber;
        workspace->setCurrentWidget(home);
        QCOMPARE(session->GetState(), DebugSession::Paused);
        QVERIFY(editor->isVisible());
        QVERIFY(editor->isReadOnly());
        QVERIFY(!start->isEnabled());
        QVERIFY(!window.findChild<QComboBox *>("specialization")->isEnabled());
        QVERIFY(!window.findChild<QSpinBox *>("attackBase")->isEnabled());
        window.findChild<QPushButton *>("showStatistics")->click();
        QCOMPARE(workspace->currentWidget(), window.findChild<QWidget *>("analysisPage"));
        window.findChild<QAction *>("findMacro")->trigger();
        QVERIFY(editor->isVisible());
        QVERIFY(window.findChild<MacroSearchBar *>()->isVisible());
        window.findChild<MacroSearchBar *>()->Close();
        workspace->setCurrentWidget(debug);
        QCOMPARE(session->GetDebugInfo().phase, phase);
        QCOMPARE(session->GetDebugInfo().lineNumber, line);
        QCOMPARE(editor->document(), document);
        QCOMPARE(editor->toPlainText(), source);
        QCOMPARE(options->Snapshot(), snapshot);
        QVERIFY(editor->HasBreakpoint(3));
        window.findChild<QAction *>("debugStop")->trigger();
        workspace->setCurrentWidget(home);
        editor->undo();
        QVERIFY(!editor->toPlainText().contains(QStringLiteral("页面切换")));
        QVERIFY(editor->HasBreakpoint(3));
        start->click();
        QTRY_VERIFY_WITH_TIMEOUT(start->isEnabled(), 10000);
        QCOMPARE(workspace->currentWidget(), home);
        QVERIFY(window.findChild<QLabel *>("meanDps")->text().toDouble() > 0);
        gains->trigger();
        QTRY_VERIFY_WITH_TIMEOUT(gains->isEnabled(), 10000);
        QCOMPARE(workspace->currentWidget(), home);
        QCOMPARE(window.findChild<QTableWidget *>("gainOverview")->rowCount(), 8);
        auto *detailGains = window.findChild<QTableWidget *>("attributeGainsTable");
        QCOMPARE(window.findChild<QTableWidget *>("gainOverview")->item(0, 1)->text().toDouble(),
            QString::number(detailGains->item(0, 5)->text().toDouble(), 'f', 2).toDouble());
        if (qEnvironmentVariableIsSet("JX3DPS_TEST_SCREENSHOT")) {
            QTest::qWait(30);
            QVERIFY(options->height() <= 800);
            QVERIFY(window.grab().save(qEnvironmentVariable("JX3DPS_TEST_SCREENSHOT") + "-overview.png"));
            window.resize(1280, 800);
            QTest::qWait(30);
            QCOMPARE(window.size(), QSize(1280, 800));
            QCOMPARE(window.findChild<QScrollArea *>("configurationScroll")->verticalScrollBar()->maximum(), 0);
            auto *talent = window.findChild<QCheckBox *>(QStringLiteral("talent%1").arg(tx::JingHuaTalent));
            QVERIFY(talent->visibleRegion().boundingRect().contains(talent->rect().center()));
            QVERIFY(window.grab().save(qEnvironmentVariable("JX3DPS_TEST_SCREENSHOT") + "-compact.png"));
            window.findChild<QComboBox *>("specialization")->setCurrentIndex(1);
            QTest::qWait(30);
            QVERIFY(window.grab().save(qEnvironmentVariable("JX3DPS_TEST_SCREENSHOT") + "-mowen-compact.png"));
        }
    }

    void attributeGainBatch()
    {
        auto compiled = rt::MacroCompiler::Compile("/cast biangong", rt::mo_wen::MacroOptions(false));
        rt::BatchOptions options{12, 9, 16 * 5, 2, 2000, 4000};
        rt::mo_wen::Config config;
        BatchWorker worker(compiled.first.program, options, config, nullptr, false, true);
        QSignalSpy stats(&worker, &BatchWorker::StatisticsReady), failure(&worker, &BatchWorker::Failed);
        worker.start();
        QVERIFY(worker.wait(10000));
        QTRY_COMPARE(stats.count(), 1);
        QCOMPARE(failure.count(), 0);
        const auto result = qvariant_cast<SimulationResult>(stats[0][0]);
        QCOMPARE(result.gains.size(), 8);
        QCOMPARE(worker.Completed(), worker.Requested());
        config.attributes.attack_base += 100;
        const auto direct = rt::RunBatch(compiled.first.program, options, rt::mo_wen::Rules(rt::mo_wen::Prepare(config)));
        QCOMPARE(result.gains[0].stats.checksum, direct.checksum);
        ResultsPanel panel;
        panel.AddResult(result, QStringLiteral("/cast biangong"), {});
        QCOMPARE(panel.findChild<QTableWidget *>("attributeGainsTable")->rowCount(), 8);
    }

    void chineseWorkbenchAndMoWenCompletion()
    {
        Workbench window;
        QCOMPARE(window.findChild<QAction *>("newMacro")->text(), QStringLiteral("新建"));
        QCOMPARE(window.findChild<QAction *>("attributeGains")->text(), QStringLiteral("计算收益"));
        auto *selection = window.findChild<QComboBox *>("specialization");
        auto *editor = window.findChild<MacroEditor *>("macroEditor");
        selection->setCurrentIndex(1);
        editor->setPlainText(QStringLiteral("/cast 无我无剑"));
        window.findChild<QAction *>("checkMacro")->trigger();
        auto *problems = window.findChild<QTableWidget *>("macroProblems");
        QCOMPARE(problems->rowCount(), 1);
        QVERIFY(problems->item(0, 1)->text().contains(QStringLiteral("当前心法不支持")));
        window.show();
        editor->setPlainText(QStringLiteral("/cast 变"));
        auto cursor = editor->textCursor();
        cursor.movePosition(QTextCursor::End);
        editor->setTextCursor(cursor);
        editor->Complete();
        auto *completer = editor->findChild<QCompleter *>("macroCompleter");
        QStringList candidates;
        for (int row = 0; row < completer->completionModel()->rowCount(); ++row)
            candidates << completer->completionModel()->index(row, 0).data(Qt::UserRole).toString();
        QVERIFY(candidates.contains(QStringLiteral("变宫")));
        QVERIFY(candidates.contains(QStringLiteral("变徵")));
        QVERIFY(!candidates.contains(QStringLiteral("无我无剑")));
        QTest::keyClick(editor, Qt::Key_Escape);
        selection->setCurrentIndex(0);
        editor->setPlainText(QStringLiteral("/cast [buff:玄门] 无我无剑\n/cast [buff:玄门>0] 无我无剑"));
        window.findChild<QAction *>("checkMacro")->trigger();
        QCOMPARE(problems->rowCount(), 0);
    }

    void debuggerMatchesTrace()
    {
        const QString text = QStringLiteral("/scast ziqi\n/cast [buff:ziqi>0&qidian>5] wuwu\n/cast sanhuan");
        DebugSimulator debugger;
        tx::Config config;
        config.team_effects = tx::YouRen;
        config.equipment = tx::SetAttribute;
        config.talents = tx::DieRenTalent | tx::XuanMen | tx::WuYu;
        config.attributes.overcome_base = 25000;
        debugger.SetOptions(config, 4800, 77);
        QString error;
        QVERIFY2(debugger.Initialize(text, error), qPrintable(error));
        QVERIFY(debugger.SetBreakpoint(2));
        debugger.Continue();
        QVERIFY(debugger.IsPaused());
        QCOMPARE(debugger.GetPlayerState().currentMacroLine, 2);
        debugger.StepInto();
        QVERIFY(debugger.GetPlayerState().condition.contains(QStringLiteral("成立")));
        QVERIFY(debugger.GetPlayerState().details.join('\n').contains(QStringLiteral("自身增益")));
        debugger.SetBreakpoint(2, false);
        debugger.StepOut();
        QVERIFY(!debugger.IsFinished());
        CombatEvents events = debugger.TakeEvents();
        BuffEvents buffEvents = debugger.TakeBuffEvents();
        BuffTrace debugBuffs;
        debugBuffs.Reset(4800);
        debugBuffs.Append(buffEvents);
        debugBuffs.Observe(debugger.GetPlayerState().currentFrame);
        for (int i = 0; i < 1000 && !debugger.IsFinished(); ++i) {
            debugger.Continue(2000);
            events += debugger.TakeEvents();
            const auto changes = debugger.TakeBuffEvents();
            buffEvents += changes;
            debugBuffs.Append(changes);
            debugBuffs.Observe(debugger.GetPlayerState().currentFrame);
        }
        QVERIFY(debugger.IsFinished());
        QCOMPARE(debugger.GetPlayerState().currentMacroLine, -1);
        QVERIFY(debugger.TakeEvents().isEmpty());
        QVERIFY(debugger.TakeBuffEvents().isEmpty());
        const auto bytes = text.toUtf8();
        auto compiled = rt::MacroCompiler::Compile(std::string_view(bytes.constData(), bytes.size()), tx::MacroOptions(false));
        QVERIFY(compiled.second.empty());
        rt::Simulation<tx::Rules> reference(compiled.first.program, tx::Rules(tx::Prepare(config)), 4800*8+64, 4800*24+128);
        reference.Start(4800, rt::IterationSeed(77, 0));
        reference.Run();
        QCOMPARE(debugger.TotalDamage(), reference.TotalDamage());
        QCOMPARE(events.size(), static_cast<qsizetype>(reference.Log().Size()));
        for (int i = 0; i < events.size(); ++i) {
            const auto &hit = reference.Log().Intents()[i];
            QCOMPARE(events[i].frame, hit.frame);
            QCOMPARE(events[i].outcome, static_cast<int>(hit.outcome));
            QCOMPARE(events[i].damage, reference.GetRules().Reduce(hit));
            QCOMPARE(events[i].sequence, hit.sequence);
        }
        std::size_t mutationCursor = 0;
        const auto referenceBuffs = ReadBuffEvents(reference.Log().Mutations(), mutationCursor);
        QCOMPARE(buffEvents, referenceBuffs);
        QCOMPARE(mutationCursor, reference.Log().Mutations().size());
        QVERIFY(ReadBuffEvents(reference.Log().Mutations(), mutationCursor).isEmpty());
        QVERIFY(std::any_of(buffEvents.begin(), buffEvents.end(), [](const BuffEvent &buff) { return buff.target; }));
        BuffTrace fullBuffs;
        fullBuffs.Reset(4800);
        fullBuffs.Append(referenceBuffs);
        fullBuffs.Observe(4800);
        const auto fullCoverage = fullBuffs.Coverage();
        const auto debugCoverage = debugBuffs.Coverage();
        QCOMPARE(debugCoverage.size(), fullCoverage.size());
        for (int i = 0; i < fullCoverage.size(); ++i) {
            QCOMPARE(debugCoverage[i].activeFrames, fullCoverage[i].activeFrames);
            QCOMPARE(debugCoverage[i].stackFrames, fullCoverage[i].stackFrames);
            QCOMPARE(debugCoverage[i].intervals, fullCoverage[i].intervals);
        }
        rt::BatchOptions traceOptions;
        traceOptions.duration = 4800;
        traceOptions.seed = 77;
        traceOptions.damage_capacity = 4800 * 8 + 64;
        traceOptions.mutation_capacity = 4800 * 24 + 128;
        BatchWorker traceWorker(compiled.first.program, traceOptions, config, nullptr, true);
        QSignalSpy traceBuffs(&traceWorker, &BatchWorker::BuffTraceReady);
        QSignalSpy traceFailed(&traceWorker, &BatchWorker::Failed);
        traceWorker.start();
        QVERIFY(traceWorker.wait(10000));
        QTRY_COMPARE(traceBuffs.count(), 1);
        QCOMPARE(traceFailed.count(), 0);
        QCOMPARE(qvariant_cast<BuffEvents>(traceBuffs.at(0).at(0)), buffEvents);
        debugger.Reset();
        QVERIFY(debugger.TakeBuffEvents().isEmpty());
    }

    void macroCompletion()
    {
        MacroEditor editor;
        editor.resize(900, 300);
        editor.show();
        editor.setFocus();
        auto *completer = editor.findChild<QCompleter *>("macroCompleter");
        QVERIFY(completer);
        auto input = [&](const QString &text, int position = -1) {
            editor.setPlainText(text);
            auto cursor = editor.textCursor();
            cursor.setPosition(position < 0 ? text.size() : position);
            editor.setTextCursor(cursor);
            editor.Complete();
        };
        auto candidates = [&] {
            QStringList values;
            for (int row = 0; row < completer->completionModel()->rowCount(); ++row)
                values << completer->completionModel()->index(row, 0).data(Qt::UserRole).toString();
            return values;
        };
        auto accept = [&](const QString &value) {
            const int row = candidates().indexOf(value);
            if (row < 0) return false;
            const auto index = completer->completionModel()->index(row, 0);
            return QMetaObject::invokeMethod(completer, "activated", Qt::DirectConnection, Q_ARG(QModelIndex, index));
        };
        input("/s");
        QCOMPARE(candidates(), (QStringList{"/scast", "/sfcast"}));
        QVERIFY(accept("/scast"));
        QCOMPARE(editor.toPlainText(), QStringLiteral("/scast"));
        input("/cast ");
        QCOMPARE(candidates().size(), static_cast<qsizetype>(tx::CastableSkills.size() * 2));
        QVERIFY(!candidates().contains(QStringLiteral("pozhao")));
        for (const auto &candidate : candidates()) {
            const auto bytes = (QStringLiteral("/cast ") + candidate).toUtf8();
            QVERIFY(rt::MacroCompiler::Compile(std::string_view(bytes.data(), bytes.size()), tx::MacroOptions()).second.empty());
        }
        input("/cast [buff:");
        QCOMPARE(candidates().size(), static_cast<qsizetype>(tx::BuffCount * 2));
        QVERIFY(candidates().contains(QStringLiteral("xuanmen")));
        QVERIFY(!candidates().contains(QStringLiteral("wuwu")));
        input("/cast [qidian>=6&tbuff:die>=2] wuwu", QStringLiteral("/cast [qidian>=6&tbuff:die").size());
        QCOMPARE(candidates(), (QStringList{"dieren"}));
        QVERIFY(accept("dieren"));
        QCOMPARE(editor.toPlainText(), QStringLiteral("/cast [qidian>=6&tbuff:dieren>=2] wuwu"));
        input("/cast [buff:xuanmen] wuwu", QStringLiteral("/cast [buff:xua").size());
        QVERIFY(accept("xuanmen"));
        QCOMPARE(editor.toPlainText(), QStringLiteral("/cast [buff:xuanmen] wuwu"));
        input("/cast [buf:xuanmen] wuwu", QStringLiteral("/cast [buf").size());
        QVERIFY(accept("buff:"));
        QCOMPARE(editor.toPlainText(), QStringLiteral("/cast [buff:xuanmen] wuwu"));
        input("/cast [last_skill:wu] sanhuan", QStringLiteral("/cast [last_skill:wu").size());
        QVERIFY(accept("wuwu"));
        QCOMPARE(editor.toPlainText(), QStringLiteral("/cast [last_skill:wuwu] sanhuan"));
        input("/cast [qidian>=6] wuwu", QStringLiteral("/cast [qidian>=6] wu").size());
        QVERIFY(accept("wuwu"));
        QCOMPARE(editor.toPlainText(), QStringLiteral("/cast [qidian>=6] wuwu"));

        editor.setPlainText(QStringLiteral("/cast [buff:"));
        editor.moveCursor(QTextCursor::End);
        QInputMethodEvent ime;
        ime.setCommitString(QStringLiteral("\u7384"));
        QCoreApplication::sendEvent(&editor, &ime);
        QVERIFY(completer->popup()->isVisible());
        QCOMPARE(candidates(), (QStringList{QStringLiteral("\u7384\u95e8")}));
        QVERIFY(accept(QStringLiteral("\u7384\u95e8")));
        QCOMPARE(editor.toPlainText(), QStringLiteral("/cast [buff:\u7384\u95e8"));
        editor.undo();
        QCOMPARE(editor.toPlainText(), QStringLiteral("/cast [buff:\u7384"));
        for (const auto &text : {"# /ca", "// /ca", "/switch ", "/cast [qidian>=3", "/cast [buff:xuanmen>2"}) {
            input(QString::fromLatin1(text));
            QVERIFY(!completer->popup()->isVisible());
        }
        input("/cast wu");
        QTest::keyClick(&editor, Qt::Key_Tab);
        QCOMPARE(editor.toPlainText(), QStringLiteral("/cast wuwu"));
        editor.undo();
        QCOMPARE(editor.toPlainText(), QStringLiteral("/cast wu"));
        editor.setPlainText(QStringLiteral("/cast "));
        editor.moveCursor(QTextCursor::End);
        QTest::keyClicks(&editor, "san");
        QVERIFY(completer->popup()->isVisible());
        QCOMPARE(candidates(), (QStringList{"sanhuan", "sanchai"}));
        QTest::keyClicks(&editor, "huan");
        QVERIFY(!completer->popup()->isVisible());
        QTest::keyClick(&editor, Qt::Key_Return);
        QCOMPARE(editor.toPlainText(), QStringLiteral("/cast sanhuan\n"));
        input("/cast wu");
        QTest::keyClick(&editor, Qt::Key_Escape);
        QVERIFY(!completer->popup()->isVisible());
        QCOMPARE(editor.toPlainText(), QStringLiteral("/cast wu"));
        input("/cast wu");
        const auto stale = completer->completionModel()->index(0, 0);
        editor.moveCursor(QTextCursor::Start);
        QVERIFY(!completer->popup()->isVisible());
        QVERIFY(QMetaObject::invokeMethod(completer, "activated", Qt::DirectConnection, Q_ARG(QModelIndex, stale)));
        QCOMPARE(editor.toPlainText(), QStringLiteral("/cast wu"));
        input("/cast wu");
        const auto locked = completer->completionModel()->index(0, 0);
        editor.setReadOnly(true);
        QVERIFY(!completer->popup()->isVisible());
        QVERIFY(QMetaObject::invokeMethod(completer, "activated", Qt::DirectConnection, Q_ARG(QModelIndex, locked)));
        editor.Complete();
        QCOMPARE(editor.toPlainText(), QStringLiteral("/cast wu"));
        QVERIFY(!completer->popup()->isVisible());
    }

    void macroSearchAndReplace()
    {
        QWidget host;
        CodeEditor editor(&host);
        MacroSearchBar search(&editor, &host);
        host.show();
        auto *query = search.findChild<QLineEdit *>("macroSearchQuery");
        auto *replacement = search.findChild<QLineEdit *>("macroReplacement");
        auto *words = search.findChild<QCheckBox *>("macroSearchWords");
        auto *matchCase = search.findChild<QCheckBox *>("macroSearchCase");
        auto *all = search.findChild<QPushButton *>("macroReplaceAll");
        auto *one = search.findChild<QPushButton *>("macroReplaceOne");
        auto *status = search.findChild<QLabel *>("macroSearchStatus");
        const QString source = QStringLiteral("/cast wuwu\n/cast WUWU\n/cast wuwu_extra\n# buff:\u7384\u95e8\n/cast wuwu");
        editor.setPlainText(source);
        editor.SetBreakpoint(2, true);
        editor.SetBreakpoint(5, true);
        search.Open(true);
        query->setText(QStringLiteral("wuwu"));
        QVERIFY(status->text().contains(QStringLiteral("4 处匹配")));
        words->setChecked(true);
        QVERIFY(status->text().contains(QStringLiteral("3 处匹配")));
        matchCase->setChecked(true);
        QVERIFY(status->text().contains(QStringLiteral("2 处匹配")));
        search.Find();
        QCOMPARE(editor.textCursor().blockNumber(), 0);
        QCOMPARE(editor.textCursor().selectedText(), QStringLiteral("wuwu"));
        search.Find();
        QCOMPARE(editor.textCursor().blockNumber(), 4);
        search.Find();
        QCOMPARE(editor.textCursor().blockNumber(), 0);
        QVERIFY(status->text().contains(QStringLiteral("已循环查找")));
        search.Find(true);
        QCOMPARE(editor.textCursor().blockNumber(), 4);
        replacement->setText(QStringLiteral("sanhuan"));
        all->click();
        QCOMPARE(editor.toPlainText(), QStringLiteral("/cast sanhuan\n/cast WUWU\n/cast wuwu_extra\n# buff:\u7384\u95e8\n/cast sanhuan"));
        QVERIFY(editor.HasBreakpoint(2) && editor.HasBreakpoint(5));
        QVERIFY(status->text().contains(QStringLiteral("已替换 2")));
        editor.undo();
        QCOMPARE(editor.toPlainText(), source);
        QVERIFY(editor.HasBreakpoint(2) && editor.HasBreakpoint(5));
        editor.redo();
        QVERIFY(editor.toPlainText().startsWith(QStringLiteral("/cast sanhuan")));
        editor.undo();
        query->setText(QStringLiteral("\u7384\u95e8"));
        replacement->clear();
        search.Find();
        one->click();
        QVERIFY(!editor.toPlainText().contains(QStringLiteral("\u7384\u95e8")));
        editor.undo();
        QCOMPARE(editor.toPlainText(), source);
        query->setText(QStringLiteral("wuwu"));
        replacement->setText(QStringLiteral("wuwuwuwu"));
        all->click();
        QVERIFY(status->text().contains(QStringLiteral("已替换 2")));
        editor.undo();
        QCOMPARE(editor.toPlainText(), source);
        auto cursor = editor.textCursor();
        cursor.clearSelection();
        cursor.movePosition(QTextCursor::Start);
        editor.setTextCursor(cursor);
        one->click(); // A non-matching selection first locates, rather than replaces.
        QCOMPARE(editor.toPlainText(), source);
        QCOMPARE(editor.textCursor().selectedText(), QStringLiteral("wuwu"));
        editor.setReadOnly(true);
        QVERIFY(!one->isEnabled() && !all->isEnabled() && !replacement->isEnabled());
        QVERIFY(status->text().contains(QStringLiteral("锁定")));
        search.Find();
        QCOMPARE(editor.textCursor().blockNumber(), 4);
        all->click();
        QTest::keyClick(replacement, Qt::Key_Return);
        QCOMPARE(editor.toPlainText(), source);
        editor.setReadOnly(false);
        QVERIFY(all->isEnabled());
        query->clear();
        QVERIFY(!all->isEnabled());
        query->setText(QStringLiteral("[buff:"));
        QVERIFY(status->text().contains(QStringLiteral("0 处匹配")));
        query->setText(QStringLiteral("wuwu"));
        editor.SetCurrentDebugLine(2);
        QVERIFY(editor.extraSelections().size() >= 3);
        search.Close();
        QCOMPARE(editor.GetCurrentDebugLine(), 2);
        QVERIFY(editor.extraSelections().size() < 3);
        editor.setPlainText(QStringLiteral("renjian renjian_suixing \u4eba\u5251\u5408\u4e00\u00b7DOT \u4eba\u5251\u5408\u4e00"));
        search.Open();
        query->setText(QStringLiteral("renjian"));
        QVERIFY(status->text().contains(QStringLiteral("1 处匹配")));
        query->setText(QStringLiteral("\u4eba\u5251\u5408\u4e00"));
        QVERIFY(status->text().contains(QStringLiteral("1 处匹配")));
        editor.setPlainText(QStringLiteral("wuwu ").repeated(1100));
        query->setText(QStringLiteral("wuwu"));
        QVERIFY(status->text().contains(QStringLiteral("1100 处匹配")));
        QVERIFY(status->text().contains(QStringLiteral("1000 处")));
        QVERIFY(editor.extraSelections().size() <= 1002);
    }

    void workbenchEditorTools()
    {
        Workbench window;
        window.show();
        auto *editor = window.findChild<MacroEditor *>("macroEditor");
        auto *search = window.findChild<MacroSearchBar *>();
        auto *session = window.findChild<DebugSession *>();
        auto *query = search->findChild<QLineEdit *>("macroSearchQuery");
        auto *replacement = search->findChild<QLineEdit *>("macroReplacement");
        auto *all = search->findChild<QPushButton *>("macroReplaceAll");
        const QString source = QStringLiteral("# Tai Xu macro\n/cast [buff:xuanmen&qidian>=6] wuwu\n/cast [qidian>=8] wuwu\n/cast bahuang\n/cast sanhuan");
        editor->setPlainText(source);
        editor->SetBreakpoint(2, true);
        window.findChild<QAction *>("replaceMacro")->trigger();
        QVERIFY(search->isVisible());
        query->setText(QStringLiteral("wuwu"));
        replacement->setText(QStringLiteral("\u65e0\u6211\u65e0\u5251"));
        search->Find();
        QCoreApplication::processEvents();
        if (qEnvironmentVariableIsSet("JX3DPS_TEST_SCREENSHOT"))
            QVERIFY(window.grab().save(qEnvironmentVariable("JX3DPS_TEST_SCREENSHOT") + QStringLiteral("-macro-search.png")));
        window.findChild<QToolButton *>("debugButton")->setChecked(true);
        emit window.findChild<SimulationToolbar *>()->StartSimulationClicked();
        QCOMPARE(session->GetState(), DebugSession::Paused);
        QVERIFY(!all->isEnabled());
        QVERIFY(!window.findChild<QAction *>("completeMacro")->isEnabled());
        search->Find();
        QVERIFY(editor->textCursor().hasSelection());
        QCOMPARE(editor->GetCurrentDebugLine(), 2);
        session->Stop();
        QVERIFY(all->isEnabled());
        all->click();
        QVERIFY(editor->toPlainText().contains(QStringLiteral("\u65e0\u6211\u65e0\u5251")));
        editor->undo();
        QCOMPARE(editor->toPlainText(), source);
        QVERIFY(editor->HasBreakpoint(2));
        search->Close();
        editor->setPlainText(QStringLiteral("# Complete a buff name\n/cast [buff:xu] wuwu\n/cast bahuang\n/cast sanhuan"));
        auto cursor = editor->textCursor();
        cursor.setPosition(editor->toPlainText().indexOf(QStringLiteral("xu")) + 2);
        editor->setTextCursor(cursor);
        QCoreApplication::processEvents();
        window.findChild<QAction *>("completeMacro")->trigger();
        auto *completer = editor->findChild<QCompleter *>("macroCompleter");
        QVERIFY(completer->popup()->isVisible());
        QCoreApplication::processEvents();
        QVERIFY(completer->popup()->isVisible());
        const auto caret = editor->viewport()->mapToGlobal(editor->cursorRect().bottomLeft());
        const auto popupOrigin = completer->popup()->mapToGlobal(QPoint{});
        QVERIFY(std::abs(popupOrigin.x() - caret.x()) < 10);
        QVERIFY(std::abs(popupOrigin.y() - caret.y()) < 10);
        if (qEnvironmentVariableIsSet("JX3DPS_TEST_SCREENSHOT")) {
            auto screenshot = window.grab();
            QPainter painter(&screenshot);
            painter.drawPixmap(completer->popup()->mapToGlobal(QPoint{}) - window.mapToGlobal(QPoint{}), completer->popup()->grab());
            painter.end();
            QVERIFY(screenshot.save(qEnvironmentVariable("JX3DPS_TEST_SCREENSHOT") + QStringLiteral("-macro-completion.png")));
        }
        completer->popup()->hide();
        QVERIFY(window.close());
    }

    void macroStepObservations()
    {
        DebugSimulator debugger;
        QString error;
        const QString source = QStringLiteral("# source mapping\n/cast [qidian<1&energy>0|qidian>=6] wuwu\n/cast sanhuan");
        debugger.SetOptions({}, 16000, 7);
        QVERIFY2(debugger.Initialize(source, error), qPrintable(error));
        QVERIFY(debugger.SetBreakpoint(2));
        debugger.Continue();
        QVERIFY(debugger.TakeMacroSteps().isEmpty());
        debugger.StepInto();
        QVERIFY(debugger.GetPlayerState().condition.startsWith(QStringLiteral("下一条件预览")));
        QVERIFY(debugger.GetPlayerState().lastStep.isEmpty());
        debugger.StepInto();
        auto steps = debugger.TakeMacroSteps();
        QCOMPARE(steps.size(), 1);
        QCOMPARE(steps[0].line, 2);
        QCOMPARE(steps[0].expression, QStringLiteral("qidian<1"));
        QVERIFY(steps[0].details.contains(QStringLiteral("10 < 1")));
        QVERIFY(steps[0].details.contains(QStringLiteral("短路求值")));
        QCOMPARE(steps[0].outcome, QStringLiteral("不成立"));
        const auto executed = debugger.GetPlayerState().lastStep;
        QVERIFY(!executed.isEmpty());
        QVERIFY(debugger.TakeMacroSteps().isEmpty());
        QCOMPARE(debugger.GetPlayerState().lastStep, executed);
        debugger.StepOver();
        steps = debugger.TakeMacroSteps();
        QCOMPARE(steps.size(), 2);
        QCOMPARE(steps[0].expression, QStringLiteral("qidian>=6"));
        QCOMPARE(steps[1].kind, MacroStepKind::Action);
        QCOMPARE(steps[1].outcome, QStringLiteral("成功"));
        QVERIFY(debugger.GetPlayerState().lastStep.contains(QStringLiteral("成功")));
        debugger.SetBreakpoint(2, false);
        debugger.Continue(1000000);
        QVERIFY(debugger.IsFinished());
        steps = debugger.TakeMacroSteps();
        QCOMPARE(steps.size(), MacroStepHistoryLimit);
        QVERIFY(steps.front().ordinal > 1);
        for (int i = 1; i < steps.size(); ++i) QCOMPARE(steps[i].ordinal, steps[i-1].ordinal + 1);
        debugger.Reset();
        QVERIFY(debugger.TakeMacroSteps().isEmpty());
        QVERIFY(debugger.GetPlayerState().lastStep.isEmpty());
        QVERIFY(debugger.Initialize(QStringLiteral("/cast wuwu"), error));
        debugger.StepOver();
        QCOMPARE(debugger.TakeMacroSteps().front().ordinal, quint64(1));
    }

    void macroStepHistory()
    {
        MacroTracePanel panel;
        panel.Reset(QStringLiteral("/cast wuwu\n/cast sanhuan"));
        MacroSteps steps;
        for (int i = 1; i <= 3000; ++i) {
            MacroStep step;
            step.ordinal = i; step.frame = i; step.line = 2;
            step.kind = i % 2 ? MacroStepKind::Action : MacroStepKind::Condition;
            step.passed = i % 3 == 0;
            step.expression = QStringLiteral("/cast sanhuan");
            step.outcome = step.passed ? QStringLiteral("成功") : QStringLiteral("受阻");
            step.details = QStringLiteral("distance, \"range\" > 4");
            steps.append(step);
        }
        panel.Append(steps);
        auto *table = panel.findChild<QTableView *>("macroSteps");
        auto *scope = panel.findChild<QComboBox *>("macroStepScope");
        auto *search = panel.findChild<QLineEdit *>("macroStepSearch");
        auto *status = panel.findChild<QLabel *>("macroStepStatus");
        auto *locate = panel.findChild<QPushButton *>("locateMacroStep");
        QCOMPARE(table->model()->rowCount(), MacroStepHistoryLimit);
        QCOMPARE(table->model()->index(0, 0).data().toULongLong(), quint64(953));
        QVERIFY(status->text().contains(QStringLiteral("已淘汰 952 条旧记录")));
        scope->setCurrentIndex(3);
        QCOMPARE(table->model()->rowCount(), MacroStepHistoryLimit / 2);
        for (int i = 0; i < table->model()->rowCount(); ++i)
            QCOMPARE(table->model()->index(i, 3).data().toString(), QStringLiteral("动作"));
        scope->setCurrentIndex(1);
        for (int i = 0; i < table->model()->rowCount(); ++i)
            QCOMPARE(table->model()->index(i, 5).data().toString(), QStringLiteral("受阻"));
        search->setText(QStringLiteral("missing"));
        QCOMPARE(table->model()->rowCount(), 0);
        search->setText(QStringLiteral("distance"));
        QVERIFY(table->model()->rowCount() > 0);
        QSignalSpy navigation(&panel, &MacroTracePanel::SourceRequested);
        table->selectRow(0);
        locate->click();
        QCOMPARE(navigation.count(), 1);
        QCOMPARE(navigation[0][0].toString(), QStringLiteral("/cast wuwu\n/cast sanhuan"));
        QCOMPARE(navigation[0][1].toInt(), 2);
        QTemporaryDir directory;
        QString error;
        const auto path = directory.filePath(QStringLiteral("steps.csv"));
        QVERIFY2(panel.ExportCsv(path, error), qPrintable(error));
        QFile file(path);
        QVERIFY(file.open(QIODevice::ReadOnly));
        const auto bytes = file.readAll();
        QCOMPARE(bytes.count('\n'), table->model()->rowCount() + 1);
        QVERIFY(bytes.contains("distance, \"\"range\"\" > 4"));
        QVERIFY(!bytes.contains("\"Accepted\""));
        scope->setCurrentIndex(0);
        search->clear();
        panel.findChild<QCheckBox *>()->setChecked(false);
        table->selectRow(100);
        const auto selectedOrdinal = table->model()->index(table->currentIndex().row(), 0).data().toULongLong();
        auto next = steps.back();
        next.ordinal = 3001;
        panel.Append({next});
        QCOMPARE(table->model()->rowCount(), MacroStepHistoryLimit);
        QCOMPARE(table->model()->index(table->currentIndex().row(), 0).data().toULongLong(), selectedOrdinal);
        panel.findChild<QCheckBox *>()->setChecked(true);
        QCOMPARE(table->model()->index(table->currentIndex().row(), 0).data().toULongLong(), quint64(3001));
        panel.Reset();
        QCOMPARE(table->model()->rowCount(), 0);
        QVERIFY(!locate->isEnabled());
    }

    void workbenchMacroSteps()
    {
        Workbench window;
        window.show();
        auto *editor = window.findChild<CodeEditor *>("macroEditor");
        auto *session = window.findChild<DebugSession *>();
        auto *toolbar = window.findChild<SimulationToolbar *>();
        auto *panel = window.findChild<MacroTracePanel *>();
        auto *table = panel->findChild<QTableView *>("macroSteps");
        auto *inspector = window.findChild<QTextBrowser *>("debugInspector");
        editor->setPlainText(QStringLiteral("# Explain conditions and blocked casts\n/cast [qidian<1&energy>0|qidian>=6] wuwu\n/cast bahuang\n/cast sanhuan"));
        window.findChild<QToolButton *>("debugButton")->setChecked(true);
        emit toolbar->StartSimulationClicked();
        QCOMPARE(session->GetState(), DebugSession::Paused);
        QCOMPARE(table->model()->rowCount(), 0);
        session->StepOver();
        QCOMPARE(table->model()->rowCount(), 3);
        session->StepOut();
        QVERIFY(table->model()->rowCount() >= 6);
        QVERIFY(inspector->toPlainText().contains(QStringLiteral("上一步执行结果")));
        QVERIFY(inspector->toPlainText().contains(QStringLiteral("公共冷却")));
        window.findChild<QTabWidget *>("workspaceTabs")->setCurrentWidget(window.findChild<QWidget *>("debugPage"));
        table->selectRow(0);
        panel->findChild<QPushButton *>("locateMacroStep")->click();
        QCOMPARE(editor->textCursor().blockNumber(), 1);
        table->selectRow(table->model()->rowCount() - 1);
        QCoreApplication::processEvents();
        if (qEnvironmentVariableIsSet("JX3DPS_TEST_SCREENSHOT"))
            QVERIFY(window.grab().save(qEnvironmentVariable("JX3DPS_TEST_SCREENSHOT") + QStringLiteral("-macro-steps.png")));
        session->Stop();
        QVERIFY(table->model()->rowCount() > 0);
        editor->setPlainText(QStringLiteral("# edited\n/cast sanhuan"));
        const auto cursor = editor->textCursor().position();
        panel->findChild<QPushButton *>("locateMacroStep")->click();
        QCOMPARE(editor->textCursor().position(), cursor);
        QVERIFY(window.findChild<QLabel *>("simulationResult")->text().contains(QStringLiteral("先前的宏")));
        emit toolbar->StartSimulationClicked();
        QCOMPARE(table->model()->rowCount(), 0);
        session->StepOver();
        QCOMPARE(table->model()->index(0, 0).data().toULongLong(), quint64(1));
        session->Stop();
        QVERIFY(window.close());
    }

    void buffCoverageMath()
    {
        BuffTrace trace;
        trace.Reset(100);
        const BuffEvents initial{{0, 0, 0, false, "self", 1, 64, 0, true},
                                 {0, 0, 0, true, "target", 3, 32, 16}};
        trace.Append(initial);
        trace.Observe(0);
        QCOMPARE(trace.Coverage()[0].activeFrames, qint64(0));
        trace.Observe(16);
        QCOMPARE(trace.Coverage()[0].activeFrames, qint64(16));
        trace.Append({{16, 1, 0, false, "self", 3, 64, 0},
                      {16, 2, 0, false, "self", 2, 80, 0},
                      {32, 3, 0, false, "self", 2, 96, 0},
                      {48, 4, 0, false, "self", 2, 96, 64}});
        trace.Observe(64);
        auto coverage = trace.Coverage();
        QCOMPARE(coverage[0].activeFrames, qint64(64));
        QCOMPARE(coverage[0].stackFrames, qint64(112));
        QCOMPARE(coverage[0].peakStacks, 3);
        QCOMPARE(coverage[0].changes, 4);
        QCOMPARE(coverage[0].intervals, QVector<BuffInterval>({{0, 16, 1}, {16, 64, 2}}));
        QCOMPARE(coverage[1].activeFrames, qint64(32));
        QCOMPARE(coverage[1].stackFrames, qint64(96));
        QCOMPARE(trace.Changes()[0].kind, QStringLiteral("初始"));
        QCOMPARE(trace.Changes()[4].kind, QStringLiteral("刷新"));
        QCOMPARE(trace.Changes()[5].kind, QStringLiteral("计时更新"));
        trace.Append({{64, 5, 0, false, "self", 0, 0, 0}, {64, 6, 0, true, "target", 1, 200, 80}});
        trace.Observe(100);
        coverage = trace.Coverage();
        QCOMPARE(coverage[0].activeFrames, qint64(64));
        QCOMPARE(coverage[0].latest.stacks, 0);
        QCOMPARE(coverage[1].activeFrames, qint64(68));
        QCOMPARE(coverage[1].stackFrames, qint64(132));
        QCOMPARE(coverage[1].intervals, QVector<BuffInterval>({{0, 32, 3}, {64, 100, 1}}));
        trace.Observe(200);
        QCOMPARE(trace.ObservedFrame(), 100);
        QCOMPARE(trace.Coverage()[1].activeFrames, qint64(68));

        rt::State state;
        state.Resize(tx::SkillCount, tx::BuffCount);
        state.self_buff_stacks[tx::Purple] = 1;
        state.self_buff_expires_at[tx::Purple] = 160;
        state.target_buff_stacks[tx::DieRen] = 3;
        state.target_buff_expires_at[tx::DieRen] = 384;
        const auto starting = InitialBuffEvents(state);
        QCOMPARE(starting.size(), 2);
        QVERIFY(starting[0].initial && starting[1].initial);
        QVERIFY(starting[0].target);
        QCOMPARE(starting[0].stacks, 3);
        trace.Reset(16);
        QVERIFY(trace.Coverage().isEmpty());
        QVERIFY(trace.Changes().isEmpty());
        trace.Append({{0, 0, 0, false, "pulse", 1, 0, 0}, {0, 1, 0, false, "pulse", 0, 0, 0}});
        trace.Observe(16);
        QCOMPARE(trace.Coverage()[0].activeFrames, qint64(0));
        QVERIFY(trace.Coverage()[0].intervals.isEmpty());
    }

    void buffFilteringAndExport()
    {
        BuffPanel panel;
        panel.Reset(160);
        panel.Append({{0, 1, 0, false, "Self, \"buff\"", 1, 160, 0}, {16, 3, 0, true, "Target", 2, 160, 32}});
        panel.Observe(32);
        auto *coverage = panel.findChild<QTableWidget *>("buffCoverage");
        auto *changes = panel.findChild<QTableWidget *>("buffChanges");
        QCOMPARE(coverage->rowCount(), 2);
        QCOMPARE(coverage->item(0, 5)->text(), QStringLiteral("100.00"));
        QCOMPARE(coverage->item(1, 5)->text(), QStringLiteral("50.00"));
        QCOMPARE(coverage->item(1, 6)->text(), QStringLiteral("1.000"));
        QCOMPARE(panel.TimelineBuffs().size(), 2);
        QCOMPARE(panel.TimelineBuffs()[1].segments[0].stacks, 2);
        QCOMPARE(panel.TimelineBuffs()[1].segments[0].endMs, 2000);
        QSignalSpy navigation(&panel, &BuffPanel::DamageRequested);
        emit changes->cellActivated(1, 0);
        QCOMPARE(navigation.count(), 1);
        QCOMPARE(navigation.at(0).at(0).toUInt(), quint32(3));
        auto *scope = panel.findChild<QComboBox *>("buffScope");
        scope->setCurrentIndex(2);
        QVERIFY(coverage->isRowHidden(0));
        QVERIFY(changes->isRowHidden(0));
        QVERIFY(!coverage->isRowHidden(1));
        QCOMPARE(panel.TimelineBuffs().size(), 1);
        panel.findChild<QLineEdit *>("buffFilter")->setText(QStringLiteral("missing"));
        QVERIFY(coverage->isRowHidden(1));
        QVERIFY(panel.TimelineBuffs().isEmpty());
        panel.findChild<QLineEdit *>("buffFilter")->clear();
        QTemporaryDir files;
        QString error;
        const auto path = files.filePath(QStringLiteral("buffs.csv"));
        QVERIFY2(panel.ExportCsv(path, true, error), qPrintable(error));
        QFile csv(path);
        QVERIFY(csv.open(QIODevice::ReadOnly));
        const auto bytes = csv.readAll();
        QVERIFY(bytes.contains(QStringLiteral("\"目标增益\",\"Target\"").toUtf8()));
        QVERIFY(bytes.contains("\"50.000000\",\"1.000000\",\"1\",\"32\",\"160\""));
        QVERIFY(!bytes.contains("Self"));
        csv.close();
        scope->setCurrentIndex(0);
        QVERIFY2(panel.ExportCsv(path, false, error), qPrintable(error));
        QVERIFY(csv.open(QIODevice::ReadOnly));
        QVERIFY(csv.readAll().contains("\"Self, \"\"buff\"\"\""));
        panel.Reset(0);
        QCOMPARE(coverage->rowCount(), 0);
        QCOMPARE(changes->rowCount(), 0);
        QVERIFY(!panel.findChild<QPushButton *>("exportBuffChanges")->isEnabled());
    }

    void batchCancellationAndFailure()
    {
        auto compiled = rt::MacroCompiler::Compile("/cast wuwu\n/cast sanhuan", tx::MacroOptions(false));
        rt::BatchOptions options;
        options.iterations = 1000000;
        options.duration = 4800;
        options.workers = 2;
        BatchWorker worker(compiled.first.program, options, {});
        QSignalSpy result(&worker, &BatchWorker::ResultReady);
        QSignalSpy statistics(&worker, &BatchWorker::StatisticsReady);
        QSignalSpy noBuffTrace(&worker, &BatchWorker::BuffTraceReady);
        QSignalSpy noHistory(&worker, &BatchWorker::HistoryReady);
        QSignalSpy failed(&worker, &BatchWorker::Failed);
        int heartbeats = 0;
        QTimer timer;
        connect(&timer, &QTimer::timeout, this, [&] { ++heartbeats; });
        timer.start(1);
        worker.start();
        QTRY_VERIFY_WITH_TIMEOUT(worker.Completed() > 0, 10000);
        QVERIFY(heartbeats > 0);
        worker.RequestCancel();
        QVERIFY(worker.wait(10000));
        QTRY_COMPARE(result.count(), 1);
        QTRY_COMPARE(statistics.count(), 1);
        const auto partial = qvariant_cast<SimulationResult>(statistics.at(0).at(0));
        QCOMPARE(partial.stats.count, worker.Completed());
        QVERIFY(!partial.Complete());
        QCOMPARE(partial.options.seed, options.seed);
        QCOMPARE(failed.count(), 0);
        QCOMPARE(noBuffTrace.count(), 0);
        QCOMPARE(noHistory.count(), 0);
        QVERIFY(result.at(0).at(0).toString().contains(QStringLiteral("已取消")));
        QVERIFY(worker.Completed() < options.iterations);
        auto invalid = tx::Config{};
        invalid.initial_qidian = -1;
        BatchWorker bad(compiled.first.program, options, invalid);
        QSignalSpy errors(&bad, &BatchWorker::Failed);
        bad.start();
        QVERIFY(bad.wait(10000));
        QTRY_COMPARE(errors.count(), 1);

        BatchWorker cancelled(compiled.first.program, options, {});
        QSignalSpy cancelledResult(&cancelled, &BatchWorker::ResultReady);
        QSignalSpy cancelledStats(&cancelled, &BatchWorker::StatisticsReady);
        cancelled.RequestCancel();
        cancelled.start();
        QVERIFY(cancelled.wait(10000));
        QTRY_COMPARE(cancelledResult.count(), 1);
        QCOMPARE(cancelled.Completed(), std::uint64_t(0));
        QTRY_COMPARE(cancelledStats.count(), 1);
        QCOMPARE(qvariant_cast<SimulationResult>(cancelledStats.at(0).at(0)).stats.count, std::uint64_t(0));
    }

    void configurationRoundTrip()
    {
        SimulationOptions options;
        auto *attack = options.findChild<QSpinBox *>("attackBase");
        attack->setValue(45678);
        options.findChild<QLineEdit *>("seed")->setText(QStringLiteral("18446744073709551615"));
        options.findChild<QDoubleSpinBox *>("seconds")->setValue(10.0625);
        options.findChild<QCheckBox *>(QStringLiteral("talent%1").arg(tx::XuanMen))->setChecked(true);
        const auto snapshot = options.Snapshot();
        QTemporaryDir files;
        QVERIFY(files.isValid());
        QString error;
        const auto path = files.filePath(QStringLiteral("config.json"));
        QVERIFY2(options.ExportConfiguration(path, error), qPrintable(error));
        attack->setValue(1);
        QVERIFY2(options.ImportConfiguration(path, error), qPrintable(error));
        QCOMPARE(options.Snapshot(), snapshot);
        QCOMPARE(options.Config().attributes.attack_base, JX3DPS::value_t(45678));
        QCOMPARE(options.Batch().seed, std::numeric_limits<std::uint64_t>::max());
        QCOMPARE(options.Batch().duration, 161);
        QVERIFY(options.Config().talents & tx::XuanMen);

        QSignalSpy changed(attack, &QSpinBox::valueChanged);
        auto reject = [&](QJsonObject bad) {
            QVERIFY(!options.RestoreSnapshot(bad, error));
            QVERIFY(!error.isEmpty());
            QCOMPARE(options.Snapshot(), snapshot);
            QCOMPARE(changed.count(), 0);
        };
        for (const auto &value : {QJsonValue(-1), QJsonValue(1.25), QJsonValue("100"), QJsonValue(1000001)}) {
            auto bad = snapshot;
            auto values = bad.value("values").toObject();
            values.insert("attackBase", value);
            bad.insert("values", values);
            reject(bad);
        }
        for (const auto &value : {QJsonValue("18446744073709551616"), QJsonValue(123), QJsonValue(" 123")}) {
            auto bad = snapshot;
            auto values = bad.value("values").toObject();
            values.insert("seed", value);
            bad.insert("values", values);
            reject(bad);
        }
        auto bad = snapshot;
        bad.insert("version", 999);
        reject(bad);
        auto values = snapshot.value("values").toObject();
        values.remove("workers");
        bad = snapshot;
        bad.insert("values", values);
        reject(bad);
        values = snapshot.value("values").toObject();
        values.insert("attackBase", 12345);
        for (unsigned flag : {tx::Crit2, tx::Crit3, tx::Crit4, tx::Damage3, tx::Damage4})
            values.insert(QStringLiteral("wuwu%1").arg(flag), true);
        bad.insert("values", values);
        reject(bad);

        QFile malformed(path);
        QVERIFY(malformed.open(QIODevice::WriteOnly));
        malformed.write("{ broken");
        malformed.close();
        QVERIFY(!options.ImportConfiguration(path, error));
        QCOMPARE(options.Snapshot(), snapshot);

        auto *profiles = options.findChild<QComboBox *>("configurationProfile");
        profiles->setCurrentText(QStringLiteral("Trial configuration"));
        options.findChild<QPushButton *>("saveProfile")->click();
        attack->setValue(99);
        options.findChild<QPushButton *>("loadProfile")->click();
        QCOMPARE(options.Snapshot(), snapshot);
        SimulationOptions restored;
        restored.findChild<QComboBox *>("configurationProfile")->setCurrentText(QStringLiteral("Trial configuration"));
        restored.findChild<QPushButton *>("loadProfile")->click();
        QCOMPARE(restored.Snapshot(), snapshot);
    }

    void resultAnalysis()
    {
        ResultsPanel panel;
        SimulationOptions options;
        SimulationResult first;
        first.options.iterations = 2;
        first.options.duration = 160;
        first.options.seed = std::numeric_limits<std::uint64_t>::max();
        first.elapsedSeconds = .5;
        first.stats.Add(1000, 0);
        first.stats.Add(3000, 1);
        const auto config = options.Snapshot();
        const QString source = QStringLiteral("/cast sanhuan");
        panel.AddResult(first, source, config);
        auto *history = panel.findChild<QTableWidget *>("resultHistory");
        QCOMPARE(history->item(0, 3)->text(), QStringLiteral("200.0"));
        QCOMPARE(history->item(0, 4)->text(), QStringLiteral("100.0"));
        panel.findChild<QPushButton *>("setResultBaseline")->click();
        auto second = first;
        second.stats = {};
        second.stats.Add(4000, 0);
        second.stats.Add(4000, 1);
        panel.AddResult(second, source, config);
        QCOMPARE(history->item(1, 5)->text(), QStringLiteral("+100.00%"));
        QTemporaryDir files;
        QString error;
        const auto path = files.filePath(QStringLiteral("run.json"));
        QVERIFY2(panel.ExportSelected(path, error), qPrintable(error));
        QFile exported(path);
        QVERIFY(exported.open(QIODevice::ReadOnly));
        const auto report = QJsonDocument::fromJson(exported.readAll()).object();
        QCOMPARE(report.value("macro").toString(), source);
        QCOMPARE(report.value("configuration").toObject(), config);
        const auto stats = report.value("result").toObject();
        QCOMPARE(stats.value("seed").toString(), QStringLiteral("18446744073709551615"));
        QCOMPARE(stats.value("checksum").toString(), QString::number(second.stats.checksum));
        QCOMPARE(stats.value("meanDps").toDouble(), 400.0);
        QCOMPARE(stats.value("minDamage").toString(), QStringLiteral("4000"));
        QSignalSpy inputs(&panel, &ResultsPanel::InputsRequested);
        panel.SetInputsEnabled(false);
        QVERIFY(!panel.findChild<QPushButton *>("useResultInputs")->isEnabled());
        panel.SetInputsEnabled(true);
        panel.findChild<QPushButton *>("useResultInputs")->click();
        QCOMPARE(inputs.count(), 1);
        QCOMPARE(inputs.at(0).at(0).toString(), source);

        panel.ClearTrace(10);
        panel.AppendTrace({{0, 0, "skill A", 100, 0}, {16, 0, "skill A", 200, 1}, {32, 1, "skill B", 50, 4}});
        panel.AppendTrace({{48, 0, "skill A", 0, 126}});
        auto *damage = panel.findChild<QTableWidget *>("skillDamage");
        QCOMPARE(damage->rowCount(), 2);
        QCOMPARE(damage->item(0, 1)->text(), QStringLiteral("3"));
        QCOMPARE(damage->item(0, 2)->text(), QStringLiteral("300"));
        QCOMPARE(damage->item(0, 3)->text(), QStringLiteral("85.7%"));
        QCOMPARE(damage->item(0, 4)->text(), QStringLiteral("30.0"));
        QCOMPARE(damage->item(0, 7)->text(), QStringLiteral("1"));
        QCOMPARE(damage->item(0, 8)->text(), QStringLiteral("33.3%"));
        QCOMPARE(damage->item(0, 10)->text(), QStringLiteral("1"));
        QCOMPARE(damage->item(1, 9)->text(), QStringLiteral("1"));
        panel.ClearTrace(10);
        panel.AppendTrace({{0, 0, "skill A", 0, 126}});
        QCOMPARE(damage->item(0, 3)->text(), QStringLiteral("0.0%"));

        auto empty = first;
        empty.stats = {};
        panel.AddResult(empty, source, config);
        QCOMPARE(history->item(2, 3)->text(), QStringLiteral("--"));
        QVERIFY(!panel.findChild<QPushButton *>("setResultBaseline")->isEnabled());
        for (int i = 0; i < 50; ++i) panel.AddResult(second, source, config);
        QCOMPARE(history->rowCount(), 50);
        QCOMPARE(history->item(49, 5)->text(), QStringLiteral("+100.00%"));
        QVERIFY(panel.findChild<QTextBrowser *>("resultDetails")->toPlainText().contains(QStringLiteral("基准 #1")));
    }

    void workbenchCapturesInputs()
    {
        Workbench window;
        auto *editor = window.findChild<CodeEditor *>("macroEditor");
        const QString source = QStringLiteral("/cast sanhuan");
        editor->setPlainText(source);
        window.findChild<QSpinBox *>("iterations")->setValue(100);
        window.findChild<QSpinBox *>("workers")->setValue(2);
        window.findChild<QDoubleSpinBox *>("seconds")->setValue(5);
        auto *toolbar = window.findChild<SimulationToolbar *>();
        emit toolbar->StartSimulationClicked();
        QVERIFY(!window.findChild<QComboBox *>("specialization")->isEnabled());
        editor->setPlainText(QStringLiteral("/cast wuwu"));
        QTRY_VERIFY_WITH_TIMEOUT(toolbar->isEnabled(), 10000);
        auto *results = window.findChild<ResultsPanel *>();
        QCOMPARE(results->findChild<QTableWidget *>("resultHistory")->rowCount(), 1);
        auto *details = results->findChild<QTextBrowser *>("resultDetails");
        QVERIFY(details->toPlainText().contains(source));
        QVERIFY(!details->toPlainText().contains(QStringLiteral("/cast wuwu")));
        window.findChild<QSpinBox *>("iterations")->setValue(1);
        editor->document()->setModified(false);
        results->findChild<QPushButton *>("useResultInputs")->click();
        QCOMPARE(editor->toPlainText(), source);
        QVERIFY(editor->document()->isModified());
        QCOMPARE(window.findChild<QSpinBox *>("iterations")->value(), 100);
    }

    void workbenchFlow()
    {
        Workbench window;
        window.show();
        auto *editor = window.findChild<CodeEditor *>("macroEditor");
        auto *table = window.findChild<QTableWidget *>("damageEvents");
        auto *problems = window.findChild<QTableWidget *>("macroProblems");
        auto *check = window.findChild<QAction *>("checkMacro");
        auto *trace = window.findChild<QAction *>("singleTrace");
        auto *session = window.findChild<DebugSession *>();
        QVERIFY(editor && table && problems && check && trace && session);
        QCoreApplication::processEvents();
        if (qEnvironmentVariableIsSet("JX3DPS_TEST_SCREENSHOT"))
            QVERIFY(window.grab().save(qEnvironmentVariable("JX3DPS_TEST_SCREENSHOT") + QStringLiteral("-configuration.png")));
        editor->setPlainText(QStringLiteral("/cast [buff:\u7384\u95e8] wuwu\n/cast [buff:\u7384\u95e8>0] wuwu\n/cast sanhuan"));
        check->trigger();
        QCOMPARE(problems->rowCount(), 0);
        editor->appendPlainText(QStringLiteral("/cast unknown_skill"));
        check->trigger();
        QCOMPARE(problems->rowCount(), 1);
        QCOMPARE(problems->item(0, 0)->text(), QStringLiteral("4"));

        editor->setPlainText(QStringLiteral("/cast [qidian>5] wuwu\n/cast sanhuan"));
        editor->SetBreakpoint(1, true);
        window.findChild<QDoubleSpinBox *>("seconds")->setValue(30);
        window.findChild<QLineEdit *>("seed")->setText(QStringLiteral("77"));
        trace->trigger();
        QTRY_VERIFY_WITH_TIMEOUT(trace->isEnabled(), 10000);
        QVERIFY(table->rowCount() > 1);
        if (qEnvironmentVariableIsSet("JX3DPS_TEST_SCREENSHOT"))
            QVERIFY(window.grab().save(qEnvironmentVariable("JX3DPS_TEST_SCREENSHOT") + QStringLiteral("-trace.png")));
        qint64 total = 0;
        for (int i = 0; i < table->rowCount(); ++i) total += table->item(i, 3)->text().toLongLong();
        auto *results = window.findChild<ResultsPanel *>();
        QCOMPARE(results->findChild<QTableWidget *>("resultHistory")->rowCount(), 1);
        auto *skills = results->findChild<QTableWidget *>("skillDamage");
        qint64 skillsTotal = 0;
        for (int i = 0; i < skills->rowCount(); ++i) skillsTotal += skills->item(i, 2)->text().toLongLong();
        QCOMPARE(skillsTotal, total);
        if (qEnvironmentVariableIsSet("JX3DPS_TEST_SCREENSHOT")) {
            auto *views = window.findChild<QTabWidget *>("analysisViews");
            views->setCurrentWidget(results);
            QCoreApplication::processEvents();
            QVERIFY(window.grab().save(qEnvironmentVariable("JX3DPS_TEST_SCREENSHOT") + QStringLiteral("-results.png")));
            results->findChild<QTabWidget *>("resultTabs")->setCurrentIndex(1);
            QCoreApplication::processEvents();
            QVERIFY(window.grab().save(qEnvironmentVariable("JX3DPS_TEST_SCREENSHOT") + QStringLiteral("-skills.png")));
            views->setCurrentIndex(0);
        }
        auto *mode = window.findChild<QToolButton *>("debugButton");
        QVERIFY(mode);
        mode->setChecked(true);
        auto *toolbar = window.findChild<SimulationToolbar *>();
        emit toolbar->StartSimulationClicked();
        QCOMPARE(session->GetState(), DebugSession::Paused);
        QVERIFY(editor->isReadOnly());
        const auto source = editor->toPlainText();
        QTest::keyClick(editor, Qt::Key_Tab);
        QTest::keyClick(editor, Qt::Key_Return);
        editor->FormatDocument();
        QCOMPARE(editor->toPlainText(), source);
        QVERIFY(QMetaObject::invokeMethod(editor, "CheckSyntax", Qt::DirectConnection));
        session->StepInto();
        QCOMPARE(session->GetDebugInfo().phase, QStringLiteral("条件"));
        QVERIFY(session->GetDebugInfo().condition.contains(QStringLiteral("成立")));
        if (qEnvironmentVariableIsSet("JX3DPS_TEST_SCREENSHOT"))
            QVERIFY(window.grab().save(qEnvironmentVariable("JX3DPS_TEST_SCREENSHOT")));
        session->RemoveBreakpoint(1);
        session->Continue();
        QTRY_COMPARE_WITH_TIMEOUT(session->GetState(), DebugSession::Finished, 10000);
        QCOMPARE(session->TotalDamage(), total);
        QVERIFY(!editor->isReadOnly());
        session->Stop();
        QVERIFY(editor->HasBreakpoint(1));
        editor->document()->setModified(true);
        QVERIFY(window.close());
        Workbench restored;
        auto *restoredEditor = restored.findChild<CodeEditor *>("macroEditor");
        QCOMPARE(restoredEditor->toPlainText(), source);
        QVERIFY(restoredEditor->HasBreakpoint(1));
        QVERIFY(restoredEditor->document()->isModified());
        QCOMPARE(restored.findChild<QLineEdit *>("seed")->text(), QStringLiteral("77"));
    }

    void breakpointsFollowEdits()
    {
        Workbench window;
        auto *editor = window.findChild<CodeEditor *>("macroEditor");
        auto *session = window.findChild<DebugSession *>();
        editor->setPlainText(QStringLiteral("/cast ziqi\n/cast wuwu\n/cast sanhuan"));
        editor->SetBreakpoint(2, true);
        QTextCursor cursor(editor->document());
        cursor.insertText(QStringLiteral("# comment\n"));
        QVERIFY(editor->HasBreakpoint(3));
        QVERIFY(session->HasBreakpoint(3));
        QVERIFY(!session->HasBreakpoint(2));
        editor->undo();
        QVERIFY(editor->HasBreakpoint(2));
        QVERIFY(session->HasBreakpoint(2));
        editor->SetBreakpoint(2, false);
        QVERIFY(editor->GetBreakpoints().isEmpty());
        QVERIFY(session->GetBreakpoints().isEmpty());
    }

    void legacyIconControls()
    {
        for (const auto specialization : {desktop::Specialization::TaiXu, desktop::Specialization::MoWen}) {
            const auto catalog = desktop::Describe(specialization);
            for (const auto &skill : catalog.skills)
                QVERIFY2(!desktop::GameIcon(specialization, desktop::DisplayName(skill.name)).pixmap(40, 40).isNull(), skill.name.data());
            for (const auto &buff : catalog.buffs)
                QVERIFY2(!desktop::GameIcon(specialization, desktop::DisplayName(buff.name)).pixmap(32, 32).isNull(), buff.name.data());
        }
        QCOMPARE(desktop::GameIcon(desktop::Specialization::TaiXu, QStringLiteral("无我无剑")).pixmap(40, 40).toImage(),
            QIcon(QStringLiteral(":/resources/images/JX3/Icons/617.png")).pixmap(40, 40).toImage());
        SimulationOptions options;
        options.resize(860, 800);
        options.show();
        for (auto *check : options.findChildren<QCheckBox *>()) {
            if (check->objectName().startsWith("talent") || check->objectName().startsWith("mwTalent")) {
                QVERIFY2(!check->icon().pixmap(36, 36).isNull(), qPrintable(check->text()));
                QCOMPARE(check->size(), QSize(44, 44));
                QVERIFY(!check->toolTip().isEmpty());
            }
        }
        for (auto *combo : options.findChildren<QComboBox *>()) {
            if (!combo->objectName().startsWith("permanent")) continue;
            for (int index = 1; index < combo->count(); ++index) {
                QVERIFY(!combo->itemIcon(index).pixmap(36, 36).isNull());
                QVERIFY(!combo->itemData(index, Qt::ToolTipRole).toString().isEmpty());
            }
        }
        auto *food = options.findChild<QComboBox *>("permanent0FoodEnhance");
        auto *talent = options.findChild<QCheckBox *>(QStringLiteral("talent%1").arg(tx::DieRenTalent));
        QVERIFY(food && talent);
        QTest::mouseClick(talent, Qt::LeftButton, {}, QPoint(20, 20));
        QVERIFY(talent->isChecked());
        QTest::keyClick(talent, Qt::Key_Space);
        QVERIFY(!talent->isChecked());
        QTest::keyClick(food, Qt::Key_Down);
        QCOMPARE(food->currentIndex(), 1);
        QVERIFY(food->toolTip().contains(food->currentText()));
        food->showPopup();
        QCoreApplication::processEvents();
        QVERIFY(food->view()->isVisible());
        QVERIFY(food->view()->window()->height() <= 60);
        if (qEnvironmentVariableIsSet("JX3DPS_TEST_SCREENSHOT"))
            QVERIFY(food->view()->window()->grab().save(qEnvironmentVariable("JX3DPS_TEST_SCREENSHOT") + QStringLiteral("-food-picker.png")));
        const auto third = food->model()->index(2, 0);
        const auto choice = food->view()->visualRect(third);
        QVERIFY(!choice.isEmpty());
        QTest::mouseClick(food->view()->viewport(), Qt::LeftButton, {}, choice.center());
        QCOMPARE(food->currentIndex(), 2);
        QTest::keyClick(food, Qt::Key_Home);
        QCOMPARE(food->currentIndex(), 0);
        food->setCurrentIndex(1);
        const auto snapshot = options.Snapshot();
        food->setCurrentIndex(0);
        QString error;
        QVERIFY2(options.RestoreSnapshot(snapshot, error), qPrintable(error));
        QCOMPARE(food->currentIndex(), 1);
    }

    void timelineContinuousCursor()
    {
        Timeline timeline;
        timeline.resize(800, 240);
        Timeline::BuffItem buff;
        buff.name = QStringLiteral("自身增益：紫气东来");
        buff.icon = desktop::GameIcon(desktop::Specialization::TaiXu, QStringLiteral("紫气东来")).pixmap(32, 32);
        buff.segments = {{0, 1000, 1}};
        timeline.SetBuffs({buff});
        timeline.SetTimeRange(0, 1000);
        timeline.show();
        QSignalSpy cursor(&timeline, &Timeline::CursorTimeChanged), leave(&timeline, &Timeline::CursorLeft);
        QTest::mouseMove(&timeline, QPoint(420, 155));
        QVERIFY(!cursor.isEmpty());
        QCOMPARE(cursor.last()[0].toDouble(), 500.0);
        QTest::mouseMove(&timeline, QPoint(610, 195));
        QCOMPARE(cursor.last()[0].toDouble(), 750.0);
        timeline.SetTimeRange(500, 1000);
        QCOMPARE(cursor.last()[0].toDouble(), 875.0);
        QTest::mouseMove(&timeline, QPoint(20, 195));
        QVERIFY(!leave.isEmpty());
        const int count = leave.size();
        QTest::mouseMove(&timeline, QPoint(610, 155));
        QEvent event(QEvent::Leave);
        QApplication::sendEvent(&timeline, &event);
        QCOMPARE(leave.size(), count + 1);
    }

    void historicalAttributesAndSeeking()
    {
        const QString source = QStringLiteral("/scast 紫气东来\n/cast 三柴剑法");
        tx::Config config;
        config.initial_qidian = 0;
        DebugSimulator debugger;
        debugger.SetOptions(config, 16 * 15, 913);
        QString error;
        QVERIFY2(debugger.Initialize(source, error), qPrintable(error));
        desktop::CombatHistory history;
        history.Append(debugger.TakeHistory());
        const auto initial = history.At(0);
        QVERIFY(initial.has_value());
        QCOMPARE(initial->state.qidian, 1);
        QVERIFY(!history.At(1).has_value());
        const auto initialAttack = initial->attributes.attack;
        int steps = 0;
        while (!debugger.IsFinished()) {
            debugger.StepInto();
            history.Append(debugger.TakeHistory());
            if (++steps > 10000) QFAIL("debugger did not finish");
        }
        const auto active = history.At(16), expired = history.At(16 * 11);
        QVERIFY(active && expired);
        QCOMPARE(active->state.self_buff_stacks[tx::Purple], 1);
        QVERIFY(active->attributes.attack > initialAttack);
        QCOMPARE(expired->state.self_buff_stacks[tx::Purple], 0);
        QCOMPARE(expired->attributes.attack, initialAttack);
        QCOMPARE(active->attributes.criticalChance - expired->attributes.criticalChance, 0.25);
        QCOMPARE(active->attributes.criticalMultiplier - expired->attributes.criticalMultiplier, 0.25);
        QCOMPARE(history.At(16)->attributes, active->attributes);
        QCOMPARE(history.At(0)->state.self_buff_stacks[tx::Purple], 1);
        QVERIFY(!history.At(241));
        QVERIFY(!history.At(-1));
        history.Clear();
        QVERIFY(!history.At(0));
    }

    void historyCheckpointReplay()
    {
        desktop::HistoryChunk chunk;
        chunk.initial.emplace();
        chunk.initial->Resize(tx::SkillCount, tx::BuffCount);
        chunk.attributes.push_back({0, {}});
        for (int frame = 0; frame < 900; ++frame)
            chunk.mutations.push_back({frame, static_cast<unsigned>(frame), rt::MutationKind::Resources, rt::INVALID_SLOT, false, frame % 11, frame, frame / 10});
        chunk.observedFrame = 900;
        desktop::CombatHistory history;
        history.Append(chunk);
        for (const int frame : {899, 0, 255, 256, 511, 512, 257, 900, 300}) {
            const auto result = history.At(frame);
            QVERIFY(result.has_value());
            QCOMPARE(result->state.energy, std::min(frame, 899));
            QCOMPARE(result->state.qidian, std::min(frame, 899) % 11);
        }
    }

    void moWenHistoricalCriticalPower()
    {
        namespace mw = rt::mo_wen;
        mw::Config config;
        config.talents = mw::ZhengLvHeMing;
        DebugSimulator debugger;
        debugger.SetOptions(config, 16 * 40, 7);
        QString error;
        QVERIFY2(debugger.Initialize(QStringLiteral("/scast 正律和鸣\n/cast 五音六律"), error), qPrintable(error));
        while (!debugger.IsFinished()) debugger.Continue();
        desktop::CombatHistory history;
        history.Append(debugger.TakeHistory());
        const auto before = history.At(319), during = history.At(320), after = history.At(560);
        QVERIFY(before && during && after);
        QCOMPARE(before->state.self_buff_stacks[mw::ZhiYinMiaoYi], 0);
        QCOMPARE(during->state.self_buff_stacks[mw::ZhiYinMiaoYi], 1);
        // Timeout grants one stack with 205 extra power in the pinned legacy rules.
        QCOMPARE(during->attributes.criticalMultiplier - before->attributes.criticalMultiplier, 205 / 1024.0);
        QCOMPARE(after->state.self_buff_stacks[mw::ZhiYinMiaoYi], 0);
        QCOMPARE(after->attributes.criticalMultiplier, before->attributes.criticalMultiplier);
    }

    void timelineEventSelection()
    {
        Timeline timeline;
        timeline.resize(800, 210);
        timeline.show();
        Timeline::EventItem event{};
        event.timestamp = 500;
        event.name = QStringLiteral("命中");
        event.damage = 123;
        timeline.SetEvents({event, event});
        timeline.SetTimeRange(0, 1000);
        int selected = -1, clicks = 0;
        connect(&timeline, &Timeline::EventClicked, this, [&](int index, const Timeline::EventItem &item) {
            selected = index;
            ++clicks;
            QCOMPARE(item.damage, qint64(123));
        });
        QTest::mouseClick(&timeline, Qt::LeftButton, {}, QPoint(400, 120));
        QCOMPARE(selected, 1);
        QTest::mouseClick(&timeline, Qt::LeftButton, {}, QPoint(200, 120));
        QCOMPARE(clicks, 1);
        QTest::mousePress(&timeline, Qt::LeftButton, {}, QPoint(400, 120));
        QTest::mouseMove(&timeline, QPoint(430, 120));
        QTest::mouseMove(&timeline, QPoint(400, 120));
        QTest::mouseRelease(&timeline, Qt::LeftButton, {}, QPoint(400, 120));
        QCOMPARE(clicks, 1);
        Timeline::BuffItem buff;
        buff.name = QStringLiteral("Buff: sample");
        buff.color = Qt::green;
        buff.segments = {{0, 1000, 2}};
        timeline.SetBuffs({buff});
        timeline.SetTimeRange(0, 1000);
        QTest::mouseClick(&timeline, Qt::LeftButton, {}, QPoint(420, 120));
        QCOMPARE(clicks, 2);
        QSignalSpy range(&timeline, &Timeline::VisibleRangeChanged);
        for (int i = 0; i < 100; ++i) timeline.ZoomIn();
        QVERIFY(!range.isEmpty());
        QCOMPARE(range.last().at(1).toInt() - range.last().at(0).toInt(), 1);
        for (int i = 0; i < 100; ++i) timeline.ZoomOut();
        QCOMPARE(range.last().at(0).toInt(), 0);
        QCOMPARE(range.last().at(1).toInt(), 1000);
        timeline.Clear();
        QTest::mouseClick(&timeline, Qt::LeftButton, {}, QPoint(400, 120));
        QCOMPARE(clicks, 2);
    }

    void workbenchBuffAnalysis()
    {
        Workbench window;
        window.show();
        auto *editor = window.findChild<CodeEditor *>("macroEditor");
        editor->setPlainText(QStringLiteral("/scast ziqi\n/cast [nobuff:suixing] suixing\n/cast [nobuff:field_shengtaiji] shengtaiji\n"
            "/cast [tnobuff:tunriyue] tunriyue\n/cast [qidian>5] wuwu\n/cast sanhuan"));
        for (unsigned flag : {tx::DieRenTalent, tx::WuYu, tx::XuanMen, tx::QiSheng, tx::GuChang})
            window.findChild<QCheckBox *>(QStringLiteral("talent%1").arg(flag))->setChecked(true);
        window.findChild<QDoubleSpinBox *>("seconds")->setValue(60);
        auto *trace = window.findChild<QAction *>("singleTrace");
        trace->trigger();
        QTRY_VERIFY_WITH_TIMEOUT(trace->isEnabled(), 10000);
        auto *panel = window.findChild<BuffPanel *>();
        auto *coverage = panel->findChild<QTableWidget *>("buffCoverage");
        auto *changes = panel->findChild<QTableWidget *>("buffChanges");
        auto *damage = window.findChild<QTableWidget *>("damageEvents");
        auto *views = window.findChild<QTabWidget *>("analysisViews");
        QVERIFY(coverage->rowCount() >= 5);
        QVERIFY(changes->rowCount() > coverage->rowCount());
        QVERIFY(panel->findChild<QLabel *>("buffStatus")->text().contains(QStringLiteral("60.0000 / 60.0000")));
        QStringList reference;
        for (int row = 0; row < coverage->rowCount(); ++row)
            for (int column = 0; column < coverage->columnCount(); ++column) reference << coverage->item(row, column)->text();
        emit changes->cellActivated(0, 0);
        QCOMPARE(views->currentWidget(), damage);
        QVERIFY(damage->currentRow() >= 0);
        QVERIFY(damage->item(damage->currentRow(), 5)->text().toUInt() >= changes->item(0, 2)->text().toUInt());
        auto *scroll = window.findChild<QScrollArea *>("timelineScroll");
        views->setCurrentIndex(0);
        QCoreApplication::processEvents();
        scroll->setMaximumHeight(220);
        QCoreApplication::processEvents();
        QVERIFY(scroll->verticalScrollBar()->maximum() > 0);
        scroll->setMaximumHeight(QWIDGETSIZE_MAX);
        QCoreApplication::processEvents();
        auto *history = window.findChild<HistoryPanel *>("timelineHistory");
        auto *timeline = window.findChild<Timeline *>("combatTimeline");
        QVERIFY(history && timeline);
        QVERIFY(!history->History().Empty());
        const auto oldAttack = history->History().At(16)->attributes.attack;
        window.findChild<QSpinBox *>("attackBase")->setValue(50000);
        QCOMPARE(history->History().At(16)->attributes.attack, oldAttack);
        window.findChild<QSpinBox *>("attackBase")->setValue(10000);
        timeline->SetTimeRange(0, 16000);
        const QPoint position(timeline->width() / 3, 195);
        QMouseEvent move(QEvent::MouseMove, position, timeline->mapToGlobal(position), Qt::NoButton, Qt::NoButton, Qt::NoModifier);
        QApplication::sendEvent(timeline, &move);
        QCoreApplication::processEvents();
        QVERIFY(!history->findChild<QLabel *>("historyTime")->text().startsWith("0.0000"));
        if (qEnvironmentVariableIsSet("JX3DPS_TEST_SCREENSHOT")) {
            QVERIFY(window.grab().save(qEnvironmentVariable("JX3DPS_TEST_SCREENSHOT") + QStringLiteral("-buff-timeline.png")));
            views->setCurrentWidget(panel);
            QCoreApplication::processEvents();
            QVERIFY(window.grab().save(qEnvironmentVariable("JX3DPS_TEST_SCREENSHOT") + QStringLiteral("-buffs.png")));
            panel->findChild<QTabWidget *>("buffTabs")->setCurrentIndex(1);
            QCoreApplication::processEvents();
            QVERIFY(window.grab().save(qEnvironmentVariable("JX3DPS_TEST_SCREENSHOT") + QStringLiteral("-buff-changes.png")));
        }
        auto *mode = window.findChild<QToolButton *>("debugButton");
        mode->setChecked(true);
        auto *session = window.findChild<DebugSession *>();
        session->AddBreakpoint(2);
        emit window.findChild<SimulationToolbar *>()->StartSimulationClicked();
        QCOMPARE(coverage->rowCount(), 0);
        session->Continue();
        QTRY_COMPARE(session->GetState(), DebugSession::Paused);
        QCOMPARE(session->GetDebugInfo().currentFrame, 0);
        QCOMPARE(coverage->rowCount(), 1);
        QCOMPARE(coverage->item(0, 4)->text(), QStringLiteral("0.0000"));
        QCOMPARE(coverage->item(0, 5)->text(), QStringLiteral("--"));
        session->RemoveBreakpoint(2);
        session->Continue();
        QTRY_COMPARE_WITH_TIMEOUT(session->GetState(), DebugSession::Finished, 10000);
        QStringList debug;
        for (int row = 0; row < coverage->rowCount(); ++row)
            for (int column = 0; column < coverage->columnCount(); ++column) debug << coverage->item(row, column)->text();
        QCOMPARE(debug, reference);
        session->Stop();
        mode->setChecked(false);
        window.findChild<QSpinBox *>("iterations")->setValue(1);
        emit window.findChild<SimulationToolbar *>()->StartSimulationClicked();
        QTRY_VERIFY_WITH_TIMEOUT(trace->isEnabled(), 10000);
        QCOMPARE(coverage->rowCount(), 0);
        QCOMPARE(changes->rowCount(), 0);
        QVERIFY(panel->TimelineBuffs().isEmpty());
        QVERIFY(window.close());
    }

    void closeDuringBatch()
    {
        Workbench window;
        window.show();
        window.findChild<QSpinBox *>("iterations")->setValue(1000000);
        window.findChild<QSpinBox *>("workers")->setValue(2);
        auto *toolbar = window.findChild<SimulationToolbar *>();
        emit toolbar->StartSimulationClicked();
        QVERIFY(!toolbar->isEnabled());
        QVERIFY(window.findChild<QPushButton *>("cancelBatch")->isEnabled());
        QVERIFY(!window.close());
        QTRY_VERIFY_WITH_TIMEOUT(!window.isVisible(), 10000);
        QVERIFY(toolbar->isEnabled());
    }
};

QTEST_MAIN(DesktopTests)
#include "desktop_tests.moc"
