#include "buff_panel.h"
#include "game_icons.h"

#include <QComboBox>
#include <limits>
#include <QFileDialog>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QSaveFile>
#include <QTabWidget>
#include <QTableWidget>
#include <QVBoxLayout>

namespace {
QTableWidget *Table(const QStringList &headers, const char *name, QWidget *parent)
{
    auto *table = new QTableWidget(0, headers.size(), parent);
    table->setObjectName(QString::fromLatin1(name));
    table->setHorizontalHeaderLabels(headers);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setSelectionMode(QAbstractItemView::SingleSelection);
    table->setAlternatingRowColors(true);
    table->verticalHeader()->hide();
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    table->setStyleSheet(QStringLiteral("QHeaderView::section { padding: 4px 6px; }"));
    return table;
}
void Row(QTableWidget *table, int row, const QStringList &cells)
{
    for (int column = 0; column < cells.size(); ++column) table->setItem(row, column, new QTableWidgetItem(cells[column]));
}
QString Side(bool target) { return target ? QStringLiteral("目标增益") : QStringLiteral("自身增益"); }
QString Seconds(qint64 frames) { return QString::number(frames / 16.0, 'f', 4); }
QString Deadline(int frame) { return frame == std::numeric_limits<int>::max() ? QStringLiteral("常驻") : frame > 0 ? Seconds(frame) : QStringLiteral("--"); }
QString Fraction(double value, int denominator, int decimals)
{
    return denominator ? QString::number(value / denominator, 'f', decimals) : QStringLiteral("--");
}
QByteArray CsvRow(const QStringList &fields)
{
    QStringList escaped;
    for (auto field : fields) {
        field.replace('"', QStringLiteral("\"\""));
        escaped << '"' + field + '"';
    }
    return (escaped.join(',') + '\n').toUtf8();
}
}

BuffPanel::BuffPanel(QWidget *parent) : QWidget(parent)
{
    auto *layout = new QVBoxLayout(this);
    auto *tools = new QHBoxLayout;
    tools->addWidget(new QLabel(QStringLiteral("显示"), this));
    m_scope = new QComboBox(this);
    m_scope->setObjectName(QStringLiteral("buffScope"));
    m_scope->addItem(QStringLiteral("全部"), -1);
    m_scope->addItem(QStringLiteral("自身增益"), 0);
    m_scope->addItem(QStringLiteral("目标增益"), 1);
    tools->addWidget(m_scope);
    auto *filterLabel = new QLabel(QStringLiteral("名称"), this);
    tools->addWidget(filterLabel);
    m_filter = new QLineEdit(this);
    m_filter->setObjectName(QStringLiteral("buffFilter"));
    m_filter->setClearButtonEnabled(true);
    filterLabel->setBuddy(m_filter);
    tools->addWidget(m_filter, 1);
    auto exportButton = [&](const QString &label, const char *name, bool coverage) {
        auto *button = new QPushButton(label, this);
        button->setObjectName(QString::fromLatin1(name));
        tools->addWidget(button);
        connect(button, &QPushButton::clicked, this, [this, coverage] {
            const auto path = QFileDialog::getSaveFileName(this, QStringLiteral("导出增益分析"),
                coverage ? QStringLiteral("buff-coverage.csv") : QStringLiteral("buff-changes.csv"), QStringLiteral("表格文件 (*.csv)"));
            if (path.isEmpty()) return;
            QString error;
            if (!ExportCsv(path, coverage, error)) QMessageBox::warning(this, QStringLiteral("导出增益"), error);
        });
        return button;
    };
    m_exportCoverage = exportButton(QStringLiteral("导出覆盖率"), "exportBuffCoverage", true);
    m_exportChanges = exportButton(QStringLiteral("导出变化"), "exportBuffChanges", false);
    layout->addLayout(tools);
    m_status = new QLabel(this);
    m_status->setObjectName(QStringLiteral("buffStatus"));
    m_status->setWordWrap(true);
    layout->addWidget(m_status);
    auto *tabs = new QTabWidget(this);
    tabs->setObjectName(QStringLiteral("buffTabs"));
    m_coverage = Table({QStringLiteral("归属"), QStringLiteral("自身增益"), QStringLiteral("最新层数"), QStringLiteral("最高层数"),
        QStringLiteral("生效时间（秒）"), QStringLiteral("覆盖率 %"), QStringLiteral("平均层数"), QStringLiteral("变化记录")}, "buffCoverage", tabs);
    m_coverage->horizontalHeaderItem(2)->setToolTip(QStringLiteral("最近一次状态变化所记录的层数。"));
    m_coverage->horizontalHeaderItem(6)->setToolTip(QStringLiteral("按时间加权的平均层数，包含未生效的时间。"));
    m_changes = Table({QStringLiteral("帧"), QStringLiteral("自身增益"), QStringLiteral("序号"), QStringLiteral("归属"),
        QStringLiteral("变化"), QStringLiteral("层数"), QStringLiteral("结束时刻（秒）"), QStringLiteral("下次跳伤（秒）")}, "buffChanges", tabs);
    m_changes->setToolTip(QStringLiteral("双击变化记录，定位此后首条伤害。时间均为战斗开始后的绝对时刻。"));
    tabs->addTab(m_coverage, QStringLiteral("覆盖率"));
    tabs->addTab(m_changes, QStringLiteral("变化记录"));
    layout->addWidget(tabs, 1);
    connect(m_scope, &QComboBox::currentIndexChanged, this, [this] { FilterChanges(); Refresh(); });
    connect(m_filter, &QLineEdit::textChanged, this, [this] { FilterChanges(); Refresh(); });
    connect(m_changes, &QTableWidget::cellActivated, this, [this](int row, int) {
        if (row >= 0 && row < m_trace.Changes().size()) emit DamageRequested(m_trace.Changes()[row].event.sequence);
    });
    Reset(0);
}

void BuffPanel::Reset(int duration, desktop::Specialization specialization)
{
    m_specialization = specialization;
    m_trace.Reset(duration);
    m_changes->setRowCount(0);
    Refresh();
}

void BuffPanel::Append(const BuffEvents &events)
{
    const int begin = m_trace.Changes().size();
    m_trace.Append(events);
    const auto &changes = m_trace.Changes();
    m_changes->setUpdatesEnabled(false);
    m_changes->setRowCount(changes.size());
    for (int row = begin; row < changes.size(); ++row) {
        const auto &change = changes[row];
        const auto &event = change.event;
        Row(m_changes, row, {QString::number(event.frame), event.name,
            event.initial ? QStringLiteral("初始") : QString::number(event.sequence), Side(event.target), change.kind,
            QStringLiteral("%1 -> %2").arg(change.before).arg(event.stacks), Deadline(event.expiresAt), Deadline(event.tickAt)});
        m_changes->setRowHidden(row, !Matches(event));
    }
    m_changes->setUpdatesEnabled(true);
}

void BuffPanel::Observe(int frame)
{
    m_trace.Observe(frame);
    Refresh();
}

bool BuffPanel::Matches(const BuffEvent &event) const
{
    const int scope = m_scope->currentData().toInt();
    return (scope < 0 || scope == static_cast<int>(event.target)) && event.name.contains(m_filter->text().trimmed(), Qt::CaseInsensitive);
}

void BuffPanel::FilterChanges()
{
    const auto &changes = m_trace.Changes();
    m_changes->setUpdatesEnabled(false);
    for (int row = 0; row < changes.size(); ++row) m_changes->setRowHidden(row, !Matches(changes[row].event));
    m_changes->setUpdatesEnabled(true);
}

void BuffPanel::Refresh()
{
    const auto coverage = m_trace.Coverage();
    const int observed = m_trace.ObservedFrame();
    m_timeline.clear();
    m_coverage->setUpdatesEnabled(false);
    m_coverage->setRowCount(coverage.size());
    for (int row = 0; row < coverage.size(); ++row) {
        const auto &buff = coverage[row];
        const auto &event = buff.latest;
        Row(m_coverage, row, {Side(event.target), event.name, QString::number(event.stacks), QString::number(buff.peakStacks),
            Seconds(buff.activeFrames), Fraction(buff.activeFrames * 100.0, observed, 2),
            Fraction(buff.stackFrames, observed, 3), QString::number(buff.changes)});
        const bool visible = Matches(event);
        m_coverage->setRowHidden(row, !visible);
        if (!visible) continue;
        Timeline::BuffItem item;
        item.name = Side(event.target) + QStringLiteral(": ") + event.name;
        item.icon = desktop::GameIcon(m_specialization, event.name).pixmap(32, 32);
        item.color = QColor::fromHsv((event.slot * 37 + (event.target ? 20 : 190)) % 360, 130, 210);
        for (const auto &interval : buff.intervals)
            item.segments.push_back({interval.start * 62.5, interval.end * 62.5, interval.stacks});
        m_timeline.push_back(std::move(item));
    }
    m_coverage->setUpdatesEnabled(true);
    m_exportCoverage->setEnabled(!coverage.isEmpty());
    m_exportChanges->setEnabled(!m_trace.Changes().isEmpty());
    m_status->setText(QStringLiteral("记录 / 调试 | 已观察 %1 / %2 秒 | %3 种增益 | %4 次变化\n覆盖率与平均层数按已模拟时间计算；筛选同时影响时间轴与表格导出。")
        .arg(Seconds(observed), Seconds(m_trace.Duration())).arg(coverage.size()).arg(m_trace.Changes().size()));
    emit CoverageChanged();
}

bool BuffPanel::ExportCsv(const QString &path, bool coverage, QString &error) const
{
    error.clear();
    QSaveFile file(path);
    if (!file.open(QIODevice::WriteOnly)) { error = file.errorString(); return false; }
    const int observed = m_trace.ObservedFrame();
    bool valid = true;
    auto write = [&](const QStringList &fields) {
        const auto bytes = CsvRow(fields);
        valid = valid && file.write(bytes) == bytes.size();
    };
    if (coverage) {
        write({"side", "buff", "slot", "last_stacks", "peak_stacks", "active_frames", "stack_frames", "uptime_percent",
            "average_stacks", "changes", "observed_frame", "duration_frames"});
        for (const auto &buff : m_trace.Coverage()) {
            const auto &event = buff.latest;
            if (!Matches(event)) continue;
            write({Side(event.target), event.name, QString::number(event.slot), QString::number(event.stacks),
                QString::number(buff.peakStacks), QString::number(buff.activeFrames), QString::number(buff.stackFrames),
                Fraction(buff.activeFrames * 100.0, observed, 6), Fraction(buff.stackFrames, observed, 6), QString::number(buff.changes),
                QString::number(observed), QString::number(m_trace.Duration())});
        }
    } else {
        write({"frame", "sequence", "initial", "side", "buff", "slot", "change", "before_stacks", "after_stacks",
            "expires_frame", "next_tick_frame", "observed_frame", "duration_frames"});
        for (const auto &change : m_trace.Changes()) {
            const auto &event = change.event;
            if (!Matches(event)) continue;
            write({QString::number(event.frame), event.initial ? QString{} : QString::number(event.sequence), event.initial ? "1" : "0",
                Side(event.target), event.name, QString::number(event.slot), change.kind, QString::number(change.before),
                QString::number(event.stacks), QString::number(event.expiresAt), QString::number(event.tickAt),
                QString::number(observed), QString::number(m_trace.Duration())});
        }
    }
    if (!valid || !file.commit()) { error = file.errorString(); return false; }
    return true;
}
