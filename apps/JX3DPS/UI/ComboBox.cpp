/**
 * Project: JX3DPS
 * File: SpinBox.cpp
 * Description:
 * Created Date: 2023-06-10 08:38:29
 * Author: 难为水
 * -----
 * Last Modified: 2023-06-18 10:14:45
 * Modified By: 难为水
 * -----
 * HISTORY:
 * Date      	By   	Comments
 * ----------	-----	----------------------------------------------------------
 */

#include "ComboBox.h"

#include <QAbstractItemDelegate>
#include <QAbstractTableModel>
#include <QEvent>
#include <QGraphicsDropShadowEffect>
#include <QHeaderView>
#include <QPainter>
#include <QTableView>

#include "Core/JX3Params.h"

#include "Color.h"

const int ROWS = 2;

ComboBoxClass::ComboBoxClass(QWidget *parent) : QComboBox(parent)
{
    this->setAttribute(Qt::WA_TranslucentBackground);
    // 创建一个阴影效果对象
    QGraphicsDropShadowEffect *shadowEffect = new QGraphicsDropShadowEffect(this);
    shadowEffect->setColor(Qt::black);     // 设置阴影的颜色
    shadowEffect->setBlurRadius(10);       // 设置阴影的模糊半径
    shadowEffect->setOffset(0, 0);         // 设置阴影的偏移量
    this->setGraphicsEffect(shadowEffect); // 为按钮应用阴影效果

    TableModelClass *model = new TableModelClass();

    QTableView *view = new QTableView(this);
    view->horizontalHeader()->setVisible(false);
    view->verticalHeader()->setVisible(false);
    view->setAlternatingRowColors(false);
    view->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    view->setMouseTracking(true);
    view->setItemDelegate((QAbstractItemDelegate *)new ItemDelegateClass(view));

    view->setFocusPolicy(Qt::NoFocus);
    view->setStyleSheet("QTableView{background-color: rgb(43, 46, 50);"
                        "border: none;"
                        "color: rgb(255, 255, 255);"
                        "selection-background-color: rgb(56, 60, 67);"
                        "selection-color: rgb(255, 255, 255);"
                        "}"
                        "QTableView::item{"
                        "border: none;"
                        "}"
                        "QTableView::item:selected{"
                        "background-color: rgb(56, 60, 67);"
                        "}"
                        "QTableView::item:hover{"
                        "background-color: rgb(56, 60, 67);"
                        "}");

    this->setModelColumn(3);
    this->setModel(model);
    this->setView(view);

    QWidget *popup = this->findChild<QFrame *>();
    popup->setFixedHeight(30 * 20 / ROWS + 2);
    popup->setFixedWidth(70 * ROWS);

    connect(view, &QTableView::entered, [=](const QModelIndex &index) {
        m_pixmap =
            new QPixmap(QString(":/resources/pics/JX3/%1.png")
                            .arg(QString::fromStdString(JX3DPS::CLASS.at(index.row() * ROWS + index.column()))));
        this->update();
        emit Signal_CurrentClassChanged(index.row() * ROWS + index.column());
    });
}

void ComboBoxClass::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

    // 画一个框
    painter.setPen(QPen(QColor(56, 60, 67)));
    painter.setBrush(QBrush(QColor(43, 46, 50)));
    painter.drawRect(this->rect());

    // 画一个图形
    if (m_pixmap != nullptr) {
        painter.setPen(QPen(QColor(0, 0, 0)));
        painter.setBrush(QBrush(QColor(0, 0, 0)));
        painter.drawRect(2, 2, this->width() - 4, this->height() - 4);
        int border = 6;
        painter.drawPixmap(border,
                           border,
                           this->width() - 2 * border,
                           this->height() - 2 * border,
                           m_pixmap->copy(3, 3, 42, 42));
    }
}

void ComboBoxClass::enterEvent(QEnterEvent *event)
{
    this->setCursor(Qt::PointingHandCursor);
}

void ComboBoxClass::showPopup()
{
    QComboBox::showPopup();
    QWidget *popup = this->findChild<QFrame *>();

    popup->move(popup->x(), popup->y() + 5);
}

ItemDelegateClass::ItemDelegateClass(QObject *parent) : QStyledItemDelegate(parent) { }

void ItemDelegateClass::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionViewItem viewOption(option);

    QStyledItemDelegate::paint(painter, option, index);
    painter->fillRect(option.rect, QBrush(QColor(43, 46, 50)));
    if (option.state & QStyle::State_Selected) {
        // 自定义选中背景色
        painter->fillRect(option.rect, QBrush(QColor(23, 29, 37)));
    }

    QFont font("微软雅黑", 10.5);
    painter->setFont(font);
    painter->setPen(QColor(208, 211, 212));
    painter->drawText(option.rect,
                      Qt::AlignHCenter | Qt::AlignVCenter,
                      JX3DPS::CLASS.at(index.row() * ROWS + index.column()).c_str());
    painter->drawText(option.rect,
                      Qt::AlignHCenter | Qt::AlignVCenter,
                      JX3DPS::CLASS.at(index.row() * ROWS + index.column()).c_str());
}

QSize ItemDelegateClass::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    return QSize(option.rect.width(), option.rect.height());
}

TableModelClass::TableModelClass(QObject *parent) : QAbstractTableModel(parent) { }

int TableModelClass::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return JX3DPS::CLASS.size() / ROWS;
}

int TableModelClass::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return ROWS;
}

QVariant TableModelClass::data(const QModelIndex &index, int role) const
{
    return QVariant();
}

QVariant TableModelClass::headerData(int section, Qt::Orientation orientation, int role) const
{
    return QVariant();
}

ComboBoxTalent::ComboBoxTalent(QWidget *parent)
{
    this->setAttribute(Qt::WA_TranslucentBackground);
    // 创建一个阴影效果对象
    QGraphicsDropShadowEffect *shadowEffect = new QGraphicsDropShadowEffect(this);
    shadowEffect->setColor(Qt::black);     // 设置阴影的颜色
    shadowEffect->setBlurRadius(10);       // 设置阴影的模糊半径
    shadowEffect->setOffset(0, 0);         // 设置阴影的偏移量
    this->setGraphicsEffect(shadowEffect); // 为按钮应用阴影效果

    m_pixmap =
        new QPixmap("C:\\Users\\NoWat\\Project\\JX3DPS\\resources\\pics\\JX3\\Talent\\玄门.png");
}

void ComboBoxTalent::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

    int border = 6;
    int width  = this->width() - border * 2;

    // 画一个框
    painter.setPen(QPen(QColor(56, 60, 67)));
    painter.setBrush(QBrush(QColor(43, 46, 50)));
    painter.drawRect(border + 10, 0, width, width);

    // 画一个图形
    if (m_pixmap != nullptr) {
        painter.setPen(QPen(QColor(0, 0, 0)));
        painter.setBrush(QBrush(QColor(0, 0, 0)));
        painter.drawRect(border + 2 + 10, 0 + 2, width - 4, width - 4);
        int imageBorder = 2;
        painter.drawPixmap(border + imageBorder + 10,
                           0 + imageBorder,
                           width - 2 * imageBorder,
                           width - 2 * imageBorder,
                           m_pixmap->copy(3, 3, 42, 42));

        painter.setPen(QPen(COLOR_BLUE));
        QFont font("微软雅黑", 11);

        painter.setFont(font);
        painter.drawText(0, 2, this->fontMetrics().horizontalAdvance("玄"), this->fontMetrics().height(), Qt::AlignCenter, "玄");
        painter.drawText(0, this->fontMetrics().height() + 3, this->fontMetrics().horizontalAdvance("玄"), this->fontMetrics().height(), Qt::AlignCenter, "门");
        painter.drawText(0, 2, this->fontMetrics().horizontalAdvance("玄"), this->fontMetrics().height(), Qt::AlignCenter, "玄");
        painter.drawText(0, this->fontMetrics().height() + 3, this->fontMetrics().horizontalAdvance("玄"), this->fontMetrics().height(), Qt::AlignCenter, "门");
        // painter.drawText(0, width + 3, this->width(), this->height() - width - 3, Qt::AlignCenter, "剑道万象");
    }
}

void ComboBoxTalent::enterEvent(QEnterEvent *event)
{
    this->setCursor(Qt::PointingHandCursor);
}

void ComboBoxTalent::showPopup()
{
    QComboBox::showPopup();
    QWidget *popup = this->findChild<QFrame *>();

    popup->move(popup->x(), popup->y() + 5);
}
