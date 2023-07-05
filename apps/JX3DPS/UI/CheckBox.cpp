/**
 * Project: JX3DPS
 * File: SpinBox.cpp
 * Description:
 * Created Date: 2023-06-10 08:38:29
 * Author: 难为水
 * -----
 * Last Modified: 2023-07-05 07:56:44
 * Modified By: 难为水
 * -----
 * HISTORY:
 * Date      	By   	Comments
 * ----------	-----	----------------------------------------------------------
 */

#include "CheckBox.h"

#include <QEvent>
#include <QGraphicsDropShadowEffect>
#include <QHBoxLayout>
#include <QIntValidator>
#include <QPainter>

#include "Network.h"
#include "ThemeColors.h"

CheckBox::CheckBox(QWidget *parent) : QCheckBox(parent)
{
    this->setAttribute(Qt::WA_TranslucentBackground);
    // 创建一个阴影效果对象
    QGraphicsDropShadowEffect *shadowEffect = new QGraphicsDropShadowEffect(this);
    shadowEffect->setColor(Qt::black);     // 设置阴影的颜色
    shadowEffect->setBlurRadius(5);        // 设置阴影的模糊半径
    shadowEffect->setOffset(0, 0);         // 设置阴影的偏移量
    this->setGraphicsEffect(shadowEffect); // 为按钮应用阴影效果
}

void CheckBox::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

    painter.setPen(QPen(QColor(56, 60, 67), 1));
    painter.setBrush(m_color);

    painter.drawRect(3, 7, this->height() - 12, this->height() - 12);

    if (this->isChecked()) {
        // 画个对号
        painter.setPen(QPen(QColor(COLOR_ACTIVE), 2.5));
        int width = this->height() - 12;

        painter.drawLines(QVector<QLineF>()
                          << QLineF(3 + 1, 7 + width / 2, 3 + width / 2 - 1, 7 + width - 2)
                          << QLineF(3 + width / 2 - 1, 7 + width - 2, 3 + width, 7 + width / 2 - 3));
    }

    painter.setPen(QPen(QColor(COLOR_INACTIVE), 1));
    painter.setBrush(Qt::NoBrush);
    painter.drawText(3 + this->height() - 12 + 6,
                     0,
                     this->width() - (3 + this->height() - 12 + 6),
                     this->height(),
                     Qt::AlignLeft | Qt::AlignVCenter,
                     this->text());
    painter.drawText(3 + this->height() - 12 + 6,
                     0,
                     this->width() - (3 + this->height() - 12 + 6),
                     this->height(),
                     Qt::AlignLeft | Qt::AlignVCenter,
                     this->text());
}

void CheckBox::enterEvent(QEnterEvent *event)
{
    m_color = QColor(35, 38, 46);
    update();
}

void CheckBox::leaveEvent(QEvent *event)
{
    m_color = QColor(COLOR_BACKGROUND_HIGHLIGHT);
    update();
}

void CheckBox::mousePressEvent(QMouseEvent *event)
{
    this->setChecked(!this->isChecked());
    update();
    // QCheckBox::mousePressEvent(event);
}

SecretImage::SecretImage(const QPixmap &pixmap, QWidget *parent)
{
    this->setAttribute(Qt::WA_TranslucentBackground);
    // 创建一个阴影效果对象
    QGraphicsDropShadowEffect *shadowEffect = new QGraphicsDropShadowEffect(this);
    shadowEffect->setColor(Qt::black);     // 设置阴影的颜色
    shadowEffect->setBlurRadius(5);        // 设置阴影的模糊半径
    shadowEffect->setOffset(0, 0);         // 设置阴影的偏移量
    this->setGraphicsEffect(shadowEffect); // 为按钮应用阴影效果

    m_pixmap = new QPixmap(pixmap);
}

void SecretImage::SetChecked(bool checked)
{
    m_checked = checked;
    update();
}

void SecretImage::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

    // 画一个框
    painter.setPen(QPen(QColor(56, 60, 67)));
    painter.setBrush(QBrush(QColor(COLOR_BACKGROUND_HIGHLIGHT)));
    painter.drawRect(this->rect());

    // 画一个图形
    if (m_pixmap != nullptr) {
        painter.setPen(QPen(QColor(0, 0, 0)));
        painter.setBrush(QBrush(QColor(0, 0, 0)));
        painter.drawRect(2, 2, this->width() - 4, this->height() - 4);
        int         border = 2;
        QIcon       icon(m_pixmap->copy(3, 3, 42, 42));
        QIcon::Mode mode = (m_checked || m_hovered) ? QIcon::Normal : QIcon::Disabled;
        painter.drawPixmap(border,
                           border,
                           this->width() - 2 * border,
                           this->height() - 2 * border,
                           icon.pixmap(m_pixmap->size(), mode).scaledToWidth(this->height() - border * 2));
    }
}

void SecretImage::enterEvent(QEnterEvent *event)
{
    m_hovered = true;
    update();
}

void SecretImage::leaveEvent(QEvent *event)
{
    m_hovered = false;
    update();
}

void SecretImage::mousePressEvent(QMouseEvent *event)
{
    m_checked = !m_checked;
    emit Signal_Checked(m_checked);
    update();
}

void SecretImage::mouseReleaseEvent(QMouseEvent *event)
{
    QWidget::mouseReleaseEvent(event); // QGroupBox的鼠标释放事件不能正常传递给父类，需要手动调用，避免无法拖动窗口
}

SecretCheckBox::SecretCheckBox(const SecretInfo &info, QWidget *parent)
{
    this->setAttribute(Qt::WA_TranslucentBackground);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    QString url = QString("https://icon.jx3box.com/icon/%1.png").arg(info.iconId);
    QPixmap pixmap(RequestIcon(url));

    m_image = new SecretImage(pixmap, this);
    m_image->setFixedSize(36, 36);

    m_checkBox = new CheckBox(this);

    m_checkBox->setFixedSize(22, 22);

    this->setFixedSize(70, 36);

    layout->addWidget(m_checkBox);
    layout->addWidget(m_image);

    connect(m_image, &SecretImage::Signal_Checked, [=](bool checked) {
        m_checkBox->setChecked(checked);
    });

    connect(m_checkBox, &QCheckBox::toggled, [=](bool checked) { m_image->SetChecked(checked); });

    this->setToolTip(QString::fromStdString(info.desc));

    // 设置样式表
    this->setStyleSheet(
        "QToolTip { color: white; background-color: rgb(23, 29, 37); border: none; }");
}

bool SecretCheckBox::IsChecked()
{
    return m_checkBox->isChecked();
}
