/**
 * Project: JX3DPS
 * File: ProgressBar.cpp
 * Description:
 * Created Date: 2023-06-30 23:42:41
 * Author: 难为水
 * -----
 * Last Modified: 2023-09-05 18:25:52
 * Modified By: 难为水
 * -----
 * HISTORY:
 * Date      	By   	Comments
 * ----------	-----	----------------------------------------------------------
 */

#include "JX3DPSImportWidget.h"

#include <unordered_set>

#include <QGraphicsDropShadowEffect>
#include <QHeaderView>
#include <QLayout>
#include <QPainter>
#include <QSortFilterProxyModel>
#include <QSpacerItem>
#include <QStandardItemModel>
#include <QTableView>

#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

#include "Button/Button.h"
#include "Common/ThemeColors.h"
#include "PlainTextEdit/PlainTextEdit.h"

JX3DPS::Simulator::ImportWidget::ImportWidget(QWidget *parent) : Widget(parent)
{
    this->setMinimumWidth(500);
    this->setMinimumHeight(600);
    this->SetTitle("导入魔盒属性");

    QGridLayout *layout = new QGridLayout(this->centralWidget);
    layout->setContentsMargins(10, 0, 10, 10);
    layout->setSpacing(10);

    PlainTextEdit *plainTextEdit = new PlainTextEdit(this);
    Button        *buttonApply   = new Button(this);
    buttonApply->setText("导入");

    Button *buttonCancel = new Button(this);
    buttonCancel->setText("取消");

    layout->addWidget(plainTextEdit, 0, 0, 1, 2);
    layout->addWidget(buttonApply, 1, 0, 1, 1);
    layout->addWidget(buttonCancel, 1, 1, 1, 1);

    connect(buttonCancel, &QPushButton::clicked, [=]() { this->close(); });

    connect(buttonApply, QOverload<bool>::of(&QPushButton::clicked), [=](bool) {
        nlohmann::ordered_json json =
            nlohmann::ordered_json::parse(plainTextEdit->toPlainText().toStdString());

        emit Signal_Import(json);
        this->close();
    });
}

void JX3DPS::Simulator::ImportWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(QColor(64, 64, 64));
    painter.setBrush(QColor(COLOR_BACKGROUND_BASE));
    painter.drawRoundedRect(this->rect(), 10, 10);
}
