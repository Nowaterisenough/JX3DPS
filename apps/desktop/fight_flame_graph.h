#pragma once

#include <QRectF>
#include <QWidget>

#include "simulation_result.h"

class QLabel;
class QPushButton;

class FightFlameGraph final : public QWidget
{
public:
    explicit FightFlameGraph(QWidget *parent = nullptr);
    void SetReport(const FightHotspots &report, quint64 run_id);

protected:
    bool eventFilter(QObject *object, QEvent *event) override;

private:
    struct Block
    {
        int    node;
        double left, width;
        int    depth;
    };

    QRectF Box(const Block &block) const;
    int    Hit(QPointF point) const;
    void   Zoom(int node);
    void   Inspect(int node);

    FightHotspots         report_;
    QVector<QVector<int>> children_;
    QVector<Block>        blocks_;
    quint64               runId_ = 0;
    int                   focus_ = 0, inspected_ = -1, levels_ = 0;
    QWidget              *canvas_;
    QLabel               *path_, *detail_;
    QPushButton          *up_, *reset_;
};
