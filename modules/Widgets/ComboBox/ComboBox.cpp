/**
 * Project: JX3DPS
 * File: SpinBox.cpp
 * Description:
 * Created Date: 2023-06-10 08:38:29
 * Author: 难为水
 * -----
 * Last Modified: 2023-08-10 23:04:11
 * Modified By: 难为水
 * -----
 * HISTORY:
 * Date      	By   	Comments
 * ----------	-----	----------------------------------------------------------
 */

#include "ComboBox.h"

#include <QApplication>
#include <QGraphicsDropShadowEffect>
#include <QLayout>
#include <QPainter>
#include <QWheelEvent>

#include "Common/ThemeColors.h"

#include "MarqueeLabel.h"

class Icon : public QWidget
{
public:
    Icon(const QPixmap &pixmap, QWidget *parent) : QWidget(parent) { m_pixmap = pixmap; }

    void SetType(ComboBox::Type type) { m_type = type; }

    void SetHovered(bool hovered) { m_hovered = hovered; }

    void SetItemInfo(const ComboBox::ItemInfo &itemInfo)
    {
        m_itemInfo = itemInfo;
        m_pixmap   = QPixmap(m_itemInfo.iconPath);
    }

protected:
    void paintEvent(QPaintEvent *event)
    {

        QPainter painter(this);
        painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing |
                               QPainter::SmoothPixmapTransform);

        double opacity = 1.0;
        if (m_type == ComboBox::Type::ICON_NAME || m_type == ComboBox::Type::ICON) {
            painter.setPen(QPen(Qt::black));
            if (m_hovered) {
                painter.setBrush(QBrush(QColor(COLOR_BACKGROUND_BASE)));
            } else {
                painter.setBrush(QBrush(QColor(COLOR_BACKGROUND_HIGHLIGHT)));
            }
            painter.drawRect(this->rect());
        }

        if (m_type != ComboBox::Type::DEFAULT) {
            if (!m_hovered) {
                opacity = 0.8;
            }
        }

        if (m_pixmap.isNull()) {
            // painter.setPen(QPen(QColor(COLOR_HIGHLIGHT)));
            // painter.drawText(this->rect(), Qt::AlignCenter, m_itemInfo.name);
            // painter.drawText(this->rect(), Qt::AlignCenter, m_itemInfo.name);
            return;
        }

        // 画一个框
        int border = 3;
        painter.setPen(QPen(Qt::black));

        painter.setBrush(QBrush(QColor(COLOR_BACKGROUND_HIGHLIGHT)));

        painter.drawRect(border, border, this->height() - border * 2, this->height() - border * 2);

        painter.setOpacity(opacity);
        // painter.setPen(Qt::NoPen);
        QIcon       icon(m_pixmap.copy(m_pixmap.width() / 12,
                                 m_pixmap.width() / 12,
                                 m_pixmap.width() / 12 * 10,
                                 m_pixmap.width() / 12 * 10));
        QIcon::Mode mode =
            (m_hovered || m_type != ComboBox::Type::DEFAULT) ? QIcon::Normal : QIcon::Disabled;
        painter.drawPixmap(border,
                           border,
                           icon.pixmap(this->size(), mode).scaledToWidth(this->height() - border * 2));
    }

    void enterEvent(QEnterEvent *event)
    {
        m_hovered = true;
        QWidget::enterEvent(event);
    }

    void leaveEvent(QEvent *event)
    {
        m_hovered = false;
        QWidget::leaveEvent(event);
    }

private:
    QPixmap            m_pixmap;
    bool               m_hovered = false;
    ComboBox::ItemInfo m_itemInfo;

    ComboBox::Type m_type = ComboBox::Type::DEFAULT;
};

ComboBox::ComboBox(QWidget *parent) : QWidget(parent)
{
    m_subComboBox = new SubComboBox(this);

    // 创建一个阴影效果对象
    QGraphicsDropShadowEffect *shadowEffect = new QGraphicsDropShadowEffect(this);
    shadowEffect->setColor(Qt::black);     // 设置阴影的颜色
    shadowEffect->setBlurRadius(10);       // 设置阴影的模糊半径
    shadowEffect->setOffset(0, 0);         // 设置阴影的偏移量

    this->setGraphicsEffect(shadowEffect); // 为按钮应用阴影效果

    connect(m_subComboBox, &SubComboBox::Signal_CurrentItemChanged, this, [=](const ItemInfo &itemInfo) {
        m_name = itemInfo.name;
        update();
        emit Signal_CurrentItemChanged(itemInfo);
    });
}

void ComboBox::Clear()
{
    m_subComboBox->clear();
    update();
}

void ComboBox::SetType(Type type)
{
    m_type = type;
    m_subComboBox->SetType(type);
    update();
}

void ComboBox::AddItem(const ItemInfo &itemInfo)
{
    m_subComboBox->AddItem(itemInfo);
    update();
}

void ComboBox::SetItemSize(int width, int height)
{
    m_subComboBox->SetItemSize(width, height);
}

void ComboBox::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    if (m_type == Type::ICON_NAME) {
        m_subComboBox->setGeometry(this->rect().x(),
                                   this->rect().y(),
                                   this->rect().width(),
                                   this->rect().width());

        painter.setFont(QFont(this->font().family(), 10));
        painter.setPen(QPen(QColor(COLOR_HIGHLIGHT)));
        painter.drawText(this->rect().x(),
                         this->rect().y() + this->rect().width(),
                         this->rect().width(),
                         this->rect().height() - this->rect().width(),
                         Qt::AlignCenter,
                         m_name);
        painter.drawText(this->rect().x(),
                         this->rect().y() + this->rect().width(),
                         this->rect().width(),
                         this->rect().height() - this->rect().width(),
                         Qt::AlignCenter,
                         m_name);
    } else {
        m_subComboBox->setGeometry(this->rect());
    }
}

SubComboBox::SubComboBox(QWidget *parent) : QComboBox(parent)
{
    this->setAttribute(Qt::WA_TranslucentBackground);
    this->setStyleSheet("QComboBox QAbstractItemView { border: none; }");
    this->setMaxVisibleItems(8);

    m_detailedView = new ItemWidget(this);
    m_icon         = new Icon(QPixmap(), this);

    m_detailedView->SetView();
    m_icon->SetType(ComboBox::Type::ICON);

    ListWidget *listWidget = new ListWidget(this);
    this->setModel(listWidget->model());
    this->setView(listWidget);

    connect(listWidget, &QListWidget::currentItemChanged, this, [=](QListWidgetItem *current, QListWidgetItem *previous) {
        if (previous != nullptr && ((ItemWidget *)listWidget->itemWidget(previous)) != nullptr)
        {
            ((ItemWidget *)listWidget->itemWidget(previous))->SetSelected(false);
        }
        if (current == nullptr && ((ItemWidget *)listWidget->itemWidget(current)) == nullptr)
        {
            return;
        }
        ComboBox::ItemInfo itemInfo = ((ItemWidget *)listWidget->itemWidget(current))->GetItemInfo();

        ((ItemWidget *)listWidget->itemWidget(current))->SetSelected(true);
        int index = listWidget->indexFromItem(current).row();
        this->setCurrentIndex(index);
        this->SetView(itemInfo);

        emit Signal_CurrentItemChanged(itemInfo);
    });
}

void SubComboBox::SetType(ComboBox::Type type)
{
    if (type == ComboBox::Type::DETAILED) {
        m_detailedView->show();
        m_icon->hide();
    } else {
        m_icon->show();
        m_detailedView->hide();
    }
}

void SubComboBox::SetView(const ComboBox::ItemInfo &itemInfo)
{
    m_detailedView->SetItemInfo(itemInfo);
    m_icon->SetItemInfo(itemInfo);
}

void SubComboBox::AddItem(const ComboBox::ItemInfo &itemInfo)
{
    QListWidgetItem *item = new QListWidgetItem((ListWidget *)this->view());
    item->setSizeHint(QSize(m_width, m_height));

    ItemWidget *itemWidget = new ItemWidget(itemInfo, this->view());

    ((ListWidget *)this->view())->addItem(item);
    ((ListWidget *)this->view())->setItemWidget(item, itemWidget);

    if (this->count() == 1) {
        this->SetView(itemInfo);
        emit Signal_CurrentItemChanged(itemInfo);
    }
}

void SubComboBox::SetItemSize(int width, int height)
{
    m_width       = width;
    m_height      = height;
    int itemCount = static_cast<ListWidget *>(this->view())->count();
    for (int i = 0; i < itemCount; i++) {
        QListWidgetItem *item = static_cast<ListWidget *>(this->view())->item(i);
        item->setSizeHint(QSize(m_width, m_height));
    }
    QWidget *popup = this->findChild<QFrame *>();
    popup->setFixedWidth(m_width);
}

void SubComboBox::showPopup()
{
    QComboBox::showPopup();
    m_detailedView->SetHovered(false);
    m_icon->SetHovered(false);
}

ComboBox::ItemInfo SubComboBox::GetItemInfo() const
{
    return m_detailedView->GetItemInfo();
}

void SubComboBox::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    m_icon->setGeometry(this->rect());
    m_detailedView->setGeometry(this->rect());
}

ListWidget::ListWidget(QWidget *parent) : QListWidget(parent)
{
    this->setAttribute(Qt::WA_TranslucentBackground);
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setSelectionMode(QAbstractItemView::SingleSelection);
    this->setSelectionBehavior(QAbstractItemView::SelectRows);
    this->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    this->setEditTriggers(QAbstractItemView::NoEditTriggers);
    this->setMouseTracking(true);
    this->setStyleSheet("QListWidget { border: none; }");
}

QRect ListWidget::visualRect(const QModelIndex &index) const
{
    QRect rect = QListWidget::visualRect(index);
    return rect;
}

void ListWidget::wheelEvent(QWheelEvent *event)
{
    QListWidget::wheelEvent(event);
    this->doItemsLayout(); // 重绘，否则会出现不刷新的情况

    // 获取当前鼠标所在位置
    QPointF pos = event->position();

    // 根据位置获取所在item
    QListWidgetItem *item = itemAt(pos.toPoint());

    if (item != m_lastEnteredItem && m_lastEnteredItem != nullptr) {
        QApplication::sendEvent(((ItemWidget *)this->itemWidget(m_lastEnteredItem)),
                                new QEvent(QEvent::Leave));
        m_lastEnteredItem = nullptr;
    }

    // 如果有item被找到，则触发enterEvent事件
    if (item != nullptr) {
        m_lastEnteredItem = item;
        QApplication::sendEvent(((ItemWidget *)this->itemWidget(item)), new QEnterEvent(pos, pos, pos));
    }
}

ItemWidget::ItemWidget(const ComboBox::ItemInfo &itemInfo, QWidget *parent) :
    QWidget(parent)
{
    m_itemInfo = itemInfo;
    m_nameText = new QLabel(m_itemInfo.name, this);
    m_descText = new MarqueeLabel(m_itemInfo.description, this);
    m_icon     = new Icon(QPixmap(m_itemInfo.iconPath), this);

    QPalette palette;
    palette.setColor(QPalette::WindowText, QColor(COLOR_HIGHLIGHT));
    m_nameText->setPalette(palette);
    m_descText->setPalette(palette);

    m_nameText->setFont(QFont(m_nameText->font().family(), 11));

    m_hLayout = new QHBoxLayout(this);
    m_vLayout = new QVBoxLayout;

    m_hLayout->setContentsMargins(0, 0, 0, 0);
    m_hLayout->setSpacing(0);
    m_hLayout->addWidget(m_icon, 0);
    m_hLayout->addLayout(m_vLayout, 1);

    m_vLayout->setContentsMargins(3, 5, 5, 3);
    m_vLayout->addWidget(m_nameText);
    m_vLayout->addWidget(m_descText);
}

ItemWidget::ItemWidget(QWidget *parent) : QWidget(parent)
{

    this->setMouseTracking(true);

    m_nameText = new QLabel(this);
    m_descText = new MarqueeLabel(this);
    m_icon     = new Icon(QPixmap(), this);

    QPalette palette;
    palette.setColor(QPalette::WindowText, QColor(COLOR_HIGHLIGHT));
    m_nameText->setPalette(palette);
    m_descText->setPalette(palette);

    m_nameText->setFont(QFont(m_nameText->font().family(), 11));

    m_hLayout = new QHBoxLayout(this);
    m_vLayout = new QVBoxLayout;

    m_hLayout->setContentsMargins(0, 0, 0, 0);
    m_hLayout->setSpacing(0);
    m_hLayout->addWidget(m_icon, 0);
    m_hLayout->addLayout(m_vLayout, 1);

    m_vLayout->setContentsMargins(3, 5, 5, 3);
    m_vLayout->addWidget(m_nameText);
    m_vLayout->addWidget(m_descText);
}

void ItemWidget::SetHovered(bool hovered)
{
    m_hovered = hovered;
}

void ItemWidget::SetView()
{
    m_icon->SetType(ComboBox::Type::DETAILED);
}

void ItemWidget::SetItemInfo(const ComboBox::ItemInfo &itemInfo)
{
    m_itemInfo = itemInfo;
    m_nameText->setText(m_itemInfo.name);
    m_descText->setText(m_itemInfo.description);
    m_icon->SetItemInfo(m_itemInfo);
}

void ItemWidget::SetSelected(bool selected)
{
    m_selected = selected;
}

ComboBox::ItemInfo ItemWidget::GetItemInfo() const
{
    return m_itemInfo;
}

void ItemWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    if (m_hovered || m_selected) {
        painter.fillRect(this->rect(), QColor(23, 29, 37));
        m_nameText->setStyleSheet("background-color: rgb(23, 29, 37);");
        m_descText->setStyleSheet("background-color: rgb(23, 29, 37);");
        this->setStyleSheet("background-color: rgb(23, 29, 37);");
        this->setStyleSheet("color: rgb(255, 255, 255);");
        m_icon->SetHovered(true);
    } else {
        painter.fillRect(this->rect(), QColor(43, 46, 50));
        m_nameText->setStyleSheet("background-color: rgb(43, 46, 50);");
        m_descText->setStyleSheet("background-color: rgb(43, 46, 50);");
        this->setStyleSheet("background-color: rgb(43, 46, 50);");
        this->setStyleSheet("color: rgb(208, 211, 212);");
        m_icon->SetHovered(false);
    }

    if (m_itemInfo.description.isEmpty()) {
        m_nameText->hide();
        m_descText->hide();
        painter.setFont(QFont(this->font().family(), 11));
        painter.setPen(QPen(QColor(COLOR_HIGHLIGHT)));
        if (m_itemInfo.iconPath.isEmpty()) {
            painter.drawText(this->rect().x(),
                             this->rect().y(),
                             this->rect().width(),
                             this->rect().height(),
                             Qt::AlignCenter,
                             m_itemInfo.name);
            painter.drawText(this->rect().x(),
                             this->rect().y(),
                             this->rect().width(),
                             this->rect().height(),
                             Qt::AlignCenter,
                             m_itemInfo.name);
        } else {
            m_icon->setFixedSize(this->height(), this->height());
            painter.drawText(this->rect().x() + this->rect().height(),
                             this->rect().y(),
                             this->rect().width() - this->rect().height(),
                             this->rect().height(),
                             Qt::AlignCenter,
                             m_itemInfo.name);
            painter.drawText(this->rect().x() + this->rect().height(),
                             this->rect().y(),
                             this->rect().width() - this->rect().height(),
                             this->rect().height(),
                             Qt::AlignCenter,
                             m_itemInfo.name);
        }

    } else {
        m_icon->setFixedSize(this->height(), this->height());
        m_nameText->show();
        m_descText->show();
    }

    painter.setPen(QColor(COLOR_BACKGROUND_PRIMARY));
    painter.setBrush(Qt::NoBrush);
    painter.drawRect(this->rect().x(),
                     this->rect().y(),
                     this->rect().width() - 1,
                     this->rect().height() - 1);

    QWidget::paintEvent(event);
}

void ItemWidget::enterEvent(QEnterEvent *event)
{
    m_hovered = true;
    m_descText->Start();
    QWidget::enterEvent(event);
}

void ItemWidget::leaveEvent(QEvent *event)
{
    m_hovered = false;
    m_descText->Reset();
    QWidget::leaveEvent(event);
}
