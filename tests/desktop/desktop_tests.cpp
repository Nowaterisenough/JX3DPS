#include <cstdlib>
#include <utility>
#include <variant>

#include <QAction>
#include <QCheckBox>
#include <QComboBox>
#include <QCompleter>
#include <QDialog>
#include <QDir>
#include <QDoubleSpinBox>
#include <QFile>
#include <QFormLayout>
#include <QGroupBox>
#include <QInputMethodEvent>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QPainter>
#include <QProgressBar>
#include <QPushButton>
#include <QScreen>
#include <QScrollArea>
#include <QScrollBar>
#include <QSettings>
#include <QSpinBox>
#include <QStyleOptionGroupBox>
#include <QTableView>
#include <QTableWidget>
#include <QTabWidget>
#include <QTemporaryDir>
#include <QTextBrowser>
#include <QToolButton>
#include <QtTest>

#include "apps/desktop/batch_worker.h"
#include "apps/desktop/buff_panel.h"
#include "apps/desktop/controls/choice_popup.h"
#include "apps/desktop/controls/code_editor/code_editor.h"
#include "apps/desktop/controls/code_editor/debug_toolbar.h"
#include "apps/desktop/controls/code_editor/simulation_toolbar.h"
#include "apps/desktop/controls/equipment_panel/equipment_detail_card.h"
#include "apps/desktop/controls/equipment_panel/equipment_panel.h"
#include "apps/desktop/controls/theme/dark_style.h"
#include "apps/desktop/controls/theme/history_chart.h"
#include "apps/desktop/controls/theme/magpie_shell.h"
#include "apps/desktop/controls/theme/segmented_tabs.h"
#include "apps/desktop/controls/timeline/timeline.h"
#include "apps/desktop/debug_session.h"
#include "apps/desktop/debug_simulator.h"
#include "apps/desktop/flow_macro.h"
#include "apps/desktop/flow_timeline_editor.h"
#include "apps/desktop/game_attributes.h"
#include "apps/desktop/game_icons.h"
#include "apps/desktop/history_panel.h"
#include "apps/desktop/macro_editor.h"
#include "apps/desktop/macro_search_bar.h"
#include "apps/desktop/macro_trace_panel.h"
#include "apps/desktop/results_panel.h"
#include "apps/desktop/release_updater.h"
#include "apps/desktop/simulation_options.h"
#include "apps/desktop/talent_panel.h"
#include "apps/desktop/traced_rules.h"
#include "apps/desktop/workbench.h"

#include "resources.h"

namespace rt = JX3DPS::runtime;

class DesktopTests : public QObject
{
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

    void releasesUseNumericTagsAndMatchTheDesktopPackage()
    {
        using Updater = desktop::ReleaseUpdater;
        QVERIFY(Updater::NumericVersion("v5.42.261123.100") > Updater::NumericVersion("5.42.261123.99"));
        QVERIFY(Updater::NumericVersion("5.42.261123.100-rc1").isNull());
        const QString tag = "5.42.261123.100";
        auto asset = [&](const QString &name) {
            return QJsonObject{{"name", name}, {"digest", "sha256:" + QString(64, 'a')}, {"size", 100},
                               {"browser_download_url", "https://github.com/Nowaterisenough/JX3DPS/releases/download/" + tag + '/' + name}};
        };
        auto gcc = asset("JX3DPS-windows-x86_64-gcc-16.3.0.zip");
        auto msvc = asset("JX3DPS-windows-x86_64-msvc-19.5.zip");
        QJsonObject release{{"tag_name", tag}, {"body", "修复木桩模拟"}, {"assets", QJsonArray{msvc, gcc}}};
        auto selected = Updater::ReadRelease(release, "windows-x86_64-gcc");
        QCOMPARE(selected.package.fileName(), gcc["name"].toString());
        QCOMPARE(selected.notes, QStringLiteral("修复木桩模拟"));
        gcc.remove("digest");
        release["assets"] = QJsonArray{msvc, gcc};
        QVERIFY(Updater::ReadRelease(release, "windows-x86_64-gcc").package.isEmpty());
        release["prerelease"] = true;
        QVERIFY(Updater::ReadRelease(release, "windows-x86_64-msvc").tag.isEmpty());
    }

    void season50Fight()
    {
        Workbench window;
        auto     *options = window.findChild<SimulationOptions *>();
        auto     *school  = options->findChild<QComboBox *>("specialization");
        QVERIFY(school);
        QCOMPARE(school->count(), static_cast<int>(rt::season50::Books().size()));
        QVERIFY(!window.findChild<QToolButton *>("openSeason50"));
        for (int i = 0; i < school->count(); ++i) {
            school->setCurrentIndex(i);
            const auto cfg = options->Config();
            QCOMPARE(desktop::Text(cfg.book->code), school->currentData().toString());
            QCOMPARE(cfg.target.level, 54);
            QVERIFY(!school->itemText(i).contains(QStringLiteral("悟")));
            auto *editor = window.findChild<MacroEditor *>("macroEditor");
            QCOMPARE(editor->toPlainText(), desktop::Text(cfg.book->default_macro));
            const auto compiled =
                rt::MacroCompiler::Compile(cfg.book->default_macro, desktop::MacroOptions(options->Specialization()));
            QVERIFY(compiled.second.empty());
            auto                            rules = desktop::MakeRules(cfg);
            rt::Simulation<decltype(rules)> sim(compiled.first.program, rules);
            sim.Start(320, rt::IterationSeed(42, 0));
            sim.Run();
            QVERIFY(sim.TotalDamage() > 0);
            const rt::BatchOptions options{ 1, 42, 320, 1, 4096, 8192 };
            auto                   handle = JX3DPS::engine::Create();
            auto batch = JX3DPS::engine::Batch(handle.get(), cfg, cfg.book->default_macro, options, true);
            QCOMPARE(batch.primary.mean_damage, double(sim.TotalDamage()));
            struct Reference {
                std::string_view code, skill;
                int              frames, hits;
            };

            constexpr Reference rotations[] = {
                { "lhj", "振翅图南",  4782, 1023 }, // 驾鸾默认宏，298.875 秒的对照木桩
                { "axzy", "龙牙", 4806, 86 },
                { "baj", "破釜沉舟", 4809, 85 },
                { "wf",  "无方中和",  4813, 407 },
                { "xcj", "灼烧(DOT)", 2955, 100 },
                { "yjj", "果报",      4813, 71  },
                { "dj",  "蝎心",      4378, 146 },
                { "bxj", "玳弦急曲",  4791, 371 }
            };
            for (const auto &reference : rotations) {
                if (cfg.book->code != reference.code) {
                    continue;
                }
                sim.Start(reference.frames, rt::IterationSeed(42, 0));
                sim.Run();
                const auto count = std::ranges::count_if(sim.Log().Intents(), [&](const auto &h) {
                    return cfg.book->skills[h.skill].name == reference.skill;
                });
                QCOMPARE(count, reference.hits);
            }
        }
        school->setCurrentIndex(school->findData("mw"));
        QCOMPARE(options->Config().book->code, std::string_view("mw"));
    }

    void iconAccentExtraction()
    {
        namespace theme = desktop::theme;
        QCOMPARE(theme::IconAccent(QImage{}), theme::DefaultAccent());
        QImage gray(64, 64, QImage::Format_ARGB32);
        gray.fill(QColor("#999999"));
        QCOMPARE(theme::IconAccent(gray), theme::DefaultAccent());
        for (const QColor SOURCE : { QColor("#ed3020"), QColor("#20b060"), QColor("#244add"), QColor("#e4b020") }) {
            QImage image(64, 64, QImage::Format_ARGB32);
            image.fill(qRgba(255, 0, 255, 0));
            QPainter painter(&image);
            painter.fillRect(QRect(4, 4, 56, 56), QColor("#080808"));
            painter.fillRect(QRect(8, 8, 48, 48), SOURCE);
            painter.fillRect(QRect(24, 24, 16, 16), Qt::white);
            painter.end();
            const auto accent   = theme::IconAccent(image);
            const auto DISTANCE = std::abs(accent.hslHueF() - SOURCE.hslHueF());
            QVERIFY(std::min(DISTANCE, 1 - DISTANCE) < .02);
            QCOMPARE(theme::IconAccent(image), accent);
            QVERIFY(theme::Contrast(accent, QColor(theme::Surface)) >= 3);
            const auto COLORS = theme::MakeAccentColors(accent);
            for (const auto &background : { COLORS.normal, COLORS.hover, COLORS.pressed }) {
                QVERIFY(theme::Contrast(background, COLORS.text) >= 4.5);
            }
        }
        const auto TAI_XU = theme::IconAccent(desktop::GameIcon(8484).pixmap(QSize(64, 64), 1.0).toImage());
        const auto MO_WEN = theme::IconAccent(desktop::GameIcon(8501).pixmap(QSize(64, 64), 1.0).toImage());
        QVERIFY(TAI_XU != MO_WEN);
        QVERIFY(TAI_XU != theme::DefaultAccent() && MO_WEN != theme::DefaultAccent());
        qInfo().noquote() << "Icon-derived accents:" << TAI_XU.name() << MO_WEN.name();
    }

    void specializationThemeTracksIcon()
    {
        namespace theme = desktop::theme;
        Workbench window;
        window.resize(1206, 860);
        window.show();
        auto       *choice  = window.findChild<QComboBox *>("specialization");
        const auto *options = window.findChild<SimulationOptions *>();
        auto       *start   = window.findChild<QToolButton *>("startButton");
        auto       *editor  = window.findChild<MacroEditor *>("macroEditor");
        editor->appendPlainText(QStringLiteral("\n# 保留主题切换前的宏"));
        editor->SetBreakpoint(1, true);
        const auto SOURCE   = editor->toPlainText();
        const auto snapshot = options->Snapshot();
        const auto EXPECTED = [&](int index) {
            return theme::IconAccent(choice->itemIcon(index).pixmap(QSize(64, 64), 1.0).toImage());
        };
        const auto PIXEL = [](QWidget *widget, QPoint point) {
            const auto IMAGE = widget->grab().toImage();
            return IMAGE.pixelColor(qRound(point.x() * IMAGE.devicePixelRatio()), qRound(point.y() * IMAGE.devicePixelRatio()));
        };
        for (const int INDEX : { 0, 1, 0 }) {
            choice->setCurrentIndex(INDEX);
            QTest::qWait(100);
            const auto accent = EXPECTED(INDEX);
            QCOMPARE(theme::AccentColor(&window), accent);
            QCOMPARE(theme::AccentColor(start), accent);
            QCOMPARE(start->palette().color(QPalette::Accent), accent);
            QVERIFY(!window.styleSheet().contains("#a8b3ff"));
            start->setAttribute(Qt::WA_UnderMouse, false);
            QCOMPARE(PIXEL(start, QPoint(10, start->height() / 2)), QColor(accent.name()));
            QCOMPARE(PIXEL(choice, QPoint(1, choice->height() / 2)), QColor(accent.name()));
            const auto STABLE = options->Snapshot();
            window.findChild<QToolButton *>("openEquipment")->click();
            auto *equipment = window.findChild<QDialog *>("equipmentDialog");
            QTRY_VERIFY(equipment->isVisible());
            QCOMPARE(theme::AccentColor(equipment), accent);
            QCOMPARE(equipment->palette().color(QPalette::Accent), accent);
            equipment->close();
            choice->showPopup();
            QTRY_VERIFY(choice->view()->isVisible());
            QCOMPARE(theme::AccentColor(choice->view()), accent);
            QCOMPARE(choice->view()->palette().color(QPalette::Accent), accent);
            choice->hidePopup();
            QCOMPARE(options->Snapshot(), STABLE);
            QCOMPARE(QColor(theme::Success), QColor("#6fd99b"));
            QCOMPARE(QColor(theme::Error), QColor("#f08078"));
            if (qEnvironmentVariableIsSet("JX3DPS_TEST_SCREENSHOT")) {
                QTest::qWait(desktop::motion::SlideMs + 40);
                QVERIFY(window.grab().save(qEnvironmentVariable("JX3DPS_TEST_SCREENSHOT") +
                                           QStringLiteral("-theme-class%1.png").arg(INDEX)));
            }
        }
        QCOMPARE(editor->toPlainText(), SOURCE);
        QVERIFY(editor->GetBreakpoints().contains(1));
        QCOMPARE(options->Snapshot(), snapshot);

        // Follow the artwork actually displayed by the selector, not a hard-coded class/color mapping.
        const auto ORIGINAL = choice->itemIcon(0);
        QPixmap    changed(64, 64);
        changed.fill(QColor("#e15f27"));
        choice->setItemIcon(0, QIcon(changed));
        choice->setCurrentIndex(1);
        choice->setCurrentIndex(0);
        QCOMPARE(theme::AccentColor(&window), theme::IconAccent(changed.toImage()));
        choice->setItemIcon(0, ORIGINAL);
        choice->setCurrentIndex(1);
        choice->setCurrentIndex(0);

        const auto FIRST_ACCENT = theme::AccentColor(&window);
        choice->setCurrentIndex(1);
        options->SaveSettings();
        choice->setCurrentIndex(0);
        Workbench restored;
        QCOMPARE(restored.findChild<QComboBox *>("specialization")->currentIndex(), 1);
        QCOMPARE(theme::AccentColor(&restored), EXPECTED(1));
        QCOMPARE(theme::AccentColor(&window), FIRST_ACCENT);
        QCOMPARE(theme::AccentColor(start), FIRST_ACCENT);
    }

    void gameAttributeOrderAndEquipmentTalents()
    {
        Workbench window;
        window.resize(1206, 860);
        window.show();
        QTest::qWait(100);
        auto *options         = window.findChild<SimulationOptions *>();
        auto *attribute_panel = window.findChild<QWidget *>("gameAttributePanel");
        auto *gains_panel     = window.findChild<QWidget *>("attributeGainsModule");
        QVERIFY(attribute_panel && gains_panel);
        QVERIFY(std::abs(attribute_panel->width() - gains_panel->width()) <= 1);
        for (const auto *panel : { attribute_panel, gains_panel }) {
            const auto *title  = panel->findChild<QLabel *>("attributeModuleTitle");
            const auto *header = panel->findChild<QWidget *>("attributeModuleHeader");
            const auto *body   = panel->findChild<QWidget *>("attributeModuleBody");
            QVERIFY(title && header && body);
            QVERIFY(!panel->findChild<QWidget *>("attributeModuleDivider"));
            QVERIFY(!body->isAncestorOf(title));
            QVERIFY(header->geometry().bottom() < body->geometry().top());
            QVERIFY(body->property("statModule").toBool());
            QVERIFY(!panel->property("statModule").toBool());
            QCOMPARE(title->font().pixelSize(), window.findChild<QLabel *>("gameAttributeValue_attack")->font().pixelSize());
            QVERIFY(title->font().weight() >= QFont::DemiBold);
            const auto *first_row =
                panel->findChild<QWidget *>(panel == attribute_panel ? "gameAttributeRow_major" : "gameAttributeGainRow_major");
            QVERIFY(body->isAncestorOf(first_row));
            QVERIFY(first_row->mapTo(body, QPoint()).y() >= 6);
        }
        for (const auto *name : { "runSettings", "permanentPanel", "macroPanel" }) {
            const auto *box = window.findChild<QGroupBox *>(QString::fromLatin1(name));
            QVERIFY(box);
            QStyleOptionGroupBox option;
            option.initFrom(box);
            option.text          = box->title();
            option.textAlignment = Qt::AlignLeft;
            option.subControls   = QStyle::SC_GroupBoxFrame | QStyle::SC_GroupBoxLabel;
            const auto TITLE = box->style()->subControlRect(QStyle::CC_GroupBox, &option, QStyle::SC_GroupBoxLabel, box);
            const auto FRAME = box->style()->subControlRect(QStyle::CC_GroupBox, &option, QStyle::SC_GroupBoxFrame, box);
            QVERIFY2(TITLE.bottom() < FRAME.top(), name);
        }
        QVERIFY(!window.findChild<QTabWidget *>("attributeTabs"));
        QVERIFY(gains_panel->mapTo(&window, QPoint()).x() >
                attribute_panel->mapTo(&window, attribute_panel->rect().topRight()).x());
        const auto CAPTURE_PAIR = [&](const QString &suffix) {
            if (!qEnvironmentVariableIsSet("JX3DPS_TEST_SCREENSHOT")) {
                return;
            }
            const QRect left(attribute_panel->mapTo(&window, QPoint()), attribute_panel->size());
            const QRect right(gains_panel->mapTo(&window, QPoint()), gains_panel->size());
            QVERIFY(window.grab(left.united(right)).save(qEnvironmentVariable("JX3DPS_TEST_SCREENSHOT") + suffix));
        };
        int previous_y = -1;
        for (int i = 0; std::cmp_less(i, desktop::GameAttributeRows.size()); ++i) {
            const auto  KEY      = QString::fromLatin1(desktop::GameAttributeRows[i].key);
            const auto *row      = window.findChild<QWidget *>("gameAttributeRow_" + KEY);
            const auto *value    = window.findChild<QLabel *>("gameAttributeValue_" + KEY);
            const auto *gain_row = window.findChild<QWidget *>("gameAttributeGainRow_" + KEY);
            QVERIFY(row && value && gain_row);
            QCOMPARE(row->property("attributeKey"), gain_row->property("attributeKey"));
            QCOMPARE(row->mapTo(&window, row->rect().center()).y(), gain_row->mapTo(&window, gain_row->rect().center()).y());
            QCOMPARE(row->height(), gain_row->height());
            QVERIFY(row->y() > previous_y);
            previous_y = row->y();
            QCOMPARE(row->findChildren<QLabel *>().first()->text(), desktop::GameAttributeName(i, desktop::Specialization::TAI_XU));
            QVERIFY(value->alignment().testFlag(Qt::AlignRight));
            QVERIFY(row->findChildren<QLabel *>().first()->width() >= 56);
            QVERIFY(value->width() >= 88);
            QCOMPARE(value->geometry().right(), row->rect().right());
        }
        const auto *import           = window.findChild<QPushButton *>("importAttributes");
        const auto *calculate_button = window.findChild<QToolButton *>("calculateInlineGains");
        QCOMPARE(import->mapTo(attribute_panel, QPoint()).x(), 12);
        QCOMPARE(import->height(), calculate_button->height());
        QCOMPARE(import->mapTo(&window, QPoint()).y(), calculate_button->mapTo(&window, QPoint()).y());
        QVERIFY(import->property("moduleAction").toBool());
        QVERIFY(calculate_button->property("magpieCheckable").toBool());
        QVERIFY(calculate_button->isCheckable());
        QVERIFY(window.findChild<QLabel *>("gameAttributeGainStatus_major")->isVisible());
        QVERIFY(window.findChild<QLabel *>("gameAttributeGainStatus_hit")->isVisible());
        QVERIFY(!window.findChild<QLabel *>("gameAttributeGain_major")->isVisible());
        QCOMPARE(window.findChild<QLabel *>("gameAttributeGainChange_attack")->text(), QStringLiteral("--"));
        auto       *attribute_row = window.findChild<QWidget *>("gameAttributeRow_critical");
        auto       *gain_row      = window.findChild<QWidget *>("gameAttributeGainRow_critical");
        QEnterEvent enter(QPointF(3, 3), QPointF(3, 3), QPointF(attribute_row->mapToGlobal(QPoint(3, 3))));
        QCoreApplication::sendEvent(attribute_row, &enter);
        QVERIFY(attribute_row->property("comparisonHighlighted").toBool());
        QVERIFY(gain_row->property("comparisonHighlighted").toBool());
        CAPTURE_PAIR(QStringLiteral("-attribute-hover.png"));
        QEvent leave(QEvent::Leave);
        QCoreApplication::sendEvent(attribute_row, &leave);
        QVERIFY(!attribute_row->property("comparisonHighlighted").toBool());
        QVERIFY(!gain_row->property("comparisonHighlighted").toBool());
        QCoreApplication::sendEvent(gain_row, &enter);
        QVERIFY(attribute_row->property("comparisonHighlighted").toBool());
        QVERIFY(gain_row->property("comparisonHighlighted").toBool());
        window.findChild<QPushButton *>("nav_macro")->click();
        QVERIFY(!attribute_row->property("comparisonHighlighted").toBool());
        QVERIFY(!gain_row->property("comparisonHighlighted").toBool());
        window.findChild<QPushButton *>("nav_simulation")->click();
        QCoreApplication::processEvents();
        const auto &base = options->Config().attributes;
        QCOMPARE(window.findChild<QLabel *>("gameAttributeValue_attack")->text(), QString::number(base.attack_base + base.attack_by_class));
        QCOMPARE(window.findChild<QLabel *>("gameAttributeValue_hit")->text(), QStringLiteral("100.00%"));
        CAPTURE_PAIR(QStringLiteral("-attribute-layout.png"));
        window.findChild<QPushButton *>("editGameAttributes")->click();
        auto *editor = window.findChild<QDialog *>("attributeValueEditor");
        QTRY_VERIFY(editor->isVisible());
        auto *attack = window.findChild<QSpinBox *>("attackBase");
        QVERIFY(attack->isVisible());
        attack->setValue(20000);
        QTRY_COMPARE(window.findChild<QLabel *>("gameAttributeValue_attack")->text(), QString::number(20000 + base.attack_by_class));
        editor->close();
        for (const QSize SIZE : { QSize(1206, 860), QSize(1120, 820) }) {
            window.resize(SIZE);
            for (const int specialization : { 0, 1 }) {
                window.findChild<QComboBox *>("specialization")->setCurrentIndex(specialization);
                const auto BEFORE = options->Snapshot();
                window.findChild<QToolButton *>("openEquipment")->click();
                auto *equipment = window.findChild<QDialog *>("equipmentDialog");
                QTRY_VERIFY(equipment->isVisible());
                QVERIFY(equipment->isWindow());
                QVERIFY(!equipment->isModal());
                QTest::qWait(100);
                QVERIFY(attribute_panel->isVisible());
                QVERIFY(gains_panel->isVisible());
                QVERIFY(window.findChild<QWidget *>("configurationTabs")->isVisible());
                const auto *talents = window.findChild<QWidget *>(QStringLiteral("talents%1").arg(specialization));
                for (int tier = 1; tier <= 9; ++tier) {
                    const auto *button = talents->findChild<QToolButton *>(QStringLiteral("talentTier%1").arg(tier));
                    QVERIFY(button->isVisible());
                    QVERIFY(button->visibleRegion().contains(button->rect()));
                }
                QCOMPARE(options->Snapshot(), BEFORE);
                if (qEnvironmentVariableIsSet("JX3DPS_TEST_SCREENSHOT")) {
                    QVERIFY(window.grab().save(qEnvironmentVariable("JX3DPS_TEST_SCREENSHOT") +
                                               QStringLiteral("-equipment-talents-%1-class%2.png").arg(SIZE.width()).arg(specialization)));
                }
                if (qEnvironmentVariableIsSet("JX3DPS_TEST_SCREENSHOT")) {
                    QVERIFY(equipment->grab().save(qEnvironmentVariable("JX3DPS_TEST_SCREENSHOT") +
                                                   QStringLiteral("-equipment-window-class%1.png").arg(specialization)));
                }
                equipment->close();
            }
        }
        SimulationResult fixture;
        fixture.options.duration   = 16;
        fixture.options.iterations = 1;
        fixture.stats.Add(1000, 0);
        fixture.gainsRequested = true;
        for (int i = 7; i >= 0; --i) {
            rt::BatchStats stats;
            stats.Add(1000 + ((i + 1) * 100), 0);
            fixture.gains.push_back({ desktop::AttributeGainName(i), 100, stats });
        }
        QTableWidget gains(0, 3);
        desktop::FillGameGainTable(&gains, fixture, desktop::Specialization::MO_WEN, false);
        QCOMPARE(gains.rowCount(), 10);
        for (int i = 0; i < 10; ++i) {
            QCOMPARE(gains.item(i, 0)->text(), desktop::GameAttributeName(i, desktop::Specialization::MO_WEN));
            QCOMPARE(gains.item(i, 0)->data(Qt::UserRole).toString(), QString::fromLatin1(desktop::GameAttributeRows[i].key));
        }
        QCOMPARE(gains.item(0, 2)->text(), QStringLiteral("暂未支持"));
        QCOMPARE(gains.item(2, 2)->text(), QStringLiteral("固定命中"));
        QCOMPARE(gains.item(5, 1)->text(), QStringLiteral("7.00"));
        QCOMPARE(gains.item(6, 1)->text(), QStringLiteral("4.00"));
        options->SetAttributeGainResult(fixture);
        QVERIFY(window.findChild<QLabel *>("gameAttributeGain_haste")->isVisible());
        attack->setValue(21000);
        QTRY_COMPARE(window.findChild<QLabel *>("gameAttributeGain_haste")->text(), QStringLiteral("--"));
        QVERIFY(window.findChild<QLabel *>("gameAttributeGain_haste")->isVisible());
        window.findChild<QSpinBox *>("iterations")->setValue(8);
        window.findChild<QDoubleSpinBox *>("seconds")->setValue(10);
        const auto *calculate = window.findChild<QAction *>("attributeGains");
        window.findChild<QToolButton *>("calculateInlineGains")->click();
        window.findChild<QToolButton *>("startButton")->click();
        QTRY_VERIFY_WITH_TIMEOUT(calculate->isEnabled(), 10000);
        QVERIFY(window.findChild<QLabel *>("gameAttributeGain_haste")->text() != QStringLiteral("--"));
        QTest::qWait(320);
        QVERIFY(window.findChild<QLabel *>("gameAttributeGain_haste")->isVisible());
        CAPTURE_PAIR(QStringLiteral("-attribute-gains.png"));
        window.findChild<QToolButton *>("openEquipment")->click();
        QVERIFY(window.close());
        QVERIFY(!window.findChild<QDialog *>("equipmentDialog")->isVisible());
    }

    void permanentPanelPreview()
    {
        Workbench window;
        window.show();
        auto *options = window.findChild<SimulationOptions *>();
        auto *school  = options->findChild<QComboBox *>("specialization");
        for (auto code : { "tx", "mw", "tlgd" }) {
            school->setCurrentIndex(school->findData(code));
            auto  spec  = options->Specialization();
            auto *combo = options->findChild<QComboBox *>(
                "permanent/" +
                QString(code == std::string_view("tx")   ? "外功"
                        : code == std::string_view("mw") ? "内功"
                                                         : "天罗诡道") +
                "/食品增强");
            QVERIFY(combo && combo->isVisible());
            int crit = 0;
            for (int i = 1; i < combo->count(); ++i) {
                if (combo->itemData(i).toJsonObject().contains("crit")) {
                    crit = i;
                    break;
                }
            }
            QVERIFY(crit);
            combo->setCurrentIndex(crit);
            const auto baseCrit = desktop::School(spec).default_attributes.crit;
            QCOMPARE(options->Config().attributes.crit, baseCrit + combo->itemData(crit).toJsonObject()["crit"].toInt());
            auto *mode = options->findChild<QTabBar *>("permanentsInPanel");
            mode->setCurrentIndex(1);
            QCoreApplication::processEvents();
            const auto baseText = QString::number(100. * baseCrit / (9.609 * 990), 'f', 2) + "%";
            QCOMPARE(window.findChild<QLabel *>("gameAttributeValue_critical")->text(), baseText);
            mode->setCurrentIndex(0);
            QCoreApplication::processEvents();
            QVERIFY(window.findChild<QLabel *>("gameAttributeValue_critical")->text() != baseText);
        }
    }

    void schoolPresetsFollowSelection()
    {
        SimulationOptions options;
        auto *school = options.findChild<QComboBox *>("specialization");
        auto *attack = options.findChild<QSpinBox *>("attackBase");
        for (int i = 0; i < school->count(); ++i) {
            school->setCurrentIndex(i);
            const auto cfg = options.Config();
            const auto &book = *cfg.book;
            QCOMPARE(cfg.attributes.attack_base, book.default_attributes.attack_base);
            QCOMPARE(cfg.attributes.attack_by_class, book.default_attributes.attack_by_class);
            QCOMPARE(cfg.attributes.crit, book.default_attributes.crit);
            QCOMPARE(cfg.attributes.strain, book.default_attributes.strain);
            QVERIFY(std::ranges::is_permutation(cfg.recipes, book.default_recipes));
            QVERIFY(std::ranges::is_permutation(cfg.talents, book.default_talents));
            QVERIFY(std::ranges::is_permutation(cfg.equipment, book.default_equipment));
        }
        school->setCurrentIndex(school->findData("mw"));
        attack->setValue(12345);
        const auto saved = options.Snapshot();
        school->setCurrentIndex(school->findData("tx"));
        QCOMPARE(attack->value(), 8784);
        QString error;
        QVERIFY2(options.RestoreSnapshot(saved, error), qPrintable(error));
        QCOMPARE(attack->value(), 12345);
        school->setCurrentIndex(school->findData("tx"));
        school->setCurrentIndex(school->findData("mw"));
        QCOMPARE(attack->value(), 12345);
    }

    void attributeGainComparisonModes()
    {
        Workbench window;
        window.resize(1120, 820);
        window.show();
        window.findChild<QSpinBox *>("iterations")->setValue(4);
        window.findChild<QDoubleSpinBox *>("seconds")->setValue(10);
        QTest::qWait(100);
        auto       *options   = window.findChild<SimulationOptions *>();
        auto       *modes     = window.findChild<QTabBar *>("attributeGainMode");
        const auto *gain      = window.findChild<QLabel *>("gameAttributeGain_attack");
        const auto *action    = window.findChild<QAction *>("attributeGains");
        auto       *calculate = window.findChild<QToolButton *>("calculateInlineGains");
        QCOMPARE(modes->count(), 2);
        QCOMPARE(modes->tabText(0), QStringLiteral("同数值"));
        QCOMPARE(modes->tabText(1), QStringLiteral("同费用"));
        QVERIFY(modes->tabToolTip(1).contains(QStringLiteral("同属性费用")));
        QVERIFY(modes->visibleRegion().contains(modes->rect()));
        for (int i = 0; i < modes->count(); ++i) {
            QVERIFY(modes->tabRect(i).width() - 18 >= QFontMetrics(modes->font()).horizontalAdvance(modes->tabText(i)));
        }
        const auto snapshot = options->Snapshot();
        const auto select   = [&](int index) {
            QTest::mouseClick(modes, Qt::LeftButton, Qt::NoModifier, modes->tabRect(index).center());
            QCOMPARE(modes->currentIndex(), index);
            QCOMPARE(options->Snapshot(), snapshot);
        };
        const auto CAPTURE = [&](const QString &suffix) {
            if (!qEnvironmentVariableIsSet("JX3DPS_TEST_SCREENSHOT")) {
                return;
            }
            QTest::qWait(desktop::motion::SlideMs + 40);
            const auto *attributes = window.findChild<QWidget *>("gameAttributePanel");
            const auto *gains      = window.findChild<QWidget *>("attributeGainsModule");
            const QRect left(attributes->mapTo(&window, QPoint()), attributes->size());
            const QRect right(gains->mapTo(&window, QPoint()), gains->size());
            QVERIFY(window.grab(left.united(right)).save(qEnvironmentVariable("JX3DPS_TEST_SCREENSHOT") + suffix));
        };
        select(1);
        QCOMPARE(options->GainMode(), desktop::GainComparisonMode::SAME_COST);
        QCOMPARE(gain->text(), QStringLiteral("--"));
        calculate->click();
        QVERIFY(calculate->isChecked());
        QVERIFY(action->isChecked());
        QVERIFY(action->isEnabled());
        QVERIFY(!window.findChild<BatchWorker *>());
        QCOMPARE(window.findChild<QTableWidget *>("resultHistory")->rowCount(), 0);
        QCOMPARE(options->Snapshot(), snapshot);
        window.findChild<QToolButton *>("startButton")->click();
        QVERIFY(!action->isEnabled());
        QVERIFY(!calculate->isEnabled());
        select(0); // A finishing job must not replace the newly selected comparison mode.
        QTRY_VERIFY_WITH_TIMEOUT(action->isEnabled(), 10000);
        QCOMPARE(gain->text(), QStringLiteral("--"));
        select(1);
        QVERIFY(gain->text() != QStringLiteral("--"));
        QVERIFY(gain->toolTip().contains(QStringLiteral("208")));
        const auto COST_VALUE = gain->text();
        CAPTURE(QStringLiteral("-same-cost.png"));
        const QTemporaryDir DIRECTORY;
        QString             error;
        const auto          PATH = DIRECTORY.filePath(QStringLiteral("cost-result.json"));
        QVERIFY(window.findChild<ResultsPanel *>()->ExportSelected(PATH, error));
        QFile report(PATH);
        QVERIFY(report.open(QIODevice::ReadOnly));
        const auto DATA = QJsonDocument::fromJson(report.readAll()).object();
        QCOMPARE(DATA.value("result").toObject().value("gainComparisonMode").toString(), QStringLiteral("same_cost"));
        QCOMPARE(DATA.value("gains").toArray()[0].toObject().value("increment").toInt(), 208);
        QCOMPARE(DATA.value("gains").toArray()[6].toObject().value("increment").toInt(), 208);
        select(0);
        QVERIFY(calculate->isChecked());
        window.findChild<QToolButton *>("startButton")->click();
        QTRY_VERIFY_WITH_TIMEOUT(action->isEnabled(), 10000);
        QVERIFY(gain->text() != QStringLiteral("--"));
        QVERIFY(gain->toolTip().contains(QStringLiteral("增加 100 点")));
        const auto value = gain->text();
        CAPTURE(QStringLiteral("-same-value.png"));
        select(1);
        QCOMPARE(gain->text(), COST_VALUE);
        select(0);
        QCOMPARE(gain->text(), value);
        window.findChild<MacroEditor *>("macroEditor")->appendPlainText(QStringLiteral("\n# 清除两种收益缓存"));
        QCOMPARE(gain->text(), QStringLiteral("--"));
        select(1);
        QCOMPARE(gain->text(), QStringLiteral("--"));
        QCOMPARE(QSettings().value("interface/gainComparisonMode").toInt(), 1);
        const desktop::AttributeGainsPanel RESTORED;
        QCOMPARE(RESTORED.Mode(), desktop::GainComparisonMode::SAME_COST);
        QVERIFY(RESTORED.calculate->isChecked());
    }

    void automaticGainsFollowSimulation()
    {
        Workbench window;
        window.resize(1120, 820);
        window.show();
        const auto *options   = window.findChild<SimulationOptions *>();
        auto       *calculate = window.findChild<QToolButton *>("calculateInlineGains");
        auto       *start     = window.findChild<QToolButton *>("startButton");
        const auto *action    = window.findChild<QAction *>("attributeGains");
        const auto *history   = window.findChild<QTableWidget *>("resultHistory");
        window.findChild<QSpinBox *>("iterations")->setValue(4);
        window.findChild<QSpinBox *>("workers")->setValue(2);
        window.findChild<QDoubleSpinBox *>("seconds")->setValue(5);
        QTest::qWait(100);
        QVERIFY(calculate->isCheckable() && !calculate->isChecked());
        QVERIFY(!options->AutoCalculateGains());
        QTemporaryDir directory;
        const auto    report = [&]() -> QJsonObject {
            QString    error;
            const auto PATH = directory.filePath(QStringLiteral("auto-gains.json"));
            if (!window.findChild<ResultsPanel *>()->ExportSelected(PATH, error)) {
                return {};
            }
            QFile file(PATH);
            if (!file.open(QIODevice::ReadOnly)) {
                return {};
            }
            return QJsonDocument::fromJson(file.readAll()).object();
        };
        const auto RUN = [&](auto launch, quint64 requested) {
            launch();
            const auto *worker = window.findChild<BatchWorker *>();
            QVERIFY(worker);
            QCOMPARE(worker->Requested(), requested);
            QVERIFY(!calculate->isEnabled());
            QTRY_VERIFY_WITH_TIMEOUT(start->isEnabled(), 10000);
            QTRY_VERIFY(window.findChildren<BatchWorker *>().isEmpty());
        };
        RUN([&] { start->click(); }, 4);
        auto data = report();
        QVERIFY(!data.isEmpty());
        QVERIFY(!data.value("result").toObject().value("gainsRequested").toBool());
        QVERIFY(data.value("gains").toArray().isEmpty());
        const auto snapshot = options->Snapshot();
        const int  count    = history->rowCount();
        calculate->click();
        QTest::qWait(200);
        QVERIFY(calculate->isChecked() && action->isChecked());
        QCOMPARE(history->rowCount(), count);
        QVERIFY(window.findChildren<BatchWorker *>().isEmpty());
        QCOMPARE(options->Snapshot(), snapshot);
        const auto EXPECTED = desktop::theme::Blend(QColor(desktop::theme::Surface), desktop::theme::AccentColor(calculate), .16);
        QVERIFY(calculate->isEnabled());
        QColor     checked_fill;
        const auto CHECKED_FILL_SETTLED = [&] {
            const auto IMAGE  = calculate->grab().toImage();
            const auto ACTUAL = checked_fill =
                IMAGE.pixelColor(qRound(3 * IMAGE.devicePixelRatio()), qRound(13 * IMAGE.devicePixelRatio()));
            return std::abs(ACTUAL.red() - EXPECTED.red()) <= 1 && std::abs(ACTUAL.green() - EXPECTED.green()) <= 1 &&
                   std::abs(ACTUAL.blue() - EXPECTED.blue()) <= 1;
        };
        QTRY_VERIFY2_WITH_TIMEOUT(
            CHECKED_FILL_SETTLED(),
            qPrintable(QStringLiteral("checked fill %1, expected %2").arg(checked_fill.name(), EXPECTED.name())),
            2000);
        RUN([&] { start->click(); }, 12);
        data = report();
        QVERIFY(data.value("result").toObject().value("gainsRequested").toBool());
        QCOMPARE(data.value("gains").toArray().size(), 8);
        QCOMPARE(data.value("gains").toArray()[0].toObject().value("increment").toInt(), 100);

        window.findChild<QTabBar *>("attributeGainMode")->setCurrentIndex(1);
        window.findChild<QPushButton *>("nav_macro")->click();
        QCoreApplication::processEvents();
        RUN([&] { window.findChild<QToolButton *>("runMacroButton")->click(); }, 12);
        data = report();
        QCOMPARE(data.value("result").toObject().value("gainComparisonMode").toString(), QStringLiteral("same_cost"));
        QCOMPARE(data.value("gains").toArray()[0].toObject().value("increment").toInt(), 208);
        RUN([&] { window.findChild<QAction *>("singleTrace")->trigger(); }, 1);
        data = report();
        QVERIFY(data.value("result").toObject().value("trace").toBool());
        QVERIFY(!data.value("result").toObject().value("gainsRequested").toBool());
        QVERIFY(calculate->isChecked());

        window.findChild<QToolButton *>("debugButton")->setChecked(true);
        window.findChild<QAction *>("debugStart")->trigger();
        QTRY_COMPARE(window.findChild<DebugSession *>()->GetState(), DebugSession::PAUSED);
        QVERIFY(window.findChildren<BatchWorker *>().isEmpty());
        QVERIFY(calculate->isChecked() && !calculate->isEnabled());
        window.findChild<QAction *>("debugStop")->trigger();
        window.findChild<QToolButton *>("debugButton")->setChecked(false);
        window.findChild<QSpinBox *>("iterations")->setValue(1000000);
        start->click();
        QVERIFY(window.findChild<BatchWorker *>());
        QCOMPARE(window.findChild<BatchWorker *>()->Requested(), quint64(3000000));
        window.findChild<QPushButton *>("cancelBatch")->click();
        QTRY_VERIFY_WITH_TIMEOUT(start->isEnabled(), 10000);
        QVERIFY(calculate->isChecked());
        data = report();
        QVERIFY(!data.value("result").toObject().value("complete").toBool());
        QVERIFY(data.value("result").toObject().value("gainsRequested").toBool());
        QVERIFY(QSettings().value("interface/autoCalculateGains").toBool());
        calculate->click();
        QVERIFY(!action->isChecked() && !options->AutoCalculateGains());
    }

    void magpiePressAndReleaseMotion()
    {
        QCOMPARE(desktop::motion::SlideMs, 280);
        QCOMPARE(desktop::motion::TextMs, 180);
        QCOMPARE(desktop::motion::PressMs, 140);
        desktop::SegmentedTabs tabs;
        for (const auto *name : { "Attributes", "Equipment", "Gains" }) {
            tabs.addTab(new QWidget, QString::fromLatin1(name));
        }
        tabs.resize(460, 240);
        tabs.show();
        QCoreApplication::processEvents();
        auto      *bar   = static_cast<desktop::SegmentedTabBar *>(tabs.tabBar());
        const auto first = bar->IndicatorRect();
        QTest::mousePress(bar, Qt::LeftButton, Qt::NoModifier, bar->tabRect(1).center());
        QTest::qWait(70);
        QCOMPARE(tabs.currentIndex(), 0);
        QCOMPARE(bar->IndicatorRect(), first);
        QVERIFY(bar->PressScale(1) < 1 && bar->PressScale(1) >= .96);
        QTest::mouseRelease(bar, Qt::LeftButton, Qt::NoModifier, QPoint(-5, -5));
        QCOMPARE(tabs.currentIndex(), 0);
        QTest::mouseClick(bar, Qt::LeftButton, Qt::NoModifier, bar->tabRect(1).center());
        QCOMPARE(tabs.currentIndex(), 1);
        QVERIFY(bar->SelectionAnimating());
        QVERIFY(!bar->KeyboardFocusVisible());
        QTest::qWait(90);
        const auto MID = bar->IndicatorRect();
        QVERIFY(MID.left() > first.left());
        tabs.setCurrentIndex(2);
        QVERIFY(std::abs(bar->IndicatorRect().left() - MID.left()) < 2);
        QTRY_VERIFY(!bar->SelectionAnimating());
        QTest::keyClick(bar, Qt::Key_Left);
        QVERIFY(bar->KeyboardFocusVisible());

        Workbench window;
        window.show();
        QTest::qWait(100);
        const auto *nav    = static_cast<desktop::NavigationStrip *>(window.findChild<QWidget *>("centeredNavigation"));
        auto       *button = static_cast<desktop::NavigationButton *>(window.findChild<QPushButton *>("nav_macro"));
        const auto  ORIGIN = nav->IndicatorRect();
        QTest::mousePress(button, Qt::LeftButton);
        QTest::qWait(70);
        QVERIFY(button->PressScale() < 1);
        QVERIFY(window.findChild<QPushButton *>("nav_simulation")->isChecked());
        QTest::mouseRelease(button, Qt::LeftButton);
        QVERIFY(button->isChecked());
        QVERIFY(nav->SelectionAnimating());
        QVERIFY(!button->KeyboardFocusVisible());
        QTest::qWait(90);
        QVERIFY(nav->IndicatorRect().left() > ORIGIN.left());
        QTRY_VERIFY(!nav->SelectionAnimating());
    }

    void gainsStayBoundToCurrentInputs()
    {
        Workbench window;
        window.show();
        auto *editor = window.findChild<MacroEditor *>("macroEditor");
        window.findChild<QSpinBox *>("iterations")->setValue(100);
        window.findChild<QDoubleSpinBox *>("seconds")->setValue(30);
        auto *calculate = window.findChild<QAction *>("attributeGains");
        calculate->trigger();
        window.findChild<QToolButton *>("startButton")->click();
        editor->appendPlainText(QStringLiteral("# 已编辑，不把上次运行的收益标成当前结果"));
        QTRY_VERIFY_WITH_TIMEOUT(calculate->isEnabled(), 10000);
        QCOMPARE(window.findChild<QLabel *>("gameAttributeGain_attack")->text(), QStringLiteral("--"));
        QCOMPARE(window.findChild<QTableWidget *>("attributeGainsTable")->rowCount(), 10);
        QVERIFY(editor->toPlainText().contains(QStringLiteral("已编辑")));
    }

    void completeConfigurationAndCenteredNavigation()
    {
        Workbench window;
        window.show();
        const auto check_module_alignment = [&] {
            auto       *attributes  = window.findChild<QWidget *>("gameAttributePanel");
            auto       *gains       = window.findChild<QWidget *>("attributeGainsModule");
            const auto *modes       = gains->findChild<QTabBar *>("attributeGainMode");
            const auto *gain_header = gains->findChild<QWidget *>("attributeModuleHeader");
            const auto *calculate   = gains->findChild<QToolButton *>("calculateInlineGains");
            const auto *actions     = gains->findChild<QWidget *>("attributeModuleActions");
            const auto *talents     = window.findChild<QTabWidget *>("configurationTabs");
            const auto *flow        = window.findChild<QGroupBox *>("flowMacroPanel");
            const auto *tabs        = talents->tabBar();
            QCOMPARE(modes->height(), desktop::AttributeActionHeight);
            QCOMPARE(modes->height(), calculate->height());
            QVERIFY(actions->isAncestorOf(modes) && actions->isAncestorOf(calculate));
            QCOMPARE(modes->mapTo(&window, QPoint()).y(), calculate->mapTo(&window, QPoint()).y());
            QVERIFY(modes->mapTo(&window, modes->rect().topRight()).x() < calculate->mapTo(&window, QPoint()).x());
            QCOMPARE(tabs->height(), gain_header->height());
            const auto TALENT_BODY_TOP = talents->currentWidget()->mapTo(&window, QPoint()).y();
            const auto GAIN_BODY_TOP = gains->findChild<QWidget *>("attributeModuleBody")->mapTo(&window, QPoint()).y();
            const auto GAIN_GAP = GAIN_BODY_TOP - gain_header->mapTo(&window, QPoint(0, gain_header->height())).y();
            QCOMPARE(GAIN_GAP, desktop::AttributeHeaderSpacing);
            QCOMPARE(TALENT_BODY_TOP - tabs->mapTo(&window, QPoint(0, tabs->height())).y(), desktop::AttributeHeaderSpacing);
            QCOMPARE(flow->mapTo(&window, QPoint()).y(), gains->mapTo(&window, QPoint()).y());
            QCOMPARE(flow->height(), gains->height());
            QCOMPARE(talents->height(), 100);
            QCOMPARE(talents->mapTo(&window, QPoint()).x(), attributes->mapTo(&window, QPoint()).x());
            QCOMPARE(talents->mapTo(&window, talents->rect().topRight()).x(),
                     flow->mapTo(&window, flow->rect().topRight()).x());
            auto *permanents = window.findChild<QGroupBox *>("permanentPanel");
            if (permanents->isVisible()) {
                const auto FRAME_TOP = [&](QGroupBox *box) {
                    QStyleOptionGroupBox option;
                    option.initFrom(box);
                    option.text        = box->title();
                    option.subControls = QStyle::SC_GroupBoxFrame | QStyle::SC_GroupBoxLabel;
                    const auto FRAME = box->style()->subControlRect(QStyle::CC_GroupBox, &option, QStyle::SC_GroupBoxFrame, box);
                    return box->mapTo(&window, FRAME.topLeft()).y();
                };
                const auto *permanent_mode = permanents->findChild<QTabBar *>("permanentsInPanel");
                QCOMPARE(FRAME_TOP(permanents) - permanent_mode->mapTo(&window, QPoint(0, permanent_mode->height())).y(), GAIN_GAP);
                QCOMPARE(FRAME_TOP(window.findChild<QGroupBox *>("runSettings")), FRAME_TOP(permanents));
            }
            for (const auto *panel : { attributes, gains }) {
                const auto *body = panel->findChild<QWidget *>("attributeModuleBody");
                QCOMPARE(body->mapTo(&window, QPoint()).y(), GAIN_BODY_TOP);
                QCOMPARE(panel->height(), gains->height());
                QCOMPARE(panel->mapTo(&window, panel->rect().bottomLeft()).y(),
                         gains->mapTo(&window, gains->rect().bottomLeft()).y());
                QVERIFY(talents->mapTo(&window, QPoint()).y() >= panel->mapTo(&window, panel->rect().bottomLeft()).y() + 4);
            }
        };
        for (const QSize SIZE : { QSize(1206, 860), QSize(1280, 840), QSize(1120, 820) }) {
            window.resize(SIZE);
            window.findChild<QPushButton *>("nav_simulation")->click();
            for (const int specialization : { 0, 1 }) {
                window.findChild<QComboBox *>("specialization")->setCurrentIndex(specialization);
                QTest::qWait(100);
                QCOMPARE(window.size(), SIZE);
                check_module_alignment();
                const auto *navigation = window.findChild<QWidget *>("topNavigation");
                const auto *items      = window.findChild<QWidget *>("centeredNavigation");
                const qreal CENTER     = items->mapTo(navigation, QPoint()).x() + (items->width() / 2.0);
                QVERIFY(std::abs(CENTER - navigation->width() / 2.0) <= 1);
                if (qEnvironmentVariableIsSet("JX3DPS_TEST_SCREENSHOT")) {
                    QVERIFY(window.grab().save(
                        qEnvironmentVariable("JX3DPS_TEST_SCREENSHOT") +
                        QStringLiteral("-complete-%1x%2-class%3.png").arg(SIZE.width()).arg(SIZE.height()).arg(specialization)));
                    QVERIFY(window.findChild<QWidget *>("simulationSettingsRow")
                                ->grab()
                                .save(qEnvironmentVariable("JX3DPS_TEST_SCREENSHOT") +
                                      QStringLiteral("-upper-%1x%2-class%3.png").arg(SIZE.width()).arg(SIZE.height()).arg(specialization)));
                    const auto *attributes = window.findChild<QWidget *>("gameAttributePanel");
                    const auto *talents    = window.findChild<QWidget *>("configurationTabs");
                    const QRect left(attributes->mapTo(&window, QPoint()), attributes->size());
                    const QRect right(talents->mapTo(&window, QPoint()), talents->size());
                    QVERIFY(window.grab(left.united(right))
                                .save(qEnvironmentVariable("JX3DPS_TEST_SCREENSHOT") +
                                      QStringLiteral("-modules-%1x%2-class%3.png").arg(SIZE.width()).arg(SIZE.height()).arg(specialization)));
                }
                for (const auto &name : { QStringLiteral("attributesScroll"),
                                          QStringLiteral("talentsScroll%1").arg(specialization),
                                          QStringLiteral("configurationScroll") })
                {
                    const auto *scroll = window.findChild<QScrollArea *>(name);
                    QVERIFY(scroll);
                    QVERIFY2(scroll->verticalScrollBar()->maximum() == 0,
                             qPrintable(QStringLiteral("%1 at %2x%3: viewport %4x%5, content %6x%7, vertical range %8")
                                            .arg(name)
                                            .arg(SIZE.width())
                                            .arg(SIZE.height())
                                            .arg(scroll->viewport()->width())
                                            .arg(scroll->viewport()->height())
                                            .arg(scroll->widget()->width())
                                            .arg(scroll->widget()->height())
                                            .arg(scroll->verticalScrollBar()->maximum())));
                    QCOMPARE(scroll->horizontalScrollBar()->maximum(), 0);
                }
                for (const auto &row : desktop::GameAttributeRows) {
                    const auto *value =
                        window.findChild<QLabel *>(QStringLiteral("gameAttributeValue_%1").arg(QString::fromLatin1(row.key)));
                    QVERIFY2(value->isVisible() && value->visibleRegion().contains(value->rect()), row.key);
                    const auto *gain_row =
                        window.findChild<QWidget *>(QStringLiteral("gameAttributeGainRow_%1").arg(QString::fromLatin1(row.key)));
                    QVERIFY2(gain_row->isVisible() && gain_row->visibleRegion().contains(gain_row->rect()), row.key);
                    QCOMPARE(value->mapTo(&window, value->rect().center()).y(),
                             gain_row->mapTo(&window, gain_row->rect().center()).y());
                }
                int talent_y   = -1;
                int previous_x = -1;
                for (int tier = 1; tier <= 9; ++tier) {
                    const auto *button = window.findChild<QWidget *>(QStringLiteral("talents%1").arg(specialization))
                                             ->findChild<QToolButton *>(QStringLiteral("talentTier%1").arg(tier));
                    QVERIFY2(button->isVisible() && button->visibleRegion().contains(button->rect()),
                             qPrintable(button->objectName()));
                    const auto POSITION = button->mapTo(&window, QPoint());
                    if (talent_y >= 0) {
                        QCOMPARE(POSITION.y(), talent_y);
                    }
                    QVERIFY(POSITION.x() > previous_x);
                    talent_y   = POSITION.y();
                    previous_x = POSITION.x();
                }
                const auto *run = window.findChild<QWidget *>("runSettings");
                QVERIFY(run->findChildren<QScrollArea *>().isEmpty());
                QVERIFY(!window.findChild<QTabWidget *>("runTabs"));
                for (const auto *key : { "iterations", "workers", "seconds", "seed", "delay" }) {
                    const auto *input = window.findChild<QWidget *>(QString::fromLatin1(key));
                    QVERIFY2(input->visibleRegion().contains(input->rect()), key);
                }
                for (const auto *page : { "attributes", "talents" }) {
                    window.findChild<QPushButton *>(QStringLiteral("nav_%1").arg(QString::fromLatin1(page)))->click();
                    QCoreApplication::processEvents();
                    const auto  NAME   = QByteArray(page) == "attributes"
                                             ? QStringLiteral("attributesScroll")
                                             : QStringLiteral("talentsScroll%1").arg(specialization);
                    const auto *scroll = window.findChild<QScrollArea *>(NAME);
                    QCOMPARE(scroll->verticalScrollBar()->maximum(), 0);
                    QCOMPARE(scroll->horizontalScrollBar()->maximum(), 0);
                    if (QByteArray(page) == "attributes") {
                        check_module_alignment();
                        const auto *attributes = window.findChild<QWidget *>("gameAttributePanel");
                        const auto *gains      = window.findChild<QWidget *>("attributeGainsModule");
                        QVERIFY(std::abs(attributes->width() - gains->width()) <= 1);
                        QVERIFY(attributes->height() <= 270 && gains->height() <= 270);
                        const auto *modes = gains->findChild<QTabBar *>("attributeGainMode");
                        QVERIFY(modes->visibleRegion().contains(modes->rect()));
                        for (int i = 0; i < modes->count(); ++i) {
                            QVERIFY(modes->tabRect(i).width() - 18 >=
                                    QFontMetrics(modes->font()).horizontalAdvance(modes->tabText(i)));
                        }
                        const auto *import    = window.findChild<QPushButton *>("importAttributes");
                        const auto *calculate = window.findChild<QToolButton *>("calculateInlineGains");
                        QCOMPARE(import->mapTo(&window, QPoint()).y(), calculate->mapTo(&window, QPoint()).y());
                        for (const auto &row : desktop::GameAttributeRows) {
                            const auto  KEY   = QString::fromLatin1(row.key);
                            const auto *value = window.findChild<QLabel *>("gameAttributeValue_" + KEY);
                            const auto *gain  = window.findChild<QWidget *>("gameAttributeGainRow_" + KEY);
                            QCOMPARE(value->mapTo(&window, value->rect().center()).y(),
                                     gain->mapTo(&window, gain->rect().center()).y());
                        }
                        if (qEnvironmentVariableIsSet("JX3DPS_TEST_SCREENSHOT")) {
                            QTest::qWait(desktop::motion::SlideMs + 40);
                            QVERIFY(window.grab().save(
                                qEnvironmentVariable("JX3DPS_TEST_SCREENSHOT") +
                                QStringLiteral("-role-%1x%2-class%3.png").arg(SIZE.width()).arg(SIZE.height()).arg(specialization)));
                        }
                    }
                }
                window.findChild<QPushButton *>("nav_simulation")->click();
            }
        }
    }

    void compactSimulationSettings()
    {
        Workbench window;
        window.show();
        auto       *options    = window.findChild<SimulationOptions *>();
        auto       *run        = window.findChild<QWidget *>("runSettings");
        const auto *content    = window.findChild<QWidget *>("runParameters");
        const auto *form       = qobject_cast<QFormLayout *>(content->layout());
        const auto *permanents = window.findChild<QWidget *>("permanentPanel");
        auto       *upper      = window.findChild<QWidget *>("simulationSettingsRow");
        auto       *seed       = window.findChild<QLineEdit *>("seed");
        const auto  MAX_SEED   = QStringLiteral("18446744073709551615");
        seed->setText(MAX_SEED);
        for (const auto *key : { "iterations", "workers", "delay" }) {
            auto *input = window.findChild<QSpinBox *>(QString::fromLatin1(key));
            input->setValue(input->maximum());
        }
        auto *seconds = window.findChild<QDoubleSpinBox *>("seconds");
        seconds->setValue(seconds->maximum());
        QVERIFY(!window.findChild<QTabWidget *>("runTabs"));
        QVERIFY(run->findChildren<QScrollArea *>().isEmpty());
        QCOMPARE(form->rowCount(), 5);
        for (const QSize SIZE : { QSize(1206, 860), QSize(1280, 840), QSize(1120, 820) }) {
            window.resize(SIZE);
            for (const int specialization : { 0, 1 }) {
                window.findChild<QComboBox *>("specialization")->setCurrentIndex(specialization);
                QTest::qWait(100);
                const auto snapshot = options->Snapshot();
                QCOMPARE(window.size(), SIZE);
                QCOMPARE(run->width(), 224);
                QCOMPARE(run->parentWidget(), upper);
                QCOMPARE(permanents->parentWidget(), upper);
                QCOMPARE(run->x(), 0);
                QCOMPARE(permanents->x() - run->geometry().right() - 1, 4);
                QCOMPARE(permanents->geometry().right(), upper->rect().right());
                QCOMPARE(run->height(), permanents->height());
                QRect pairs;
                QRect labels;
                int   field_left = -1;
                for (int row = 0; row < form->rowCount(); ++row) {
                    const auto *label = form->itemAt(row, QFormLayout::LabelRole);
                    const auto *field = form->itemAt(row, QFormLayout::FieldRole);
                    QVERIFY(label && field);
                    QVERIFY(field->geometry().width() >= 128 && field->geometry().width() <= 136);
                    if (field_left >= 0) {
                        QCOMPARE(field->geometry().left(), field_left);
                    }
                    field_left = field->geometry().left();
                    QVERIFY(label->geometry().width() >= label->sizeHint().width());
                    QCOMPARE(field->widget()->font().pixelSize(), 13);
                    labels = labels.united(label->geometry());
                    pairs  = pairs.united(label->geometry()).united(field->geometry());
                    QVERIFY(field->widget()->visibleRegion().contains(field->widget()->rect()));
                    QVERIFY(run->rect().contains(QRect(field->widget()->mapTo(run, QPoint()), field->widget()->size())));
                }
                QCOMPARE(field_left - labels.right() - 1, 6);
                QVERIFY(qAbs(pairs.center().x() - content->rect().center().x()) <= 1);
                QVERIFY(pairs.left() >= 4 && pairs.left() <= 8);
                QCOMPARE(options->Snapshot(), snapshot);
                QSet<int> columns;
                QSet<int> rows;
                int       permanent_count = 0;
                for (const auto *choice : options->findChildren<QComboBox *>()) {
                    if (!choice->objectName().startsWith(
                            QStringLiteral("permanent/%1/").arg(specialization ? QStringLiteral("内功") : QStringLiteral("外功"))))
                    {
                        continue;
                    }
                    ++permanent_count;
                    QCOMPARE(choice->size(), QSize(38, 38));
                    QVERIFY(choice->visibleRegion().contains(choice->rect()));
                    const auto *caption =
                        options->findChild<QLabel *>(choice->objectName() + QStringLiteral("Caption"));
                    QVERIFY(caption && caption->visibleRegion().contains(caption->rect()));
                    QVERIFY(caption->width() >= caption->fontMetrics().horizontalAdvance(caption->text()));
                    QCOMPARE(caption->font().pixelSize(), 11);
                    const auto ICON_POSITION = choice->mapTo(permanents, QPoint());
                    const auto TEXT_POSITION = caption->mapTo(permanents, QPoint());
                    QCOMPARE(TEXT_POSITION.x() - ICON_POSITION.x() - choice->width(), 6);
                    columns.insert(ICON_POSITION.x());
                    rows.insert(ICON_POSITION.y());
                }
                QCOMPARE(permanent_count, 10);
                QCOMPARE(columns.size(), 3);
                QCOMPARE(rows.size(), 4);
                const auto *permanent_scroll = options->findChild<QScrollArea *>("permanentsScroll");
                QCOMPARE(permanent_scroll->horizontalScrollBar()->maximum(), 0);
                QCOMPARE(permanent_scroll->verticalScrollBar()->maximum(), 0);
                QCOMPARE(options->Batch().seed, std::numeric_limits<std::uint64_t>::max());
                QVERIFY(seed->toolTip().contains(MAX_SEED));
                seed->setFocus();
                QTest::keyClick(seed, Qt::Key_Home);
                QCOMPARE(seed->cursorPosition(), 0);
                QTest::keyClick(seed, Qt::Key_End);
                QCOMPARE(seed->cursorPosition(), MAX_SEED.size());
                QCOMPARE(seed->text(), MAX_SEED);
                if (qEnvironmentVariableIsSet("JX3DPS_TEST_SCREENSHOT")) {
                    QVERIFY(run->grab().save(
                        qEnvironmentVariable("JX3DPS_TEST_SCREENSHOT") +
                        QStringLiteral("-settings-%1x%2-class%3.png").arg(SIZE.width()).arg(SIZE.height()).arg(specialization)));
                }
                window.findChild<QPushButton *>("nav_permanents")->click();
                QCoreApplication::processEvents();
                QVERIFY(permanents->isVisible());
                QCOMPARE(permanents->parentWidget(), options);
                QVERIFY(!upper->isVisible());
                window.findChild<QPushButton *>("nav_simulation")->click();
                QCoreApplication::processEvents();
                QCOMPARE(permanents->parentWidget(), upper);
                QCOMPARE(run->width(), 224);
                QCOMPARE(options->Snapshot(), snapshot);
            }
        }
    }

    void simulationSecondsUseMutedUnit()
    {
        Workbench window;
        window.resize(1120, 820);
        window.show();
        auto       *seconds = window.findChild<QDoubleSpinBox *>("seconds");
        auto       *edit    = seconds->findChild<QLineEdit *>();
        const auto *unit    = seconds->findChild<QLabel *>("spinBoxUnit");
        auto       *seed    = window.findChild<QLineEdit *>("seed");
        QVERIFY(edit && unit);
        QVERIFY(seconds->suffix().isEmpty());
        QCOMPARE(unit->text(), QStringLiteral("秒"));
        QCOMPARE(unit->focusPolicy(), Qt::NoFocus);
        QCOMPARE(unit->textInteractionFlags(), Qt::NoTextInteraction);
        QVERIFY(unit->testAttribute(Qt::WA_TransparentForMouseEvents));
        QVERIFY(seconds->accessibleDescription().contains(QStringLiteral("秒")));
        QCOMPARE(seconds->decimals(), 4);
        QCOMPARE(seconds->singleStep(), 0.0625);
        const auto COUNT_COLOR = [](const QImage &image, const QColor &color) {
            int count = 0;
            for (int y = 0; y < image.height(); ++y) {
                for (int x = 0; x < image.width(); ++x) {
                    const auto PIXEL = image.pixelColor(x, y);
                    if (qAbs(PIXEL.red() - color.red()) <= 2 && qAbs(PIXEL.green() - color.green()) <= 2 &&
                        qAbs(PIXEL.blue() - color.blue()) <= 2)
                    {
                        ++count;
                    }
                }
            }
            return count;
        };
        const QColor MUTED(desktop::theme::Muted);
        const QColor text(desktop::theme::Text);
        for (const double value : { 0.0625, 300.0 }) {
            seconds->setValue(value);
            seed->setFocus();
            QTest::qWait(40);
            QCOMPARE(seconds->value(), value);
            QVERIFY(!edit->text().contains(QStringLiteral("秒")));
            const auto IMAGE = edit->grab().toImage();
            QVERIFY(COUNT_COLOR(IMAGE, MUTED) > 2);
            QVERIFY(COUNT_COLOR(IMAGE, text) > 2);
            QVERIFY(seconds->visibleRegion().contains(seconds->rect()));
            QVERIFY(unit->visibleRegion().contains(unit->rect()));
            QVERIFY(edit->textMargins().right() >= unit->width() + 6);
        }
        seconds->setValue(86400);
        QCOMPARE(seconds->value(), 86400.0);
        QCOMPARE(edit->text(), QStringLiteral("86400.0000"));
        seconds->setFocus();
        QTest::keyClick(edit, Qt::Key_A, Qt::ControlModifier);
        QCOMPARE(edit->selectedText(), QStringLiteral("86400.0000"));
        QTest::keyClick(edit, Qt::Key_Backspace);
        QVERIFY(edit->text().isEmpty());
        const auto FIXED_UNIT_BOUNDS = unit->geometry();
        QVERIFY(unit->isVisible());
        QVERIFY(COUNT_COLOR(edit->grab().toImage(), MUTED) > 2);
        QTest::keyClicks(edit, "abc");
        QVERIFY(edit->text().isEmpty());
        QInputMethodEvent unit_input;
        unit_input.setCommitString(QStringLiteral("秒"));
        QApplication::sendEvent(edit, &unit_input);
        QVERIFY(edit->text().isEmpty());
        QTest::keyClicks(edit, "12.1875");
        QTest::keyClick(edit, Qt::Key_Return);
        QCOMPARE(seconds->value(), 12.1875);
        seconds->stepUp();
        QCOMPARE(seconds->value(), 12.25);
        QCOMPARE(window.findChild<SimulationOptions *>()->Batch().duration, 196);
        QTest::keyClick(edit, Qt::Key_A, Qt::ControlModifier);
        QCOMPARE(edit->selectedText(), QStringLiteral("12.2500"));
        QVERIFY(COUNT_COLOR(edit->grab().toImage(), MUTED) > 2);
        QVERIFY(!unit->hasSelectedText());
        QTest::keyClick(edit, Qt::Key_Delete);
        QVERIFY(edit->text().isEmpty());
        QCOMPARE(unit->geometry(), FIXED_UNIT_BOUNDS);
        QVERIFY(unit->isVisible());
        QTest::keyClicks(edit, "10.05");
        QTest::keyClick(edit, Qt::Key_Return);
        QCOMPARE(seconds->value(), 10.0625);
        QTest::mouseClick(edit, Qt::LeftButton, {}, unit->geometry().center());
        QVERIFY(!edit->selectedText().contains(QStringLiteral("秒")));
        QCOMPARE(seconds->value(), 10.0625);
        seconds->setEnabled(false);
        QVERIFY(unit->isVisible());
        QVERIFY(COUNT_COLOR(edit->grab().toImage(), MUTED.darker(140)) > 2);
        seconds->setEnabled(true);
        for (const auto *page : { "nav_profiles", "nav_simulation" }) {
            window.findChild<QPushButton *>(QString::fromLatin1(page))->click();
            QCoreApplication::processEvents();
            QCOMPARE(unit->font().pixelSize(), edit->font().pixelSize());
            QVERIFY(unit->visibleRegion().contains(unit->rect()));
            QVERIFY(edit->textMargins().right() >= unit->width() + 6);
        }
        seconds->setValue(300);
        edit->deselect();
        seed->setFocus();
        QTest::qWait(40);
        if (qEnvironmentVariableIsSet("JX3DPS_TEST_SCREENSHOT")) {
            QVERIFY(window.findChild<QWidget *>("runSettings")->grab().save(qEnvironmentVariable("JX3DPS_TEST_SCREENSHOT") + "-seconds-unit.png"));
        }
    }

    void talentFooterExpansion()
    {
        Workbench window;
        window.show();
        auto *options = window.findChild<SimulationOptions *>();
        auto *tabs    = options->findChild<QTabWidget *>("configurationTabs");
        QVERIFY(tabs);
        auto *school = options->findChild<QComboBox *>("specialization");
        for (auto code : { "tx", "mw", "gfj", "wf" }) {
            school->setCurrentIndex(school->findData(code));
            tabs->setCurrentIndex(0);
            QCOMPARE(tabs->height(), 100);
            auto *active = options->findChild<desktop::TalentPanel *>("talents" + QString::number(school->currentIndex()));
            QVERIFY(active);
            QCOMPARE(active->Selection().size(), 9);
            QVERIFY(active->isVisible());
            QVERIFY(!active->HeaderWidget()->findChild<QToolButton *>("legacyTalentToggle"));
            tabs->setCurrentIndex(1);
            QVERIFY(tabs->height() > 100);
        }
    }

    void flowMacroCompiler()
    {
        const auto DEFAULTS = desktop::FlowDefaults();
        const auto SOURCE   = desktop::FlowMacroFromValues(DEFAULTS);
        const auto COMPILED = desktop::CompileFlowMacro(SOURCE);
        QVERIFY(COMPILED.errors.isEmpty());
        QCOMPARE(COMPILED.values, DEFAULTS);
        QCOMPARE(desktop::CompileFlowMacro(QString{}).values, DEFAULTS);
        const auto ORDERED = desktop::CompileFlowMacro(QStringLiteral(
            "# opening\r\n00:00.00 /set_target id=1 level=53 life=25% vulnerable=5%\n"
            "/set_target level=54 distance=3 // last assignment wins\n"
            "/set_player qidian=3"));
        QVERIFY2(ORDERED.errors.isEmpty(), qPrintable(ORDERED.ErrorText()));
        QCOMPARE(ORDERED.values.value("targetLevel").toInt(), 54);
        QCOMPARE(ORDERED.values.value("targetLife").toDouble(), 25.);
        QCOMPARE(ORDERED.values.value("vulnerable").toDouble(), 5.);
        QCOMPARE(ORDERED.values.value("qidian").toInt(), 3);
        QCOMPARE(desktop::CompileFlowMacro("/set_target life=0.25 vulnerable=0.05").values.value("targetLife").toDouble(), 25.);
        for (const auto &field : desktop::FlowFields) {
            for (const double BOUND : { field.minimum, field.maximum }) {
                if (field.combat_only) {
                    const auto hit = desktop::CompileFlowMacro("00:01 /set_player hit=" + desktop::FlowValueText(field, BOUND));
                    QVERIFY(hit.errors.isEmpty());
                    QCOMPARE(hit.TargetEvents().at(0).incoming_damage_basis_points.value(), qRound(BOUND * 100));
                    continue;
                }
                auto values = DEFAULTS;
                values.insert(QString::fromLatin1(field.key), BOUND);
                const auto PARSED = desktop::CompileFlowMacro(desktop::FlowMacroFromValues(values));
                QVERIFY2(PARSED.errors.isEmpty(), qPrintable(PARSED.ErrorText()));
                QCOMPARE(PARSED.values, values);
            }
        }
        for (const auto &bad :
             { QStringLiteral("/set_target"),
               QStringLiteral("/cast 三柴剑法"),
               QStringLiteral("/set_target typo=1"),
               QStringLiteral("/set_target level=52 level=53"),
               QStringLiteral("/set_target id=2"),
               QStringLiteral("00:01 /set_target level=53"),
               QStringLiteral("/set_target level=52.1"),
               QStringLiteral("/set_target life=0%"),
               QStringLiteral("/set_target life=100"),
               QStringLiteral("/set_target level=NaN"),
               QStringLiteral("/set_target shield=1e999"),
               QStringLiteral("/set_target distance=-1"),
               QStringLiteral("/set_target shield=30%"),
               QStringLiteral("/set_player style=2"),
               QStringLiteral("/set_player shadows=7"),
               QStringLiteral("/set_player qidian=11"),
               QStringLiteral("/set_target life=50.001%"),
               QStringLiteral("/set_target level = 124") })
        {
            const auto PARSED = desktop::CompileFlowMacro("# first line\n  " + bad);
            QVERIFY2(!PARSED.errors.isEmpty(), qPrintable(bad));
            QCOMPARE(PARSED.errors.front().line, 2);
            QVERIFY(PARSED.errors.front().column >= 2);
            QVERIFY(PARSED.errors.front().length > 0);
        }
        QVERIFY(!desktop::CompileFlowMacro(QString(65537, ' ')).errors.isEmpty());
    }

    void flowTimelineCompiler()
    {
        for (const int FRAME : { 0, 1, 8, 15, 16, 161, 959, 960, 1382400 }) {
            int parsed = -1;
            QVERIFY(desktop::ParseFlowTime(desktop::FlowTimeText(FRAME), parsed));
            QCOMPARE(parsed, FRAME);
        }
        for (const auto &time : { "00:00.1", "00:60", "-1:00", "1440:00.0625", "999999999999999999999:00", "00:00.06250" })
        {
            int parsed = 0;
            QVERIFY2(!desktop::ParseFlowTime(time, parsed), time);
        }
        const auto SOURCE = QStringLiteral("# phase test\n/set_target life=100%\n"
                                           "  00:10.0625 /set_target id=1 life=20% distance=3  // retained\n"
                                           "00:05.5 /set_target life=50%\n00:05.5 /set_target life=40%");
        const auto PARSED = desktop::CompileFlowMacro(SOURCE);
        QVERIFY2(PARSED.errors.isEmpty(), qPrintable(PARSED.ErrorText()));
        QCOMPARE(PARSED.values.value("targetLife").toDouble(), 100.);
        const auto EVENTS = PARSED.TargetEvents();
        QCOMPARE(EVENTS.size(), size_t(3));
        QCOMPARE(EVENTS[0].frame, 88);
        QCOMPARE(*EVENTS[0].life_basis_points, 5000);
        QCOMPARE(*EVENTS[1].life_basis_points, 4000);
        QCOMPARE(EVENTS[2].frame, 161);
        QCOMPARE(*EVENTS[2].distance_hundredths, 300);
        const auto &node = PARSED.nodes[1];
        QVERIFY(node.CanMove());
        QCOMPARE(node.WithTime(240), QStringLiteral("  00:15 /set_target id=1 life=20% distance=3  // retained"));
        QCOMPARE(node.WithValues(0,
                                 "/set_target",
                                 {
                                     { "targetLife", 30 }
        }),
                 QStringLiteral("  00:00 /set_target id=1 life=30%  // retained"));
        for (const auto &bad :
             { "/set_player hit=30%", "00:01 /set_player hit=0%", "00:01 /set_player qidian=5", "00:01 /set_target vulnerable=5%", "00:01 /set_target shield=100", "00:01 /set_target id=1", "00:00.1 /set_target life=50%" })
        {
            QVERIFY2(!desktop::CompileFlowMacro(bad).errors.isEmpty(), bad);
        }
        QStringList lines;
        for (int i = 0; i < 256; ++i) {
            lines << "00:01 /set_target distance=3";
        }
        QVERIFY(desktop::CompileFlowMacro(lines.join('\n')).errors.isEmpty());
        lines << "/set_player qidian=5";
        QVERIFY(!desktop::CompileFlowMacro(lines.join('\n')).errors.isEmpty());
    }

    void flowTimelineEditing()
    {
        Workbench window;
        window.resize(1120, 820);
        window.show();
        auto *options = window.findChild<SimulationOptions *>();
        auto *script  = window.findChild<CodeEditor *>("flowMacroEditor");
        auto *editor  = static_cast<desktop::FlowTimelineEditor *>(window.findChild<QWidget *>("flowTimelineEditor"));
        auto *tabs    = window.findChild<QTabBar *>("flowViewMode");
        QVERIFY(editor && tabs);
        const auto SOURCE =
            QStringLiteral("# opening\n/set_target level=54 life=100% distance=4\n"
                           "  00:30 /set_target life=50%  // phase two\n00:45 /set_target distance=3\n");
        script->setPlainText(SOURCE);
        const auto snapshot = options->Snapshot();
        tabs->setCurrentIndex(0);
        tabs->setCurrentIndex(1);
        QCOMPARE(options->Snapshot(), snapshot);
        window.findChild<QToolButton *>("openFlowTimeline")->click();
        auto *dialog = window.findChild<QDialog *>("flowTimelineDialog");
        auto *canvas = static_cast<desktop::FlowTimelineCanvas *>(window.findChild<QWidget *>("flowTimelineCanvas"));
        const auto *table = window.findChild<QTableWidget *>("flowEventTable");
        QTRY_VERIFY(dialog && dialog->isVisible());
        QCOMPARE(table->rowCount(), 3);
        const auto OVERVIEW_POINT = canvas->NodePoint(3, 2);
        canvas->Zoom(2, 0);
        QVERIFY(canvas->NodePoint(3, 2).x() > OVERVIEW_POINT.x());
        canvas->FitAll();
        QCOMPARE(canvas->NodePoint(3, 2), OVERVIEW_POINT);
        QVERIFY(!editor->MoveNode(2, 16));
        QVERIFY(editor->MoveNode(3, 640));
        QVERIFY(script->toPlainText().contains("  00:40 /set_target life=50%  // phase two"));
        script->undo();
        QCOMPARE(options->Snapshot(), snapshot);
        script->redo();
        QCOMPARE(editor->Model().nodes[1].frame, 640);
        script->undo();
        QTest::qWait(60);
        const auto start = canvas->NodePoint(3, 2);
        QVERIFY(start.x() >= canvas->PlotRect().left());
        const auto FINISH   = start + QPoint(60, 0);
        const auto EXPECTED = canvas->FrameAt(FINISH.x());
        QTest::mousePress(canvas, Qt::LeftButton, Qt::NoModifier, start);
        QTest::mouseMove(canvas, FINISH, 30);
        QTest::mouseRelease(canvas, Qt::LeftButton, Qt::NoModifier, FINISH);
        QCOMPARE(editor->Model().nodes[1].frame, EXPECTED);
        QTest::keyClick(canvas, Qt::Key_Right);
        QCOMPARE(editor->Model().nodes[1].frame, EXPECTED + 1);
        QTest::keyClick(canvas, Qt::Key_Delete);
        QCOMPARE(editor->Model().nodes.size(), 2);
        script->undo();
        script->undo();
        script->undo();
        QCOMPARE(options->Snapshot(), snapshot);

        editor->AddNode(80, 2);
        auto *node_dialog = window.findChild<QDialog *>("flowNodeDialog");
        QTRY_VERIFY(node_dialog && node_dialog->isVisible());
        auto *time  = node_dialog->findChild<QDoubleSpinBox *>("flowNodeTime");
        auto *life  = node_dialog->findChild<QDoubleSpinBox *>("flowNodeValue_targetLife");
        auto *apply = node_dialog->findChild<QPushButton *>("applyFlowNode");
        QVERIFY(time && life && apply);
        life->setValue(25);
        time->setValue(5.5);
        QCOMPARE(options->Snapshot(), snapshot); // Proxy inputs must never leak into configuration.
        apply->click();
        QTRY_VERIFY(!node_dialog->isVisible());
        QVERIFY(script->toPlainText().endsWith("00:05.5 /set_target life=25%"));
        QCOMPARE(editor->Model().TargetEvents().front().frame, 88);
        script->undo();
        QCOMPARE(options->Snapshot(), snapshot);
        editor->EditNode(2);
        QTRY_VERIFY(node_dialog->isVisible());
        QVERIFY(!time->isEnabled());
        node_dialog->reject();
        QCOMPARE(options->Snapshot(), snapshot);
        editor->EditNode(3);
        life->setValue(35);
        node_dialog->reject();
        QCOMPARE(options->Snapshot(), snapshot);
        editor->EditNode(3);
        life->setValue(35);
        apply->click();
        QVERIFY(script->toPlainText().contains("  00:30 /set_target life=35%  // phase two"));
        script->undo();
        QCOMPARE(options->Snapshot(), snapshot);

        QTest::mouseDClick(canvas, Qt::LeftButton, Qt::NoModifier, canvas->NodePoint(3, 2));
        QTRY_VERIFY(node_dialog->isVisible());
        QCOMPARE(life->value(), 50.);
        node_dialog->reject();
        const auto EMPTY = canvas->NodePoint(3, 2) + QPoint(120, 0);
        QTest::mouseDClick(canvas, Qt::LeftButton, Qt::NoModifier, EMPTY);
        QTRY_VERIFY(node_dialog->isVisible());
        QCOMPARE(time->value(), canvas->FrameAt(EMPTY.x()) / 16.);
        QCOMPARE(life->value(), 50.);
        node_dialog->reject();
        QCOMPARE(options->Snapshot(), snapshot);

        editor->EditNode(3);
        script->appendPlainText("# concurrent edit");
        const auto CHANGED = script->toPlainText();
        apply->click();
        QVERIFY(node_dialog->isVisible() && !apply->isEnabled());
        QCOMPARE(script->toPlainText(), CHANGED);
        node_dialog->reject();
        script->undo();
        QCOMPARE(options->Snapshot(), snapshot);
        editor->EditNode(3);
        options->SetInputsEnabled(false);
        QVERIFY(!node_dialog->isVisible());
        QVERIFY(!editor->MoveNode(3, 80) && !editor->RemoveNode(3));
        QVERIFY(!window.findChild<QToolButton *>("addFlowNode")->isEnabled());
        options->SetInputsEnabled(true);
        script->appendPlainText("bad instruction");
        QVERIFY(!editor->MoveNode(3, 80));
        QVERIFY(!window.findChild<QToolButton *>("addFlowNode")->isEnabled());
        QTest::keyClick(canvas, Qt::Key_Z, Qt::ControlModifier);
        QCOMPARE(options->Snapshot(), snapshot);
        options->findChild<QDoubleSpinBox *>("seconds")->setValue(30);
        QVERIFY(window.findChild<QLabel *>("flowTimelineNotice")->text().contains(QStringLiteral("2 个超出")));
        QCOMPARE(table->item(1, 0)->foreground().color(), QColor(desktop::theme::Muted));
        options->findChild<QDoubleSpinBox *>("seconds")->setValue(300);
        QCOMPARE(table->item(1, 0)->foreground().color(), QColor(desktop::theme::Text));
        QCOMPARE(options->Snapshot(), snapshot);
        QString error;
        QVERIFY2(options->RestoreSnapshot(snapshot, error), qPrintable(error));
        auto legacy = snapshot;
        legacy.insert("version", 3);
        legacy["specialization"] = 0;
        QVERIFY2(options->RestoreSnapshot(legacy, error), qPrintable(error));
        QCOMPARE(options->Config().target_events.size(), std::size_t(2));
        QVERIFY2(options->RestoreSnapshot(snapshot, error), qPrintable(error));
        dialog->close();
        if (qEnvironmentVariableIsSet("JX3DPS_TEST_SCREENSHOT")) {
            script->setPlainText(desktop::FlowMacroFromValues(desktop::FlowDefaults()) +
                                 QStringLiteral("\n00:30 /set_target life=70%\n01:00 /set_target distance=20\n01:15 "
                                                "/set_target distance=3\n02:00 /set_target life=30%"));
            QTest::qWait(200);
            QVERIFY(window.grab().save(qEnvironmentVariable("JX3DPS_TEST_SCREENSHOT") + "-flow-preview.png"));
            editor->OpenEditor();
            QTest::qWait(100);
            QVERIFY(dialog->grab().save(qEnvironmentVariable("JX3DPS_TEST_SCREENSHOT") + "-flow-timeline.png"));
            editor->EditNode(9);
            QTest::qWait(100);
            QVERIFY(node_dialog->grab().save(qEnvironmentVariable("JX3DPS_TEST_SCREENSHOT") + "-flow-node.png"));
            node_dialog->reject();
            dialog->close();
        }
        window.close();
    }

    void flowMacroDrivesSimulation()
    {
        SimulationOptions options;
        auto             *choice = options.findChild<QComboBox *>("specialization");
        auto             *flow   = options.findChild<CodeEditor *>("flowMacroEditor");
        flow->setPlainText("/set_target level=52 shield=4500 life=25% distance=3 vulnerable=5%\n/set_player "
                           "qidian=3\n00:02 /set_target distance=30");
        QString error;
        QVERIFY2(options.CheckFlowMacro(error), qPrintable(error));
        for (auto code : { "tx", "mw", "gfj" }) {
            choice->setCurrentIndex(choice->findData(code));
            const auto cfg = options.Config();
            QCOMPARE(cfg.target.level, 52);
            QCOMPARE(cfg.target.shield, 4500);
            QCOMPARE(cfg.target.life, .25);
            QCOMPARE(cfg.target.vulnerable, 51);
            QCOMPARE(cfg.target_events.size(), std::size_t(1));
            DebugSimulator dbg;
            dbg.SetOptions(cfg, 160, 7);
            QVERIFY(dbg.Initialize(desktop::Text(cfg.book->default_macro), error));
            QCOMPARE(dbg.GetPlayerState().targetLifePercent, .25);
            if (cfg.book->code == "tx") {
                QCOMPARE(dbg.GetPlayerState().qidian, 3);
            }
            while (!dbg.IsFinished()) {
                dbg.Continue();
            }
            QVERIFY(!dbg.TakeEvents().isEmpty());
        }
    }

    void flowMacroPersistenceAndValidation()
    {
        Workbench window;
        window.resize(1120, 820);
        window.show();
        const auto *options  = window.findChild<SimulationOptions *>();
        auto       *flow     = window.findChild<CodeEditor *>("flowMacroEditor");
        auto       *rotation = window.findChild<MacroEditor *>("macroEditor");
        QVERIFY(!window.findChild<QDialog *>("targetSettingsDialog"));
        QVERIFY(!window.findChild<QToolButton *>("openTargetSettings"));
        QVERIFY(!window.findChild<QSpinBox *>("targetLevel"));
        const auto SOURCE = QStringLiteral("# keep comments\n/set_target level=52 life=50%\n/set_player qidian=3");
        flow->setPlainText(SOURCE);
        rotation->SetBreakpoint(1, true);
        const auto ROTATION_SOURCE = rotation->toPlainText();
        QString    error;
        QVERIFY2(options->CheckFlowMacro(error), qPrintable(error));
        const auto snapshot = options->Snapshot();
        QCOMPARE(snapshot.value("flowMacro").toString(), SOURCE);
        QCOMPARE(snapshot.value("values").toObject().value("targetLevel").toInt(), 52);
        const QTemporaryDir DIRECTORY;
        const auto          PATH = DIRECTORY.filePath("flow.json");
        QVERIFY2(options->ExportConfiguration(PATH, error), qPrintable(error));
        SimulationOptions restored;
        QVERIFY2(restored.ImportConfiguration(PATH, error), qPrintable(error));
        QCOMPARE(restored.Snapshot(), snapshot);
        auto legacy = snapshot;
        legacy["version"]        = 4;
        legacy["specialization"] = 0;
        legacy.remove("flowMacro");
        auto values = legacy.value("values").toObject();
        values.insert("targetLevel", 127);
        values.insert("mwStyle", 0);
        legacy.insert("values", values);
        QVERIFY2(restored.RestoreSnapshot(legacy, error), qPrintable(error));
        QCOMPARE(restored.Config().target.level, 54);
        QCOMPARE(restored.Config().target.shield, 6592);
        const auto MIGRATED = restored.Snapshot();
        auto       mismatch = snapshot;
        mismatch.insert("values", values);
        QVERIFY(!restored.RestoreSnapshot(mismatch, error));
        QCOMPARE(restored.Snapshot(), MIGRATED);
        flow->appendPlainText(QStringLiteral("/set_target life=wrong"));
        QVERIFY(!options->CheckFlowMacro(error));
        QVERIFY(error.contains(QStringLiteral("第 4 行")));
        QVERIFY_THROWS_EXCEPTION(std::invalid_argument, options->ActiveConfig());
        QVERIFY(!options->ExportConfiguration(PATH, error));
        window.findChild<QToolButton *>("startButton")->click();
        QVERIFY(!window.findChild<BatchWorker *>());
        QVERIFY(window.findChild<QLabel *>("simulationResult")->text().contains(QStringLiteral("流程宏")));
        QCOMPARE(rotation->toPlainText(), ROTATION_SOURCE);
        QVERIFY(rotation->HasBreakpoint(1));
        const auto INVALID_SOURCE = flow->toPlainText();
        options->SaveSettings();
        const SimulationOptions draft;
        QCOMPARE(draft.findChild<CodeEditor *>("flowMacroEditor")->toPlainText(), INVALID_SOURCE);
        QVERIFY(!draft.CheckFlowMacro(error));
        QCOMPARE(draft.Snapshot().value("values"), snapshot.value("values"));
        flow->undo();
        QCOMPARE(flow->toPlainText(), SOURCE);
        QVERIFY(options->CheckFlowMacro(error));
        QCOMPARE(options->Snapshot(), snapshot);
        window.findChild<QToolButton *>("debugButton")->click();
        window.findChild<QToolButton *>("startButton")->click();
        const auto *session = window.findChild<DebugSession *>();
        QTRY_COMPARE(session->GetState(), DebugSession::PAUSED);
        QVERIFY(flow->isReadOnly());
        QCOMPARE(session->GetDebugInfo().targetLifePercent, .5);
        QCOMPARE(session->GetDebugInfo().qidian, 3);
        window.findChild<QPushButton *>("nav_simulation")->click();
        window.findChild<QToolButton *>("openFlowMacroHelp")->click();
        auto *help = window.findChild<QDialog *>("flowMacroHelpDialog");
        QTRY_VERIFY(help->isVisible());
        QCOMPARE(session->GetState(), DebugSession::PAUSED);
        QCOMPARE(flow->toPlainText(), SOURCE);
        QCOMPARE(rotation->toPlainText(), ROTATION_SOURCE);
        help->close();
        window.findChild<QAction *>("debugStop")->trigger();
        QVERIFY(!flow->isReadOnly());
        if (qEnvironmentVariableIsSet("JX3DPS_TEST_SCREENSHOT")) {
            QVERIFY(window.grab().save(qEnvironmentVariable("JX3DPS_TEST_SCREENSHOT") + "-flow-macro.png"));
        }
        QVERIFY(window.close());
    }

    void historyChartInteractions()
    {
        desktop::HistoryChart chart;
        chart.resize(850, 310);
        chart.show();
        chart.SetPoints({
            { 1, 0 },
            { 2, 0 },
            { 3, 0 }
        });
        for (int i = 0; i < 3; ++i) {
            QVERIFY(chart.PlotRect().contains(chart.PointPosition(i)));
        }
        chart.SetPoints({
            { 10, 30000 },
            { 11, 36000 },
            { 12, 34000 }
        });
        chart.SetReference(50000);
        for (int i = 0; i < 3; ++i) {
            QVERIFY(chart.PlotRect().contains(chart.PointPosition(i)));
        }
        quint64 selected  = 0;
        chart.runSelected = [&selected](quint64 id) {
            selected = id;
        };
        QTest::mouseClick(&chart, Qt::LeftButton, Qt::NoModifier, chart.PointPosition(0).toPoint());
        QCOMPARE(selected, quint64(10));
        QTest::keyClick(&chart, Qt::Key_Right);
        QCOMPARE(selected, quint64(11));
        QTest::keyClick(&chart, Qt::Key_Left);
        QCOMPARE(selected, quint64(10));
        chart.SetReference(std::nullopt);
        chart.SetPoints({
            { 13, 34000 }
        });
        QCOMPARE(chart.PointPosition(0).x(), chart.PlotRect().center().x());
        chart.SetPoints({});
        QVERIFY(chart.accessibleDescription().contains(QStringLiteral("尚无")));
        QCOMPARE(chart.grab().toImage().pixelColor(10, 80), QColor(desktop::theme::Surface));
    }

    void magpieShellNavigation()
    {
        Workbench window;
        window.resize(1206, 860);
        window.show();
        QTest::qWait(100);
        auto       *navigation = window.findChild<QWidget *>("topNavigation");
        auto       *editor     = window.findChild<MacroEditor *>("macroEditor");
        const auto *options    = window.findChild<SimulationOptions *>();
        const auto *pages      = window.findChild<QStackedWidget *>("workspacePages");
        QVERIFY(navigation);
        QVERIFY(!window.findChild<QWidget *>("navigationSidebar"));
        QCOMPARE(navigation->height(), desktop::theme::NavigationHeight);
        QCOMPARE(navigation->width(), window.width());
        QCOMPARE(navigation->grab().toImage().pixelColor(3, 3), QColor(desktop::theme::Canvas));
        QVERIFY(pages->mapTo(&window, QPoint()).x() <= 20);
        QVERIFY(pages->mapTo(&window, QPoint()).y() > navigation->mapTo(&window, navigation->rect().bottomLeft()).y());
        QCOMPARE(window.findChild<QWidget *>("windowTitleBar")->height(), 30);
        QTRY_VERIFY(window.findChild<QToolButton *>("debugButton")->isVisible());
        const auto snapshot = options->Snapshot();
        editor->appendPlainText(QStringLiteral("# 导航不丢失宏草稿"));
        editor->SetBreakpoint(2, true);
        const auto SOURCE   = editor->toPlainText();
        auto      *document = editor->document();
        const auto shot     = [&window](const QString &name) {
            if (qEnvironmentVariableIsSet("JX3DPS_TEST_SCREENSHOT")) {
                QTest::qWait(180);
                QVERIFY(window.grab().save(qEnvironmentVariable("JX3DPS_TEST_SCREENSHOT") + "-magpie-" + name + ".png"));
            }
        };
        shot("simulation");
        QTest::mouseClick(window.findChild<QPushButton *>("nav_character"), Qt::LeftButton);
        QVERIFY(window.findChild<QPushButton *>("nav_attributes")->isChecked());
        QVERIFY(window.findChild<QWidget *>("characterChildren")->isVisible());
        for (const auto *page :
             { "profiles", "attributes", "talents", "permanents", "macro", "general", "about", "statistics", "skills", "gains", "hotspots", "timeline", "events", "buffs" })
        {
            auto *button = window.findChild<QPushButton *>(QStringLiteral("nav_%1").arg(QString::fromLatin1(page)));
            QVERIFY(button);
            const QString KEY = QString::fromLatin1(page);
            if (KEY == "profiles" || KEY == "attributes" || KEY == "talents" || KEY == "permanents") {
                window.findChild<QPushButton *>("nav_character")->click();
            }
            if (KEY == "general" || KEY == "about") {
                window.findChild<QPushButton *>("nav_settings")->click();
            }
            if (button->parentWidget()->objectName() == "analysisChildren") {
                window.findChild<QPushButton *>("nav_analysis")->click();
            }
            QCoreApplication::processEvents();
            QVERIFY(button->isVisible());
            QTest::mouseClick(button, Qt::LeftButton);
            QCoreApplication::processEvents();
            QVERIFY(button->isChecked());
            QCOMPARE(options->Snapshot(), snapshot);
            QCOMPARE(editor->document(), document);
            QCOMPARE(editor->toPlainText(), SOURCE);
            QVERIFY(editor->HasBreakpoint(2));
            QVERIFY(pages->currentWidget()->isVisible());
            if (KEY == "hotspots") {
                QVERIFY(window.findChild<QPushButton *>("runWithHotspots")->isVisible());
            }
            if (KEY == "events") {
                QCOMPARE(window.findChild<QTabWidget *>("analysisViews")->currentWidget(),
                         window.findChild<QTableWidget *>("damageEvents"));
            }
            QVERIFY(window.findChild<QLabel *>("breadcrumb")->text().contains(button->text().trimmed()));
            shot(QString::fromLatin1(page));
        }
        window.findChild<QPushButton *>("nav_attributes")->click();
        window.resize(1120, 820);
        QCoreApplication::processEvents();
        QCOMPARE(window.size(), QSize(1120, 820));
        QVERIFY(window.findChild<QWidget *>("gameAttributePanel")->isVisible());
        QCOMPARE(window.findChild<QScrollArea *>("configurationScroll")->horizontalScrollBar()->maximum(), 0);
        window.resize(1206, 860);
        window.findChild<QPushButton *>("nav_general")->click();
        auto *restore = window.findChild<QAbstractButton *>("restoreDrafts");
        restore->click();
        QVERIFY(!QSettings().value("workspace/restoreDrafts").toBool());
        restore->click();
        auto *size = window.findChild<QComboBox *>("editorFontSize");
        size->setCurrentIndex(size->findData(18));
        QCOMPARE(editor->font().pixelSize(), 18);
        auto *search = window.findChild<QLineEdit *>("searchSettings");
        search->setText(QStringLiteral("窗口尺寸"));
        QVERIFY(window.findChild<QAbstractButton *>("rememberGeometry")->isVisible());
        QVERIFY(!restore->isVisible());
        search->clear();
        QVERIFY(restore->isVisible());
        window.findChild<QPushButton *>("nav_simulation")->click();
        window.findChild<QSpinBox *>("iterations")->setValue(5);
        window.findChild<QDoubleSpinBox *>("seconds")->setValue(10);
        window.findChild<QToolButton *>("startButton")->click();
        QTRY_VERIFY_WITH_TIMEOUT(window.findChild<QToolButton *>("startButton")->isEnabled(), 10000);
        QVERIFY(window.findChild<QLabel *>("meanDps")->text().toDouble() > 0);
        for (const int ATTACK : { 11000, 10500, 12500, 14000 }) {
            window.findChild<QSpinBox *>("attackBase")->setValue(ATTACK);
            window.findChild<QToolButton *>("startButton")->click();
            QTRY_VERIFY_WITH_TIMEOUT(window.findChild<QToolButton *>("startButton")->isEnabled(), 10000);
        }
        window.findChild<QPushButton *>("nav_statistics")->click();
        window.findChild<QTabWidget *>("resultTabs")->setCurrentIndex(0);
        QCOMPARE(window.findChild<QTableWidget *>("resultHistory")->rowCount(), 5);
        QVERIFY(window.findChild<QWidget *>("historyDpsChart")->accessibleDescription().contains(QStringLiteral("5 次运行")));
        shot("history");
        auto *chart = static_cast<desktop::HistoryChart *>(window.findChild<QWidget *>("historyDpsChart"));
        QTest::mouseClick(chart, Qt::LeftButton, Qt::NoModifier, chart->PointPosition(0).toPoint());
        QCOMPARE(window.findChild<QTableWidget *>("resultHistory")->currentRow(), 0);
        QTest::keyClick(chart, Qt::Key_Right);
        QCOMPARE(window.findChild<QTableWidget *>("resultHistory")->currentRow(), 1);
        const auto *details = window.findChild<QTextBrowser *>("resultDetails");
        QVERIFY(!details->isVisible());
        window.findChild<QPushButton *>("toggleResultDetails")->click();
        QVERIFY(details->isVisible());
        QVERIFY(details->toPlainText().contains(QStringLiteral("#2")));
        window.findChild<QPushButton *>("toggleResultDetails")->click();
        QVERIFY(!details->isVisible());
        window.findChild<QToolButton *>("closeAnalysis")->click();
        QVERIFY(editor->isVisible());
        QTRY_VERIFY(window.findChild<QToolButton *>("runMacroButton")->isVisible());
        QVERIFY(window.findChild<QToolButton *>("runMacroButton")->isEnabled());
        window.findChild<QToolButton *>("runMacroButton")->click();
        QTRY_VERIFY_WITH_TIMEOUT(window.findChild<QToolButton *>("runMacroButton")->isEnabled(), 10000);
        QCOMPARE(window.findChild<QTableWidget *>("resultHistory")->rowCount(), 6);
        window.findChild<QPushButton *>("nav_simulation")->click();
        window.resize(1120, 820);
        QCoreApplication::processEvents();
        QCOMPARE(window.size(), QSize(1120, 820));
        QVERIFY(editor->width() >= 250);
        const auto *debug_toggle = window.findChild<QToolButton *>("debugButton");
        QTRY_VERIFY(debug_toggle->isVisible());
        QVERIFY(debug_toggle->visibleRegion().contains(debug_toggle->rect().center()));
        const auto *more = window.findChild<QToolButton *>("macroMore");
        QVERIFY(more->isVisible());
        QVERIFY(more->visibleRegion().contains(more->rect().center()));
        shot("compact");
        for (const auto *button : navigation->findChildren<QPushButton *>()) {
            QVERIFY(button->isVisible());
            QVERIFY2(button->visibleRegion().boundingRect().contains(button->rect()), qPrintable(button->objectName()));
            QVERIFY(navigation->rect().contains(button->mapTo(navigation, button->rect().bottomRight())));
        }
        window.findChild<QPushButton *>("nav_settings")->click();
        QVERIFY(window.findChild<QPushButton *>("nav_general")->isVisible());
        QVERIFY(window.findChild<QPushButton *>("nav_settings")->isChecked());
        QVERIFY(!window.findChild<QWidget *>("characterChildren")->isVisible());
        QVERIFY(!window.findChild<QToolButton *>("toggleSidebar"));
        QVERIFY(window.close());
    }

    void analysisNavigationRunsFromEmptyPages()
    {
        Workbench window;
        window.resize(1120, 820);
        window.show();
        const auto *options = window.findChild<SimulationOptions *>();
        window.findChild<QSpinBox *>("iterations")->setValue(12);
        window.findChild<QDoubleSpinBox *>("seconds")->setValue(30);
        const auto CLICK_PAGE = [&](const char *key) {
            auto *button = window.findChild<QPushButton *>(QStringLiteral("nav_") + QString::fromLatin1(key));
            QVERIFY(button && button->isVisible());
            QTest::mouseClick(button, Qt::LeftButton);
        };
        CLICK_PAGE("analysis");
        CLICK_PAGE("hotspots");
        auto *start = window.findChild<QPushButton *>("runWithHotspots");
        QVERIFY(start->isVisible());
        QTest::mouseClick(start, Qt::LeftButton);
        QTRY_COMPARE_WITH_TIMEOUT(window.findChild<QTableWidget *>("resultHistory")->rowCount(), 1, 10000);
        QVERIFY(window.findChild<QPushButton *>("nav_hotspots")->isChecked());
        QVERIFY(start->isHidden());
        CLICK_PAGE("gains");
        auto *gains = window.findChild<QPushButton *>("runWithGains");
        QTRY_VERIFY(gains->isVisible() && gains->isEnabled());
        QTest::mouseClick(gains, Qt::LeftButton);
        QTRY_COMPARE_WITH_TIMEOUT(window.findChild<QTableWidget *>("resultHistory")->rowCount(), 2, 10000);
        QVERIFY(options->AutoCalculateGains());
        QVERIFY(window.findChild<QPushButton *>("nav_gains")->isChecked());
        QVERIFY(gains->isHidden());
    }

    void equipmentReferenceCard()
    {
        // Reference values exercise every visible section; production data never uses this fixture.
        const QJsonObject set{
            { "name",       QStringLiteral("西塞·冷辉")                                                                                                           },
            { "siblings",
             QJsonArray{ QStringLiteral("西塞·冷辉护手"),
                          QStringLiteral("西塞·冷辉腰带"),
                          QStringLiteral("西塞·冷辉履"),
                          QStringLiteral("西塞·冷辉冠"),
                          QStringLiteral("西塞·冷辉衣") }                                                                                                         },
            { "attributes",
             QJsonObject{ { "2", QStringLiteral("装备：施展外功伤害招式，一定几率提高自身外功会心几率4%，会心效果4%，持续6秒。") },
                           { "4", QStringLiteral("<text>text=\"“无我无剑”伤害提高10%\" font=101 </text><text>text=\"“对阵招式”伤害提高5%\" font=101 </text>") } } }
        };
        const auto ATTRIBUTE = [](const QString &label, const char *color, int bonus = 0) {
            return QJsonObject{
                { "label",        label                      },
                { "color",        QString::fromLatin1(color) },
                { "refine_bonus", bonus                      }
            };
        };
        QJsonObject item{
            { "id",                  "7_101482"                                                                                                                                           },
            { "name",                QStringLiteral("孤漠·立雪衣")                                                                                                                        },
            { "type",                QStringLiteral("上衣")                                                                                                                               },
            { "quality",             4                                                                                                                                                    },
            { "max_refine",          6                                                                                                                                                    },
            { "refine_level",        6                                                                                                                                                    },
            { "attributes",
             QJsonArray{ ATTRIBUTE(QStringLiteral("外功防御等级提高1060"), "white"),
                          ATTRIBUTE(QStringLiteral("内功防御等级提高848"), "white"),
                          ATTRIBUTE(QStringLiteral("体质提高14219"), "white", 1066),
                          ATTRIBUTE(QStringLiteral("身法提高1160"), "white", 87),
                          ATTRIBUTE(QStringLiteral("外功攻击提高2635"), "green", 198),
                          ATTRIBUTE(QStringLiteral("外功会心等级提高9695"), "green", 727),
                          ATTRIBUTE(QStringLiteral("无双等级提高7756"), "green", 582) }                                                                                                   },
            { "diamonds",            QJsonArray{ QStringLiteral("外功会心等级提高1317"), QStringLiteral("外功会心效果等级提高1317") }                                                     },
            { "require_level",       130                                                                                                                                                  },
            { "durability",          4800                                                                                                                                                 },
            { "max_durability",      4800                                                                                                                                                 },
            { "enchantments",
             QJsonArray{ QJsonObject{ { "icon_id", 9527 }, { "description", QStringLiteral("无双等级提高624") } },
                          QJsonObject{ { "icon_id", 18272 },
                                       { "description", QStringLiteral("自身受到的疗伤成效降低10%，基础攻击小幅提高（内功攻击提高791点，外功攻击提高710点），不在名剑大会中生效。") } } } },
            { "set",                 set                                                                                                                                                  },
            { "level",               25900                                                                                                                                                },
            { "level_refine_bonus",  1943                                                                                                                                                 },
            { "score",               46620                                                                                                                                                },
            { "score_refine_bonus",  3497                                                                                                                                                 },
            { "score_enchant_bonus", 9180                                                                                                                                                 },
            { "source",              QStringLiteral("套装兑换（兑换牌·纯阳） / 侠行点（侠行·纯阳，侠行·初级套装）")                                                                       }
        };
        QJsonObject equipped;
        for (const auto &slot : { QStringLiteral("帽子"), QStringLiteral("腰带"), QStringLiteral("鞋子") }) {
            equipped.insert(slot,
                            QJsonObject{
                                { "set", set }
            });
        }
        const QString                HTML = desktop::EquipmentDetailCard::Html(item, equipped, QStringLiteral("上衣"));
        desktop::EquipmentDetailCard card;
        card.icon = [](int id) {
            return desktop::GameIcon(id).pixmap(32, 32);
        };
        card.resize(751, 1133);
        card.SetContentHtml(HTML);
        card.show();
        QCoreApplication::processEvents();
        const auto text = card.toPlainText();
        QVERIFY(text.contains(QStringLiteral("精炼等级: 6 / 6")));
        QVERIFY(text.contains(QStringLiteral("身法+1160 (+87)")));
        QVERIFY(text.contains(QStringLiteral("西塞·冷辉(4/5)")));
        QVERIFY(text.contains(QStringLiteral("装备分数 46620 (+3497+9180)")));
        QVERIFY(!text.contains("font=101"));
        QCOMPARE(card.horizontalScrollBar()->maximum(), 0);
        QCOMPARE(card.grab().toImage().pixelColor(700, 600), QColor("#132221"));
        if (qEnvironmentVariableIsSet("JX3DPS_TEST_SCREENSHOT")) {
            QVERIFY(card.grab().save(qEnvironmentVariable("JX3DPS_TEST_SCREENSHOT") + "-reference-card.png"));
        }

        // Missing metadata must not turn into invented durability, enchantments or refinement.
        item.remove("refine_level");
        item.remove("durability");
        item.remove("enchantments");
        const QString BASE = desktop::EquipmentDetailCard::Html(item, {}, QStringLiteral("上衣"));
        card.SetContentHtml(BASE);
        QVERIFY(card.toPlainText().contains(QStringLiteral("精炼上限: 6")));
        QVERIFY(card.toPlainText().contains(QStringLiteral("耐久度上限：4800")));
        QVERIFY(card.toPlainText().contains(QStringLiteral("西塞·冷辉(1/5)")));
        QVERIFY(!card.toPlainText().contains(QStringLiteral("无双等级提高624")));
        QVERIFY(BASE.contains("color:#acaeb1'>[4]"));
        card.resize(500, 640);
        QCoreApplication::processEvents();
        QCOMPARE(card.horizontalScrollBar()->maximum(), 0);
        QVERIFY(card.verticalScrollBar()->maximum() > 0);
        item["name"] = QStringLiteral("<img src='https://example.invalid'>测试");
        card.SetContentHtml(desktop::EquipmentDetailCard::Html(item, {}, QStringLiteral("上衣")));
        QVERIFY(card.toPlainText().contains(item.value("name").toString()));
    }

    void macroHelpOpensFromPanel()
    {
        Workbench window;
        window.show();
        const auto *options = window.findChild<SimulationOptions *>();
        auto       *editor  = window.findChild<MacroEditor *>("macroEditor");
        const auto *pages   = window.findChild<QStackedWidget *>("workspacePages");
        auto       *button  = window.findChild<QToolButton *>("openMacroHelp");
        auto       *dialog  = window.findChild<QDialog *>("macroHelpDialog");
        const auto *browser = window.findChild<QTextBrowser *>("macroHelpBrowser");
        const auto *panel   = window.findChild<QGroupBox *>("macroPanel");
        QVERIFY(button && dialog && browser && panel);
        QVERIFY(!window.findChild<QPushButton *>("nav_help"));
        QVERIFY(!window.findChild<QWidget *>("macroHelpPage"));
        QVERIFY(panel->findChild<QWidget *>("groupHeaderActions")->isAncestorOf(button));
        const auto ORIGINAL = editor->toPlainText();
        editor->appendPlainText(QStringLiteral("\n# 帮助窗口不修改宏"));
        editor->SetBreakpoint(2, true);
        const auto SOURCE   = editor->toPlainText();
        const auto snapshot = options->Snapshot();
        auto      *document = editor->document();
        for (const QSize SIZE : { QSize(1206, 860), QSize(1120, 820) }) {
            window.resize(SIZE);
            for (const auto *page : { "simulation", "macro" }) {
                auto *nav = window.findChild<QPushButton *>(QStringLiteral("nav_%1").arg(QString::fromLatin1(page)));
                nav->click();
                QTest::qWait(desktop::motion::SlideMs + 40);
                QVERIFY(button->isVisible() && button->visibleRegion().contains(button->rect()));
                QStyleOptionGroupBox style;
                style.initFrom(panel);
                style.text        = panel->title();
                style.subControls = QStyle::SC_GroupBoxFrame | QStyle::SC_GroupBoxLabel;
                const auto FRAME = panel->style()->subControlRect(QStyle::CC_GroupBox, &style, QStyle::SC_GroupBoxFrame, panel);
                QVERIFY(button->mapTo(panel, button->rect().bottomLeft()).y() < FRAME.top());
                QVERIFY(button->mapTo(panel, button->rect().topRight()).x() >= panel->width() - 4);
                auto *current_page = pages->currentWidget();
                if (qEnvironmentVariableIsSet("JX3DPS_TEST_SCREENSHOT")) {
                    QVERIFY(window.grab().save(
                        qEnvironmentVariable("JX3DPS_TEST_SCREENSHOT") +
                        QStringLiteral("-macro-help-entry-%1-%2.png").arg(QString::fromLatin1(page)).arg(SIZE.width())));
                }
                QTest::mouseClick(button, Qt::LeftButton);
                QTRY_VERIFY(dialog->isVisible());
                QVERIFY(dialog->isWindow() && !dialog->isModal());
                QVERIFY(browser->toPlainText().contains(QStringLiteral("/cast")));
                QCOMPARE(pages->currentWidget(), current_page);
                QVERIFY(nav->isChecked());
                QCOMPARE(options->Snapshot(), snapshot);
                QCOMPARE(editor->document(), document);
                QCOMPARE(editor->toPlainText(), SOURCE);
                QVERIFY(editor->HasBreakpoint(2));
                QVERIFY(editor->document()->isUndoAvailable());
                QVERIFY(browser->toPlainText().contains(QStringLiteral("太虚剑意")));
                browser->verticalScrollBar()->setValue(qMin(80, browser->verticalScrollBar()->maximum()));
                const auto SCROLL_POSITION = browser->verticalScrollBar()->value();
                dialog->close();
                button->click();
                QTRY_VERIFY(dialog->isVisible());
                QCOMPARE(window.findChildren<QDialog *>("macroHelpDialog").size(), 1);
                QCOMPARE(browser->verticalScrollBar()->value(), SCROLL_POSITION);
                dialog->close();
            }
        }
        editor->undo();
        QCOMPARE(editor->toPlainText(), ORIGINAL);
        editor->redo();
        QCOMPARE(editor->toPlainText(), SOURCE);
        window.findChild<QAction *>("macroHelp")->trigger();
        QTRY_VERIFY(dialog->isVisible());
        window.findChild<QComboBox *>("specialization")->setCurrentIndex(1);
        QVERIFY(browser->toPlainText().contains(QStringLiteral("莫问")));
        QVERIFY(browser->toPlainText().contains(desktop::DefaultMacro(desktop::Specialization::MO_WEN).trimmed()));
        QCOMPARE(desktop::theme::AccentColor(dialog), desktop::theme::AccentColor(&window));
        if (qEnvironmentVariableIsSet("JX3DPS_TEST_SCREENSHOT")) {
            QVERIFY(dialog->grab().save(qEnvironmentVariable("JX3DPS_TEST_SCREENSHOT") + "-macro-help-window.png"));
        }
        dialog->close();
        window.findChild<QPushButton *>("nav_about")->click();
        auto *about = pages->currentWidget();
        window.findChild<QToolButton *>("openMacroHelpFromAbout")->click();
        QTRY_VERIFY(dialog->isVisible());
        QCOMPARE(pages->currentWidget(), about);
        dialog->close();
        window.findChild<QPushButton *>("nav_simulation")->click();
        window.findChild<QAction *>("debugStart")->trigger();
        const auto *session = window.findChild<DebugSession *>();
        QTRY_COMPARE(session->GetState(), DebugSession::PAUSED);
        const auto PAUSED_LINE     = editor->GetCurrentDebugLine();
        const auto PAUSED_SOURCE   = editor->toPlainText();
        const auto PAUSED_SNAPSHOT = options->Snapshot();
        button->click();
        QTRY_VERIFY(dialog->isVisible());
        QCOMPARE(session->GetState(), DebugSession::PAUSED);
        QCOMPARE(editor->GetCurrentDebugLine(), PAUSED_LINE);
        QCOMPARE(editor->toPlainText(), PAUSED_SOURCE);
        QCOMPARE(options->Snapshot(), PAUSED_SNAPSHOT);
        QVERIFY(editor->isReadOnly());
        QVERIFY(window.close());
        QVERIFY(!dialog->isVisible());
    }

    void equipmentPreviewPreservesPausedSimulation()
    {
        Workbench window;
        window.show();
        auto *options = window.findChild<SimulationOptions *>();
        auto *school  = options->findChild<QComboBox *>("specialization");
        school->setCurrentIndex(school->findData("mw"));
        window.findChild<QAction *>("debugStart")->trigger();
        auto *session = window.findChild<DebugSession *>();
        QCOMPARE(session->GetState(), DebugSession::PAUSED);
        auto before = options->Snapshot();
        options->ShowEquipment();
        auto *dlg   = options->findChild<QDialog *>("equipmentDialog");
        auto *panel = dlg->findChild<EquipmentPanel *>();
        QTRY_VERIFY(panel && panel->isVisible());
        auto *effect = options->findChild<QCheckBox *>("equipment/mw/神兵·羽");
        QVERIFY(effect && !effect->isEnabled());
        panel->findChild<QToolButton *>("equipmentSlot0")->click();
        auto *items = panel->findChild<QListWidget *>("equipmentChoices");
        QVERIFY(items && items->count() > 0);
        QCOMPARE(session->GetState(), DebugSession::PAUSED);
        dlg->close();
        QCOMPARE(options->Snapshot(), before);
        window.findChild<QAction *>("debugStop")->trigger();
        school->setCurrentIndex(school->findData("gfj"));
        options->ShowEquipment();
        QCOMPARE(panel->findChild<QLabel *>("equipmentSchool")->text(), QStringLiteral("孤锋诀"));
        QVERIFY(effect->isHidden() || !effect->isVisible());
        dlg->close();
    }

    void optionalFightHotspots_data()
    {
        QTest::addColumn<bool>("mo_wen");
        QTest::addColumn<bool>("trace");
        QTest::addColumn<bool>("gains");
        QTest::newRow("tai-xu-batch") << false << false << false;
        QTest::newRow("mo-wen-batch") << true << false << false;
        QTest::newRow("tai-xu-trace") << false << true << false;
        QTest::newRow("mo-wen-gains") << true << false << true;
    }

    void optionalFightHotspots()
    {
        QFETCH(bool, mo_wen);
        QFETCH(bool, trace);
        QFETCH(bool, gains);
        const desktop::Config CONFIG = JX3DPS::engine::Scenario(mo_wen ? "mw" : "tx");
        const auto SPEC     = desktop::ClassOf(CONFIG);
        const auto SOURCE   = desktop::DefaultMacro(SPEC);
        const auto COMPILED = rt::MacroCompiler::Compile(SOURCE.toStdString(), desktop::MacroOptions(SPEC));
        QVERIFY(COMPILED.second.empty());
        const rt::BatchOptions OPTIONS{ trace ? 1u : 512u, 42, 4800, 2, 4800 * 16, 4800 * 40 };
        SimulationResult       plain;
        ResultsPanel           panel;
        const auto            *tabs = panel.findChild<QTabWidget *>("resultTabs");
        const int              TAB  = tabs->count() - 1;
        for (const bool ENABLED : { false, true }) {
            BatchWorker worker(COMPILED.first.program, OPTIONS, CONFIG, nullptr, trace, gains, desktop::GainComparisonMode::SAME_VALUE, ENABLED);
            QSignalSpy ready(&worker, &BatchWorker::StatisticsReady);
            worker.start();
            QVERIFY(worker.wait(20000));
            QTRY_COMPARE(ready.size(), 1);
            const auto result = qvariant_cast<SimulationResult>(ready.front().front());
            QVERIFY(result.Complete());
            QCOMPARE(result.hotspots.has_value(), ENABLED);
            panel.AddResult(result, SOURCE, {});
            QVERIFY(tabs->isTabVisible(TAB));
            QCOMPARE(panel.findChild<QPushButton *>("runWithHotspots")->isHidden(), ENABLED);
            if (!ENABLED) {
                plain = result;
                continue;
            }
            QCOMPARE(result.stats.checksum, plain.stats.checksum);
            QCOMPARE(result.skills.size(), plain.skills.size());
            for (int i = 0; i < result.gains.size(); ++i) {
                QCOMPARE(result.gains[i].stats.checksum, plain.gains[i].stats.checksum);
            }
            QCOMPARE(result.hotspots->fights,
                     OPTIONS.iterations * (gains ? rt::GainSimulationStages(CONFIG) : 1));
            QCOMPARE(result.hotspots->workers.size(), trace ? 1 : 2);
            quint64 worker_points = 0;
            quint64 worker_fights = 0;
            for (const auto &w : result.hotspots->workers) {
                QVERIFY(w.fights > 0);
                worker_points += w.samplePoints;
                worker_fights += w.fights;
            }
            QCOMPARE(worker_fights, result.hotspots->fights);
            QCOMPARE(worker_points, result.hotspots->samplePoints);
            QVERIFY(result.hotspots->workerMs > 0);
            const auto &calls = result.hotspots->calls;
            QVERIFY(!calls.isEmpty());
            QCOMPARE(calls.front().inclusive, result.hotspots->samplePoints);
            QVector<quint64> accounted(calls.size());
            for (int i = 0; i < calls.size(); ++i) {
                const auto &call  = calls[i];
                accounted[i]     += call.self;
                if (i != 0) {
                    QVERIFY(std::cmp_less(call.parent, i));
                    accounted[call.parent] += call.inclusive;
                }
            }
            for (int i = 0; i < calls.size(); ++i) {
                QCOMPARE(calls[i].inclusive, accounted[i]);
            }
            double estimate = 0;
            for (const double STAGE : result.hotspots->phaseUs) {
                estimate += STAGE;
            }
            if (result.hotspots->samplePoints != 0u) {
                QVERIFY(qAbs(estimate - result.hotspots->workerMs * 1000 / result.hotspots->fights) < 1e-6);
            }
            QCOMPARE(tabs->currentIndex(), TAB);
            QCOMPARE(panel.findChild<QTableWidget *>("fightHotspotTable")->rowCount(), 10);
            panel.findChild<QTableWidget *>("resultHistory")->selectRow(0);
            QVERIFY(!panel.findChild<QPushButton *>("runWithHotspots")->isHidden());
        }
    }

    void moWenDesktopParity()
    {
        const auto SOURCE = desktop::DefaultMacro(desktop::Specialization::MO_WEN);
        const auto BYTES  = SOURCE.toUtf8();
        const auto COMPILED =
            rt::MacroCompiler::Compile(std::string_view(BYTES.constData(), BYTES.size()), desktop::MacroOptions(desktop::Specialization::MO_WEN));
        QVERIFY(COMPILED.second.empty());
        auto config = JX3DPS::engine::Scenario("mw");
        const rt::BatchOptions OPTIONS{ 1, 913, 16 * 45, 1, (16 * 45 * 16) + 64, (16 * 45 * 40) + 128 };
        BatchWorker            worker(COMPILED.first.program, OPTIONS, config, nullptr, true);
        QSignalSpy             trace(&worker, &BatchWorker::TraceReady);
        QSignalSpy             buffs(&worker, &BatchWorker::BuffTraceReady);
        QSignalSpy             history_signal(&worker, &BatchWorker::HistoryReady);
        QSignalSpy             stats(&worker, &BatchWorker::StatisticsReady);
        QSignalSpy             failure(&worker, &BatchWorker::Failed);
        worker.start();
        QVERIFY(worker.wait(10000));
        QTRY_COMPARE(stats.count(), 1);
        QCOMPARE(failure.count(), 0);
        DebugSimulator debugger;
        debugger.SetOptions(config, OPTIONS.duration, OPTIONS.seed);
        QString error;
        QVERIFY2(debugger.Initialize(SOURCE, error), qPrintable(error));
        QVERIFY(debugger.GetPlayerState().details.join('\n').contains(QStringLiteral("曲风")));
        QVERIFY(debugger.SetBreakpoint(3));
        debugger.Continue();
        QVERIFY(debugger.IsPaused());
        QCOMPARE(debugger.GetPlayerState().currentMacroLine, 3);
        debugger.SetBreakpoint(3, false);
        debugger.StepInto();
        while (!debugger.IsFinished()) {
            debugger.Continue();
        }
        const auto DEBUG_EVENTS = debugger.TakeEvents();
        const auto TRACE_EVENTS = qvariant_cast<CombatEvents>(trace[0][0]);
        QTRY_COMPARE(history_signal.count(), 1);
        const auto TRACE_HISTORY = qvariant_cast<desktop::HistoryChunk>(history_signal[0][0]);
        const auto DEBUG_HISTORY = debugger.TakeHistory();
        QVERIFY(TRACE_HISTORY.initial.has_value());
        QCOMPARE(TRACE_HISTORY.attributes, DEBUG_HISTORY.attributes);
        QCOMPARE(TRACE_HISTORY.mutations, DEBUG_HISTORY.mutations);
        desktop::CombatHistory recorded;
        recorded.Append(TRACE_HISTORY);
        QVERIFY(recorded.At(OPTIONS.duration).has_value());
        QCOMPARE(recorded.At(OPTIONS.duration)->attributes, DEBUG_HISTORY.attributes.back().values);
        QCOMPARE(DEBUG_EVENTS.size(), TRACE_EVENTS.size());
        for (int index = 0; index < DEBUG_EVENTS.size(); ++index) {
            QCOMPARE(DEBUG_EVENTS[index].frame, TRACE_EVENTS[index].frame);
            QCOMPARE(DEBUG_EVENTS[index].skill, TRACE_EVENTS[index].skill);
            QCOMPARE(DEBUG_EVENTS[index].damage, TRACE_EVENTS[index].damage);
            QCOMPARE(DEBUG_EVENTS[index].outcome, TRACE_EVENTS[index].outcome);
            QCOMPARE(DEBUG_EVENTS[index].name, TRACE_EVENTS[index].name);
        }
        QCOMPARE(debugger.TakeBuffEvents(), qvariant_cast<BuffEvents>(buffs[0][0]));
        QCOMPARE(debugger.TotalDamage(), qvariant_cast<SimulationResult>(stats[0][0]).stats.min_damage);
        config.equipment = {std::string(rt::season50::WindPendant)};
        debugger.SetOptions(config, OPTIONS.duration, OPTIONS.seed);
        QVERIFY(debugger.Initialize(QStringLiteral("/scast 徵\n/fcast 使用风腰坠\n/cast 宫"), error));
        bool interruption = false;
        for (int step = 0; step < 200 && !debugger.IsFinished(); ++step) {
            debugger.StepInto();
            for (const auto &observation : debugger.TakeMacroSteps()) {
                if (observation.kind == MacroStepKind::ACTION && observation.details.contains(QStringLiteral("先打断 徵"))) {
                    interruption = true;
                }
            }
        }
        QVERIFY(interruption);
    }

    void teamBuffOptionsAndSnapshots()
    {
        SimulationOptions options;
        auto             *combo = options.findChild<QComboBox *>(QStringLiteral("permanent/外功/食品增强"));
        QVERIFY(combo);
        QVERIFY(combo->count() > 1);
        combo->setCurrentIndex(1);
        const auto        snapshot = options.Snapshot();
        SimulationOptions restored;
        QString           error;
        QVERIFY2(restored.RestoreSnapshot(snapshot, error), qPrintable(error));
        QCOMPARE(restored.Snapshot(), snapshot);
        auto bad              = snapshot;
        bad["specialization"] = "w_gfj";
        QVERIFY(!restored.RestoreSnapshot(bad, error));
        QCOMPARE(restored.Snapshot(), snapshot);
    }

    void moWenOptionsAndImports()
    {
        SimulationOptions options;
        auto             *pick = options.findChild<QComboBox *>("specialization");
        pick->setCurrentIndex(pick->findData("mw"));
        QString     error;
        QJsonObject data{
            { "LunarAttackPowerBase",     2000 },
            { "LunarCriticalStrike",      500  },
            { "LunarCriticalDamagePower", 300  },
            { "LunarOvercome",            400  },
            { "SurplusValue",             0    },
            { "Strain",                   10   },
            { "Haste",                    20   },
            { "Spirit",                   1000 },
            { "MeleeWeaponDamage",        155  },
            { "MeleeWeaponDamageRand",    10   }
        };
        QVERIFY2(options.ImportAttributes(data, error), qPrintable(error));
        const auto cfg = options.Config();
        QCOMPARE(cfg.book->code, std::string_view("mw"));
        QCOMPARE(cfg.attributes.weapon, 160);
        QCOMPARE(cfg.attributes.attack_base, 2000);
        QVERIFY(cfg.attributes.attack_by_class > 0);
        auto original = options.Snapshot();
        data.remove("LunarOvercome");
        QVERIFY(!options.ImportAttributes(data, error));
        QCOMPARE(options.Snapshot(), original);
        SimulationOptions copy;
        QVERIFY2(copy.RestoreSnapshot(original, error), qPrintable(error));
        QCOMPARE(copy.Snapshot(), original);
    }

    void moWenWorkbenchFlow()
    {
        Workbench window;
        window.show();
        auto *editor    = window.findChild<MacroEditor *>("macroEditor");
        auto *selection = window.findChild<QComboBox *>("specialization");
        editor->setPlainText(QStringLiteral("# 保留太虚草稿\n/cast 三环套月"));
        editor->SetBreakpoint(2, true);
        selection->setCurrentIndex(1);
        QVERIFY(editor->toPlainText().contains(QStringLiteral("羽")));
        window.findChild<QAction *>("checkMacro")->trigger();
        QCOMPARE(window.findChild<QTableWidget *>("macroProblems")->rowCount(), 0);
        editor->SetBreakpoint(3, true);
        window.findChild<QDoubleSpinBox *>("seconds")->setValue(15);
        window.findChild<CodeEditor *>("flowMacroEditor")->appendPlainText(QStringLiteral("/set_target distance=5"));
        if (qEnvironmentVariableIsSet("JX3DPS_TEST_SCREENSHOT")) {
            QTest::qWait(30);
            QVERIFY(window.grab().save(qEnvironmentVariable("JX3DPS_TEST_SCREENSHOT") + "-mowen-configuration.png"));
        }
        window.findChild<QToolButton *>("debugButton")->click();
        window.findChild<QToolButton *>("startButton")->click();
        const auto *session = window.findChild<DebugSession *>();
        QCOMPARE(session->GetState(), DebugSession::PAUSED);
        QCOMPARE(session->GetDebugInfo().currentMacro, QStringLiteral("莫问"));
        QVERIFY(!selection->isEnabled());
        window.findChild<QAction *>("debugContinue")->trigger();
        QTRY_COMPARE(session->GetState(), DebugSession::PAUSED);
        QCOMPARE(session->GetDebugInfo().lineNumber, 3);
        QCOMPARE(window.findChild<QListWidget *>("breakpointList")->count(), 1);
        QCOMPARE(session->GetDebugInfo().currentMacro, QStringLiteral("莫问"));
        QVERIFY(window.findChild<QTextBrowser *>("debugInspector")->toPlainText().contains(QStringLiteral("目标气血")));
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
        const Workbench REOPENED;
        QCOMPARE(REOPENED.findChild<QComboBox *>("specialization")->currentIndex(), 1);
        REOPENED.findChild<QComboBox *>("specialization")->setCurrentIndex(0);
        QCOMPARE(REOPENED.findChild<MacroEditor *>("macroEditor")->toPlainText(), QStringLiteral("# 保留太虚草稿\n/cast 三环套月"));
    }

    void workbenchResultAnalysisNavigation()
    {
        Workbench window;
        window.resize(1366, 900);
        window.show();
        auto       *analysis      = window.findChild<QDialog *>("resultAnalysis");
        auto       *show_analysis = window.findChild<QPushButton *>("showAnalysis");
        const auto *home          = window.findChild<QWidget *>("simulationPage");
        const auto *debug         = window.findChild<QTabWidget *>("debugTabs");
        QVERIFY(analysis);
        QVERIFY(!analysis->isVisible());
        QVERIFY(!analysis->isModal());
        QVERIFY(!window.findChild<QTabWidget *>("workspaceTabs"));
        QVERIFY(window.findChild<QWidget *>("outputPanel")->isAncestorOf(show_analysis));
        QVERIFY(!window.findChild<QWidget *>("debugPage"));
        QVERIFY(window.findChild<QWidget *>("macroPanel")->isAncestorOf(debug));
        auto       *editor   = window.findChild<MacroEditor *>("macroEditor");
        auto       *document = editor->document();
        const auto *options  = window.findChild<SimulationOptions *>();
        const auto *session  = window.findChild<DebugSession *>();
        auto       *start    = window.findChild<QToolButton *>("startButton");
        auto       *gains    = window.findChild<QAction *>("attributeGains");
        editor->setPlainText(desktop::DefaultMacro(desktop::Specialization::TAI_XU));
        editor->appendPlainText(QStringLiteral("# 页面切换保留撤销与断点"));
        editor->SetBreakpoint(3, true);
        window.findChild<QDoubleSpinBox *>("seconds")->setValue(20);
        window.findChild<QSpinBox *>("iterations")->setValue(20);
        const auto snapshot = options->Snapshot();
        const auto SOURCE   = editor->toPlainText();
        window.findChild<QToolButton *>("debugButton")->setChecked(true);
        QCOMPARE(editor->document(), document);
        QVERIFY(editor->isVisible());
        window.findChild<QAction *>("debugStart")->trigger();
        QCOMPARE(session->GetState(), DebugSession::PAUSED);
        window.findChild<QAction *>("debugStepOver")->trigger();
        const auto PHASE = session->GetDebugInfo().phase;
        const auto LINE  = session->GetDebugInfo().lineNumber;
        QCOMPARE(session->GetState(), DebugSession::PAUSED);
        QVERIFY(editor->isVisible());
        QVERIFY(editor->isReadOnly());
        QVERIFY(!start->isEnabled());
        QVERIFY(!window.findChild<QComboBox *>("specialization")->isEnabled());
        QVERIFY(!window.findChild<QSpinBox *>("attackBase")->isEnabled());
        show_analysis->click();
        QTRY_VERIFY(analysis->isVisible());
        QVERIFY(!home->isVisible());
        QVERIFY(!analysis->isWindow());
        QCOMPARE(window.findChild<QTabWidget *>("analysisViews")->currentWidget(), window.findChild<ResultsPanel *>());
        window.findChild<QToolButton *>("closeAnalysis")->click();
        QVERIFY(!analysis->isVisible());
        show_analysis->click();
        window.activateWindow();
        QTRY_VERIFY(window.isActiveWindow());
        QTest::keyClick(window.windowHandle(), Qt::Key_F, Qt::ControlModifier);
        QTRY_VERIFY(!analysis->isVisible());
        QVERIFY(editor->isVisible());
        QVERIFY(window.findChild<MacroSearchBar *>()->isVisible());
        window.findChild<MacroSearchBar *>()->Close();
        QCOMPARE(session->GetDebugInfo().phase, PHASE);
        QCOMPARE(session->GetDebugInfo().lineNumber, LINE);
        QCOMPARE(editor->document(), document);
        QCOMPARE(editor->toPlainText(), SOURCE);
        QCOMPARE(options->Snapshot(), snapshot);
        QVERIFY(editor->HasBreakpoint(3));
        window.findChild<QAction *>("debugStop")->trigger();
        window.findChild<QToolButton *>("debugButton")->setChecked(false);
        editor->undo();
        QVERIFY(!editor->toPlainText().contains(QStringLiteral("页面切换")));
        QVERIFY(editor->HasBreakpoint(3));
        start->click();
        QTRY_VERIFY_WITH_TIMEOUT(start->isEnabled(), 10000);
        QVERIFY(!analysis->isVisible());
        QVERIFY(window.findChild<QLabel *>("meanDps")->text().toDouble() > 0);
        QCOMPARE(show_analysis->text(), QStringLiteral("技能统计"));
        show_analysis->click();
        QTRY_VERIFY(analysis->isVisible());
        QCOMPARE(window.findChild<QTabWidget *>("resultTabs")->currentIndex(), 1);
        const auto *skills = window.findChild<QTableWidget *>("skillDamage");
        QVERIFY(skills->rowCount() > 0);
        double skill_dps = 0;
        for (int row = 0; row < skills->rowCount(); ++row) {
            skill_dps += skills->item(row, 4)->text().toDouble();
        }
        QVERIFY(std::abs(skill_dps - window.findChild<QLabel *>("meanDps")->text().toDouble()) < skills->rowCount() * .1);
        QVERIFY(window.findChild<QLabel *>("skillDamageSummary")->text().contains(QStringLiteral("20 场已完成战斗")));
        if (qEnvironmentVariableIsSet("JX3DPS_TEST_SCREENSHOT")) {
            QTest::qWait(250);
            QVERIFY(analysis->grab().save(qEnvironmentVariable("JX3DPS_TEST_SCREENSHOT") + "-batch-skills.png"));
        }
        analysis->close();
        gains->trigger();
        start->click();
        QTRY_VERIFY_WITH_TIMEOUT(gains->isEnabled(), 10000);
        QVERIFY(!analysis->isVisible());
        const auto *detail_gains = window.findChild<QTableWidget *>("attributeGainsTable");
        QVERIFY(std::abs(window.findChild<QLabel *>("gameAttributeGain_attack")->text().toDouble() -
                         detail_gains->item(1, 4)->text().toDouble()) <= .051);
        show_analysis->click();
        QTRY_VERIFY(analysis->isVisible());
        auto *history = window.findChild<QTableWidget *>("resultHistory");
        QCOMPARE(history->rowCount(), 2);
        if (qEnvironmentVariableIsSet("JX3DPS_TEST_SCREENSHOT")) {
            QTest::qWait(250);
            QVERIFY(analysis->grab().save(qEnvironmentVariable("JX3DPS_TEST_SCREENSHOT") + "-analysis.png"));
        }
        analysis->close();
        QVERIFY(!analysis->isVisible());
        show_analysis->click();
        QCOMPARE(window.findChild<QTableWidget *>("resultHistory"), history);
        QCOMPARE(history->rowCount(), 2);
        analysis->close();
        if (qEnvironmentVariableIsSet("JX3DPS_TEST_SCREENSHOT")) {
            window.findChild<QPushButton *>("nav_simulation")->click();
            QTest::qWait(30);
            QVERIFY(window.grab().save(qEnvironmentVariable("JX3DPS_TEST_SCREENSHOT") + "-overview.png"));
            window.resize(1280, 840);
            QTest::qWait(30);
            QCOMPARE(window.size(), QSize(1280, 840));
            QCOMPARE(window.findChild<QScrollArea *>("configurationScroll")->horizontalScrollBar()->maximum(), 0);
            window.findChild<QPushButton *>("nav_talents")->click();
            QCoreApplication::processEvents();
            const auto *talent = window.findChild<QWidget *>("talents0")->findChild<QToolButton *>("talentTier4");
            QVERIFY2(talent->visibleRegion().boundingRect().contains(talent->rect().center()),
                     qPrintable(QStringLiteral("Talent at %1,%2 in %3x%4; visible=%5, region=%6x%7")
                                    .arg(talent->pos().x())
                                    .arg(talent->pos().y())
                                    .arg(talent->parentWidget()->width())
                                    .arg(talent->parentWidget()->height())
                                    .arg(talent->isVisible())
                                    .arg(talent->visibleRegion().boundingRect().width())
                                    .arg(talent->visibleRegion().boundingRect().height())));
            QVERIFY(window.grab().save(qEnvironmentVariable("JX3DPS_TEST_SCREENSHOT") + "-compact.png"));
            window.findChild<QComboBox *>("specialization")->setCurrentIndex(1);
            QTest::qWait(30);
            QVERIFY(window.grab().save(qEnvironmentVariable("JX3DPS_TEST_SCREENSHOT") + "-mowen-compact.png"));
        }
        show_analysis->click();
        QVERIFY(window.close());
        QVERIFY(!analysis->isVisible());
    }

    void segmentedPageNavigation()
    {
        desktop::SegmentedTabs tabs;
        tabs.resize(420, 220);
        for (const auto *name : { "First", "Second", "Third" }) {
            tabs.addTab(new QWidget, QString::fromLatin1(name));
        }
        tabs.show();
        QCoreApplication::processEvents();
        auto      *bar   = static_cast<desktop::SegmentedTabBar *>(tabs.tabBar());
        const auto first = bar->IndicatorRect();
        QTest::mouseClick(bar, Qt::LeftButton, Qt::NoModifier, bar->tabRect(1).center());
        QCOMPARE(tabs.currentIndex(), 1);
        QVERIFY(bar->SelectionAnimating());
        QVERIFY(bar->IndicatorRect().left() < bar->tabRect(1).left());
        tabs.setCurrentIndex(2);
        QCOMPARE(tabs.currentIndex(), 2);
        QTRY_VERIFY(!bar->SelectionAnimating());
        QVERIFY(bar->IndicatorRect().center().x() > first.center().x());
        QVERIFY(bar->tabRect(2).contains(bar->IndicatorRect().toRect()));
        tabs.setTabEnabled(1, false);
        QTest::mouseClick(bar, Qt::LeftButton, Qt::NoModifier, bar->tabRect(1).center());
        QCOMPARE(tabs.currentIndex(), 2);
        QTest::keyClick(bar, Qt::Key_Left);
        QCOMPARE(tabs.currentIndex(), 0);
        tabs.hide();
        tabs.setCurrentIndex(2);
        tabs.resize(520, 240);
        tabs.show();
        QCoreApplication::processEvents();
        QVERIFY(!bar->SelectionAnimating());
        QVERIFY(bar->tabRect(2).contains(bar->IndicatorRect().toRect()));
    }

    void workbenchLayoutBalance()
    {
        Workbench window;
        window.resize(1280, 840);
        window.show();
        QCoreApplication::processEvents();
        const auto *editor        = window.findChild<MacroEditor *>("macroEditor");
        const auto *configuration = window.findChild<QScrollArea *>("configurationScroll");
        const auto *profiles      = window.findChild<QWidget *>("configurationSchemes");
        const auto *start         = window.findChild<QToolButton *>("startButton");
        QVERIFY(editor->width() >= 280);
        QCOMPARE(configuration->horizontalScrollBar()->maximum(), 0);
        QCOMPARE(window.findChild<QWidget *>("topNavigation")->height(), desktop::theme::NavigationHeight);
        QVERIFY(profiles->isAncestorOf(start));
        for (const auto *name : { "iterations", "workers", "seconds", "seed", "delay" }) {
            const auto *input = window.findChild<QWidget *>(QString::fromLatin1(name));
            QVERIFY(input);
            QVERIFY2(input->visibleRegion().boundingRect().contains(input->rect().center()), name);
        }
        const auto *attributes = window.findChild<QWidget *>("gameAttributePanel");
        const auto *gains      = window.findChild<QWidget *>("attributeGainsModule");
        window.findChild<SimulationOptions *>()->ShowAttributeGains();
        QVERIFY(attributes->isVisible());
        QVERIFY(gains->isVisible());
        QVERIFY(!attributes->isAncestorOf(gains));
        window.findChild<QToolButton *>("debugButton")->click();
        window.findChild<QAction *>("debugStart")->trigger();
        QCoreApplication::processEvents();
        QCOMPARE(window.size(), QSize(1280, 840));
        QVERIFY(editor->width() >= 280);
        QCOMPARE(configuration->horizontalScrollBar()->maximum(), 0);
        const auto PROGRESS = window.findChild<QProgressBar *>("batchProgress")->mapTo(&window, QPoint(0, 0));
        const auto STATUS   = window.findChild<QLabel *>("simulationResult")->mapTo(&window, QPoint(0, 0));
        QVERIFY(std::abs(PROGRESS.y() - STATUS.y()) <= 8);
        QVERIFY(window.close());
    }

    void floatingDebuggingInMacroEditor()
    {
        Workbench window;
        window.resize(1366, 900);
        window.show();
        auto       *analysis = window.findChild<QDialog *>("resultAnalysis");
        auto       *editor   = window.findChild<MacroEditor *>("macroEditor");
        const auto *macro    = window.findChild<QWidget *>("macroPanel");
        auto       *details  = window.findChild<QTabWidget *>("debugTabs");
        auto       *toggle   = window.findChild<QToolButton *>("debugButton");
        auto       *floating = window.findChild<DebugToolbar *>("macroDebugToolbar");
        const auto *session  = window.findChild<DebugSession *>();
        auto       *play     = floating->findChild<QToolButton *>("debugContinueButton");
        auto       *pause    = floating->findChild<QToolButton *>("debugPauseButton");
        auto       *step     = floating->findChild<QToolButton *>("debugStepOverButton");
        auto       *into     = floating->findChild<QToolButton *>("debugStepIntoButton");
        auto       *restart  = floating->findChild<QToolButton *>("debugRestartButton");
        auto       *stop     = floating->findChild<QToolButton *>("debugStopButton");
        QVERIFY(!window.findChild<QTabWidget *>("workspaceTabs"));
        QVERIFY(!window.findChild<QWidget *>("debugPage"));
        QVERIFY(macro->isAncestorOf(toggle));
        QVERIFY(macro->isAncestorOf(details));
        QVERIFY(floating->isWindow());
        QVERIFY(!floating->isVisible());
        QVERIFY(!details->isVisible());
        editor->setPlainText(QStringLiteral("/cast [qidian>=8] 无我无剑\n/cast 三环套月\n/cast 三柴剑法"));
        editor->SetBreakpoint(1, true);
        const auto SOURCE   = editor->toPlainText();
        auto      *document = editor->document();
        window.findChild<QDoubleSpinBox *>("seconds")->setValue(3);
        toggle->click();
        QTRY_VERIFY(floating->isVisible());
        QVERIFY(details->isVisible());
        QVERIFY(!analysis->isVisible());
        QVERIFY(play->isEnabled());
        QVERIFY(!step->isEnabled());
        play->click();
        QTRY_COMPARE(session->GetState(), DebugSession::PAUSED);
        QCOMPARE(editor->GetCurrentDebugLine(), 1);
        QVERIFY(editor->isReadOnly());
        QVERIFY(step->isEnabled());
        QVERIFY(stop->isEnabled());
        QVERIFY(window.findChild<QLabel *>("debugPosition")->text().contains(QStringLiteral("已暂停")));
        into->click();
        QVERIFY(session->GetDebugInfo().condition.contains(QStringLiteral("qidian")));
        const auto *steps            = window.findChild<MacroTracePanel *>()->findChild<QTableView *>("macroSteps");
        const auto  BEFORE_CONDITION = steps->model()->rowCount();
        window.findChild<QAction *>("debugStepInto")->trigger();
        QCOMPARE(steps->model()->rowCount(), BEFORE_CONDITION + 1);
        step->click();
        const auto PHASE = session->GetDebugInfo().phase;
        const auto LINE  = session->GetDebugInfo().lineNumber;
        QCOMPARE(editor->document(), document);
        QCOMPARE(editor->toPlainText(), SOURCE);

        const auto WITHIN_EDITOR = [&] {
            return QRect(editor->mapToGlobal(QPoint(0, 0)), editor->size()).contains(floating->geometry());
        };
        QTRY_VERIFY(WITHIN_EDITOR());
        const auto   PREVIOUS_POSITION = floating->pos();
        const QPoint HANDLE(20, floating->height() / 2);
        const QPoint DRAG(-12, 60);
        QTest::mousePress(floating, Qt::LeftButton, Qt::NoModifier, HANDLE);
        QMouseEvent movement(QEvent::MouseMove, QPointF(HANDLE + DRAG), QPointF(floating->mapToGlobal(HANDLE) + DRAG), Qt::NoButton, Qt::LeftButton, Qt::NoModifier);
        QApplication::sendEvent(floating, &movement);
        QTest::mouseRelease(floating, Qt::LeftButton, Qt::NoModifier, HANDLE);
        QVERIFY(floating->pos() != PREVIOUS_POSITION);
        QVERIFY(WITHIN_EDITOR());
        const auto RELATIVE_POSITION = window.mapFromGlobal(floating->pos());
        window.move(window.pos() + QPoint(10, 10));
        QCoreApplication::processEvents();
        QCOMPARE(window.mapFromGlobal(floating->pos()), RELATIVE_POSITION);

        const auto screenshot = [&](const QString &suffix) {
            if (!qEnvironmentVariableIsSet("JX3DPS_TEST_SCREENSHOT")) {
                return;
            }
            for (auto *bar : window.findChildren<QTabBar *>()) {
                if (bar->isVisible() && bar->property("segmentedControl").toBool()) {
                    QTRY_VERIFY(!static_cast<desktop::SegmentedTabBar *>(bar)->SelectionAnimating());
                }
            }
            auto     pixmap = window.grab();
            QPainter painter(&pixmap);
            painter.drawPixmap(floating->mapToGlobal(QPoint(0, 0)) - window.mapToGlobal(QPoint(0, 0)), floating->grab());
            painter.end();
            QVERIFY(pixmap.save(qEnvironmentVariable("JX3DPS_TEST_SCREENSHOT") + suffix));
        };
        screenshot(QStringLiteral("-floating-debug.png"));
        details->setCurrentWidget(window.findChild<MacroTracePanel *>());
        QCoreApplication::processEvents();
        QVERIFY(steps->viewport()->visibleRegion().boundingRect().contains(steps->visualRect(steps->model()->index(0, 0))));
        screenshot(QStringLiteral("-floating-steps.png"));
        window.resize(1280, 840);
        QCoreApplication::processEvents();
        QCOMPARE(window.size(), QSize(1280, 840));
        QTRY_VERIFY(WITHIN_EDITOR());
        screenshot(QStringLiteral("-floating-compact.png"));
        window.findChild<QPushButton *>("showAnalysis")->click();
        QTRY_VERIFY(analysis->isVisible());
        QVERIFY(!floating->isVisible());
        QVERIFY(toggle->isChecked());
        QCOMPARE(session->GetState(), DebugSession::PAUSED);
        analysis->close();
        QTRY_VERIFY(floating->isVisible());
        QCOMPARE(session->GetDebugInfo().phase, PHASE);
        QCOMPARE(session->GetDebugInfo().lineNumber, LINE);
        play->click();
        QCOMPARE(session->GetState(), DebugSession::RUNNING);
        QVERIFY(pause->isVisible());
        QVERIFY(!step->isEnabled());
        pause->click();
        QCOMPARE(session->GetState(), DebugSession::PAUSED);
        restart->click();
        QCOMPARE(session->GetState(), DebugSession::PAUSED);
        QCOMPARE(session->GetDebugInfo().currentFrame, 0);
        toggle->click();
        QCOMPARE(session->GetState(), DebugSession::STOPPED);
        QVERIFY(!editor->isReadOnly());
        QVERIFY(!floating->isVisible());
        QVERIFY(!details->isVisible());
        QVERIFY(window.findChild<QComboBox *>("specialization")->isEnabled());
        editor->ClearAllBreakpoints();
        toggle->click();
        play->click();
        QTRY_COMPARE(session->GetState(), DebugSession::FINISHED);
        QCOMPARE(floating->GetDebugState(), DebugToolbar::FINISHED);
        QVERIFY(!step->isEnabled());
        QVERIFY(restart->isEnabled());
        QVERIFY(stop->isEnabled());
        stop->click();
        QCOMPARE(session->GetState(), DebugSession::STOPPED);
        QVERIFY(window.close());
        QVERIFY(!floating->isVisible());
    }

    void attributeGainBatch()
    {
        const auto             COMPILED = rt::MacroCompiler::Compile("/cast gong", desktop::MacroOptions(desktop::Specialization::MO_WEN));
        const rt::BatchOptions OPTIONS{ 12, 9, 16 * 5, 2, 2000, 4000 };
        auto config = JX3DPS::engine::Scenario("mw");
        BatchWorker            worker(COMPILED.first.program, OPTIONS, config, nullptr, false, true);
        QSignalSpy             stats(&worker, &BatchWorker::StatisticsReady);
        QSignalSpy             failure(&worker, &BatchWorker::Failed);
        worker.start();
        QVERIFY(worker.wait(10000));
        QTRY_COMPARE(stats.count(), 1);
        QCOMPARE(failure.count(), 0);
        const auto result = qvariant_cast<SimulationResult>(stats[0][0]);
        QCOMPARE(result.gains.size(), 8);
        QVERIFY(result.skillsCollected);
        QVERIFY(!result.skills.isEmpty());
        qint64 skill_damage = 0;
        for (const auto &skill : result.skills) {
            skill_damage += skill.damage;
        }
        QVERIFY(std::abs(skill_damage - result.stats.mean_damage * result.stats.count) < .01);
        QCOMPARE(worker.Completed(), worker.Requested());
        QCOMPARE(worker.Requested(), OPTIONS.iterations * rt::GainSimulationStages(config));
        config.attributes.attack_base += 100;
        const auto direct = rt::RunBatch(COMPILED.first.program, OPTIONS, desktop::MakeRules(config));
        QCOMPARE(result.gains[0].stats.checksum, direct.checksum);
        ResultsPanel panel;
        panel.AddResult(result, QStringLiteral("/cast gong"), {});
        QCOMPARE(panel.findChild<QTableWidget *>("attributeGainsTable")->rowCount(), 10);
    }

    void attributeGainReplayUsesCommonPipeline()
    {
        const rt::BatchOptions OPTIONS{ 8, 19, 16 * 4, 2, 2000, 4000 };
        const auto             CHECK = [&](const auto &config, const auto &compiled) {
            for (const auto MODE : { desktop::GainComparisonMode::SAME_VALUE, desktop::GainComparisonMode::SAME_COST })
            {
                const auto INCREMENTS =
                    desktop::GainComparisonIncrements(MODE);
                BatchWorker worker(compiled.first.program, OPTIONS, config, nullptr, false, true, MODE);
                QSignalSpy  stats(&worker, &BatchWorker::StatisticsReady);
                QSignalSpy  failure(&worker, &BatchWorker::Failed);
                worker.start();
                QVERIFY(worker.wait(10000));
                QTRY_COMPARE(stats.count(), 1);
                QCOMPARE(failure.count(), 0);
                const auto result = qvariant_cast<SimulationResult>(stats[0][0]);
                QCOMPARE(result.gainMode, MODE);
                QCOMPARE(result.gains.size(), 8);
                QCOMPARE(worker.Completed(), worker.Requested());
                QCOMPARE(worker.Requested(), OPTIONS.iterations * rt::GainSimulationStages(config));
                for (int index = 0; index < 8; ++index) {
                    QCOMPARE(result.gains[index].increment, INCREMENTS[index]);
                    const auto CHANGED = desktop::ApplyAttributeGain(config, index, INCREMENTS[index]);
                    const auto direct  = rt::RunBatch(compiled.first.program, OPTIONS, desktop::MakeRules(CHANGED));
                    QCOMPARE(result.gains[index].stats.count, direct.count);
                    QCOMPARE(result.gains[index].stats.checksum, direct.checksum);
                    QCOMPARE(result.gains[index].stats.mean_damage, direct.mean_damage);
                }
            }
        };

        for (auto code : {"mw", "tx"}) {
            const auto config = JX3DPS::engine::Scenario(code);
            CHECK(config, rt::MacroCompiler::Compile(config.book->default_macro, rt::CatalogMacroOptions(config.book->Catalog(), false)));
        }
    }

    void chineseWorkbenchAndMoWenCompletion()
    {
        Workbench window;
        QCOMPARE(window.findChild<QAction *>("newMacro")->text(), QStringLiteral("新建"));
        QCOMPARE(window.findChild<QAction *>("attributeGains")->text(), QStringLiteral("自动收益"));
        auto *selection = window.findChild<QComboBox *>("specialization");
        auto *editor    = window.findChild<MacroEditor *>("macroEditor");
        selection->setCurrentIndex(1);
        editor->setPlainText(QStringLiteral("/cast 无我无剑"));
        window.findChild<QAction *>("checkMacro")->trigger();
        const auto *problems = window.findChild<QTableWidget *>("macroProblems");
        QCOMPARE(problems->rowCount(), 1);
        QVERIFY(problems->item(0, 1)->text().contains(QStringLiteral("当前心法不支持")));
        window.show();
        editor->setPlainText(QStringLiteral("/cast "));
        auto cursor = editor->textCursor();
        cursor.movePosition(QTextCursor::End);
        editor->setTextCursor(cursor);
        editor->Complete();
        const auto *completer = editor->findChild<QCompleter *>("macroCompleter");
        QStringList candidates;
        for (int row = 0; row < completer->completionModel()->rowCount(); ++row) {
            candidates << completer->completionModel()->index(row, 0).data(Qt::UserRole).toString();
        }
        QVERIFY(candidates.contains(QStringLiteral("宫")));
        QVERIFY(candidates.contains(QStringLiteral("徵")));
        QVERIFY(!candidates.contains(QStringLiteral("无我无剑")));
        QTest::keyClick(editor, Qt::Key_Escape);
        selection->setCurrentIndex(0);
        editor->setPlainText(QStringLiteral("/cast [buff:碎星辰] 无我无剑\n/cast [buff:碎星辰>0] 无我无剑"));
        window.findChild<QAction *>("checkMacro")->trigger();
        QCOMPARE(problems->rowCount(), 0);
    }

    void debuggerMatchesTrace()
    {
        const QString  text = QStringLiteral("/scast 使用风腰坠\n/cast [buff:风·灭虚>0&qidian>5] wuwu\n/cast sanhuan");
        DebugSimulator debugger;
        auto config = JX3DPS::engine::Scenario("tx");
        config.equipment = {std::string(rt::season50::WindPendant)};
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
        CombatEvents events     = debugger.TakeEvents();
        BuffEvents   buffEvents = debugger.TakeBuffEvents();
        BuffTrace    debug_buffs;
        debug_buffs.Reset(4800);
        debug_buffs.Append(buffEvents);
        debug_buffs.Observe(debugger.GetPlayerState().currentFrame);
        for (int i = 0; i < 1000 && !debugger.IsFinished(); ++i) {
            debugger.Continue(2000);
            events             += debugger.TakeEvents();
            const auto CHANGES  = debugger.TakeBuffEvents();
            buffEvents         += CHANGES;
            debug_buffs.Append(CHANGES);
            debug_buffs.Observe(debugger.GetPlayerState().currentFrame);
        }
        QVERIFY(debugger.IsFinished());
        QCOMPARE(debugger.GetPlayerState().currentMacroLine, -1);
        QVERIFY(debugger.TakeEvents().isEmpty());
        QVERIFY(debugger.TakeBuffEvents().isEmpty());
        const auto BYTES = text.toUtf8();
        const auto COMPILED =
            rt::MacroCompiler::Compile(std::string_view(BYTES.constData(), BYTES.size()), desktop::MacroOptions(desktop::Specialization::TAI_XU));
        QVERIFY(COMPILED.second.empty());
        rt::Simulation<rt::season50::Rules> reference(COMPILED.first.program, desktop::MakeRules(config), (4800 * 8) + 64, (4800 * 24) + 128);
        reference.Start(4800, rt::IterationSeed(77, 0));
        const auto initial_buffs = InitialBuffEvents(reference.GetState(), desktop::Specialization::TAI_XU);
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
        std::size_t mutation_cursor = 0;
        const auto  REFERENCE_BUFFS =
            initial_buffs + ReadBuffEvents(reference.Log().Mutations(), mutation_cursor, desktop::Specialization::TAI_XU);
        QCOMPARE(buffEvents, REFERENCE_BUFFS);
        QCOMPARE(mutation_cursor, reference.Log().Mutations().size());
        QVERIFY(ReadBuffEvents(reference.Log().Mutations(), mutation_cursor, desktop::Specialization::TAI_XU).isEmpty());
        QVERIFY(std::any_of(buffEvents.begin(), buffEvents.end(), [](const BuffEvent &buff) { return buff.target; }));
        BuffTrace full_buffs;
        full_buffs.Reset(4800);
        full_buffs.Append(REFERENCE_BUFFS);
        full_buffs.Observe(4800);
        const auto FULL_COVERAGE  = full_buffs.Coverage();
        const auto DEBUG_COVERAGE = debug_buffs.Coverage();
        QCOMPARE(DEBUG_COVERAGE.size(), FULL_COVERAGE.size());
        for (int i = 0; i < FULL_COVERAGE.size(); ++i) {
            QCOMPARE(DEBUG_COVERAGE[i].activeFrames, FULL_COVERAGE[i].activeFrames);
            QCOMPARE(DEBUG_COVERAGE[i].stackFrames, FULL_COVERAGE[i].stackFrames);
            QCOMPARE(DEBUG_COVERAGE[i].intervals, FULL_COVERAGE[i].intervals);
        }
        rt::BatchOptions trace_options;
        trace_options.duration          = 4800;
        trace_options.seed              = 77;
        trace_options.damage_capacity   = (4800 * 8) + 64;
        trace_options.mutation_capacity = (4800 * 24) + 128;
        BatchWorker      trace_worker(COMPILED.first.program, trace_options, config, nullptr, true);
        const QSignalSpy TRACE_BUFFS(&trace_worker, &BatchWorker::BuffTraceReady);
        const QSignalSpy TRACE_FAILED(&trace_worker, &BatchWorker::Failed);
        trace_worker.start();
        QVERIFY(trace_worker.wait(10000));
        QTRY_COMPARE(TRACE_BUFFS.count(), 1);
        QCOMPARE(TRACE_FAILED.count(), 0);
        QCOMPARE(qvariant_cast<BuffEvents>(TRACE_BUFFS.at(0).at(0)), buffEvents);
        debugger.Reset();
        QVERIFY(debugger.TakeBuffEvents().isEmpty());
    }

    void macroCompletion()
    {
        MacroEditor editor;
        editor.SetSpecialization(desktop::Specialization::TAI_XU);
        editor.resize(900, 300);
        editor.show();
        editor.setFocus();
        auto *completer = editor.findChild<QCompleter *>("macroCompleter");
        QVERIFY(completer);
        const auto input = [&](const QString &text, int position = -1) {
            editor.setPlainText(text);
            auto cursor = editor.textCursor();
            cursor.setPosition(position < 0 ? text.size() : position);
            editor.setTextCursor(cursor);
            editor.Complete();
        };
        auto candidates = [&] {
            QStringList values;
            for (int row = 0; row < completer->completionModel()->rowCount(); ++row) {
                values << completer->completionModel()->index(row, 0).data(Qt::UserRole).toString();
            }
            return values;
        };
        const auto accept = [&](const QString &value) {
            const int ROW = candidates().indexOf(value);
            if (ROW < 0) {
                return false;
            }
            const auto INDEX = completer->completionModel()->index(ROW, 0);
            return QMetaObject::invokeMethod(completer, "activated", Qt::DirectConnection, Q_ARG(QModelIndex, INDEX));
        };
        input("/s");
        QCOMPARE(candidates(), (QStringList{ "/scast", "/sfcast" }));
        QVERIFY(accept("/scast"));
        QCOMPARE(editor.toPlainText(), QStringLiteral("/scast"));
        input("/cast ");
        QVERIFY(candidates().size() >= static_cast<qsizetype>(desktop::School(desktop::Specialization::TAI_XU).castable.size()));
        QVERIFY(!candidates().contains(QStringLiteral("pozhao")));
        for (const auto &candidate : candidates()) {
            const auto BYTES = (QStringLiteral("/cast ") + candidate).toUtf8();
            QVERIFY(rt::MacroCompiler::Compile(std::string_view(BYTES.data(), BYTES.size()), desktop::MacroOptions(desktop::Specialization::TAI_XU)).second.empty());
        }
        input("/cast [buff:");
        QCOMPARE(candidates().size(), static_cast<qsizetype>(desktop::School(desktop::Specialization::TAI_XU).buffs.size()));
        QVERIFY(candidates().contains(QStringLiteral("碎星辰")));
        QVERIFY(!candidates().contains(QStringLiteral("wuwu")));
        input("/cast [qidian>=6&tbuff:叠>=2] wuwu", QStringLiteral("/cast [qidian>=6&tbuff:叠").size());
        QCOMPARE(candidates(), (QStringList{ "叠刃" }));
        QVERIFY(accept("叠刃"));
        QCOMPARE(editor.toPlainText(), QStringLiteral("/cast [qidian>=6&tbuff:叠刃>=2] wuwu"));
        input("/cast [buff:碎星辰] wuwu", QStringLiteral("/cast [buff:碎").size());
        QVERIFY(accept("碎星辰"));
        QCOMPARE(editor.toPlainText(), QStringLiteral("/cast [buff:碎星辰] wuwu"));
        input("/cast [buf:碎星辰] wuwu", QStringLiteral("/cast [buf").size());
        QVERIFY(accept("buff:"));
        QCOMPARE(editor.toPlainText(), QStringLiteral("/cast [buff:碎星辰] wuwu"));
        input("/cast [last_skill:wu] sanhuan", QStringLiteral("/cast [last_skill:wu").size());
        QVERIFY(accept("wuwu"));
        QCOMPARE(editor.toPlainText(), QStringLiteral("/cast [last_skill:wuwu] sanhuan"));
        input("/cast [qidian>=6] wuwu", QStringLiteral("/cast [qidian>=6] wu").size());
        QVERIFY(accept("wuwu"));
        QCOMPARE(editor.toPlainText(), QStringLiteral("/cast [qidian>=6] wuwu"));

        editor.setPlainText(QStringLiteral("/cast [buff:"));
        editor.moveCursor(QTextCursor::End);
        QInputMethodEvent ime;
        ime.setCommitString(QStringLiteral("碎"));
        QCoreApplication::sendEvent(&editor, &ime);
        QVERIFY(completer->popup()->isVisible());
        QCOMPARE(candidates(), (QStringList{ QStringLiteral("碎星辰") }));
        QVERIFY(accept(QStringLiteral("碎星辰")));
        QCOMPARE(editor.toPlainText(), QStringLiteral("/cast [buff:碎星辰"));
        editor.undo();
        QCOMPARE(editor.toPlainText(), QStringLiteral("/cast [buff:碎"));
        for (const auto &text : { "# /ca", "// /ca", "/switch ", "/cast [qidian>=3", "/cast [buff:碎星辰>2" }) {
            input(QString::fromUtf8(text));
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
        QCOMPARE(candidates(), (QStringList{ "sanhuan", "sanchai" }));
        QTest::keyClicks(&editor, "huan");
        QVERIFY(!completer->popup()->isVisible());
        QTest::keyClick(&editor, Qt::Key_Return);
        QCOMPARE(editor.toPlainText(), QStringLiteral("/cast sanhuan\n"));
        input("/cast wu");
        QTest::keyClick(&editor, Qt::Key_Escape);
        QVERIFY(!completer->popup()->isVisible());
        QCOMPARE(editor.toPlainText(), QStringLiteral("/cast wu"));
        input("/cast wu");
        const auto STALE = completer->completionModel()->index(0, 0);
        editor.moveCursor(QTextCursor::Start);
        QVERIFY(!completer->popup()->isVisible());
        QVERIFY(QMetaObject::invokeMethod(completer, "activated", Qt::DirectConnection, Q_ARG(QModelIndex, STALE)));
        QCOMPARE(editor.toPlainText(), QStringLiteral("/cast wu"));
        input("/cast wu");
        const auto LOCKED = completer->completionModel()->index(0, 0);
        editor.setReadOnly(true);
        QVERIFY(!completer->popup()->isVisible());
        QVERIFY(QMetaObject::invokeMethod(completer, "activated", Qt::DirectConnection, Q_ARG(QModelIndex, LOCKED)));
        editor.Complete();
        QCOMPARE(editor.toPlainText(), QStringLiteral("/cast wu"));
        QVERIFY(!completer->popup()->isVisible());
    }

    void macroSearchAndReplace()
    {
        QWidget        host;
        CodeEditor     editor(&host);
        MacroSearchBar search(&editor, &host);
        host.show();
        auto         *query       = search.findChild<QLineEdit *>("macroSearchQuery");
        auto         *replacement = search.findChild<QLineEdit *>("macroReplacement");
        auto         *words       = search.findChild<QCheckBox *>("macroSearchWords");
        auto         *match_case  = search.findChild<QCheckBox *>("macroSearchCase");
        auto         *all         = search.findChild<QPushButton *>("macroReplaceAll");
        auto         *one         = search.findChild<QPushButton *>("macroReplaceOne");
        const auto   *status      = search.findChild<QLabel *>("macroSearchStatus");
        const QString SOURCE      = QStringLiteral("/cast wuwu\n/cast WUWU\n/cast wuwu_extra\n# buff:玄门\n/cast wuwu");
        editor.setPlainText(SOURCE);
        editor.SetBreakpoint(2, true);
        editor.SetBreakpoint(5, true);
        search.Open(true);
        query->setText(QStringLiteral("wuwu"));
        QVERIFY(status->text().contains(QStringLiteral("4 处匹配")));
        words->setChecked(true);
        QVERIFY(status->text().contains(QStringLiteral("3 处匹配")));
        match_case->setChecked(true);
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
        QCOMPARE(editor.toPlainText(), QStringLiteral("/cast sanhuan\n/cast WUWU\n/cast wuwu_extra\n# buff:玄门\n/cast sanhuan"));
        QVERIFY(editor.HasBreakpoint(2) && editor.HasBreakpoint(5));
        QVERIFY(status->text().contains(QStringLiteral("已替换 2")));
        editor.undo();
        QCOMPARE(editor.toPlainText(), SOURCE);
        QVERIFY(editor.HasBreakpoint(2) && editor.HasBreakpoint(5));
        editor.redo();
        QVERIFY(editor.toPlainText().startsWith(QStringLiteral("/cast sanhuan")));
        editor.undo();
        query->setText(QStringLiteral("玄门"));
        replacement->clear();
        search.Find();
        one->click();
        QVERIFY(!editor.toPlainText().contains(QStringLiteral("玄门")));
        editor.undo();
        QCOMPARE(editor.toPlainText(), SOURCE);
        query->setText(QStringLiteral("wuwu"));
        replacement->setText(QStringLiteral("wuwuwuwu"));
        all->click();
        QVERIFY(status->text().contains(QStringLiteral("已替换 2")));
        editor.undo();
        QCOMPARE(editor.toPlainText(), SOURCE);
        auto cursor = editor.textCursor();
        cursor.clearSelection();
        cursor.movePosition(QTextCursor::Start);
        editor.setTextCursor(cursor);
        one->click(); // A non-matching selection first locates, rather than replaces.
        QCOMPARE(editor.toPlainText(), SOURCE);
        QCOMPARE(editor.textCursor().selectedText(), QStringLiteral("wuwu"));
        editor.setReadOnly(true);
        QVERIFY(!one->isEnabled() && !all->isEnabled() && !replacement->isEnabled());
        QVERIFY(status->text().contains(QStringLiteral("锁定")));
        search.Find();
        QCOMPARE(editor.textCursor().blockNumber(), 4);
        all->click();
        QTest::keyClick(replacement, Qt::Key_Return);
        QCOMPARE(editor.toPlainText(), SOURCE);
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
        editor.setPlainText(QStringLiteral("renjian renjian_suixing 人剑合一·DOT 人剑合一"));
        search.Open();
        query->setText(QStringLiteral("renjian"));
        QVERIFY(status->text().contains(QStringLiteral("1 处匹配")));
        query->setText(QStringLiteral("人剑合一"));
        QVERIFY(status->text().contains(QStringLiteral("1 处匹配")));
        editor.setPlainText(QStringLiteral("wuwu ").repeated(1100));
        query->setText(QStringLiteral("wuwu"));
        QVERIFY(status->text().contains(QStringLiteral("1100 处匹配")));
        QVERIFY(status->text().contains(QStringLiteral("1000 处")));
        QVERIFY(editor.extraSelections().size() <= 1002);
    }

    void builtinMacroCheckerAndCustomValidator()
    {
        CodeEditor editor;
        editor.setPlainText(QStringLiteral("/unknown"));
        editor.SetSyntaxCheckEnabled(true);
        QCOMPARE(editor.GetSyntaxErrors().size(), 1);
        QCOMPARE(editor.GetSyntaxErrors().front().severity, SyntaxError::SYNTAX_ERROR);

        editor.SetSyntaxType(CodeEditor::J_X3_MACRO);
        editor.SetSyntaxValidator([](const QString &source) {
            return QList<SyntaxError>{ SyntaxError(1, 0, source.size(), SyntaxError::WARNING, QStringLiteral("custom")) };
        });
        QCOMPARE(editor.GetSyntaxErrors().front().message, QStringLiteral("custom"));
        editor.SetSyntaxType(CodeEditor::CPP);
        QCOMPARE(editor.GetSyntaxErrors().front().severity, SyntaxError::WARNING);
        editor.SetSyntaxValidator({});
        editor.SetSyntaxCheckEnabled(false);
        editor.SetSyntaxCheckEnabled(true);
        QVERIFY(editor.GetSyntaxErrors().isEmpty());
        editor.SetSyntaxType(CodeEditor::J_X3_MACRO);
        QCOMPARE(editor.GetSyntaxErrors().size(), 1);
        QCOMPARE(editor.GetSyntaxErrors().front().severity, SyntaxError::SYNTAX_ERROR);
    }

    void workbenchEditorTools()
    {
        Workbench window;
        window.show();
        auto         *editor      = window.findChild<MacroEditor *>("macroEditor");
        auto         *search      = window.findChild<MacroSearchBar *>();
        auto         *session     = window.findChild<DebugSession *>();
        auto         *query       = search->findChild<QLineEdit *>("macroSearchQuery");
        auto         *replacement = search->findChild<QLineEdit *>("macroReplacement");
        auto         *all         = search->findChild<QPushButton *>("macroReplaceAll");
        const QString SOURCE = QStringLiteral("# Tai Xu macro\n/cast [buff:碎星辰&qidian>=6] wuwu\n/cast [qidian>=8] "
                                              "wuwu\n/cast bahuang\n/cast sanhuan");
        editor->setPlainText(SOURCE);
        editor->SetBreakpoint(2, true);
        window.findChild<QAction *>("replaceMacro")->trigger();
        QVERIFY(search->isVisible());
        query->setText(QStringLiteral("wuwu"));
        replacement->setText(QStringLiteral("无我无剑"));
        search->Find();
        QCoreApplication::processEvents();
        if (qEnvironmentVariableIsSet("JX3DPS_TEST_SCREENSHOT")) {
            QVERIFY(window.grab().save(qEnvironmentVariable("JX3DPS_TEST_SCREENSHOT") +
                                       QStringLiteral("-macro-search.png")));
        }
        window.findChild<QToolButton *>("debugButton")->setChecked(true);
        emit window.findChild<SimulationToolbar *>()->StartSimulationClicked();
        QCOMPARE(session->GetState(), DebugSession::PAUSED);
        QVERIFY(!all->isEnabled());
        QVERIFY(!window.findChild<QAction *>("completeMacro")->isEnabled());
        search->Find();
        QVERIFY(editor->textCursor().hasSelection());
        QCOMPARE(editor->GetCurrentDebugLine(), 2);
        session->Stop();
        QVERIFY(all->isEnabled());
        all->click();
        QVERIFY(editor->toPlainText().contains(QStringLiteral("无我无剑")));
        editor->undo();
        QCOMPARE(editor->toPlainText(), SOURCE);
        QVERIFY(editor->HasBreakpoint(2));
        search->Close();
        editor->setPlainText(
            QStringLiteral("# Complete a buff name\n/cast [buff:碎] wuwu\n/cast bahuang\n/cast sanhuan"));
        auto cursor = editor->textCursor();
        cursor.setPosition(editor->toPlainText().indexOf(QStringLiteral("buff:碎")) + QStringLiteral("buff:碎").size());
        editor->setTextCursor(cursor);
        QCoreApplication::processEvents();
        window.findChild<QAction *>("completeMacro")->trigger();
        const auto *completer = editor->findChild<QCompleter *>("macroCompleter");
        QVERIFY(completer->popup()->isVisible());
        QCoreApplication::processEvents();
        QVERIFY(completer->popup()->isVisible());
        const auto caret        = editor->viewport()->mapToGlobal(editor->cursorRect().bottomLeft());
        const auto POPUP_ORIGIN = completer->popup()->mapToGlobal(QPoint{});
        const auto AVAILABLE    = editor->screen()->availableGeometry();
        const auto POPUP_X =
            qBound(AVAILABLE.left(), caret.x(), qMax(AVAILABLE.left(), AVAILABLE.right() - completer->popup()->width() + 1));
        QVERIFY(std::abs(POPUP_ORIGIN.x() - POPUP_X) < 10);
        QVERIFY(std::abs(POPUP_ORIGIN.y() - caret.y()) < 10);
        if (qEnvironmentVariableIsSet("JX3DPS_TEST_SCREENSHOT")) {
            auto     screenshot = window.grab();
            QPainter painter(&screenshot);
            painter.drawPixmap(completer->popup()->mapToGlobal(QPoint{}) - window.mapToGlobal(QPoint{}),
                               completer->popup()->grab());
            painter.end();
            QVERIFY(screenshot.save(qEnvironmentVariable("JX3DPS_TEST_SCREENSHOT") +
                                    QStringLiteral("-macro-completion.png")));
        }
        completer->popup()->hide();
        QVERIFY(window.close());
    }

    void macroStepObservations()
    {
        DebugSimulator debugger;
        QString        error;
        const QString  SOURCE =
            QStringLiteral("# source mapping\n/cast [qidian<1&energy>0|qidian>=6] wuwu\n/cast sanhuan");
        debugger.SetOptions(JX3DPS::engine::Scenario("tx"), 16000, 7);
        QVERIFY2(debugger.Initialize(SOURCE, error), qPrintable(error));
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
        const auto EXECUTED = debugger.GetPlayerState().lastStep;
        QVERIFY(!EXECUTED.isEmpty());
        QVERIFY(debugger.TakeMacroSteps().isEmpty());
        QCOMPARE(debugger.GetPlayerState().lastStep, EXECUTED);
        debugger.StepOver();
        steps = debugger.TakeMacroSteps();
        QCOMPARE(steps.size(), 2);
        QCOMPARE(steps[0].expression, QStringLiteral("qidian>=6"));
        QCOMPARE(steps[1].kind, MacroStepKind::ACTION);
        QCOMPARE(steps[1].outcome, QStringLiteral("成功"));
        QVERIFY(debugger.GetPlayerState().lastStep.contains(QStringLiteral("成功")));
        debugger.SetBreakpoint(2, false);
        debugger.Continue(1000000);
        QVERIFY(debugger.IsFinished());
        steps = debugger.TakeMacroSteps();
        QCOMPARE(steps.size(), MacroStepHistoryLimit);
        QVERIFY(steps.front().ordinal > 1);
        for (int i = 1; i < steps.size(); ++i) {
            QCOMPARE(steps[i].ordinal, steps[i - 1].ordinal + 1);
        }
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
            step.ordinal    = i;
            step.frame      = i;
            step.line       = 2;
            step.kind       = ((i % 2) != 0) ? MacroStepKind::ACTION : MacroStepKind::CONDITION;
            step.passed     = i % 3 == 0;
            step.expression = QStringLiteral("/cast sanhuan");
            step.outcome    = step.passed ? QStringLiteral("成功") : QStringLiteral("受阻");
            step.details    = QStringLiteral("distance, \"range\" > 4");
            steps.append(step);
        }
        panel.Append(steps);
        auto       *table  = panel.findChild<QTableView *>("macroSteps");
        auto       *scope  = panel.findChild<QComboBox *>("macroStepScope");
        auto       *search = panel.findChild<QLineEdit *>("macroStepSearch");
        const auto *status = panel.findChild<QLabel *>("macroStepStatus");
        auto       *locate = panel.findChild<QPushButton *>("locateMacroStep");
        QCOMPARE(table->model()->rowCount(), MacroStepHistoryLimit);
        QCOMPARE(table->model()->index(0, 0).data().toULongLong(), quint64(953));
        QVERIFY(status->text().contains(QStringLiteral("已淘汰 952 条旧记录")));
        scope->setCurrentIndex(3);
        QCOMPARE(table->model()->rowCount(), MacroStepHistoryLimit / 2);
        for (int i = 0; i < table->model()->rowCount(); ++i) {
            QCOMPARE(table->model()->index(i, 3).data().toString(), QStringLiteral("动作"));
        }
        scope->setCurrentIndex(1);
        for (int i = 0; i < table->model()->rowCount(); ++i) {
            QCOMPARE(table->model()->index(i, 5).data().toString(), QStringLiteral("受阻"));
        }
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
        const QTemporaryDir DIRECTORY;
        QString             error;
        const auto          PATH = DIRECTORY.filePath(QStringLiteral("steps.csv"));
        QVERIFY2(panel.ExportCsv(PATH, error), qPrintable(error));
        QFile file(PATH);
        QVERIFY(file.open(QIODevice::ReadOnly));
        const auto BYTES = file.readAll();
        QCOMPARE(BYTES.count('\n'), table->model()->rowCount() + 1);
        QVERIFY(BYTES.contains("distance, \"\"range\"\" > 4"));
        QVERIFY(!BYTES.contains("\"Accepted\""));
        scope->setCurrentIndex(0);
        search->clear();
        panel.findChild<QCheckBox *>()->setChecked(false);
        table->selectRow(100);
        const auto SELECTED_ORDINAL = table->model()->index(table->currentIndex().row(), 0).data().toULongLong();
        auto       next             = steps.back();
        next.ordinal                = 3001;
        panel.Append({ next });
        QCOMPARE(table->model()->rowCount(), MacroStepHistoryLimit);
        QCOMPARE(table->model()->index(table->currentIndex().row(), 0).data().toULongLong(), SELECTED_ORDINAL);
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
        auto       *editor    = window.findChild<CodeEditor *>("macroEditor");
        auto       *session   = window.findChild<DebugSession *>();
        auto       *toolbar   = window.findChild<SimulationToolbar *>();
        auto       *panel     = window.findChild<MacroTracePanel *>();
        auto       *table     = panel->findChild<QTableView *>("macroSteps");
        const auto *inspector = window.findChild<QTextBrowser *>("debugInspector");
        editor->setPlainText(QStringLiteral("# Explain conditions and blocked casts\n/cast "
                                            "[qidian<1&energy>0|qidian>=6] wuwu\n/cast bahuang\n/cast sanhuan"));
        window.findChild<QToolButton *>("debugButton")->setChecked(true);
        emit toolbar->StartSimulationClicked();
        QCOMPARE(session->GetState(), DebugSession::PAUSED);
        QCOMPARE(table->model()->rowCount(), 0);
        session->StepOver();
        QCOMPARE(table->model()->rowCount(), 3);
        session->StepOut();
        QVERIFY(table->model()->rowCount() >= 6);
        QVERIFY(inspector->toPlainText().contains(QStringLiteral("上一步执行结果")));
        QVERIFY(inspector->toPlainText().contains(QStringLiteral("公共冷却")));
        window.findChild<QTabWidget *>("debugTabs")->setCurrentWidget(panel);
        table->selectRow(0);
        panel->findChild<QPushButton *>("locateMacroStep")->click();
        QCOMPARE(editor->textCursor().blockNumber(), 1);
        table->selectRow(table->model()->rowCount() - 1);
        QCoreApplication::processEvents();
        if (qEnvironmentVariableIsSet("JX3DPS_TEST_SCREENSHOT")) {
            QVERIFY(window.grab().save(qEnvironmentVariable("JX3DPS_TEST_SCREENSHOT") +
                                       QStringLiteral("-macro-steps.png")));
        }
        session->Stop();
        QVERIFY(table->model()->rowCount() > 0);
        editor->setPlainText(QStringLiteral("# edited\n/cast sanhuan"));
        const auto CURSOR = editor->textCursor().position();
        panel->findChild<QPushButton *>("locateMacroStep")->click();
        QCOMPARE(editor->textCursor().position(), CURSOR);
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
        const BuffEvents INITIAL{
            { 0, 0, 0, false, "self", 1, 64, 0, true },
            { 0, 0, 0, true, "target", 3, 32, 16 }
        };
        trace.Append(INITIAL);
        trace.Observe(0);
        QCOMPARE(trace.Coverage()[0].activeFrames, qint64(0));
        trace.Observe(16);
        QCOMPARE(trace.Coverage()[0].activeFrames, qint64(16));
        trace.Append({
            { 16, 1, 0, false, "self", 3, 64, 0  },
            { 16, 2, 0, false, "self", 2, 80, 0  },
            { 32, 3, 0, false, "self", 2, 96, 0  },
            { 48, 4, 0, false, "self", 2, 96, 64 }
        });
        trace.Observe(64);
        auto coverage = trace.Coverage();
        QCOMPARE(coverage[0].activeFrames, qint64(64));
        QCOMPARE(coverage[0].stackFrames, qint64(112));
        QCOMPARE(coverage[0].peakStacks, 3);
        QCOMPARE(coverage[0].changes, 4);
        QCOMPARE(coverage[0].intervals,
                 QVector<BuffInterval>({
                     { 0,  16, 1 },
                     { 16, 64, 2 }
        }));
        QCOMPARE(coverage[1].activeFrames, qint64(32));
        QCOMPARE(coverage[1].stackFrames, qint64(96));
        QCOMPARE(trace.Changes()[0].kind, QStringLiteral("初始"));
        QCOMPARE(trace.Changes()[4].kind, QStringLiteral("刷新"));
        QCOMPARE(trace.Changes()[5].kind, QStringLiteral("计时更新"));
        trace.Append({
            { 64, 5, 0, false, "self",   0, 0,   0  },
            { 64, 6, 0, true,  "target", 1, 200, 80 }
        });
        trace.Observe(100);
        coverage = trace.Coverage();
        QCOMPARE(coverage[0].activeFrames, qint64(64));
        QCOMPARE(coverage[0].latest.stacks, 0);
        QCOMPARE(coverage[1].activeFrames, qint64(68));
        QCOMPARE(coverage[1].stackFrames, qint64(132));
        QCOMPARE(coverage[1].intervals,
                 QVector<BuffInterval>({
                     { 0,  32,  3 },
                     { 64, 100, 1 }
        }));
        trace.Observe(200);
        QCOMPARE(trace.ObservedFrame(), 100);
        QCOMPARE(trace.Coverage()[1].activeFrames, qint64(68));

        rt::State state;
        const auto catalog = desktop::Describe(desktop::Specialization::TAI_XU);
        state.Resize(catalog.skills.size(), catalog.buffs.size());
        const auto options = rt::CatalogMacroOptions(catalog, false);
        const auto field = options.resolve_buff("碎星辰"), dot = options.resolve_buff("叠刃");
        state.self_buff_stacks[field]        = 1;
        state.self_buff_expires_at[field]    = 160;
        state.target_buff_stacks[dot]     = 3;
        state.target_buff_expires_at[dot] = 384;
        const auto STARTING                       = InitialBuffEvents(state);
        QCOMPARE(STARTING.size(), 2);
        QVERIFY(STARTING[0].initial && STARTING[1].initial);
        QVERIFY(STARTING[1].target);
        QCOMPARE(STARTING[1].stacks, 3);
        trace.Reset(16);
        QVERIFY(trace.Coverage().isEmpty());
        QVERIFY(trace.Changes().isEmpty());
        trace.Append({
            { 0, 0, 0, false, "pulse", 1, 0, 0 },
            { 0, 1, 0, false, "pulse", 0, 0, 0 }
        });
        trace.Observe(16);
        QCOMPARE(trace.Coverage()[0].activeFrames, qint64(0));
        QVERIFY(trace.Coverage()[0].intervals.isEmpty());
    }

    void buffFilteringAndExport()
    {
        BuffPanel panel;
        panel.Reset(160);
        panel.Append({
            { 0,  1, 0, false, "Self, \"buff\"", 1, 160, 0  },
            { 16, 3, 0, true,  "Target",         2, 160, 32 }
        });
        panel.Observe(32);
        const auto *coverage = panel.findChild<QTableWidget *>("buffCoverage");
        auto       *changes  = panel.findChild<QTableWidget *>("buffChanges");
        QCOMPARE(coverage->rowCount(), 2);
        QCOMPARE(coverage->item(0, 5)->text(), QStringLiteral("100.00"));
        QCOMPARE(coverage->item(1, 5)->text(), QStringLiteral("50.00"));
        QCOMPARE(coverage->item(1, 6)->text(), QStringLiteral("1.000"));
        QCOMPARE(panel.TimelineBuffs().size(), 2);
        QCOMPARE(panel.TimelineBuffs()[1].segments[0].stacks, 2);
        QCOMPARE(panel.TimelineBuffs()[1].segments[0].endMs, 2000);
        const QSignalSpy NAVIGATION(&panel, &BuffPanel::DamageRequested);
        emit             changes->cellActivated(1, 0);
        QCOMPARE(NAVIGATION.count(), 1);
        QCOMPARE(NAVIGATION.at(0).at(0).toUInt(), quint32(3));
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
        const QTemporaryDir FILES;
        QString             error;
        const auto          PATH = FILES.filePath(QStringLiteral("buffs.csv"));
        QVERIFY2(panel.ExportCsv(PATH, true, error), qPrintable(error));
        QFile csv(PATH);
        QVERIFY(csv.open(QIODevice::ReadOnly));
        const auto BYTES = csv.readAll();
        QVERIFY(BYTES.contains(QStringLiteral("\"目标增益\",\"Target\"").toUtf8()));
        QVERIFY(BYTES.contains("\"50.000000\",\"1.000000\",\"1\",\"32\",\"160\""));
        QVERIFY(!BYTES.contains("Self"));
        csv.close();
        scope->setCurrentIndex(0);
        QVERIFY2(panel.ExportCsv(PATH, false, error), qPrintable(error));
        QVERIFY(csv.open(QIODevice::ReadOnly));
        QVERIFY(csv.readAll().contains("\"Self, \"\"buff\"\"\""));
        panel.Reset(0);
        QCOMPARE(coverage->rowCount(), 0);
        QCOMPARE(changes->rowCount(), 0);
        QVERIFY(!panel.findChild<QPushButton *>("exportBuffChanges")->isEnabled());
    }

    void batchSkillStatistics_data()
    {
        QTest::addColumn<int>("specialization");
        QTest::newRow("TaiXu") << 0;
        QTest::newRow("MoWen") << 1;
    }

    void batchSkillStatistics()
    {
        QFETCH(int, specialization);
        const auto SPEC = specialization == 1 ? desktop::Specialization::MO_WEN : desktop::Specialization::TAI_XU;
        auto configuration = JX3DPS::engine::Scenario(specialization == 1 ? "mw" : "tx");
        const auto SOURCE   = desktop::DefaultMacro(SPEC);
        auto       compiled = rt::MacroCompiler::Compile(SOURCE.toStdString(), desktop::MacroOptions(SPEC, false));
        QVERIFY(compiled.second.empty());
        rt::BatchOptions options{ 7, 181, 16 * 30, 1, 4000, 12000 };
        const auto       RUN = [&](const rt::BatchOptions &batch, bool gains, bool trace, SimulationResult &output) {
            BatchWorker      worker(compiled.first.program, batch, configuration, nullptr, trace, gains);
            QSignalSpy       stats(&worker, &BatchWorker::StatisticsReady);
            QSignalSpy       failure(&worker, &BatchWorker::Failed);
            QSignalSpy       events(&worker, &BatchWorker::TraceReady);
            QSignalSpy       buffs(&worker, &BatchWorker::BuffTraceReady);
            const QSignalSpy HISTORY(&worker, &BatchWorker::HistoryReady);
            worker.start();
            QVERIFY(worker.wait(10000));
            QTRY_COMPARE(stats.count(), 1);
            QCOMPARE(failure.count(), 0);
            output = qvariant_cast<SimulationResult>(stats[0][0]);
            QVERIFY(output.skillsCollected);
            QVERIFY(output.Complete());
            QCOMPARE(worker.Completed(), worker.Requested());
            QCOMPARE(events.count(), trace ? 1 : 0);
            QCOMPARE(buffs.count(), trace ? 1 : 0);
            QCOMPARE(HISTORY.count(), trace ? 1 : 0);
        };
        const auto COMPARE = [](const QVector<SkillStatistics> &actual, const QVector<SkillStatistics> &expected) {
            QCOMPARE(actual.size(), expected.size());
            for (int index = 0; index < actual.size(); ++index) {
                const auto &a = actual[index];
                const auto &e = expected[index];
                QCOMPARE(a.skill, e.skill);
                QCOMPARE(a.name, e.name);
                QCOMPARE(a.damage, e.damage);
                QCOMPARE(a.count, e.count);
                QCOMPARE(a.hit, e.hit);
                QCOMPARE(a.critical, e.critical);
                QCOMPARE(a.insight, e.insight);
                QCOMPARE(a.miss, e.miss);
                QCOMPARE(a.hit + a.critical + a.insight + a.miss, a.count);
            }
        };
        SimulationResult serial;
        SimulationResult parallel;
        SimulationResult excess_workers;
        SimulationResult gains;
        SimulationResult trace;
        RUN(options, false, false, serial);
        QVERIFY(!serial.skills.isEmpty());
        options.workers = 3;
        RUN(options, false, false, parallel);
        COMPARE(parallel.skills, serial.skills);
        options.workers = 16;
        RUN(options, false, false, excess_workers);
        COMPARE(excess_workers.skills, serial.skills);
        options.workers = 3;
        RUN(options, true, false, gains);
        COMPARE(gains.skills, serial.skills);
        QCOMPARE(gains.gains.size(), 8);
        QCOMPARE(gains.stats.checksum, parallel.stats.checksum);

        // Independently sum seeded fight logs, including DOT ticks and zero-damage outcomes.
        QVector<SkillStatistics> expected;
        {
            const auto &config = configuration;
                const auto RULES = desktop::MakeRules(config);
                const auto PLAIN = rt::RunBatch(compiled.first.program, options, RULES);
                QCOMPARE(parallel.stats.checksum, PLAIN.checksum);
                QCOMPARE(parallel.stats.mean_damage, PLAIN.mean_damage);
                rt::Simulation simulation(compiled.first.program, RULES, options.damage_capacity, options.mutation_capacity);
                QMap<int, SkillStatistics> totals;
                for (std::uint64_t iteration = 0; iteration < options.iterations; ++iteration) {
                    simulation.Start(options.duration, rt::IterationSeed(options.seed, iteration));
                    simulation.Run();
                    for (const auto &hit : simulation.Log().Intents()) {
                        auto &skill   = totals[hit.skill];
                        skill.skill   = hit.skill;
                        skill.name    = desktop::SkillName(SPEC, hit.skill);
                        skill.damage += simulation.GetRules().Reduce(hit);
                        ++skill.count;
                        if (hit.outcome == JX3DPS::RollResult::HIT) {
                            ++skill.hit;
                        } else if (hit.outcome == JX3DPS::RollResult::DOUBLE) {
                            ++skill.critical;
                        } else if (hit.outcome == JX3DPS::RollResult::INSIGHT) {
                            ++skill.insight;
                        } else {
                            ++skill.miss;
                        }
                    }
                }
                expected = totals.values();
        }
        COMPARE(parallel.skills, expected);
        qint64 total_damage = 0;
        for (const auto &skill : parallel.skills) {
            total_damage += skill.damage;
        }
        QVERIFY(std::abs(total_damage - parallel.stats.mean_damage * parallel.stats.count) < .01);
        options.iterations = 1;
        RUN(options, false, false, serial);
        RUN(options, false, true, trace);
        COMPARE(trace.skills, serial.skills);
    }

    void skillStatisticsHistory()
    {
        ResultsPanel     panel;
        SimulationResult first;
        first.options = { 2, 0, 160, 1, 4096, 8192 };
        first.stats.Add(1000, 0);
        first.stats.Add(3000, 1);
        first.skillsCollected = true;
        first.skills          = {
            { 0, "A", 3000, 3, 1, 1, 0, 1 },
            { 1, "B", 1000, 2, 1, 0, 1, 0 }
        };
        panel.AddResult(first, "first", {});
        const auto *damage  = panel.findChild<QTableWidget *>("skillDamage");
        auto       *history = panel.findChild<QTableWidget *>("resultHistory");
        const auto *tabs    = panel.findChild<QTabWidget *>("resultTabs");
        QCOMPARE(tabs->currentIndex(), 1);
        QCOMPARE(damage->rowCount(), 2);
        QCOMPARE(damage->item(0, 2)->text(), QStringLiteral("3000"));
        QCOMPARE(damage->item(0, 3)->text(), QStringLiteral("75.0%"));
        QCOMPARE(damage->item(0, 4)->text(), QStringLiteral("150.0"));
        auto second  = first;
        second.stats = {};
        second.stats.Add(5000, 0);
        second.skills = {
            { 2, "C", 5000, 10, 4, 3, 2, 1 }
        };
        panel.AddResult(second, "second", {});
        QCOMPARE(damage->rowCount(), 1);
        QCOMPARE(damage->item(0, 0)->text(), QStringLiteral("C"));
        QCOMPARE(damage->item(0, 4)->text(), QStringLiteral("500.0"));
        history->selectRow(0);
        panel.ShowSkillStatistics();
        QCOMPARE(damage->rowCount(), 2);
        QCOMPARE(damage->item(0, 0)->text(), QStringLiteral("A"));
        const QTemporaryDir FILES;
        QString             error;
        const auto          PATH = FILES.filePath(QStringLiteral("skills.json"));
        QVERIFY2(panel.ExportSelected(PATH, error), qPrintable(error));
        QFile exported(PATH);
        QVERIFY(exported.open(QIODevice::ReadOnly));
        const auto report = QJsonDocument::fromJson(exported.readAll()).object();
        QVERIFY(report.value("skillsCollected").toBool());
        const auto SKILLS = report.value("skills").toArray();
        QCOMPARE(SKILLS.size(), 2);
        QCOMPARE(SKILLS[0].toObject().value("damage").toString(), QStringLiteral("3000"));
        QCOMPARE(SKILLS[0].toObject().value("critical").toString(), QStringLiteral("1"));
        panel.ClearTrace(10);
        QCOMPARE(damage->rowCount(), 0);
        panel.AppendTrace({
            { 0, 4, "debug", 100, 0 }
        });
        panel.ShowSkillStatistics();
        QCOMPARE(damage->item(0, 0)->text(), QStringLiteral("debug"));
        QCOMPARE(damage->item(0, 4)->text(), QStringLiteral("10.0"));
        history->selectRow(1);
        QCOMPARE(damage->item(0, 0)->text(), QStringLiteral("C"));
        auto empty  = second;
        empty.stats = {};
        empty.skills.clear();
        panel.AddResult(empty, "cancelled", {});
        QCOMPARE(damage->rowCount(), 0);
        QVERIFY(panel.findChild<QLabel *>("skillDamageSummary")->text().contains(QStringLiteral("0 场已完成战斗")));
    }

    void batchCancellationAndFailure()
    {
        const auto       COMPILED = rt::MacroCompiler::Compile("/cast wuwu\n/cast sanhuan", desktop::MacroOptions(desktop::Specialization::TAI_XU));
        rt::BatchOptions options;
        options.iterations = 1000000;
        options.duration   = 4800;
        options.workers    = 2;
        BatchWorker      worker(COMPILED.first.program, options, JX3DPS::engine::Scenario("tx"));
        const QSignalSpy result(&worker, &BatchWorker::ResultReady);
        const QSignalSpy STATISTICS(&worker, &BatchWorker::StatisticsReady);
        const QSignalSpy NO_BUFF_TRACE(&worker, &BatchWorker::BuffTraceReady);
        const QSignalSpy NO_HISTORY(&worker, &BatchWorker::HistoryReady);
        const QSignalSpy FAILED(&worker, &BatchWorker::Failed);
        int              heartbeats = 0;
        QTimer           timer;
        connect(&timer, &QTimer::timeout, this, [&] { ++heartbeats; });
        timer.start(1);
        worker.start();
        QTRY_VERIFY_WITH_TIMEOUT(worker.Completed() > 0, 10000);
        QVERIFY(heartbeats > 0);
        worker.RequestCancel();
        QVERIFY(worker.wait(10000));
        QTRY_COMPARE(result.count(), 1);
        QTRY_COMPARE(STATISTICS.count(), 1);
        const auto PARTIAL = qvariant_cast<SimulationResult>(STATISTICS.at(0).at(0));
        QCOMPARE(PARTIAL.stats.count, worker.Completed());
        QVERIFY(PARTIAL.skillsCollected);
        QVERIFY(!PARTIAL.skills.isEmpty());
        qint64 skill_damage = 0;
        for (const auto &skill : PARTIAL.skills) {
            skill_damage += skill.damage;
        }
        QVERIFY(std::abs(skill_damage - PARTIAL.stats.mean_damage * PARTIAL.stats.count) < .01);
        QVERIFY(!PARTIAL.Complete());
        QCOMPARE(PARTIAL.options.seed, options.seed);
        QCOMPARE(FAILED.count(), 0);
        QCOMPARE(NO_BUFF_TRACE.count(), 0);
        QCOMPARE(NO_HISTORY.count(), 0);
        QVERIFY(result.at(0).at(0).toString().contains(QStringLiteral("已取消")));
        QVERIFY(worker.Completed() < options.iterations);
        auto invalid           = JX3DPS::engine::Scenario("tx");
        invalid.initial_qidian = -1;
        BatchWorker      bad(COMPILED.first.program, options, invalid);
        const QSignalSpy ERRORS(&bad, &BatchWorker::Failed);
        bad.start();
        QVERIFY(bad.wait(10000));
        QTRY_COMPARE(ERRORS.count(), 1);

        BatchWorker      cancelled(COMPILED.first.program, options, JX3DPS::engine::Scenario("tx"));
        const QSignalSpy cancelled_result(&cancelled, &BatchWorker::ResultReady);
        const QSignalSpy CANCELLED_STATS(&cancelled, &BatchWorker::StatisticsReady);
        cancelled.RequestCancel();
        cancelled.start();
        QVERIFY(cancelled.wait(10000));
        QTRY_COMPARE(cancelled_result.count(), 1);
        QCOMPARE(cancelled.Completed(), std::uint64_t(0));
        QTRY_COMPARE(CANCELLED_STATS.count(), 1);
        QCOMPARE(qvariant_cast<SimulationResult>(CANCELLED_STATS.at(0).at(0)).stats.count, std::uint64_t(0));
        QVERIFY(qvariant_cast<SimulationResult>(CANCELLED_STATS.at(0).at(0)).skills.isEmpty());
    }

    void configurationRoundTrip()
    {
        SimulationOptions options;
        auto             *attack = options.findChild<QSpinBox *>("attackBase");
        attack->setValue(45678);
        options.findChild<QLineEdit *>("seed")->setText(QStringLiteral("18446744073709551615"));
        options.findChild<QDoubleSpinBox *>("seconds")->setValue(10.0625);
        options.findChild<QCheckBox *>(QStringLiteral("recipe/tx/无我无剑·5%伤害"))->setChecked(false);
        const auto saved_recipes = options.Config().recipes;
        const auto          snapshot = options.Snapshot();
        // A workspace saved before another school shipped has no fields for that school.
        auto older = snapshot;
        auto oldTalents = older["talents"].toObject(), oldValues = older["values"].toObject();
        oldTalents.remove("yly");
        for (const auto &key : oldValues.keys())
            if (key.startsWith("recipe/yly/") || key.startsWith("equipment/yly/")) oldValues.remove(key);
        older["talents"] = oldTalents;
        older["values"] = oldValues;
        QString restoreError;
        QVERIFY2(options.RestoreSnapshot(older, restoreError), qPrintable(restoreError));
        QCOMPARE(options.Snapshot(), snapshot);
        const QTemporaryDir FILES;
        QVERIFY(FILES.isValid());
        QString    error;
        const auto PATH = FILES.filePath(QStringLiteral("config.json"));
        QVERIFY2(options.ExportConfiguration(PATH, error), qPrintable(error));
        attack->setValue(1);
        QVERIFY2(options.ImportConfiguration(PATH, error), qPrintable(error));
        QCOMPARE(options.Snapshot(), snapshot);
        QCOMPARE(options.Config().attributes.attack_base, JX3DPS::value_t(45678));
        QCOMPARE(options.Batch().seed, std::numeric_limits<std::uint64_t>::max());
        QCOMPARE(options.Batch().duration, 161);
        QCOMPARE(options.Config().recipes, saved_recipes);

        QSignalSpy changed(attack, &QSpinBox::valueChanged);
        const auto reject = [&](QJsonObject bad) {
            QVERIFY(!options.RestoreSnapshot(bad, error));
            QVERIFY(!error.isEmpty());
            QCOMPARE(options.Snapshot(), snapshot);
            QCOMPARE(changed.count(), 0);
        };
        for (const auto &value : { QJsonValue(-1), QJsonValue(1.25), QJsonValue("100"), QJsonValue(1000001) }) {
            auto bad    = snapshot;
            auto values = bad.value("values").toObject();
            values.insert("attackBase", value);
            bad.insert("values", values);
            reject(bad);
        }
        for (const auto &value : { QJsonValue("18446744073709551616"), QJsonValue(123), QJsonValue(" 123") }) {
            auto bad    = snapshot;
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
        for (const unsigned FLAG : { 1u, 2u, 4u, 8u, 16u }) {
            values.insert(QStringLiteral("wuwu%1").arg(FLAG), true);
        }
        bad.insert("values", values);
        reject(bad);

        QFile malformed(PATH);
        QVERIFY(malformed.open(QIODevice::WriteOnly));
        malformed.write("{ broken");
        malformed.close();
        QVERIFY(!options.ImportConfiguration(PATH, error));
        QCOMPARE(options.Snapshot(), snapshot);

        auto *profiles = options.findChild<QComboBox *>("configurationProfile");
        profiles->setCurrentText(QStringLiteral("Trial configuration"));
        options.findChild<QPushButton *>("saveProfile")->click();
        attack->setValue(99);
        options.findChild<QPushButton *>("loadProfile")->click();
        QCOMPARE(options.Snapshot(), snapshot);
        const SimulationOptions RESTORED;
        RESTORED.findChild<QComboBox *>("configurationProfile")->setCurrentText(QStringLiteral("Trial configuration"));
        RESTORED.findChild<QPushButton *>("loadProfile")->click();
        QCOMPARE(RESTORED.Snapshot(), snapshot);
    }

    void resultAnalysis()
    {
        ResultsPanel            panel;
        const SimulationOptions OPTIONS;
        SimulationResult        first;
        first.options.iterations = 2;
        first.options.duration   = 160;
        first.options.seed       = std::numeric_limits<std::uint64_t>::max();
        first.elapsedSeconds     = .5;
        first.stats.Add(1000, 0);
        first.stats.Add(3000, 1);
        const auto    CONFIG = OPTIONS.Snapshot();
        const QString SOURCE = QStringLiteral("/cast sanhuan");
        panel.AddResult(first, SOURCE, CONFIG);
        const auto *history = panel.findChild<QTableWidget *>("resultHistory");
        QCOMPARE(history->item(0, 3)->text(), QStringLiteral("200.0"));
        QCOMPARE(history->item(0, 4)->text(), QStringLiteral("100.0"));
        panel.findChild<QPushButton *>("setResultBaseline")->click();
        auto second  = first;
        second.stats = {};
        second.stats.Add(4000, 0);
        second.stats.Add(4000, 1);
        panel.AddResult(second, SOURCE, CONFIG);
        QCOMPARE(history->item(1, 5)->text(), QStringLiteral("+100.00%"));
        const QTemporaryDir FILES;
        QString             error;
        const auto          PATH = FILES.filePath(QStringLiteral("run.json"));
        QVERIFY2(panel.ExportSelected(PATH, error), qPrintable(error));
        QFile exported(PATH);
        QVERIFY(exported.open(QIODevice::ReadOnly));
        const auto report = QJsonDocument::fromJson(exported.readAll()).object();
        QCOMPARE(report.value("macro").toString(), SOURCE);
        QCOMPARE(report.value("configuration").toObject(), CONFIG);
        const auto STATS = report.value("result").toObject();
        QCOMPARE(STATS.value("seed").toString(), QStringLiteral("18446744073709551615"));
        QCOMPARE(STATS.value("checksum").toString(), QString::number(second.stats.checksum));
        QCOMPARE(STATS.value("meanDps").toDouble(), 400.0);
        QCOMPARE(STATS.value("minDamage").toString(), QStringLiteral("4000"));
        const QSignalSpy INPUTS(&panel, &ResultsPanel::InputsRequested);
        panel.SetInputsEnabled(false);
        QVERIFY(!panel.findChild<QPushButton *>("useResultInputs")->isEnabled());
        panel.SetInputsEnabled(true);
        panel.findChild<QPushButton *>("useResultInputs")->click();
        QCOMPARE(INPUTS.count(), 1);
        QCOMPARE(INPUTS.at(0).at(0).toString(), SOURCE);

        panel.ClearTrace(10);
        panel.AppendTrace({
            { 0,  0, "skill A", 100, 0 },
            { 16, 0, "skill A", 200, 1 },
            { 32, 1, "skill B", 50,  4 }
        });
        panel.AppendTrace({
            { 48, 0, "skill A", 0, 126 }
        });
        const auto *damage = panel.findChild<QTableWidget *>("skillDamage");
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
        panel.AppendTrace({
            { 0, 0, "skill A", 0, 126 }
        });
        QCOMPARE(damage->item(0, 3)->text(), QStringLiteral("0.0%"));

        auto empty  = first;
        empty.stats = {};
        panel.AddResult(empty, SOURCE, CONFIG);
        QCOMPARE(history->item(2, 3)->text(), QStringLiteral("--"));
        QVERIFY(!panel.findChild<QPushButton *>("setResultBaseline")->isEnabled());
        for (int i = 0; i < 50; ++i) {
            panel.AddResult(second, SOURCE, CONFIG);
        }
        QCOMPARE(history->rowCount(), 50);
        QCOMPARE(history->item(49, 5)->text(), QStringLiteral("+100.00%"));
        QVERIFY(panel.findChild<QTextBrowser *>("resultDetails")->toPlainText().contains(QStringLiteral("基准 #1")));
    }

    void workbenchCapturesInputs()
    {
        const Workbench WINDOW;
        auto           *editor = WINDOW.findChild<CodeEditor *>("macroEditor");
        const QString   SOURCE = QStringLiteral("/cast sanhuan");
        editor->setPlainText(SOURCE);
        WINDOW.findChild<QSpinBox *>("iterations")->setValue(100);
        WINDOW.findChild<QSpinBox *>("workers")->setValue(2);
        WINDOW.findChild<QDoubleSpinBox *>("seconds")->setValue(5);
        auto *toolbar = WINDOW.findChild<SimulationToolbar *>();
        emit  toolbar->StartSimulationClicked();
        QVERIFY(!WINDOW.findChild<QComboBox *>("specialization")->isEnabled());
        editor->setPlainText(QStringLiteral("/cast wuwu"));
        QTRY_VERIFY_WITH_TIMEOUT(toolbar->isEnabled(), 10000);
        const auto *results = WINDOW.findChild<ResultsPanel *>();
        QCOMPARE(results->findChild<QTableWidget *>("resultHistory")->rowCount(), 1);
        const auto *details = results->findChild<QTextBrowser *>("resultDetails");
        QVERIFY(details->toPlainText().contains(SOURCE));
        QVERIFY(!details->toPlainText().contains(QStringLiteral("/cast wuwu")));
        WINDOW.findChild<QSpinBox *>("iterations")->setValue(1);
        editor->document()->setModified(false);
        results->findChild<QPushButton *>("useResultInputs")->click();
        QCOMPARE(editor->toPlainText(), SOURCE);
        QVERIFY(editor->document()->isModified());
        QCOMPARE(WINDOW.findChild<QSpinBox *>("iterations")->value(), 100);
    }

    void workbenchFlow()
    {
        Workbench window;
        window.show();
        auto       *editor   = window.findChild<CodeEditor *>("macroEditor");
        const auto *table    = window.findChild<QTableWidget *>("damageEvents");
        const auto *problems = window.findChild<QTableWidget *>("macroProblems");
        auto       *check    = window.findChild<QAction *>("checkMacro");
        auto       *trace    = window.findChild<QAction *>("singleTrace");
        auto       *session  = window.findChild<DebugSession *>();
        QVERIFY(editor && table && problems && check && trace && session);
        QCoreApplication::processEvents();
        if (qEnvironmentVariableIsSet("JX3DPS_TEST_SCREENSHOT")) {
            QVERIFY(window.grab().save(qEnvironmentVariable("JX3DPS_TEST_SCREENSHOT") +
                                       QStringLiteral("-configuration.png")));
        }
        editor->setPlainText(QStringLiteral("/cast [buff:碎星辰] wuwu\n/cast [buff:碎星辰>0] wuwu\n/cast sanhuan"));
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
        if (qEnvironmentVariableIsSet("JX3DPS_TEST_SCREENSHOT")) {
            QVERIFY(window.findChild<QDialog *>("resultAnalysis")->grab().save(qEnvironmentVariable("JX3DPS_TEST_SCREENSHOT") + QStringLiteral("-trace.png")));
        }
        qint64 total = 0;
        for (int i = 0; i < table->rowCount(); ++i) {
            total += table->item(i, 3)->text().toLongLong();
        }
        auto *results = window.findChild<ResultsPanel *>();
        QCOMPARE(results->findChild<QTableWidget *>("resultHistory")->rowCount(), 1);
        const auto *skills       = results->findChild<QTableWidget *>("skillDamage");
        qint64      skills_total = 0;
        for (int i = 0; i < skills->rowCount(); ++i) {
            skills_total += skills->item(i, 2)->text().toLongLong();
        }
        QCOMPARE(skills_total, total);
        if (qEnvironmentVariableIsSet("JX3DPS_TEST_SCREENSHOT")) {
            auto *views    = window.findChild<QTabWidget *>("analysisViews");
            auto *analysis = window.findChild<QDialog *>("resultAnalysis");
            views->setCurrentWidget(results);
            QTest::qWait(250);
            QVERIFY(analysis->grab().save(qEnvironmentVariable("JX3DPS_TEST_SCREENSHOT") +
                                          QStringLiteral("-results.png")));
            results->findChild<QTabWidget *>("resultTabs")->setCurrentIndex(1);
            QTest::qWait(250);
            QVERIFY(analysis->grab().save(qEnvironmentVariable("JX3DPS_TEST_SCREENSHOT") +
                                          QStringLiteral("-skills.png")));
            views->setCurrentIndex(0);
        }
        auto *mode = window.findChild<QToolButton *>("debugButton");
        QVERIFY(mode);
        mode->setChecked(true);
        auto *toolbar = window.findChild<SimulationToolbar *>();
        emit  toolbar->StartSimulationClicked();
        QCOMPARE(session->GetState(), DebugSession::PAUSED);
        QVERIFY(editor->isReadOnly());
        const auto SOURCE = editor->toPlainText();
        QTest::keyClick(editor, Qt::Key_Tab);
        QTest::keyClick(editor, Qt::Key_Return);
        editor->FormatDocument();
        QCOMPARE(editor->toPlainText(), SOURCE);
        QVERIFY(QMetaObject::invokeMethod(editor, "CheckSyntax", Qt::DirectConnection));
        session->StepInto();
        QCOMPARE(session->GetDebugInfo().phase, QStringLiteral("条件"));
        QVERIFY(session->GetDebugInfo().condition.contains(QStringLiteral("成立")));
        if (qEnvironmentVariableIsSet("JX3DPS_TEST_SCREENSHOT")) {
            QVERIFY(window.grab().save(qEnvironmentVariable("JX3DPS_TEST_SCREENSHOT")));
        }
        session->RemoveBreakpoint(1);
        session->Continue();
        QTRY_COMPARE_WITH_TIMEOUT(session->GetState(), DebugSession::FINISHED, 10000);
        QCOMPARE(session->TotalDamage(), total);
        QVERIFY(!editor->isReadOnly());
        session->Stop();
        QVERIFY(editor->HasBreakpoint(1));
        editor->document()->setModified(true);
        QVERIFY(window.close());
        const Workbench RESTORED;
        const auto     *restored_editor = RESTORED.findChild<CodeEditor *>("macroEditor");
        QCOMPARE(restored_editor->toPlainText(), SOURCE);
        QVERIFY(restored_editor->HasBreakpoint(1));
        QVERIFY(restored_editor->document()->isModified());
        QCOMPARE(RESTORED.findChild<QLineEdit *>("seed")->text(), QStringLiteral("77"));
    }

    void breakpointsFollowEdits()
    {
        const Workbench WINDOW;
        auto           *editor  = WINDOW.findChild<CodeEditor *>("macroEditor");
        const auto     *session = WINDOW.findChild<DebugSession *>();
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
        Workbench window;
        window.show();
        auto *options = window.findChild<SimulationOptions *>();
        auto *picker  = options->findChild<QComboBox *>("specialization");
        QCOMPARE(picker->count(), static_cast<int>(rt::season50::Books().size()));
        for (int i = 0; i < picker->count(); ++i) {
            picker->setCurrentIndex(i);
            const auto &book = desktop::School(options->Specialization());
            QVERIFY(!picker->itemIcon(i).pixmap(38, 38).isNull());
            QCOMPARE(picker->itemData(i).toString(), desktop::Text(book.code));
            const auto *panel = options->findChild<desktop::TalentPanel *>("talents" + QString::number(i));
            QVERIFY(panel);
            QCOMPARE(panel->Selection().size(), 9);
            QVERIFY(!panel->HeaderWidget()->findChild<QToolButton *>("legacyTalentToggle"));
        }
        QVERIFY(!window.findChild<QToolButton *>("openSeason50"));
        if (qEnvironmentVariableIsSet("JX3DPS_TEST_SCREENSHOT")) {
            picker->setCurrentIndex(picker->findData("gfj"));
            QCoreApplication::processEvents();
            QVERIFY(window.grab().save(qEnvironmentVariable("JX3DPS_TEST_SCREENSHOT") + "-flagship.png"));
            picker->showPopup();
            QTest::qWait(80);
            auto *popup = QApplication::activePopupWidget();
            QVERIFY(popup);
            QVERIFY(popup->grab().save(qEnvironmentVariable("JX3DPS_TEST_SCREENSHOT") + "-schools.png"));
        }
    }

    void iconChoicesUseVerticalRows()
    {
        Workbench window;
        window.resize(1120, 820);
        window.show();
        auto *options        = window.findChild<SimulationOptions *>();
        auto *specialization = static_cast<desktop::IconComboBox *>(window.findChild<QComboBox *>("specialization"));
        specialization->setCurrentIndex(1);
        QTest::qWait(80);
        auto *team = static_cast<desktop::IconComboBox *>(options->findChild<QComboBox *>("permanent/内功/阵法"));
        team->setCurrentIndex(1);
        QCoreApplication::processEvents();
        const auto check_list = [&](desktop::IconComboBox *combo) {
            const auto BEFORE = options->Snapshot();
            combo->showPopup();
            const auto *view  = combo->choiceView();
            auto       *popup = static_cast<desktop::ChoicePopup *>(view->window());
            QTRY_VERIFY(popup->isVisible());
            QTRY_VERIFY(!popup->ExpansionAnimating());
            QCOMPARE(view->viewMode(), QListView::ListMode);
            QCOMPARE(view->flow(), QListView::TopToBottom);
            QVERIFY(!view->isWrapping());
            QCOMPARE(view->horizontalScrollBar()->maximum(), 0);
            if (combo->count() > 8) {
                QVERIFY(view->verticalScrollBar()->maximum() > 0);
            }
            QVERIFY(popup->screen()->availableGeometry().contains(popup->geometry()));
            int previous_bottom = view->visualRect(combo->model()->index(0, combo->modelColumn())).top() - 1;
            for (int i = 0; i < combo->count(); ++i) {
                const auto ROW = view->visualRect(combo->model()->index(i, combo->modelColumn()));
                QCOMPARE(ROW.left(), 0);
                QCOMPARE(ROW.width(), view->viewport()->width());
                QCOMPARE(ROW.top(), previous_bottom + 1);
                QCOMPARE(ROW.height(), view->sizeHintForRow(i));

                previous_bottom        = ROW.bottom();
                if (!view->viewport()->rect().contains(ROW)) {
                    continue;
                }
                // The text column must actually be painted, not just available through a tooltip.
                const auto IMAGE       = view->viewport()->grab(ROW).toImage();
                const auto RATIO       = IMAGE.devicePixelRatio();
                int        text_pixels = 0;
                for (int y = 0; y < IMAGE.height(); ++y) {
                    for (int x = qRound(52 * RATIO); x < qRound((ROW.width() - 34) * RATIO); ++x) {
                        const auto COLOR  = IMAGE.pixelColor(x, y);
                        text_pixels      += COLOR.red() > 150 && COLOR.green() > 150 && COLOR.blue() > 150;
                    }
                }
                QVERIFY2(text_pixels > 3, qPrintable(combo->itemText(i)));
            }
            QCOMPARE(options->Snapshot(), BEFORE);
            if (qEnvironmentVariableIsSet("JX3DPS_TEST_SCREENSHOT")) {
                QVERIFY(popup->grab().save(qEnvironmentVariable("JX3DPS_TEST_SCREENSHOT") +
                                           QStringLiteral("-vertical-%1.png").arg(combo->objectName())));
            }
        };
        check_list(team);
        QCOMPARE(team->itemText(0), QStringLiteral("无"));
        auto            *view = team->choiceView();
        const QSignalSpy ACTIVATED(team, &QComboBox::activated);
        const auto       last = view->visualRect(team->model()->index(2, 0));
        QTest::mouseClick(view->viewport(), Qt::LeftButton, Qt::NoModifier, QPoint(80, last.center().y()));
        QCOMPARE(team->currentIndex(), 2);
        QCOMPARE(ACTIVATED.count(), 1);
        QVERIFY(!view->window()->isVisible());
        team->showPopup();
        QTest::keyClick(view, Qt::Key_Home);
        QTest::keyClick(view, Qt::Key_Return);
        QCOMPARE(team->currentIndex(), 0);
        QVERIFY(!view->window()->isVisible());
        check_list(specialization);
        QTest::keyClick(specialization->choiceView(), Qt::Key_Home);
        QTest::keyClick(specialization->choiceView(), Qt::Key_Escape);
        QCOMPARE(specialization->currentIndex(), 1);
        QVERIFY(!specialization->choiceView()->window()->isVisible());
        auto *food = static_cast<desktop::IconComboBox *>(options->findChild<QComboBox *>("permanent/内功/食品增强"));
        check_list(food);
        food->hidePopup();
    }

    void iconPopupExpansion()
    {
        QWidget    host;
        const auto AVAILABLE = host.screen()->availableGeometry();
        host.resize(180, 120);
        host.move(AVAILABLE.topLeft() + QPoint(20, 20));
        desktop::IconComboBox choice(QStringLiteral("图标"), &host);
        choice.move(20, 20);
        for (int index = 0; index < 16; ++index) {
            choice.addItem(desktop::GameIcon(8484), QString::number(index));
        }
        host.show();
        QCoreApplication::processEvents();
        choice.showPopup();
        const auto *popup  = static_cast<desktop::ChoicePopup *>(choice.choiceView()->window());
        const auto  BOUNDS = popup->geometry();
        QVERIFY(AVAILABLE.contains(BOUNDS));
        QVERIFY(popup->ExpansionAnimating());
        QVERIFY(popup->mask().boundingRect().height() < popup->height());
        QCOMPARE(popup->mask().boundingRect().top(), 0);
        QTRY_VERIFY(!popup->ExpansionAnimating());
        QCOMPARE(popup->geometry(), BOUNDS);
        QVERIFY(popup->mask().isEmpty());
        QCOMPARE(choice.choiceView()->viewMode(), QListView::ListMode);
        QCOMPARE(choice.choiceView()->horizontalScrollBar()->maximum(), 0);
        QVERIFY(choice.choiceView()->verticalScrollBar()->maximum() > 0);
        choice.hidePopup();

        host.move(AVAILABLE.right() - host.width() + 1, AVAILABLE.bottom() - host.height() + 1);
        QCoreApplication::processEvents();
        choice.showPopup();
        QVERIFY(popup->ExpansionAnimating());
        QVERIFY(AVAILABLE.contains(popup->geometry()));
        QVERIFY(popup->geometry().bottom() < choice.mapToGlobal(QPoint()).y());
        QCOMPARE(popup->mask().boundingRect().bottom(), popup->height() - 1);
        QVERIFY(popup->mask().boundingRect().top() > 0);
        QTest::keyClick(choice.choiceView(), Qt::Key_Escape);
        QVERIFY(!popup->isVisible());
        QVERIFY(!popup->ExpansionAnimating());
        QVERIFY(popup->mask().isEmpty());

        choice.showPopup();
        QVERIFY(popup->ExpansionAnimating());
        QTest::keyClick(choice.choiceView(), Qt::Key_End);
        QTest::keyClick(choice.choiceView(), Qt::Key_Return);
        QCOMPARE(choice.currentIndex(), 15);
        QVERIFY(!popup->isVisible());
        QVERIFY(!popup->ExpansionAnimating());
        QVERIFY(popup->mask().isEmpty());
        choice.showPopup();
        QTRY_VERIFY(!popup->ExpansionAnimating());
        const auto SELECTED = choice.choiceView()->visualRect(choice.model()->index(15, 0));
        QVERIFY(choice.choiceView()->viewport()->rect().contains(SELECTED));
        choice.hidePopup();
        choice.setEnabled(false);
        choice.showPopup();
        QVERIFY(!popup->isVisible());
    }

    void talentTierSelection()
    {
        SimulationOptions options;
        auto             *school = options.findChild<QComboBox *>("specialization");
        school->setCurrentIndex(school->findData("gfj"));
        auto *panel = options.findChild<desktop::TalentPanel *>(QStringLiteral("talents%1").arg(school->currentIndex()));
        QVERIFY(panel);
        QCOMPARE(panel->Selection().size(), 9);
        const auto &defaults = desktop::School(options.Specialization()).default_talents;
        QVERIFY(std::ranges::is_permutation(options.Config().talents, defaults));
        panel->ClearSelection();
        QVERIFY(options.Config().talents.empty());
        panel->RestoreSelection(panel->DefaultSelection());
        QVERIFY(!options.Config().talents.empty());
        auto *recipe = options.findChild<QCheckBox *>(QStringLiteral("recipe/gfj/孤锋破浪·5%伤害"));
        QVERIFY(recipe);
        recipe->setChecked(false);
        const auto savedRecipes = options.Config().recipes;
        const auto id = options.Snapshot();
        school->setCurrentIndex(school->findData("mw"));
        const auto &mwRecipes = desktop::School(desktop::Specialization::MO_WEN).default_recipes;
        QVERIFY(std::ranges::is_permutation(options.Config().recipes, mwRecipes));
        QString error;
        QVERIFY(options.RestoreSnapshot(id, error));
        QCOMPARE(options.Config().recipes, savedRecipes);
        QVERIFY(!panel->findChild<QToolButton *>("legacyTalentToggle"));
    }

    void timelineContinuousCursor()
    {
        Timeline timeline;
        timeline.resize(800, 240);
        Timeline::BuffItem buff;
        buff.name     = QStringLiteral("自身增益：紫气东来");
        buff.icon     = desktop::GameIcon(desktop::Specialization::TAI_XU, QStringLiteral("紫气东来")).pixmap(32, 32);
        buff.segments = {
            { 0, 1000, 1 }
        };
        timeline.SetBuffs({ buff });
        timeline.SetTimeRange(0, 1000);
        timeline.show();
        QSignalSpy cursor(&timeline, &Timeline::CursorTimeChanged);
        QSignalSpy leave(&timeline, &Timeline::CursorLeft);
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
        auto config = JX3DPS::engine::Scenario("tx");
        config.initial_qidian = 0;
        DebugSimulator debugger;
        debugger.SetOptions(config, 640, 913);
        QString error;
        QVERIFY2(debugger.Initialize(QStringLiteral("/scast 碎星辰\n/cast 三柴剑法"), error), qPrintable(error));
        while (!debugger.IsFinished()) debugger.Continue();
        desktop::CombatHistory history;
        history.Append(debugger.TakeHistory());
        const auto field = desktop::MacroOptions(desktop::Specialization::TAI_XU).resolve_buff("碎星辰");
        const auto active = history.At(32), expired = history.At(639);
        QVERIFY(active && expired);
        QCOMPARE(active->state.self_buff_stacks[field], 1);
        QCOMPARE(expired->state.self_buff_stacks[field], 0);
        QCOMPARE(history.At(32)->attributes, active->attributes);
        QVERIFY(!history.At(641));
        history.Clear();
        QVERIFY(!history.At(0));
    }

    void historyCheckpointReplay()
    {
        desktop::HistoryChunk chunk;
        chunk.initial.emplace();
        const auto catalog = desktop::Describe(desktop::Specialization::TAI_XU);
        chunk.initial->Resize(catalog.skills.size(), catalog.buffs.size());
        chunk.attributes.push_back({ 0, {} });
        for (int frame = 0; frame < 900; ++frame) {
            chunk.mutations.push_back(
                { frame, static_cast<unsigned>(frame), rt::MutationKind::RESOURCES, rt::INVALID_SLOT, false, frame % 11, frame, frame / 10 });
        }
        chunk.observedFrame = 900;
        desktop::CombatHistory history;
        history.Append(chunk);
        for (const int FRAME : { 899, 0, 255, 256, 511, 512, 257, 900, 300 }) {
            const auto result = history.At(FRAME);
            QVERIFY(result.has_value());
            QCOMPARE(result->state.energy, std::min(FRAME, 899));
            QCOMPARE(result->state.qidian, std::min(FRAME, 899) % 11);
        }
    }

    void timelineEventSelection()
    {
        Timeline timeline;
        timeline.resize(800, 210);
        timeline.show();
        Timeline::EventItem event{};
        event.timestamp = 500;
        event.name      = QStringLiteral("命中");
        event.damage    = 123;
        timeline.SetEvents({ event, event });
        timeline.SetTimeRange(0, 1000);
        int selected = -1;
        int clicks   = 0;
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
        buff.name     = QStringLiteral("Buff: sample");
        buff.color    = Qt::green;
        buff.segments = {
            { 0, 1000, 2 }
        };
        timeline.SetBuffs({ buff });
        timeline.SetTimeRange(0, 1000);
        QTest::mouseClick(&timeline, Qt::LeftButton, {}, QPoint(420, 120));
        QCOMPARE(clicks, 2);
        QSignalSpy range(&timeline, &Timeline::VisibleRangeChanged);
        for (int i = 0; i < 100; ++i) {
            timeline.ZoomIn();
        }
        QVERIFY(!range.isEmpty());
        QCOMPARE(range.last().at(1).toInt() - range.last().at(0).toInt(), 1);
        for (int i = 0; i < 100; ++i) {
            timeline.ZoomOut();
        }
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
        // This fixture follows the sword debuff at frame zero in isolation.
        for (auto *box : window.findChildren<QCheckBox *>())
            if (box->objectName().startsWith("equipment/tx/")) box->setChecked(false);
        auto *editor = window.findChild<CodeEditor *>("macroEditor");
        editor->setPlainText(
            QStringLiteral("/cast [qidian>=8] 无我无剑\n/cast [nobuff:碎星辰] 碎星辰\n/cast 三环套月"));

        window.findChild<QDoubleSpinBox *>("seconds")->setValue(60);
        auto *trace = window.findChild<QAction *>("singleTrace");
        trace->trigger();
        QTRY_VERIFY_WITH_TIMEOUT(trace->isEnabled(), 10000);
        auto       *panel    = window.findChild<BuffPanel *>();
        const auto *coverage = panel->findChild<QTableWidget *>("buffCoverage");
        auto       *changes  = panel->findChild<QTableWidget *>("buffChanges");
        auto       *damage   = window.findChild<QTableWidget *>("damageEvents");
        auto       *views    = window.findChild<QTabWidget *>("analysisViews");
        QVERIFY(coverage->rowCount() >= 2);
        QVERIFY(changes->rowCount() > coverage->rowCount());
        QVERIFY(panel->findChild<QLabel *>("buffStatus")->text().contains(QStringLiteral("60.0000 / 60.0000")));
        QStringList reference;
        for (int row = 0; row < coverage->rowCount(); ++row) {
            for (int column = 0; column < coverage->columnCount(); ++column) {
                reference << coverage->item(row, column)->text();
            }
        }
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
        const auto *history  = window.findChild<HistoryPanel *>("timelineHistory");
        auto       *timeline = window.findChild<Timeline *>("combatTimeline");
        QVERIFY(history && timeline);
        QVERIFY(!history->History().Empty());
        const auto OLD_ATTACK = history->History().At(16)->attributes.attack;
        window.findChild<QSpinBox *>("attackBase")->setValue(50000);
        QCOMPARE(history->History().At(16)->attributes.attack, OLD_ATTACK);
        window.findChild<QSpinBox *>("attackBase")->setValue(10000);
        timeline->SetTimeRange(0, 16000);
        const QPoint POSITION(timeline->width() / 3, 195);
        QMouseEvent move(QEvent::MouseMove, POSITION, timeline->mapToGlobal(POSITION), Qt::NoButton, Qt::NoButton, Qt::NoModifier);
        QApplication::sendEvent(timeline, &move);
        QCoreApplication::processEvents();
        QVERIFY(!history->findChild<QLabel *>("historyTime")->text().startsWith("0.0000"));
        if (qEnvironmentVariableIsSet("JX3DPS_TEST_SCREENSHOT")) {
            auto *analysis = window.findChild<QDialog *>("resultAnalysis");
            QTest::qWait(250);
            QVERIFY(analysis->grab().save(qEnvironmentVariable("JX3DPS_TEST_SCREENSHOT") +
                                          QStringLiteral("-buff-timeline.png")));
            views->setCurrentWidget(panel);
            QTest::qWait(250);
            QVERIFY(analysis->grab().save(qEnvironmentVariable("JX3DPS_TEST_SCREENSHOT") +
                                          QStringLiteral("-buffs.png")));
            panel->findChild<QTabWidget *>("buffTabs")->setCurrentIndex(1);
            QTest::qWait(250);
            QVERIFY(analysis->grab().save(qEnvironmentVariable("JX3DPS_TEST_SCREENSHOT") +
                                          QStringLiteral("-buff-changes.png")));
        }
        auto *mode = window.findChild<QToolButton *>("debugButton");
        mode->setChecked(true);
        auto *session = window.findChild<DebugSession *>();
        session->AddBreakpoint(2);
        emit window.findChild<SimulationToolbar *>()->StartSimulationClicked();
        QCOMPARE(coverage->rowCount(), 1);
        QCOMPARE(coverage->item(0, 1)->text(), QStringLiteral("玄应回气"));
        session->Continue();
        QTRY_COMPARE(session->GetState(), DebugSession::PAUSED);
        QCOMPARE(session->GetDebugInfo().currentFrame, 0);
        QCOMPARE(coverage->rowCount(), 2);
        QCOMPARE(coverage->item(0, 4)->text(), QStringLiteral("0.0000"));
        QCOMPARE(coverage->item(0, 5)->text(), QStringLiteral("--"));
        session->RemoveBreakpoint(2);
        session->Continue();
        QTRY_COMPARE_WITH_TIMEOUT(session->GetState(), DebugSession::FINISHED, 10000);
        QStringList debug;
        for (int row = 0; row < coverage->rowCount(); ++row) {
            for (int column = 0; column < coverage->columnCount(); ++column) {
                debug << coverage->item(row, column)->text();
            }
        }
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
        emit  toolbar->StartSimulationClicked();
        QVERIFY(!toolbar->isEnabled());
        QVERIFY(window.findChild<QPushButton *>("cancelBatch")->isEnabled());
        QVERIFY(!window.close());
        QTRY_VERIFY_WITH_TIMEOUT(!window.isVisible(), 10000);
        QVERIFY(toolbar->isEnabled());
    }
};

QTEST_MAIN(DesktopTests)
#include "desktop_tests.moc"
