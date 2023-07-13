/**
 * Project: JX3DPS
 * File: SpinBox.cpp
 * Description:
 * Created Date: 2023-06-10 08:38:29
 * Author: 难为水
 * -----
 * Last Modified: 2023-07-13 21:35:53
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
    TWidget   *tWidget   = new TWidget(this);
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
    m_tabs.emplace_back(QPair<TabButton *, TWidget *>(tabButton, tWidget));
}

TWidget *TabWidget::Widget(int index)
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

TWidget::TWidget(QWidget *parent) : QWidget(parent)
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

void TWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

    painter.setPen(QPen(QColor(56, 60, 67), 1));
    painter.setBrush(QBrush(QColor(COLOR_BACKGROUND_PRIMARY)));

    painter.drawRect(this->rect());
}

int VTabButton::s_width = 0;

VTabButton::VTabButton(QWidget *parent) : QPushButton(parent)
{
    this->setAttribute(Qt::WA_TranslucentBackground);
    this->setCheckable(true);

    connect(this, QOverload<bool>::of(&QPushButton::toggled), [this](bool checked) {
        if (checked) {
            m_color = QColor(COLOR_ACTIVE);

            QPropertyAnimation *animation = new QPropertyAnimation(this, "line");
            animation->setDuration(120);
            animation->setStartValue(0);
            animation->setEndValue(this->fontMetrics().height());
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

QColor VTabButton::GetColor() const
{
    return m_color;
}

void VTabButton::SetColor(QColor color)
{
    m_color = color;
    update();
}

int VTabButton::GetLine() const
{
    return m_line;
}

void VTabButton::SetLine(int line)
{
    m_line = line;
    this->update();
}

void VTabButton::paintEvent(QPaintEvent *event)
{

    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
    painter.setPen(QPen(m_color));
    painter.setFont(QFont(painter.font().family(), painter.font().pointSize()));

    painter.drawText(this->rect().x(),
                     this->rect().y(),
                     this->fontMetrics().horizontalAdvance(this->text()),
                     this->rect().height(),
                     Qt::AlignCenter,
                     this->text());
    painter.drawText(this->rect().x(),
                     this->rect().y(),
                     this->fontMetrics().horizontalAdvance(this->text()),
                     this->rect().height(),
                     Qt::AlignCenter,
                     this->text());

    if (this->isChecked()) {
        painter.setPen(QPen(QColor(COLOR_ACTIVE), 1.5));
        painter.setBrush(QBrush(QColor(COLOR_ACTIVE)));
        // 画条文字下的横线
        painter.drawLine(this->rect().topRight().x(),
                         this->rect().topRight().y() + (this->rect().height() - m_line) / 2,
                         this->rect().bottomRight().x(),
                         this->rect().bottomRight().y() - (this->rect().height() - m_line) / 2);
        painter.drawLine(this->rect().topRight().x(),
                         this->rect().topRight().y() + (this->rect().height() - m_line) / 2,
                         this->rect().bottomRight().x(),
                         this->rect().bottomRight().y() - (this->rect().height() - m_line) / 2);
    }

    QWidget::paintEvent(event);
}

void VTabButton::enterEvent(QEnterEvent *event)
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

void VTabButton::leaveEvent(QEvent *event)
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

VTabWidget::VTabWidget(QWidget *parent) : QWidget(parent)
{
    this->setAttribute(Qt::WA_TranslucentBackground);
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

void VTabWidget::AddTab(const QString &text)
{
    VTabButton *tabButton = new VTabButton(this);
    TWidget    *tWidget   = new TWidget(this);
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

    QFont font("NoWatsFont", 11);
    tabButton->setFont(font);
    tabButton->setText(text);
    int width = QFontMetrics(font).horizontalAdvance(text);

    if (m_tabs.empty()) {
        tabButton->setGeometry(0,
                               0,
                               QFontMetrics(font).horizontalAdvance(text) + 5,
                               QFontMetrics(font).height());
        tabButton->setChecked(true);
    } else {
        VTabButton::s_width = VTabButton::s_width > m_tabs.last().first->rect().width()
                                  ? VTabButton::s_width
                                  : m_tabs.last().first->rect().width();

        tabButton->setGeometry(VTabButton::s_width - QFontMetrics(font).horizontalAdvance(text) - 5,
                               m_tabs.last().first->y() + m_tabs.last().first->height() + 6,
                               QFontMetrics(font).horizontalAdvance(text) + 5,
                               QFontMetrics(font).height());
    }
    m_tabs.emplace_back(QPair<VTabButton *, TWidget *>(tabButton, tWidget));
}

TWidget *VTabWidget::Widget(int index)
{
    if (index >= 0 && index < m_tabs.size()) {
        return m_tabs[index].second;
    }
    return nullptr;
}

void VTabWidget::resizeEvent(QResizeEvent *event)
{
    for (auto &tab : m_tabs) {
        QFont font("NoWatsFont", 11);
        int   width = tab.first->x() + tab.first->rect().width();
        tab.second->setGeometry(width + 3, 0, this->width() - width - 3, this->height());
    }
}

VerticalTabButton::VerticalTabButton(QWidget *parent) : QPushButton(parent)
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

void VerticalTabButton::paintEvent(QPaintEvent *event)
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

QColor VerticalTabButton::GetColor() const
{
    return m_color;
}

void VerticalTabButton::SetColor(QColor color)
{
    m_color = color;
    update();
}

int VerticalTabButton::GetLine() const
{
    return m_line;
}

void VerticalTabButton::SetLine(int line)
{
    m_line = line;
    this->update();
}

void VerticalTabButton::enterEvent(QEnterEvent *event)
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

void VerticalTabButton::leaveEvent(QEvent *event)
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

VerticalTabWidget::VerticalTabWidget(QWidget *parent) : QWidget(parent)
{
    this->setAttribute(Qt::WA_TranslucentBackground);
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

void VerticalTabWidget::AddTab(const QString &text)
{
    VerticalTabButton *tabButton = new VerticalTabButton(this);
    TWidget           *tWidget   = new TWidget(this);

    connect(tabButton, &QPushButton::toggled, [=](bool checked) {
        if (checked) {
            for (int i = 0; i < m_tabs.size(); i++) {
                if (m_tabs[i].first == tabButton && !m_flag) {
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
    if (m_tabs.empty()) {
        tabButton->setChecked(true);
    }
    m_tabs.emplace_back(QPair<VerticalTabButton *, TWidget *>(tabButton, tWidget));

    update();
}

TWidget *VerticalTabWidget::Widget(int index)
{
    return m_tabs[index].second;
}

void VerticalTabWidget::resizeEvent(QResizeEvent *event)
{

    Resize();

    QWidget::resizeEvent(event);
}

void VerticalTabWidget::paintEvent(QPaintEvent *event)
{
    if (m_currentIndex == m_lastIndex) {
        Resize();
        QWidget::paintEvent(event);
        return;
    }

    int space       = 2;
    int buttonWidth = 25;
    if (!m_tabs.empty()) {
        buttonWidth = m_tabs[0].first->width();
    }

    if (m_currentIndex > m_lastIndex) {

        for (int i = m_lastIndex + 1; i <= m_currentIndex; i++) {
            VerticalTabButton *tabButton = m_tabs[i].first;
            tabButton->setGeometry(this->width() - (space + buttonWidth) * (m_tabs.size() - i) + space - m_dx,
                                   tabButton->y(),
                                   tabButton->width(),
                                   tabButton->height());
        }
        TWidget *tLastWidget    = m_tabs[m_lastIndex].second;
        TWidget *tCurrentWidget = m_tabs[m_currentIndex].second;
        int      width          = this->width() - m_tabs.size() * (buttonWidth + space);
        if (width - m_dx >= 0) {
            tLastWidget->setGeometry(tLastWidget->x(), 0, width - m_dx, this->height());
        }
        if (m_dx > space) {
            tCurrentWidget->setGeometry(m_tabs[m_currentIndex].first->x() + buttonWidth + space, 0, m_dx - space, this->height());
            m_tabs[m_currentIndex].second->show();
        }
    } else if (m_currentIndex < m_lastIndex) {
        for (int i = m_currentIndex + 1; i <= m_lastIndex; i++) {
            VerticalTabButton *tabButton = m_tabs[i].first;
            TWidget           *tWidget   = m_tabs[i].second;
            tabButton->setGeometry((space + buttonWidth) * i + m_dx,
                                   tabButton->y(),
                                   tabButton->width(),
                                   tabButton->height());
        }
        TWidget *tLastWidget    = m_tabs[m_lastIndex].second;
        TWidget *tCurrentWidget = m_tabs[m_currentIndex].second;
        int      width          = this->width() - m_tabs.size() * (buttonWidth + space);
        if (width - m_dx >= 0) {
            tLastWidget->setGeometry(m_tabs[m_lastIndex].first->x() + buttonWidth + space, 0, width - m_dx, this->height());
        }
        if (m_dx > space) {
            tCurrentWidget->setGeometry(
                m_tabs[m_currentIndex].first->x() + m_tabs[m_currentIndex].first->width() + space,
                0,
                m_dx - space,
                this->height());
            m_tabs[m_currentIndex].second->show();
        }
    }

    QWidget::paintEvent(event);
}

void VerticalTabWidget::Resize()
{
    bool checked     = false;
    int  y           = height() / 2;
    int  space       = 2;
    int  buttonWidth = 25;
    if (!m_tabs.empty()) {
        buttonWidth = m_tabs[0].first->width();
    }
    for (int i = 0; i < m_tabs.size(); ++i) {
        VerticalTabButton *tabButton = m_tabs[i].first;
        TWidget           *tWidget   = m_tabs[i].second;
        if (checked) {
            tabButton->setGeometry(this->width() - (space + buttonWidth) * (m_tabs.size() - i) + space,
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
                                     this->width() - x - (space + buttonWidth) * (m_tabs.size() - i),
                                     this->height());
                tWidget->show();
                checked = true;
            }
        }
        tabButton->show();
    }
    update();
}

void VerticalTabWidget::AnimatedResize(int index)
{
    if (index == m_currentIndex) {
        return;
    }
    m_flag          = true;
    int space       = 2;
    int buttonWidth = m_tabs[0].first->width();
    int dx          = this->width() - m_tabs.size() * (buttonWidth + space) + space;
    m_currentIndex  = index;
    m_tabs[m_lastIndex].first->setChecked(false);

    QPropertyAnimation *animation = new QPropertyAnimation(this, "dx");

    connect(animation, QOverload<>::of(&QPropertyAnimation::finished), [=, this]() {
        for (int i = 0; i < m_tabs.size(); ++i) {
            VerticalTabButton *tabButton = m_tabs[i].first;
            TWidget           *tWidget   = m_tabs[i].second;
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

int VerticalTabWidget::GetDx() const
{
    return m_dx;
}

void VerticalTabWidget::SetDx(int dx)
{
    m_dx = dx;
    this->update();
}
