/**
 * Project: JX3DPS
 * File: SpinBox.cpp
 * Description:
 * Created Date: 2023-06-10 08:38:29
 * Author: 难为水
 * -----
 * Last Modified: 2023-07-18 05:55:35
 * Modified By: 难为水
 * -----
 * HISTORY:
 * Date      	By   	Comments
 * ----------	-----	----------------------------------------------------------
 */

#include "ComboBox.h"

#include <QGraphicsDropShadowEffect>
#include <QLayout>
#include <QPainter>

#include "MarqueeLabel.h"

#define COLOR_BACKGROUND_BASE      23, 29, 37
#define COLOR_BACKGROUND_HIGHLIGHT 43, 46, 50

#define TO_STR(x)                  ADD_QUOTES_HELPER(x)
#define ADD_QUOTES_HELPER(x)       #x

class Icon : public QWidget
{
public:
    Icon(const QPixmap &pixmap, QWidget *parent) : QWidget(parent) { m_pixmap = pixmap; }

    void SetComboBoxView() { m_view = true; }

    void SetHovered(bool hovered) { m_hovered = hovered; }

    void SetPixmap(const QPixmap &pixmap) { m_pixmap = pixmap; }

protected:
    void paintEvent(QPaintEvent *event)
    {

        QPainter painter(this);
        painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);

        double opacity = 1.0;
        if (m_view) {
            painter.setPen(QPen(Qt::black));
            painter.setBrush(QBrush(QColor(COLOR_BACKGROUND_HIGHLIGHT)));
            painter.drawRect(this->rect());
            if (!m_hovered) {
                opacity = 0.8;
            }
        }

        if (m_pixmap.isNull()) {
            return;
        }

        // 画一个框
        int border = 3;
        painter.setPen(QPen(Qt::black));
        painter.setBrush(QBrush(QColor(COLOR_BACKGROUND_HIGHLIGHT)));
        painter.drawRect(border, border, this->height() - border * 2, this->height() - border * 2);

        painter.setOpacity(opacity);
        // painter.setPen(Qt::NoPen);
        QIcon       icon(m_pixmap.copy(4, 4, 40, 40));
        QIcon::Mode mode = (m_hovered || m_view) ? QIcon::Normal : QIcon::Disabled;
        painter.drawPixmap(border, border, icon.pixmap(this->size(), mode).scaledToWidth(this->height() - border * 2));
    }

    void enterEvent(QEnterEvent *event)
    {
        m_hovered = true;
        this->update();
        QWidget::enterEvent(event);
    }

    void leaveEvent(QEvent *event)
    {
        m_hovered = false;
        this->update();
        QWidget::leaveEvent(event);
    }

private:
    QPixmap m_pixmap;
    bool    m_hovered = false;
    bool    m_view    = false;
};

#define COLOR_FOCUS     255, 255, 255
#define COLOR_HIGHLIGHT 208, 211, 212

ItemWidget::ItemWidget(const ItemInfo &itemInfo, QWidget *parent) : QWidget(parent)
{
    m_itemInfo = itemInfo;

    m_nameText = new QLabel(QString::fromStdString(m_itemInfo.name), this);
    m_descText = new MarqueeLabel(QString::fromStdString(m_itemInfo.desc), this);
    m_icon     = new Icon(QPixmap(m_itemInfo.icon.c_str()), this);
    m_icon->setFixedSize(this->height(), this->height());

    QPalette palette;
    palette.setColor(QPalette::WindowText, QColor(COLOR_HIGHLIGHT));
    m_nameText->setPalette(palette);
    m_descText->setPalette(palette);
    m_nameText->setFont(QFont("微软雅黑", 11));
    m_descText->setFont(QFont("微软雅黑", 10));

    m_hLayout = new QHBoxLayout(this);
    m_vLayout = new QVBoxLayout;

    SetItemInfo(itemInfo);
}

ItemWidget::ItemWidget(QWidget *parent) : QWidget(parent)
{
    m_nameText = new QLabel(QString::fromStdString(m_itemInfo.name), this);
    m_descText = new MarqueeLabel(QString::fromStdString(m_itemInfo.desc), this);
    m_icon     = new Icon(QPixmap(m_itemInfo.icon.c_str()), this);
    m_icon->setFixedSize(this->height(), this->height());

    QPalette palette;
    palette.setColor(QPalette::WindowText, QColor(COLOR_HIGHLIGHT));
    m_nameText->setPalette(palette);
    m_descText->setPalette(palette);
    m_nameText->setFont(QFont("微软雅黑", 11));
    m_descText->setFont(QFont("微软雅黑", 10));

    m_hLayout = new QHBoxLayout(this);
    m_vLayout = new QVBoxLayout;

    SetItemInfo(m_itemInfo);
}

void ItemWidget::SetHovered(bool hovered)
{
    m_hovered = hovered;
    this->update();
}

void ItemWidget::SetSelected(bool selected)
{
    m_selected = selected;
    m_hovered  = false;
    this->update();
}

void ItemWidget::SetComboBoxView()
{
    m_icon->SetComboBoxView();
}

ItemInfo ItemWidget::GetItemInfo() const
{
    return m_itemInfo;
}

void ItemWidget::SetItemInfo(const ItemInfo &itemInfo)
{
    m_nameText->setText(itemInfo.name.c_str());
    m_descText->setText(itemInfo.desc.c_str());
    m_icon->SetPixmap(QPixmap(itemInfo.icon.c_str()));

    if (itemInfo.icon.empty()) {
        m_hLayout->removeItem(m_vLayout);
        m_hLayout->removeWidget(m_icon);
        m_vLayout->removeWidget(m_descText);
        m_vLayout->removeWidget(m_nameText);
        m_vLayout->setParent(NULL);

        m_descText->close();
        m_icon->SetPixmap(QPixmap());
        m_icon->close();

        m_hLayout->addWidget(m_nameText, 0, Qt::AlignCenter);
    } else {
        m_hLayout->removeWidget(m_nameText);
        m_vLayout->setParent(NULL);

        m_descText->show();
        m_icon->show();

        m_hLayout->setContentsMargins(0, 0, 0, 0);
        m_hLayout->setSpacing(0);
        m_hLayout->addWidget(m_icon, 0);
        m_hLayout->addLayout(m_vLayout, 1);

        m_vLayout->setContentsMargins(3, 5, 5, 3);
        m_vLayout->addWidget(m_nameText);
        m_vLayout->addWidget(m_descText);
    }
}

void ItemWidget::paintEvent(QPaintEvent *event)
{
    m_icon->setFixedSize(this->height(), this->height());
    QPainter painter(this);
    if (m_hovered || m_selected) {
        painter.fillRect(this->rect(), QColor(23, 29, 37));
        m_nameText->setStyleSheet("background-color: rgb(23, 29, 37);");
        m_descText->setStyleSheet("background-color: rgb(23, 29, 37);");
        m_icon->setStyleSheet("background-color: rgb(23, 29, 37);");

        m_icon->SetHovered(true);
        this->setStyleSheet("background-color: rgb(23, 29, 37);");
        this->setStyleSheet("color: rgb(255, 255, 255);");
    } else {
        painter.fillRect(this->rect(), QColor(43, 46, 50));
        m_nameText->setStyleSheet("background-color: rgb(43, 46, 50);");
        m_descText->setStyleSheet("background-color: rgb(43, 46, 50);");
        m_icon->setStyleSheet("background-color: rgb(43, 46, 50);");

        m_icon->SetHovered(false);
        this->setStyleSheet("background-color: rgb(43, 46, 50);");
        this->setStyleSheet("color: rgb(208, 211, 212);");
    }

    QWidget::paintEvent(event);
}

void ItemWidget::enterEvent(QEnterEvent *event)
{
    m_hovered = true;
    this->update();
    m_descText->Start();
    QWidget::enterEvent(event);
}

void ItemWidget::leaveEvent(QEvent *event)
{
    m_hovered = false;
    this->update();
    m_descText->Reset();
    QWidget::leaveEvent(event);
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

#include <QApplication>
#include <QWheelEvent>

void ListWidget::wheelEvent(QWheelEvent *event)
{
    QListWidget::wheelEvent(event);
    this->doItemsLayout(); // 重绘，否则会出现不刷新的情况

    // 获取当前鼠标所在位置
    QPointF pos = event->position();

    // 根据位置获取所在item
    QListWidgetItem *item = itemAt(pos.toPoint());

    if (item != m_lastEnteredItem && m_lastEnteredItem != nullptr) {
        QApplication::sendEvent(((ItemWidget *)this->itemWidget(m_lastEnteredItem)), new QEvent(QEvent::Leave));
        m_lastEnteredItem = nullptr;
    }

    // 如果有item被找到，则触发enterEvent事件
    if (item != nullptr) {
        m_lastEnteredItem = item;
        QApplication::sendEvent(((ItemWidget *)this->itemWidget(item)), new QEnterEvent(pos, pos, pos));
    }
}

SubComboBox::SubComboBox(ComboBoxType type, QWidget *parent) : QComboBox(parent)
{
    this->setAttribute(Qt::WA_TranslucentBackground);
    this->setStyleSheet("QComboBox QAbstractItemView { border: none; }");
    this->setMaxVisibleItems(8);

    // 创建一个阴影效果对象
    QGraphicsDropShadowEffect *shadowEffect = new QGraphicsDropShadowEffect(this);
    shadowEffect->setColor(Qt::black);     // 设置阴影的颜色
    shadowEffect->setBlurRadius(10);       // 设置阴影的模糊半径
    shadowEffect->setOffset(0, 0);         // 设置阴影的偏移量
    this->setGraphicsEffect(shadowEffect); // 为按钮应用阴影效果

    QGridLayout *gLayout = new QGridLayout(this);
    gLayout->setContentsMargins(1, 1, 1, 1);
    gLayout->setSpacing(0);

    if (type == ComboBoxType::DETAILED_MODE) {
        m_view = new ItemWidget(this);
        static_cast<ItemWidget *>(m_view)->SetComboBoxView();
    } else {
        m_view = new Icon(QPixmap(), this);
        static_cast<Icon *>(m_view)->SetComboBoxView();
    }
    gLayout->addWidget(m_view);

    ListWidget *listWidget = new ListWidget(this);
    this->setModel(listWidget->model());
    this->setView(listWidget);

    connect(listWidget, &ListWidget::itemClicked, this, [=](QListWidgetItem *item) {
        this->hidePopup();
    });

    connect(listWidget, &QListWidget::currentItemChanged, this, [=](QListWidgetItem *current, QListWidgetItem *previous) {
        if (previous != nullptr) {
            ((ItemWidget *)listWidget->itemWidget(previous))->SetSelected(false);
        }
        ItemInfo itemInfo = ((ItemWidget *)listWidget->itemWidget(current))->GetItemInfo();

        ((ItemWidget *)listWidget->itemWidget(current))->SetSelected(true);
        int index = listWidget->indexFromItem(current).row();
        this->setCurrentIndex(index);

        if (type == ComboBoxType::DETAILED_MODE) {
            static_cast<ItemWidget *>(m_view)->SetItemInfo(itemInfo);
        } else {
            static_cast<Icon *>(m_view)->SetPixmap(QPixmap(itemInfo.icon.c_str()));
        }
    });
}

void SubComboBox::AddItem(const ItemInfo &itemInfo)
{

    QListWidgetItem *item = new QListWidgetItem((ListWidget *)this->view());
    item->setSizeHint(QSize(m_width, m_height));

    ItemWidget *itemWidget = new ItemWidget(itemInfo, this->view());

    ((ListWidget *)this->view())->addItem(item);
    ((ListWidget *)this->view())->setItemWidget(item, itemWidget);
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

QWidget *SubComboBox::View()
{
    return m_view;
}

void SubComboBox::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.setPen(Qt::black);
    painter.drawRect(this->rect());

    // QComboBox::paintEvent(event);
}

void SubComboBox::enterEvent(QEnterEvent *event)
{
    this->setCursor(Qt::PointingHandCursor);
    m_hovered = true;
    //this->update();
    QComboBox::enterEvent(event);
}

void SubComboBox::leaveEvent(QEvent *event)
{
    m_hovered = false;
    //this->update();
    QComboBox::leaveEvent(event);
}

void SubComboBox::showPopup()
{
    
    QComboBox::showPopup();
    ((ListWidget *)this->view())->doItemsLayout();
}

ComboBox::ComboBox(ComboBoxType type, QWidget *parent) : QWidget(parent)
{
    // this->setAttribute(Qt::WA_TranslucentBackground);
    m_type = type;

    QGridLayout *gLayout = new QGridLayout(this);
    gLayout->setContentsMargins(0, 0, 0, 0);
    gLayout->setSpacing(0);
    m_comboBox = new SubComboBox(m_type, this);

    gLayout->addWidget(m_comboBox, 0, 0, 1, 1, Qt::AlignCenter);

    if (m_type == ComboBoxType::ICON_AND_NAME_MODE) {
        m_label = new QLabel(this);
        m_label->setFont(QFont("微软雅黑", 10));

        gLayout->addWidget(m_label, 1, 0, 1, 1, Qt::AlignCenter);
        connect(m_comboBox, &QComboBox::currentIndexChanged, this, [=](int index) {
            ListWidget *listWidget = static_cast<ListWidget *>(m_comboBox->view());
            if (listWidget->currentItem() != nullptr) {
                std::string name =
                    static_cast<ItemWidget *>(listWidget->itemWidget(listWidget->currentItem()))
                        ->GetItemInfo()
                        .name;
                m_label->setText(name.c_str());
            }
        });
    } else {
        // 创建一个阴影效果对象
        QGraphicsDropShadowEffect *shadowEffect = new QGraphicsDropShadowEffect(this);
        shadowEffect->setColor(Qt::black);     // 设置阴影的颜色
        shadowEffect->setBlurRadius(10);       // 设置阴影的模糊半径
        shadowEffect->setOffset(0, 0);         // 设置阴影的偏移量
        this->setGraphicsEffect(shadowEffect); // 为按钮应用阴影效果
    }
}

void ComboBox::AddItem(const ItemInfo &itemInfo)
{
    m_comboBox->AddItem(itemInfo);
    if (m_comboBox->count() == 1) {
        if (m_type == ComboBoxType::ICON_AND_NAME_MODE) {
            m_label->setText(itemInfo.name.c_str());
        } else {
            static_cast<ItemWidget *>(m_comboBox->View())->SetItemInfo(itemInfo);
        }
    }
    update();
}

void ComboBox::SetItemSize(int width, int height)
{
    m_comboBox->SetItemSize(width, height);
}

ItemInfo ComboBox::GetItemInfo() const
{
    ListWidget *listWidget = static_cast<ListWidget *>(m_comboBox->view());
    if (listWidget->currentItem() != nullptr) {
        return static_cast<ItemWidget *>(listWidget->itemWidget(listWidget->currentItem()))->GetItemInfo();
    }
    return ItemInfo();
}

void ComboBox::paintEvent(QPaintEvent *event)
{
    if (m_type == ComboBoxType::ICON_AND_NAME_MODE) {
        m_comboBox->setFixedSize(this->height() - 22, this->height() - 22);
        if (m_hovered) {
            m_label->setStyleSheet("color: rgb(255, 255, 255);");
        } else {
            m_label->setStyleSheet("color: rgb(208, 211, 212);");
        }
    } else {
        m_comboBox->setFixedSize(this->size());
    }
    // QWidget::paintEvent(event);
}

void ComboBox::enterEvent(QEnterEvent *event)
{
    this->setCursor(Qt::PointingHandCursor);
    m_hovered = true;
    //this->update();
    QWidget::enterEvent(event);
}

void ComboBox::leaveEvent(QEvent *event)
{
    m_hovered = false;
    //this->update();
    QWidget::leaveEvent(event);
}
