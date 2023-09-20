/**
 * Project: JX3DPS
 * File: SpinBox.cpp
 * Description:
 * Created Date: 2023-06-10 08:38:29
 * Author: 难为水
 * -----
 * Last Modified: 2023-09-05 16:37:50
 * Modified By: 难为水
 * -----
 * HISTORY:
 * Date      	By   	Comments
 * ----------	-----	----------------------------------------------------------
 */

#include "SpinBox.h"

#include <QEvent>
#include <QGraphicsDropShadowEffect>
#include <QIntValidator>
#include <QPainter>

#include "Common/ThemeColors.h"

SpinBox::SpinBox(QWidget *parent) :
    QWidget(parent), m_value(0), m_min(0), m_max(99999), m_dragging(false)
{
    QGraphicsDropShadowEffect *shadowEffect = new QGraphicsDropShadowEffect(this);
    shadowEffect->setColor(Qt::black);     // 设置阴影的颜色
    shadowEffect->setBlurRadius(5);        // 设置阴影的模糊半径
    shadowEffect->setOffset(0, 0);         // 设置阴影的偏移量
    this->setGraphicsEffect(shadowEffect); // 为按钮应用阴影效果
}

int SpinBox::value() const
{
    return m_value;
}

void SpinBox::setValue(int value)
{

    if (value < m_min) {
        value = m_min;
    } else if (value > m_max) {
        value = m_max;
    }

    m_value = value;
    update();
    emit Signal_UpdateValue(value);
}

void SpinBox::setRange(int min, int max)
{
    m_min = min;
    m_max = max;
    // if (m_value < m_min) {
    //     m_value = m_min;
    // } else if (m_value > m_max) {
    //     m_value = m_max;
    // }

    update();
}

void SpinBox::UpdateValue(int value)
{
    m_value = value;
    update();
}

void SpinBox::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.setPen(QColor(COLOR_BACKGROUND_HIGHLIGHT));
    painter.setBrush(QColor(COLOR_BACKGROUND_HIGHLIGHT));
    painter.drawRect(rect());

    painter.setPen(QColor(COLOR_HIGHLIGHT));
    painter.drawText(4, 0, width() - 8, height(), Qt::AlignVCenter | Qt::AlignRight, QString::number(m_value));
    painter.drawText(4, 0, width() - 8, height(), Qt::AlignVCenter | Qt::AlignRight, QString::number(m_value));
}

void SpinBox::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && rect().contains(event->pos())) {
        m_dragging     = true;
        m_dragStartPos = event->pos();
    }
}

void SpinBox::mouseMoveEvent(QMouseEvent *event)
{
    if (m_dragging) {
        int diffX = event->pos().x() - m_dragStartPos.x();
        int diffY = event->pos().y() - m_dragStartPos.y();

        int diffValue = diffX - diffY;
        int newValue  = m_value + diffValue;

        setValue(newValue);

        m_dragStartPos = event->pos();
    }
}

void SpinBox::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && m_dragging) {
        m_dragging = false;
    }
}

void SpinBox::enterEvent(QEnterEvent *event)
{
    setCursor(Qt::SizeHorCursor);
}

void SpinBox::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        LineEdit *m_lineEdit = new LineEdit(this);
        m_lineEdit->setText(QString::number(m_value));
        m_lineEdit->selectAll();
        m_lineEdit->setGeometry(rect());
        m_lineEdit->setAlignment(Qt::AlignCenter);
        m_lineEdit->show();
        m_lineEdit->setFocus();
        m_lineEdit->setCursorPosition(m_lineEdit->text().length());
        connect(m_lineEdit, &QLineEdit::editingFinished, this, [=]() {
            bool ok;
            int  value = m_lineEdit->text().toInt(&ok);
            if (ok) {
                setValue(value);
                emit Signal_UpdateValue(value);
            }
            delete m_lineEdit;
        });
    }
}

void SpinBox::focusOutEvent(QFocusEvent *event)
{
    if (event->reason() == Qt::MouseFocusReason || event->reason() == Qt::TabFocusReason) {
        bool ok;
        int  value = m_lineEdit->text().toInt(&ok);
        if (ok) {
            setValue(value);
            emit Signal_UpdateValue(value);
        }
        delete m_lineEdit;
    }
    QWidget::focusOutEvent(event);
}