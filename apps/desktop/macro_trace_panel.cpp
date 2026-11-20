#include "macro_trace_panel.h"

#include <QAbstractTableModel>
#include <QCheckBox>
#include <QColor>
#include <QComboBox>
#include <QFileDialog>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPalette>
#include <QPushButton>
#include <QResizeEvent>
#include <QSaveFile>
#include <QTableView>
#include <QTextBrowser>
#include <QVBoxLayout>

#include "src/global/types.h"

namespace {
QString Kind(MacroStepKind kind)
{
    switch (kind) {
        case MacroStepKind::CONDITION: return QStringLiteral("条件");
        case MacroStepKind::ACTION: return QStringLiteral("动作");
        case MacroStepKind::INTERRUPTION: return QStringLiteral("引导打断");
        case MacroStepKind::SKIPPED: return QStringLiteral("跳过");
    }
    return {};
}

QStringList Cells(const MacroStep &step)
{
    return { QString::number(step.ordinal), QString::number(step.frame), QString::number(step.line), Kind(step.kind), step.expression, step.outcome };
}

QByteArray CsvRow(QStringList cells)
{
    for (auto &cell : cells) {
        cell.replace('"', QStringLiteral("\"\""));
        cell = '"' + cell + '"';
    }
    return (cells.join(',') + '\n').toUtf8();
}
} // namespace

class MacroStepModel : public QAbstractTableModel
{
public:
    explicit MacroStepModel(QObject *parent) : QAbstractTableModel(parent) { }

    MacroSteps   steps;
    QVector<int> visible;

    int rowCount(const QModelIndex &parent = {}) const override { return parent.isValid() ? 0 : visible.size(); }

    int columnCount(const QModelIndex &parent = {}) const override { return parent.isValid() ? 0 : 6; }

    const MacroStep *Step(int row) const { return row >= 0 && row < visible.size() ? &steps[visible[row]] : nullptr; }

    QVariant data(const QModelIndex &index, int role) const override
    {
        const auto *step = index.isValid() ? Step(index.row()) : nullptr;
        if (step == nullptr) {
            return {};
        }
        if (role == Qt::DisplayRole) {
            return Cells(*step).value(index.column());
        }
        if (role == Qt::ToolTipRole) {
            return step->details;
        }
        if (role == Qt::ForegroundRole && index.column() == 5) {
            return QColor(step->kind == MacroStepKind::SKIPPED ? "#a0a7b0" : step->passed ? "#89d4ad" : "#efbd83");
        }
        return {};
    }

    QVariant headerData(int section, Qt::Orientation orientation, int role) const override
    {
        if (orientation != Qt::Horizontal || role != Qt::DisplayRole) {
            return {};
        }
        return QStringList{ "步骤", "帧", "宏行", "阶段", "条件 / 指令", "结果" }.value(section);
    }

    void Update(const MacroSteps &incoming, bool clear, int scope, const QString &search)
    {
        beginResetModel();
        if (clear) {
            steps.clear();
        }
        // Bound both the retained history and temporary append size.
        if (incoming.size() >= MacroStepHistoryLimit) {
            steps = incoming.mid(incoming.size() - MacroStepHistoryLimit);
        } else {
            const auto overflow = steps.size() + incoming.size() - MacroStepHistoryLimit;
            if (overflow > 0) {
                steps.remove(0, overflow);
            }
            steps += incoming;
        }
        visible.clear();
        for (int i = 0; i < steps.size(); ++i) {
            const auto &step = steps[i];
            if (scope == 1 && (step.passed || step.kind == MacroStepKind::SKIPPED)) {
                continue;
            }
            if (scope >= 2 && static_cast<int>(step.kind) != scope - 2) {
                continue;
            }
            if (!search.isEmpty() && !(step.expression + ' ' + step.details).contains(search, Qt::CaseInsensitive)) {
                continue;
            }
            visible.push_back(i);
        }
        endResetModel();
    }
};

MacroTracePanel::MacroTracePanel(QWidget *parent) : QWidget(parent)
{
    setObjectName(QStringLiteral("macroTracePanel"));
    auto *layout     = new QVBoxLayout(this);
    auto *filter_row = new QWidget(this);
    auto *tools = m_filters = new QHBoxLayout(filter_row);
    tools->setContentsMargins(0, 0, 0, 0);
    m_scope = new QComboBox(this);
    m_scope->setObjectName(QStringLiteral("macroStepScope"));
    m_scope->addItems({ "全部步骤", "不成立 / 受阻", "条件", "动作", "跳过", "引导打断" });
    tools->addWidget(m_scope);
    m_search = new QLineEdit(this);
    m_search->setObjectName(QStringLiteral("macroStepSearch"));
    m_search->setPlaceholderText(QStringLiteral("筛选表达式或原因"));
    auto search_palette = m_search->palette();
    search_palette.setColor(QPalette::PlaceholderText, QColor("#98a7b8"));
    m_search->setPalette(search_palette);
    m_search->setClearButtonEnabled(true);
    tools->addWidget(m_search, 1);
    m_follow = new QCheckBox(QStringLiteral("跟随最新"), this);
    m_follow->setChecked(true);
    layout->addWidget(filter_row);
    m_actionRow   = new QWidget(this);
    auto *actions = m_actions = new QHBoxLayout(m_actionRow);
    actions->setContentsMargins(0, 0, 0, 0);
    actions->addWidget(m_follow);
    actions->addStretch();
    m_locate = new QPushButton(QStringLiteral("定位宏行"), this);
    m_locate->setObjectName(QStringLiteral("locateMacroStep"));
    actions->addWidget(m_locate);
    m_export = new QPushButton(QStringLiteral("导出步骤"), this);
    actions->addWidget(m_export);
    layout->addWidget(m_actionRow);
    m_status = new QLabel(this);
    m_status->setObjectName(QStringLiteral("macroStepStatus"));
    m_status->setWordWrap(true);
    layout->addWidget(m_status);
    m_model = new MacroStepModel(this);
    m_table = new QTableView(this);
    m_table->setObjectName(QStringLiteral("macroSteps"));
    m_table->setMinimumHeight(110);
    m_table->setModel(m_model);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setSelectionMode(QAbstractItemView::SingleSelection);
    m_table->setAlternatingRowColors(true);
    m_table->setShowGrid(false);
    m_table->setStyleSheet(QStringLiteral("QHeaderView::section { padding: 4px 6px; }"));
    m_table->verticalHeader()->hide();
    m_table->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    m_table->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Stretch);
    layout->addWidget(m_table, 1);
    m_details = new QTextBrowser(this);
    m_details->setObjectName(QStringLiteral("macroStepDetails"));
    m_details->setFrameShape(QFrame::NoFrame);
    m_details->setMinimumHeight(58);
    m_details->setMaximumHeight(96);
    m_details->setStyleSheet(QStringLiteral("padding: 12px; background: #232327; color: #ededf1;"));
    layout->addWidget(m_details);
    connect(m_scope, &QComboBox::currentIndexChanged, this, [this] { Refresh(); });
    connect(m_search, &QLineEdit::textChanged, this, [this] { Refresh(); });
    connect(m_follow, &QCheckBox::toggled, this, [this](bool enabled) {
        if (enabled) {
            Refresh();
        }
    });
    connect(m_table->selectionModel(), &QItemSelectionModel::currentRowChanged, this, [this] { ShowSelection(); });
    connect(m_table, &QTableView::clicked, this, [this] { m_follow->setChecked(false); });
    connect(m_table, &QTableView::activated, this, [this] { LocateSelection(); });
    connect(m_locate, &QPushButton::clicked, this, &MacroTracePanel::LocateSelection);
    connect(m_export, &QPushButton::clicked, this, [this] {
        const auto PATH =
            QFileDialog::getSaveFileName(this, QStringLiteral("导出当前调试步骤"), QStringLiteral("macro-steps.csv"), QStringLiteral("表格文件 (*.csv)"));
        if (PATH.isEmpty()) {
            return;
        }
        QString error;
        if (!ExportCsv(PATH, error)) {
            QMessageBox::warning(this, QStringLiteral("导出调试步骤"), error);
        }
    });
    Reset();
}

void MacroTracePanel::Reset(const QString &source)
{
    m_source = source;
    Refresh({}, true);
}

void MacroTracePanel::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    const bool WIDE = event->size().width() >= 600;
    if (WIDE == m_wide) {
        return;
    }
    m_wide = WIDE;
    if (WIDE) {
        m_filters->addWidget(m_follow);
        m_filters->addWidget(m_locate);
        m_filters->addWidget(m_export);
    } else {
        m_actions->insertWidget(0, m_follow);
        m_actions->addWidget(m_locate);
        m_actions->addWidget(m_export);
    }
    m_actionRow->setVisible(!WIDE);
}

void MacroTracePanel::Append(const MacroSteps &steps)
{
    if (steps.isEmpty()) {
        return;
    }
    Refresh(steps);
}

void MacroTracePanel::Refresh(const MacroSteps &steps, bool reset)
{
    const auto *current  = m_model->Step(m_table->currentIndex().row());
    const auto  SELECTED = (current != nullptr) && !reset ? current->ordinal : 0;
    m_model->Update(steps, reset, m_scope->currentIndex(), m_search->text().trimmed());
    const auto count   = m_model->steps.size();
    const auto REMOVED = (count != 0) ? m_model->steps.back().ordinal - count : 0;
    m_status->setText((count != 0)
                          ? QStringLiteral("显示 %1 条 · 共 %2 步%3")
                                .arg(m_model->rowCount())
                                .arg(m_model->steps.back().ordinal)
                                .arg((REMOVED != 0u) ? QStringLiteral(" · 已淘汰 %1 条旧记录").arg(REMOVED) : QString{})
                          : QStringLiteral("启动调试后记录条件与动作的实际结果。"));
    m_status->setToolTip(QStringLiteral("最多保留 %1 步；导出包含当前筛选的全部记录。").arg(MacroStepHistoryLimit));
    m_export->setEnabled(m_model->rowCount() > 0);
    int row = -1;
    if (m_follow->isChecked()) {
        row = m_model->rowCount() - 1;
    } else if (SELECTED != 0u) {
        for (int i = 0; i < m_model->rowCount(); ++i) {
            if (m_model->Step(i)->ordinal == SELECTED) {
                row = i;
                break;
            }
        }
    }
    if (row >= 0) {
        m_table->selectRow(row);
        m_table->scrollTo(m_model->index(row, 0));
    }
    ShowSelection();
}

void MacroTracePanel::ShowSelection()
{
    const auto *step = m_model->Step(m_table->currentIndex().row());
    m_locate->setEnabled((step != nullptr) && !m_source.isEmpty());
    m_details->setPlainText((step != nullptr)
                                ? QStringLiteral("#%1 | %2 秒 | 第 %3 行 | %4 -> %5\n%6")
                                      .arg(step->ordinal)
                                      .arg(JX3DPS::FramesToSeconds(step->frame), 0, 'f', 4)
                                      .arg(step->line)
                                      .arg(step->expression, step->outcome, step->details)
                                : QStringLiteral("选择步骤，查看实际参与比较的数值或首个阻止施放的原因。"));
}

void MacroTracePanel::LocateSelection()
{
    if (const auto *step = m_model->Step(m_table->currentIndex().row()); (step != nullptr) && !m_source.isEmpty()) {
        emit SourceRequested(m_source, step->line);
    }
}

bool MacroTracePanel::ExportCsv(const QString &path, QString &error) const
{
    error.clear();
    QSaveFile file(path);
    if (!file.open(QIODevice::WriteOnly)) {
        error = file.errorString();
        return false;
    }
    QByteArray bytes =
        CsvRow({ "step", "frame", "line", "phase", "expression", "result", "details", "source_text", "latest_step" });
    const auto SOURCE_LINES = m_source.split('\n');
    for (int row = 0; row < m_model->rowCount(); ++row) {
        const auto &step  = *m_model->Step(row);
        auto        cells = Cells(step);
        cells << step.details << SOURCE_LINES.value(step.line - 1) << QString::number(m_model->steps.back().ordinal);
        bytes += CsvRow(cells);
    }
    if (file.write(bytes) != bytes.size() || !file.commit()) {
        error = file.errorString();
        return false;
    }
    return true;
}
