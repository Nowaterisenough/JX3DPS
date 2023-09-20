/**
 * @Description :
 * @Author      : NoWats
 * @Date        : 2023-04-08 02:23:52
 * @Update      : NoWats
 * @LastTime    : 2023-04-13 02:29:07
 * @FilePath    : \LegionTDSim\apps\LegionTDSim\Widget.cpp
 */

#include "Widget.h"

#include <QLayout>
#include <QPainter>
#include <QPropertyAnimation>

Title::Title(QWidget *parent) : QPushButton(parent)
{
    this->setAttribute(Qt::WA_TranslucentBackground);
}

QColor Title::GetColor() const
{
    return m_color;
}

void Title::SetColor(QColor color)
{
    m_color = color;
    this->update();
}

void Title::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
    painter.setPen(QPen(m_color));
    painter.setFont(QFont(painter.font().family(), painter.font().pointSize()));
    painter.drawText(this->rect(), Qt::AlignCenter, this->text());
    painter.drawText(this->rect(), Qt::AlignCenter, this->text());
}

void Title::enterEvent(QEnterEvent *event)
{
    QPropertyAnimation *animation = new QPropertyAnimation(this, "color");
    animation->setDuration(150);
    animation->setStartValue(m_color);
    animation->setEndValue(QColor(COLOR_FOCUS));
    animation->setEasingCurve(QEasingCurve::InOutQuad);
    animation->start(QAbstractAnimation::DeleteWhenStopped);
    this->setCursor(Qt::PointingHandCursor);
}

void Title::leaveEvent(QEvent *event)
{
    QPropertyAnimation *animation = new QPropertyAnimation(this, "color");
    animation->setDuration(200);
    animation->setStartValue(m_color);
    animation->setEndValue(QColor(COLOR_INACTIVE));
    animation->setEasingCurve(QEasingCurve::InOutQuad);
    animation->start(QAbstractAnimation::DeleteWhenStopped);
}

CloseButton::CloseButton(QWidget *parent) : QPushButton(parent)
{
    this->setAttribute(Qt::WA_TranslucentBackground);
}

QColor CloseButton::GetColor() const
{
    return m_color;
}

void CloseButton::SetColor(QColor color)
{
    m_color = color;
    this->update();
}

QColor CloseButton::GetTextColor() const
{
    return m_textColor;
}

void CloseButton::SetTextColor(QColor color)
{
    m_textColor = color;
    this->update();
}

void CloseButton::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

    painter.setBrush(QBrush(m_color));
    painter.setPen(QColor(64, 64, 64));

    // 画一个只有右上角为(10, 10)的圆角矩形
    painter.drawRoundedRect(this->rect(), 10, 10);
    painter.setPen(QPen(m_color));
    painter.drawRect(this->rect().x(),
                     this->rect().y(),
                     this->rect().width() - 10,
                     this->rect().height() - 10);
    painter.drawRect(this->rect().x(),
                     this->rect().y() + 10,
                     this->rect().width(),
                     this->rect().height() - 10);
    painter.setPen(QColor(64, 64, 64));
    painter.drawLine(this->rect().x(),
                     this->rect().y(),
                     this->rect().width() - 10,
                     this->rect().y());
    painter.drawLine(this->rect().x() + this->rect().width(),
                     this->rect().y() + 10,
                     this->rect().x() + this->rect().width(),
                     this->rect().y() + this->rect().height());

    painter.setPen(QPen(m_textColor, 1.5));
    painter.setBrush(m_textColor);

    // 在正中心画一个X
    painter.drawLine(this->rect().center().x() - 4,
                     this->rect().center().y() - 4,
                     this->rect().center().x() + 4,
                     this->rect().center().y() + 4);
    painter.drawLine(this->rect().center().x() - 4,
                     this->rect().center().y() + 4,
                     this->rect().center().x() + 4,
                     this->rect().center().y() - 4);
}

void CloseButton::enterEvent(QEnterEvent *event)
{
    QPropertyAnimation *animation = new QPropertyAnimation(this, "color");
    animation->setDuration(150);
    animation->setStartValue(m_color);
    animation->setEndValue(QColor(226, 42, 39));
    animation->setEasingCurve(QEasingCurve::InOutQuad);

    QPropertyAnimation *animation2 = new QPropertyAnimation(this, "textColor");
    animation2->setDuration(150);
    animation2->setStartValue(m_textColor);
    animation2->setEndValue(QColor(COLOR_FOCUS));
    animation2->setEasingCurve(QEasingCurve::InOutQuad);

    animation->start(QAbstractAnimation::DeleteWhenStopped);
    animation2->start(QAbstractAnimation::DeleteWhenStopped);
}

void CloseButton::leaveEvent(QEvent *event)
{
    QPropertyAnimation *animation = new QPropertyAnimation(this, "color");
    animation->setDuration(200);
    animation->setStartValue(m_color);
    animation->setEndValue(QColor(COLOR_BACKGROUND_BASE));
    animation->setEasingCurve(QEasingCurve::InOutQuad);

    QPropertyAnimation *animation2 = new QPropertyAnimation(this, "textColor");
    animation2->setDuration(150);
    animation2->setStartValue(m_textColor);
    animation2->setEndValue(QColor(COLOR_INACTIVE));
    animation2->setEasingCurve(QEasingCurve::InOutQuad);

    animation->start(QAbstractAnimation::DeleteWhenStopped);
    animation2->start(QAbstractAnimation::DeleteWhenStopped);
}

Widget::Widget(QWidget *parent) : QWidget(parent)
{
    m_frameless = new FramelessWidget(this);
    m_frameless->SetWidget(this);

    this->setWindowFlags(Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_TranslucentBackground, true);

    QGridLayout *layout = new QGridLayout(this);

    m_title = new Title(this);

    CloseButton *closeButton = new CloseButton(this);
    closeButton->setFixedSize(30, 30);
    closeButton->setToolTip("关闭");
    connect(closeButton, &QPushButton::clicked, this, [=] {
        this->close();
        closeButton->SetColor(QColor(COLOR_BACKGROUND_BASE));
    });

    centralWidget = new QWidget(this);

    QSpacerItem *horizontalSpacer1 = new QSpacerItem(2, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);
    QSpacerItem *horizontalSpacer2 =
        new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(5);

    layout->addItem(horizontalSpacer1, 0, 0, 1, 1);
    layout->addWidget(m_title, 0, 1, 1, 1);
    layout->addItem(horizontalSpacer2, 0, 2, 1, 1);
    layout->addWidget(closeButton, 0, 3, 1, 1);
    layout->addWidget(centralWidget, 1, 0, 1, 4);
}

void Widget::SetTitle(const QString &title)
{
    m_title->setText(title);
    m_title->setFont(QFont("微软雅黑"));
    m_title->setFixedWidth(QFontMetrics(m_title->font()).horizontalAdvance(title));
    m_title->update();
}

void Widget::SetMousePressed(bool press)
{
    m_frameless->SetMousePressed(press);
}

void Widget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(QColor(64, 64, 64));
    painter.setBrush(QColor(COLOR_BACKGROUND_BASE));
    painter.drawRoundedRect(this->rect(), 10, 10);
}
