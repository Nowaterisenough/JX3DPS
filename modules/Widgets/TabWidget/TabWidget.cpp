/**
 * Project: JX3DPS
 * File: SpinBox.cpp
 * Description:
 * Created Date: 2023-06-10 08:38:29
 * Author: 难为水
 * -----
 * Last Modified: 2023-08-07 00:39:31
 * Modified By: 难为水
 * -----
 * HISTORY:
 * Date      	By   	Comments
 * ----------	-----	----------------------------------------------------------
 */

#include "TabWidget.h"

#include <QGraphicsDropShadowEffect>
#include <QLayout>
#include <QMouseEvent>
#include <QPainter>
#include <QPropertyAnimation>

Button::Button(QWidget *parent) : QPushButton(parent)
{
    this->setAttribute(Qt::WA_TranslucentBackground);
}

QColor Button::GetColor() const
{
    return m_color;
}

void Button::SetColor(QColor color)
{
    m_color = color;
    this->update();
}

QColor Button::GetTextColor() const
{
    return m_textColor;
}

void Button::SetTextColor(QColor color)
{
    m_textColor = color;
    this->update();
}

void Button::SetButtonColor(const QColor &hover, const QColor &normal)
{
    m_color = m_normalColor = normal;
    m_hoverColor            = hover;
    this->update();
}

void Button::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

    painter.setBrush(QBrush(m_color));
    painter.setPen(QPen(m_color));
    painter.drawRoundedRect(this->rect(), 1, 1);

    painter.setPen(QPen(m_textColor));
    painter.setFont(QFont(painter.font().family(), painter.font().pointSize()));
    painter.drawText(this->rect(), Qt::AlignCenter, this->text());
    painter.drawText(this->rect(), Qt::AlignCenter, this->text());
}

void Button::enterEvent(QEnterEvent *event)
{
    QPropertyAnimation *animation = new QPropertyAnimation(this, "color");
    animation->setDuration(150);
    animation->setStartValue(m_color);
    animation->setEndValue(m_hoverColor);
    animation->setEasingCurve(QEasingCurve::InOutQuad);

    QPropertyAnimation *animation2 = new QPropertyAnimation(this, "textColor");
    animation2->setDuration(150);
    animation2->setStartValue(m_textColor);
    animation2->setEndValue(QColor(COLOR_FOCUS));
    animation2->setEasingCurve(QEasingCurve::InOutQuad);

    animation->start(QAbstractAnimation::DeleteWhenStopped);
    animation2->start(QAbstractAnimation::DeleteWhenStopped);

    this->setCursor(Qt::PointingHandCursor);
}

void Button::leaveEvent(QEvent *event)
{
    QPropertyAnimation *animation = new QPropertyAnimation(this, "color");
    animation->setDuration(200);
    animation->setStartValue(m_color);
    animation->setEndValue(m_normalColor);
    animation->setEasingCurve(QEasingCurve::InOutQuad);

    QPropertyAnimation *animation2 = new QPropertyAnimation(this, "textColor");
    animation2->setDuration(150);
    animation2->setStartValue(m_textColor);
    animation2->setEndValue(QColor(193, 255, 203));
    animation2->setEasingCurve(QEasingCurve::InOutQuad);

    animation->start(QAbstractAnimation::DeleteWhenStopped);
    animation2->start(QAbstractAnimation::DeleteWhenStopped);
}

RenameLineEdit::RenameLineEdit(QWidget *parent) : QLineEdit(parent)
{
    this->setAttribute(Qt::WA_TranslucentBackground);
    // 创建一个阴影效果对象
    QGraphicsDropShadowEffect *shadowEffect = new QGraphicsDropShadowEffect(this);
    shadowEffect->setColor(Qt::black);     // 设置阴影的颜色
    shadowEffect->setBlurRadius(5);        // 设置阴影的模糊半径
    shadowEffect->setOffset(0, 0);         // 设置阴影的偏移量
    this->setGraphicsEffect(shadowEffect); // 为按钮应用阴影效果

    this->setStyleSheet(
        QString("QLineEdit{"
                "background-color: rgb(%1);"
                "border: none;"
                "color: rgb(%2);"
                "selection-background-color: rgb(56, 60, 67);"
                "selection-color: rgb(%3);"
                "font-size: 11pt;"
                "}"
                "QLineEdit::hover{"
                "background-color: rgb(35, 38, 46);"
                "}"
                "QLineEdit::focus{"
                "background-color: rgb(35, 38, 46);"
                "}"
                "QLineEdit::disabled{"
                "background-color: rgb(%4);"
                "color: rgb(%5);"
                "}")
            .arg(TO_STR(COLOR_BACKGROUND_HIGHLIGHT))
            .arg(TO_STR(COLOR_FOCUS))
            .arg(TO_STR(COLOR_FOCUS))
            .arg(TO_STR(COLOR_BACKGROUND_HIGHLIGHT))
            .arg(TO_STR(COLOR_HIGHLIGHT)));
}

RenameWidget::RenameWidget(QWidget *parent) : Widget(parent)
{
    QGridLayout *layout = new QGridLayout(centralWidget);

    m_lineEdit           = new RenameLineEdit(centralWidget);
    Button *buttonOk     = new Button(centralWidget);
    Button *buttonDelete = new Button(centralWidget);
    QSpacerItem *spacer = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding);

    buttonOk->setText("确定");
    buttonDelete->setText("删除");

    layout->setContentsMargins(10, 10, 10, 10);
    layout->setSpacing(10);

    layout->addWidget(m_lineEdit, 0, 0, 1, 3);
    layout->addItem(spacer, 1, 0, 1, 1);
    layout->addWidget(buttonOk, 1, 1, 1, 1);
    layout->addWidget(buttonDelete, 1, 2, 1, 1);

    connect(buttonOk, &QPushButton::clicked, [=]() {
        emit Signal_Rename(m_lineEdit->text());
    });

    connect(buttonDelete, &QPushButton::clicked, [=]() { emit Signal_Delete(); });
}

void RenameWidget::SetText(const QString &text)
{
    m_lineEdit->setText(text);
}

TabWidget::TabWidget(QWidget *parent) : QWidget(parent)
{
    this->setAttribute(Qt::WA_TranslucentBackground);
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    m_widget = new QWidget(this);
    m_widget->setAttribute(Qt::WA_TranslucentBackground);

    m_tabButton = new TabButton(this);
    m_tabButton->setText("+");
    m_tabButton->setFont(QFont(m_tabButton->font().family(), 16));
    m_tabButton->show();

    connect(m_tabButton, &QPushButton::clicked, [=, this]() {
        AddTab(QString("新标签页%1").arg(m_tabs.size() + 1));
        m_tabButton->setChecked(false);
    });
}

void TabWidget::AddTab(const QString &text)
{
    TabButton *tabButton = new TabButton(m_widget);
    Tab       *tab       = new Tab(this);

    tabButton->setText(text);
    tabButton->setFont(QFont(tabButton->font().family(), 11));
    tabButton->show();

    tab->hide();
    m_tabs.emplace_back(QPair<TabButton *, Tab *>(tabButton, tab));

    connect(tabButton, &QPushButton::toggled, [=](bool checked) {
        if (checked) {
            tab->show();

            for (auto &[button, tWidget] : m_tabs) {
                if (button != tabButton) {
                    button->setChecked(false);
                    tWidget->hide();
                }
            }
        }
    });

    connect(tabButton, &TabButton::Signal_Delete, this, [=] {
        for (auto &[button, tab] : m_tabs) {
            if (button == tabButton) {

                tab->deleteLater();
                button->deleteLater();
                m_tabs.removeOne(QPair<TabButton *, Tab *>(button, tab));
                break;
            }
        }
    });

    if (m_tabs.size() == 1) {
        tabButton->setChecked(true);
    }
}

Tab *TabWidget::Widget(int index)
{
    if (index >= 0 && index < m_tabs.size()) {
        return m_tabs[index].second;
    }
    return nullptr;
}

#include <set>

void TabWidget::paintEvent(QPaintEvent *event)
{
    if (m_tabs.size() == 0) {
        m_tabButton->setGeometry(5, -5, 12, 30);
        return;
    }
    int d = 32;

    int index = 0;
    for (auto &[button, tWidget] : m_tabs) {
        tWidget->setGeometry(0, 30, this->width(), this->height() - 30);
        if (button->isChecked()) {
            break;
        }
        index++;
    }
    if (index == m_tabs.size()) {
        index = 0;
        m_tabs[0].first->setChecked(true);
    }

    std::set<int> sets;
    sets.insert(index);
    int width =
        QFontMetrics(m_tabs[index].first->font()).horizontalAdvance(m_tabs[index].first->text()) + 5;
    int left  = index - 1;
    int right = index + 1;

    while (width < this->width() - d && (left >= 0 || right < m_tabs.size())) {
        if (left >= 0) {
            width += QFontMetrics(m_tabs[left].first->font())
                         .horizontalAdvance(m_tabs[left].first->text());
            width += 10;
            sets.insert(left);
            left--;
        }
        if (right < m_tabs.size() && width < this->width() - d) {
            width += QFontMetrics(m_tabs[right].first->font())
                         .horizontalAdvance(m_tabs[right].first->text());
            width += 10;
            sets.insert(right);
            right++;
        }
    }

    int dx = width - (this->width() - d);
    if (dx > 0) {
        m_widget->setFixedWidth(this->width() - d);
        for (int i = 0; i < *sets.begin(); i++) {
            m_tabs[i].first->hide();
        }

        for (int i = *sets.rbegin() + 1; i < m_tabs.size(); i++) {
            m_tabs[i].first->hide();
        }

        if (*sets.rbegin() != index) {
            int x = 5;
            for (int i = *sets.begin(); i < *sets.rbegin(); i++) {
                m_tabs[i].first->show();
                m_tabs[i].first->setGeometry(x,
                                             0,
                                             QFontMetrics(m_tabs[i].first->font())
                                                 .horizontalAdvance(m_tabs[i].first->text()),
                                             25);
                x += QFontMetrics(m_tabs[i].first->font())
                         .horizontalAdvance(m_tabs[i].first->text()) +
                     10;
            }
            m_tabs[*sets.rbegin()].first->show();
            m_tabs[*sets.rbegin()].first->setGeometry(
                x,
                0,
                QFontMetrics(m_tabs[*sets.rbegin()].first->font())
                        .horizontalAdvance(m_tabs[*sets.rbegin()].first->text()) -
                    dx,
                25);
        } else {
            int x = 5;
            m_tabs[*sets.begin()].first->show();
            m_tabs[*sets.begin()].first->setGeometry(
                x,
                0,
                QFontMetrics(m_tabs[*sets.begin()].first->font())
                        .horizontalAdvance(m_tabs[*sets.begin()].first->text()) -
                    dx,
                25);
            x += QFontMetrics(m_tabs[*sets.begin()].first->font())
                     .horizontalAdvance(m_tabs[*sets.begin()].first->text()) +
                 10 - dx;
            for (int i = *sets.begin() + 1; i < *sets.rbegin() + 1; i++) {
                m_tabs[i].first->show();
                m_tabs[i].first->setGeometry(x,
                                             0,
                                             QFontMetrics(m_tabs[i].first->font())
                                                 .horizontalAdvance(m_tabs[i].first->text()),
                                             25);
                x += QFontMetrics(m_tabs[i].first->font())
                         .horizontalAdvance(m_tabs[i].first->text()) +
                     10;
            }
        }

    } else {
        m_widget->setFixedWidth(width);
        int x = 5;
        for (int i = 0; i < m_tabs.size(); i++) {
            m_tabs[i].first->show();
            m_tabs[i].first->setGeometry(
                x,
                0,
                QFontMetrics(m_tabs[i].first->font()).horizontalAdvance(m_tabs[i].first->text()),
                25);
            x += QFontMetrics(m_tabs[i].first->font()).horizontalAdvance(m_tabs[i].first->text()) + 10;
        }
    }

    m_tabButton->setGeometry(m_widget->geometry().right() + 10, -5, 12, 30);
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

    if (this->width() < QFontMetrics(this->font()).horizontalAdvance(this->text())) {
        QString text = this->text().left(this->text().size()) + ("...");
        while (this->width() < QFontMetrics(this->font()).horizontalAdvance(text)) {
            int width = text.size() - 4;
            if (width <= 0) {
                break;
            }
            text = text.left(text.size() - 4).append("...");
        }
        painter.drawText(this->rect().x(),
                         this->rect().y(),
                         this->rect().width(),
                         this->fontMetrics().height(),
                         Qt::AlignLeft | Qt::AlignVCenter,
                         text);
        painter.drawText(this->rect().x(),
                         this->rect().y(),
                         this->rect().width(),
                         this->fontMetrics().height(),
                         Qt::AlignLeft | Qt::AlignVCenter,
                         text);
    } else {
        painter.drawText(this->rect().x(),
                         this->rect().y(),
                         this->rect().width(),
                         this->fontMetrics().height(),
                         Qt::AlignLeft | Qt::AlignVCenter,
                         this->text());
        painter.drawText(this->rect().x(),
                         this->rect().y(),
                         this->rect().width(),
                         this->fontMetrics().height(),
                         Qt::AlignLeft | Qt::AlignVCenter,
                         this->text());
    }

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

void TabButton::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        QPushButton::mousePressEvent(event);
    } else if (event->button() == Qt::RightButton) {
        RenameWidget *rename = new RenameWidget();
        rename->show();
        rename->SetText(this->text());

        connect(rename, &RenameWidget::Signal_Rename, [this, rename](QString text) {
            this->setText(text);
            rename->close();
            rename->deleteLater();
        });

        connect(rename, &RenameWidget::Signal_Delete, [this, rename]() {
            emit Signal_Delete();
            rename->close();
            rename->deleteLater();
        });
    }
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