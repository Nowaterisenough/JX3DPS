/**
 * Project: JX3DPS
 * File: ProgressBar.cpp
 * Description:
 * Created Date: 2023-06-30 23:42:41
 * Author: 难为水
 * -----
 * Last Modified: 2023-09-19 04:25:55
 * Modified By: 难为水
 * -----
 * HISTORY:
 * Date      	By   	Comments
 * ----------	-----	----------------------------------------------------------
 */

#include "TimeLineWidget.h"

#include <algorithm>
#include <random>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <QFont>
#include <QGraphicsDropShadowEffect>
#include <QHeaderView>
#include <QImage>
#include <QLayout>
#include <QPainter>
#include <QPixmap>
#include <QSortFilterProxyModel>
#include <QSpacerItem>
#include <QStandardItemModel>
#include <QTableView>

#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

#include "Button/Button.h"
#include "Common/BlurImage.hpp"
#include "Common/ThemeColors.h"
#include "PlainTextEdit/PlainTextEdit.h"

#include "JX3DPSJsonParser.h"

class ThumbnailView : public QGraphicsView
{
public:
    ThumbnailView(QWidget *parent) : QGraphicsView(parent)
    {
        this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        setOptimizationFlag(DontSavePainterState, true);
        setViewportUpdateMode(FullViewportUpdate);
        this->setFrameStyle(QFrame::NoFrame);
        m_rect.setY(0);
        m_rect.setHeight(this->height());
    }

    void SetView(QGraphicsView *view) { m_view = view; }

protected:
    void paintEvent(QPaintEvent *event) override
    {
        QGraphicsView::paintEvent(event);
        QPainter painter(this->viewport());

        painter.setRenderHint(QPainter::Antialiasing, true);

        painter.setPen(QColor(COLOR_BACKGROUND_HIGHLIGHT));
        painter.setBrush(Qt::NoBrush);
        painter.drawRect(this->rect());

        if (m_hovered) {
            painter.setBrush(QColor(64, 64, 64, 128));
            painter.setBrush(QColor(64, 64, 64, 128));
            painter.drawRect(m_rect);
        }
        m_rect.setHeight(this->height());
        if (m_view && !m_hovered) {
            QPointF posX1 = m_view->mapToScene(m_view->x(), m_view->y());
            QPointF posX2 = m_view->mapToScene(m_view->x() + m_view->width(), m_view->y());
            m_rect.setX(this->mapFromScene(posX1).x());
            m_rect.setWidth(this->mapFromScene(posX2).x() - m_rect.x());

            // double rate = (m_rect.x()) * 1.0 / (this->width() - m_rect.width());
            // rate        = rate < 0.0 ? 0.0 : rate > 1.0 ? 1.0 : rate;
            // this->centerOn((this->scene()->width() -
            //                 this->mapToScene(this->rect().width(), 0).x() +
            //                 this->mapToScene(0, 0).x()) *
            //                        rate +
            //                    (this->mapToScene(this->rect().width(), 0).x() -
            //                     this->mapToScene(0, 0).x()) /
            //                        2,

            // 40);
            // this->centerOn(this->mapToScene(m_rect.center().x(), m_rect.center().y()));
        }
    }

    void enterEvent(QEnterEvent *event) override
    {
        m_hovered = true;
        update();
        QGraphicsView::enterEvent(event);
    }

    void leaveEvent(QEvent *event) override
    {
        m_hovered = false;
        update();
    }

    void mousePressEvent(QMouseEvent *event) override
    {
        if (event->button() == Qt::LeftButton) {
            m_dragging = true;

            if (!m_rect.contains(event->pos())) {
                int width = m_rect.width();
                if (event->pos().x() - width / 2 < 0) {
                    m_rect.setX(0);
                } else if (event->pos().x() + width / 2 > this->width()) {
                    m_rect.setX(this->width() - width);
                } else {
                    m_rect.setX(event->pos().x() - width / 2);
                }
                m_lastPos.setX(m_rect.x() + width / 2);
                m_rect.setWidth(width);

                double rate = (m_rect.x()) * 1.0 / (this->width() - m_rect.width());
                rate        = rate < 0.0 ? 0.0 : rate > 1.0 ? 1.0 : rate;
                this->centerOn((this->scene()->width() -
                                this->mapToScene(this->rect().width(), 0).x() +
                                this->mapToScene(0, 0).x()) *
                                       rate +
                                   (this->mapToScene(this->rect().width(), 0).x() -
                                    this->mapToScene(0, 0).x()) /
                                       2,

                               80);
                this->centerOn(this->mapToScene(m_rect.center().x(), m_rect.center().y()));

                SyncView();
            } else {
                m_lastPos = event->pos();
            }
        }
    }

    void mouseMoveEvent(QMouseEvent *event) override
    {
        if (m_dragging) {
            QPoint delta = event->pos() - m_lastPos;
            delta.setY(0);
            if (m_rect.x() + delta.x() < 0 ||
                m_rect.x() + delta.x() > this->width() - m_rect.width())
            {
                return;
            }
            m_rect.translate(delta);

            m_lastPos = event->pos();

            double rate = (m_rect.x() + delta.x()) * 1.0 / (this->width() - m_rect.width());
            rate = rate < 0.0 ? 0.0 : rate > 1.0 ? 1.0 : rate;
            this->centerOn((this->scene()->width() -
                            this->mapToScene(this->rect().width(), 0).x() +
                            this->mapToScene(0, 0).x()) *
                                   rate +
                               (this->mapToScene(this->rect().width(), 0).x() -
                                this->mapToScene(0, 0).x()) /
                                   2,

                           80);
            this->centerOn(this->mapToScene(m_rect.center().x(), m_rect.center().y()));

            SyncView();
            update();
        }
    }

    void mouseReleaseEvent(QMouseEvent *event) override
    {
        if (event->button() == Qt::LeftButton) {
            m_dragging = false;
        }
    }

    void SyncView()
    {
        QPointF pos = this->mapToScene(m_rect.center().x(), m_rect.center().y());
        m_view->centerOn(pos);
    }

private:
    bool           m_hovered  = false;
    bool           m_dragging = false;
    QPoint         m_lastPos;
    QRectF         m_rect = this->rect();
    QGraphicsView *m_view = nullptr;
};

class TimeLineItem : public QGraphicsItem
{
public:
    TimeLineItem() { this->setAcceptHoverEvents(true); }

    ~TimeLineItem() { }

    void SetFrames(int frames) { m_frames = frames; }

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override
    {
        painter->setRenderHint(QPainter::Antialiasing);

        QPen pen;
        pen.setCapStyle(Qt::RoundCap);
        pen.setWidth(1);
        pen.setBrush(QColor(COLOR_BACKGROUND_PRIMARY));

        painter->setPen(pen);
        painter->setBrush(QColor(COLOR_BACKGROUND_PRIMARY));
        painter->drawRoundedRect(0, -2, 27000, 4, 5, 5);
    }

    QRectF boundingRect() const override { return QRectF(0, -2, m_frames * 5, 4); }

    int m_frames = 0;
};

class BuffItem : public QGraphicsItem
{
public:
    BuffItem() { this->setAcceptHoverEvents(true); }

    ~BuffItem() { }

    // void SetPixmap(const QPixmap &pixmap)
    // {
    //     m_pixmap = pixmap;
    //     m_color  = GetDominantColor(m_pixmap);
    // }

    void SetDuration(int duration) { m_duration = duration; }

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override
    {
        painter->setPen(QColor(COLOR_BACKGROUND_PRIMARY));
        painter->setBrush(QColor(COLOR_BACKGROUND_PRIMARY));
        painter->drawRoundedRect(0, -7, 27000, 32, 5, 5);

        painter->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform |
                                QPainter::LosslessImageRendering);
        QSize size(this->boundingRect().width(), this->boundingRect().height());

        // QImage blur2(size, QImage::Format_ARGB32);
        // blur2.fill(QColor(0, 0, 0, 0));
        // QPainter blurPainter2(&blur2);
        // blurPainter2.setRenderHint(QPainter::Antialiasing);
        // blurPainter2.setPen(QColor(m_color));
        // blurPainter2.setBrush(QColor(m_color));
        // blurPainter2.drawRoundedRect(0 + 120, -7 + 50, m_duration * 5 - 20, 32, 5, 5);
        // blurPainter2.end();

        // // painter->setOpacity(0.7);
        // qt_blurImage(blur2, 30.0, false, false);
        // painter->drawImage(this->boundingRect(), blur2);

        // painter->setOpacity(0.8);
        painter->setPen(QColor(m_color));
        painter->setBrush(QColor(m_color));
        painter->drawRoundedRect(0, -7, m_duration * 5, 32, 5, 5);

        painter->setOpacity(1);
        painter->setPen(QColor(m_color));
        painter->setBrush(QColor(m_color));
        painter->drawRect(0, -7, 32, 32);

        painter->drawPixmap(0, -7, m_pixmap.copy(3, 3, 42, 42).scaled(32, 32));
    }

    QRectF boundingRect() const override { return QRectF(-100, -50, 1200, 400); }

private:
    QColor  m_color;
    QPixmap m_pixmap;
    int     m_duration = 0;
};

namespace JX3DPS {

namespace Simulator {

void SkillItem::SetInfo(const Info &info)
{
    m_info = info;
    setX(m_info.timeStamp * 5);
    int second  = x() / 16.0 / 5;
    int minute  = second / 60;
    second      = second % 60;
    int msecond = (x() / 16.0 / 5 - second - minute * 60) * 10000;
    while (msecond % 10 == 0 && msecond != 0) {
        msecond /= 10;
    }
    m_timeStamp =
        QString("%1:%2.%3").arg(minute, 2, 10, QChar('0')).arg(second, 2, 10, QChar('0')).arg(msecond);
}

void SkillItem::Colliding()

{
    // 检测与当前图形项发生碰撞的所有图形项
    QList<QGraphicsItem *> collidingItems = scene()->collidingItems(this, Qt::IntersectsItemShape);
    collidingItems.pop_back();
    QList<double> collidingX;
    bool          otherHovered = false;
    for (QGraphicsItem *item : collidingItems) {
        collidingX.push_back(item->x());
        if (static_cast<SkillItem *>(item)->IsHovered()) {
            otherHovered = true;
        }
    }

    if (m_hovered || collidingX.size() == 0) {
        m_view = true;
    } else if (otherHovered) {
        m_view = false;
    } else if (this->x() < collidingX.first()) {
        bool b = false;
        for (auto &item : collidingItems) {
            if (static_cast<SkillItem *>(item)->IsFirstView()) {
                b = true;
                break;
            }
        }
        if (b) {
            m_firstView = false;
            m_view      = false;
        } else {
            m_firstView = true;
            m_view      = true;
        }
    } else if (this->x() > collidingX.first()) {

        bool b = false;
        for (auto &item : collidingItems) {
            if (static_cast<SkillItem *>(item)->IsFirstView()) {
                b = true;
                break;
            }
        }
        if (b) {
            m_firstView = false;
            m_view      = false;
        } else {
            m_firstView = true;
            m_view      = true;
        }
    }
}

void SkillItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Colliding();

    painter->setOpacity(1);
    setZValue(2);
    if (!m_view) {
        painter->setOpacity(0.25);
        setZValue(1);
    }

    painter->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform |
                            QPainter::LosslessImageRendering);

    QSize size(this->boundingRect().width(), this->boundingRect().height());

    QImage blur2(size, QImage::Format_ARGB32);
    blur2.fill(QColor(0, 0, 0, 0));
    QPainter blurPainter2(&blur2);
    blurPainter2.setRenderHint(QPainter::Antialiasing);
    blurPainter2.setPen(m_info.skills.front().color);
    blurPainter2.setBrush(m_info.skills.front().color);
    blurPainter2.drawEllipse(-7 - this->boundingRect().x(), -7 - this->boundingRect().y(), 14, 14);
    blurPainter2.end();

    if (m_hovered) {
        qt_blurImage(blur2, 10.0, false, false);
        painter->drawImage(this->boundingRect(), blur2);
    }
    QPen pen;
    pen.setWidthF(1.5);
    pen.setColor(Qt::black);
    painter->setPen(pen);
    painter->setBrush(m_info.skills.front().color);
    painter->drawEllipse(QRectF(-7, -7, 14, 14));

    pen.setWidthF(1.0);
    pen.setColor(Qt::black);
    painter->setPen(pen);
    if (m_hovered) {
        painter->setBrush(m_info.skills.front().color);
    } else {
        painter->setBrush(Qt::black);
    }
    painter->drawEllipse(QRectF(-2.5, -2.5, 5, 5));

    QImage blur3(size, QImage::Format_ARGB32);
    blur3.fill(QColor(0, 0, 0, 0));
    QPainter blurPainter3(&blur3);
    blurPainter3.setRenderHint(QPainter::Antialiasing);
    blurPainter3.setPen(m_info.skills.front().color);
    blurPainter3.setBrush(m_info.skills.front().color);
    blurPainter3.drawPolygon(
        QPolygonF() << QPointF(-5 - this->boundingRect().x(), -19 - this->boundingRect().y())
                    << QPointF(-1 - this->boundingRect().x(), -15 - this->boundingRect().y())
                    << QPointF(0 - this->boundingRect().x(), -15 - this->boundingRect().y())
                    << QPointF(4 - this->boundingRect().x(), -19 - this->boundingRect().y()));
    blurPainter3.end();

    if (m_hovered) {
        qt_blurImage(blur3, 30.0, false, false);
        painter->drawImage(this->boundingRect(), blur3);
    }

    painter->setPen(m_info.skills.front().color);
    painter->setBrush(m_info.skills.front().color);
    painter->drawPolygon(QPolygonF() << QPointF(-5, -19) << QPointF(-1, -15)
                                     << QPointF(0, -15) << QPointF(4, -19));

    // 技能图标
    int y0 = 0;
    for (auto &skillInfo : m_info.skills) {

        QImage blur4(size, QImage::Format_ARGB32);
        blur4.fill(QColor(0, 0, 0, 0));
        QPainter blurPainter4(&blur4);
        blurPainter4.setRenderHint(QPainter::Antialiasing);
        blurPainter4.setPen(skillInfo.color);
        blurPainter4.setBrush(skillInfo.color);
        blurPainter4.drawRect(-19 - this->boundingRect().x(),
                              y0 - 64 - this->boundingRect().y(),
                              38,
                              38);
        blurPainter4.end();

        if (m_hovered) {
            qt_blurImage(blur4, 30.0, false, false);
            painter->drawImage(this->boundingRect(), blur4);
        }

        painter->setPen(skillInfo.color);
        painter->setBrush(skillInfo.color);
        painter->drawRect(-20, y0 - 65, 40, 40);

        painter->drawPixmap(-20, y0 - 65, skillInfo.pixmap.copy(3, 3, 42, 42).scaled(40, 40));

        // QPixmap(JX3DPS::Simulator::JsonParser::JX3IconPath(skillInfo.icon))

        if (m_hovered) {

            int y = -108;
            int x = 70;
            setZValue(3);

            QImage blur5(size, QImage::Format_ARGB32);
            blur5.fill(QColor(0, 0, 0, 0));
            QPainter blurPainter5(&blur5);
            blurPainter5.setRenderHint(QPainter::Antialiasing);
            blurPainter5.setPen(QColor(COLOR_BACKGROUND_PRIMARY));
            blurPainter5.setBrush(QColor(COLOR_BACKGROUND_PRIMARY));
            blurPainter5.drawRoundedRect(QRectF(-40 - this->boundingRect().x() + x,
                                                y0 + y + 38 - this->boundingRect().y(),
                                                90,
                                                54),
                                         5,
                                         5);
            blurPainter5.end();

            qt_blurImage(blur5, 20.0, false, false);
            painter->drawImage(this->boundingRect(), blur5);

            QPen pen;
            pen.setWidthF(1);
            pen.setColor(QColor(COLOR_BACKGROUND_PRIMARY));
            painter->setPen(pen);
            painter->setBrush(QColor(COLOR_BACKGROUND_HIGHLIGHT));
            painter->drawRoundedRect(QRectF(-40 + x, y0 + y + 36, 90, 54), 5, 5);

            painter->setPen(QColor(COLOR_HIGHLIGHT));
            painter->setFont(QFont(painter->font().family(), 11));
            painter->drawText(QRectF(-40 + x, y0 + y + 41, 90, 20),
                              Qt::AlignCenter,
                              skillInfo.name);
            painter->drawText(QRectF(-40 + x, y0 + y + 41, 90, 20),
                              Qt::AlignCenter,
                              skillInfo.name);

            pen.setWidthF(0.5);
            pen.setColor(Qt::black);
            painter->setPen(pen);

            painter->drawLine(-24 + x, y0 + y + 65, 30 + x, y0 + y + 65);

            painter->setFont(QFont(painter->font().family(), 10));
            painter->setPen(QColor(COLOR_HIGHLIGHT));
            painter->drawText(QRectF(-42 + x, y0 + y + 66, 94, 20),
                              Qt::AlignCenter,
                              skillInfo.roll + " " + QString::number(skillInfo.damage));
            painter->drawText(QRectF(-42 + x, y0 + y + 66, 94, 20),
                              Qt::AlignCenter,
                              skillInfo.roll + " " + QString::number(skillInfo.damage));
        }
        y0 -= 60;
    }
    if (m_hovered) {
        painter->setPen(QColor(COLOR_HIGHLIGHT));
        painter->drawText(QRectF(-40, 15, 80, 20), Qt::AlignCenter, m_timeStamp);
        painter->drawText(QRectF(-40, 15, 80, 20), Qt::AlignCenter, m_timeStamp);

        // pen.setWidthF(1);
        // pen.setColor(Qt::black);
        // painter->setPen(pen);
        // painter->setBrush(QColor(COLOR_BACKGROUND_PRIMARY));
        // painter->drawRoundedRect(QRectF(-5, 45, 42, 550), 5, 5);

        // QImage blur6(size, QImage::Format_ARGB32);
        // blur6.fill(QColor(0, 0, 0, 0));
        // QPainter blurPainter6(&blur6);
        // blurPainter6.setRenderHint(QPainter::Antialiasing);
        // blurPainter6.setPen(m_info.skills.front().color);
        // blurPainter6.setBrush(m_info.skills.front().color);
        // blurPainter6.drawPixmap(0 - this->boundingRect().x(),
        //                         55 - this->boundingRect().y(),
        //                         m_pixmap.copy(3, 3, 42, 42).scaled(32, 32));
        // blurPainter6.end();

        // qt_blurImage(blur6, 20.0, false, false);
        // painter->drawImage(this->boundingRect(), blur6);

        // painter->setPen(m_info.skills.front().color);
        // painter->setBrush(m_info.skills.front().color);
        // painter->drawRect(0, 55, 32, 32);

        // painter->drawPixmap(0, 55, m_pixmap.copy(3, 3, 42, 42).scaled(32, 32));

        // painter->setOpacity(0.3);
        // painter->setPen(m_info.skills.front().color);
        // painter->setBrush(m_info.skills.front().color);
        // painter->drawRect(0, 105, 32, 32);

        // painter->drawPixmap(0, 105, m_pixmap.copy(3, 3, 42, 42).scaled(32, 32));
    }
}

QRectF SkillItem::boundingRect() const
{
    return QRectF(-80, -370, 200, 420);
}

void SkillItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    QGraphicsItem::hoverEnterEvent(event);
    m_hovered = true;
    this->update();
}

void SkillItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    QGraphicsItem::hoverLeaveEvent(event);
    m_hovered = false;
    this->update();
}

QPainterPath SkillItem::shape() const
{
    QPainterPath path;
    int          y0 = 0;
    for (auto &info : m_info.skills) {
        path.addEllipse(-20, y0 - 65, 40, 40);
        y0 -= 60;
    }

    path.addEllipse(-7, -7, 14, 14);
    return path;
}

void SkillItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    static_cast<Widget *>(this->scene()->parent())->SetMousePressed(false);
    QGraphicsItem::mouseReleaseEvent(event);
    this->update();
    static_cast<Widget *>(this->scene()->parent())->SetMousePressed(false);
}

class GraphicsView : public QGraphicsView
{
public:
    GraphicsView(QWidget *parent) : QGraphicsView(parent)
    {
        this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    }

protected:
};

TimeLineWidget::TimeLineWidget(QWidget *parent) : Widget(parent)
{
    this->setMinimumWidth(500);
    this->setMinimumHeight(600);

    this->SetTitle("时间轴");

    QGridLayout *layout = new QGridLayout(this->centralWidget);
    layout->setContentsMargins(10, 0, 10, 10);
    layout->setSpacing(10);

    m_scene = new QGraphicsScene(this);

    GraphicsView *view = new GraphicsView(this);

    view->setFrameStyle(QFrame::NoFrame);
    view->setMouseTracking(true);
    view->setAttribute(Qt::WA_NoMousePropagation, true);
    m_scene->setBackgroundBrush(QColor(COLOR_BACKGROUND_BASE));
    // m_scene->setSceneRect(-100, -250, 100, 120);

    layout->addWidget(view);

    ThumbnailView *thumbnailView = new ThumbnailView(this);
    thumbnailView->setAttribute(Qt::WA_NoMousePropagation, true);
    thumbnailView->setFixedHeight(110);
    thumbnailView->SetView((QGraphicsView *)view);

    m_scene->setSceneRect(-100, -250, 5000 * 5 + 200, 250);
    thumbnailView->fitInView(m_scene->sceneRect(), Qt::KeepAspectRatioByExpanding);
    thumbnailView->setScene(m_scene);

    thumbnailView->setAlignment(Qt::AlignLeft | Qt::AlignVCenter); // 设置对齐方式
    layout->addWidget(thumbnailView);

    connect(this,
            &TimeLineWidget::Signal_Import,
            this,
            [=](const nlohmann::ordered_json &json, const nlohmann::ordered_json &config) {
        m_scene->clear();

        TimeLineItem *timeLineItem = new TimeLineItem();
        m_scene->addItem(timeLineItem);
        std::list<SkillItem::Info> skillInfos;
        if (JsonParser::LoadJsonTimeLine(json, config, skillInfos)) {
            int frames = json["Frames"].get<int>();
            timeLineItem->SetFrames(frames);
            m_scene->setSceneRect(-100, -450, frames * 5 + 200, 580);
            view->setScene(m_scene);
            view->centerOn(0, -160);
            thumbnailView->fitInView(m_scene->sceneRect(), Qt::KeepAspectRatioByExpanding);
            thumbnailView->setScene(m_scene);
            thumbnailView->centerOn(0, 0);
            for (auto &skillInfo : skillInfos) {
                SkillItem *item = new SkillItem();
                item->SetInfo(skillInfo);
                m_scene->addItem(item);
            }
        }
    });

    this->resize(QSize(1000, 500));
}

void TimeLineWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(QColor(64, 64, 64));
    painter.setBrush(QColor(COLOR_BACKGROUND_BASE));
    painter.drawRoundedRect(this->rect(), 10, 10);
}

} // namespace Simulator

} // namespace JX3DPS