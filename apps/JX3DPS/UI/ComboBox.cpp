/**
 * Project: JX3DPS
 * File: SpinBox.cpp
 * Description:
 * Created Date: 2023-06-10 08:38:29
 * Author: 难为水
 * -----
 * Last Modified: 2023-07-13 21:39:50
 * Modified By: 难为水
 * -----
 * HISTORY:
 * Date      	By   	Comments
 * ----------	-----	----------------------------------------------------------
 */

#include "ComboBox.h"

#include <QAbstractItemDelegate>
#include <QAbstractTableModel>
#include <QEvent>
#include <QGraphicsDropShadowEffect>
#include <QGridLayout>
#include <QHeaderView>
#include <QPainter>
#include <QTableView>

#include "Core/JX3Params.h"

#include "Button.h"
#include "Network.h"
#include "ThemeColors.h"

const int ROWS = 2;

ComboBoxClass::ComboBoxClass(QWidget *parent) : QComboBox(parent)
{
    this->setAttribute(Qt::WA_TranslucentBackground);
    // 创建一个阴影效果对象
    QGraphicsDropShadowEffect *shadowEffect = new QGraphicsDropShadowEffect(this);
    shadowEffect->setColor(Qt::black);     // 设置阴影的颜色
    shadowEffect->setBlurRadius(10);       // 设置阴影的模糊半径
    shadowEffect->setOffset(0, 0);         // 设置阴影的偏移量
    this->setGraphicsEffect(shadowEffect); // 为按钮应用阴影效果
    this->setStyleSheet("QComboBox QAbstractItemView { border: none; }");
    TableModelClass *model = new TableModelClass();

    QTableView *view = new QTableView(this);
    view->horizontalHeader()->setVisible(false);
    view->verticalHeader()->setVisible(false);
    view->setAlternatingRowColors(false);
    view->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    view->setMouseTracking(true);
    view->setItemDelegate((QAbstractItemDelegate *)new ItemDelegateClass(view));

    view->setFocusPolicy(Qt::NoFocus);
    view->setStyleSheet(
        QString("QTableView{background-color: rgb(%1);"
                "border: none;"
                "color: rgb(%2);"
                "selection-background-color: rgb(56, 60, 67);"
                "selection-color: rgb(%3);"
                "}"
                "QTableView::item{"
                "border: none;"
                "}"
                "QTableView::item:selected{"
                "background-color: rgb(56, 60, 67);"
                "}"
                "QTableView::item:hover{"
                "background-color: rgb(56, 60, 67);"
                "}")
            .arg(TO_STR(COLOR_BACKGROUND_HIGHLIGHT))
            .arg(TO_STR(COLOR_FOCUS))
            .arg(TO_STR(COLOR_FOCUS)));

    this->setModelColumn(3);
    this->setModel(model);
    this->setView(view);

    QWidget *popup = this->findChild<QFrame *>();
    popup->setFixedHeight(30 * 20 / ROWS);
    popup->setFixedWidth(70 * ROWS);

    connect(view, &QTableView::entered, [=](const QModelIndex &index) {
        m_pixmap =
            new QPixmap(QString(":/resources/pics/JX3/%1.png")
                            .arg(QString::fromStdString(JX3DPS::CLASS.at(index.row() * ROWS + index.column()))));
        this->update();
        // emit Signal_CurrentClassChanged(index.row() * ROWS + index.column());
    });

    connect(this, &QComboBox::currentIndexChanged, this, &ComboBoxClass::Signal_CurrentClassChanged);
}

void ComboBoxClass::paintEvent(QPaintEvent *event)
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
        int border = 6;
        painter.drawPixmap(border,
                           border,
                           this->width() - 2 * border,
                           this->height() - 2 * border,
                           m_pixmap->copy(3, 3, 42, 42));
    }
}

void ComboBoxClass::enterEvent(QEnterEvent *event)
{
    this->setCursor(Qt::PointingHandCursor);
}

void ComboBoxClass::showPopup()
{
    QComboBox::showPopup();
    QWidget *popup = this->findChild<QFrame *>();

    popup->move(popup->x(), popup->y() + 5);
}

ItemDelegateClass::ItemDelegateClass(QObject *parent) : QStyledItemDelegate(parent) { }

void ItemDelegateClass::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionViewItem viewOption(option);

    QStyledItemDelegate::paint(painter, option, index);
    painter->fillRect(option.rect, QBrush(QColor(COLOR_BACKGROUND_HIGHLIGHT)));
    if (option.state & QStyle::State_Selected) {
        // 自定义选中背景色
        painter->fillRect(option.rect, QBrush(QColor(COLOR_BACKGROUND_BASE)));
    }

    QFont font("NoWatsFont", 10.5);
    painter->setFont(font);
    painter->setPen(QColor(COLOR_HIGHLIGHT));
    painter->drawText(option.rect,
                      Qt::AlignHCenter | Qt::AlignVCenter,
                      JX3DPS::CLASS.at(index.row() * ROWS + index.column()).c_str());
    painter->drawText(option.rect,
                      Qt::AlignHCenter | Qt::AlignVCenter,
                      JX3DPS::CLASS.at(index.row() * ROWS + index.column()).c_str());
}

QSize ItemDelegateClass::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    return QSize(option.rect.width(), option.rect.height());
}

TableModelClass::TableModelClass(QObject *parent) : QAbstractTableModel(parent) { }

int TableModelClass::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return JX3DPS::CLASS.size() / ROWS;
}

int TableModelClass::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return ROWS;
}

QVariant TableModelClass::data(const QModelIndex &index, int role) const
{
    return QVariant();
}

QVariant TableModelClass::headerData(int section, Qt::Orientation orientation, int role) const
{
    return QVariant();
}

ComboBoxTalent::ComboBoxTalent(QWidget *parent)
{
    this->setAttribute(Qt::WA_TranslucentBackground);
    // 创建一个阴影效果对象
    QGraphicsDropShadowEffect *shadowEffect = new QGraphicsDropShadowEffect(this);
    shadowEffect->setColor(Qt::black);     // 设置阴影的颜色
    shadowEffect->setBlurRadius(10);       // 设置阴影的模糊半径
    shadowEffect->setOffset(0, 0);         // 设置阴影的偏移量
    this->setGraphicsEffect(shadowEffect); // 为按钮应用阴影效果
    this->setMaxVisibleItems(8);
    ListWidgetTalent *listWidgetTalent = new ListWidgetTalent(this);

    this->setModel(listWidgetTalent->model());
    this->setView(listWidgetTalent);

    connect(listWidgetTalent, &ListWidgetTalent::itemClicked, this, [=](QListWidgetItem *item) {
        this->hidePopup();
    });
    connect(listWidgetTalent, &QListWidget::currentItemChanged, this, [=](QListWidgetItem *current, QListWidgetItem *previous) {
        if (previous != nullptr) {
            ((ItemWidgetTalent *)listWidgetTalent->itemWidget(previous))->SetSelected(false);
        }
        ((ItemWidgetTalent *)listWidgetTalent->itemWidget(current))->SetSelected(true);
        m_pixmap  = ((ItemWidgetTalent *)listWidgetTalent->itemWidget(current))->GetIcon();
        m_name    = ((ItemWidgetTalent *)listWidgetTalent->itemWidget(current))->GetName();
        m_id      = ((ItemWidgetTalent *)listWidgetTalent->itemWidget(current))->GetId();
        int index = listWidgetTalent->indexFromItem(current).row();
        this->setCurrentIndex(index);
        update();
    });

    this->setStyleSheet("QComboBox QAbstractItemView { border: none; }");
}

void ComboBoxTalent::AddItem(const TalentInfo &talentInfo)
{
    QListWidgetItem *item = new QListWidgetItem((ListWidgetTalent *)this->view());
    item->setSizeHint(QSize(120, 44));

    ItemWidgetTalent *itemWidget = new ItemWidgetTalent(talentInfo, this->view());
    itemWidget->setFixedSize(item->sizeHint());

    ((ListWidgetTalent *)this->view())->addItem(item);
    ((ListWidgetTalent *)this->view())->setItemWidget(item, itemWidget);
}

QString ComboBoxTalent::GetName() const
{
    return m_name;
}

void ComboBoxTalent::SetTalent(const QString &name)
{
    QListWidgetItem *currentItem = static_cast<ListWidgetTalent *>(this->view())->item(currentIndex());
    static_cast<ItemWidgetTalent *>(static_cast<ListWidgetTalent *>(this->view())->itemWidget(currentItem))
        ->SetSelected(false);
    currentItem->setSelected(false);
    for (int i = 0; i < this->count(); i++) {
        QListWidgetItem  *indexItem = static_cast<ListWidgetTalent *>(this->view())->item(i);
        ItemWidgetTalent *itemWidget =
            static_cast<ItemWidgetTalent *>(static_cast<ListWidgetTalent *>(this->view())->itemWidget(indexItem));
        if (itemWidget->GetName() == name) {
            itemWidget->SetSelected(true);
            m_pixmap = itemWidget->GetIcon();
            m_name   = itemWidget->GetName();
            m_id     = itemWidget->GetId();
            int index = static_cast<ListWidgetTalent *>(this->view())->indexFromItem(indexItem).row();
            this->setCurrentIndex(i);
            update();
            break;
        }
    }
}

int ComboBoxTalent::GetId() const
{
    QListWidgetItem *currentItem = static_cast<ListWidgetTalent *>(this->view())->item(currentIndex());
    return static_cast<ItemWidgetTalent *>(static_cast<ListWidgetTalent *>(this->view())->itemWidget(currentItem))
        ->GetId();
}

void ComboBoxTalent::paintEvent(QPaintEvent *event)
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
        int border = 3;
        painter.drawPixmap(border,
                           border,
                           this->width() - 2 * border,
                           this->height() - 2 * border,
                           m_pixmap->copy(3, 3, 42, 42));
    }
}

void ComboBoxTalent::enterEvent(QEnterEvent *event)
{
    this->setCursor(Qt::PointingHandCursor);
    m_isHovered = true;
    QComboBox::enterEvent(event);
}

void ComboBoxTalent::leaveEvent(QEvent *event)
{
    m_isHovered = false;
    QComboBox::leaveEvent(event);
}

void ComboBoxTalent::showPopup()
{

    QWidget *popup = this->findChild<QFrame *>();
    popup->setFixedWidth(120);
    popup->move(popup->x(), popup->y() + 5);

    QComboBox::showPopup();

    ((ListWidgetTalent *)this->view())->doItemsLayout();
}

ListWidgetTalent::ListWidgetTalent(QWidget *parent) : QListWidget(parent)
{
    this->setAttribute(Qt::WA_TranslucentBackground);
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    // this->setWindowFlags(Qt::Popup);
    // this->setFrameStyle(QFrame::NoFrame);
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setSelectionMode(QAbstractItemView::SingleSelection);
    this->setSelectionBehavior(QAbstractItemView::SelectRows);
    this->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    this->setEditTriggers(QAbstractItemView::NoEditTriggers);
    this->setMouseTracking(true);
    // this->setStyleSheet("QListWidget {border: 0px solid blue;}");
    this->setStyleSheet("QListWidget { border: none; }");
}

ListWidgetTalent::~ListWidgetTalent() { }

QRect ListWidgetTalent::visualRect(const QModelIndex &index) const
{
    QRect rect = QListWidget::visualRect(index);
    return rect;
}

void ListWidgetTalent::wheelEvent(QWheelEvent *event)
{
    QListWidget::wheelEvent(event);
    this->doItemsLayout(); // 重绘，否则会出现不刷新的情况
}

void ListWidgetTalent::paintEvent(QPaintEvent *event)
{
    QStyleOptionViewItem opt;
    opt.initFrom(this);

    QPainter painter(viewport());
    painter.setPen(QPen(QColor(56, 60, 67)));
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);

    QListWidget::paintEvent(event);
}

ItemWidgetTalent::ItemWidgetTalent(const TalentInfo &talentInfo, QWidget *parent) : QWidget(parent)
{
    m_name = QString::fromStdString(talentInfo.name);
    if (talentInfo.iconId != -1) {
        QString url = QString("https://icon.jx3box.com/icon/%1.png").arg(talentInfo.iconId);
        m_pixmap    = new QPixmap(RequestIcon(url));
    }

    m_desc = QString::fromStdString(talentInfo.desc);
    m_id   = talentInfo.id;
    this->setToolTip(QString::fromStdString(talentInfo.desc));

    // 设置样式表
    this->setStyleSheet(
        "QToolTip { color: white; background-color: rgb(23, 29, 37); border: none; }");
}

ItemWidgetTalent::~ItemWidgetTalent() { }

void ItemWidgetTalent::HoveredEvent(bool isHovered)
{
    m_isHovered = isHovered;
    this->update();
}

void ItemWidgetTalent::SetSelected(bool isSelected)
{
    m_selected  = isSelected;
    m_isHovered = false;
    this->update();
}

QPixmap *ItemWidgetTalent::GetIcon() const
{
    return m_pixmap;
}

QString ItemWidgetTalent::GetName() const
{
    return m_name;
}

int ItemWidgetTalent::GetId() const
{
    return m_id;
}

void ItemWidgetTalent::paintEvent(QPaintEvent *event)
{

    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);

    painter.setPen(QPen((m_isHovered || m_selected) ? QColor(COLOR_BACKGROUND_BASE) : QColor(COLOR_BACKGROUND_HIGHLIGHT)));
    painter.setBrush(QBrush((m_isHovered || m_selected) ? QColor(COLOR_BACKGROUND_BASE) : QColor(COLOR_BACKGROUND_HIGHLIGHT)));
    painter.drawRect(this->rect());

    double opacity = (m_isHovered || m_selected) ? 0.8 : 0.6;

    // 画一个框
    int border = 3;
    painter.setPen(QPen(Qt::black));
    painter.setBrush(QBrush(QColor(COLOR_BACKGROUND_HIGHLIGHT)));
    painter.drawRect(border, border, this->height() - border * 2, this->height() - border * 2);

    border = 4;
    if (m_pixmap != nullptr) {
        QIcon       icon(m_pixmap->copy(3, 3, 42, 42));
        QIcon::Mode mode = (m_isHovered || m_selected) ? QIcon::Normal : QIcon::Disabled;
        painter.drawPixmap(border, border, icon.pixmap(m_pixmap->size(), mode).scaledToWidth(this->height() - border * 2));
    }

    painter.setFont(QFont("NoWatsFont", 11));
    painter.setPen(QPen((m_isHovered || m_selected) ? QColor(COLOR_FOCUS) : QColor(COLOR_INACTIVE)));
    painter.drawText(this->height(), 0, this->width() - this->height(), this->height(), Qt::AlignCenter, m_name);
    painter.drawText(this->height(), 0, this->width() - this->height(), this->height(), Qt::AlignCenter, m_name);

    // QWidget::paintEvent(event);
}

void ItemWidgetTalent::enterEvent(QEnterEvent *event)
{
    m_isHovered = true;
    this->update();
    QWidget::enterEvent(event);
}

void ItemWidgetTalent::leaveEvent(QEvent *event)
{
    m_isHovered = false;
    this->update();
    QWidget::leaveEvent(event);
}

TalentWidget::TalentWidget(QWidget *parent) : QWidget(parent)
{
    this->setFixedSize(50, 65);

    QHBoxLayout *hLayout    = new QHBoxLayout;
    QSpacerItem *spacerItem = new QSpacerItem(1, 0, QSizePolicy::Fixed, QSizePolicy::Minimum);

    m_comboBox = new ComboBoxTalent(this);
    m_comboBox->setFixedSize(46, 46);

    hLayout->addItem(spacerItem);
    hLayout->addWidget(m_comboBox);

    QVBoxLayout *vLayout = new QVBoxLayout(this);

    m_button = new TextButton(this);

    vLayout->setContentsMargins(0, 0, 0, 0);
    m_button->setFixedSize(50, 18);

    vLayout->addLayout(hLayout);
    vLayout->addWidget(m_button);

    connect(m_button, &TextButton::clicked, m_comboBox, &QComboBox::showPopup);

    connect(m_comboBox, &QComboBox::activated, [=](int index) {
        m_button->setText(m_comboBox->GetName());
        m_id = m_comboBox->GetId();
    });

    TalentInfo talentInfo;
    talentInfo.name = "无";
    m_button->setText("无");
    m_comboBox->AddItem(talentInfo);
}

void TalentWidget::AddItem(const TalentInfo &talentInfo)
{
    m_comboBox->AddItem(talentInfo);
}

void TalentWidget::SetTalent(const QString &name)
{
    m_comboBox->SetTalent(name);
    m_button->setText(name);
}

int TalentWidget::GetId() const
{
    return m_comboBox->GetId();
}
