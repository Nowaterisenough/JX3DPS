/**
 * Project: 
 * File: SpinBox.cpp
 * Description:
 * Created Date: 2023-06-10 08:38:29
 * Author: 难为水
 * -----
 * Last Modified: 2023-07-19 07:28:35
 * Modified By: 难为水
 * -----
 * HISTORY:
 * Date      	By   	Comments
 * ----------	-----	----------------------------------------------------------
 */

#include "TabWidget.h"

#include <QGraphicsDropShadowEffect>
#include <QPainter>
#include <QPropertyAnimation>

#include "ThemeColors.h"

TabWidget::TabWidget(QWidget *parent) : QWidget(parent)
{
    this->setAttribute(Qt::WA_TranslucentBackground);
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

void TabWidget::AddTab(const QString &text)
{
    TabButton *tabButton = new TabButton(this);
    Tab   *tWidget   = new Tab(this);
    tWidget->hide();
    connect(tabButton, &QPushButton::toggled, [=](bool checked) {
        if (checked) {
            for (auto &tab : m_tabs) {
                if (tab.first != tabButton) {
                    tab.first->setChecked(false);
                    tab.second->hide();
                }
            }
            tWidget->show();
        }
    });

    tabButton->setText(text);
    QFont font("NoWatsFont", 11);
    tabButton->setFont(font);
    int width = QFontMetrics(font).horizontalAdvance(text);
    if (m_tabs.empty()) {
        tabButton->setGeometry(15, 0, width, 25);
        tabButton->setChecked(true);
    } else {
        tabButton->setGeometry(m_tabs.last().first->x() + m_tabs.last().first->width() + 10, 0, width, 25);
    }
    m_tabs.emplace_back(QPair<TabButton *, Tab *>(tabButton, tWidget));
}

Tab *TabWidget::Widget(int index)
{
    if (index >= 0 && index < m_tabs.size()) {
        return m_tabs[index].second;
    }
    return nullptr;
}

void TabWidget::resizeEvent(QResizeEvent *event)
{
    for (auto &tab : m_tabs) {
        tab.second->setGeometry(0, 30, this->width(), this->height() - 30);
    }
}

TabButton::TabButton(QWidget *parent) : QPushButton(parent)
{
    this->setAttribute(Qt::WA_TranslucentBackground);
    this->setCheckable(true);

    m_color = QColor(COLOR_HIGHLIGHT);
    
    connect(this, QOverload<bool>::of(&QPushButton::toggled), [this](bool checked) {
        if (checked) {
            m_color = QColor(COLOR_ACTIVE);

            QPropertyAnimation *animation = new QPropertyAnimation(this, "line");
            animation->setDuration(120);
            animation->setStartValue(0);
            animation->setEndValue(this->fontMetrics().horizontalAdvance(this->text()));
            animation->setEasingCurve(QEasingCurve::OutQuart);
            animation->start(QAbstractAnimation::DeleteWhenStopped);
        } else {
            m_color = QColor(COLOR_HIGHLIGHT);
            m_line  = 0;
        }
        update();
    });

    connect(this, QOverload<bool>::of(&QPushButton::clicked), [this](bool checked) {
        this->setChecked(true);
    });
}

QColor TabButton::GetColor() const
{
    return m_color;
}

void TabButton::SetColor(QColor color)
{
    m_color = color;
    this->update();
}

int TabButton::GetLine() const
{
    return m_line;
}

void TabButton::SetLine(int line)
{
    m_line = line;
    this->update();
}

void TabButton::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
    painter.setPen(QPen(m_color));
    painter.setFont(QFont(painter.font().family(), painter.font().pointSize()));
    painter.drawText(this->rect().x(),
                     this->rect().y(),
                     this->rect().width(),
                     this->fontMetrics().height(),
                     Qt::AlignCenter,
                     this->text());
    painter.drawText(this->rect().x(),
                     this->rect().y(),
                     this->rect().width(),
                     this->fontMetrics().height(),
                     Qt::AlignCenter,
                     this->text());

    if (this->isChecked()) {
        painter.setPen(QPen(QColor(COLOR_ACTIVE), 2.5));
        painter.setBrush(QBrush(QColor(COLOR_ACTIVE)));
        // 画条文字下的横线
        painter.drawLine(this->rect().bottomLeft().x() + (this->rect().width() - m_line) / 2,
                         this->rect().bottomLeft().y(),
                         this->rect().bottomRight().x() - (this->rect().width() - m_line) / 2,
                         this->rect().bottomLeft().y());
        painter.drawLine(this->rect().bottomLeft().x() + (this->rect().width() - m_line) / 2,
                         this->rect().bottomLeft().y(),
                         this->rect().bottomRight().x() - (this->rect().width() - m_line) / 2,
                         this->rect().bottomLeft().y());
    }
    QWidget::paintEvent(event);
}

void TabButton::enterEvent(QEnterEvent *event)
{
    if (this->isChecked()) {
        return;
    }
    QPropertyAnimation *animation = new QPropertyAnimation(this, "color");
    animation->setDuration(150);
    animation->setStartValue(m_color);
    animation->setEndValue(QColor(COLOR_FOCUS));
    animation->setEasingCurve(QEasingCurve::InOutQuad);
    animation->start(QAbstractAnimation::DeleteWhenStopped);
    this->setCursor(Qt::PointingHandCursor);
}

void TabButton::leaveEvent(QEvent *event)
{
    if (this->isChecked()) {
        return;
    }
    QPropertyAnimation *animation = new QPropertyAnimation(this, "color");
    animation->setDuration(200);
    animation->setStartValue(m_color);
    animation->setEndValue(QColor(COLOR_HIGHLIGHT));
    animation->setEasingCurve(QEasingCurve::InOutQuad);
    animation->start(QAbstractAnimation::DeleteWhenStopped);
}

Tab::Tab(QWidget *parent) : QWidget(parent)
{
    this->setAttribute(Qt::WA_TranslucentBackground);
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    // 创建一个阴影效果对象
    QGraphicsDropShadowEffect *shadowEffect = new QGraphicsDropShadowEffect(this);
    shadowEffect->setColor(Qt::black);     // 设置阴影的颜色
    shadowEffect->setBlurRadius(5);        // 设置阴影的模糊半径
    shadowEffect->setOffset(0, 0);         // 设置阴影的偏移量
    this->setGraphicsEffect(shadowEffect); // 为按钮应用阴影效果
}

void Tab::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

    painter.setPen(QPen(QColor(56, 60, 67), 1));
    painter.setBrush(QBrush(QColor(COLOR_BACKGROUND_PRIMARY)));

    painter.drawRect(this->rect());
}