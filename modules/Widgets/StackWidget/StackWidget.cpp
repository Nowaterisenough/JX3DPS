/**
 * Project: JX3DPS
 * File: SpinBox.cpp
 * Description:
 * Created Date: 2023-06-10 08:38:29
 * Author: 难为水
 * -----
 * Last Modified: 2023-08-12 04:36:11
 * Modified By: 难为水
 * -----
 * HISTORY:
 * Date      	By   	Comments
 * ----------	-----	----------------------------------------------------------
 */

#include "StackWidget.h"

#include <QGraphicsDropShadowEffect>
#include <QPainter>
#include <QPropertyAnimation>

#include "Common/ThemeColors.h"

Stack::Stack(QWidget *parent) : QWidget(parent)
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

void Stack::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

    painter.setPen(QPen(QColor(56, 60, 67), 1));
    painter.setBrush(QBrush(QColor(COLOR_BACKGROUND_PRIMARY)));

    painter.drawRect(this->rect());
}

StackButton::StackButton(QWidget *parent) : QPushButton(parent)
{
    this->setAttribute(Qt::WA_TranslucentBackground);
    this->setCheckable(true);

    connect(this, QOverload<bool>::of(&QPushButton::toggled), [this](bool checked) {
        if (checked) {
            m_color = QColor(COLOR_ACTIVE);

            QPropertyAnimation *animation = new QPropertyAnimation(this, "line");
            animation->setDuration(120);
            animation->setStartValue(0);
            int totalHeight = (this->fontMetrics().height() - 2) * this->text().length();
            int lineHeight  = totalHeight / this->text().length();
            animation->setEndValue(lineHeight * this->text().length());
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

void StackButton::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
    painter.setPen(QPen(m_color));
    painter.setFont(QFont(painter.font().family(), painter.font().pointSize()));

    int x = width() / 2;
    int y = height() / 2;

    int totalHeight = (this->fontMetrics().height() - 2) * this->text().length();
    int lineHeight  = totalHeight / this->text().length();
    for (int i = 0; i < this->text().length(); i++) {
        QRect boundingRect(x - painter.fontMetrics().horizontalAdvance(this->text()[i]) / 2,
                           y - totalHeight / 2 + lineHeight * i,
                           painter.fontMetrics().horizontalAdvance(this->text()[i]),
                           lineHeight);
        painter.drawText(boundingRect, Qt::AlignCenter, QString(this->text()[i]));
        painter.drawText(boundingRect, Qt::AlignCenter, QString(this->text()[i]));
    }
    if (this->isChecked()) {
        painter.drawLine(x + painter.fontMetrics().horizontalAdvance(this->text()[0]) / 2 + 5,
                         y - m_line / 2,
                         x + painter.fontMetrics().horizontalAdvance(this->text()[0]) / 2 + 5,
                         y + m_line / 2);
        painter.drawLine(x + painter.fontMetrics().horizontalAdvance(this->text()[0]) / 2 + 5,
                         y - m_line / 2,
                         x + painter.fontMetrics().horizontalAdvance(this->text()[0]) / 2 + 5,
                         y + m_line / 2);
    }
}

QColor StackButton::GetColor() const
{
    return m_color;
}

void StackButton::SetColor(QColor color)
{
    m_color = color;
    update();
}

int StackButton::GetLine() const
{
    return m_line;
}

void StackButton::SetLine(int line)
{
    m_line = line;
    this->update();
}

void StackButton::enterEvent(QEnterEvent *event)
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

void StackButton::leaveEvent(QEvent *event)
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

StackWidget::StackWidget(QWidget *parent) : QWidget(parent)
{
    this->setAttribute(Qt::WA_TranslucentBackground);
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

void StackWidget::AddTab(const QString &text)
{
    StackButton *tabButton = new StackButton(this);
    Stack       *tWidget   = new Stack(this);

    connect(tabButton, &QPushButton::toggled, [=](bool checked) {
        if (checked) {
            for (int i = 0; i < m_stacks.size(); i++) {
                if (m_stacks[i].first == tabButton && !m_flag) {
                    AnimatedResize(i);
                }
            }
        }
    });

    int y = height() / 2;
    tabButton->setFont(QFont("NoWatsFont", 11));
    tabButton->setText(text);
    tabButton->setFixedSize(QFontMetrics(tabButton->font()).horizontalAdvance(text.at(0)) + 10,
                            (QFontMetrics(tabButton->font()).height() - 2) * text.length() + 2);
    tWidget->hide();
    if (m_stacks.empty()) {
        tabButton->setChecked(true);
    }
    m_stacks.emplace_back(QPair<StackButton *, Stack *>(tabButton, tWidget));

    update();
}

Stack *StackWidget::Widget(int index)
{
    return m_stacks[index].second;
}

void StackWidget::resizeEvent(QResizeEvent *event)
{

    Resize();

    QWidget::resizeEvent(event);
}

void StackWidget::paintEvent(QPaintEvent *event)
{
    if (m_currentIndex == m_lastIndex) {
        Resize();
        QWidget::paintEvent(event);
        return;
    }

    int space       = 2;
    int buttonWidth = 25;
    if (!m_stacks.empty()) {
        buttonWidth = m_stacks[0].first->width();
    }

    if (m_currentIndex > m_lastIndex) {

        for (int i = m_lastIndex + 1; i <= m_currentIndex; i++) {
            StackButton *tabButton = m_stacks[i].first;
            tabButton->setGeometry(this->width() - (space + buttonWidth) * (m_stacks.size() - i) + space - m_dx,
                                   tabButton->y(),
                                   tabButton->width(),
                                   tabButton->height());
        }
        Stack *tLastWidget    = m_stacks[m_lastIndex].second;
        Stack *tCurrentWidget = m_stacks[m_currentIndex].second;
        int    width          = this->width() - m_stacks.size() * (buttonWidth + space);
        if (width - m_dx >= 0) {
            tLastWidget->setGeometry(tLastWidget->x(), 0, width - m_dx, this->height());
        }
        if (m_dx > space) {
            tCurrentWidget->setGeometry(m_stacks[m_currentIndex].first->x() + buttonWidth + space,
                                        0,
                                        m_dx - space,
                                        this->height());
            m_stacks[m_currentIndex].second->show();
        }
    } else if (m_currentIndex < m_lastIndex) {
        for (int i = m_currentIndex + 1; i <= m_lastIndex; i++) {
            StackButton *tabButton = m_stacks[i].first;
            Stack       *tWidget   = m_stacks[i].second;
            tabButton->setGeometry((space + buttonWidth) * i + m_dx,
                                   tabButton->y(),
                                   tabButton->width(),
                                   tabButton->height());
        }
        Stack *tLastWidget    = m_stacks[m_lastIndex].second;
        Stack *tCurrentWidget = m_stacks[m_currentIndex].second;
        int    width          = this->width() - m_stacks.size() * (buttonWidth + space);
        if (width - m_dx >= 0) {
            tLastWidget->setGeometry(m_stacks[m_lastIndex].first->x() + buttonWidth + space,
                                     0,
                                     width - m_dx,
                                     this->height());
        }
        if (m_dx > space) {
            tCurrentWidget->setGeometry(m_stacks[m_currentIndex].first->x() +
                                            m_stacks[m_currentIndex].first->width() + space,
                                        0,
                                        m_dx - space,
                                        this->height());
            m_stacks[m_currentIndex].second->show();
        }
    }

    QWidget::paintEvent(event);
}

void StackWidget::Resize()
{
    bool checked     = false;
    int  y           = height() / 2;
    int  space       = 2;
    int  buttonWidth = 25;
    if (!m_stacks.empty()) {
        buttonWidth = m_stacks[0].first->width();
    }
    for (int i = 0; i < m_stacks.size(); ++i) {
        StackButton *tabButton = m_stacks[i].first;
        Stack       *tWidget   = m_stacks[i].second;
        if (checked) {
            tabButton->setGeometry(this->width() - (space + buttonWidth) * (m_stacks.size() - i) + space,
                                   y - tabButton->height() / 2,
                                   tabButton->width(),
                                   tabButton->height());
        } else {
            tabButton->setGeometry((space + buttonWidth) * i,
                                   y - tabButton->height() / 2,
                                   tabButton->width(),
                                   tabButton->height());
            if (tabButton->isChecked()) {
                int x = (space + buttonWidth) * i;
                tWidget->setGeometry((space + buttonWidth) * (i + 1),
                                     0,
                                     this->width() - x - (space + buttonWidth) * (m_stacks.size() - i),
                                     this->height());
                tWidget->show();
                checked = true;
            }
        }
        tabButton->show();
    }
    update();
}

void StackWidget::AnimatedResize(int index)
{
    if (index == m_currentIndex) {
        return;
    }
    m_flag          = true;
    int space       = 2;
    int buttonWidth = m_stacks[0].first->width();
    int dx          = this->width() - m_stacks.size() * (buttonWidth + space) + space;
    m_currentIndex  = index;
    m_stacks[m_lastIndex].first->setChecked(false);

    QPropertyAnimation *animation = new QPropertyAnimation(this, "dx");

    connect(animation, QOverload<>::of(&QPropertyAnimation::finished), [=, this]() {
        for (int i = 0; i < m_stacks.size(); ++i) {
            StackButton *tabButton = m_stacks[i].first;
            Stack       *tWidget   = m_stacks[i].second;
            if (i != index) {
                tabButton->setChecked(false);
                tWidget->hide();
            }
        }
        m_currentIndex = m_lastIndex = index;
        m_dx                         = 0;
        m_flag                       = false;
    });

    animation->setDuration(400);
    animation->setStartValue(0);
    animation->setEndValue(dx);
    animation->setEasingCurve(QEasingCurve::InOutQuad);
    animation->start(QAbstractAnimation::DeleteWhenStopped);
}

void StackWidget::Clear()
{
    for (auto &[button, widget] : m_stacks) {
        delete button;
        delete widget;
    }
    m_stacks.clear();
    m_currentIndex = 0;
    m_lastIndex    = 0;
    m_dx           = 0;
    m_flag         = false;
}

QList<QPair<StackButton*,Stack*>>& StackWidget::Stacks()
{
    return m_stacks;
}

int StackWidget::GetDx() const
{
    return m_dx;
}

void StackWidget::SetDx(int dx)
{
    m_dx = dx;
    this->update();
}
