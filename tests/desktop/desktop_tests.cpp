#include <QtTest>
#include <QAction>
#include <QCheckBox>
#include <QComboBox>
#include <QCompleter>
#include <QDoubleSpinBox>
#include <QGroupBox>
#include <QDialog>
#include <QDir>
#include <QFile>
#include <QFormLayout>
#include <QJsonArray>
#include <QLabel>
#include <QLineEdit>
#include <QJsonDocument>
#include <QListWidget>
#include "src/class/mo_wen/runtime_rules.hpp"
#include <QInputMethodEvent>
#include <QPainter>
#include <QPushButton>
#include <QProgressBar>
#include <QScrollArea>
#include <QScrollBar>
#include <QScreen>
#include <QSettings>
#include <QSpinBox>
#include <QStyleOptionGroupBox>
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
#include "apps/desktop/talent_panel.h"
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
#include "apps/desktop/controls/code_editor/debug_toolbar.h"
#include "apps/desktop/controls/code_editor/simulation_toolbar.h"
#include "apps/desktop/controls/choice_popup.h"
#include "apps/desktop/controls/theme/dark_style.h"
#include "apps/desktop/controls/theme/segmented_tabs.h"
#include "apps/desktop/controls/theme/magpie_shell.h"
#include "apps/desktop/controls/theme/history_chart.h"
#include "apps/desktop/game_attributes.h"
#include "apps/desktop/flow_macro.h"
#include "apps/desktop/flow_timeline_editor.h"
#include "apps/desktop/controls/timeline/timeline.h"
#include "apps/desktop/controls/equipment_panel/equipment_panel.h"
#include "apps/desktop/controls/equipment_panel/equipment_detail_card.h"
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

    void iconAccentExtraction()
    {
        namespace theme = desktop::theme;
        QCOMPARE(theme::IconAccent(QImage{}), theme::DefaultAccent());
        QImage gray(64, 64, QImage::Format_ARGB32);
        gray.fill(QColor("#999999"));
        QCOMPARE(theme::IconAccent(gray), theme::DefaultAccent());
        for (const QColor source : {QColor("#ed3020"), QColor("#20b060"), QColor("#244add"), QColor("#e4b020")}) {
            QImage image(64, 64, QImage::Format_ARGB32);
            image.fill(qRgba(255, 0, 255, 0));
            QPainter painter(&image);
            painter.fillRect(QRect(4, 4, 56, 56), QColor("#080808"));
            painter.fillRect(QRect(8, 8, 48, 48), source);
            painter.fillRect(QRect(24, 24, 16, 16), Qt::white);
            painter.end();
            const auto accent = theme::IconAccent(image);
            auto distance = std::abs(accent.hslHueF() - source.hslHueF());
            QVERIFY(std::min(distance, 1 - distance) < .02);
            QCOMPARE(theme::IconAccent(image), accent);
            QVERIFY(theme::Contrast(accent, QColor(theme::Surface)) >= 3);
            const auto colors = theme::MakeAccentColors(accent);
            for (const auto &background : {colors.normal, colors.hover, colors.pressed})
                QVERIFY(theme::Contrast(background, colors.text) >= 4.5);
        }
        const auto taiXu = theme::IconAccent(desktop::GameIcon(8484).pixmap(QSize(64, 64), 1.0).toImage());
        const auto moWen = theme::IconAccent(desktop::GameIcon(8501).pixmap(QSize(64, 64), 1.0).toImage());
        QVERIFY(taiXu != moWen);
        QVERIFY(taiXu != theme::DefaultAccent() && moWen != theme::DefaultAccent());
        qInfo().noquote() << "Icon-derived accents:" << taiXu.name() << moWen.name();
    }

    void specializationThemeTracksIcon()
    {
        namespace theme = desktop::theme;
        Workbench window;
        window.resize(1206, 860);
        window.show();
        auto *choice = window.findChild<QComboBox *>("specialization");
        auto *options = window.findChild<SimulationOptions *>();
        auto *start = window.findChild<QToolButton *>("startButton");
        auto *editor = window.findChild<MacroEditor *>("macroEditor");
        editor->appendPlainText(QStringLiteral("\n# 保留主题切换前的宏"));
        editor->SetBreakpoint(1, true);
        const auto source = editor->toPlainText();
        const auto snapshot = options->Snapshot();
        auto expected = [&](int index) { return theme::IconAccent(choice->itemIcon(index).pixmap(QSize(64, 64), 1.0).toImage()); };
        auto pixel = [](QWidget *widget, QPoint point) {
            const auto image = widget->grab().toImage();
            return image.pixelColor(qRound(point.x() * image.devicePixelRatio()), qRound(point.y() * image.devicePixelRatio()));
        };
        for (int index : {0, 1, 0}) {
            choice->setCurrentIndex(index);
            QTest::qWait(100);
            const auto accent = expected(index);
            QCOMPARE(theme::AccentColor(&window), accent);
            QCOMPARE(theme::AccentColor(start), accent);
            QCOMPARE(start->palette().color(QPalette::Accent), accent);
            QVERIFY(!window.styleSheet().contains("#a8b3ff"));
            start->setAttribute(Qt::WA_UnderMouse, false);
            QCOMPARE(pixel(start, QPoint(10, start->height() / 2)), QColor(accent.name()));
            QCOMPARE(pixel(choice, QPoint(1, choice->height() / 2)), QColor(accent.name()));
            const auto stable = options->Snapshot();
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
            QCOMPARE(options->Snapshot(), stable);
            QCOMPARE(QColor(theme::Success), QColor("#6fd99b"));
            QCOMPARE(QColor(theme::Error), QColor("#f08078"));
            if (qEnvironmentVariableIsSet("JX3DPS_TEST_SCREENSHOT")) {
                QTest::qWait(desktop::motion::SlideMs + 40);
                QVERIFY(window.grab().save(qEnvironmentVariable("JX3DPS_TEST_SCREENSHOT") + QStringLiteral("-theme-class%1.png").arg(index)));
            }
        }
        QCOMPARE(editor->toPlainText(), source);
        QVERIFY(editor->GetBreakpoints().contains(1));
        QCOMPARE(options->Snapshot(), snapshot);

        // Follow the artwork actually displayed by the selector, not a hard-coded class/color mapping.
        const auto original = choice->itemIcon(0);
        QPixmap changed(64, 64);
        changed.fill(QColor("#e15f27"));
        choice->setItemIcon(0, QIcon(changed));
        choice->setCurrentIndex(1);
        choice->setCurrentIndex(0);
        QCOMPARE(theme::AccentColor(&window), theme::IconAccent(changed.toImage()));
        choice->setItemIcon(0, original);
        choice->setCurrentIndex(1);
        choice->setCurrentIndex(0);

        const auto firstAccent = theme::AccentColor(&window);
        QSettings().setValue("simulation/specialization", 1);
        Workbench restored;
        QCOMPARE(restored.findChild<QComboBox *>("specialization")->currentIndex(), 1);
        QCOMPARE(theme::AccentColor(&restored), expected(1));
        QCOMPARE(theme::AccentColor(&window), firstAccent);
        QCOMPARE(theme::AccentColor(start), firstAccent);
    }

    void gameAttributeOrderAndEquipmentTalents()
    {
        Workbench window;
        window.resize(1206, 860);
        window.show();
        QTest::qWait(100);
        auto *options = window.findChild<SimulationOptions *>();
        auto *attributePanel = window.findChild<QWidget *>("gameAttributePanel");
        auto *gainsPanel = window.findChild<QWidget *>("attributeGainsModule");
        QVERIFY(attributePanel && gainsPanel);
        QVERIFY(std::abs(attributePanel->width() - gainsPanel->width()) <= 1);
        for (auto *panel : {attributePanel, gainsPanel}) {
            auto *title = panel->findChild<QLabel *>("attributeModuleTitle");
            auto *header = panel->findChild<QWidget *>("attributeModuleHeader");
            auto *body = panel->findChild<QWidget *>("attributeModuleBody");
            QVERIFY(title && header && body);
            QVERIFY(!panel->findChild<QWidget *>("attributeModuleDivider"));
            QVERIFY(!body->isAncestorOf(title));
            QVERIFY(header->geometry().bottom() < body->geometry().top());
            QVERIFY(body->property("statModule").toBool());
            QVERIFY(!panel->property("statModule").toBool());
            QCOMPARE(title->font().pixelSize(), window.findChild<QLabel *>("gameAttributeValue_attack")->font().pixelSize());
            QVERIFY(title->font().weight() >= QFont::DemiBold);
            auto *firstRow = panel->findChild<QWidget *>(panel == attributePanel ? "gameAttributeRow_major" : "gameAttributeGainRow_major");
            QVERIFY(body->isAncestorOf(firstRow));
            QVERIFY(firstRow->mapTo(body, QPoint()).y() >= 6);
        }
        for (const auto *name : {"runSettings", "permanentPanel", "macroPanel"}) {
            auto *box = window.findChild<QGroupBox *>(QString::fromLatin1(name));
            QVERIFY(box);
            QStyleOptionGroupBox option;
            option.initFrom(box);
            option.text = box->title();
            option.textAlignment = Qt::AlignLeft;
            option.subControls = QStyle::SC_GroupBoxFrame | QStyle::SC_GroupBoxLabel;
            const auto title = box->style()->subControlRect(QStyle::CC_GroupBox, &option, QStyle::SC_GroupBoxLabel, box);
            const auto frame = box->style()->subControlRect(QStyle::CC_GroupBox, &option, QStyle::SC_GroupBoxFrame, box);
            QVERIFY2(title.bottom() < frame.top(), name);
        }
        QVERIFY(!window.findChild<QTabWidget *>("attributeTabs"));
        QVERIFY(gainsPanel->mapTo(&window, QPoint()).x() > attributePanel->mapTo(&window, attributePanel->rect().topRight()).x());
        auto capturePair = [&](const QString &suffix) {
            if (!qEnvironmentVariableIsSet("JX3DPS_TEST_SCREENSHOT")) return;
            const QRect left(attributePanel->mapTo(&window, QPoint()), attributePanel->size());
            const QRect right(gainsPanel->mapTo(&window, QPoint()), gainsPanel->size());
            QVERIFY(window.grab(left.united(right)).save(qEnvironmentVariable("JX3DPS_TEST_SCREENSHOT") + suffix));
        };
        int previousY = -1;
        for (int i = 0; i < int(desktop::GameAttributeRows.size()); ++i) {
            const auto key = QString::fromLatin1(desktop::GameAttributeRows[i].key);
            auto *row = window.findChild<QWidget *>("gameAttributeRow_" + key);
            auto *value = window.findChild<QLabel *>("gameAttributeValue_" + key);
            auto *gainRow = window.findChild<QWidget *>("gameAttributeGainRow_" + key);
            QVERIFY(row && value && gainRow);
            QCOMPARE(row->property("attributeKey"), gainRow->property("attributeKey"));
            QCOMPARE(row->mapTo(&window, row->rect().center()).y(), gainRow->mapTo(&window, gainRow->rect().center()).y());
            QCOMPARE(row->height(), gainRow->height());
            QVERIFY(row->y() > previousY);
            previousY = row->y();
            QCOMPARE(row->findChildren<QLabel *>().first()->text(), desktop::GameAttributeName(i, desktop::Specialization::TaiXu));
            QVERIFY(value->alignment().testFlag(Qt::AlignRight));
            QVERIFY(row->findChildren<QLabel *>().first()->width() >= 56);
            QVERIFY(value->width() >= 88);
            QCOMPARE(value->geometry().right(), row->rect().right());
        }
        auto *import = window.findChild<QPushButton *>("importAttributes");
        auto *calculateButton = window.findChild<QToolButton *>("calculateInlineGains");
        QCOMPARE(import->mapTo(attributePanel, QPoint()).x(), 12);
        QCOMPARE(import->height(), calculateButton->height());
        QCOMPARE(import->mapTo(&window, QPoint()).y(), calculateButton->mapTo(&window, QPoint()).y());
        QVERIFY(import->property("moduleAction").toBool());
        QVERIFY(calculateButton->property("magpieCheckable").toBool());
        QVERIFY(calculateButton->isCheckable());
        QVERIFY(window.findChild<QLabel *>("gameAttributeGainStatus_major")->isVisible());
        QVERIFY(window.findChild<QLabel *>("gameAttributeGainStatus_hit")->isVisible());
        QVERIFY(!window.findChild<QLabel *>("gameAttributeGain_major")->isVisible());
        QCOMPARE(window.findChild<QLabel *>("gameAttributeGainChange_attack")->text(), QStringLiteral("--"));
        auto *attributeRow = window.findChild<QWidget *>("gameAttributeRow_critical");
        auto *gainRow = window.findChild<QWidget *>("gameAttributeGainRow_critical");
        QEnterEvent enter(QPointF(3, 3), QPointF(3, 3), QPointF(attributeRow->mapToGlobal(QPoint(3, 3))));
        QCoreApplication::sendEvent(attributeRow, &enter);
        QVERIFY(attributeRow->property("comparisonHighlighted").toBool());
        QVERIFY(gainRow->property("comparisonHighlighted").toBool());
        capturePair(QStringLiteral("-attribute-hover.png"));
        QEvent leave(QEvent::Leave);
        QCoreApplication::sendEvent(attributeRow, &leave);
        QVERIFY(!attributeRow->property("comparisonHighlighted").toBool());
        QVERIFY(!gainRow->property("comparisonHighlighted").toBool());
        QCoreApplication::sendEvent(gainRow, &enter);
        QVERIFY(attributeRow->property("comparisonHighlighted").toBool());
        QVERIFY(gainRow->property("comparisonHighlighted").toBool());
        window.findChild<QPushButton *>("nav_macro")->click();
        QVERIFY(!attributeRow->property("comparisonHighlighted").toBool());
        QVERIFY(!gainRow->property("comparisonHighlighted").toBool());
        window.findChild<QPushButton *>("nav_simulation")->click();
        QCoreApplication::processEvents();
        QCOMPARE(window.findChild<QLabel *>("gameAttributeValue_attack")->text(), QStringLiteral("12500"));
        QCOMPARE(window.findChild<QLabel *>("gameAttributeValue_hit")->text(), QStringLiteral("100.00%"));
        capturePair(QStringLiteral("-attribute-layout.png"));
        window.findChild<QPushButton *>("editGameAttributes")->click();
        auto *editor = window.findChild<QDialog *>("attributeValueEditor");
        QTRY_VERIFY(editor->isVisible());
        auto *attack = window.findChild<QSpinBox *>("attackBase");
        QVERIFY(attack->isVisible());
        attack->setValue(20000);
        QTRY_COMPARE(window.findChild<QLabel *>("gameAttributeValue_attack")->text(), QStringLiteral("22500"));
        editor->close();
        for (const QSize size : {QSize(1206, 860), QSize(1120, 820)}) {
            window.resize(size);
            for (int specialization : {0, 1}) {
                window.findChild<QComboBox *>("specialization")->setCurrentIndex(specialization);
                const auto before = options->Snapshot();
                window.findChild<QToolButton *>("openEquipment")->click();
                auto *equipment = window.findChild<QDialog *>("equipmentDialog");
                QTRY_VERIFY(equipment->isVisible());
                QVERIFY(equipment->isWindow());
                QVERIFY(!equipment->isModal());
                QTest::qWait(100);
                QVERIFY(attributePanel->isVisible());
                QVERIFY(gainsPanel->isVisible());
                QVERIFY(window.findChild<QWidget *>("configurationTabs")->isVisible());
                auto *talents = window.findChild<QWidget *>(QStringLiteral("talents%1").arg(specialization));
                for (int tier = 1; tier <= 9; ++tier) {
                    auto *button = talents->findChild<QToolButton *>(QStringLiteral("talentTier%1").arg(tier));
                    QVERIFY(button->isVisible());
                    QVERIFY(button->visibleRegion().contains(button->rect()));
                }
                QCOMPARE(options->Snapshot(), before);
                if (qEnvironmentVariableIsSet("JX3DPS_TEST_SCREENSHOT"))
                    QVERIFY(window.grab().save(qEnvironmentVariable("JX3DPS_TEST_SCREENSHOT") + QStringLiteral("-equipment-talents-%1-class%2.png").arg(size.width()).arg(specialization)));
                if (qEnvironmentVariableIsSet("JX3DPS_TEST_SCREENSHOT"))
                    QVERIFY(equipment->grab().save(qEnvironmentVariable("JX3DPS_TEST_SCREENSHOT") + QStringLiteral("-equipment-window-class%1.png").arg(specialization)));
                equipment->close();
            }
        }
        SimulationResult fixture;
        fixture.options.duration = 16;
        fixture.options.iterations = 1;
        fixture.stats.Add(1000, 0);
        fixture.gainsRequested = true;
        for (int i = 7; i >= 0; --i) {
            rt::BatchStats stats; stats.Add(1000 + (i + 1) * 100, 0);
            fixture.gains.push_back({desktop::AttributeGainName(i), 100, stats});
        }
        QTableWidget gains(0, 3);
        desktop::FillGameGainTable(&gains, fixture, desktop::Specialization::MoWen, false);
        QCOMPARE(gains.rowCount(), 10);
        for (int i = 0; i < 10; ++i) {
            QCOMPARE(gains.item(i, 0)->text(), desktop::GameAttributeName(i, desktop::Specialization::MoWen));
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
        auto *calculate = window.findChild<QAction *>("attributeGains");
        window.findChild<QToolButton *>("calculateInlineGains")->click();
        window.findChild<QToolButton *>("startButton")->click();
        QTRY_VERIFY_WITH_TIMEOUT(calculate->isEnabled(), 10000);
        QVERIFY(window.findChild<QLabel *>("gameAttributeGain_haste")->text() != QStringLiteral("--"));
        QTest::qWait(320);
        QVERIFY(window.findChild<QLabel *>("gameAttributeGain_haste")->isVisible());
        capturePair(QStringLiteral("-attribute-gains.png"));
        window.findChild<QToolButton *>("openEquipment")->click();
        QVERIFY(window.close());
        QVERIFY(!window.findChild<QDialog *>("equipmentDialog")->isVisible());
    }

    void permanentPanelPreview()
    {
        Workbench window;
        window.resize(1120, 820);
        window.show();
        auto *options = window.findChild<SimulationOptions *>();
        auto *toggle = window.findChild<QTabBar *>("permanentsInPanel");
        QVERIFY(toggle && toggle->currentIndex() == 0);
        QCOMPARE(toggle->tabText(0), QStringLiteral("计入面板"));
        QCOMPARE(toggle->tabText(1), QStringLiteral("基础面板"));
        QVERIFY(toggle->property("segmentedControl").toBool());
        QCOMPARE(toggle->height(), desktop::AttributeHeaderHeight);
        auto selectPanel = [&](bool included) {
            const int index = included ? 0 : 1;
            QTest::mouseClick(toggle, Qt::LeftButton, Qt::NoModifier, toggle->tabRect(index).center());
            QCOMPARE(toggle->currentIndex(), index);
        };
        auto *permanents = window.findChild<QWidget *>("permanentPanel");
        auto *header = permanents->findChild<QWidget *>("groupHeaderActions");
        QVERIFY(header);
        QCOMPARE(header->height(), desktop::AttributeHeaderHeight);
        QStyleOptionGroupBox titleOption;
        titleOption.initFrom(permanents);
        titleOption.text = qobject_cast<QGroupBox *>(permanents)->title();
        titleOption.textAlignment = Qt::AlignLeft;
        titleOption.subControls = QStyle::SC_GroupBoxFrame | QStyle::SC_GroupBoxLabel;
        const auto titleBounds = permanents->style()->subControlRect(QStyle::CC_GroupBox, &titleOption, QStyle::SC_GroupBoxLabel, permanents);
        QCoreApplication::processEvents();
        const auto frameBounds = permanents->style()->subControlRect(QStyle::CC_GroupBox, &titleOption, QStyle::SC_GroupBoxFrame, permanents);
        QCOMPARE(frameBounds.top() - toggle->mapTo(permanents, QPoint(0, toggle->height())).y(), desktop::AttributeHeaderSpacing);
        QVERIFY(std::abs(titleBounds.center().y() - toggle->mapTo(permanents, toggle->rect().center()).y()) <= 2);
        QVERIFY(titleBounds.right() + 6 <= toggle->mapTo(permanents, QPoint()).x());
        QVERIFY(toggle->visibleRegion().contains(toggle->rect()));
        for (int i = 0; i < toggle->count(); ++i)
            QVERIFY(toggle->tabRect(i).width() - 18 >= QFontMetrics(toggle->font()).horizontalAdvance(toggle->tabText(i)));
        auto valueLabel = [&](int i) {
            return window.findChild<QLabel *>(QStringLiteral("gameAttributeValue_%1").arg(QString::fromLatin1(desktop::GameAttributeRows[i].key)));
        };
        options->findChild<QSpinBox *>("importedMajor")->setValue(8000);
        options->findChild<QDoubleSpinBox *>("attackBonus")->setValue(5);
        for (int specialization : {0, 1}) {
            window.findChild<QComboBox *>("specialization")->setCurrentIndex(specialization);
            QTest::qWait(60);
            std::array<QString, 10> base;
            for (int i = 0; i < 10; ++i) base[i] = valueLabel(i)->property("displayValue").toString();
            for (auto *input : options->findChildren<QComboBox *>())
                if (input->objectName().startsWith(QStringLiteral("permanent%1").arg(specialization))) input->setCurrentIndex(1);
            QTest::qWait(60);
            const auto snapshot = options->Snapshot();
            const auto config = options->ActiveConfig();
            auto attack = [](const desktop::Config &c) {
                return std::visit([](const auto &v) { return v.attributes.attack_base; }, c);
            };
            QVERIFY(!snapshot.value("values").toObject().contains("permanentsInPanel"));
            for (int i = 0; i < 10; ++i) {
                auto *label = valueLabel(i);
                const auto current = label->property("displayValue").toString();
                auto raw = current; raw.remove('%');
                auto original = base[i]; original.remove('%');
                const auto delta = raw.toDouble() - original.toDouble();
                const auto difference = label->property("permanentDelta").toString();
                if (std::abs(delta) < .0001) QVERIFY(difference.isEmpty());
                else {
                    auto number = difference; number.remove('%');
                    QVERIFY(std::abs(number.toDouble() - delta) < .0001);
                    QVERIFY(label->text().contains(desktop::theme::Success));
                    QVERIFY(label->text().contains('(') && label->text().contains(')'));
                }
                QVERIFY(label->visibleRegion().contains(label->rect()));
                QTextDocument text;
                text.setDocumentMargin(0);
                text.setDefaultFont(label->font());
                text.setHtml(label->text());
                QCOMPARE(text.toPlainText(), difference.isEmpty() ? current : '(' + difference + ") " + current);
                QVERIFY2(text.idealWidth() <= label->contentsRect().width() + 1,
                    qPrintable(QStringLiteral("%1: text %2, available %3").arg(label->objectName()).arg(text.idealWidth()).arg(label->width())));
            }
            for (const auto *name : {"attributesScroll", "configurationScroll", "permanentsScroll"}) {
                auto *scroll = window.findChild<QScrollArea *>(QString::fromLatin1(name));
                QCOMPARE(scroll->horizontalScrollBar()->maximum(), 0);
                QCOMPARE(scroll->verticalScrollBar()->maximum(), 0);
            }
            window.findChild<QPushButton *>("nav_attributes")->click();
            QCoreApplication::processEvents();
            for (int i = 0; i < 10; ++i) {
                auto *label = valueLabel(i);
                QTextDocument text;
                text.setDocumentMargin(0);
                text.setDefaultFont(label->font());
                text.setHtml(label->text());
                QVERIFY2(text.idealWidth() <= label->contentsRect().width() + 1, qPrintable(label->text()));
                QVERIFY(label->visibleRegion().contains(label->rect()));
                auto *gain = window.findChild<QWidget *>(QStringLiteral("gameAttributeGainRow_%1")
                    .arg(QString::fromLatin1(desktop::GameAttributeRows[i].key)));
                QCOMPARE(label->mapTo(&window, label->rect().center()).y(), gain->mapTo(&window, gain->rect().center()).y());
            }
            QCOMPARE(window.findChild<QScrollArea *>("attributesScroll")->horizontalScrollBar()->maximum(), 0);
            window.findChild<QPushButton *>("nav_simulation")->click();
            QTest::qWait(desktop::motion::SlideMs + 40);
            if (qEnvironmentVariableIsSet("JX3DPS_TEST_SCREENSHOT")) {
                QVERIFY(window.grab().save(qEnvironmentVariable("JX3DPS_TEST_SCREENSHOT") + QStringLiteral("-permanents-on-class%1.png").arg(specialization)));
                auto *attributes = window.findChild<QWidget *>("gameAttributePanel");
                const QRect top(permanents->mapTo(&window, QPoint()), permanents->size());
                const QRect bottom(attributes->mapTo(&window, QPoint()), attributes->size());
                QVERIFY(window.grab(top.united(bottom)).save(qEnvironmentVariable("JX3DPS_TEST_SCREENSHOT") +
                    QStringLiteral("-permanents-detail-class%1.png").arg(specialization)));
            }

            SimulationResult fixture;
            fixture.options.duration = 16;
            fixture.options.iterations = 1;
            fixture.stats.Add(1000, 0);
            fixture.gainsRequested = true;
            for (int i = 0; i < 8; ++i) {
                rt::BatchStats stats; stats.Add(1100, 0);
                fixture.gains.push_back({desktop::AttributeGainName(i), 100, stats});
            }
            options->SetAttributeGainResult(fixture);
            auto *gain = window.findChild<QLabel *>("gameAttributeGain_attack");
            const auto gainText = gain->text();
            QVERIFY(gainText != QStringLiteral("--"));
            options->SetInputsEnabled(false);
            QVERIFY(toggle->isEnabled());
            QVERIFY(!window.findChild<QScrollArea *>("permanentsScroll")->isEnabled());
            selectPanel(false);
            QCOMPARE(options->Snapshot(), snapshot);
            QCOMPARE(attack(options->ActiveConfig()), attack(config));
            QCOMPARE(gain->text(), gainText);
            for (int i = 0; i < 10; ++i) {
                QCOMPARE(valueLabel(i)->property("displayValue").toString(), base[i]);
                QVERIFY(valueLabel(i)->property("permanentDelta").toString().isEmpty());
                QVERIFY(!valueLabel(i)->text().contains("<span"));
            }
            options->SetInputsEnabled(true);
            if (qEnvironmentVariableIsSet("JX3DPS_TEST_SCREENSHOT"))
                QVERIFY(window.grab().save(qEnvironmentVariable("JX3DPS_TEST_SCREENSHOT") + QStringLiteral("-permanents-off-class%1.png").arg(specialization)));
            selectPanel(true);
            QCOMPARE(gain->text(), gainText);
        }
        // Exercise negative effects without adding fabricated buffs to the production catalog.
        for (auto *input : options->findChildren<QComboBox *>())
            if (input->objectName().startsWith("permanent")) input->setCurrentIndex(0);
        auto *food = options->findChild<QComboBox *>("permanent1FoodEnhance");
        food->setItemData(1, QJsonArray{QJsonObject{{"Type", QStringLiteral("基础攻击")}, {"Value", -500}},
            QJsonObject{{"Type", QStringLiteral("会心等级")}, {"Value", -1000}}});
        food->setCurrentIndex(1);
        QTest::qWait(60);
        for (int i : {1, 3}) {
            QVERIFY(valueLabel(i)->property("permanentDelta").toString().startsWith('-'));
            QVERIFY(valueLabel(i)->text().contains(desktop::theme::Error));
            QTextDocument text;
            text.setHtml(valueLabel(i)->text());
            QVERIFY(text.toPlainText().startsWith("(-"));
            QVERIFY(text.toPlainText().endsWith(valueLabel(i)->property("displayValue").toString()));
        }
        food->setCurrentIndex(0);
        options->findChild<QSpinBox *>("importedMajor")->setValue(0);
        options->findChild<QComboBox *>("permanent1FoodSupport")->setCurrentIndex(1);
        QTRY_COMPARE(valueLabel(0)->property("displayValue").toString(), QStringLiteral("--"));
        QTRY_COMPARE(valueLabel(0)->property("permanentDelta").toString(), QStringLiteral("+347"));
        selectPanel(false);
        options->SaveSettings();
        SimulationOptions restored;
        QCOMPARE(restored.findChild<QTabBar *>("permanentsInPanel")->currentIndex(), 1);
        QCOMPARE(restored.Snapshot(), options->Snapshot());
    }

    void attributeGainComparisonModes()
    {
        Workbench window;
        window.resize(1120, 820);
        window.show();
        window.findChild<QSpinBox *>("iterations")->setValue(4);
        window.findChild<QDoubleSpinBox *>("seconds")->setValue(10);
        QTest::qWait(100);
        auto *options = window.findChild<SimulationOptions *>();
        auto *modes = window.findChild<QTabBar *>("attributeGainMode");
        auto *gain = window.findChild<QLabel *>("gameAttributeGain_attack");
        auto *action = window.findChild<QAction *>("attributeGains");
        auto *calculate = window.findChild<QToolButton *>("calculateInlineGains");
        QCOMPARE(modes->count(), 2);
        QCOMPARE(modes->tabText(0), QStringLiteral("同数值"));
        QCOMPARE(modes->tabText(1), QStringLiteral("同费用"));
        QVERIFY(modes->tabToolTip(1).contains(QStringLiteral("同属性费用")));
        QVERIFY(modes->visibleRegion().contains(modes->rect()));
        for (int i = 0; i < modes->count(); ++i)
            QVERIFY(modes->tabRect(i).width() - 18 >= QFontMetrics(modes->font()).horizontalAdvance(modes->tabText(i)));
        const auto snapshot = options->Snapshot();
        auto select = [&](int index) {
            QTest::mouseClick(modes, Qt::LeftButton, Qt::NoModifier, modes->tabRect(index).center());
            QCOMPARE(modes->currentIndex(), index);
            QCOMPARE(options->Snapshot(), snapshot);
        };
        auto capture = [&](const QString &suffix) {
            if (!qEnvironmentVariableIsSet("JX3DPS_TEST_SCREENSHOT")) return;
            QTest::qWait(desktop::motion::SlideMs + 40);
            auto *attributes = window.findChild<QWidget *>("gameAttributePanel");
            auto *gains = window.findChild<QWidget *>("attributeGainsModule");
            const QRect left(attributes->mapTo(&window, QPoint()), attributes->size());
            const QRect right(gains->mapTo(&window, QPoint()), gains->size());
            QVERIFY(window.grab(left.united(right)).save(qEnvironmentVariable("JX3DPS_TEST_SCREENSHOT") + suffix));
        };
        select(1);
        QCOMPARE(options->GainMode(), desktop::GainComparisonMode::SameCost);
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
        QVERIFY(gain->toolTip().contains(QStringLiteral("398")));
        const auto costValue = gain->text();
        capture(QStringLiteral("-same-cost.png"));
        QTemporaryDir directory;
        QString error;
        const auto path = directory.filePath(QStringLiteral("cost-result.json"));
        QVERIFY(window.findChild<ResultsPanel *>()->ExportSelected(path, error));
        QFile report(path);
        QVERIFY(report.open(QIODevice::ReadOnly));
        const auto data = QJsonDocument::fromJson(report.readAll()).object();
        QCOMPARE(data.value("result").toObject().value("gainComparisonMode").toString(), QStringLiteral("same_cost"));
        QCOMPARE(data.value("gains").toArray()[0].toObject().value("increment").toInt(), 398);
        QCOMPARE(data.value("gains").toArray()[6].toObject().value("increment").toInt(), 883);
        select(0);
        QVERIFY(calculate->isChecked());
        window.findChild<QToolButton *>("startButton")->click();
        QTRY_VERIFY_WITH_TIMEOUT(action->isEnabled(), 10000);
        QVERIFY(gain->text() != QStringLiteral("--"));
        QVERIFY(gain->toolTip().contains(QStringLiteral("增加 100 点")));
        const auto value = gain->text();
        capture(QStringLiteral("-same-value.png"));
        select(1);
        QCOMPARE(gain->text(), costValue);
        select(0);
        QCOMPARE(gain->text(), value);
        window.findChild<MacroEditor *>("macroEditor")->appendPlainText(QStringLiteral("\n# 清除两种收益缓存"));
        QCOMPARE(gain->text(), QStringLiteral("--"));
        select(1);
        QCOMPARE(gain->text(), QStringLiteral("--"));
        QCOMPARE(QSettings().value("interface/gainComparisonMode").toInt(), 1);
        desktop::AttributeGainsPanel restored;
        QCOMPARE(restored.Mode(), desktop::GainComparisonMode::SameCost);
        QVERIFY(restored.calculate->isChecked());
    }

    void automaticGainsFollowSimulation()
    {
        Workbench window;
        window.resize(1120, 820);
        window.show();
        auto *options = window.findChild<SimulationOptions *>();
        auto *calculate = window.findChild<QToolButton *>("calculateInlineGains");
        auto *start = window.findChild<QToolButton *>("startButton");
        auto *action = window.findChild<QAction *>("attributeGains");
        auto *history = window.findChild<QTableWidget *>("resultHistory");
        window.findChild<QSpinBox *>("iterations")->setValue(4);
        window.findChild<QSpinBox *>("workers")->setValue(2);
        window.findChild<QDoubleSpinBox *>("seconds")->setValue(5);
        QTest::qWait(100);
        QVERIFY(calculate->isCheckable() && !calculate->isChecked());
        QVERIFY(!options->AutoCalculateGains());
        QTemporaryDir directory;
        auto report = [&]() -> QJsonObject {
            QString error;
            const auto path = directory.filePath(QStringLiteral("auto-gains.json"));
            if (!window.findChild<ResultsPanel *>()->ExportSelected(path, error)) return {};
            QFile file(path);
            if (!file.open(QIODevice::ReadOnly)) return {};
            return QJsonDocument::fromJson(file.readAll()).object();
        };
        auto run = [&](auto launch, quint64 requested) {
            launch();
            auto *worker = window.findChild<BatchWorker *>();
            QVERIFY(worker);
            QCOMPARE(worker->Requested(), requested);
            QVERIFY(!calculate->isEnabled());
            QTRY_VERIFY_WITH_TIMEOUT(start->isEnabled(), 10000);
            QTRY_VERIFY(window.findChildren<BatchWorker *>().isEmpty());
        };
        run([&] { start->click(); }, 4);
        auto data = report();
        QVERIFY(!data.isEmpty());
        QVERIFY(!data.value("result").toObject().value("gainsRequested").toBool());
        QVERIFY(data.value("gains").toArray().isEmpty());
        const auto snapshot = options->Snapshot();
        const int count = history->rowCount();
        calculate->click();
        QTest::qWait(200);
        QVERIFY(calculate->isChecked() && action->isChecked());
        QCOMPARE(history->rowCount(), count);
        QVERIFY(window.findChildren<BatchWorker *>().isEmpty());
        QCOMPARE(options->Snapshot(), snapshot);
        const auto expected = desktop::theme::Blend(QColor(desktop::theme::Surface), desktop::theme::AccentColor(calculate), .16);
        QVERIFY(calculate->isEnabled());
        QColor checkedFill;
        auto checkedFillSettled = [&] {
            const auto image = calculate->grab().toImage();
            const auto actual = checkedFill = image.pixelColor(qRound(3 * image.devicePixelRatio()), qRound(13 * image.devicePixelRatio()));
            return std::abs(actual.red() - expected.red()) <= 1 && std::abs(actual.green() - expected.green()) <= 1
                && std::abs(actual.blue() - expected.blue()) <= 1;
        };
        QTRY_VERIFY2_WITH_TIMEOUT(checkedFillSettled(), qPrintable(QStringLiteral("checked fill %1, expected %2")
            .arg(checkedFill.name(), expected.name())), 2000);
        run([&] { start->click(); }, 12);
        data = report();
        QVERIFY(data.value("result").toObject().value("gainsRequested").toBool());
        QCOMPARE(data.value("gains").toArray().size(), 8);
        QCOMPARE(data.value("gains").toArray()[0].toObject().value("increment").toInt(), 100);

        window.findChild<QTabBar *>("attributeGainMode")->setCurrentIndex(1);
        window.findChild<QPushButton *>("nav_macro")->click();
        QCoreApplication::processEvents();
        run([&] { window.findChild<QToolButton *>("runMacroButton")->click(); }, 12);
        data = report();
        QCOMPARE(data.value("result").toObject().value("gainComparisonMode").toString(), QStringLiteral("same_cost"));
        QCOMPARE(data.value("gains").toArray()[0].toObject().value("increment").toInt(), 398);
        run([&] { window.findChild<QAction *>("singleTrace")->trigger(); }, 1);
        data = report();
        QVERIFY(data.value("result").toObject().value("trace").toBool());
        QVERIFY(!data.value("result").toObject().value("gainsRequested").toBool());
        QVERIFY(calculate->isChecked());

        window.findChild<QToolButton *>("debugButton")->setChecked(true);
        window.findChild<QAction *>("debugStart")->trigger();
        QTRY_COMPARE(window.findChild<DebugSession *>()->GetState(), DebugSession::Paused);
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
        for (const auto *name : {"Attributes", "Equipment", "Gains"}) tabs.addTab(new QWidget, QString::fromLatin1(name));
        tabs.resize(460, 240); tabs.show(); QCoreApplication::processEvents();
        auto *bar = static_cast<desktop::SegmentedTabBar *>(tabs.tabBar());
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
        const auto mid = bar->IndicatorRect();
        QVERIFY(mid.left() > first.left());
        tabs.setCurrentIndex(2);
        QVERIFY(std::abs(bar->IndicatorRect().left() - mid.left()) < 2);
        QTRY_VERIFY(!bar->SelectionAnimating());
        QTest::keyClick(bar, Qt::Key_Left);
        QVERIFY(bar->KeyboardFocusVisible());

        Workbench window; window.show(); QTest::qWait(100);
        auto *nav = static_cast<desktop::NavigationStrip *>(window.findChild<QWidget *>("centeredNavigation"));
        auto *button = static_cast<desktop::NavigationButton *>(window.findChild<QPushButton *>("nav_macro"));
        const auto origin = nav->IndicatorRect();
        QTest::mousePress(button, Qt::LeftButton);
        QTest::qWait(70);
        QVERIFY(button->PressScale() < 1);
        QVERIFY(window.findChild<QPushButton *>("nav_simulation")->isChecked());
        QTest::mouseRelease(button, Qt::LeftButton);
        QVERIFY(button->isChecked());
        QVERIFY(nav->SelectionAnimating());
        QVERIFY(!button->KeyboardFocusVisible());
        QTest::qWait(90);
        QVERIFY(nav->IndicatorRect().left() > origin.left());
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
        auto checkModuleAlignment = [&] {
            auto *attributes = window.findChild<QWidget *>("gameAttributePanel");
            auto *gains = window.findChild<QWidget *>("attributeGainsModule");
            auto *modes = gains->findChild<QTabBar *>("attributeGainMode");
            auto *gainHeader = gains->findChild<QWidget *>("attributeModuleHeader");
            auto *calculate = gains->findChild<QToolButton *>("calculateInlineGains");
            auto *actions = gains->findChild<QWidget *>("attributeModuleActions");
            auto *talents = window.findChild<QTabWidget *>("configurationTabs");
            auto *flow = window.findChild<QGroupBox *>("flowMacroPanel");
            auto *tabs = talents->tabBar();
            QCOMPARE(modes->height(), desktop::AttributeActionHeight);
            QCOMPARE(modes->height(), calculate->height());
            QVERIFY(actions->isAncestorOf(modes) && actions->isAncestorOf(calculate));
            QCOMPARE(modes->mapTo(&window, QPoint()).y(), calculate->mapTo(&window, QPoint()).y());
            QVERIFY(modes->mapTo(&window, modes->rect().topRight()).x() < calculate->mapTo(&window, QPoint()).x());
            QCOMPARE(tabs->height(), gainHeader->height());
            const auto talentBodyTop = talents->currentWidget()->mapTo(&window, QPoint()).y();
            const auto gainBodyTop = gains->findChild<QWidget *>("attributeModuleBody")->mapTo(&window, QPoint()).y();
            const auto gainGap = gainBodyTop - gainHeader->mapTo(&window, QPoint(0, gainHeader->height())).y();
            QCOMPARE(gainGap, desktop::AttributeHeaderSpacing);
            QCOMPARE(talentBodyTop - tabs->mapTo(&window, QPoint(0, tabs->height())).y(), desktop::AttributeHeaderSpacing);
            QCOMPARE(flow->mapTo(&window, QPoint()).y(), gains->mapTo(&window, QPoint()).y());
            QCOMPARE(flow->height(), gains->height());
            QCOMPARE(talents->height(), 100);
            QCOMPARE(talents->mapTo(&window, QPoint()).x(), attributes->mapTo(&window, QPoint()).x());
            QCOMPARE(talents->mapTo(&window, talents->rect().topRight()).x(), flow->mapTo(&window, flow->rect().topRight()).x());
            auto *permanents = window.findChild<QGroupBox *>("permanentPanel");
            if (permanents->isVisible()) {
                auto frameTop = [&](QGroupBox *box) {
                    QStyleOptionGroupBox option;
                    option.initFrom(box);
                    option.text = box->title();
                    option.subControls = QStyle::SC_GroupBoxFrame | QStyle::SC_GroupBoxLabel;
                    const auto frame = box->style()->subControlRect(QStyle::CC_GroupBox, &option, QStyle::SC_GroupBoxFrame, box);
                    return box->mapTo(&window, frame.topLeft()).y();
                };
                auto *permanentMode = permanents->findChild<QTabBar *>("permanentsInPanel");
                QCOMPARE(frameTop(permanents) - permanentMode->mapTo(&window, QPoint(0, permanentMode->height())).y(), gainGap);
                QCOMPARE(frameTop(window.findChild<QGroupBox *>("runSettings")), frameTop(permanents));
            }
            for (auto *panel : {attributes, gains}) {
                auto *body = panel->findChild<QWidget *>("attributeModuleBody");
                QCOMPARE(body->mapTo(&window, QPoint()).y(), gainBodyTop);
                QCOMPARE(panel->height(), gains->height());
                QCOMPARE(panel->mapTo(&window, panel->rect().bottomLeft()).y(),
                    gains->mapTo(&window, gains->rect().bottomLeft()).y());
                QVERIFY(talents->mapTo(&window, QPoint()).y() >= panel->mapTo(&window, panel->rect().bottomLeft()).y() + 4);
            }
        };
        for (const QSize size : {QSize(1206, 860), QSize(1280, 840), QSize(1120, 820)}) {
            window.resize(size);
            window.findChild<QPushButton *>("nav_simulation")->click();
            for (int specialization : {0, 1}) {
                window.findChild<QComboBox *>("specialization")->setCurrentIndex(specialization);
                QTest::qWait(100);
                QCOMPARE(window.size(), size);
                checkModuleAlignment();
                auto *navigation = window.findChild<QWidget *>("topNavigation");
                auto *items = window.findChild<QWidget *>("centeredNavigation");
                const qreal center = items->mapTo(navigation, QPoint()).x() + items->width() / 2.0;
                QVERIFY(std::abs(center - navigation->width() / 2.0) <= 1);
                if (qEnvironmentVariableIsSet("JX3DPS_TEST_SCREENSHOT")) {
                    QVERIFY(window.grab().save(qEnvironmentVariable("JX3DPS_TEST_SCREENSHOT") +
                        QStringLiteral("-complete-%1x%2-class%3.png").arg(size.width()).arg(size.height()).arg(specialization)));
                    QVERIFY(window.findChild<QWidget *>("simulationSettingsRow")->grab().save(qEnvironmentVariable("JX3DPS_TEST_SCREENSHOT") +
                        QStringLiteral("-upper-%1x%2-class%3.png").arg(size.width()).arg(size.height()).arg(specialization)));
                    auto *attributes = window.findChild<QWidget *>("gameAttributePanel");
                    auto *talents = window.findChild<QWidget *>("configurationTabs");
                    const QRect left(attributes->mapTo(&window, QPoint()), attributes->size());
                    const QRect right(talents->mapTo(&window, QPoint()), talents->size());
                    QVERIFY(window.grab(left.united(right)).save(qEnvironmentVariable("JX3DPS_TEST_SCREENSHOT") +
                        QStringLiteral("-modules-%1x%2-class%3.png").arg(size.width()).arg(size.height()).arg(specialization)));
                }
                for (const auto &name : {QStringLiteral("attributesScroll"), QStringLiteral("talentsScroll%1").arg(specialization), QStringLiteral("configurationScroll")}) {
                    auto *scroll = window.findChild<QScrollArea *>(name);
                    QVERIFY(scroll);
                    QVERIFY2(scroll->verticalScrollBar()->maximum() == 0,
                        qPrintable(QStringLiteral("%1 at %2x%3: viewport %4x%5, content %6x%7, vertical range %8")
                        .arg(name).arg(size.width()).arg(size.height()).arg(scroll->viewport()->width()).arg(scroll->viewport()->height())
                        .arg(scroll->widget()->width()).arg(scroll->widget()->height()).arg(scroll->verticalScrollBar()->maximum())));
                    QCOMPARE(scroll->horizontalScrollBar()->maximum(), 0);
                }
                for (const auto &row : desktop::GameAttributeRows) {
                    auto *value = window.findChild<QLabel *>(QStringLiteral("gameAttributeValue_%1").arg(QString::fromLatin1(row.key)));
                    QVERIFY2(value->isVisible() && value->visibleRegion().contains(value->rect()), row.key);
                    auto *gainRow = window.findChild<QWidget *>(QStringLiteral("gameAttributeGainRow_%1").arg(QString::fromLatin1(row.key)));
                    QVERIFY2(gainRow->isVisible() && gainRow->visibleRegion().contains(gainRow->rect()), row.key);
                    QCOMPARE(value->mapTo(&window, value->rect().center()).y(), gainRow->mapTo(&window, gainRow->rect().center()).y());
                }
                int talentY = -1, previousX = -1;
                for (int tier = 1; tier <= 9; ++tier) {
                    auto *button = window.findChild<QWidget *>(QStringLiteral("talents%1").arg(specialization))
                        ->findChild<QToolButton *>(QStringLiteral("talentTier%1").arg(tier));
                    QVERIFY2(button->isVisible() && button->visibleRegion().contains(button->rect()), qPrintable(button->objectName()));
                    const auto position = button->mapTo(&window, QPoint());
                    if (talentY >= 0) QCOMPARE(position.y(), talentY);
                    QVERIFY(position.x() > previousX);
                    talentY = position.y(); previousX = position.x();
                }
                auto *run = window.findChild<QWidget *>("runSettings");
                QVERIFY(run->findChildren<QScrollArea *>().isEmpty());
                QVERIFY(!window.findChild<QTabWidget *>("runTabs"));
                for (const auto *key : {"iterations", "workers", "seconds", "seed", "delay"}) {
                    auto *input = window.findChild<QWidget *>(QString::fromLatin1(key));
                    QVERIFY2(input->visibleRegion().contains(input->rect()), key);
                }
                for (const auto *page : {"attributes", "talents"}) {
                    window.findChild<QPushButton *>(QStringLiteral("nav_%1").arg(QString::fromLatin1(page)))->click();
                    QCoreApplication::processEvents();
                    const auto name = QByteArray(page) == "attributes" ? QStringLiteral("attributesScroll")
                        : QStringLiteral("talentsScroll%1").arg(specialization);
                    auto *scroll = window.findChild<QScrollArea *>(name);
                    QCOMPARE(scroll->verticalScrollBar()->maximum(), 0);
                    QCOMPARE(scroll->horizontalScrollBar()->maximum(), 0);
                    if (QByteArray(page) == "attributes") {
                        checkModuleAlignment();
                        auto *attributes = window.findChild<QWidget *>("gameAttributePanel");
                        auto *gains = window.findChild<QWidget *>("attributeGainsModule");
                        QVERIFY(std::abs(attributes->width() - gains->width()) <= 1);
                        QVERIFY(attributes->height() <= 270 && gains->height() <= 270);
                        auto *modes = gains->findChild<QTabBar *>("attributeGainMode");
                        QVERIFY(modes->visibleRegion().contains(modes->rect()));
                        for (int i = 0; i < modes->count(); ++i)
                            QVERIFY(modes->tabRect(i).width() - 18 >= QFontMetrics(modes->font()).horizontalAdvance(modes->tabText(i)));
                        auto *import = window.findChild<QPushButton *>("importAttributes");
                        auto *calculate = window.findChild<QToolButton *>("calculateInlineGains");
                        QCOMPARE(import->mapTo(&window, QPoint()).y(), calculate->mapTo(&window, QPoint()).y());
                        for (const auto &row : desktop::GameAttributeRows) {
                            const auto key = QString::fromLatin1(row.key);
                            auto *value = window.findChild<QLabel *>("gameAttributeValue_" + key);
                            auto *gain = window.findChild<QWidget *>("gameAttributeGainRow_" + key);
                            QCOMPARE(value->mapTo(&window, value->rect().center()).y(), gain->mapTo(&window, gain->rect().center()).y());
                        }
                        if (qEnvironmentVariableIsSet("JX3DPS_TEST_SCREENSHOT")) {
                            QTest::qWait(desktop::motion::SlideMs + 40);
                            QVERIFY(window.grab().save(qEnvironmentVariable("JX3DPS_TEST_SCREENSHOT") +
                                QStringLiteral("-role-%1x%2-class%3.png").arg(size.width()).arg(size.height()).arg(specialization)));
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
        auto *options = window.findChild<SimulationOptions *>();
        auto *run = window.findChild<QWidget *>("runSettings");
        auto *content = window.findChild<QWidget *>("runParameters");
        auto *form = qobject_cast<QFormLayout *>(content->layout());
        auto *permanents = window.findChild<QWidget *>("permanentPanel");
        auto *upper = window.findChild<QWidget *>("simulationSettingsRow");
        auto *seed = window.findChild<QLineEdit *>("seed");
        const auto maxSeed = QStringLiteral("18446744073709551615");
        seed->setText(maxSeed);
        for (const auto *key : {"iterations", "workers", "delay"}) {
            auto *input = window.findChild<QSpinBox *>(QString::fromLatin1(key));
            input->setValue(input->maximum());
        }
        auto *seconds = window.findChild<QDoubleSpinBox *>("seconds");
        seconds->setValue(seconds->maximum());
        QVERIFY(!window.findChild<QTabWidget *>("runTabs"));
        QVERIFY(run->findChildren<QScrollArea *>().isEmpty());
        QCOMPARE(form->rowCount(), 5);
        for (const QSize size : {QSize(1206, 860), QSize(1280, 840), QSize(1120, 820)}) {
            window.resize(size);
            for (int specialization : {0, 1}) {
                window.findChild<QComboBox *>("specialization")->setCurrentIndex(specialization);
                QTest::qWait(100);
                const auto snapshot = options->Snapshot();
                QCOMPARE(window.size(), size);
                QCOMPARE(run->width(), 224);
                QCOMPARE(run->parentWidget(), upper);
                QCOMPARE(permanents->parentWidget(), upper);
                QCOMPARE(run->x(), 0);
                QCOMPARE(permanents->x() - run->geometry().right() - 1, 4);
                QCOMPARE(permanents->geometry().right(), upper->rect().right());
                QCOMPARE(run->height(), permanents->height());
                QRect pairs, labels;
                int fieldLeft = -1;
                for (int row = 0; row < form->rowCount(); ++row) {
                    auto *label = form->itemAt(row, QFormLayout::LabelRole);
                    auto *field = form->itemAt(row, QFormLayout::FieldRole);
                    QVERIFY(label && field);
                    QVERIFY(field->geometry().width() >= 128 && field->geometry().width() <= 136);
                    if (fieldLeft >= 0) QCOMPARE(field->geometry().left(), fieldLeft);
                    fieldLeft = field->geometry().left();
                    QVERIFY(label->geometry().width() >= label->sizeHint().width());
                    QCOMPARE(field->widget()->font().pixelSize(), 13);
                    labels = labels.united(label->geometry());
                    pairs = pairs.united(label->geometry()).united(field->geometry());
                    QVERIFY(field->widget()->visibleRegion().contains(field->widget()->rect()));
                    QVERIFY(run->rect().contains(QRect(field->widget()->mapTo(run, QPoint()), field->widget()->size())));
                }
                QCOMPARE(fieldLeft - labels.right() - 1, 6);
                QVERIFY(qAbs(pairs.center().x() - content->rect().center().x()) <= 1);
                QVERIFY(pairs.left() >= 4 && pairs.left() <= 8);
                QCOMPARE(options->Snapshot(), snapshot);
                QSet<int> columns, rows;
                int permanentCount = 0;
                for (auto *choice : options->findChildren<QComboBox *>()) {
                    if (!choice->objectName().startsWith(QStringLiteral("permanent%1").arg(specialization))) continue;
                    ++permanentCount;
                    QCOMPARE(choice->size(), QSize(38, 38));
                    QVERIFY(choice->visibleRegion().contains(choice->rect()));
                    auto *caption = options->findChild<QLabel *>(choice->objectName() + QStringLiteral("Caption"));
                    QVERIFY(caption && caption->visibleRegion().contains(caption->rect()));
                    QVERIFY(caption->width() >= caption->fontMetrics().horizontalAdvance(caption->text()));
                    QCOMPARE(caption->font().pixelSize(), 11);
                    const auto iconPosition = choice->mapTo(permanents, QPoint());
                    const auto textPosition = caption->mapTo(permanents, QPoint());
                    QCOMPARE(textPosition.x() - iconPosition.x() - choice->width(), 6);
                    columns.insert(iconPosition.x());
                    rows.insert(iconPosition.y());
                }
                QCOMPARE(permanentCount, 12);
                QCOMPARE(columns.size(), 3);
                QCOMPARE(rows.size(), 4);
                auto *permanentScroll = options->findChild<QScrollArea *>("permanentsScroll");
                QCOMPARE(permanentScroll->horizontalScrollBar()->maximum(), 0);
                QCOMPARE(permanentScroll->verticalScrollBar()->maximum(), 0);
                QCOMPARE(options->Batch().seed, std::numeric_limits<std::uint64_t>::max());
                QVERIFY(seed->toolTip().contains(maxSeed));
                seed->setFocus();
                QTest::keyClick(seed, Qt::Key_Home);
                QCOMPARE(seed->cursorPosition(), 0);
                QTest::keyClick(seed, Qt::Key_End);
                QCOMPARE(seed->cursorPosition(), maxSeed.size());
                QCOMPARE(seed->text(), maxSeed);
                if (qEnvironmentVariableIsSet("JX3DPS_TEST_SCREENSHOT"))
                    QVERIFY(run->grab().save(qEnvironmentVariable("JX3DPS_TEST_SCREENSHOT") +
                        QStringLiteral("-settings-%1x%2-class%3.png").arg(size.width()).arg(size.height()).arg(specialization)));
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
        auto *seconds = window.findChild<QDoubleSpinBox *>("seconds");
        auto *edit = seconds->findChild<QLineEdit *>();
        auto *unit = seconds->findChild<QLabel *>("spinBoxUnit");
        auto *seed = window.findChild<QLineEdit *>("seed");
        QVERIFY(edit && unit);
        QVERIFY(seconds->suffix().isEmpty());
        QCOMPARE(unit->text(), QStringLiteral("秒"));
        QCOMPARE(unit->focusPolicy(), Qt::NoFocus);
        QCOMPARE(unit->textInteractionFlags(), Qt::NoTextInteraction);
        QVERIFY(unit->testAttribute(Qt::WA_TransparentForMouseEvents));
        QVERIFY(seconds->accessibleDescription().contains(QStringLiteral("秒")));
        QCOMPARE(seconds->decimals(), 4);
        QCOMPARE(seconds->singleStep(), 0.0625);
        auto countColor = [](const QImage &image, const QColor &color) {
            int count = 0;
            for (int y = 0; y < image.height(); ++y)
                for (int x = 0; x < image.width(); ++x) {
                    const auto pixel = image.pixelColor(x, y);
                    if (qAbs(pixel.red() - color.red()) <= 2 && qAbs(pixel.green() - color.green()) <= 2
                        && qAbs(pixel.blue() - color.blue()) <= 2) ++count;
                }
            return count;
        };
        const QColor muted(desktop::theme::Muted), text(desktop::theme::Text);
        for (double value : {0.0625, 300.0}) {
            seconds->setValue(value);
            seed->setFocus();
            QTest::qWait(40);
            QCOMPARE(seconds->value(), value);
            QVERIFY(!edit->text().contains(QStringLiteral("秒")));
            const auto image = edit->grab().toImage();
            QVERIFY(countColor(image, muted) > 2);
            QVERIFY(countColor(image, text) > 2);
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
        const auto fixedUnitBounds = unit->geometry();
        QVERIFY(unit->isVisible());
        QVERIFY(countColor(edit->grab().toImage(), muted) > 2);
        QTest::keyClicks(edit, "abc");
        QVERIFY(edit->text().isEmpty());
        QInputMethodEvent unitInput;
        unitInput.setCommitString(QStringLiteral("秒"));
        QApplication::sendEvent(edit, &unitInput);
        QVERIFY(edit->text().isEmpty());
        QTest::keyClicks(edit, "12.1875");
        QTest::keyClick(edit, Qt::Key_Return);
        QCOMPARE(seconds->value(), 12.1875);
        seconds->stepUp();
        QCOMPARE(seconds->value(), 12.25);
        QCOMPARE(window.findChild<SimulationOptions *>()->Batch().duration, 196);
        QTest::keyClick(edit, Qt::Key_A, Qt::ControlModifier);
        QCOMPARE(edit->selectedText(), QStringLiteral("12.2500"));
        QVERIFY(countColor(edit->grab().toImage(), muted) > 2);
        QVERIFY(!unit->hasSelectedText());
        QTest::keyClick(edit, Qt::Key_Delete);
        QVERIFY(edit->text().isEmpty());
        QCOMPARE(unit->geometry(), fixedUnitBounds);
        QVERIFY(unit->isVisible());
        QTest::keyClicks(edit, "10.05");
        QTest::keyClick(edit, Qt::Key_Return);
        QCOMPARE(seconds->value(), 10.0625);
        QTest::mouseClick(edit, Qt::LeftButton, {}, unit->geometry().center());
        QVERIFY(!edit->selectedText().contains(QStringLiteral("秒")));
        QCOMPARE(seconds->value(), 10.0625);
        seconds->setEnabled(false);
        QVERIFY(unit->isVisible());
        QVERIFY(countColor(edit->grab().toImage(), muted.darker(140)) > 2);
        seconds->setEnabled(true);
        for (const auto *page : {"nav_profiles", "nav_simulation"}) {
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
        if (qEnvironmentVariableIsSet("JX3DPS_TEST_SCREENSHOT"))
            QVERIFY(window.findChild<QWidget *>("runSettings")->grab().save(qEnvironmentVariable("JX3DPS_TEST_SCREENSHOT") + "-seconds-unit.png"));
    }

    void talentFooterExpansion()
    {
        Workbench window;
        window.resize(1120, 820);
        window.show();
        auto *options = window.findChild<SimulationOptions *>();
        auto *tabs = window.findChild<QTabWidget *>("configurationTabs");
        auto *scroll = window.findChild<QScrollArea *>("configurationScroll");
        for (int specialization : {0, 1}) {
            window.findChild<QComboBox *>("specialization")->setCurrentIndex(specialization);
            QTest::qWait(60);
            const auto snapshot = options->Snapshot();
            auto *panel = static_cast<desktop::TalentPanel *>(window.findChild<QWidget *>(QStringLiteral("talents%1").arg(specialization)));
            auto *legacy = panel->HeaderWidget()->findChild<QToolButton *>("legacyTalentToggle");
            auto *clear = panel->HeaderWidget()->findChild<QToolButton *>("clearTalentPreview");
            QVERIFY(legacy->visibleRegion().contains(legacy->rect()));
            QVERIFY(clear->visibleRegion().contains(clear->rect()));
            QCOMPARE(tabs->height(), 100);
            legacy->click();
            QCOMPARE(tabs->height(), 270);
            legacy->click();
            QCOMPARE(tabs->height(), 100);
            tabs->setCurrentIndex(1);
            QCOMPARE(tabs->height(), 270);
            QVERIFY(!panel->HeaderWidget()->isVisible());
            tabs->setCurrentIndex(0);
            QCOMPARE(tabs->height(), 100);
            QVERIFY(panel->HeaderWidget()->isVisible());
            QTRY_COMPARE(scroll->verticalScrollBar()->maximum(), 0);
            QCOMPARE(options->Snapshot(), snapshot);
        }
    }

    void flowMacroCompiler()
    {
        const auto defaults = desktop::FlowDefaults();
        const auto source = desktop::FlowMacroFromValues(defaults);
        const auto compiled = desktop::CompileFlowMacro(source);
        QVERIFY(compiled.errors.isEmpty());
        QCOMPARE(compiled.values, defaults);
        QCOMPARE(desktop::CompileFlowMacro(QString{}).values, defaults);
        const auto ordered = desktop::CompileFlowMacro(QStringLiteral(
            "# opening\r\n00:00.00 /set_target id=1 level=125 life=25% vulnerable=5%\n"
            "/set_target level=126 distance=3 // last assignment wins\n"
            "/set_player qidian=3 shadows=2 style=阳春白雪"));
        QVERIFY2(ordered.errors.isEmpty(), qPrintable(ordered.ErrorText()));
        QCOMPARE(ordered.values.value("targetLevel").toInt(), 126);
        QCOMPARE(ordered.values.value("targetLife").toDouble(), 25.);
        QCOMPARE(ordered.values.value("vulnerable").toDouble(), 5.);
        QCOMPARE(ordered.values.value("mwStyle").toInt(), 0);
        QCOMPARE(desktop::CompileFlowMacro("/set_target life=0.25 vulnerable=0.05").values.value("targetLife").toDouble(), 25.);
        for (const auto &field : desktop::FlowFields) {
            for (double bound : {field.minimum, field.maximum}) {
                auto values = defaults;
                values.insert(QString::fromLatin1(field.key), bound);
                const auto parsed = desktop::CompileFlowMacro(desktop::FlowMacroFromValues(values));
                QVERIFY2(parsed.errors.isEmpty(), qPrintable(parsed.ErrorText()));
                QCOMPARE(parsed.values, values);
            }
        }
        for (const auto &bad : {QStringLiteral("/set_target"), QStringLiteral("/cast 三柴剑法"),
                QStringLiteral("/set_target typo=1"), QStringLiteral("/set_target level=124 level=125"),
                QStringLiteral("/set_target id=2"), QStringLiteral("00:01 /set_target level=125"),
                QStringLiteral("/set_target level=124.1"), QStringLiteral("/set_target life=0%"),
                QStringLiteral("/set_target life=100"), QStringLiteral("/set_target level=NaN"),
                QStringLiteral("/set_target shield=1e999"), QStringLiteral("/set_target distance=-1"),
                QStringLiteral("/set_target shield=30%"), QStringLiteral("/set_player style=2"),
                QStringLiteral("/set_player shadows=7"), QStringLiteral("/set_player qidian=11"),
                QStringLiteral("/set_target life=50.001%"), QStringLiteral("/set_target level = 124")}) {
            const auto parsed = desktop::CompileFlowMacro("# first line\n  " + bad);
            QVERIFY2(!parsed.errors.isEmpty(), qPrintable(bad));
            QCOMPARE(parsed.errors.front().line, 2);
            QVERIFY(parsed.errors.front().column >= 2);
            QVERIFY(parsed.errors.front().length > 0);
        }
        QVERIFY(!desktop::CompileFlowMacro(QString(65537, ' ')).errors.isEmpty());
    }

    void flowTimelineCompiler()
    {
        for (int frame : {0, 1, 8, 15, 16, 161, 959, 960, 1382400}) {
            int parsed = -1;
            QVERIFY(desktop::ParseFlowTime(desktop::FlowTimeText(frame), parsed));
            QCOMPARE(parsed, frame);
        }
        for (const auto &time : {"00:00.1", "00:60", "-1:00", "1440:00.0625", "999999999999999999999:00", "00:00.06250"}) {
            int parsed;
            QVERIFY2(!desktop::ParseFlowTime(time, parsed), time);
        }
        const auto source = QStringLiteral("# phase test\n/set_target life=100%\n"
            "  00:10.0625 /set_target id=1 life=20% distance=3  // retained\n"
            "00:05.5 /set_target life=50%\n00:05.5 /set_target life=40%");
        const auto parsed = desktop::CompileFlowMacro(source);
        QVERIFY2(parsed.errors.isEmpty(), qPrintable(parsed.ErrorText()));
        QCOMPARE(parsed.values.value("targetLife").toDouble(), 100.);
        const auto events = parsed.TargetEvents();
        QCOMPARE(events.size(), size_t(3));
        QCOMPARE(events[0].frame, 88);
        QCOMPARE(*events[0].life_basis_points, 5000);
        QCOMPARE(*events[1].life_basis_points, 4000);
        QCOMPARE(events[2].frame, 161);
        QCOMPARE(*events[2].distance_hundredths, 300);
        const auto &node = parsed.nodes[1];
        QVERIFY(node.CanMove());
        QCOMPARE(node.WithTime(240), QStringLiteral("  00:15 /set_target id=1 life=20% distance=3  // retained"));
        QCOMPARE(node.WithValues(0, "/set_target", {{"targetLife", 30}}), QStringLiteral("  00:00 /set_target id=1 life=30%  // retained"));
        for (const auto &bad : {"00:01 /set_player qidian=5", "00:01 /set_target vulnerable=5%", "00:01 /set_target shield=100",
                "00:01 /set_target id=1", "00:00.1 /set_target life=50%"})
            QVERIFY2(!desktop::CompileFlowMacro(bad).errors.isEmpty(), bad);
        QStringList lines;
        for (int i = 0; i < 256; ++i) lines << "00:01 /set_target distance=3";
        QVERIFY(desktop::CompileFlowMacro(lines.join('\n')).errors.isEmpty());
        lines << "/set_player qidian=5";
        QVERIFY(!desktop::CompileFlowMacro(lines.join('\n')).errors.isEmpty());
    }

    void flowTimelineEditing()
    {
        Workbench window;
        window.resize(1120, 820); window.show();
        auto *options = window.findChild<SimulationOptions *>();
        auto *script = window.findChild<CodeEditor *>("flowMacroEditor");
        auto *editor = static_cast<desktop::FlowTimelineEditor *>(window.findChild<QWidget *>("flowTimelineEditor"));
        auto *tabs = window.findChild<QTabBar *>("flowViewMode");
        QVERIFY(editor && tabs);
        const auto source = QStringLiteral("# opening\n/set_target level=124 life=100% distance=4\n"
            "  00:30 /set_target life=50%  // phase two\n00:45 /set_target distance=3\n");
        script->setPlainText(source);
        const auto snapshot = options->Snapshot();
        tabs->setCurrentIndex(0); tabs->setCurrentIndex(1);
        QCOMPARE(options->Snapshot(), snapshot);
        window.findChild<QToolButton *>("openFlowTimeline")->click();
        auto *dialog = window.findChild<QDialog *>("flowTimelineDialog");
        auto *canvas = static_cast<desktop::FlowTimelineCanvas *>(window.findChild<QWidget *>("flowTimelineCanvas"));
        auto *table = window.findChild<QTableWidget *>("flowEventTable");
        QTRY_VERIFY(dialog && dialog->isVisible());
        QCOMPARE(table->rowCount(), 3);
        const auto overviewPoint = canvas->NodePoint(3, 2);
        canvas->Zoom(2, 0);
        QVERIFY(canvas->NodePoint(3, 2).x() > overviewPoint.x());
        canvas->FitAll();
        QCOMPARE(canvas->NodePoint(3, 2), overviewPoint);
        QVERIFY(!editor->MoveNode(2, 16));
        QVERIFY(editor->MoveNode(3, 640));
        QVERIFY(script->toPlainText().contains("  00:40 /set_target life=50%  // phase two"));
        script->undo(); QCOMPARE(options->Snapshot(), snapshot);
        script->redo(); QCOMPARE(editor->Model().nodes[1].frame, 640);
        script->undo();
        QTest::qWait(60);
        const auto start = canvas->NodePoint(3, 2);
        QVERIFY(start.x() >= canvas->PlotRect().left());
        const auto finish = start + QPoint(60, 0);
        const auto expected = canvas->FrameAt(finish.x());
        QTest::mousePress(canvas, Qt::LeftButton, Qt::NoModifier, start);
        QTest::mouseMove(canvas, finish, 30);
        QTest::mouseRelease(canvas, Qt::LeftButton, Qt::NoModifier, finish);
        QCOMPARE(editor->Model().nodes[1].frame, expected);
        QTest::keyClick(canvas, Qt::Key_Right);
        QCOMPARE(editor->Model().nodes[1].frame, expected + 1);
        QTest::keyClick(canvas, Qt::Key_Delete);
        QCOMPARE(editor->Model().nodes.size(), 2);
        script->undo(); script->undo(); script->undo();
        QCOMPARE(options->Snapshot(), snapshot);

        editor->AddNode(80, 2);
        auto *nodeDialog = window.findChild<QDialog *>("flowNodeDialog");
        QTRY_VERIFY(nodeDialog && nodeDialog->isVisible());
        auto *time = nodeDialog->findChild<QDoubleSpinBox *>("flowNodeTime");
        auto *life = nodeDialog->findChild<QDoubleSpinBox *>("flowNodeValue_targetLife");
        auto *apply = nodeDialog->findChild<QPushButton *>("applyFlowNode");
        QVERIFY(time && life && apply);
        life->setValue(25); time->setValue(5.5);
        QCOMPARE(options->Snapshot(), snapshot); // Proxy inputs must never leak into configuration.
        apply->click();
        QTRY_VERIFY(!nodeDialog->isVisible());
        QVERIFY(script->toPlainText().endsWith("00:05.5 /set_target life=25%"));
        QCOMPARE(editor->Model().TargetEvents().front().frame, 88);
        script->undo(); QCOMPARE(options->Snapshot(), snapshot);
        editor->EditNode(2);
        QTRY_VERIFY(nodeDialog->isVisible());
        QVERIFY(!time->isEnabled());
        nodeDialog->reject(); QCOMPARE(options->Snapshot(), snapshot);
        editor->EditNode(3); life->setValue(35); nodeDialog->reject();
        QCOMPARE(options->Snapshot(), snapshot);
        editor->EditNode(3); life->setValue(35); apply->click();
        QVERIFY(script->toPlainText().contains("  00:30 /set_target life=35%  // phase two"));
        script->undo(); QCOMPARE(options->Snapshot(), snapshot);

        QTest::mouseDClick(canvas, Qt::LeftButton, Qt::NoModifier, canvas->NodePoint(3, 2));
        QTRY_VERIFY(nodeDialog->isVisible());
        QCOMPARE(life->value(), 50.);
        nodeDialog->reject();
        const auto empty = canvas->NodePoint(3, 2) + QPoint(120, 0);
        QTest::mouseDClick(canvas, Qt::LeftButton, Qt::NoModifier, empty);
        QTRY_VERIFY(nodeDialog->isVisible());
        QCOMPARE(time->value(), canvas->FrameAt(empty.x()) / 16.);
        QCOMPARE(life->value(), 50.);
        nodeDialog->reject();
        QCOMPARE(options->Snapshot(), snapshot);

        editor->EditNode(3);
        script->appendPlainText("# concurrent edit");
        const auto changed = script->toPlainText();
        apply->click();
        QVERIFY(nodeDialog->isVisible() && !apply->isEnabled());
        QCOMPARE(script->toPlainText(), changed);
        nodeDialog->reject(); script->undo();
        QCOMPARE(options->Snapshot(), snapshot);
        editor->EditNode(3);
        options->SetInputsEnabled(false);
        QVERIFY(!nodeDialog->isVisible());
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
        auto legacy = snapshot; legacy.insert("version", 3);
        QVERIFY2(options->RestoreSnapshot(legacy, error), qPrintable(error));
        QCOMPARE(options->Snapshot(), snapshot);
        dialog->close();
        if (qEnvironmentVariableIsSet("JX3DPS_TEST_SCREENSHOT")) {
            script->setPlainText(desktop::FlowMacroFromValues(desktop::FlowDefaults()) + QStringLiteral(
                "\n00:30 /set_target life=70%\n01:00 /set_target distance=20\n01:15 /set_target distance=3\n02:00 /set_target life=30%"));
            QTest::qWait(200);
            QVERIFY(window.grab().save(qEnvironmentVariable("JX3DPS_TEST_SCREENSHOT") + "-flow-preview.png"));
            editor->OpenEditor(); QTest::qWait(100);
            QVERIFY(dialog->grab().save(qEnvironmentVariable("JX3DPS_TEST_SCREENSHOT") + "-flow-timeline.png"));
            editor->EditNode(9); QTest::qWait(100);
            QVERIFY(nodeDialog->grab().save(qEnvironmentVariable("JX3DPS_TEST_SCREENSHOT") + "-flow-node.png"));
            nodeDialog->reject(); dialog->close();
        }
        window.close();
    }

    void flowMacroDrivesSimulation()
    {
        SimulationOptions options;
        auto *flow = options.findChild<CodeEditor *>("flowMacroEditor");
        QVERIFY(flow);
        options.findChild<QSpinBox *>("iterations")->setValue(3);
        options.findChild<QSpinBox *>("workers")->setValue(1);
        options.findChild<QDoubleSpinBox *>("seconds")->setValue(10);
        for (int specialization : {0, 1}) {
            options.findChild<QComboBox *>("specialization")->setCurrentIndex(specialization);
            flow->clear();
            auto referenceConfig = options.ActiveConfig();
            flow->setPlainText(QStringLiteral(
                "/set_target level=125 shield=45678 life=25% distance=3 vulnerable=5%\n"
                "/set_player qidian=3 shadows=2 style=阳春白雪\n"
                "00:02 /set_target life=50%\n00:03 /set_target distance=30\n00:06 /set_target distance=3"));
            QString error;
            QVERIFY2(options.CheckFlowMacro(error), qPrintable(error));
            const auto actual = options.ActiveConfig();
            std::visit([](auto &config) {
                config.target.level = 125;
                config.target.shield = 45678;
                config.target.life = .25;
                config.target.distance = 3;
                config.target.vulnerable = 51;
                config.target_events = {{32, 5000, {}}, {48, {}, 3000}, {96, {}, 300}};
                if constexpr (requires { config.initial_qidian; }) config.initial_qidian = 3;
                else { config.initial_yingzi = 2; config.initial_style = rt::mo_wen::Style::YangChunBaiXue; }
            }, referenceConfig);
            std::visit([&](const auto &config) {
                QCOMPARE(config.target.level, 125);
                QCOMPARE(config.target.shield, JX3DPS::value_t(45678));
                QCOMPARE(config.target.life, .25);
                QCOMPARE(config.target.distance, 3.);
                QCOMPARE(config.target.vulnerable, 51);
            }, actual);
            const auto source = desktop::DefaultMacro(options.Specialization());
            const auto bytes = source.toUtf8();
            const auto program = rt::MacroCompiler::Compile(std::string_view(bytes.constData(), bytes.size()), desktop::MacroOptions(options.Specialization(), false));
            QVERIFY(program.second.empty());
            const auto batch = options.Batch();
            const auto expected = std::visit([&](const auto &config) {
                return rt::RunBatch(program.first.program, batch, desktop::MakeRules(config));
            }, referenceConfig);
            BatchWorker worker(program.first.program, batch, actual);
            QSignalSpy result(&worker, &BatchWorker::StatisticsReady);
            worker.start();
            QVERIFY(worker.wait(10000));
            QTRY_COMPARE(result.count(), 1);
            const auto stats = qvariant_cast<SimulationResult>(result.front().front());
            QCOMPARE(stats.stats.checksum, expected.checksum);
            QCOMPARE(stats.stats.mean_damage, expected.mean_damage);
            QVERIFY(stats.stats.mean_damage > 0);
            DebugSimulator debug;
            debug.SetOptions(actual, batch.duration, batch.seed);
            QVERIFY2(debug.Initialize(source, error), qPrintable(error));
            QCOMPARE(debug.GetPlayerState().targetLifePercent, .25);
            if (specialization == 0) {
                QCOMPARE(std::get<tx::Config>(actual).initial_qidian, 3);
                QCOMPARE(debug.GetPlayerState().qidian, 4); // Existing rules add one opening qi point.
            }
            else {
                QCOMPARE(std::get<rt::mo_wen::Config>(actual).initial_yingzi, 2);
                QCOMPARE(std::get<rt::mo_wen::Config>(actual).initial_style, rt::mo_wen::Style::YangChunBaiXue);
            }
        }
    }

    void flowMacroPersistenceAndValidation()
    {
        Workbench window;
        window.resize(1120, 820);
        window.show();
        auto *options = window.findChild<SimulationOptions *>();
        auto *flow = window.findChild<CodeEditor *>("flowMacroEditor");
        auto *rotation = window.findChild<MacroEditor *>("macroEditor");
        QVERIFY(!window.findChild<QDialog *>("targetSettingsDialog"));
        QVERIFY(!window.findChild<QToolButton *>("openTargetSettings"));
        QVERIFY(!window.findChild<QSpinBox *>("targetLevel"));
        const auto source = QStringLiteral("# keep comments\n/set_target level=125 life=50%\n/set_player qidian=3");
        flow->setPlainText(source);
        rotation->SetBreakpoint(1, true);
        const auto rotationSource = rotation->toPlainText();
        QString error;
        QVERIFY2(options->CheckFlowMacro(error), qPrintable(error));
        const auto snapshot = options->Snapshot();
        QCOMPARE(snapshot.value("flowMacro").toString(), source);
        QCOMPARE(snapshot.value("values").toObject().value("targetLevel").toInt(), 125);
        QTemporaryDir directory;
        const auto path = directory.filePath("flow.json");
        QVERIFY2(options->ExportConfiguration(path, error), qPrintable(error));
        SimulationOptions restored;
        QVERIFY2(restored.ImportConfiguration(path, error), qPrintable(error));
        QCOMPARE(restored.Snapshot(), snapshot);
        auto legacy = snapshot;
        legacy.remove("flowMacro");
        auto values = legacy.value("values").toObject();
        values.insert("targetLevel", 127);
        values.insert("mwStyle", 0);
        legacy.insert("values", values);
        QVERIFY2(restored.RestoreSnapshot(legacy, error), qPrintable(error));
        QCOMPARE(restored.Config().target.level, 127);
        QCOMPARE(restored.Snapshot().value("values"), QJsonValue(values));
        const auto migrated = restored.Snapshot();
        auto mismatch = snapshot;
        mismatch.insert("values", values);
        QVERIFY(!restored.RestoreSnapshot(mismatch, error));
        QCOMPARE(restored.Snapshot(), migrated);
        flow->appendPlainText(QStringLiteral("/set_target life=wrong"));
        QVERIFY(!options->CheckFlowMacro(error));
        QVERIFY(error.contains(QStringLiteral("第 4 行")));
        QVERIFY_EXCEPTION_THROWN(options->ActiveConfig(), std::invalid_argument);
        QVERIFY(!options->ExportConfiguration(path, error));
        window.findChild<QToolButton *>("startButton")->click();
        QVERIFY(!window.findChild<BatchWorker *>());
        QVERIFY(window.findChild<QLabel *>("simulationResult")->text().contains(QStringLiteral("流程宏")));
        QCOMPARE(rotation->toPlainText(), rotationSource);
        QVERIFY(rotation->HasBreakpoint(1));
        const auto invalidSource = flow->toPlainText();
        options->SaveSettings();
        SimulationOptions draft;
        QCOMPARE(draft.findChild<CodeEditor *>("flowMacroEditor")->toPlainText(), invalidSource);
        QVERIFY(!draft.CheckFlowMacro(error));
        QCOMPARE(draft.Snapshot().value("values"), snapshot.value("values"));
        flow->undo();
        QCOMPARE(flow->toPlainText(), source);
        QVERIFY(options->CheckFlowMacro(error));
        QCOMPARE(options->Snapshot(), snapshot);
        window.findChild<QToolButton *>("debugButton")->click();
        window.findChild<QToolButton *>("startButton")->click();
        auto *session = window.findChild<DebugSession *>();
        QTRY_COMPARE(session->GetState(), DebugSession::Paused);
        QVERIFY(flow->isReadOnly());
        QCOMPARE(session->GetDebugInfo().targetLifePercent, .5);
        QCOMPARE(session->GetDebugInfo().qidian, 4);
        window.findChild<QPushButton *>("nav_simulation")->click();
        window.findChild<QToolButton *>("openFlowMacroHelp")->click();
        auto *help = window.findChild<QDialog *>("flowMacroHelpDialog");
        QTRY_VERIFY(help->isVisible());
        QCOMPARE(session->GetState(), DebugSession::Paused);
        QCOMPARE(flow->toPlainText(), source);
        QCOMPARE(rotation->toPlainText(), rotationSource);
        help->close();
        window.findChild<QAction *>("debugStop")->trigger();
        QVERIFY(!flow->isReadOnly());
        if (qEnvironmentVariableIsSet("JX3DPS_TEST_SCREENSHOT"))
            QVERIFY(window.grab().save(qEnvironmentVariable("JX3DPS_TEST_SCREENSHOT") + "-flow-macro.png"));
        QVERIFY(window.close());
    }

    void historyChartInteractions()
    {
        desktop::HistoryChart chart;
        chart.resize(850, 310);
        chart.show();
        chart.SetPoints({{1, 0}, {2, 0}, {3, 0}});
        for (int i = 0; i < 3; ++i) QVERIFY(chart.PlotRect().contains(chart.PointPosition(i)));
        chart.SetPoints({{10, 30000}, {11, 36000}, {12, 34000}});
        chart.SetReference(50000);
        for (int i = 0; i < 3; ++i) QVERIFY(chart.PlotRect().contains(chart.PointPosition(i)));
        quint64 selected = 0;
        chart.runSelected = [&selected](quint64 id) { selected = id; };
        QTest::mouseClick(&chart, Qt::LeftButton, Qt::NoModifier, chart.PointPosition(0).toPoint());
        QCOMPARE(selected, quint64(10));
        QTest::keyClick(&chart, Qt::Key_Right);
        QCOMPARE(selected, quint64(11));
        QTest::keyClick(&chart, Qt::Key_Left);
        QCOMPARE(selected, quint64(10));
        chart.SetReference(std::nullopt);
        chart.SetPoints({{13, 34000}});
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
        auto *navigation = window.findChild<QWidget *>("topNavigation");
        auto *editor = window.findChild<MacroEditor *>("macroEditor");
        auto *options = window.findChild<SimulationOptions *>();
        auto *pages = window.findChild<QStackedWidget *>("workspacePages");
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
        const auto source = editor->toPlainText();
        auto *document = editor->document();
        auto shot = [&window](const QString &name) {
            if (qEnvironmentVariableIsSet("JX3DPS_TEST_SCREENSHOT")) {
                QTest::qWait(180);
                QVERIFY(window.grab().save(qEnvironmentVariable("JX3DPS_TEST_SCREENSHOT") + "-magpie-" + name + ".png"));
            }
        };
        shot("simulation");
        QTest::mouseClick(window.findChild<QPushButton *>("nav_character"), Qt::LeftButton);
        QVERIFY(window.findChild<QPushButton *>("nav_attributes")->isChecked());
        QVERIFY(window.findChild<QWidget *>("characterChildren")->isVisible());
        for (const auto *page : {"profiles", "attributes", "talents", "permanents", "macro", "general", "about", "statistics", "skills", "gains", "hotspots", "timeline", "events", "buffs"}) {
            auto *button = window.findChild<QPushButton *>(QStringLiteral("nav_%1").arg(QString::fromLatin1(page)));
            QVERIFY(button);
            const QString key = QString::fromLatin1(page);
            if (key == "profiles" || key == "attributes" || key == "talents" || key == "permanents")
                window.findChild<QPushButton *>("nav_character")->click();
            if (key == "general" || key == "about")
                window.findChild<QPushButton *>("nav_settings")->click();
            if (button->parentWidget()->objectName() == "analysisChildren")
                window.findChild<QPushButton *>("nav_analysis")->click();
            QCoreApplication::processEvents();
            QVERIFY(button->isVisible());
            QTest::mouseClick(button, Qt::LeftButton);
            QCoreApplication::processEvents();
            QVERIFY(button->isChecked());
            QCOMPARE(options->Snapshot(), snapshot);
            QCOMPARE(editor->document(), document);
            QCOMPARE(editor->toPlainText(), source);
            QVERIFY(editor->HasBreakpoint(2));
            QVERIFY(pages->currentWidget()->isVisible());
            if (key == "hotspots") QVERIFY(window.findChild<QPushButton *>("runWithHotspots")->isVisible());
            if (key == "events") QCOMPARE(window.findChild<QTabWidget *>("analysisViews")->currentWidget(), window.findChild<QTableWidget *>("damageEvents"));
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
        for (int attack : {11000, 10500, 12500, 14000}) {
            window.findChild<QSpinBox *>("attackBase")->setValue(attack);
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
        auto *details = window.findChild<QTextBrowser *>("resultDetails");
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
        auto *debugToggle = window.findChild<QToolButton *>("debugButton");
        QTRY_VERIFY(debugToggle->isVisible());
        QVERIFY(debugToggle->visibleRegion().contains(debugToggle->rect().center()));
        auto *more = window.findChild<QToolButton *>("macroMore");
        QVERIFY(more->isVisible());
        QVERIFY(more->visibleRegion().contains(more->rect().center()));
        shot("compact");
        for (auto *button : navigation->findChildren<QPushButton *>()) {
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
        auto *options = window.findChild<SimulationOptions *>();
        window.findChild<QSpinBox *>("iterations")->setValue(12);
        window.findChild<QDoubleSpinBox *>("seconds")->setValue(30);
        auto clickPage = [&](const char *key) {
            auto *button = window.findChild<QPushButton *>(QStringLiteral("nav_") + QString::fromLatin1(key));
            QVERIFY(button && button->isVisible());
            QTest::mouseClick(button, Qt::LeftButton);
        };
        clickPage("analysis");
        clickPage("hotspots");
        auto *start = window.findChild<QPushButton *>("runWithHotspots");
        QVERIFY(start->isVisible());
        QTest::mouseClick(start, Qt::LeftButton);
        QTRY_COMPARE_WITH_TIMEOUT(window.findChild<QTableWidget *>("resultHistory")->rowCount(), 1, 10000);
        QVERIFY(window.findChild<QPushButton *>("nav_hotspots")->isChecked());
        QVERIFY(start->isHidden());
        clickPage("gains");
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
        const QJsonObject set{{"name", QStringLiteral("西塞·冷辉")},
            {"siblings", QJsonArray{QStringLiteral("西塞·冷辉护手"), QStringLiteral("西塞·冷辉腰带"),
                QStringLiteral("西塞·冷辉履"), QStringLiteral("西塞·冷辉冠"), QStringLiteral("西塞·冷辉衣")}},
            {"attributes", QJsonObject{{"2", QStringLiteral("装备：施展外功伤害招式，一定几率提高自身外功会心几率4%，会心效果4%，持续6秒。")},
                {"4", QStringLiteral("<text>text=\"“无我无剑”伤害提高10%\" font=101 </text><text>text=\"“对阵招式”伤害提高5%\" font=101 </text>")}}}};
        const auto attribute = [](const QString &label, const char *color, int bonus = 0) {
            return QJsonObject{{"label", label}, {"color", QString::fromLatin1(color)}, {"refine_bonus", bonus}};
        };
        QJsonObject item{{"id", "7_101482"}, {"name", QStringLiteral("孤漠·立雪衣")},
            {"type", QStringLiteral("上衣")}, {"quality", 4}, {"max_refine", 6}, {"refine_level", 6},
            {"attributes", QJsonArray{attribute(QStringLiteral("外功防御等级提高1060"), "white"),
                attribute(QStringLiteral("内功防御等级提高848"), "white"), attribute(QStringLiteral("体质提高14219"), "white", 1066),
                attribute(QStringLiteral("身法提高1160"), "white", 87), attribute(QStringLiteral("外功攻击提高2635"), "green", 198),
                attribute(QStringLiteral("外功会心等级提高9695"), "green", 727), attribute(QStringLiteral("无双等级提高7756"), "green", 582)}},
            {"diamonds", QJsonArray{QStringLiteral("外功会心等级提高1317"), QStringLiteral("外功会心效果等级提高1317")}},
            {"require_level", 130}, {"durability", 4800}, {"max_durability", 4800},
            {"enchantments", QJsonArray{QJsonObject{{"icon_id", 9527}, {"description", QStringLiteral("无双等级提高624")}},
                QJsonObject{{"icon_id", 18272}, {"description", QStringLiteral("自身受到的疗伤成效降低10%，基础攻击小幅提高（内功攻击提高791点，外功攻击提高710点），不在名剑大会中生效。")}}}},
            {"set", set}, {"level", 25900}, {"level_refine_bonus", 1943}, {"score", 46620},
            {"score_refine_bonus", 3497}, {"score_enchant_bonus", 9180},
            {"source", QStringLiteral("套装兑换（兑换牌·纯阳） / 侠行点（侠行·纯阳，侠行·初级套装）")}};
        QJsonObject equipped;
        for (const auto &slot : {QStringLiteral("帽子"), QStringLiteral("腰带"), QStringLiteral("鞋子")})
            equipped.insert(slot, QJsonObject{{"set", set}});
        const QString html = desktop::EquipmentDetailCard::Html(item, equipped, QStringLiteral("上衣"));
        desktop::EquipmentDetailCard card;
        card.icon = [](int id) { return desktop::GameIcon(id).pixmap(32, 32); };
        card.resize(751, 1133);
        card.setHtml(html);
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
        if (qEnvironmentVariableIsSet("JX3DPS_TEST_SCREENSHOT"))
            QVERIFY(card.grab().save(qEnvironmentVariable("JX3DPS_TEST_SCREENSHOT") + "-reference-card.png"));

        // Missing metadata must not turn into invented durability, enchantments or refinement.
        item.remove("refine_level");
        item.remove("durability");
        item.remove("enchantments");
        const QString base = desktop::EquipmentDetailCard::Html(item, {}, QStringLiteral("上衣"));
        card.setHtml(base);
        QVERIFY(card.toPlainText().contains(QStringLiteral("精炼上限: 6")));
        QVERIFY(card.toPlainText().contains(QStringLiteral("耐久度上限：4800")));
        QVERIFY(card.toPlainText().contains(QStringLiteral("西塞·冷辉(1/5)")));
        QVERIFY(!card.toPlainText().contains(QStringLiteral("无双等级提高624")));
        QVERIFY(base.contains("color:#acaeb1'>[4]"));
        card.resize(500, 640);
        QCoreApplication::processEvents();
        QCOMPARE(card.horizontalScrollBar()->maximum(), 0);
        QVERIFY(card.verticalScrollBar()->maximum() > 0);
        item["name"] = QStringLiteral("<img src='https://example.invalid'>测试");
        card.setHtml(desktop::EquipmentDetailCard::Html(item, {}, QStringLiteral("上衣")));
        QVERIFY(card.toPlainText().contains(item.value("name").toString()));
    }

    void macroHelpOpensFromPanel()
    {
        Workbench window;
        window.show();
        auto *options = window.findChild<SimulationOptions *>();
        auto *editor = window.findChild<MacroEditor *>("macroEditor");
        auto *pages = window.findChild<QStackedWidget *>("workspacePages");
        auto *button = window.findChild<QToolButton *>("openMacroHelp");
        auto *dialog = window.findChild<QDialog *>("macroHelpDialog");
        auto *browser = window.findChild<QTextBrowser *>("macroHelpBrowser");
        auto *panel = window.findChild<QGroupBox *>("macroPanel");
        QVERIFY(button && dialog && browser && panel);
        QVERIFY(!window.findChild<QPushButton *>("nav_help"));
        QVERIFY(!window.findChild<QWidget *>("macroHelpPage"));
        QVERIFY(panel->findChild<QWidget *>("groupHeaderActions")->isAncestorOf(button));
        const auto original = editor->toPlainText();
        editor->appendPlainText(QStringLiteral("\n# 帮助窗口不修改宏"));
        editor->SetBreakpoint(2, true);
        const auto source = editor->toPlainText();
        const auto snapshot = options->Snapshot();
        auto *document = editor->document();
        for (const QSize size : {QSize(1206, 860), QSize(1120, 820)}) {
            window.resize(size);
            for (const auto *page : {"simulation", "macro"}) {
                auto *nav = window.findChild<QPushButton *>(QStringLiteral("nav_%1").arg(QString::fromLatin1(page)));
                nav->click();
                QTest::qWait(desktop::motion::SlideMs + 40);
                QVERIFY(button->isVisible() && button->visibleRegion().contains(button->rect()));
                QStyleOptionGroupBox style;
                style.initFrom(panel);
                style.text = panel->title();
                style.subControls = QStyle::SC_GroupBoxFrame | QStyle::SC_GroupBoxLabel;
                const auto frame = panel->style()->subControlRect(QStyle::CC_GroupBox, &style, QStyle::SC_GroupBoxFrame, panel);
                QVERIFY(button->mapTo(panel, button->rect().bottomLeft()).y() < frame.top());
                QVERIFY(button->mapTo(panel, button->rect().topRight()).x() >= panel->width() - 4);
                auto *currentPage = pages->currentWidget();
                if (qEnvironmentVariableIsSet("JX3DPS_TEST_SCREENSHOT"))
                    QVERIFY(window.grab().save(qEnvironmentVariable("JX3DPS_TEST_SCREENSHOT") +
                        QStringLiteral("-macro-help-entry-%1-%2.png").arg(QString::fromLatin1(page)).arg(size.width())));
                QTest::mouseClick(button, Qt::LeftButton);
                QTRY_VERIFY(dialog->isVisible());
                QVERIFY(dialog->isWindow() && !dialog->isModal());
                QVERIFY(browser->toPlainText().contains(QStringLiteral("/cast")));
                QCOMPARE(pages->currentWidget(), currentPage);
                QVERIFY(nav->isChecked());
                QCOMPARE(options->Snapshot(), snapshot);
                QCOMPARE(editor->document(), document);
                QCOMPARE(editor->toPlainText(), source);
                QVERIFY(editor->HasBreakpoint(2));
                QVERIFY(editor->document()->isUndoAvailable());
                QTRY_VERIFY(browser->verticalScrollBar()->maximum() > 0);
                browser->verticalScrollBar()->setValue(qMin(80, browser->verticalScrollBar()->maximum()));
                const auto scrollPosition = browser->verticalScrollBar()->value();
                dialog->close();
                button->click();
                QTRY_VERIFY(dialog->isVisible());
                QCOMPARE(window.findChildren<QDialog *>("macroHelpDialog").size(), 1);
                QCOMPARE(browser->verticalScrollBar()->value(), scrollPosition);
                dialog->close();
            }
        }
        editor->undo();
        QCOMPARE(editor->toPlainText(), original);
        editor->redo();
        QCOMPARE(editor->toPlainText(), source);
        window.findChild<QAction *>("macroHelp")->trigger();
        QTRY_VERIFY(dialog->isVisible());
        window.findChild<QComboBox *>("specialization")->setCurrentIndex(1);
        QVERIFY(browser->toPlainText().contains(QStringLiteral("莫问")));
        QVERIFY(browser->toPlainText().contains(QStringLiteral("tbuff:商")));
        QCOMPARE(desktop::theme::AccentColor(dialog), desktop::theme::AccentColor(&window));
        if (qEnvironmentVariableIsSet("JX3DPS_TEST_SCREENSHOT"))
            QVERIFY(dialog->grab().save(qEnvironmentVariable("JX3DPS_TEST_SCREENSHOT") + "-macro-help-window.png"));
        dialog->close();
        window.findChild<QPushButton *>("nav_about")->click();
        auto *about = pages->currentWidget();
        window.findChild<QToolButton *>("openMacroHelpFromAbout")->click();
        QTRY_VERIFY(dialog->isVisible());
        QCOMPARE(pages->currentWidget(), about);
        dialog->close();
        window.findChild<QPushButton *>("nav_simulation")->click();
        window.findChild<QAction *>("debugStart")->trigger();
        auto *session = window.findChild<DebugSession *>();
        QTRY_COMPARE(session->GetState(), DebugSession::Paused);
        const auto pausedLine = editor->GetCurrentDebugLine();
        const auto pausedSource = editor->toPlainText();
        const auto pausedSnapshot = options->Snapshot();
        button->click();
        QTRY_VERIFY(dialog->isVisible());
        QCOMPARE(session->GetState(), DebugSession::Paused);
        QCOMPARE(editor->GetCurrentDebugLine(), pausedLine);
        QCOMPARE(editor->toPlainText(), pausedSource);
        QCOMPARE(options->Snapshot(), pausedSnapshot);
        QVERIFY(editor->isReadOnly());
        QVERIFY(window.close());
        QVERIFY(!dialog->isVisible());
    }

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
                                    {"type", QStringLiteral("帽子")}, {"icon_id", 0},
                                    {"level", 27100}, {"quality", 4}, {"max_refine", 6},
                                    {"diamonds", QJsonArray{QStringLiteral("外功会心等级提高100")}},
                                    {"attributes", QJsonArray{QJsonObject{
                                        {"label", QStringLiteral("外功攻击提高1195")}, {"color", QStringLiteral("green")}}}}};
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
        window.findChild<QPushButton *>("nav_attributes")->click();
        auto *openEquipment = window.findChild<QToolButton *>("openEquipment");
        QVERIFY(openEquipment && openEquipment->isEnabled());
        openEquipment->click();
        auto *equipmentWindow = options->findChild<QDialog *>("equipmentDialog");
        QVERIFY(equipmentWindow && equipmentWindow->isWindow() && !equipmentWindow->isModal());
        auto *panel = equipmentWindow->findChild<EquipmentPanel *>();
        QVERIFY(panel);
        QTRY_VERIFY(panel->isVisible());
        QVERIFY(!options->findChild<QSpinBox *>("crit")->isEnabled());
        auto *effects = options->findChild<QCheckBox *>(QStringLiteral("equipment%1").arg(tx::WeaponCW));
        QVERIFY(effects);
        QVERIFY(!effects->isEnabled());
        auto *scroll = options->findChild<QScrollArea *>("equipmentScroll");
        QVERIFY(scroll);
        auto *slotButton = panel->findChild<QToolButton *>(QStringLiteral("equipmentSlot0"));
        QVERIFY(slotButton);
        QSignalSpy changed(panel, &EquipmentPanel::LoadoutChanged);
        QVERIFY(changed.isValid());
        slotButton->click();
        auto *catalog = panel->findChild<QListWidget *>(QStringLiteral("equipmentChoices"));
        QVERIFY(catalog);
        QTRY_VERIFY(catalog->isVisible());
        QVERIFY(catalog->screen()->availableGeometry().contains(catalog->window()->geometry()));
        if (qEnvironmentVariableIsSet("JX3DPS_TEST_SCREENSHOT"))
            QVERIFY(catalog->window()->grab().save(qEnvironmentVariable("JX3DPS_TEST_SCREENSHOT") + "-equipment-picker.png"));
        QCOMPARE(catalog->count(), 1);
        QCOMPARE(catalog->item(0)->data(Qt::UserRole).toJsonObject(), candidate);
        QTest::mouseClick(catalog->viewport(), Qt::LeftButton, Qt::NoModifier,
                          catalog->visualItemRect(catalog->item(0)).center());
        QCOMPARE(changed.count(), 1);
        QCOMPARE(panel->Loadout().value("items").toObject().value(QStringLiteral("帽子")).toObject(), candidate);
        const auto details = panel->findChild<QTextBrowser *>(QStringLiteral("equipmentDetails"))->toHtml();
        QVERIFY(details.contains(QStringLiteral("测试帽子")));
        QVERIFY(details.contains(QStringLiteral("外功攻击提高1195")));
        auto *effectDetails = panel->findChild<QToolButton *>("equipmentEffectsToggle");
        auto *equippedEffects = panel->findChild<QListWidget *>("equipmentEffects");
        QVERIFY(effectDetails);
        QVERIFY(equippedEffects);
        QVERIFY(effectDetails->isVisible());
        QVERIFY(!equippedEffects->isVisible());
        effectDetails->click();
        QVERIFY(equippedEffects->isVisible());
        effectDetails->click();
        QVERIFY(!equippedEffects->isVisible());
        QCOMPARE(changed.count(), 1);
        QCOMPARE(options->Snapshot(), configuration);
        QCOMPARE(editor->toPlainText(), macro);
        QCOMPARE(editor->GetCurrentDebugLine(), line);
        QCOMPARE(session->GetState(), DebugSession::Paused);
        QVERIFY(editor->isReadOnly());
        QVERIFY(!window.findChild<QTabWidget *>("workspaceTabs"));
        auto *detailButton = panel->findChild<QToolButton *>("viewEquipmentDetails");
        QVERIFY(detailButton->isEnabled());
        detailButton->click();
        auto *detailPopup = panel->findChild<QWidget *>("equipmentDetailsPopup");
        QVERIFY(detailPopup);
        QTRY_VERIFY(detailPopup->isVisible());
        QVERIFY(detailPopup->screen()->availableGeometry().contains(detailPopup->geometry()));
        equipmentWindow->close();
        QVERIFY(!detailPopup->isVisible());
        openEquipment->click();
        QCOMPARE(equipmentWindow->findChild<EquipmentPanel *>(), panel);
        QCOMPARE(panel->Loadout().value("items").toObject().size(), 1);
        session->Stop();
        QVERIFY(effects->isEnabled());
        auto *specialization = options->findChild<QComboBox *>("specialization");
        specialization->setCurrentIndex(1);
        QVERIFY(!panel->isVisible());
        specialization->setCurrentIndex(0);
        QVERIFY(panel->isVisible());
        QCOMPARE(panel->Loadout().value("items").toObject().size(), 1);
        equipmentWindow->resize(720, 680);
        QCoreApplication::processEvents();
        scroll->verticalScrollBar()->setValue(0);
        if (qEnvironmentVariableIsSet("JX3DPS_TEST_SCREENSHOT"))
            QVERIFY(equipmentWindow->grab().save(qEnvironmentVariable("JX3DPS_TEST_SCREENSHOT") + "-equipment-layout.png"));
        for (auto *button : panel->findChildren<QToolButton *>()) {
            if (!button->property("equipmentSlot").isValid()) continue;
            const auto corner = button->mapTo(scroll->viewport(), button->rect().bottomRight());
            QVERIFY2(scroll->viewport()->rect().contains(corner), qPrintable(QStringLiteral("%1 at %2,%3 outside %4x%5")
                .arg(button->objectName()).arg(corner.x()).arg(corner.y())
                .arg(scroll->viewport()->width()).arg(scroll->viewport()->height())));
        }
        if (qEnvironmentVariableIsSet("JX3DPS_TEST_SCREENSHOT"))
            QVERIFY(equipmentWindow->grab().save(qEnvironmentVariable("JX3DPS_TEST_SCREENSHOT") + "-equipment-overview.png"));
        equipmentWindow->resize(500, 640);
        QCoreApplication::processEvents();
        QCOMPARE(scroll->horizontalScrollBar()->maximum(), 0);
        QVERIFY(panel->width() <= scroll->viewport()->width());
        if (qEnvironmentVariableIsSet("JX3DPS_TEST_SCREENSHOT"))
            QVERIFY(equipmentWindow->grab().save(qEnvironmentVariable("JX3DPS_TEST_SCREENSHOT") + "-equipment.png"));
        scroll->ensureWidgetVisible(effects);
        QVERIFY(scroll->viewport()->rect().contains(effects->mapTo(scroll->viewport(), effects->rect().center())));
    }

    void optionalFightHotspots_data()
    {
        QTest::addColumn<bool>("moWen");
        QTest::addColumn<bool>("trace");
        QTest::addColumn<bool>("gains");
        QTest::newRow("tai-xu-batch") << false << false << false;
        QTest::newRow("mo-wen-batch") << true << false << false;
        QTest::newRow("tai-xu-trace") << false << true << false;
        QTest::newRow("mo-wen-gains") << true << false << true;
    }

    void optionalFightHotspots()
    {
        QFETCH(bool, moWen);
        QFETCH(bool, trace);
        QFETCH(bool, gains);
        desktop::Config config = moWen ? desktop::Config{rt::mo_wen::Config{}} : desktop::Config{rt::tai_xu::Config{}};
        const auto spec = desktop::ClassOf(config);
        const auto source = desktop::DefaultMacro(spec);
        const auto compiled = rt::MacroCompiler::Compile(source.toStdString(), desktop::MacroOptions(spec));
        QVERIFY(compiled.second.empty());
        rt::BatchOptions options{trace ? 1u : 512u, 42, 4800, 2, 4800 * 16, 4800 * 40};
        SimulationResult plain;
        ResultsPanel panel;
        auto *tabs = panel.findChild<QTabWidget *>("resultTabs");
        const int tab = tabs->count() - 1;
        for (bool enabled : {false, true}) {
            BatchWorker worker(compiled.first.program, options, config, nullptr, trace, gains, desktop::GainComparisonMode::SameValue, enabled);
            QSignalSpy ready(&worker, &BatchWorker::StatisticsReady);
            worker.start();
            QVERIFY(worker.wait(20000));
            QTRY_COMPARE(ready.size(), 1);
            const auto result = qvariant_cast<SimulationResult>(ready.front().front());
            QVERIFY(result.Complete());
            QCOMPARE(result.hotspots.has_value(), enabled);
            panel.AddResult(result, source, {});
            QVERIFY(tabs->isTabVisible(tab));
            QCOMPARE(panel.findChild<QPushButton *>("runWithHotspots")->isHidden(), enabled);
            if (!enabled) { plain = result; continue; }
            QCOMPARE(result.stats.checksum, plain.stats.checksum);
            QCOMPARE(result.skills.size(), plain.skills.size());
            for (int i = 0; i < result.gains.size(); ++i) QCOMPARE(result.gains[i].stats.checksum, plain.gains[i].stats.checksum);
            QCOMPARE(result.hotspots->fights, options.iterations * (gains ? rt::AttributeGainSimulationStages<rt::mo_wen::Config> : 1));
            QCOMPARE(result.hotspots->workers.size(), trace ? 1 : 2);
            quint64 workerPoints = 0, workerFights = 0;
            for (const auto &w : result.hotspots->workers) {
                QVERIFY(w.fights > 0);
                workerPoints += w.samplePoints;
                workerFights += w.fights;
            }
            QCOMPARE(workerFights, result.hotspots->fights);
            QCOMPARE(workerPoints, result.hotspots->samplePoints);
            QVERIFY(result.hotspots->workerMs > 0);
            const auto &calls = result.hotspots->calls;
            QVERIFY(!calls.isEmpty());
            QCOMPARE(calls.front().inclusive, result.hotspots->samplePoints);
            QVector<quint64> accounted(calls.size());
            for (int i = 0; i < calls.size(); ++i) {
                const auto &call = calls[i];
                accounted[i] += call.self;
                if (i) {
                    QVERIFY(call.parent < static_cast<unsigned>(i));
                    accounted[call.parent] += call.inclusive;
                }
            }
            for (int i = 0; i < calls.size(); ++i) QCOMPARE(calls[i].inclusive, accounted[i]);
            double estimate = 0;
            for (double stage : result.hotspots->phaseUs) estimate += stage;
            if (result.hotspots->samplePoints) QVERIFY(qAbs(estimate - result.hotspots->workerMs * 1000 / result.hotspots->fights) < 1e-6);
            QCOMPARE(tabs->currentIndex(), tab);
            QCOMPARE(panel.findChild<QTableWidget *>("fightHotspotTable")->rowCount(), 10);
            panel.findChild<QTableWidget *>("resultHistory")->selectRow(0);
            QVERIFY(!panel.findChild<QPushButton *>("runWithHotspots")->isHidden());
        }
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

    void teamBuffOptionsAndSnapshots()
    {
        SimulationOptions options;
        const auto prefix = QStringLiteral("txTeam%1_").arg(tx::team::ZhenFen);
        options.findChild<QCheckBox *>(prefix + "enabled")->setChecked(true);
        options.findChild<QDoubleSpinBox *>(prefix + "start")->setValue(1.0625);
        options.findChild<QDoubleSpinBox *>(prefix + "duration")->setValue(3.125);
        options.findChild<QSpinBox *>(prefix + "stacks")->setValue(35);
        const auto config = options.Config();
        QCOMPARE(config.team_buffs.size(), std::size_t(1));
        QCOMPARE(config.team_buffs[0].frame, 17);
        QCOMPARE(config.team_buffs[0].duration, 50);
        QCOMPARE(config.team_buffs[0].stacks, 35);
        const auto snapshot = options.Snapshot();
        SimulationOptions restored;
        QString error;
        QVERIFY2(restored.RestoreSnapshot(snapshot, error), qPrintable(error));
        QCOMPARE(restored.Config().team_buffs, config.team_buffs);
        auto old = snapshot;
        auto values = old.value("values").toObject();
        for (const auto &key : values.keys()) if (key.startsWith("txTeam")) values.remove(key);
        old.insert("values", values);
        QVERIFY2(restored.RestoreSnapshot(old, error), qPrintable(error));
        QVERIFY(restored.Config().team_buffs.empty());
        auto invalid = snapshot;
        values = snapshot.value("values").toObject();
        values.insert(prefix + "stacks", 126);
        invalid.insert("values", values);
        QVERIFY(!restored.RestoreSnapshot(invalid, error));
        QVERIFY(restored.Config().team_buffs.empty());
    }

    void moWenOptionsAndImports()
    {
        namespace mw = rt::mo_wen;
        SimulationOptions options;
        options.findChild<QComboBox *>("specialization")->setCurrentIndex(1);
        options.findChild<CodeEditor *>("flowMacroEditor")->appendPlainText(QStringLiteral("/set_player style=0 shadows=3"));
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
        window.findChild<CodeEditor *>("flowMacroEditor")->appendPlainText(QStringLiteral("/set_player shadows=2"));
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

    void workbenchResultAnalysisNavigation()
    {
        Workbench window;
        window.resize(1366, 900);
        window.show();
        auto *analysis = window.findChild<QDialog *>("resultAnalysis");
        auto *showAnalysis = window.findChild<QPushButton *>("showAnalysis");
        auto *home = window.findChild<QWidget *>("simulationPage");
        auto *debug = window.findChild<QTabWidget *>("debugTabs");
        QVERIFY(analysis);
        QVERIFY(!analysis->isVisible());
        QVERIFY(!analysis->isModal());
        QVERIFY(!window.findChild<QTabWidget *>("workspaceTabs"));
        QVERIFY(window.findChild<QWidget *>("outputPanel")->isAncestorOf(showAnalysis));
        QVERIFY(!window.findChild<QWidget *>("debugPage"));
        QVERIFY(window.findChild<QWidget *>("macroPanel")->isAncestorOf(debug));
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
        window.findChild<QToolButton *>("debugButton")->setChecked(true);
        QCOMPARE(editor->document(), document);
        QVERIFY(editor->isVisible());
        window.findChild<QAction *>("debugStart")->trigger();
        QCOMPARE(session->GetState(), DebugSession::Paused);
        window.findChild<QAction *>("debugStepOver")->trigger();
        const auto phase = session->GetDebugInfo().phase;
        const auto line = session->GetDebugInfo().lineNumber;
        QCOMPARE(session->GetState(), DebugSession::Paused);
        QVERIFY(editor->isVisible());
        QVERIFY(editor->isReadOnly());
        QVERIFY(!start->isEnabled());
        QVERIFY(!window.findChild<QComboBox *>("specialization")->isEnabled());
        QVERIFY(!window.findChild<QSpinBox *>("attackBase")->isEnabled());
        showAnalysis->click();
        QTRY_VERIFY(analysis->isVisible());
        QVERIFY(!home->isVisible());
        QVERIFY(!analysis->isWindow());
        QCOMPARE(window.findChild<QTabWidget *>("analysisViews")->currentWidget(), window.findChild<ResultsPanel *>());
        window.findChild<QToolButton *>("closeAnalysis")->click();
        QVERIFY(!analysis->isVisible());
        showAnalysis->click();
        window.activateWindow();
        QTRY_VERIFY(window.isActiveWindow());
        QTest::keyClick(window.windowHandle(), Qt::Key_F, Qt::ControlModifier);
        QTRY_VERIFY(!analysis->isVisible());
        QVERIFY(editor->isVisible());
        QVERIFY(window.findChild<MacroSearchBar *>()->isVisible());
        window.findChild<MacroSearchBar *>()->Close();
        QCOMPARE(session->GetDebugInfo().phase, phase);
        QCOMPARE(session->GetDebugInfo().lineNumber, line);
        QCOMPARE(editor->document(), document);
        QCOMPARE(editor->toPlainText(), source);
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
        QCOMPARE(showAnalysis->text(), QStringLiteral("技能统计"));
        showAnalysis->click();
        QTRY_VERIFY(analysis->isVisible());
        QCOMPARE(window.findChild<QTabWidget *>("resultTabs")->currentIndex(), 1);
        auto *skills = window.findChild<QTableWidget *>("skillDamage");
        QVERIFY(skills->rowCount() > 0);
        double skillDps = 0;
        for (int row = 0; row < skills->rowCount(); ++row) skillDps += skills->item(row, 4)->text().toDouble();
        QVERIFY(std::abs(skillDps - window.findChild<QLabel *>("meanDps")->text().toDouble()) < skills->rowCount() * .1);
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
        auto *detailGains = window.findChild<QTableWidget *>("attributeGainsTable");
        QVERIFY(std::abs(window.findChild<QLabel *>("gameAttributeGain_attack")->text().toDouble() -
            detailGains->item(1, 4)->text().toDouble()) <= .051);
        showAnalysis->click();
        QTRY_VERIFY(analysis->isVisible());
        auto *history = window.findChild<QTableWidget *>("resultHistory");
        QCOMPARE(history->rowCount(), 2);
        if (qEnvironmentVariableIsSet("JX3DPS_TEST_SCREENSHOT")) {
            QTest::qWait(250);
            QVERIFY(analysis->grab().save(qEnvironmentVariable("JX3DPS_TEST_SCREENSHOT") + "-analysis.png"));
        }
        analysis->close();
        QVERIFY(!analysis->isVisible());
        showAnalysis->click();
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
            auto *talent = window.findChild<QWidget *>("talents0")->findChild<QToolButton *>("talentTier4");
            QVERIFY2(talent->visibleRegion().boundingRect().contains(talent->rect().center()),
                qPrintable(QStringLiteral("Talent at %1,%2 in %3x%4; visible=%5, region=%6x%7")
                    .arg(talent->pos().x()).arg(talent->pos().y()).arg(talent->parentWidget()->width()).arg(talent->parentWidget()->height())
                    .arg(talent->isVisible()).arg(talent->visibleRegion().boundingRect().width()).arg(talent->visibleRegion().boundingRect().height())));
            QVERIFY(window.grab().save(qEnvironmentVariable("JX3DPS_TEST_SCREENSHOT") + "-compact.png"));
            window.findChild<QComboBox *>("specialization")->setCurrentIndex(1);
            QTest::qWait(30);
            QVERIFY(window.grab().save(qEnvironmentVariable("JX3DPS_TEST_SCREENSHOT") + "-mowen-compact.png"));
        }
        showAnalysis->click();
        QVERIFY(window.close());
        QVERIFY(!analysis->isVisible());
    }

    void segmentedPageNavigation()
    {
        desktop::SegmentedTabs tabs;
        tabs.resize(420, 220);
        for (const auto *name : {"First", "Second", "Third"}) tabs.addTab(new QWidget, QString::fromLatin1(name));
        tabs.show();
        QCoreApplication::processEvents();
        auto *bar = static_cast<desktop::SegmentedTabBar *>(tabs.tabBar());
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
        auto *editor = window.findChild<MacroEditor *>("macroEditor");
        auto *configuration = window.findChild<QScrollArea *>("configurationScroll");
        auto *profiles = window.findChild<QWidget *>("configurationSchemes");
        auto *start = window.findChild<QToolButton *>("startButton");
        QVERIFY(editor->width() >= 280);
        QCOMPARE(configuration->horizontalScrollBar()->maximum(), 0);
        QCOMPARE(window.findChild<QWidget *>("topNavigation")->height(), desktop::theme::NavigationHeight);
        QVERIFY(profiles->isAncestorOf(start));
        for (const auto *name : {"iterations", "workers", "seconds", "seed", "delay"}) {
            auto *input = window.findChild<QWidget *>(QString::fromLatin1(name));
            QVERIFY(input);
            QVERIFY2(input->visibleRegion().boundingRect().contains(input->rect().center()), name);
        }
        auto *attributes = window.findChild<QWidget *>("gameAttributePanel");
        auto *gains = window.findChild<QWidget *>("attributeGainsModule");
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
        const auto progress = window.findChild<QProgressBar *>("batchProgress")->mapTo(&window, QPoint(0, 0));
        const auto status = window.findChild<QLabel *>("simulationResult")->mapTo(&window, QPoint(0, 0));
        QVERIFY(std::abs(progress.y() - status.y()) <= 8);
        QVERIFY(window.close());
    }

    void floatingDebuggingInMacroEditor()
    {
        Workbench window;
        window.resize(1366, 900);
        window.show();
        auto *analysis = window.findChild<QDialog *>("resultAnalysis");
        auto *editor = window.findChild<MacroEditor *>("macroEditor");
        auto *macro = window.findChild<QWidget *>("macroPanel");
        auto *details = window.findChild<QTabWidget *>("debugTabs");
        auto *toggle = window.findChild<QToolButton *>("debugButton");
        auto *floating = window.findChild<DebugToolbar *>("macroDebugToolbar");
        auto *session = window.findChild<DebugSession *>();
        auto *play = floating->findChild<QToolButton *>("debugContinueButton");
        auto *pause = floating->findChild<QToolButton *>("debugPauseButton");
        auto *step = floating->findChild<QToolButton *>("debugStepOverButton");
        auto *into = floating->findChild<QToolButton *>("debugStepIntoButton");
        auto *restart = floating->findChild<QToolButton *>("debugRestartButton");
        auto *stop = floating->findChild<QToolButton *>("debugStopButton");
        QVERIFY(!window.findChild<QTabWidget *>("workspaceTabs"));
        QVERIFY(!window.findChild<QWidget *>("debugPage"));
        QVERIFY(macro->isAncestorOf(toggle));
        QVERIFY(macro->isAncestorOf(details));
        QVERIFY(floating->isWindow());
        QVERIFY(!floating->isVisible());
        QVERIFY(!details->isVisible());
        editor->setPlainText(QStringLiteral("/cast [qidian>=8] 无我无剑\n/cast 三环套月\n/cast 三柴剑法"));
        editor->SetBreakpoint(1, true);
        const auto source = editor->toPlainText();
        auto *document = editor->document();
        window.findChild<QDoubleSpinBox *>("seconds")->setValue(3);
        toggle->click();
        QTRY_VERIFY(floating->isVisible());
        QVERIFY(details->isVisible());
        QVERIFY(!analysis->isVisible());
        QVERIFY(play->isEnabled());
        QVERIFY(!step->isEnabled());
        play->click();
        QTRY_COMPARE(session->GetState(), DebugSession::Paused);
        QCOMPARE(editor->GetCurrentDebugLine(), 1);
        QVERIFY(editor->isReadOnly());
        QVERIFY(step->isEnabled());
        QVERIFY(stop->isEnabled());
        QVERIFY(window.findChild<QLabel *>("debugPosition")->text().contains(QStringLiteral("已暂停")));
        into->click();
        QVERIFY(session->GetDebugInfo().condition.contains(QStringLiteral("qidian")));
        auto *steps = window.findChild<MacroTracePanel *>()->findChild<QTableView *>("macroSteps");
        const auto beforeCondition = steps->model()->rowCount();
        window.findChild<QAction *>("debugStepInto")->trigger();
        QCOMPARE(steps->model()->rowCount(), beforeCondition + 1);
        step->click();
        const auto phase = session->GetDebugInfo().phase;
        const auto line = session->GetDebugInfo().lineNumber;
        QCOMPARE(editor->document(), document);
        QCOMPARE(editor->toPlainText(), source);

        auto withinEditor = [&] {
            return QRect(editor->mapToGlobal(QPoint(0, 0)), editor->size()).contains(floating->geometry());
        };
        QTRY_VERIFY(withinEditor());
        const auto previousPosition = floating->pos();
        const QPoint handle(20, floating->height() / 2);
        const QPoint drag(-12, 60);
        QTest::mousePress(floating, Qt::LeftButton, Qt::NoModifier, handle);
        QMouseEvent movement(QEvent::MouseMove, QPointF(handle + drag),
                             QPointF(floating->mapToGlobal(handle) + drag), Qt::NoButton, Qt::LeftButton, Qt::NoModifier);
        QApplication::sendEvent(floating, &movement);
        QTest::mouseRelease(floating, Qt::LeftButton, Qt::NoModifier, handle);
        QVERIFY(floating->pos() != previousPosition);
        QVERIFY(withinEditor());
        const auto relativePosition = window.mapFromGlobal(floating->pos());
        window.move(window.pos() + QPoint(10, 10));
        QCoreApplication::processEvents();
        QCOMPARE(window.mapFromGlobal(floating->pos()), relativePosition);

        auto screenshot = [&](const QString &suffix) {
            if (!qEnvironmentVariableIsSet("JX3DPS_TEST_SCREENSHOT")) return;
            for (auto *bar : window.findChildren<QTabBar *>()) {
                if (bar->isVisible() && bar->property("segmentedControl").toBool())
                    QTRY_VERIFY(!static_cast<desktop::SegmentedTabBar *>(bar)->SelectionAnimating());
            }
            auto pixmap = window.grab();
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
        QTRY_VERIFY(withinEditor());
        screenshot(QStringLiteral("-floating-compact.png"));
        window.findChild<QPushButton *>("showAnalysis")->click();
        QTRY_VERIFY(analysis->isVisible());
        QVERIFY(!floating->isVisible());
        QVERIFY(toggle->isChecked());
        QCOMPARE(session->GetState(), DebugSession::Paused);
        analysis->close();
        QTRY_VERIFY(floating->isVisible());
        QCOMPARE(session->GetDebugInfo().phase, phase);
        QCOMPARE(session->GetDebugInfo().lineNumber, line);
        play->click();
        QCOMPARE(session->GetState(), DebugSession::Running);
        QVERIFY(pause->isVisible());
        QVERIFY(!step->isEnabled());
        pause->click();
        QCOMPARE(session->GetState(), DebugSession::Paused);
        restart->click();
        QCOMPARE(session->GetState(), DebugSession::Paused);
        QCOMPARE(session->GetDebugInfo().currentFrame, 0);
        toggle->click();
        QCOMPARE(session->GetState(), DebugSession::Stopped);
        QVERIFY(!editor->isReadOnly());
        QVERIFY(!floating->isVisible());
        QVERIFY(!details->isVisible());
        QVERIFY(window.findChild<QComboBox *>("specialization")->isEnabled());
        editor->ClearAllBreakpoints();
        toggle->click();
        play->click();
        QTRY_COMPARE(session->GetState(), DebugSession::Finished);
        QCOMPARE(floating->GetDebugState(), DebugToolbar::Finished);
        QVERIFY(!step->isEnabled());
        QVERIFY(restart->isEnabled());
        QVERIFY(stop->isEnabled());
        stop->click();
        QCOMPARE(session->GetState(), DebugSession::Stopped);
        QVERIFY(window.close());
        QVERIFY(!floating->isVisible());
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
        QVERIFY(result.skillsCollected);
        QVERIFY(!result.skills.isEmpty());
        qint64 skillDamage = 0;
        for (const auto &skill : result.skills) skillDamage += skill.damage;
        QVERIFY(std::abs(skillDamage - result.stats.mean_damage * result.stats.count) < .01);
        QCOMPARE(worker.Completed(), worker.Requested());
        QCOMPARE(worker.Requested(), options.iterations * 3);
        config.attributes.attack_base += 100;
        const auto direct = rt::RunBatch(compiled.first.program, options, rt::mo_wen::Rules(rt::mo_wen::Prepare(config)));
        QCOMPARE(result.gains[0].stats.checksum, direct.checksum);
        ResultsPanel panel;
        panel.AddResult(result, QStringLiteral("/cast biangong"), {});
        QCOMPARE(panel.findChild<QTableWidget *>("attributeGainsTable")->rowCount(), 10);
    }

    void attributeGainReplayUsesCommonPipeline()
    {
        const rt::BatchOptions options{8, 19, 16 * 4, 2, 2000, 4000};
        auto check = [&](const auto &config, const auto &compiled) {
          for (const auto mode : {desktop::GainComparisonMode::SameValue, desktop::GainComparisonMode::SameCost}) {
            const auto increments = desktop::GainComparisonIncrements(mode, desktop::ClassOf(desktop::Config(config)) == desktop::Specialization::MoWen);
            BatchWorker worker(compiled.first.program, options, config, nullptr, false, true, mode);
            QSignalSpy stats(&worker, &BatchWorker::StatisticsReady), failure(&worker, &BatchWorker::Failed);
            worker.start();
            QVERIFY(worker.wait(10000));
            QTRY_COMPARE(stats.count(), 1);
            QCOMPARE(failure.count(), 0);
            const auto result = qvariant_cast<SimulationResult>(stats[0][0]);
            QCOMPARE(result.gainMode, mode);
            QCOMPARE(result.gains.size(), 8);
            QCOMPARE(worker.Completed(), worker.Requested());
            QCOMPARE(worker.Requested(), options.iterations * 3);
            for (int index = 0; index < 8; ++index) {
                QCOMPARE(result.gains[index].increment, increments[index]);
                const auto changed = desktop::ApplyAttributeGain(config, index, increments[index]);
                const auto direct = rt::RunBatch(compiled.first.program, options, desktop::MakeRules(changed));
                QCOMPARE(result.gains[index].stats.count, direct.count);
                QCOMPARE(result.gains[index].stats.checksum, direct.checksum);
                QCOMPARE(result.gains[index].stats.mean_damage, direct.mean_damage);
            }
          }
        };

        rt::mo_wen::Config mo_wen;
        check(mo_wen, rt::MacroCompiler::Compile("/cast biangong", rt::mo_wen::MacroOptions(false)));
        rt::tai_xu::Config tai_xu;
        tai_xu.team_buffs.push_back({tx::team::HanRuLei, 0, 0, 1});
        check(tai_xu, rt::MacroCompiler::Compile("/cast wuwu\n/cast sanhuan", rt::tai_xu::MacroOptions(false)));
    }

    void chineseWorkbenchAndMoWenCompletion()
    {
        Workbench window;
        QCOMPARE(window.findChild<QAction *>("newMacro")->text(), QStringLiteral("新建"));
        QCOMPARE(window.findChild<QAction *>("attributeGains")->text(), QStringLiteral("自动收益"));
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
        const auto available = editor->screen()->availableGeometry();
        const auto popupX = qBound(available.left(), caret.x(),
                                  qMax(available.left(), available.right() - completer->popup()->width() + 1));
        QVERIFY(std::abs(popupOrigin.x() - popupX) < 10);
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
        window.findChild<QTabWidget *>("debugTabs")->setCurrentWidget(panel);
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

    void batchSkillStatistics_data()
    {
        QTest::addColumn<int>("specialization");
        QTest::newRow("TaiXu") << 0;
        QTest::newRow("MoWen") << 1;
    }

    void batchSkillStatistics()
    {
        QFETCH(int, specialization);
        const auto spec = static_cast<desktop::Specialization>(specialization);
        desktop::Config configuration;
        if (spec == desktop::Specialization::MoWen) configuration = rt::mo_wen::Config{};
        const auto source = desktop::DefaultMacro(spec);
        auto compiled = rt::MacroCompiler::Compile(source.toStdString(), desktop::MacroOptions(spec, false));
        QVERIFY(compiled.second.empty());
        rt::BatchOptions options{7, 181, 16 * 30, 1, 4000, 12000};
        auto run = [&](const rt::BatchOptions &batch, bool gains, bool trace, SimulationResult &output) {
            BatchWorker worker(compiled.first.program, batch, configuration, nullptr, trace, gains);
            QSignalSpy stats(&worker, &BatchWorker::StatisticsReady), failure(&worker, &BatchWorker::Failed);
            QSignalSpy events(&worker, &BatchWorker::TraceReady), buffs(&worker, &BatchWorker::BuffTraceReady);
            QSignalSpy history(&worker, &BatchWorker::HistoryReady);
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
            QCOMPARE(history.count(), trace ? 1 : 0);
        };
        auto compare = [](const QVector<SkillStatistics> &actual, const QVector<SkillStatistics> &expected) {
            QCOMPARE(actual.size(), expected.size());
            for (int index = 0; index < actual.size(); ++index) {
                const auto &a = actual[index], &e = expected[index];
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
        SimulationResult serial, parallel, excessWorkers, gains, trace;
        run(options, false, false, serial);
        QVERIFY(!serial.skills.isEmpty());
        options.workers = 3;
        run(options, false, false, parallel);
        compare(parallel.skills, serial.skills);
        options.workers = 16;
        run(options, false, false, excessWorkers);
        compare(excessWorkers.skills, serial.skills);
        options.workers = 3;
        run(options, true, false, gains);
        compare(gains.skills, serial.skills);
        QCOMPARE(gains.gains.size(), 8);
        QCOMPARE(gains.stats.checksum, parallel.stats.checksum);

        // Independently sum seeded fight logs, including DOT ticks and zero-damage outcomes.
        QVector<SkillStatistics> expected;
        std::visit([&](const auto &config) {
            const auto rules = desktop::MakeRules(config);
            const auto plain = rt::RunBatch(compiled.first.program, options, rules);
            QCOMPARE(parallel.stats.checksum, plain.checksum);
            QCOMPARE(parallel.stats.mean_damage, plain.mean_damage);
            rt::Simulation simulation(compiled.first.program, rules, options.damage_capacity, options.mutation_capacity);
            QMap<int, SkillStatistics> totals;
            for (std::uint64_t iteration = 0; iteration < options.iterations; ++iteration) {
                simulation.Start(options.duration, rt::IterationSeed(options.seed, iteration));
                simulation.Run();
                for (const auto &hit : simulation.Log().Intents()) {
                    auto &skill = totals[hit.skill];
                    skill.skill = hit.skill;
                    skill.name = desktop::SkillName(spec, hit.skill);
                    skill.damage += simulation.GetRules().Reduce(hit);
                    ++skill.count;
                    if (hit.outcome == JX3DPS::RollResult::HIT) ++skill.hit;
                    else if (hit.outcome == JX3DPS::RollResult::DOUBLE) ++skill.critical;
                    else if (hit.outcome == JX3DPS::RollResult::INSIGHT) ++skill.insight;
                    else ++skill.miss;
                }
            }
            expected = totals.values();
        }, configuration);
        compare(parallel.skills, expected);
        qint64 totalDamage = 0;
        for (const auto &skill : parallel.skills) totalDamage += skill.damage;
        QVERIFY(std::abs(totalDamage - parallel.stats.mean_damage * parallel.stats.count) < .01);
        options.iterations = 1;
        run(options, false, false, serial);
        run(options, false, true, trace);
        compare(trace.skills, serial.skills);
    }

    void skillStatisticsHistory()
    {
        ResultsPanel panel;
        SimulationResult first;
        first.options = {2, 0, 160, 1, 4096, 8192};
        first.stats.Add(1000, 0);
        first.stats.Add(3000, 1);
        first.skillsCollected = true;
        first.skills = {{0, "A", 3000, 3, 1, 1, 0, 1}, {1, "B", 1000, 2, 1, 0, 1, 0}};
        panel.AddResult(first, "first", {});
        auto *damage = panel.findChild<QTableWidget *>("skillDamage");
        auto *history = panel.findChild<QTableWidget *>("resultHistory");
        auto *tabs = panel.findChild<QTabWidget *>("resultTabs");
        QCOMPARE(tabs->currentIndex(), 1);
        QCOMPARE(damage->rowCount(), 2);
        QCOMPARE(damage->item(0, 2)->text(), QStringLiteral("3000"));
        QCOMPARE(damage->item(0, 3)->text(), QStringLiteral("75.0%"));
        QCOMPARE(damage->item(0, 4)->text(), QStringLiteral("150.0"));
        auto second = first;
        second.stats = {};
        second.stats.Add(5000, 0);
        second.skills = {{2, "C", 5000, 10, 4, 3, 2, 1}};
        panel.AddResult(second, "second", {});
        QCOMPARE(damage->rowCount(), 1);
        QCOMPARE(damage->item(0, 0)->text(), QStringLiteral("C"));
        QCOMPARE(damage->item(0, 4)->text(), QStringLiteral("500.0"));
        history->selectRow(0);
        panel.ShowSkillStatistics();
        QCOMPARE(damage->rowCount(), 2);
        QCOMPARE(damage->item(0, 0)->text(), QStringLiteral("A"));
        QTemporaryDir files;
        QString error;
        const auto path = files.filePath(QStringLiteral("skills.json"));
        QVERIFY2(panel.ExportSelected(path, error), qPrintable(error));
        QFile exported(path);
        QVERIFY(exported.open(QIODevice::ReadOnly));
        const auto report = QJsonDocument::fromJson(exported.readAll()).object();
        QVERIFY(report.value("skillsCollected").toBool());
        const auto skills = report.value("skills").toArray();
        QCOMPARE(skills.size(), 2);
        QCOMPARE(skills[0].toObject().value("damage").toString(), QStringLiteral("3000"));
        QCOMPARE(skills[0].toObject().value("critical").toString(), QStringLiteral("1"));
        panel.ClearTrace(10);
        QCOMPARE(damage->rowCount(), 0);
        panel.AppendTrace({{0, 4, "debug", 100, 0}});
        panel.ShowSkillStatistics();
        QCOMPARE(damage->item(0, 0)->text(), QStringLiteral("debug"));
        QCOMPARE(damage->item(0, 4)->text(), QStringLiteral("10.0"));
        history->selectRow(1);
        QCOMPARE(damage->item(0, 0)->text(), QStringLiteral("C"));
        auto empty = second;
        empty.stats = {};
        empty.skills.clear();
        panel.AddResult(empty, "cancelled", {});
        QCOMPARE(damage->rowCount(), 0);
        QVERIFY(panel.findChild<QLabel *>("skillDamageSummary")->text().contains(QStringLiteral("0 场已完成战斗")));
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
        QVERIFY(partial.skillsCollected);
        QVERIFY(!partial.skills.isEmpty());
        qint64 skillDamage = 0;
        for (const auto &skill : partial.skills) skillDamage += skill.damage;
        QVERIFY(std::abs(skillDamage - partial.stats.mean_damage * partial.stats.count) < .01);
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
        QVERIFY(qvariant_cast<SimulationResult>(cancelledStats.at(0).at(0)).skills.isEmpty());
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
            QVERIFY(window.findChild<QDialog *>("resultAnalysis")->grab().save(qEnvironmentVariable("JX3DPS_TEST_SCREENSHOT") + QStringLiteral("-trace.png")));
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
            auto *analysis = window.findChild<QDialog *>("resultAnalysis");
            views->setCurrentWidget(results);
            QTest::qWait(250);
            QVERIFY(analysis->grab().save(qEnvironmentVariable("JX3DPS_TEST_SCREENSHOT") + QStringLiteral("-results.png")));
            results->findChild<QTabWidget *>("resultTabs")->setCurrentIndex(1);
            QTest::qWait(250);
            QVERIFY(analysis->grab().save(qEnvironmentVariable("JX3DPS_TEST_SCREENSHOT") + QStringLiteral("-skills.png")));
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
        auto *specialization = static_cast<desktop::IconComboBox *>(options.findChild<QComboBox *>("specialization"));
        QVERIFY(specialization);
        QCOMPARE(specialization->count(), 2);
        QCOMPARE(specialization->size(), QSize(38, 38));
        QCOMPARE(specialization->itemIcon(0).pixmap(36, 36).toImage(), desktop::GameIcon(8484).pixmap(36, 36).toImage());
        QCOMPARE(specialization->itemIcon(1).pixmap(36, 36).toImage(), desktop::GameIcon(8501).pixmap(36, 36).toImage());
        QTest::mouseClick(specialization, Qt::LeftButton);
        QTRY_VERIFY(specialization->view()->isVisible());
        auto *classPopup = static_cast<desktop::ChoicePopup *>(specialization->view()->window());
        QVERIFY(classPopup->ExpansionAnimating());
        QTRY_VERIFY(!classPopup->ExpansionAnimating());
        QVERIFY2(specialization->view()->verticalScrollBar()->maximum() == 0,
            qPrintable(QStringLiteral("Popup %1x%2; view %3x%4; viewport %5x%6; scrollbar %7; frame %8")
                .arg(classPopup->width()).arg(classPopup->height())
                .arg(specialization->view()->width()).arg(specialization->view()->height())
                .arg(specialization->view()->viewport()->width()).arg(specialization->view()->viewport()->height())
                .arg(specialization->view()->verticalScrollBar()->width()).arg(specialization->view()->frameWidth())));
        const auto moWen = specialization->model()->index(1, 0);
        QVERIFY(specialization->view()->viewport()->rect().contains(specialization->view()->visualRect(moWen)));
        if (qEnvironmentVariableIsSet("JX3DPS_TEST_SCREENSHOT"))
            QVERIFY(specialization->view()->window()->grab().save(qEnvironmentVariable("JX3DPS_TEST_SCREENSHOT") + "-specialization-picker.png"));
        QTest::mouseClick(specialization->view()->viewport(), Qt::LeftButton, {}, specialization->view()->visualRect(moWen).center());
        QCOMPARE(options.Specialization(), desktop::Specialization::MoWen);
        QVERIFY(!specialization->view()->isVisible());
        QVERIFY(specialization->toolTip().contains(QStringLiteral("莫问")));
        const auto selectedClass = options.Snapshot();
        QTest::keyClick(specialization, Qt::Key_Home);
        QCOMPARE(options.Specialization(), desktop::Specialization::TaiXu);
        QVERIFY(specialization->toolTip().contains(QStringLiteral("太虚剑意")));
        QString classError;
        QVERIFY2(options.RestoreSnapshot(selectedClass, classError), qPrintable(classError));
        QCOMPARE(options.Specialization(), desktop::Specialization::MoWen);
        specialization->setCurrentIndex(0);
        for (auto *check : options.findChildren<QCheckBox *>()) {
            if ((check->objectName().startsWith("talent") || check->objectName().startsWith("mwTalent")) &&
                check->property("talentTier").toInt() > 0) {
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
        auto *talentPanel = options.findChild<QWidget *>("talents0");
        auto *legacyToggle = static_cast<desktop::TalentPanel *>(talentPanel)->HeaderWidget()->findChild<QToolButton *>("legacyTalentToggle");
        legacyToggle->click();
        talent->setFocus();
        QTest::keyClick(talent, Qt::Key_Space);
        QVERIFY(talent->isChecked());
        QTest::keyClick(talent, Qt::Key_Space);
        QVERIFY(!talent->isChecked());
        legacyToggle->click();
        food->setFocus();
        QCoreApplication::processEvents();
        QTest::keyClick(food, Qt::Key_Down);
        QCOMPARE(food->currentIndex(), 1);
        QVERIFY(food->toolTip().contains(food->currentText()));
        food->showPopup();
        QCoreApplication::processEvents();
        QVERIFY(food->view()->isVisible());
        auto *foodPopup = static_cast<desktop::ChoicePopup *>(food->view()->window());
        QVERIFY(foodPopup->ExpansionAnimating());
        QTRY_VERIFY(!foodPopup->ExpansionAnimating());
        QVERIFY(food->view()->window()->height() >= food->count() * food->view()->sizeHintForRow(0));
        QCOMPARE(food->view()->verticalScrollBar()->maximum(), 0);
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

    void iconChoicesUseVerticalRows()
    {
        Workbench window;
        window.resize(1120, 820);
        window.show();
        auto *options = window.findChild<SimulationOptions *>();
        auto *specialization = static_cast<desktop::IconComboBox *>(window.findChild<QComboBox *>("specialization"));
        specialization->setCurrentIndex(1);
        QTest::qWait(80);
        auto *team = static_cast<desktop::IconComboBox *>(options->findChild<QComboBox *>("permanent1TeamCore"));
        team->setCurrentIndex(1);
        QCoreApplication::processEvents();
        auto checkList = [&](desktop::IconComboBox *combo) {
            const auto before = options->Snapshot();
            combo->showPopup();
            auto *view = combo->choiceView();
            auto *popup = static_cast<desktop::ChoicePopup *>(view->window());
            QTRY_VERIFY(popup->isVisible());
            QTRY_VERIFY(!popup->ExpansionAnimating());
            QCOMPARE(view->viewMode(), QListView::ListMode);
            QCOMPARE(view->flow(), QListView::TopToBottom);
            QVERIFY(!view->isWrapping());
            QCOMPARE(view->horizontalScrollBar()->maximum(), 0);
            QCOMPARE(view->verticalScrollBar()->maximum(), 0);
            QVERIFY(popup->screen()->availableGeometry().contains(popup->geometry()));
            int previousBottom = -1;
            for (int i = 0; i < combo->count(); ++i) {
                const auto row = view->visualRect(combo->model()->index(i, combo->modelColumn()));
                QCOMPARE(row.left(), 0);
                QCOMPARE(row.width(), view->viewport()->width());
                QCOMPARE(row.top(), previousBottom + 1);
                QCOMPARE(row.height(), view->sizeHintForRow(i));
                QVERIFY(view->viewport()->rect().contains(row));
                previousBottom = row.bottom();
                // The text column must actually be painted, not just available through a tooltip.
                const auto image = view->viewport()->grab(row).toImage();
                const auto ratio = image.devicePixelRatio();
                int textPixels = 0;
                for (int y = 0; y < image.height(); ++y)
                    for (int x = qRound(52 * ratio); x < qRound((row.width() - 34) * ratio); ++x) {
                        const auto color = image.pixelColor(x, y);
                        textPixels += color.red() > 150 && color.green() > 150 && color.blue() > 150;
                    }
                QVERIFY2(textPixels > 3, qPrintable(combo->itemText(i)));
            }
            QCOMPARE(options->Snapshot(), before);
            if (qEnvironmentVariableIsSet("JX3DPS_TEST_SCREENSHOT"))
                QVERIFY(popup->grab().save(qEnvironmentVariable("JX3DPS_TEST_SCREENSHOT") +
                    QStringLiteral("-vertical-%1.png").arg(combo->objectName())));
        };
        checkList(team);
        QCOMPARE(team->itemText(0), QStringLiteral("无"));
        auto *view = team->choiceView();
        QSignalSpy activated(team, &QComboBox::activated);
        const auto last = view->visualRect(team->model()->index(2, 0));
        QTest::mouseClick(view->viewport(), Qt::LeftButton, Qt::NoModifier, QPoint(80, last.center().y()));
        QCOMPARE(team->currentIndex(), 2);
        QCOMPARE(activated.count(), 1);
        QVERIFY(!view->window()->isVisible());
        team->showPopup();
        QTest::keyClick(view, Qt::Key_Home);
        QTest::keyClick(view, Qt::Key_Return);
        QCOMPARE(team->currentIndex(), 0);
        QVERIFY(!view->window()->isVisible());
        checkList(specialization);
        QTest::keyClick(specialization->choiceView(), Qt::Key_Home);
        QTest::keyClick(specialization->choiceView(), Qt::Key_Escape);
        QCOMPARE(specialization->currentIndex(), 1);
        QVERIFY(!specialization->choiceView()->window()->isVisible());
        auto *food = static_cast<desktop::IconComboBox *>(options->findChild<QComboBox *>("permanent1FoodEnhance"));
        checkList(food);
        food->hidePopup();
    }

    void iconPopupExpansion()
    {
        QWidget host;
        const auto available = host.screen()->availableGeometry();
        host.resize(180, 120);
        host.move(available.topLeft() + QPoint(20, 20));
        desktop::IconComboBox choice(QStringLiteral("图标"), &host);
        choice.move(20, 20);
        for (int index = 0; index < 16; ++index)
            choice.addItem(desktop::GameIcon(8484), QString::number(index));
        host.show();
        QCoreApplication::processEvents();
        choice.showPopup();
        auto *popup = static_cast<desktop::ChoicePopup *>(choice.choiceView()->window());
        const auto bounds = popup->geometry();
        QVERIFY(available.contains(bounds));
        QVERIFY(popup->ExpansionAnimating());
        QVERIFY(popup->mask().boundingRect().height() < popup->height());
        QCOMPARE(popup->mask().boundingRect().top(), 0);
        QTRY_VERIFY(!popup->ExpansionAnimating());
        QCOMPARE(popup->geometry(), bounds);
        QVERIFY(popup->mask().isEmpty());
        QCOMPARE(choice.choiceView()->viewMode(), QListView::ListMode);
        QCOMPARE(choice.choiceView()->horizontalScrollBar()->maximum(), 0);
        QVERIFY(choice.choiceView()->verticalScrollBar()->maximum() > 0);
        choice.hidePopup();

        host.move(available.right() - host.width() + 1, available.bottom() - host.height() + 1);
        QCoreApplication::processEvents();
        choice.showPopup();
        QVERIFY(popup->ExpansionAnimating());
        QVERIFY(available.contains(popup->geometry()));
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
        const auto selected = choice.choiceView()->visualRect(choice.model()->index(15, 0));
        QVERIFY(choice.choiceView()->viewport()->rect().contains(selected));
        choice.hidePopup();
        choice.setEnabled(false);
        choice.showPopup();
        QVERIFY(!popup->isVisible());
    }

    void talentTierSelection()
    {
        Workbench window;
        window.resize(1280, 840);
        window.show();
        auto *options = window.findChild<SimulationOptions *>();
        auto *panel = static_cast<desktop::TalentPanel *>(options->findChild<QWidget *>("talents0"));
        auto *popup = static_cast<desktop::ChoicePopup *>(panel->findChild<QWidget *>("talentPopup"));
        QCOMPARE(panel->findChildren<QToolButton *>(QRegularExpression("^talentTier[0-9]+$")).size(), 9);
        auto choose = [&](int tier, const char *id) {
            QTest::mouseClick(panel->findChild<QToolButton *>(QStringLiteral("talentTier%1").arg(tier)), Qt::LeftButton);
            QTRY_VERIFY(popup->isVisible());
            QTRY_VERIFY(!popup->ExpansionAnimating());
            auto *button = panel->findChild<QToolButton *>(QStringLiteral("chooseTalent") + QString::fromLatin1(id));
            QVERIFY(button && button->isEnabled());
            QVERIFY(button->visibleRegion().boundingRect().contains(button->rect().center()));
            button->click();
        };
        choose(1, "14833");
        QCOMPARE(panel->Selection()[0].toString(), QStringLiteral("14833"));
        QVERIFY(!options->findChild<QCheckBox *>(QStringLiteral("talent%1").arg(tx::XuanMen))->isChecked());
        QVERIFY_EXCEPTION_THROWN(options->ActiveConfig(), std::invalid_argument);
        choose(1, "38539");
        QCOMPARE(panel->Selection()[0].toString(), QStringLiteral("38539"));
        choose(7, "41904");
        auto *mixed = panel->findChild<QToolButton *>("talentTier8");
        QTest::mouseClick(mixed, Qt::LeftButton);
        QTRY_VERIFY(!popup->ExpansionAnimating());
        QCOMPARE(panel->findChildren<QToolButton *>(QRegularExpression("^chooseTalent[0-9]+$")).size(), 12);
        QVERIFY(!panel->findChild<QToolButton *>("chooseTalent41904")->isEnabled());
        if (qEnvironmentVariableIsSet("JX3DPS_TEST_SCREENSHOT"))
            QVERIFY(popup->grab().save(qEnvironmentVariable("JX3DPS_TEST_SCREENSHOT") + "-talent-picker.png"));
        panel->findChild<QToolButton *>("chooseTalent32400")->click();
        choose(9, "17743");
        const auto valid = options->Snapshot();
        QCOMPARE(valid.value("version").toInt(), 4);
        QString error;
        auto invalid = valid;
        auto talents = invalid.value("talents").toObject();
        auto duplicate = talents.value("0").toArray();
        duplicate[7] = duplicate[6];
        talents.insert("0", duplicate);
        invalid.insert("talents", talents);
        QVERIFY(!options->RestoreSnapshot(invalid, error));
        QVERIFY(error.contains(QStringLiteral("\u91cd\u590d")));
        QCOMPARE(options->Snapshot(), valid);
        duplicate[7] = QStringLiteral("unknown");
        talents.insert("0", duplicate);
        invalid.insert("talents", talents);
        QVERIFY(!options->RestoreSnapshot(invalid, error));
        QCOMPARE(options->Snapshot(), valid);
        panel->ClearSelection();
        QVERIFY(!panel->HasSelection());
        QVERIFY2(options->RestoreSnapshot(valid, error), qPrintable(error));
        QCOMPARE(options->Snapshot(), valid);
        QTemporaryDir directory;
        QVERIFY2(options->ExportConfiguration(directory.filePath("talents.json"), error), qPrintable(error));
        panel->ClearSelection();
        QVERIFY2(options->ImportConfiguration(directory.filePath("talents.json"), error), qPrintable(error));
        QCOMPARE(options->Snapshot(), valid);
        QTest::mouseClick(mixed, Qt::LeftButton);
        options->SetInputsEnabled(false);
        QVERIFY(!popup->isVisible());
        options->SetInputsEnabled(true);
        auto old = valid;
        old.insert("version", 2);
        old.remove("talents");
        auto legacy = old.value("values").toObject();
        legacy.insert(QStringLiteral("talent%1").arg(tx::XuanMen), true);
        old.insert("values", legacy);
        QVERIFY2(options->RestoreSnapshot(old, error), qPrintable(error));
        QVERIFY(!panel->HasSelection());
        QVERIFY(options->Config().talents & tx::XuanMen);
        const QJsonArray txPreview{QStringLiteral("38539"),QStringLiteral("44381"),QStringLiteral("17731"),
            QStringLiteral("14829"),QStringLiteral("310"),QStringLiteral("24972"),
            QStringLiteral("41904"),QStringLiteral("32400"),QStringLiteral("17743")};
        panel->RestoreSelection(txPreview);
        options->SaveSettings();
        SimulationOptions restored;
        QCOMPARE(static_cast<desktop::TalentPanel *>(restored.findChild<QWidget *>("talents0"))->Selection(), txPreview);
        if (qEnvironmentVariableIsSet("JX3DPS_TEST_SCREENSHOT"))
            QVERIFY(window.grab().save(qEnvironmentVariable("JX3DPS_TEST_SCREENSHOT") + "-talents.png"));
        window.findChild<QComboBox *>("specialization")->setCurrentIndex(1);
        auto *moWen = static_cast<desktop::TalentPanel *>(options->findChild<QWidget *>("talents1"));
        const QJsonArray mwPreview{QStringLiteral("25220"),QStringLiteral("14285"),QStringLiteral("14466"),
            QStringLiteral("32486"),QStringLiteral("14163"),QStringLiteral("30797"),
            QStringLiteral("40743"),QStringLiteral("14336"),QStringLiteral("14282")};
        moWen->RestoreSelection(mwPreview);
        QCOMPARE(moWen->Selection().size(), 9);
        QVERIFY_EXCEPTION_THROWN(options->ActiveConfig(), std::invalid_argument);
        if (qEnvironmentVariableIsSet("JX3DPS_TEST_SCREENSHOT")) {
            QTest::qWait(220);
            QVERIFY(window.grab().save(qEnvironmentVariable("JX3DPS_TEST_SCREENSHOT") + "-talents-mowen.png"));
        }
        moWen->ClearSelection();
        panel->ClearSelection();
        window.findChild<QComboBox *>("specialization")->setCurrentIndex(0);
        QVERIFY(options->Config().talents & tx::XuanMen);
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
            auto *analysis = window.findChild<QDialog *>("resultAnalysis");
            QTest::qWait(250);
            QVERIFY(analysis->grab().save(qEnvironmentVariable("JX3DPS_TEST_SCREENSHOT") + QStringLiteral("-buff-timeline.png")));
            views->setCurrentWidget(panel);
            QTest::qWait(250);
            QVERIFY(analysis->grab().save(qEnvironmentVariable("JX3DPS_TEST_SCREENSHOT") + QStringLiteral("-buffs.png")));
            panel->findChild<QTabWidget *>("buffTabs")->setCurrentIndex(1);
            QTest::qWait(250);
            QVERIFY(analysis->grab().save(qEnvironmentVariable("JX3DPS_TEST_SCREENSHOT") + QStringLiteral("-buff-changes.png")));
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
