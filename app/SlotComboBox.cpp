#include "SlotComboBox.h"

#include <QPainter>

SlotComboBoxWidget::SlotComboBoxWidget(QWidget *parent) : QLabel(parent)
{
    m_pixmap = QPixmap("P:\\MyProject\\JX3DPS\\EquipNone.png");
    m_bgPixmap = QPixmap("P:\\MyProject\\JX3DPS\\EquipNone.png");
}

QSize SlotComboBoxWidget::minimumSizeHint() const
{
    return QSize(m_size, m_size);
}

QSize SlotComboBoxWidget::sizeHint() const
{
    return QSize(m_size, m_size);
}

void SlotComboBoxWidget::SetSize(int size)
{
    m_size = size;
}

void SlotComboBoxWidget::SetType(int type)
{
    switch (type) {
    case 0:
        m_pixmap = QPixmap("P:\\MyProject\\JX3DPS\\Slot6.jpg");
        break;
    case 1:
        m_pixmap = QPixmap("P:\\MyProject\\JX3DPS\\SuperEnchant.png");
        break;
    default:
        break;
    }
}

void SlotComboBoxWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
//    QPixmap pix = m_bgPixmap.scaled(m_size, m_size, Qt::KeepAspectRatio);
//    QIcon icon(pix);
//    painter.drawPixmap(QRectF(0, 0, m_size, m_size), icon.pixmap(pix.size(), m_isColor ? QIcon::Normal : QIcon::Disabled), QRectF(0, 0, m_size, m_size));

    painter.setOpacity(m_opacity + 0.1);
    QPixmap pix2 = m_pixmap.scaled(m_size, m_size, Qt::KeepAspectRatio);
    QIcon icon2(pix2);
    painter.drawPixmap(QRectF(0, 0, m_size, m_size), icon2.pixmap(pix2.size(), m_isColor ? QIcon::Normal : QIcon::Disabled), QRectF(1, 1, m_size - 2, m_size - 2));
}

void SlotComboBoxWidget::enterEvent(QEnterEvent *event)
{
    m_opacity += 0.4;
}

void SlotComboBoxWidget::leaveEvent(QEvent *event)
{
    m_opacity -= 0.4;
}

SlotComboBox::SlotComboBox(QWidget *parent) : QComboBox(parent)
{
    setStyleSheet("QComboBox::drop-down{""border-style: none;}");
    m_slotComboBoxWidget->setGeometry(1, 1, 30, 30);
}

void SlotComboBox::SetType(int type)
{
    m_slotComboBoxWidget->SetType(type);
}
