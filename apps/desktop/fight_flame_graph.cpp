#include "fight_flame_graph.h"

#include <algorithm>

#include <QLabel>
#include <QMouseEvent>
#include <QPainter>
#include <QPushButton>
#include <QScrollArea>
#include <QVBoxLayout>

namespace {
constexpr int RowHeight = 26;
constexpr int Margin    = 6;
} // namespace

FightFlameGraph::FightFlameGraph(QWidget *parent) : QWidget(parent)
{
    auto *layout     = new QVBoxLayout(this);
    auto *navigation = new QHBoxLayout;
    up_              = new QPushButton(tr("上一级"), this);
    reset_           = new QPushButton(tr("全图"), this);
    reset_->setObjectName("flameReset");
    path_ = new QLabel(this);
    path_->setWordWrap(true);
    navigation->addWidget(up_);
    navigation->addWidget(reset_);
    navigation->addWidget(path_, 1);
    layout->addLayout(navigation);
    auto *scroll = new QScrollArea(this);
    scroll->setFrameShape(QFrame::NoFrame);
    scroll->setWidgetResizable(true);
    canvas_ = new QWidget;
    canvas_->setObjectName("fightFlameCanvas");
    canvas_->setAccessibleName(tr("已标记函数的采样调用栈"));
    canvas_->setMouseTracking(true);
    canvas_->installEventFilter(this);
    scroll->setWidget(canvas_);
    layout->addWidget(scroll, 1);
    detail_ = new QLabel(this);
    detail_->setObjectName("flameDetails");
    detail_->setWordWrap(true);
    layout->addWidget(detail_);
    auto *hint = new QLabel(
        tr("从下往上读调用栈。宽度表示含子调用的耗时；悬停查看明细，点击色块放大。自身耗时包含未单独标记的调用。"),
        this);
    hint->setWordWrap(true);
    layout->addWidget(hint);
    connect(up_, &QPushButton::clicked, this, [this] { Zoom(report_.calls[focus_].parent); });
    connect(reset_, &QPushButton::clicked, this, [this] { Zoom(0); });
    up_->setEnabled(false);
    reset_->setEnabled(false);
}

void FightFlameGraph::SetReport(const FightHotspots &report, quint64 run_id)
{
    if (run_id == runId_) {
        return;
    }
    runId_    = run_id;
    report_   = report;
    children_ = QVector<QVector<int>>(report_.calls.size());
    for (int i = 1; i < report_.calls.size(); ++i) {
        if (report_.calls[i].inclusive != 0u) {
            children_[report_.calls[i].parent].push_back(i);
        }
    }
    for (auto &children : children_) {
        std::sort(children.begin(), children.end(), [this](int a, int b) {
            return report_.calls[a].function < report_.calls[b].function;
        });
    }
    Zoom(0);
}

void FightFlameGraph::Zoom(int node)
{
    focus_  = node;
    levels_ = 0;
    blocks_.clear();
    up_->setEnabled(node != 0);
    reset_->setEnabled(node != 0);
    if (report_.calls.isEmpty() || (report_.samplePoints == 0u)) {
        path_->clear();
        detail_->clear();
        canvas_->update();
        return;
    }
    QStringList path;
    for (int i = node;; i = report_.calls[i].parent) {
        path.prepend(report_.calls[i].function);
        if (i == 0) {
            break;
        }
    }
    path_->setText(path.join(QStringLiteral("  ›  ")));
    const auto TOTAL  = report_.calls[node].inclusive;
    auto       append = [&](auto &self, int id, int depth, double left) -> void {
        blocks_.push_back({ id, left, double(report_.calls[id].inclusive) / TOTAL, depth });
        levels_ = std::max(levels_, depth + 1);
        for (const int CHILD : children_[id]) {
            self(self, CHILD, depth + 1, left);
            left += double(report_.calls[CHILD].inclusive) / TOTAL;
        }
    };
    append(append, node, 0, 0.0);
    canvas_->setMinimumHeight((2 * Margin) + (levels_ * RowHeight));
    Inspect(node);
    canvas_->update();
}

QRectF FightFlameGraph::Box(const Block &block) const
{
    const int WIDTH = std::max(0, canvas_->width() - (2 * Margin));
    return { Margin + (block.left * WIDTH), double(Margin + ((levels_ - block.depth - 1) * RowHeight)), block.width * WIDTH, double(RowHeight) };
}

int FightFlameGraph::Hit(QPointF point) const
{
    for (const auto &block : blocks_) {
        if (Box(block).contains(point)) {
            return block.node;
        }
    }
    return -1;
}

void FightFlameGraph::Inspect(int node)
{
    inspected_          = node;
    const auto &call    = report_.calls[node];
    const auto  percent = 100.0 / report_.samplePoints;
    const auto  MS      = report_.workerMs / report_.samplePoints;
    detail_->setText(tr("%1　含子调用 %2 线程毫秒（%3%）　自身 %4 线程毫秒（%5%）　命中 %6 / %7 点")
                         .arg(call.function)
                         .arg(MS * call.inclusive, 0, 'f', 2)
                         .arg(percent * call.inclusive, 0, 'f', 1)
                         .arg(MS * call.self, 0, 'f', 2)
                         .arg(percent * call.self, 0, 'f', 1)
                         .arg(call.inclusive)
                         .arg(report_.samplePoints));
    canvas_->update();
}

bool FightFlameGraph::eventFilter(QObject *object, QEvent *event)
{
    if (object != canvas_) {
        return QWidget::eventFilter(object, event);
    }
    if (event->type() == QEvent::Paint) {
        QPainter p(canvas_);
        p.fillRect(canvas_->rect(), QColor("#202126"));
        for (const auto &block : blocks_) {
            const auto  &call = report_.calls[block.node];
            const auto   BOX  = Box(block);
            const auto   HASH = qHash(call.function, 0);
            const QColor FILL = QColor::fromHsl(12 + int(HASH % 36), 148, block.node == inspected_ ? 183 : 150);
            p.fillRect(QRectF(BOX.x(), BOX.y(), std::max(0.6, BOX.width() - 1), RowHeight - 1), FILL);
            if (BOX.width() < 28) {
                continue;
            }
            const auto text = BOX.adjusted(5, 0, -4, 0);
            p.setPen(QColor("#281d19"));
            const auto LABEL = block.node == focus_ ? call.function : call.function.section("::", -1);
            p.drawText(text, Qt::AlignVCenter | Qt::AlignLeft, p.fontMetrics().elidedText(LABEL, Qt::ElideRight, int(text.width())));
        }
        return true;
    }
    if (blocks_.empty()) {
        return false;
    }
    if (event->type() == QEvent::MouseMove || event->type() == QEvent::MouseButtonRelease) {
        const auto *mouse = static_cast<QMouseEvent *>(event);
        const int   hit   = Hit(mouse->position());
        if (hit >= 0 && event->type() == QEvent::MouseButtonRelease && mouse->button() == Qt::LeftButton) {
            Zoom(hit);
        } else if (inspected_ != (hit < 0 ? focus_ : hit)) {
            Inspect(hit < 0 ? focus_ : hit);
        }
        canvas_->setCursor(hit >= 0 ? Qt::PointingHandCursor : Qt::ArrowCursor);
    } else if (event->type() == QEvent::Leave) {
        Inspect(focus_);
    }
    return QWidget::eventFilter(object, event);
}
