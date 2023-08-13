/**
 * Project: JX3DPS
 * File: ProgressBar.cpp
 * Description:
 * Created Date: 2023-06-30 23:42:41
 * Author: 难为水
 * -----
 * Last Modified: 2023-08-13 09:07:02
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
#include "Splitter/Splitter.h"
#include "TableView/TableView.h"
#include "Common/ThemeColors.h"

long long GetTargetDamage(const nlohmann::ordered_json &stats, const std::string &targetName)
{
    long long targetDamage = 0;

    // 获取目标
    nlohmann::ordered_json target = stats["Stats"]["默认"][targetName];

    // 遍历技能
    for (auto &skill : target.items()) {
        // 遍历词缀
        for (auto &prefix : skill.value().items()) {
            // 遍历强度
            for (auto &intensity : prefix.value().items()) {
                // 遍历判定
                for (auto &roll : intensity.value().items()) {
                    // 遍历伤害类型
                    for (auto &damageType : roll.value().items()) {
                        // 累加伤害值
                        if (damageType.key() != "数目") {
                            targetDamage += damageType.value().get<long long>();
                        }
                    }
                }
            }
        }
    }

    return targetDamage * 1.0 / stats["sim_iterations"];
}

std::vector<std::string> GetEffectNames(const nlohmann::ordered_json &stats, const std::string &targetName)
{
    std::vector<std::string> effectNames;

    std::unordered_set<std::string> temp;

    if (targetName == "总计") {
        // 遍历目标
        for (auto &target : stats["Stats"]["默认"].items()) {
            // 遍历技能
            for (auto &skill : target.value().items()) {
                temp.insert(skill.key());
            }
        }
    } else {
        // 获取目标
        nlohmann::ordered_json target = stats["Stats"]["默认"][targetName];

        // 遍历技能
        for (auto &skill : target.items()) {
            temp.insert(skill.key());
        }
    }

    for (auto &effectName : temp) {
        effectNames.push_back(effectName);
    }

    return effectNames;
}

long long GetEffectDamage(const nlohmann::ordered_json &stats, const std::string &targetName, const std::string &effectName)
{
    long long effectDamage = 0;
    if (targetName == "总计") {
        // 遍历目标
        for (auto &target : stats["Stats"]["默认"].items()) {
            if (target.value().find(effectName) == target.value().end()) {
                continue;
            }

            // 遍历词缀
            for (auto &prefix : target.value()[effectName].items()) {
                // 遍历强度
                for (auto &intensity : prefix.value().items()) {
                    // 遍历判定
                    for (auto &roll : intensity.value().items()) {
                        // 遍历伤害类型
                        for (auto &damageType : roll.value().items()) {
                            // 累加伤害值
                            if (damageType.key() != "数目") {
                                effectDamage += damageType.value().get<long long>();
                            }
                        }
                    }
                }
            }
        }
    } else {
        // 获取目标
        nlohmann::ordered_json skill = stats["Stats"]["默认"][targetName][effectName];

        // 遍历词缀
        for (auto &prefix : skill.items()) {
            // 遍历强度
            for (auto &intensity : prefix.value().items()) {
                // 遍历判定
                for (auto &roll : intensity.value().items()) {
                    // 遍历伤害类型
                    for (auto &damageType : roll.value().items()) {
                        // 累加伤害值
                        if (damageType.key() != "数目") {
                            effectDamage += damageType.value().get<long long>();
                        }
                    }
                }
            }
        }
    }

    return effectDamage * 1.0 / stats["SimIterations"];
}

double GetEffectCount(const nlohmann::ordered_json &stats, const std::string &targetName, const std::string &effectName)
{
    long long count = 0;

    if (targetName == "总计") {
        // 遍历目标
        for (auto &target : stats["Stats"]["默认"].items()) {
            if (target.value().find(effectName) == target.value().end()) {
                continue;
            }
            // 遍历词缀
            for (auto &prefix : target.value()[effectName].items()) {
                // 遍历强度
                for (auto &intensity : prefix.value().items()) {
                    // 遍历判定
                    for (auto &roll : intensity.value().items()) {
                        // 遍历伤害类型
                        for (auto &damageType : roll.value().items()) {
                            // 累加伤害值
                            if (damageType.key() == "数目") {
                                count += damageType.value().get<long long>();
                            }
                        }
                    }
                }
            }
        }
    } else {
        // 获取目标
        nlohmann::ordered_json skill = stats["Stats"]["默认"][targetName][effectName];

        // 遍历词缀
        for (auto &prefix : skill.items()) {
            // 遍历强度
            for (auto &intensity : prefix.value().items()) {
                // 遍历判定
                for (auto &roll : intensity.value().items()) {
                    // 遍历伤害类型
                    for (auto &damageType : roll.value().items()) {
                        // 累加伤害值
                        if (damageType.key() == "数目") {
                            count += damageType.value().get<long long>();
                        }
                    }
                }
            }
        }
    }

    return count * 1.0 / stats["SimIterations"];
}

long long GetEffectRollDamage(const nlohmann::ordered_json &stats,
                              const std::string    &targetName,
                              const std::string    &effectName,
                              const std::string    &rollName)
{
    long long effectDamage = 0;
    if (targetName == "总计") {
        // 遍历目标
        for (auto &target : stats["Stats"]["默认"].items()) {
            if (target.value().find(effectName) == target.value().end()) {
                continue;
            }

            // 遍历词缀
            for (auto &prefix : target.value()[effectName].items()) {
                // 遍历强度
                for (auto &intensity : prefix.value().items()) {
                    // 遍历判定
                    for (auto &roll : intensity.value().items()) {
                        // 遍历伤害类型
                        for (auto &damageType : roll.value().items()) {
                            // 累加伤害值
                            if (damageType.key() != "数目" && roll.key() == rollName)
                            {
                                effectDamage += damageType.value().get<long long>();
                            }
                        }
                    }
                }
            }
        }
    } else {
        // 获取目标
        nlohmann::ordered_json skill = stats["Stats"]["默认"][targetName][effectName];

        // 遍历词缀
        for (auto &prefix : skill.items()) {
            // 遍历强度
            for (auto &intensity : prefix.value().items()) {
                // 遍历判定
                for (auto &roll : intensity.value().items()) {
                    // 遍历伤害类型
                    for (auto &damageType : roll.value().items()) {
                        // 累加伤害值
                        if (damageType.key() != "数目" && roll.key() == rollName)
                        {
                            effectDamage += damageType.value().get<long long>();
                        }
                    }
                }
            }
        }
    }

    return effectDamage * 1.0 / stats["SimIterations"];
}

double GetEffectRollCount(const nlohmann::ordered_json &stats,
                          const std::string    &targetName,
                          const std::string    &effectName,
                          const std::string    &rollName)
{
    long long count = 0;

    if (targetName == "总计") {
        // 遍历目标
        for (auto &target : stats["Stats"]["默认"].items()) {
            if (target.value().find(effectName) == target.value().end()) {
                continue;
            }
            // 遍历词缀
            for (auto &prefix : target.value()[effectName].items()) {
                // 遍历强度
                for (auto &intensity : prefix.value().items()) {
                    // 遍历判定
                    for (auto &roll : intensity.value().items()) {
                        // 遍历伤害类型
                        for (auto &damageType : roll.value().items()) {
                            // 累加伤害值
                            if (damageType.key() == "数目" && roll.key() == rollName) {
                                count += damageType.value().get<long long>();
                            }
                        }
                    }
                }
            }
        }
    } else {
        // 获取目标
        nlohmann::ordered_json skill = stats["Stats"]["默认"][targetName][effectName];

        // 遍历词缀
        for (auto &prefix : skill.items()) {
            // 遍历强度
            for (auto &intensity : prefix.value().items()) {
                // 遍历判定
                for (auto &roll : intensity.value().items()) {
                    // 遍历伤害类型
                    for (auto &damageType : roll.value().items()) {
                        // 累加伤害值
                        if (damageType.key() == "数目" && roll.key() == rollName) {
                            count += damageType.value().get<long long>();
                        }
                    }
                }
            }
        }
    }

    return count * 1.0 / stats["SimIterations"];
}

JX3DPS::Simulator::StatsWidget::StatsWidget(QWidget *parent) : Widget(parent)
{
    // m_frameless = new FramelessWidget(this);
    // m_frameless->SetWidget(this);

    // this->setAttribute(Qt::WA_TranslucentBackground);

    // this->setFixedWidth(580);
    // this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);

    // QGridLayout *layout = new QGridLayout(this);

    // TextButton *name = new TextButton(this);
    // name->setText("详细统计");
    // name->setFixedWidth(55);

    // CloseButton *closeButton = new CloseButton(this);
    // closeButton->setFixedSize(30, 30);
    // closeButton->setToolTip("关闭");
    // connect(closeButton, &QPushButton::clicked, this, &StatsWidget::close);

    // QWidget *widget = new QWidget(this);

    // QSpacerItem *horizontalSpacer1 = new QSpacerItem(3, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);
    // QSpacerItem *horizontalSpacer2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    // layout->setContentsMargins(0, 0, 0, 0);
    // layout->setSpacing(5);

    // layout->addItem(horizontalSpacer1, 0, 0, 1, 1);
    // layout->addWidget(name, 0, 1, 1, 1);
    // layout->addItem(horizontalSpacer2, 0, 2, 1, 1);
    // layout->addWidget(closeButton, 0, 3, 1, 1);
    // layout->addWidget(widget, 1, 0, 1, 4);

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

    // tableViewRoll->setFixedHeight(110);

    vLayout->addWidget(splitter, 0);

    tableViewTarget->setModel(new QStandardItemModel());

    QStandardItemModel *modelTarget = static_cast<QStandardItemModel *>(tableViewTarget->model());

    modelTarget->setColumnCount(3);
    modelTarget->setHeaderData(0, Qt::Horizontal, "目标");
    modelTarget->setHeaderData(1, Qt::Horizontal, "伤害");
    modelTarget->setHeaderData(2, Qt::Horizontal, "占比");

    connect(this, &StatsWidget::Signal_UpdateStats, this, [=](const nlohmann::ordered_json &stats) {
        m_stats  = stats;
        spdlog::info("stats {}", stats["默认"].dump());
        int rows = stats["默认"].size() + 1;
        modelTarget->setRowCount(rows);

        long long                totalDamage = 0;
        std::vector<long long>   damageList;
        std::vector<std::string> targetNames;
        for (auto &item : stats["默认"].items()) {
            std::string key = item.key();
            targetNames.emplace_back(key);
            long long damage = GetTargetDamage(stats, key);
            damageList.push_back(damage);
            totalDamage += GetTargetDamage(stats, key);
        }

        int row = 0;
        modelTarget->setData(modelTarget->index(row, 0), "总计");
        modelTarget->setData(modelTarget->index(row, 1), QString::number(totalDamage));
        modelTarget->setData(modelTarget->index(row++, 2), QString::number(100, 'f', 2).append("%"));
        for (auto &item : stats["默认"].items()) {
            std::string key = item.key();
            modelTarget->setData(modelTarget->index(row, 0), QString::fromStdString(key));
            modelTarget->setData(modelTarget->index(row, 1), QString::number(damageList[row - 1]));
            modelTarget->setData(
                modelTarget->index(row, 2),
                QString::number(damageList[row - 1] * 1.0 / totalDamage * 100, 'f', 2).append("%"));
            row++;
        }
    });

    connect(tableViewTarget, &QTableView::clicked, this, [=](const QModelIndex &index) {
        QSortFilterProxyModel *model = static_cast<QSortFilterProxyModel *>(tableViewEffect->model());
        tableViewEffect->setModel(nullptr);

        if (model != nullptr) {
            delete model;
        }
        QSortFilterProxyModel *proxyModel = new QSortFilterProxyModel(tableViewEffect);
        proxyModel->setSourceModel(new QStandardItemModel());
        QStandardItemModel *modelEffect = static_cast<QStandardItemModel *>(proxyModel->sourceModel());

        // 获取目标名
        std::string targetName =
            modelTarget->data(modelTarget->index(index.row(), 0)).toString().toStdString();
        std::vector<std::string> effectNames = GetEffectNames(m_stats, targetName);

        modelEffect->setColumnCount(5);
        modelEffect->setHeaderData(0, Qt::Horizontal, "效果");
        modelEffect->setHeaderData(1, Qt::Horizontal, "数目");
        modelEffect->setHeaderData(2, Qt::Horizontal, "平均伤害");
        modelEffect->setHeaderData(3, Qt::Horizontal, "总伤害");
        modelEffect->setHeaderData(4, Qt::Horizontal, "占比");

        modelEffect->setRowCount(effectNames.size());
        int row = 0;
        for (auto &effectName : effectNames) {

            modelEffect->setData(modelEffect->index(row, 0), QString::fromStdString(effectName));
            double count = GetEffectCount(m_stats, targetName, effectName);
            modelEffect->setData(modelEffect->index(row, 1), QString::number(count));
            long long damage = GetEffectDamage(m_stats, targetName, effectName);
            modelEffect->setData(modelEffect->index(row, 2), QString::number(damage / count));
            long long totalDamage =
                modelTarget->data(modelTarget->index(index.row(), 1)).toString().toLongLong();
            modelEffect->setData(modelEffect->index(row, 3), damage);
            if (totalDamage == 0) {
                modelEffect->setData(modelEffect->index(row, 4), "0%");
            } else {
                modelEffect->setData(modelEffect->index(row, 4),
                                     QString::number(damage * 1.0 / totalDamage * 100, 'f', 2).append("%"));
            }

            row++;
        }

        tableViewEffect->setModel(proxyModel);

        // 设置按列1从大到小排序
        proxyModel->sort(3, Qt::DescendingOrder);
        tableViewEffect->setModel(proxyModel);
    });

    connect(tableViewEffect, &QTableView::clicked, this, [=](const QModelIndex &index) {
        QSortFilterProxyModel *model = static_cast<QSortFilterProxyModel *>(tableViewRoll->model());
        tableViewRoll->setModel(nullptr);

        if (model != nullptr) {
            delete model;
        }
        QSortFilterProxyModel *proxyModel = new QSortFilterProxyModel(tableViewRoll);
        proxyModel->setSourceModel(new QStandardItemModel());
        QStandardItemModel *modelRoll = static_cast<QStandardItemModel *>(proxyModel->sourceModel());

        modelRoll->setRowCount(2);
        modelRoll->setColumnCount(6);

        modelRoll->setHeaderData(1, Qt::Horizontal, "数目");
        modelRoll->setHeaderData(2, Qt::Horizontal, "数目占比");
        modelRoll->setHeaderData(3, Qt::Horizontal, "平均伤害");
        modelRoll->setHeaderData(4, Qt::Horizontal, "总伤害");
        modelRoll->setHeaderData(5, Qt::Horizontal, "伤害占比");

        proxyModel->sort(4, Qt::DescendingOrder);
        tableViewRoll->setModel(proxyModel);

        // 获取目标名
        std::string targetName = modelTarget->data(modelTarget->index(0, 0)).toString().toStdString();
        std::string effectName =
            static_cast<QSortFilterProxyModel *>(tableViewEffect->model())
                ->data(static_cast<QSortFilterProxyModel *>(tableViewEffect->model())->index(index.row(), 0))
                .toString()
                .toStdString();
        modelRoll->setHeaderData(0,
                                 Qt::Horizontal,
                                /*  QString("%1-%2").arg(targetName.c_str()).arg(effectName.c_str())*/"判定");

        std::vector<std::string> rollNames = { "命中", "会心" };
        int                      row       = 0;
        for (auto &rollName : rollNames) {
            modelRoll->setData(modelRoll->index(row, 0), QString::fromStdString(rollName));

            double count = GetEffectRollCount(m_stats, targetName, effectName, rollName);
            modelRoll->setData(modelRoll->index(row, 1), QString::number(count));

            double sum = GetEffectCount(m_stats, targetName, effectName);
            if (sum == 0) {
                modelRoll->setData(modelRoll->index(row, 2), QString::number(0, 'f', 2).append("%"));
            } else {
                modelRoll->setData(modelRoll->index(row, 2),
                                   QString::number(count * 1.0 / sum * 100, 'f', 2).append("%"));
            }

            long long damage = GetEffectRollDamage(m_stats, targetName, effectName, rollName);
            if (count == 0) {
                modelRoll->setData(modelRoll->index(row, 3), 0);
            } else {
                modelRoll->setData(modelRoll->index(row, 3), QString::number(damage / count));
            }
            modelRoll->setData(modelRoll->index(row, 4), damage);

            long long totalDamage = GetEffectDamage(m_stats, targetName, effectName);

            if (totalDamage == 0) {
                modelRoll->setData(modelRoll->index(row, 5), QString::number(0, 'f', 2).append("%"));
            } else {
                modelRoll->setData(modelRoll->index(row, 5),
                                   QString::number(damage * 1.0 / totalDamage * 100, 'f', 2).append("%"));
            }
            row++;
        }

        // tableViewRoll->setWindowTitle("My Table");
        // tableViewRoll->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
        // tableViewRoll->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents); // 设置其他列的调整方式
        // tableViewRoll->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents); // 设置其他列的调整方式
        // tableViewRoll->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents); // 设置其他列的调整方式
        // // tableViewRoll->horizontalHeader()->setSectionResizeMode(4, QHeaderView::ResizeToContents); // 设置其他列的调整方式
        // tableViewRoll->horizontalHeader()->setSectionResizeMode(5, QHeaderView::ResizeToContents); // 设置其他列的调整方式


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
