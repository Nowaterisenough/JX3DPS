/**
 * Project: JX3DPS
 * File: ProgressBar.cpp
 * Description:
 * Created Date: 2023-06-30 23:42:41
 * Author: 难为水
 * -----
 * Last Modified: 2023-07-08 09:16:55
 * Modified By: 难为水
 * -----
 * HISTORY:
 * Date      	By   	Comments
 * ----------	-----	----------------------------------------------------------
 */

#include "StatsWidget.h"

#include <unordered_set>

#include <QGraphicsDropShadowEffect>
#include <QLayout>
#include <QPainter>
#include <QSortFilterProxyModel>
#include <QSpacerItem>
#include <QStandardItemModel>
#include <QTableView>

#include <spdlog/spdlog.h>

#include "Button.h"
#include "TableView.h"
#include "ThemeColors.h"

long long GetTargetDamage(const nlohmann::json &stats, const std::string &targetName)
{
    long long targetDamage = 0;

    // 获取目标
    nlohmann::json target = stats["stats"][targetName];

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
    ;
}

std::vector<std::string> GetEffectNames(const nlohmann::json &stats, const std::string &targetName)
{
    std::vector<std::string> effectNames;

    std::unordered_set<std::string> temp;

    if (targetName == "总计") {
        // 遍历目标
        for (auto &target : stats["stats"].items()) {
            // 遍历技能
            for (auto &skill : target.value().items()) {
                temp.insert(skill.key());

                // 遍历词缀
                for (auto &prefix : skill.value().items()) {
                    // 遍历强度
                    for (auto &intensity : prefix.value().items()) {
                        // 遍历判定
                        for (auto &roll : intensity.value().items()) {
                            // 遍历伤害类型
                            for (auto &damageType : roll.value().items()) {
                                // 累加伤害值
                                if (damageType.key() == "破招伤害" && damageType.value().get<long long>() > 0)
                                {
                                    temp.insert("破招·" + skill.key());
                                }
                            }
                        }
                    }
                }
            }
        }
    } else {
        // 获取目标
        nlohmann::json target = stats["stats"][targetName];

        // 遍历技能
        for (auto &skill : target.items()) {
            temp.insert(skill.key());

            // 遍历词缀
            for (auto &prefix : skill.value().items()) {
                // 遍历强度
                for (auto &intensity : prefix.value().items()) {
                    // 遍历判定
                    for (auto &roll : intensity.value().items()) {
                        // 遍历伤害类型
                        for (auto &damageType : roll.value().items()) {
                            // 累加伤害值
                            if (damageType.key() == "破招伤害" && damageType.value().get<long long>() > 0)
                            {
                                temp.insert("破招·" + skill.key());
                            }
                        }
                    }
                }
            }
        }
    }

    for (auto &effectName : temp) {
        effectNames.push_back(effectName);
    }

    return effectNames;
}

long long GetEffectDamage(const nlohmann::json &stats, const std::string &targetName, const std::string &effectName)
{
    long long effectDamage = 0;

    if (effectName.find("破招·") != std::string::npos) {
        std::string name = effectName.substr(effectName.find("破招·") + std::string("破招·").length());
        if (targetName == "总计") {
            // 遍历目标
            for (auto &target : stats["stats"].items()) {
                if (target.value().find(name) == target.value().end()) {
                    continue;
                }

                // 遍历词缀
                for (auto &prefix : target.value()[name].items()) {
                    // 遍历强度
                    for (auto &intensity : prefix.value().items()) {
                        // 遍历判定
                        for (auto &roll : intensity.value().items()) {
                            // 遍历伤害类型
                            for (auto &damageType : roll.value().items()) {
                                // 累加伤害值
                                if (damageType.key() == "破招伤害") {
                                    effectDamage += damageType.value().get<long long>();
                                }
                            }
                        }
                    }
                }
            }
        } else {
            // 获取目标
            nlohmann::json skill = stats["stats"][targetName][name];

            // 遍历词缀
            for (auto &prefix : skill.items()) {
                // 遍历强度
                for (auto &intensity : prefix.value().items()) {
                    // 遍历判定
                    for (auto &roll : intensity.value().items()) {

                        // 遍历伤害类型
                        for (auto &damageType : roll.value().items()) {
                            // 累加伤害值
                            if (damageType.key() == "破招伤害") {
                                effectDamage += damageType.value().get<long long>();
                            }
                        }
                    }
                }
            }
        }

        return effectDamage * 1.0 / stats["sim_iterations"];
    }

    if (targetName == "总计") {
        // 遍历目标
        for (auto &target : stats["stats"].items()) {
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
                            if (damageType.key() != "数目" && damageType.key() != "破招伤害") {
                                effectDamage += damageType.value().get<long long>();
                            }
                        }
                    }
                }
            }
        }
    } else {
        // 获取目标
        nlohmann::json skill = stats["stats"][targetName][effectName];

        // 遍历词缀
        for (auto &prefix : skill.items()) {
            // 遍历强度
            for (auto &intensity : prefix.value().items()) {
                // 遍历判定
                for (auto &roll : intensity.value().items()) {
                    // 遍历伤害类型
                    for (auto &damageType : roll.value().items()) {
                        // 累加伤害值
                        if (damageType.key() != "数目" && damageType.key() != "破招伤害") {
                            effectDamage += damageType.value().get<long long>();
                        }
                    }
                }
            }
        }
    }

    return effectDamage * 1.0 / stats["sim_iterations"];
}

float GetEffectCount(const nlohmann::json &stats, const std::string &targetName, const std::string &effectName)
{
    long long count = 0;
    if (effectName.find("破招·") != std::string::npos) {
        std::string name = effectName.substr(effectName.find("破招·") + std::string("破招·").length());
        spdlog::info("name: {}", name);
        if (targetName == "总计") {
            // 遍历目标
            for (auto &target : stats["stats"].items()) {
                if (target.value().find(name) == target.value().end()) {
                    continue;
                }

                // 遍历词缀
                for (auto &prefix : target.value()[name].items()) {
                    // 遍历强度
                    for (auto &intensity : prefix.value().items()) {
                        // 遍历判定
                        for (auto &roll : intensity.value().items()) {
                            // 遍历伤害类型
                            for (auto &damageType : roll.value().items()) {
                                // 累加伤害值
                                if (damageType.key() == "数目" && roll.value()["破招伤害"] != 0) {
                                    count += damageType.value().get<long long>();
                                }
                            }
                        }
                    }
                }
            }
        } else {
            // 获取目标
            nlohmann::json skill = stats["stats"][targetName][name];

            // 遍历词缀
            for (auto &prefix : skill.items()) {
                // 遍历强度
                for (auto &intensity : prefix.value().items()) {
                    // 遍历判定
                    for (auto &roll : intensity.value().items()) {

                        // 遍历伤害类型
                        for (auto &damageType : roll.value().items()) {
                            // 累加伤害值
                            if (damageType.key() == "数目" && roll.value()["破招伤害"] != 0) {
                                count += damageType.value().get<long long>();
                            }
                        }
                    }
                }
            }
        }

        return count * 1.0 / stats["sim_iterations"];
    }

    if (targetName == "总计") {
        // 遍历目标
        for (auto &target : stats["stats"].items()) {
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
        nlohmann::json skill = stats["stats"][targetName][effectName];

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

    return count * 1.0 / stats["sim_iterations"];
}

StatsWidget::StatsWidget(QWidget *parent) : QDialog(parent)
{
    m_frameless = new FramelessWidget(this);
    m_frameless->SetWidget(this);

    this->setAttribute(Qt::WA_TranslucentBackground);

    this->setFixedWidth(550);
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);

    QGridLayout *layout = new QGridLayout(this);

    TextButton *name = new TextButton(this);
    name->setText("详细统计");
    name->setFixedWidth(55);

    CloseButton *closeButton = new CloseButton(this);
    closeButton->setFixedSize(30, 30);
    closeButton->setToolTip("关闭");
    connect(closeButton, &QPushButton::clicked, this, &StatsWidget::close);

    QWidget *widget = new QWidget(this);

    QSpacerItem *horizontalSpacer1 = new QSpacerItem(3, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);
    QSpacerItem *horizontalSpacer2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(5);

    layout->addItem(horizontalSpacer1, 0, 0, 1, 1);
    layout->addWidget(name, 0, 1, 1, 1);
    layout->addItem(horizontalSpacer2, 0, 2, 1, 1);
    layout->addWidget(closeButton, 0, 3, 1, 1);
    layout->addWidget(widget, 1, 0, 1, 4);

    QVBoxLayout *vLayout = new QVBoxLayout(widget);
    vLayout->setContentsMargins(10, 0, 10, 10);
    vLayout->setSpacing(10);

    TableView *tableViewTarget = new TableView(widget);
    TableView *tableViewEffect = new TableView(widget);

    vLayout->addWidget(tableViewTarget, 0);
    vLayout->addWidget(tableViewEffect, 1);

    tableViewTarget->setModel(new QStandardItemModel());

    QStandardItemModel *modelTarget = static_cast<QStandardItemModel *>(tableViewTarget->model());

    modelTarget->setColumnCount(3);
    modelTarget->setHeaderData(0, Qt::Horizontal, "目标");
    modelTarget->setHeaderData(1, Qt::Horizontal, "伤害");
    modelTarget->setHeaderData(2, Qt::Horizontal, "占比");

    connect(this, &StatsWidget::Signal_UpdateStats, this, [=](const nlohmann::json &stats) {
        m_stats  = stats;
        int rows = stats["stats"].size() + 1;
        modelTarget->setRowCount(rows);

        long long                totalDamage = 0;
        std::vector<long long>   damageList;
        std::vector<std::string> targetNames;
        for (auto &item : stats["stats"].items()) {
            std::string key = item.key();
            targetNames.emplace_back(key);
            long long damage = GetTargetDamage(stats, key);
            damageList.push_back(damage);
            totalDamage += GetTargetDamage(stats, key);
        }

        int row = 0;
        modelTarget->setData(modelTarget->index(row, 0), "总计");
        modelTarget->setData(modelTarget->index(row, 1), QString::number(totalDamage));
        modelTarget->setData(modelTarget->index(row++, 2), "100%");
        for (auto &item : stats["stats"].items()) {
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
            float count = GetEffectCount(m_stats, targetName, effectName);
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
}

void StatsWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(QColor(64, 64, 64));
    painter.setBrush(QColor(COLOR_BACKGROUND_BASE));
    painter.drawRoundedRect(this->rect(), 10, 10);
}
