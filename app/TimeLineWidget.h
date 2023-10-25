/**
 * Project: JX3DPS
 * File: ProgressBar.h
 * Description:
 * Created Date: 2023-06-30 23:40:42
 * Author: 难为水
 * -----
 * Last Modified: 2023-10-26 03:20:30
 * Modified By: 难为水
 * -----
 * HISTORY:
 * Date      	By   	Comments
 * ----------	-----	----------------------------------------------------------
 */

#ifndef __JX3DPS_TIME_LINE_WIDGET_H__
#define __JX3DPS_TIME_LINE_WIDGET_H__

#include <QDialog>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QtOpenGLWidgets/QOpenGLWidget>

#include <nlohmann/json.hpp>

#include <Attribute/Attribute.hpp>

#include "Widget/Widget.h"

namespace JX3DPS {

namespace Simulator {

class SkillItem : public QGraphicsItem
{
public:

    struct SkillInfo
    {
        int icon;
        int damage     = 0;
        int rollResult = 1;
        int type       = 1;

        QString name;
        QPixmap pixmap;
        QColor  color;
        QString roll;
    };

    struct Info
    {
        int                  timeStamp = 0;
        std::list<SkillInfo> skills;
    };

    SkillItem() { this->setAcceptHoverEvents(true); }

    ~SkillItem() { }

    bool IsHovered() const { return m_hovered; }

    bool IsFirstView() const { return m_firstView; }

    void SetInfo(const Info &info);

    void Colliding();

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    QRectF boundingRect() const override;

    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;

    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;

    QPainterPath shape() const override;

    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

private:
    bool m_hovered   = false;
    bool m_view      = true;
    bool m_firstView = true;

    QString m_timeStamp;

    Info m_info;
};

class TimeLineWidget : public Widget
{
    Q_OBJECT

public:
    TimeLineWidget(QWidget *parent = nullptr);

signals:
    void Signal_Import(const nlohmann::ordered_json &json, const nlohmann::ordered_json &config);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    std::vector<SkillItem *> m_skillItems;
    QGraphicsScene          *m_scene;
};

} // namespace Simulator

} // namespace JX3DPS

#endif // __JX3DPS_TIME_LINE_WIDGET_H__