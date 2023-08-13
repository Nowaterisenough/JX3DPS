/**
 * Project: JX3DPS
 * File: ProgressBar.cpp
 * Description:
 * Created Date: 2023-06-30 23:42:41
 * Author: 难为水
 * -----
 * Last Modified: 2023-08-13 13:36:17
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

JX3DPS::Simulator::StatsWidget::StatsWidget(QWidget *parent) : Widget(parent)
{
    QVBoxLayout *vLayout = new QVBoxLayout(this->centralWidget);
    vLayout->setContentsMargins(10, 0, 10, 10);
    vLayout->setSpacing(10);

    TableView *tableViewTarget = new TableView(this->centralWidget);
    TableView *tableViewEffect = new TableView(this->centralWidget);
    TableView *tableViewRoll   = new TableView(this->centralWidget);

    Splitter *splitter = new Splitter(this->centralWidget);
    splitter->setOrientation(Qt::Vertical);
    splitter->addWidget(tableViewTarget);
    splitter->addWidget(tableViewEffect);
    splitter->addWidget(tableViewRoll);

    vLayout->addWidget(splitter, 0);

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
