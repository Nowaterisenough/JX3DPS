#include "results_panel.h"

#include <algorithm>

#include <QFileDialog>
#include <QHeaderView>
#include <QJsonArray>
#include <QJsonDocument>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QSaveFile>
#include <QSplitter>
#include <QStyledItemDelegate>
#include <QTableWidget>
#include <QTabWidget>
#include <QTextBrowser>
#include <QVBoxLayout>

#include "runtime_catalog.h"
#include "game_attributes.h"
#include "controls/theme/segmented_tabs.h"
#include "controls/theme/history_chart.h"

namespace {
class DamageShareDelegate final : public QStyledItemDelegate {
public:
    using QStyledItemDelegate::QStyledItemDelegate;
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override {
        QStyledItemDelegate::paint(painter, option, index);
        bool valid = false;
        const auto share = index.data().toString().remove('%').toDouble(&valid);
        if (!valid) return;
        const QRectF track(option.rect.left() + 8, option.rect.bottom() - 5, qMax(0, option.rect.width() - 16), 3);
        painter->save();
        painter->setRenderHint(QPainter::Antialiasing);
        painter->setPen(Qt::NoPen);
        painter->setBrush(QColor(desktop::theme::Raised));
        painter->drawRoundedRect(track, 1.5, 1.5);
        painter->setBrush(desktop::theme::AccentColor(option.widget));
        painter->drawRoundedRect(QRectF(track.topLeft(), QSizeF(track.width() * std::clamp(share / 100, 0.0, 1.0), track.height())), 1.5, 1.5);
        painter->restore();
    }
};

QTableWidget *Table(const QStringList &headers, const char *name, QWidget *parent)
{
    auto *table = new QTableWidget(0, headers.size(), parent);
    table->setObjectName(QString::fromLatin1(name));
    table->setHorizontalHeaderLabels(headers);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setSelectionMode(QAbstractItemView::SingleSelection);
    table->setAlternatingRowColors(true);
    table->setShowGrid(false);
    table->verticalHeader()->hide();
    table->verticalHeader()->setDefaultSectionSize(34);
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    table->horizontalHeader()->setMinimumSectionSize(65);
    table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    table->setStyleSheet(QStringLiteral("QHeaderView::section { padding: 4px 8px; }"));
    return table;
}

void Row(QTableWidget *table, int row, const QStringList &cells)
{
    for (int column = 0; column < cells.size(); ++column) {
        auto *item = new QTableWidgetItem(cells[column]);
        if (column) {
            item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        }
        table->setItem(row, column, item);
    }
}

QString Number(double value)
{
    return QString::number(value, 'f', 1);
}

QString Delta(double current, double baseline)
{
    if (!baseline) {
        return QStringLiteral("--");
    }
    const auto percent = (current / baseline - 1) * 100;
    return (percent > 0 ? QStringLiteral("+") : QString{}) + QString::number(percent, 'f', 2) + '%';
}
} // namespace

ResultsPanel::ResultsPanel(QWidget *parent) : QWidget(parent)
{
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    m_tabs = new desktop::SegmentedTabs(this);
    m_tabs->setObjectName(QStringLiteral("resultTabs"));
    layout->addWidget(m_tabs);
    auto *history = new QWidget(m_tabs);
    history->setObjectName(QStringLiteral("resultHistoryPage"));
    auto *rows    = new QVBoxLayout(history);
    rows->setContentsMargins(0, 14, 0, 0);
    rows->setSpacing(14);
    auto *actions = new QHBoxLayout;
    actions->addWidget(new QLabel(QStringLiteral("本次会话最近 50 次模拟"), history));
    actions->addStretch();
    auto button = [&](const QString &label, const char *name, auto callback) {
        auto *item = new QPushButton(label, history);
        item->setObjectName(QString::fromLatin1(name));
        actions->addWidget(item);
        connect(item, &QPushButton::clicked, this, callback);
        return item;
    };
    m_setBaseline   = button(QStringLiteral("设为基准"), "setResultBaseline", [this] {
        if (const auto *entry = Selected(); entry && entry->result.stats.count) {
            m_baseline = *entry;
            RefreshHistory();
            ShowDetails();
        }
    });
    m_clearBaseline = button(QStringLiteral("清除基准"), "clearResultBaseline", [this] {
        m_baseline.reset();
        RefreshHistory();
        ShowDetails();
    });
    m_useInputs     = button(QStringLiteral("恢复宏与配置"), "useResultInputs", [this] {
        if (const auto *entry = Selected()) {
            emit InputsRequested(entry->source, entry->configuration);
        }
    });
    m_export        = button(QStringLiteral("导出结果"), "exportResult", [this] {
        const auto path =
            QFileDialog::getSaveFileName(this, QStringLiteral("导出所选模拟"), QStringLiteral("run.json"), QStringLiteral("模拟报告 (*.json)"));
        if (path.isEmpty()) {
            return;
        }
        QString error;
        if (!ExportSelected(path, error)) {
            QMessageBox::warning(this, QStringLiteral("导出结果"), error);
        }
    });
    auto *detailsToggle = button(QStringLiteral("宏与配置"), "toggleResultDetails", [this] {
        m_details->setVisible(findChild<QPushButton *>(QStringLiteral("toggleResultDetails"))->isChecked());
    });
    detailsToggle->setCheckable(true);
    detailsToggle->setToolTip(QStringLiteral("展开所选运行的宏、配置和详细统计"));
    rows->addLayout(actions);
    m_chart = new desktop::HistoryChart(history);
    rows->addWidget(m_chart);
    auto *splitter = new QSplitter(Qt::Vertical, history);
    rows->addWidget(splitter, 1);
    m_history =
        Table({ QStringLiteral("运行"),
                QStringLiteral("状态"),
                QStringLiteral("模拟次数"),
                QStringLiteral("平均秒伤"),
                QStringLiteral("秒伤标准差"),
                QStringLiteral("相对基准"),
                QStringLiteral("耗时（秒）") },
              "resultHistory",
              splitter);
    m_details = new QTextBrowser(splitter);
    m_details->setObjectName(QStringLiteral("resultDetails"));
    m_details->hide();
    splitter->setSizes({360, 140});
    connect(m_history, &QTableWidget::itemSelectionChanged, this, [this] { ShowDetails(); });
    m_chart->runSelected = [this](quint64 id) {
        for (int row = 0; row < m_entries.size(); ++row) if (m_entries[row].id == id) {
            m_history->selectRow(row);
            m_history->scrollToItem(m_history->item(row, 0));
            break;
        }
    };
    m_tabs->addTab(history, QStringLiteral("模拟历史"));
    auto *trace       = new QWidget(m_tabs);
    auto *traceLayout = new QVBoxLayout(trace);
    m_traceSummary    = new QLabel(trace);
    m_traceSummary->setObjectName(QStringLiteral("skillDamageSummary"));
    m_traceSummary->setWordWrap(true);
    traceLayout->addWidget(m_traceSummary);
    m_damage =
        Table({ QStringLiteral("技能"),
                QStringLiteral("事件数"),
                QStringLiteral("伤害"),
                QStringLiteral("占比"),
                QStringLiteral("秒伤"),
                QStringLiteral("平均伤害"),
                QStringLiteral("命中"),
                QStringLiteral("会心"),
                QStringLiteral("会心率 %"),
                QStringLiteral("识破"),
                QStringLiteral("未命中") },
              "skillDamage",
              trace);
    m_damage->setItemDelegateForColumn(3, new DamageShareDelegate(m_damage));
    traceLayout->addWidget(m_damage, 1);
    m_tabs->addTab(trace, QStringLiteral("技能统计"));
    auto *gainPage   = new QWidget(m_tabs);
    auto *gainLayout = new QVBoxLayout(gainPage);
    auto *gainNote   = new QLabel;
    gainNote->setObjectName(QStringLiteral("attributeGainDescription"));
    gainNote->setWordWrap(true);
    gainLayout->addWidget(gainNote);
    m_gains =
        Table({ QStringLiteral("属性"),
                QStringLiteral("增加点数"),
                QStringLiteral("模拟次数"),
                QStringLiteral("变化后秒伤"),
                QStringLiteral("秒伤变化"),
                QStringLiteral("每点收益"),
                QStringLiteral("提升比例") },
              "attributeGainsTable",
              gainPage);
    gainLayout->addWidget(m_gains, 1);
    m_tabs->addTab(gainPage, QStringLiteral("属性收益"));
    ClearTrace(1);
    ShowDetails();
}

const ResultsPanel::Entry *ResultsPanel::Selected() const
{
    const int row = m_history->currentRow();
    return row >= 0 && row < m_entries.size() ? &m_entries[row] : nullptr;
}

void ResultsPanel::ShowOverview() { m_tabs->setCurrentIndex(0); }

void ResultsPanel::AddResult(const SimulationResult &result, const QString &source, const QJsonObject &configuration)
{
    if (m_entries.size() == 50) {
        m_entries.removeFirst();
    }
    m_entries.push_back({ m_nextId++, QDateTime::currentDateTimeUtc(), result, source, configuration });
    RefreshHistory();
    m_history->selectRow(m_entries.size() - 1);
    m_tabs->setCurrentIndex(0);
    ShowDetails();
    if (result.skillsCollected && result.stats.count) m_tabs->setCurrentIndex(1);
    if (!result.gains.isEmpty()) {
        m_tabs->setCurrentIndex(2);
    }
}

void ResultsPanel::RefreshHistory()
{
    QVector<desktop::HistoryChart::Point> points;
    m_history->setUpdatesEnabled(false);
    m_history->setRowCount(m_entries.size());
    for (int row = 0; row < m_entries.size(); ++row) {
        const auto &entry   = m_entries[row];
        const auto &r       = entry.result;
        const bool  hasData = r.stats.count > 0;
        if (hasData && r.Complete()) points.push_back({entry.id, r.MeanDps()});
        const auto  label   = QStringLiteral("#%1%2").arg(entry.id).arg(
            m_baseline && m_baseline->id == entry.id ? QStringLiteral("（基准）") : QString{});
        Row(m_history,
            row,
            { label,
              r.Complete() ? (r.trace ? QStringLiteral("单次记录") : QStringLiteral("已完成")) : QStringLiteral("已取消"),
              QStringLiteral("%1/%2").arg(r.stats.count).arg(r.options.iterations),
              hasData ? Number(r.MeanDps()) : QStringLiteral("--"),
              hasData ? Number(r.stats.PopulationDeviation() / r.FightSeconds()) : QStringLiteral("--"),
              hasData && m_baseline ? Delta(r.MeanDps(), m_baseline->result.MeanDps()) : QStringLiteral("--"),
              QString::number(r.elapsedSeconds, 'f', 3) });
    }
    m_history->setUpdatesEnabled(true);
    m_chart->SetPoints(std::move(points));
    m_chart->SetReference(m_baseline ? std::optional<double>(m_baseline->result.MeanDps()) : std::nullopt);
}

void ResultsPanel::ShowDetails()
{
    const auto *entry = Selected();
    m_setBaseline->setEnabled(entry && entry->result.stats.count);
    m_clearBaseline->setEnabled(m_baseline.has_value());
    m_useInputs->setEnabled(entry && m_inputsEnabled);
    m_export->setEnabled(entry);
    m_gains->setRowCount(0);
    auto *gainNote = findChild<QLabel *>(QStringLiteral("attributeGainDescription"));
    if (!entry) {
        gainNote->setText(QStringLiteral("在收益卡片底部选择比较方式，选中“计算”，再开始模拟。"));
        m_details->setPlainText(QStringLiteral("运行批量模拟或单次记录，查看统计及本次使用的宏与配置。"));
        return;
    }
    const auto &r = entry->result;
    gainNote->setText(QStringLiteral("%1：实际增加点数见表格，显示平均秒伤变化。会心、无双、加速重新模拟循环；"
        "基础攻击、会效、破防、破招、武器伤害使用基准战斗统计重算。"
        "重跑使用相同宏与种子，差值可能含模拟波动。取消时保留完整计算的项目。")
        .arg(desktop::GainComparisonName(r.gainMode)));
    m_chart->SetSelectedRun(entry->id);
    ShowResultSkills(r);
    desktop::FillGameGainTable(m_gains, r,
        static_cast<desktop::Specialization>(entry->configuration.value("specialization").toInt()), true);
    QString text =
        desktop::ClassName(static_cast<desktop::Specialization>(entry->configuration.value("specialization").toInt())) +
        QStringLiteral(" | ") +
        QStringLiteral("模拟 #%1 | %2 | 时长 %3 秒 | 种子 %4 | 线程 %5\n")
            .arg(entry->id)
            .arg(entry->time.toLocalTime().toString(Qt::ISODate))
            .arg(r.FightSeconds())
            .arg(r.options.seed)
            .arg(r.options.workers);
    if (r.stats.count) {
        text += QStringLiteral("平均秒伤 %1 | 最低 %2 | 最高 %3 | 总体标准差 %4\n校验值 %5 | 每秒 %6 次\n")
                    .arg(Number(r.MeanDps()),
                         Number(r.stats.min_damage / r.FightSeconds()),
                         Number(r.stats.max_damage / r.FightSeconds()),
                         Number(r.stats.PopulationDeviation() / r.FightSeconds()))
                    .arg(r.stats.checksum)
                    .arg(Number(r.elapsedSeconds > 0 ? r.stats.count / r.elapsedSeconds : 0));
    } else {
        text += QStringLiteral("没有已完成的模拟，暂无秒伤统计。\n");
    }
    if (r.gainsRequested) {
        text += QStringLiteral("属性收益 · %1：%2 / 8 项完成（下列统计为基准战斗）。\n%3\n")
            .arg(desktop::GainComparisonName(r.gainMode)).arg(r.gains.size()).arg(desktop::GainComparisonDescription(r.gainMode));
    }
    if (!r.Complete()) {
        text += QStringLiteral("模拟未全部完成；统计仅包含已完成的战斗。\n");
    }
    if (m_baseline && r.stats.count) {
        text += QStringLiteral("基准 #%1：秒伤变化 %2（%3）。\n")
                    .arg(m_baseline->id)
                    .arg(Number(r.MeanDps() - m_baseline->result.MeanDps()), Delta(r.MeanDps(), m_baseline->result.MeanDps()));
        QStringList changes;
        if (entry->source != m_baseline->source) {
            changes << QStringLiteral("宏文本");
        }
        const auto current = entry->configuration.value("values").toObject();
        const auto base    = m_baseline->configuration.value("values").toObject();
        for (auto it = current.begin(); it != current.end(); ++it) {
            if (it.value() != base.value(it.key())) {
                changes << desktop::FieldName(it.key());
            }
        }
        if (r.options.duration != m_baseline->result.options.duration && !changes.contains(QStringLiteral("战斗时长")))
        {
            changes << QStringLiteral("战斗时长");
        }
        if (r.options.iterations != m_baseline->result.options.iterations) {
            changes << QStringLiteral("实际模拟次数");
        }
        if (entry->configuration.value("specialization") != m_baseline->configuration.value("specialization")) {
            changes.prepend(QStringLiteral("心法"));
        }
        changes.removeDuplicates();
        text += changes.isEmpty() ? QStringLiteral("配置与基准相同。\n")
                                  : QStringLiteral("变更配置：%1\n").arg(changes.join(QStringLiteral(", ")));
        text += QStringLiteral("此处比较模拟均值，不代表统计显著性检验。\n");
    }
    text += QStringLiteral("\n开始模拟时的宏：\n%1").arg(entry->source);
    m_details->setPlainText(text);
}

void ResultsPanel::SetInputsEnabled(bool enabled)
{
    m_inputsEnabled = enabled;
    m_useInputs->setEnabled(enabled && Selected());
}

void ResultsPanel::ShowSkillStatistics() { m_tabs->setCurrentIndex(1); }

bool ResultsPanel::ExportSelected(const QString &path, QString &error) const
{
    error.clear();
    const auto *entry = Selected();
    if (!entry) {
        error = QStringLiteral("请先选择一条模拟结果。");
        return false;
    }
    const auto &r = entry->result;
    QJsonObject stats{
        { "completed",      QString::number(r.stats.count)        },
        { "requested",      QString::number(r.options.iterations) },
        { "checksum",       QString::number(r.stats.checksum)     },
        { "elapsedSeconds", r.elapsedSeconds                      },
        { "complete",       r.Complete()                          },
        { "trace",          r.trace                               },
        { "gainsRequested", r.gainsRequested                      },
        { "gainComparisonMode", r.gainMode == desktop::GainComparisonMode::SameCost ? "same_cost" : "same_value" },
        { "gainBudgetModel", "jx3box_71692_level120" },
        { "durationFrames", r.options.duration                    },
        { "seed",           QString::number(r.options.seed)       },
        { "workers",        static_cast<int>(r.options.workers)   }
    };
    if (r.stats.count) {
        stats.insert("meanDamage", r.stats.mean_damage);
        stats.insert("m2Damage", r.stats.m2_damage);
        stats.insert("minDamage", QString::number(r.stats.min_damage));
        stats.insert("maxDamage", QString::number(r.stats.max_damage));
        stats.insert("meanDps", r.MeanDps());
        stats.insert("populationDpsDeviation", r.stats.PopulationDeviation() / r.FightSeconds());
    }
    QJsonArray gains;
    for (const auto &gain : r.gains) {
        gains.append(QJsonObject{
            { "attribute",  gain.attribute                       },
            { "increment",  gain.increment                       },
            { "count",      QString::number(gain.stats.count)    },
            { "meanDamage", gain.stats.mean_damage               },
            { "checksum",   QString::number(gain.stats.checksum) }
        });
    }
    QJsonArray skills;
    for (const auto &skill : r.skills) {
        skills.append(QJsonObject{{"skill", skill.skill}, {"name", skill.name},
            {"damage", QString::number(skill.damage)}, {"count", QString::number(skill.count)},
            {"hit", QString::number(skill.hit)}, {"critical", QString::number(skill.critical)},
            {"insight", QString::number(skill.insight)}, {"miss", QString::number(skill.miss)}});
    }
    const QJsonObject report{
        { "skills", skills },
        { "skillsCollected", r.skillsCollected },
        { "gains",         gains                             },
        { "format",        "jx3dps.desktop.run"              },
        { "version",       2                                 },
        { "id",            QString::number(entry->id)        },
        { "time",          entry->time.toString(Qt::ISODate) },
        { "macro",         entry->source                     },
        { "configuration", entry->configuration              },
        { "result",        stats                             }
    };
    const auto bytes = QJsonDocument(report).toJson();
    QSaveFile  file(path);
    if (!file.open(QIODevice::WriteOnly) || file.write(bytes) != bytes.size() || !file.commit()) {
        error = file.errorString();
        return false;
    }
    return true;
}

void ResultsPanel::ClearTrace(double seconds)
{
    m_skills.clear();
    m_traceDamage  = 0;
    m_traceCount   = 0;
    m_traceSeconds = seconds > 0 ? seconds : 1;
    m_damage->setRowCount(0);
    m_traceSummary->setText(QStringLiteral("模拟完成后可查看技能统计；单次记录或调试时持续更新当前战斗。"));
}

void ResultsPanel::AppendTrace(const CombatEvents &events)
{
    if (events.isEmpty()) {
        return;
    }
    for (const auto &event : events) {
        auto &skill   = m_skills[event.skill];
        skill.skill   = event.skill;
        skill.name    = event.name;
        skill.damage += event.damage;
        ++skill.count;
        if (event.outcome == 0) {
            ++skill.hit;
        } else if (event.outcome == 1) {
            ++skill.critical;
        } else if (event.outcome == 4) {
            ++skill.insight;
        } else {
            ++skill.miss;
        }
        m_traceDamage += event.damage;
        ++m_traceCount;
    }
    RenderSkills(m_skills.values(), static_cast<double>(m_traceDamage), m_traceSeconds);
    m_traceSummary->setText(
        QStringLiteral("当前记录 / 调试：%1 条事件 | 总伤害 %2 | 秒伤按配置的 %3 "
                       "秒战斗计算。\n计数包含持续伤害跳数，会心率包含全部命中结果；调试过程中持续累积。")
            .arg(m_traceCount)
            .arg(m_traceDamage)
            .arg(m_traceSeconds));
}

void ResultsPanel::RenderSkills(QVector<Skill> skills, double totalDamage, double totalSeconds)
{
    std::sort(skills.begin(), skills.end(), [](const Skill &a, const Skill &b) {
        return a.damage != b.damage ? a.damage > b.damage : a.skill < b.skill;
    });
    m_damage->setUpdatesEnabled(false);
    m_damage->setRowCount(skills.size());
    for (int row = 0; row < skills.size(); ++row) {
        const auto &skill = skills[row];
        Row(m_damage,
            row,
            { skill.name,
              QString::number(skill.count),
              QString::number(skill.damage),
              Number(totalDamage ? skill.damage * 100.0 / totalDamage : 0) + '%',
              Number(totalSeconds > 0 ? skill.damage / totalSeconds : 0),
              Number(skill.damage / static_cast<double>(skill.count)),
              QString::number(skill.hit),
              QString::number(skill.critical),
              Number(skill.critical * 100.0 / skill.count) + '%',
              QString::number(skill.insight),
              QString::number(skill.miss) });
    }
    m_damage->setUpdatesEnabled(true);
}

void ResultsPanel::ShowResultSkills(const SimulationResult &result)
{
    double damage = 0;
    quint64 events = 0;
    for (const auto &skill : result.skills) { damage += static_cast<double>(skill.damage); events += skill.count; }
    RenderSkills(result.skills, damage, result.stats.count * result.FightSeconds());
    m_traceSummary->setText(result.skillsCollected
        ? QStringLiteral("所选模拟：%1 场已完成战斗 · %2 条伤害事件 · 平均秒伤 %3\n"
                         "事件数、伤害和命中结果为整批合计；技能秒伤按已完成场次及战斗时长取平均，持续伤害按跳数计数。")
              .arg(result.stats.count).arg(events).arg(Number(result.MeanDps()))
        : QStringLiteral("此历史结果没有技能统计，请重新模拟。"));
}
