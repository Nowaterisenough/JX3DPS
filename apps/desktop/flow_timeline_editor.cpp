#include "flow_timeline_editor.h"

#include <algorithm>
#include <utility>

#include <QApplication>
#include <QCheckBox>
#include <QComboBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QHeaderView>
#include <QKeyEvent>
#include <QLabel>
#include <QMap>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QPushButton>
#include <QSettings>
#include <QSignalBlocker>
#include <QSpinBox>
#include <QStackedWidget>
#include <QTableWidget>
#include <QTextBlock>
#include <QToolButton>
#include <QToolTip>
#include <QVBoxLayout>
#include <QWheelEvent>

#include "controls/code_editor/code_editor.h"
#include "controls/theme/fixed_unit_spin_box.h"
#include "controls/theme/segmented_tabs.h"

namespace desktop {
FlowTimelineCanvas::FlowTimelineCanvas(bool compact, QWidget *parent) : QWidget(parent), compact_(compact)
{
    setMinimumSize(compact ? QSize(140, 120) : QSize(420, 320));
    setFocusPolicy(Qt::StrongFocus);
    setMouseTracking(true);
    setAccessibleName(QStringLiteral("流程宏事件时间轴"));
}

QRect FlowTimelineCanvas::PlotRect() const
{
    return QRect(compact_ ? 46 : 100, 26, qMax(1, width() - (compact_ ? 58 : 112)), qMax(1, height() - 34));
}

int FlowTimelineCanvas::Track(int field) const
{
    return !compact_ ? field : field == 3 ? 2 : FlowFields[field].during_combat ? 1 : 0;
}

int FlowTimelineCanvas::FieldAt(int y) const
{
    const auto plot = PlotRect();
    const int  ROW  = std::clamp((y - plot.y()) * (compact_ ? 3 : int(FlowFields.size())) / plot.height(),
                                 0,
                                 compact_ ? 2 : int(FlowFields.size()) - 1);
    return compact_ ? (ROW == 1 ? 2 : ROW == 2 ? 3 : 0) : ROW;
}

int FlowTimelineCanvas::FrameAt(int x) const
{
    const auto plot = PlotRect();
    return std::clamp(qRound(begin_ + ((x - plot.left()) * double(span_) / plot.width())), 0, end_);
}

int FlowTimelineCanvas::XAt(int frame) const
{
    const auto plot = PlotRect();
    return plot.left() + qRound((frame - begin_) * double(plot.width()) / span_);
}

const FlowNode *FlowTimelineCanvas::Node(int line) const
{
    for (const auto &node : model_.nodes) {
        if (node.line == line) {
            return &node;
        }
    }
    return nullptr;
}

void FlowTimelineCanvas::CancelDrag()
{
    dragging_ = panning_ = changed_ = false;
    dragLine_                       = -1;
    update();
}

void FlowTimelineCanvas::SetModel(const CompiledFlow &model, int duration)
{
    const bool FITTED = begin_ == 0 && span_ == end_;
    CancelDrag();
    model_    = model;
    duration_ = duration;
    end_      = qMax(16, duration);
    for (const auto &node : model.nodes) {
        end_ = qMax(end_, node.frame);
    }
    if (FITTED) {
        FitAll();
    } else {
        span_  = qMin(span_, end_);
        begin_ = std::clamp(begin_, 0, end_ - span_);
    }
    update();
}

void FlowTimelineCanvas::SetReadOnly(bool value)
{
    readOnly_ = value;
    CancelDrag();
}

void FlowTimelineCanvas::SelectLine(int line)
{
    selection_ = line;
    if (const auto *node = Node(line)) {
        cursorFrame_ = node->frame;
        if (node->frame < begin_ || node->frame > begin_ + span_) {
            begin_ = std::clamp(node->frame - (span_ / 2), 0, end_ - span_);
        }
    }
    update();
}

void FlowTimelineCanvas::FitAll()
{
    begin_ = 0;
    span_  = end_;
    update();
}

void FlowTimelineCanvas::Zoom(double factor, int anchor)
{
    if (anchor < 0) {
        anchor = begin_ + (span_ / 2);
    }
    const double FRACTION = std::clamp((anchor - begin_) / double(span_), 0., 1.);
    span_                 = std::clamp(qRound(span_ / factor), qMin(16, end_), end_);
    begin_                = std::clamp(qRound(anchor - (span_ * FRACTION)), 0, end_ - span_);
    update();
}

void FlowTimelineCanvas::Pan(int delta)
{
    begin_ = std::clamp(begin_ + delta, 0, end_ - span_);
    update();
}

QVector<FlowTimelineCanvas::Marker> FlowTimelineCanvas::Markers() const
{
    QVector<Marker> result;
    const auto      plot = PlotRect();
    for (const auto &node : model_.nodes) {
        const int FRAME = dragging_ && node.line == dragLine_ ? previewFrame_ : node.frame;
        if (FRAME < begin_ || FRAME > begin_ + span_) {
            continue;
        }
        QSet<int> used;
        for (int field = 0; std::cmp_less(field, FlowFields.size()); ++field) {
            if (!node.values.contains(QString::fromLatin1(FlowFields[field].key)) && (!node.values.isEmpty() || field != 0))
            {
                continue;
            }
            const int TRACK = Track(field);
            if (used.contains(TRACK)) {
                continue;
            }
            used.insert(TRACK);
            result.append({
                node.line,
                field,
                FRAME,
                { XAt(FRAME), plot.top() + (((2 * TRACK) + 1) * plot.height() / (2 * (compact_ ? 3 : int(FlowFields.size())))) }
            });
        }
    }
    return result;
}

QPoint FlowTimelineCanvas::NodePoint(int line, int field) const
{
    for (const auto &marker : Markers()) {
        if (marker.line == line && (field < 0 || marker.field == field)) {
            return marker.point;
        }
    }
    return { -1, -1 };
}

int FlowTimelineCanvas::Hit(QPoint point) const
{
    int line = -1;
    for (const auto &marker : Markers()) {
        if (QRect(marker.point - QPoint(8, 10), QSize(17, 21)).contains(point)) {
            if (marker.line == selection_) {
                return selection_;
            }
            line = marker.line;
        }
    }
    return line;
}

void FlowTimelineCanvas::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    p.fillRect(rect(), QColor(theme::Surface));
    auto font = this->font();
    font.setPixelSize(11);
    p.setFont(font);
    const auto plot   = PlotRect();
    const int  TRACKS = compact_ ? 3 : int(FlowFields.size());
    const auto accent = theme::AccentColor(this);
    p.fillRect(QRect(0, 0, width(), 24), QColor(theme::Raised));
    p.setPen(QColor(theme::Muted));
    p.drawText(QRect(4, 0, plot.x() - 4, 24), Qt::AlignVCenter, QStringLiteral("时间"));
    for (int row = 0; row < TRACKS; ++row) {
        const int TOP    = plot.y() + (row * plot.height() / TRACKS);
        const int BOTTOM = plot.y() + ((row + 1) * plot.height() / TRACKS);
        if (row % 2 == 0) {
            p.fillRect(QRect(0, TOP, width(), BOTTOM - TOP), QColor("#252529"));
        }
        p.setPen(QColor(theme::Border));
        p.drawLine(0, BOTTOM, width(), BOTTOM);
        p.setPen(QColor(theme::Muted));
        const auto LABEL =
            compact_ ? (row == 0   ? QStringLiteral("开场")
                        : row == 1 ? QStringLiteral("气血")
                                   : QStringLiteral("距离"))
                     : FlowFieldName(row);
        p.drawText(QRect(4, TOP, plot.x() - 10, BOTTOM - TOP), Qt::AlignVCenter, LABEL);
    }
    if (duration_ < begin_ + span_) {
        const int start = std::clamp(XAt(duration_), plot.left(), plot.right());
        p.fillRect(QRect(start, plot.top(), plot.right() - start + 1, plot.height()), QBrush(QColor("#35353d"), Qt::BDiagPattern));
    }
    constexpr double STEPS[]{ .0625, .125, .25, .5,  1,    2,    5,    10,    15,    30,
                              60,    120,  300, 600, 1800, 3600, 7200, 14400, 28800, 86400 };
    int              step   = 16;
    const double     NEEDED = span_ / 16. / qMax(2, plot.width() / 85);
    for (const double SECONDS : STEPS) {
        step = qRound(SECONDS * JX3DPS::FRAMES_PER_SECOND);
        if (SECONDS >= NEEDED) {
            break;
        }
    }
    for (int frame = ((begin_ + step - 1) / step) * step; frame <= begin_ + span_; frame += step) {
        const int X = XAt(frame);
        p.setPen(QPen(QColor(theme::Border), 1, Qt::DashLine));
        p.drawLine(X, plot.top(), X, plot.bottom());
        p.setPen(QColor(theme::Muted));
        const int left = std::clamp(X - 40, plot.left(), qMax(plot.left(), plot.right() - 79));
        p.drawText(QRect(left, 0, 80, 24), X == plot.left() ? Qt::AlignLeft | Qt::AlignVCenter : Qt::AlignCenter, FlowTimeText(frame));
    }
    if (!model_.errors.isEmpty()) {
        p.fillRect(plot, QColor(theme::Surface));
        p.setPen(QColor(theme::Error));
        p.drawText(plot.adjusted(6, 0, -6, 0), Qt::AlignCenter | Qt::TextWordWrap, QStringLiteral("脚本有误\n请切换脚本修正后编辑"));
        return;
    }
    QMap<QPair<int, int>, QVector<Marker>> groups;
    for (const auto &marker : Markers()) {
        groups[{ Track(marker.field), marker.point.x() }].append(marker);
    }
    for (auto it = groups.begin(); it != groups.end(); ++it) {
        const auto &group  = it.value();
        auto        marker = group.back();
        for (const auto &candidate : group) {
            if (candidate.line == selection_) {
                marker = candidate;
            }
        }
        const auto *node = Node(marker.line);
        if (node == nullptr) {
            continue;
        }
        const bool ACTIVE = marker.frame < duration_;
        const auto INK    = ACTIVE ? accent : QColor(theme::Muted);
        auto       fill   = INK;
        fill.setAlpha(marker.line == selection_ ? 200 : 70);
        p.setPen(QPen(INK, marker.line == selection_ ? 2 : 1));
        p.setBrush(fill);
        QPainterPath diamond;
        diamond.moveTo(marker.point + QPoint(0, -6));
        diamond.lineTo(marker.point + QPoint(6, 0));
        diamond.lineTo(marker.point + QPoint(0, 6));
        diamond.lineTo(marker.point + QPoint(-6, 0));
        diamond.closeSubpath();
        p.drawPath(diamond);
        const auto next = std::next(it);
        const int right = next != groups.end() && next.key().first == it.key().first ? next.key().second - 10 : plot.right();
        const QRect LABEL(marker.point.x() + 9, marker.point.y() - 10, qMax(0, right - marker.point.x() - 9), 20);
        const auto  text =
            group.size() > 1 ? QStringLiteral("×%1").arg(group.size())
            : node->values.isEmpty()
                ? QStringLiteral("目标 1")
                : FlowValueText(FlowFields[marker.field],
                                node->values.value(QString::fromLatin1(FlowFields[marker.field].key)).toDouble());
        p.setPen(QColor(ACTIVE ? theme::Text : theme::Muted));
        if (LABEL.width() >= 16) {
            p.drawText(LABEL, Qt::AlignVCenter, p.fontMetrics().elidedText(text, Qt::ElideRight, LABEL.width()));
        }
    }
    if (dragging_ || (cursorFrame_ >= begin_ && cursorFrame_ <= begin_ + span_)) {
        const int X   = XAt(dragging_ ? previewFrame_ : cursorFrame_);
        auto      pen = accent;
        pen.setAlpha(100);
        p.setPen(QPen(pen, 1, Qt::DashLine));
        p.drawLine(X, plot.top(), X, plot.bottom());
        if (dragging_ && !compact_) {
            const auto  text  = FlowTimeText(previewFrame_);
            const int   WIDTH = p.fontMetrics().horizontalAdvance(text) + 12;
            const int   left  = std::clamp(X - (WIDTH / 2), plot.left(), qMax(plot.left(), plot.right() - WIDTH));
            const QRect BADGE(left, 1, WIDTH, 22);
            p.setPen(Qt::NoPen);
            p.setBrush(QColor(theme::Raised));
            p.drawRect(BADGE);
            p.setPen(accent);
            p.drawText(BADGE, Qt::AlignCenter, text);
        }
    }
    if (readOnly_) {
        p.setPen(QColor(theme::Muted));
        p.drawText(rect().adjusted(0, 0, -4, 0), Qt::AlignRight | Qt::AlignTop, QStringLiteral("只读"));
    }
}

void FlowTimelineCanvas::mousePressEvent(QMouseEvent *event)
{
    if (event->button() != Qt::LeftButton || !PlotRect().adjusted(-8, 0, 8, 0).contains(event->position().toPoint())) {
        return;
    }
    setFocus(Qt::MouseFocusReason);
    pressed_       = event->position().toPoint();
    changed_       = false;
    const int LINE = Hit(pressed_);
    if (LINE >= 0) {
        SelectLine(LINE);
        if (selected) {
            selected(LINE);
        }
        const auto *node = Node(LINE);
        if (!readOnly_ && (node != nullptr) && node->CanMove()) {
            dragging_     = true;
            dragLine_     = LINE;
            previewFrame_ = node->frame;
        }
    } else {
        cursorFrame_ = FrameAt(pressed_.x());
        panning_     = true;
        panBegin_    = begin_;
    }
    update();
    event->accept();
}

void FlowTimelineCanvas::mouseMoveEvent(QMouseEvent *event)
{
    const auto point = event->position().toPoint();
    if (dragging_ || panning_) {
        changed_ |= (point - pressed_).manhattanLength() >= QApplication::startDragDistance();
        if (changed_) {
            if (dragging_) {
                previewFrame_ = FrameAt(point.x());
            } else {
                begin_ = panBegin_;
                Pan(qRound((pressed_.x() - point.x()) * double(span_) / PlotRect().width()));
            }
            update();
        }
        return;
    }
    const auto *node = Node(Hit(point));
    setCursor((node != nullptr) && node->CanMove() && !readOnly_ ? Qt::SizeHorCursor
              : (node != nullptr)                                ? Qt::PointingHandCursor
                                                                 : Qt::CrossCursor);
    if (node != nullptr) {
        QToolTip::showText(
            event->globalPosition().toPoint(),
            QStringLiteral("%1 · 第 %2 行\n%3\n%4%5")
                .arg(FlowTimeText(node->frame))
                .arg(node->line)
                .arg(node->Summary())
                .arg(node->CanMove() ? QStringLiteral("拖动调整时间；双击编辑") : QStringLiteral("仅支持开场；双击编辑参数"))
                .arg(node->frame >= duration_ ? QStringLiteral("\n位于战斗结束处或之后，本次不会执行") : QString{}),
            this);
    }
}

void FlowTimelineCanvas::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() != Qt::LeftButton) {
        return;
    }
    const int   LINE   = dragLine_;
    const int   FRAME  = previewFrame_;
    const auto *node   = Node(LINE);
    const bool  commit = dragging_ && changed_ && (node != nullptr) && node->frame != FRAME && !readOnly_;
    CancelDrag();
    if (commit && moved) {
        moved(LINE, FRAME);
    }
}

void FlowTimelineCanvas::mouseDoubleClickEvent(QMouseEvent *event)
{
    CancelDrag();
    if (event->button() != Qt::LeftButton || !model_.errors.isEmpty()) {
        return;
    }
    const auto point = event->position().toPoint();
    const int  LINE  = Hit(point);
    if (compact_ && LINE >= 0 && Track(FieldAt(point.y())) == 0 && expand) {
        expand();
        return;
    }
    if (readOnly_) {
        return;
    }
    if (LINE >= 0) {
        if (edited) {
            edited(LINE);
        }
    } else if (PlotRect().contains(point) && added) {
        added(FrameAt(point.x()), FieldAt(point.y()));
    }
}

void FlowTimelineCanvas::wheelEvent(QWheelEvent *event)
{
    if (event->modifiers() & Qt::ShiftModifier != 0u) {
        Pan(event->angleDelta().y() > 0 ? -span_ / 8 : span_ / 8);
    } else {
        Zoom(event->angleDelta().y() > 0 ? 1.4 : 1 / 1.4, FrameAt(qRound(event->position().x())));
    }
    event->accept();
}

void FlowTimelineCanvas::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape) {
        CancelDrag();
        return;
    }
    if (!readOnly_ && event->matches(QKeySequence::Undo)) {
        if (undo) {
            undo();
        }
        return;
    }
    if (!readOnly_ && event->matches(QKeySequence::Redo)) {
        if (redo) {
            redo();
        }
        return;
    }
    const auto *node = Node(selection_);
    if ((node != nullptr) && !readOnly_) {
        if (event->key() == Qt::Key_Delete) {
            if (removed) {
                removed(selection_);
            }
            return;
        }
        if (event->key() == Qt::Key_Return) {
            if (edited) {
                edited(selection_);
            }
            return;
        }
        if (node->CanMove() && (event->key() == Qt::Key_Left || event->key() == Qt::Key_Right)) {
            const int STEP = (event->modifiers() & Qt::ShiftModifier != 0u) ? 16 : 1;
            if (moved) {
                moved(selection_,
                      std::clamp(node->frame + (event->key() == Qt::Key_Left ? -STEP : STEP), 0, int(JX3DPS::runtime::MaximumFlowFrame)));
            }
            return;
        }
    }
    QWidget::keyPressEvent(event);
}

struct FlowTimelineEditor::Impl
{
    FlowTimelineEditor *host;
    CodeEditor         *script;
    SegmentedTabBar    *tabs;
    QStackedWidget     *views;
    FlowTimelineCanvas *preview, *canvas = nullptr;
    QDialog            *expanded = nullptr, *nodeDialog = nullptr;
    QTableWidget       *table = nullptr;
    QToolButton *addButton = nullptr, *editButton = nullptr, *deleteButton = nullptr, *undoButton = nullptr, *redoButton = nullptr;
    QLabel                    *notice = nullptr, *nodeHint = nullptr;
    QComboBox                 *kind        = nullptr;
    FixedUnitDoubleSpinBox    *time        = nullptr;
    QDialogButtonBox          *nodeButtons = nullptr;
    std::array<QCheckBox *, FlowFields.size()> checks{};
    std::array<QWidget *, FlowFields.size()>   inputs{};
    CompiledFlow               model;
    QString                    source, draftSource;
    FlowNode                   draft;
    bool                       initialized = false, readOnly = false, populating = false;
    int                        duration = 4800, selectedLine = -1;

    Impl(FlowTimelineEditor *owner, CodeEditor *editor) : host(owner), script(editor)
    {
        auto *layout = new QVBoxLayout(host);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->setSpacing(4);
        tabs = new SegmentedTabBar(host);
        tabs->setObjectName(QStringLiteral("flowViewMode"));
        tabs->SetSegmentHeight(26);
        tabs->addTab(QStringLiteral("脚本"));
        tabs->addTab(QStringLiteral("时间轴"));
        tabs->setUsesScrollButtons(false);
        tabs->setExpanding(false);
        layout->addWidget(tabs, 0, Qt::AlignLeft);
        views = new QStackedWidget(host);
        views->addWidget(script);
        preview = new FlowTimelineCanvas(true, views);
        preview->setObjectName(QStringLiteral("flowTimelinePreview"));
        views->addWidget(preview);
        layout->addWidget(views, 1);
        Bind(preview);
        tabs->setCurrentIndex(QSettings().value(QStringLiteral("interface/flowView"), 1).toInt() == 0 ? 0 : 1);
        views->setCurrentIndex(tabs->currentIndex());
        QObject::connect(tabs, &QTabBar::currentChanged, host, [this](int index) {
            views->setCurrentIndex(index);
            QSettings().setValue(QStringLiteral("interface/flowView"), index);
        });
        QObject::connect(script, &QPlainTextEdit::textChanged, host, [this] { Sync(); });
        QObject::connect(script->document(), &QTextDocument::undoAvailable, host, [this] { Buttons(); });
        QObject::connect(script->document(), &QTextDocument::redoAvailable, host, [this] { Buttons(); });
        Sync();
    }

    bool Editable() const { return !readOnly && !script->isReadOnly() && model.errors.isEmpty(); }

    const FlowNode *Node(int line) const
    {
        for (const auto &node : model.nodes) {
            if (node.line == line) {
                return &node;
            }
        }
        return nullptr;
    }

    void Bind(FlowTimelineCanvas *view)
    {
        view->selected = [this](int line) {
            Select(line);
        };
        view->moved = [this](int line, int frame) {
            host->MoveNode(line, frame);
        };
        view->edited = [this](int line) {
            host->EditNode(line);
        };
        view->removed = [this](int line) {
            host->RemoveNode(line);
        };
        view->added = [this](int frame, int field) {
            host->AddNode(frame, field);
        };
        view->expand = [this] {
            host->OpenEditor();
        };
        view->undo = [this] {
            if (!readOnly && !script->isReadOnly()) {
                script->undo();
            }
        };
        view->redo = [this] {
            if (!readOnly && !script->isReadOnly()) {
                script->redo();
            }
        };
    }

    void Report(const QString &text, bool error = false)
    {
        if (notice != nullptr) {
            notice->setText(text);
            notice->setToolTip(text);
        }
        if (host->statusChanged) {
            host->statusChanged(text, error);
        }
    }

    void Summary()
    {
        if (!model.errors.isEmpty()) {
            Report(model.ErrorText(), true);
            return;
        }
        int later   = 0;
        int outside = 0;
        for (const auto &node : std::as_const(model.nodes)) {
            later   += static_cast<int>(node.frame > 0);
            outside += static_cast<int>(node.frame > 0 && node.frame >= duration);
        }
        auto text = QStringLiteral("%1 个开场 · %2 个战中节点").arg(model.nodes.size() - later).arg(later);
        if (outside != 0) {
            text += QStringLiteral(" · %1 个超出本次时长").arg(outside);
        }
        if (readOnly) {
            text += QStringLiteral(" · 只读");
        }
        Report(text);
    }

    void Buttons()
    {
        if (addButton == nullptr) {
            return;
        }
        addButton->setEnabled(Editable() && model.nodes.size() < int(JX3DPS::runtime::MaximumTargetEvents));
        editButton->setEnabled(Editable() && (Node(selectedLine) != nullptr));
        deleteButton->setEnabled(Editable() && (Node(selectedLine) != nullptr));
        undoButton->setEnabled(!readOnly && !script->isReadOnly() && script->document()->isUndoAvailable());
        redoButton->setEnabled(!readOnly && !script->isReadOnly() && script->document()->isRedoAvailable());
    }

    void Select(int line)
    {
        selectedLine = (Node(line) != nullptr) ? line : -1;
        preview->SelectLine(selectedLine);
        if (canvas != nullptr) {
            canvas->SelectLine(selectedLine);
        }
        if (table != nullptr) {
            const QSignalBlocker BLOCKER(table);
            table->clearSelection();
            for (int row = 0; row < table->rowCount(); ++row) {
                if (table->item(row, 0)->data(Qt::UserRole).toInt() == selectedLine) {
                    table->selectRow(row);
                }
            }
        }
        Buttons();
    }

    void Sync()
    {
        const auto text = script->toPlainText();
        if (initialized && text == source) {
            return;
        }
        initialized = true;
        source      = text;
        model       = CompileFlowMacro(source);
        preview->SetModel(model, duration);
        if (canvas != nullptr) {
            canvas->SetModel(model, duration);
        }
        if (table != nullptr) {
            const QSignalBlocker BLOCKER(table);
            table->setRowCount(0);
            auto nodes = model.nodes;
            std::stable_sort(nodes.begin(), nodes.end(), [](const auto &a, const auto &b) { return a.frame < b.frame; });
            if (model.errors.isEmpty()) {
                for (const auto &node : nodes) {
                    const int ROW = table->rowCount();
                    table->insertRow(ROW);
                    const QStringList COLUMNS{ FlowTimeText(node.frame),
                                               node.command == "/set_target" ? QStringLiteral("目标") : QStringLiteral("角色"),
                                               node.Summary(),
                                               QString::number(node.line) };
                    for (int column = 0; column < COLUMNS.size(); ++column) {
                        auto *item = new QTableWidgetItem(COLUMNS[column]);
                        item->setData(Qt::UserRole, node.line);
                        item->setToolTip(node.original);
                        if (node.frame >= duration && node.frame > 0) {
                            item->setForeground(QColor(theme::Muted));
                        }
                        table->setItem(ROW, column, item);
                    }
                }
            }
        }
        Select(selectedLine);
        Summary();
    }

    QToolButton *Button(QHBoxLayout *layout, const QString &text, const char *name, std::function<void()> action)
    {
        auto *button = new QToolButton;
        button->setText(text);
        button->setObjectName(QString::fromLatin1(name));
        layout->addWidget(button);
        QObject::connect(button, &QToolButton::clicked, host, std::move(action));
        return button;
    }

    void Open()
    {
        if (expanded == nullptr) {
            expanded = new QDialog(host);
            expanded->setObjectName(QStringLiteral("flowTimelineDialog"));
            expanded->setWindowTitle(QStringLiteral("流程宏 · 时间轴编辑"));
            expanded->resize(980, 640);
            expanded->setMinimumSize(780, 540);
            auto *layout = new QVBoxLayout(expanded);
            auto *tools  = new QHBoxLayout;
            addButton =
                Button(tools, QStringLiteral("添加节点"), "addFlowNode", [this] { host->AddNode(canvas->CursorFrame()); });
            editButton = Button(tools, QStringLiteral("编辑"), "editFlowNode", [this] { host->EditNode(selectedLine); });
            deleteButton =
                Button(tools, QStringLiteral("删除"), "deleteFlowNode", [this] { host->RemoveNode(selectedLine); });
            tools->addSpacing(14);
            undoButton = Button(tools, QStringLiteral("撤销"), "undoFlowEdit", [this] { script->undo(); });
            redoButton = Button(tools, QStringLiteral("重做"), "redoFlowEdit", [this] { script->redo(); });
            tools->addStretch();
            Button(tools, QStringLiteral("−"), "zoomOutFlow", [this] { canvas->Zoom(1 / 1.5); });
            Button(tools, QStringLiteral("+"), "zoomInFlow", [this] { canvas->Zoom(1.5); });
            Button(tools, QStringLiteral("适应"), "fitFlowTimeline", [this] { canvas->FitAll(); });
            layout->addLayout(tools);
            canvas = new FlowTimelineCanvas(false, expanded);
            canvas->setObjectName(QStringLiteral("flowTimelineCanvas"));
            Bind(canvas);
            canvas->SetModel(model, duration);
            canvas->SetReadOnly(readOnly);
            layout->addWidget(canvas, 1);
            auto *hint =
                new QLabel(QStringLiteral("双击空白添加 / 节点编辑 · 拖动节点调整时间 · 滚轮缩放，拖动空白平移 · ←/→ "
                                          "一帧，Shift 加速 · 战中可设置气血、受击和距离"));
            hint->setWordWrap(true);
            hint->setProperty("muted", true);
            layout->addWidget(hint);
            table = new QTableWidget(0, 4, expanded);
            table->setObjectName(QStringLiteral("flowEventTable"));
            table->setHorizontalHeaderLabels({ QStringLiteral("时间"), QStringLiteral("对象"), QStringLiteral("参数"), QStringLiteral("源码行") });
            table->setEditTriggers(QAbstractItemView::NoEditTriggers);
            table->setSelectionBehavior(QAbstractItemView::SelectRows);
            table->setSelectionMode(QAbstractItemView::SingleSelection);
            table->verticalHeader()->hide();
            table->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
            table->setColumnWidth(0, 130);
            table->setColumnWidth(1, 70);
            table->setColumnWidth(3, 70);
            table->setFixedHeight(160);
            layout->addWidget(table);
            QObject::connect(table, &QTableWidget::itemSelectionChanged, host, [this] {
                if (table->currentRow() >= 0 && table->item(table->currentRow(), 0)) {
                    Select(table->item(table->currentRow(), 0)->data(Qt::UserRole).toInt());
                }
            });
            QObject::connect(table, &QTableWidget::cellDoubleClicked, host, [this](int row, int) {
                host->EditNode(table->item(row, 0)->data(Qt::UserRole).toInt());
            });
            notice = new QLabel;
            notice->setObjectName(QStringLiteral("flowTimelineNotice"));
            notice->setTextFormat(Qt::PlainText);
            notice->setWordWrap(true);
            notice->setProperty("muted", true);
            layout->addWidget(notice);
            initialized = false;
            Sync();
        }
        expanded->show();
        expanded->raise();
        expanded->activateWindow();
        Select(selectedLine);
        Buttons();
        Summary();
    }

    double Value(int index) const
    {
        if (const auto *spin = qobject_cast<QSpinBox *>(inputs[index])) {
            return spin->value();
        }
        if (const auto *spin = qobject_cast<QDoubleSpinBox *>(inputs[index])) {
            return spin->value();
        }
        return qobject_cast<QComboBox *>(inputs[index])->currentIndex();
    }

    void SetValue(int index, double value)
    {
        if (auto *spin = qobject_cast<QSpinBox *>(inputs[index])) {
            spin->setValue(qRound(value));
        } else if (auto *spin = qobject_cast<QDoubleSpinBox *>(inputs[index])) {
            spin->setValue(value);
        } else {
            qobject_cast<QComboBox *>(inputs[index])->setCurrentIndex(qRound(value));
        }
    }

    void Availability()
    {
        if (populating) {
            return;
        }
        const auto COMMAND = kind->currentData().toString();
        bool       locked = false, combat_only = false;
        for (int i = 0; std::cmp_less(i, FlowFields.size()); ++i) {
            if (COMMAND == QLatin1String(FlowFields[i].command) && checks[i]->isChecked()) {
                locked |= !FlowFields[i].during_combat;
                combat_only |= FlowFields[i].combat_only;
            }
        }
        {
            const QSignalBlocker BLOCKER(time);
            time->setMinimum(combat_only ? 1. / 16 : 0.);
            if (locked) time->setValue(0);
        }
        time->setEnabled(!locked);
        for (int i = 0; std::cmp_less(i, FlowFields.size()); ++i) {
            const bool VISIBLE = COMMAND == QLatin1String(FlowFields[i].command);
            checks[i]->setVisible(VISIBLE);
            inputs[i]->setVisible(VISIBLE);
            checks[i]->setEnabled(VISIBLE && (time->value() == 0 || FlowFields[i].during_combat) && (!locked || !FlowFields[i].combat_only));
            inputs[i]->setEnabled(checks[i]->isEnabled() && checks[i]->isChecked());
        }
        nodeHint->setText(
            locked
                ? QStringLiteral("此节点包含开场参数，时间固定为 0 秒。")
                : QStringLiteral("时间吸附到 1/16 秒；在本次战斗结束时或之后的事件不会执行。气血/易伤按百分数填写。"));
    }

    void BuildNodeDialog()
    {
        nodeDialog = new QDialog(host);
        nodeDialog->setObjectName(QStringLiteral("flowNodeDialog"));
        nodeDialog->setWindowModality(Qt::WindowModal);
        nodeDialog->setMinimumWidth(430);
        auto *layout = new QVBoxLayout(nodeDialog);
        auto *form   = new QFormLayout;
        form->setVerticalSpacing(8);
        layout->addLayout(form);
        time = new FixedUnitDoubleSpinBox;
        time->setObjectName(QStringLiteral("flowNodeTime"));
        time->setProperty("flowViewOnly", true);
        time->setDecimals(4);
        time->setRange(0, 86400);
        time->setSingleStep(.0625);
        time->SetUnit(QStringLiteral("秒"));
        form->addRow(QStringLiteral("执行时间"), time);
        kind = new QComboBox;
        kind->setObjectName(QStringLiteral("flowNodeKind"));
        kind->setProperty("flowViewOnly", true);
        kind->addItem(QStringLiteral("目标参数"), "/set_target");
        kind->addItem(QStringLiteral("角色参数"), "/set_player");
        form->addRow(QStringLiteral("节点类型"), kind);
        for (int i = 0; std::cmp_less(i, FlowFields.size()); ++i) {
            const auto &field = FlowFields[i];
            checks[i]         = new QCheckBox(FlowFieldName(i));
            checks[i]->setObjectName("flowNodeEnabled_" + QString::fromLatin1(field.key));
            checks[i]->setProperty("flowViewOnly", true);
            if (field.decimals == 0) {
                auto *input = new QSpinBox;
                input->setRange(qRound(field.minimum), qRound(field.maximum));
                inputs[i] = input;
            } else {
                auto *input = new FixedUnitDoubleSpinBox;
                input->setDecimals(field.decimals);
                input->setRange(field.minimum, field.maximum);
                if (field.percentage) {
                    input->SetUnit(QStringLiteral("%"));
                }
                inputs[i] = input;
            }
            inputs[i]->setObjectName("flowNodeValue_" + QString::fromLatin1(field.key));
            inputs[i]->setProperty("flowViewOnly", true);
            form->addRow(checks[i], inputs[i]);
            QObject::connect(checks[i], &QCheckBox::toggled, host, [this] { Availability(); });
        }
        QObject::connect(time, &QDoubleSpinBox::valueChanged, host, [this] { Availability(); });
        QObject::connect(time, &QDoubleSpinBox::editingFinished, host, [this] {
            time->setValue(qRound(time->value() * 16) / 16.);
        });
        QObject::connect(kind, &QComboBox::currentIndexChanged, host, [this] {
            if (populating) {
                return;
            }
            populating = true;
            for (int i = 0; std::cmp_less(i, FlowFields.size()); ++i) {
                checks[i]->setChecked(i == (kind->currentIndex() == 0 ? 2 : time->value() > 0 ? 6 : 5));
            }
            populating = false;
            Availability();
        });
        nodeHint = new QLabel;
        nodeHint->setObjectName(QStringLiteral("flowNodeHint"));
        nodeHint->setWordWrap(true);
        nodeHint->setMinimumHeight(44);
        nodeHint->setTextFormat(Qt::PlainText);
        layout->addWidget(nodeHint);
        nodeButtons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
        nodeButtons->button(QDialogButtonBox::Ok)->setText(QStringLiteral("应用"));
        nodeButtons->button(QDialogButtonBox::Ok)->setObjectName(QStringLiteral("applyFlowNode"));
        nodeButtons->button(QDialogButtonBox::Cancel)->setText(QStringLiteral("取消"));
        layout->addWidget(nodeButtons);
        QObject::connect(nodeButtons, &QDialogButtonBox::rejected, nodeDialog, &QDialog::reject);
        QObject::connect(nodeButtons, &QDialogButtonBox::accepted, host, [this] { ApplyDraft(); });
    }

    void Edit(const FlowNode &node)
    {
        if (!Editable()) {
            return;
        }
        if (nodeDialog == nullptr) {
            BuildNodeDialog();
        }
        if (nodeDialog->isVisible()) {
            nodeDialog->raise();
            return;
        }
        auto *owner = (expanded != nullptr) && expanded->isVisible() ? static_cast<QWidget *>(expanded) : host;
        if (nodeDialog->parentWidget() != owner) {
            nodeDialog->setParent(owner, Qt::Dialog);
        }
        nodeDialog->setWindowModality(Qt::WindowModal);
        draft       = node;
        draftSource = script->toPlainText();
        populating  = true;
        kind->setCurrentIndex(node.command == "/set_player" ? 1 : 0);
        time->setMinimum(0);
        time->setValue(node.frame / 16.);
        for (int i = 0; std::cmp_less(i, FlowFields.size()); ++i) {
            const auto KEY = QString::fromLatin1(FlowFields[i].key);
            checks[i]->setChecked(node.values.contains(KEY));
            SetValue(i, node.values.value(KEY).toDouble(model.values.value(KEY).toDouble()));
        }
        populating = false;
        Availability();
        nodeButtons->button(QDialogButtonBox::Ok)->setEnabled(true);
        nodeDialog->setWindowTitle(node.line > 0 ? QStringLiteral("编辑节点 · 第 %1 行").arg(node.line) : QStringLiteral("添加流程节点"));
        nodeDialog->adjustSize();
        nodeDialog->open();
    }

    bool Replace(int line, const QString &replacement, bool remove = false)
    {
        if (!Editable()) {
            return false;
        }
        auto lines = script->toPlainText().split('\n');
        if (line > lines.size() || line < 0) {
            return false;
        }
        if (line == 0) {
            if (lines.size() == 1 && lines[0].isEmpty()) {
                lines[0] = replacement;
            } else if (lines.back().isEmpty()) {
                lines.back() = replacement;
            } else {
                lines.append(replacement);
            }
        } else if (remove) {
            lines.removeAt(line - 1);
        } else {
            lines[line - 1] = replacement;
        }
        const auto PROPOSED = CompileFlowMacro(lines.join('\n'));
        if (!PROPOSED.errors.isEmpty()) {
            Report(PROPOSED.ErrorText(), true);
            return false;
        }
        QTextCursor cursor(script->document());
        cursor.beginEditBlock();
        int selected = line;
        if (line == 0) {
            cursor.movePosition(QTextCursor::End);
            if (!cursor.block().text().isEmpty()) {
                cursor.insertBlock();
            }
            selected = cursor.blockNumber() + 1;
            cursor.insertText(replacement);
        } else {
            const auto BLOCK = script->document()->findBlockByNumber(line - 1);
            if (remove) {
                int from = BLOCK.position();
                int to   = BLOCK.position() + BLOCK.length();
                if (!BLOCK.next().isValid()) {
                    to = script->document()->characterCount() - 1;
                    if (from > 0) {
                        --from;
                    }
                }
                cursor.setPosition(from);
                cursor.setPosition(to, QTextCursor::KeepAnchor);
                cursor.removeSelectedText();
                selected = -1;
            } else {
                cursor.setPosition(BLOCK.position());
                cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
                cursor.insertText(replacement);
            }
        }
        cursor.endEditBlock();
        Select(selected);
        return true;
    }

    void ApplyDraft()
    {
        if (!Editable()) {
            return;
        }
        if (script->toPlainText() != draftSource) {
            nodeHint->setText(QStringLiteral("脚本已在其他视图更改。请关闭并重新选择节点，当前修改未写入。"));
            nodeButtons->button(QDialogButtonBox::Ok)->setEnabled(false);
            return;
        }
        const auto  COMMAND = kind->currentData().toString();
        QJsonObject values;
        for (int i = 0; std::cmp_less(i, FlowFields.size()); ++i) {
            if (checks[i]->isChecked() && COMMAND == QLatin1String(FlowFields[i].command)) {
                values.insert(QString::fromLatin1(FlowFields[i].key), Value(i));
            }
        }
        if (values.isEmpty()) {
            nodeHint->setText(QStringLiteral("请至少勾选一项要设置的参数。"));
            return;
        }
        const auto replacement = draft.WithValues(qRound(time->value() * 16), COMMAND, values);
        if (Replace(draft.line, replacement)) {
            nodeDialog->accept();
        } else {
            nodeHint->setText(QStringLiteral("节点无法写入，请检查脚本诊断和节点数量限制。"));
        }
    }
};

FlowTimelineEditor::FlowTimelineEditor(CodeEditor *script, QWidget *parent) :
    QWidget(parent), d(std::make_unique<Impl>(this, script))
{
    setObjectName(QStringLiteral("flowTimelineEditor"));
}

FlowTimelineEditor::~FlowTimelineEditor()
{
    // Editor/highlighter teardown can emit signals after Impl has been destroyed.
    for (const auto *object : findChildren<QObject *>()) {
        QObject::disconnect(object, nullptr, this, nullptr);
    }
}

const CompiledFlow &FlowTimelineEditor::Model() const
{
    return d->model;
}

void FlowTimelineEditor::SetDuration(int frames)
{
    d->duration = qMax(1, frames);
    d->preview->SetModel(d->model, d->duration);
    if (d->canvas != nullptr) {
        d->canvas->SetModel(d->model, d->duration);
    }
    if (d->table != nullptr) {
        for (int row = 0; row < d->table->rowCount(); ++row) {
            const auto *node = d->Node(d->table->item(row, 0)->data(Qt::UserRole).toInt());
            const auto  COLOR =
                QColor((node != nullptr) && node->frame >= d->duration && node->frame > 0 ? theme::Muted : theme::Text);
            for (int column = 0; column < d->table->columnCount(); ++column) {
                d->table->item(row, column)->setForeground(COLOR);
            }
        }
    }
    d->Summary();
}

void FlowTimelineEditor::SetReadOnly(bool read_only)
{
    d->readOnly = read_only;
    d->script->setReadOnly(read_only);
    d->preview->SetReadOnly(read_only);
    if (d->canvas != nullptr) {
        d->canvas->SetReadOnly(read_only);
    }
    if (read_only && (d->nodeDialog != nullptr)) {
        d->nodeDialog->reject();
    }
    d->Buttons();
    d->Summary();
}

void FlowTimelineEditor::ShowScript()
{
    d->tabs->setCurrentIndex(0);
}

void FlowTimelineEditor::OpenEditor()
{
    d->Open();
}

void FlowTimelineEditor::AddNode(int frame, int field)
{
    if (field < 0 || std::cmp_greater_equal(field, FlowFields.size())) {
        field = 2;
    }
    FlowNode node;
    node.command      = QString::fromLatin1(FlowFields[field].command);
    node.frame        = FlowFields[field].during_combat ? std::clamp(frame, int(FlowFields[field].combat_only), int(JX3DPS::runtime::MaximumFlowFrame)) : 0;
    const auto KEY    = QString::fromLatin1(FlowFields[field].key);
    auto       value  = d->model.values.value(KEY).toDouble(FlowFields[field].initial);
    int        latest = 0;
    for (const auto &existing : std::as_const(d->model.nodes)) {
        if (existing.frame > 0 && existing.frame <= node.frame && existing.frame >= latest && existing.values.contains(KEY))
        {
            value  = existing.values.value(KEY).toDouble();
            latest = existing.frame;
        }
    }
    node.values.insert(KEY, value);
    d->Edit(node);
}

void FlowTimelineEditor::EditNode(int line)
{
    if (const auto *node = d->Node(line)) {
        d->Edit(*node);
    }
}

bool FlowTimelineEditor::MoveNode(int line, int frame)
{
    const auto *node = d->Node(line);
    if ((node == nullptr) || !d->Editable() || !node->CanMove() || frame < 0 || frame > JX3DPS::runtime::MaximumFlowFrame)
    {
        return false;
    }
    if (frame == 0 && node->values.contains("incomingDamage")) frame = 1;
    return node->frame == frame || d->Replace(line, node->WithTime(frame));
}

bool FlowTimelineEditor::RemoveNode(int line)
{
    return (d->Node(line) != nullptr) && d->Replace(line, {}, true);
}
} // namespace desktop
