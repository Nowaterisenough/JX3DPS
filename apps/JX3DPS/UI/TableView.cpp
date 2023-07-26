/**
 * Project: JX3DPS
 * File: SpinBox.cpp
 * Description:
 * Created Date: 2023-06-10 08:38:29
 * Author: 难为水
 * -----
 * Last Modified: 2023-07-13 21:38:36
 * Modified By: 难为水
 * -----
 * HISTORY:
 * Date      	By   	Comments
 * ----------	-----	----------------------------------------------------------
 */

#include "TableView.h"

#include <QHeaderView>
#include <QIntValidator>
#include <QPainter>
#include <QSortFilterProxyModel>
#include <QStyledItemDelegate>
#include <QStylePainter>

#include "ThemeColors.h"

TableView::TableView(QWidget *parent) : QTableView(parent)
{
    this->setAttribute(Qt::WA_TranslucentBackground);
    horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    // horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    verticalHeader()->setHidden(true);
    resizeColumnsToContents();
    horizontalHeader()->hideSection(0);
    // 去除垂直滚动条
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // 去除水平滚动条
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    // BorderDelegate delegate;
    // this->setItemDelegate(&delegate);
    //  setShowGrid(false);

    // 修改tableView的背景颜色
    QPalette palette = this->palette();
    palette.setColor(QPalette::Base, QColor(COLOR_BACKGROUND_HIGHLIGHT));
    this->setPalette(palette);

    // 修改tableView的字体颜色
    QPalette textPalette = this->palette();
    textPalette.setColor(QPalette::Text, QColor(COLOR_HIGHLIGHT));
    this->setPalette(textPalette);

    this->setStyleSheet("QTableView { border: 1px solid rgb(139, 146, 154); }"
                        "QTableView::item:selected {"
                        "  background-color: rgb(10, 115, 230);"
                        "  color: white;"
                        "}");

    // 获取表头视图
    QHeaderView *header = this->horizontalHeader();

    // 设置表头字体颜色

    // 设置表头背景颜色
    QString headerStyle =
        "QHeaderView::section { background-color: rgb(13, 19, 27); color: "
        "rgb(230, 230, 230); height:42px;} "
        "QHeaderView::section:checked {"
        "  background-color: rgb(10, 115, 230);"
        "  border: none;" // 去掉边框
        "  padding: 0;"   // 去掉内边距
        "  margin: 0;}";  // 去掉外边距;
    horizontalHeader()->setStyleSheet(headerStyle);
    horizontalHeader()->setFont(QFont("NoWatsFont", 11));

    this->setFocusPolicy(Qt::NoFocus);
    this->verticalHeader()->setDefaultSectionSize(35);
}

void TableView::paintEvent(QPaintEvent *event)
{
    if (!this->model()) {
        return;
    }
    QBrush brush1(QColor(COLOR_BACKGROUND_BASE));
    QBrush brush2(QColor(COLOR_BACKGROUND_PRIMARY));
    for (int row = 0; row < this->model()->rowCount(); ++row) {
        QBrush brush = (row % 2 == 0) ? brush1 : brush2;
        for (int col = 0; col < this->model()->columnCount(); ++col) {
            this->model()->setData(this->model()->index(row, col), brush, Qt::BackgroundRole);
        }
    }
    QTableView::paintEvent(event);
}
