/**
 * Project: JX3DPS
 * File: ProgressBar.cpp
 * Description:
 * Created Date: 2023-06-30 23:42:41
 * Author: 难为水
 * -----
 * Last Modified: 2023-08-22 23:27:13
 * Modified By: 难为水
 * -----
 * HISTORY:
 * Date      	By   	Comments
 * ----------	-----	----------------------------------------------------------
 */

#include "JX3DPSStatsWidget.h"

#include <unordered_set>

#include <QGraphicsDropShadowEffect>
#include <QHeaderView>
#include <QLayout>
#include <QPainter>
#include <QSortFilterProxyModel>
#include <QSpacerItem>
#include <QStandardItemModel>
#include <QTableView>

#include <spdlog/spdlog.h>

#include "Button/Button.h"
#include "Common/ThemeColors.h"
#include "Splitter/Splitter.h"
#include "TableView/TableView.h"

#include "JX3DPSJsonParser.h"
#include "qcustomplot.h"

std::vector<int> Histogram(const std::vector<int> &data, int numBins)
{
    std::vector<int> histogram(numBins, 0);
    int              minValue = *std::min_element(data.begin(), data.end());
    int              maxValue = *std::max_element(data.begin(), data.end());
    double           binWidth = static_cast<double>(maxValue - minValue) / numBins;
    for (int i = 0; i < data.size(); ++i) {
        int binIndex = static_cast<int>((data[i] - minValue) / binWidth);
        if (binIndex < 0) {
            binIndex = 0;
        } else if (binIndex >= numBins) {
            binIndex = numBins - 1;
        }
        ++histogram[binIndex];
    }
    return histogram;
}

std::vector<double> GaussianFilter(const std::vector<double> &data, double sigma)
{
    std::vector<double> result(data.size(), 0.0);
    int                 halfSize = static_cast<int>(std::ceil(3 * sigma));
    std::vector<double> kernel(2 * halfSize + 1, 0.0);
    double              sum = 0.0;
    for (int i = -halfSize; i <= halfSize; ++i) {
        kernel[i + halfSize]  = std::exp(-i * i / (2 * sigma * sigma));
        sum                  += kernel[i + halfSize];
    }
    for (int i = 0; i < kernel.size(); ++i) {
        kernel[i] /= sum;
    }
    for (int i = halfSize; i < data.size() - halfSize; ++i) {
        double value = 0.0;
        for (int j = -halfSize; j <= halfSize; ++j) {
            value += data[i + j] * kernel[j + halfSize];
        }
        result[i] = value;
    }
    return result;
}

JX3DPS::Simulator::StatsWidget::StatsWidget(QWidget *parent) : Widget(parent)
{
    this->setMinimumWidth(1200);

    QGridLayout *layout = new QGridLayout(this->centralWidget);
    layout->setContentsMargins(10, 0, 10, 10);
    layout->setSpacing(10);

    QCustomPlot *customPlot = new QCustomPlot(this->centralWidget);
    layout->addWidget(customPlot, 0, 1, 1, 1);

    QLinearGradient plotGradient;
    plotGradient.setStart(0, 0);
    plotGradient.setFinalStop(0, 350);
    plotGradient.setColorAt(0, QColor(13, 19, 27));
    plotGradient.setColorAt(1, QColor(13, 19, 27));
    customPlot->setBackground(plotGradient); // 设置背景颜色

    // 每条网格对应一个刻度
    customPlot->xAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine)); // 网格线(对应刻度)画笔
    customPlot->yAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
    customPlot->xAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine)); // 子网格线(对应子刻度)画笔
    customPlot->yAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
    customPlot->xAxis->grid()->setSubGridVisible(true); // 显示子网格线
    customPlot->yAxis->grid()->setSubGridVisible(true);
    // customPlot->xAxis->grid()->setZeroLinePen(QPen(Qt::white));   // 设置刻度为0时的网格线的画笔
    // customPlot->yAxis->grid()->setZeroLinePen(QPen(Qt::white));
    customPlot->xAxis->grid()->setZeroLinePen(Qt::NoPen);
    customPlot->yAxis->grid()->setZeroLinePen(Qt::NoPen);

    customPlot->xAxis->setTickLabels(false);
    customPlot->yAxis->setTickLabels(false);

    customPlot->xAxis->setSubTicks(false);
    customPlot->yAxis->setSubTicks(false);

    customPlot->xAxis->setBasePen(Qt::NoPen);
    customPlot->yAxis->setBasePen(Qt::NoPen);

    customPlot->xAxis->setTickPen(Qt::NoPen);
    customPlot->yAxis->setTickPen(Qt::NoPen);

    customPlot->xAxis->setSubTickPen(Qt::NoPen);
    customPlot->yAxis->setSubTickPen(Qt::NoPen);

    // 添加数据的 x 最小值和最大值
    QCPItemText *textMin = new QCPItemText(customPlot);
    textMin->setPositionAlignment(Qt::AlignLeft | Qt::AlignBottom);
    textMin->setColor(Qt::white);
    textMin->setFont(QFont(this->font().family(), 11));

    QCPItemText *textMax = new QCPItemText(customPlot);
    textMax->setPositionAlignment(Qt::AlignRight | Qt::AlignBottom);
    textMax->setColor(Qt::white);
    textMax->setFont(QFont(this->font().family(), 11));

    connect(this, &StatsWidget::Signal_UpdateStats, this, [=](const nlohmann::ordered_json &result) {
        std::vector<long long> res = result["DamageList"].get<std::vector<long long>>();

        int frames = result["Frames"].get<int>();

        std::vector<int> nums;
        for (auto &damage : res) {
            nums.push_back(damage * 16 / frames);
        }

        std::vector<int> histogram = Histogram(nums, 100);

        QVector<double> x(histogram.size()), y(histogram.size());
        for (int i = 0; i < histogram.size(); ++i) {
            x[i] = i;
            y[i] = histogram[i];
        }

        // 创建曲线对象
        customPlot->clearGraphs();
        QCPGraph *graph = customPlot->addGraph();

        // 设置曲线样式
        QPen pen; // 添加一个曲线图QGraph
        pen.setColor(QColor(26, 159, 255));
        pen.setWidthF(1.5);

        customPlot->graph(0)->setPen(pen);

        // 对曲线进行高斯滤波
        std::vector<double> ySmooth = GaussianFilter(std::vector<double>(y.begin(), y.end()), 1.5);
        QVector<double> ySmoothQCP(ySmooth.size());
        std::copy(ySmooth.begin(), ySmooth.end(), ySmoothQCP.begin());
        graph->setData(x, ySmoothQCP);

        // 设置坐标轴范围
        customPlot->xAxis->setRange(0, histogram.size() - 1);
        customPlot->yAxis->setRange(0, *std::max_element(histogram.begin(), histogram.end()));

        int       mean = 0;
        long long sum  = 0;
        for (int i = 0; i < nums.size(); ++i) {
            sum += nums[i];
        }
        mean = sum * 1.0 / nums.size();

        // 添加数据的 x 最小值和最大值
        textMin->position->setCoords(x[0] + 2, 0.5);
        textMin->setText(
            QString("%1 %2%")
                .arg(*std::min_element(nums.begin(), nums.end()))
                .arg((*std::min_element(nums.begin(), nums.end()) - mean) * 100.0 / mean, 0, 'f', 2));

        textMax->position->setCoords(x[x.size() - 1] - 2, 0.5);
        textMax->setText(
            QString("%1 %2%")
                .arg(*std::max_element(nums.begin(), nums.end()))
                .arg((*std::max_element(nums.begin(), nums.end()) - mean) * 100.0 / mean, 0, 'f', 2));

        customPlot->replot();
    });

    // ----------------------------------------------

    TableView *tableViewTarget = new TableView(this->centralWidget);
    TableView *tableViewEffect = new TableView(this->centralWidget);
    TableView *tableViewRoll   = new TableView(this->centralWidget);

    tableViewRoll->setFixedHeight(113);

    Splitter *splitter = new Splitter(this->centralWidget);
    splitter->setOrientation(Qt::Vertical);
    splitter->addWidget(tableViewTarget);
    splitter->addWidget(tableViewEffect);
    splitter->addWidget(tableViewRoll);

    layout->addWidget(splitter, 0, 0, 1, 1);

    tableViewTarget->setModel(new QStandardItemModel());
    tableViewEffect->setModel(new QStandardItemModel());
    tableViewRoll->setModel(new QStandardItemModel());

    QStandardItemModel *modelTarget = static_cast<QStandardItemModel *>(tableViewTarget->model());
    QStandardItemModel *modelEffect = static_cast<QStandardItemModel *>(tableViewEffect->model());
    QStandardItemModel *modelRoll = static_cast<QStandardItemModel *>(tableViewRoll->model());

    modelTarget->setColumnCount(3);
    modelTarget->setHeaderData(0, Qt::Horizontal, "目标");
    modelTarget->setHeaderData(1, Qt::Horizontal, "伤害");
    modelTarget->setHeaderData(2, Qt::Horizontal, "占比");

    modelEffect->setColumnCount(5);
    modelEffect->setHeaderData(0, Qt::Horizontal, "效果");
    modelEffect->setHeaderData(1, Qt::Horizontal, "数目");
    modelEffect->setHeaderData(2, Qt::Horizontal, "平均伤害");
    modelEffect->setHeaderData(3, Qt::Horizontal, "总伤害");
    modelEffect->setHeaderData(4, Qt::Horizontal, "占比");

    modelRoll->setColumnCount(6);
    modelRoll->setHeaderData(0, Qt::Horizontal, "判定");
    modelRoll->setHeaderData(1, Qt::Horizontal, "数目");
    modelRoll->setHeaderData(2, Qt::Horizontal, "数目占比");
    modelRoll->setHeaderData(3, Qt::Horizontal, "平均伤害");
    modelRoll->setHeaderData(4, Qt::Horizontal, "总伤害");
    modelRoll->setHeaderData(5, Qt::Horizontal, "伤害占比");

    connect(this, &StatsWidget::Signal_UpdateStats, this, [=](const nlohmann::ordered_json &result) {
        m_stats         = result["Stats"]["默认"];
        m_simIterations = result["SimIterations"].get<int>();

        emit Signal_UpdateTargetStats();

        std::string targetName =
            modelTarget->data(modelTarget->index(tableViewTarget->currentIndex().row(), 0))
                .toString()
                .toStdString();
        long long targetDamage =
            modelTarget->data(modelTarget->index(tableViewTarget->currentIndex().row(), 1))
                .toLongLong();

        emit Signal_UpdateTargetEffectStats(targetName, targetDamage);

        std::string effectName =
            modelEffect->data(modelEffect->index(tableViewEffect->currentIndex().row(), 0))
                .toString()
                .toStdString();
        int effectCount =
            modelEffect->data(modelEffect->index(tableViewEffect->currentIndex().row(), 1))
                .toLongLong();
        long long effectDamage =
            modelEffect->data(modelEffect->index(tableViewEffect->currentIndex().row(), 3))
                .toLongLong();

        emit Signal_UpdateTargetEffectRollStats(targetName, effectName, effectDamage, effectCount);
    });

    connect(this, &StatsWidget::Signal_UpdateTargetStats, this, [=]() {
        auto      index         = tableViewTarget->currentIndex();
        long long totalDamage   = JsonParser::GetTotalDamage(m_stats);
        auto      targetDamages = JsonParser::GetTargetDamages(m_stats);
        modelTarget->setRowCount(targetDamages.size());
        int row = 0;
        for (auto &[targetName, damage] : targetDamages) {
            modelTarget->setData(modelTarget->index(row, 0), QString::fromStdString(targetName));
            modelTarget->setData(modelTarget->index(row, 1), damage / m_simIterations);
            modelTarget->setData(modelTarget->index(row, 2),
                                 QString::number(damage * 100.0 / totalDamage, 'f', 2).append("%"));
            row++;
        }

        modelTarget->sort(1, Qt::DescendingOrder);
        tableViewTarget->setCurrentIndex(index);
    });

    connect(this,
            &StatsWidget::Signal_UpdateTargetEffectStats,
            this,
            [=](const std::string &targetName, long long targetDamage) {
        auto index = tableViewEffect->currentIndex();
        auto targetEffectDamages = JsonParser::GetTargetEffectDamages(m_stats, targetName);
        modelEffect->setRowCount(targetEffectDamages.size());
        int row = 0;
        for (auto &[effectName, info] : targetEffectDamages) {
            auto &[count, damage] = info;
            modelEffect->setData(modelEffect->index(row, 0), QString::fromStdString(effectName));
            modelEffect->setData(modelEffect->index(row, 1), count * 1.0 / m_simIterations);
            modelEffect->setData(modelEffect->index(row, 2), damage * 1.0 / count);
            modelEffect->setData(modelEffect->index(row, 3), damage / m_simIterations);
            modelEffect->setData(
                modelEffect->index(row, 4),
                QString::number(damage * 100.0 / targetDamage / m_simIterations, 'f', 2).append("%"));
            row++;
        }

        modelEffect->sort(3, Qt::DescendingOrder);
        tableViewEffect->setCurrentIndex(index);
    });

    connect(tableViewTarget, &QTableView::clicked, this, [=](const QModelIndex &index) {
        // 获取目标名
        std::string targetName =
            modelTarget->data(modelTarget->index(index.row(), 0)).toString().toStdString();
        long long targetDamage = modelTarget->data(modelTarget->index(index.row(), 1)).toLongLong();

        emit Signal_UpdateTargetEffectStats(targetName, targetDamage);
    });

    connect(this,
            &StatsWidget::Signal_UpdateTargetEffectRollStats,
            this,
            [=](const std::string &targetName, const std::string &effectName, long long effectDamage, int effectCount) {
        auto index = tableViewRoll->currentIndex();
        auto targetEffectRollDamages =
            JsonParser::GetTargetEffectDamages(m_stats, targetName, effectName);
        modelRoll->setRowCount(targetEffectRollDamages.size());
        int row = 0;
        for (auto &[rollName, info] : targetEffectRollDamages) {
            auto &[count, damage] = info;
            modelRoll->setData(modelRoll->index(row, 0), QString::fromStdString(rollName));
            modelRoll->setData(modelRoll->index(row, 1), count * 1.0 / m_simIterations);
            modelRoll->setData(
                modelRoll->index(row, 2),
                QString::number(count * 100.0 / m_simIterations / effectCount, 'f', 2).append("%"));
            modelRoll->setData(modelRoll->index(row, 3), damage * 1.0 / count);
            modelRoll->setData(modelRoll->index(row, 4), damage / m_simIterations);
            modelRoll->setData(
                modelRoll->index(row, 5),
                QString::number(damage * 100.0 / effectDamage / m_simIterations, 'f', 2).append("%"));
            row++;
        }

        modelRoll->sort(4, Qt::DescendingOrder);
        tableViewRoll->setCurrentIndex(index);
    });

    connect(tableViewEffect, &QTableView::clicked, this, [=](const QModelIndex &index) {
        // 获取目标名
        std::string targetName =
            modelTarget->data(modelTarget->index(tableViewTarget->currentIndex().row(), 0))
                .toString()
                .toStdString();
        std::string effectName =
            modelEffect->data(modelEffect->index(index.row(), 0)).toString().toStdString();
        int effectCount = modelEffect->data(modelEffect->index(index.row(), 1)).toLongLong();
        long long effectDamage = modelEffect->data(modelEffect->index(index.row(), 3)).toLongLong();

        emit Signal_UpdateTargetEffectRollStats(targetName, effectName, effectDamage, effectCount);
    });
}

void JX3DPS::Simulator::StatsWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(QColor(64, 64, 64));
    painter.setBrush(QColor(COLOR_BACKGROUND_BASE));
    painter.drawRoundedRect(this->rect(), 10, 10);
}
