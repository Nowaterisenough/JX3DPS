#include "equipment_panel.h"

#include <array>
#include <memory>

#include <QApplication>
#include <QButtonGroup>
#include <QCoreApplication>
#include <QDialog>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QHBoxLayout>
#include <QIcon>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMap>
#include <QNetworkReply>
#include <QPainter>
#include <QPushButton>
#include <QScrollArea>
#include <QShowEvent>
#include <QSignalBlocker>
#include <QStandardPaths>
#include <QTabWidget>
#include <QTextBrowser>
#include <QToolButton>
#include <QUrl>
#include <QVBoxLayout>

#include "../choice_popup.h"

#include "equipment_detail_card.h"

struct EquipmentPanel::Impl
{
    QNetworkAccessManager           network;
    QGridLayout                    *slotGrid        = nullptr;
    QListWidget                    *catalog         = nullptr;
    QListWidget                    *effects         = nullptr;
    QListWidget                    *skills          = nullptr;
    QListWidget                    *talents         = nullptr;
    QListWidget                    *recipes         = nullptr;
    QLabel                         *setSummary      = nullptr;
    QLabel                         *sourceLabel     = nullptr;
    QLabel                         *equippedCount   = nullptr;
    QToolButton                    *effectsToggle   = nullptr;
    QWidget                        *effectsContent  = nullptr;
    desktop::ChoicePopup           *popup           = nullptr;
    desktop::ChoicePopup           *detailsPopup    = nullptr;
    QDialog                        *referenceDialog = nullptr;
    QLabel                         *selectedLabel   = nullptr;
    QToolButton                    *viewDetails     = nullptr;
    bool                            loaded          = false;
    QLabel                         *pickerTitle     = nullptr;
    QLabel                         *emptyResults    = nullptr;
    QLineEdit                      *search          = nullptr;
    QPushButton                    *clear           = nullptr;
    desktop::EquipmentDetailCard   *details         = nullptr;
    desktop::EquipmentDetailCard   *preview         = nullptr;
    QString                         activeSlot;
    QHash<QString, QToolButton *>   slotButtons;
    QHash<QString, QJsonObject>     loadout;
    QHash<QString, QPixmap>         iconCache;
    QHash<QNetworkReply *, QString> replies;
    QJsonArray                      catalogData;
};

namespace {

const QStringList kSlots = {
    QStringLiteral("帽子"),  QStringLiteral("上衣"),  QStringLiteral("腰带"), QStringLiteral("护腕"),
    QStringLiteral("下装"),  QStringLiteral("鞋子"),  QStringLiteral("项链"), QStringLiteral("腰坠"),
    QStringLiteral("戒指1"), QStringLiteral("戒指2"), QStringLiteral("武器"), QStringLiteral("特殊")
};

QString QualityColor(int quality)
{
    return desktop::EquipmentDetailCard::QualityColor(quality);
}

QPixmap PlaceholderIcon(int iconId)
{
    QPixmap pixmap(52, 52);
    pixmap.fill(QColor(48, 53, 62));
    QPainter painter(&pixmap);
    painter.setPen(QColor(160, 170, 185));
    painter.drawRect(1, 1, 50, 50);
    painter.drawText(pixmap.rect(), Qt::AlignCenter, (iconId != 0) ? QStringLiteral("装备") : QStringLiteral("空"));
    return pixmap;
}

QPixmap EmptySlotIcon(const QString &slot)
{
    QPixmap pixmap(40, 40);
    pixmap.fill(Qt::transparent);
    QPainter p(&pixmap);
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(QPen(QColor("#71838c"), 1.5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    p.setBrush(QColor("#233139"));
    if (slot == QStringLiteral("帽子")) {
        p.drawRoundedRect(QRectF(9, 11, 22, 19), 8, 8);
        p.drawLine(6, 30, 34, 30);
        p.drawLine(20, 11, 20, 23);
    } else if (slot == QStringLiteral("上衣")) {
        p.drawPolygon(QPolygon{
            { 13, 8  },
            { 7,  13 },
            { 4,  22 },
            { 11, 24 },
            { 13, 18 },
            { 13, 33 },
            { 28, 33 },
            { 28, 18 },
            { 30, 24 },
            { 36, 22 },
            { 33, 13 },
            { 27, 8  },
            { 20, 13 }
        });
        p.drawLine(20, 14, 20, 31);
    } else if (slot == QStringLiteral("下装")) {
        p.drawPolygon(QPolygon{
            { 10, 8  },
            { 30, 8  },
            { 29, 33 },
            { 22, 33 },
            { 20, 21 },
            { 18, 33 },
            { 11, 33 }
        });
        p.drawLine(11, 13, 29, 13);
    } else if (slot == QStringLiteral("鞋子")) {
        p.drawPolygon(QPolygon{
            { 18, 8  },
            { 29, 8  },
            { 29, 31 },
            { 8,  31 },
            { 8,  26 },
            { 17, 23 }
        });
        p.drawLine(19, 13, 28, 13);
    } else if (slot == QStringLiteral("腰带")) {
        p.drawRoundedRect(QRectF(5, 15, 30, 12), 2, 2);
        p.drawRoundedRect(QRectF(16, 12, 9, 18), 2, 2);
    } else if (slot == QStringLiteral("护腕")) {
        p.drawPolygon(QPolygon{
            { 12, 8  },
            { 29, 11 },
            { 27, 33 },
            { 10, 29 }
        });
        p.drawLine(13, 16, 28, 19);
        p.drawLine(12, 22, 27, 25);
    } else if (slot.startsWith(QStringLiteral("戒指"))) {
        p.drawEllipse(QRectF(10, 13, 20, 20));
        p.drawPolygon(QPolygon{
            { 20, 6  },
            { 26, 12 },
            { 20, 18 },
            { 14, 12 }
        });
    } else if (slot == QStringLiteral("项链")) {
        p.drawArc(QRectF(8, 3, 24, 25), 180 * 16, 180 * 16);
        p.drawPolygon(QPolygon{
            { 20, 25 },
            { 25, 30 },
            { 20, 36 },
            { 15, 30 }
        });
    } else if (slot == QStringLiteral("腰坠")) {
        p.drawLine(20, 5, 20, 12);
        p.drawRoundedRect(QRectF(12, 12, 16, 18), 5, 5);
        p.drawLine(16, 30, 14, 35);
        p.drawLine(24, 30, 26, 35);
    } else if (slot == QStringLiteral("武器")) {
        p.drawPolygon(QPolygon{
            { 29, 6  },
            { 30, 15 },
            { 15, 30 },
            { 10, 25 }
        });
        p.drawLine(8, 21, 19, 32);
        p.drawLine(13, 27, 7, 34);
    } else {
        p.drawRoundedRect(QRectF(11, 11, 19, 23), 5, 5);
        p.drawLine(14, 7, 27, 7);
        p.drawLine(14, 7, 17, 12);
        p.drawLine(27, 7, 24, 12);
        p.drawLine(13, 18, 28, 18);
    }
    return pixmap;
}

QStringList SocketLabels(const QJsonValue &value)
{
    QStringList labels;
    if (value.isString() && !value.toString().isEmpty()) {
        labels << value.toString();
    }
    for (const auto &entry : value.toArray()) {
        const QString LABEL = entry.isObject() ? entry.toObject().value("label").toString() : entry.toString();
        if (!LABEL.isEmpty()) {
            labels << LABEL;
        }
    }
    return labels;
}

class SlotButton final : public QToolButton
{
public:
    using QToolButton::QToolButton;

protected:
    void paintEvent(QPaintEvent *event) override
    {
        QToolButton::paintEvent(event);
        QPainter painter(this);
        painter.setPen(Qt::NoPen);
        painter.setBrush(QColor(227, 232, 240));
        painter.drawPolygon(QPolygon{ QPoint(width() - 11, height() - 9),
                                      QPoint(width() - 3, height() - 9),
                                      QPoint(width() - 7, height() - 5) });
    }
};

} // namespace

EquipmentPanel::EquipmentPanel(QWidget *parent) : QWidget(parent), d(std::make_unique<Impl>())
{
    setObjectName(QStringLiteral("equipmentPanel"));
    setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred);
    BuildUi();
    connect(&d->network, &QNetworkAccessManager::finished, this, &EquipmentPanel::IconDownloaded);
}

EquipmentPanel::~EquipmentPanel() = default;

void EquipmentPanel::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    if (!d->loaded) {
        LoadLatestData();
    }
}

void EquipmentPanel::hideEvent(QHideEvent *event)
{
    d->popup->hide();
    d->detailsPopup->hide();
    d->referenceDialog->hide();
    QWidget::hideEvent(event);
}

void EquipmentPanel::BuildUi()
{
    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(6);

    auto *title_row = new QHBoxLayout();
    auto *title     = new QLabel(QStringLiteral("太虚剑意"), this);
    title->setStyleSheet(QStringLiteral("font-size: 13px; font-weight: 600; color: #c4d9d2;"));
    d->equippedCount = new QLabel(QStringLiteral("0 / 12"), this);
    d->equippedCount->setObjectName(QStringLiteral("equippedCount"));
    d->equippedCount->setStyleSheet(QStringLiteral("color: #84979e; font-size: 11px;"));
    d->sourceLabel = new QLabel(QStringLiteral("读取中…"), this);
    d->sourceLabel->setWordWrap(true);
    d->sourceLabel->setStyleSheet(QStringLiteral("font-size: 10px; color: #8d99a8;"));
    title_row->addWidget(title);
    title_row->addWidget(d->equippedCount);
    title_row->addStretch();
    auto *references = new QToolButton(this);
    references->setObjectName(QStringLiteral("equipmentReferences"));
    references->setText(QStringLiteral("资料"));
    references->setAutoRaise(true);
    references->setToolTip(QStringLiteral("查看太虚剑意的技能、奇穴与秘籍资料"));
    title_row->addWidget(references);
    root->addLayout(title_row);

    d->referenceDialog = new QDialog(this);
    d->referenceDialog->setObjectName(QStringLiteral("equipmentReferenceDialog"));
    d->referenceDialog->setWindowTitle(QStringLiteral("太虚剑意 · 魔盒资料"));
    d->referenceDialog->resize(640, 640);
    auto *reference_layout = new QVBoxLayout(d->referenceDialog);
    auto *tabs             = new QTabWidget(d->referenceDialog);
    reference_layout->addWidget(tabs);
    connect(references, &QToolButton::clicked, this, [this] {
        d->referenceDialog->show();
        d->referenceDialog->raise();
        d->referenceDialog->activateWindow();
    });
    auto *equipment_tab    = new QWidget(this);
    auto *equipment_layout = new QVBoxLayout(equipment_tab);
    equipment_layout->setContentsMargins(0, 0, 0, 0);
    equipment_layout->setSpacing(6);

    auto *sidebar = new QVBoxLayout;
    sidebar->setSpacing(6);
    equipment_layout->addLayout(sidebar);

    auto *slots_widget = new QWidget(equipment_tab);
    slots_widget->setObjectName(QStringLiteral("equipmentSlots"));
    slots_widget->setToolTip(QStringLiteral("点击部位图标更换装备"));
    d->slotGrid = new QGridLayout(slots_widget);
    d->slotGrid->setContentsMargins(0, 0, 0, 0);
    d->slotGrid->setHorizontalSpacing(4);
    d->slotGrid->setVerticalSpacing(3);
    for (const auto &[text, COLUMN] :
         {
             std::pair{ QStringLiteral("防具"),        0 },
             std::pair{ QStringLiteral("首饰 / 兵器"), 3 }
    })
    {
        auto *label = new QLabel(text, slots_widget);
        label->setStyleSheet(QStringLiteral("color: #8da4ac; font-size: 11px;"));
        label->setAlignment(Qt::AlignCenter);
        d->slotGrid->addWidget(label, 0, COLUMN, 1, 2);
    }
    auto *divider = new QFrame(slots_widget);
    divider->setFixedWidth(1);
    divider->setStyleSheet(QStringLiteral("background: #2c3b43;"));
    d->slotGrid->addWidget(divider, 0, 2, 4, 1);
    auto *slot_selection = new QButtonGroup(this);
    slot_selection->setExclusive(true);
    // Keep stable slot identifiers while arranging armor and accessories in two columns each.
    const std::array<QPoint, 12> POSITIONS{
        { { 0, 1 }, { 1, 1 }, { 1, 2 }, { 0, 2 }, { 0, 3 }, { 1, 3 }, { 3, 1 }, { 4, 1 }, { 3, 2 }, { 4, 2 }, { 3, 3 }, { 4, 3 } }
    };
    for (int i = 0; i < kSlots.size(); ++i) {
        const QString &slot        = kSlots.at(i);
        auto          *cell        = new QWidget(slots_widget);
        auto          *cell_layout = new QVBoxLayout(cell);
        cell_layout->setContentsMargins(0, 1, 0, 1);
        cell_layout->setSpacing(1);
        auto *button = new SlotButton(cell);
        button->setObjectName(QStringLiteral("equipmentSlot%1").arg(i));
        button->setProperty("equipmentSlot", slot);
        button->setToolButtonStyle(Qt::ToolButtonIconOnly);
        button->setIconSize(QSize(36, 36));
        button->setFixedSize(44, 44);
        button->setCheckable(true);
        slot_selection->addButton(button);
        button->setCursor(Qt::PointingHandCursor);
        cell_layout->addWidget(button, 0, Qt::AlignHCenter);
        auto *caption = new QLabel(slot, cell);
        caption->setStyleSheet(QStringLiteral("font-size: 11px; color: #b2c0c8;"));
        caption->setAlignment(Qt::AlignCenter);
        cell_layout->addWidget(caption);
        d->slotButtons.insert(slot, button);
        UpdateSlotButton(slot);
        connect(button, &QToolButton::clicked, this, [this, slot] { OpenSlot(slot); });
        d->slotGrid->addWidget(cell, POSITIONS[i].y(), POSITIONS[i].x());
    }
    sidebar->addWidget(slots_widget);

    auto *selection_row = new QHBoxLayout;
    d->selectedLabel    = new QLabel(QStringLiteral("点击图标选择装备"), equipment_tab);
    d->selectedLabel->setObjectName(QStringLiteral("selectedEquipment"));
    d->selectedLabel->setWordWrap(true);
    d->selectedLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred);
    selection_row->addWidget(d->selectedLabel, 1);
    d->viewDetails = new QToolButton(equipment_tab);
    d->viewDetails->setObjectName(QStringLiteral("viewEquipmentDetails"));
    d->viewDetails->setText(QStringLiteral("详情"));
    d->viewDetails->setAutoRaise(true);
    d->viewDetails->setEnabled(false);
    selection_row->addWidget(d->viewDetails);
    sidebar->addLayout(selection_row);
    d->detailsPopup = new desktop::ChoicePopup(this);
    d->detailsPopup->setObjectName(QStringLiteral("equipmentDetailsPopup"));
    auto *details_layout = new QVBoxLayout(d->detailsPopup);
    details_layout->setContentsMargins(1, 1, 1, 1);
    d->details       = new desktop::EquipmentDetailCard(d->detailsPopup);
    d->details->icon = [this](int id) {
        return RequestIcon(id).pixmap(32, 32);
    };
    d->details->setObjectName(QStringLiteral("equipmentDetails"));
    d->details->setOpenLinks(false);
    d->details->setMinimumHeight(180);
    d->details->SetContentHtml(ItemDetails({}, QStringLiteral("装备")));
    details_layout->addWidget(d->details);
    d->detailsPopup->Watch(d->details);
    connect(d->viewDetails, &QToolButton::clicked, this, [this] {
        if (d->detailsPopup->isVisible()) {
            d->detailsPopup->hide();
        } else {
            d->detailsPopup->Open(d->viewDetails, { 750, 900 }, d->details, desktop::ChoicePopup::BESIDE);
        }
    });

    d->effectsToggle = new QToolButton(this);
    d->effectsToggle->setObjectName(QStringLiteral("equipmentEffectsToggle"));
    d->effectsToggle->setCheckable(true);
    d->effectsToggle->setAutoRaise(true);
    d->effectsToggle->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    d->effectsToggle->setArrowType(Qt::RightArrow);
    sidebar->addWidget(d->effectsToggle);
    d->effectsContent   = new QWidget(this);
    auto *effect_layout = new QVBoxLayout(d->effectsContent);
    effect_layout->setContentsMargins(0, 0, 0, 0);
    effect_layout->setSpacing(4);
    sidebar->addWidget(d->effectsContent);
    d->effectsContent->hide();
    connect(d->effectsToggle, &QToolButton::toggled, this, [this](bool expanded) {
        d->effectsToggle->setArrowType(expanded ? Qt::DownArrow : Qt::RightArrow);
        d->effectsContent->setVisible(expanded);
    });
    d->setSummary = new QLabel(QStringLiteral("尚未装备套装"), this);
    d->setSummary->setWordWrap(true);
    d->setSummary->setStyleSheet(QStringLiteral("color: #9ca8b8; font-size: 11px;"));
    effect_layout->addWidget(d->setSummary);
    d->effects = new QListWidget(this);
    d->effects->setViewMode(QListView::IconMode);
    d->effects->setIconSize(QSize(36, 36));
    d->effects->setObjectName(QStringLiteral("equipmentEffects"));
    d->effects->setGridSize(QSize(76, 58));
    d->effects->setResizeMode(QListView::Adjust);
    d->effects->setMovement(QListView::Static);
    d->effects->setMaximumHeight(68);
    d->effects->setStyleSheet(QStringLiteral("QListWidget { background: #1a2029; border: 1px solid #303a49; }"
                                             "QListWidget::item { color: #cbd5e1; padding: 1px; }"));
    effect_layout->addWidget(d->effects);
    d->sourceLabel->setToolTip(QStringLiteral("装备选择用于预览；模拟属性以「属性」页为准。"));

    d->popup           = new desktop::ChoicePopup(this);
    auto *popup_layout = new QVBoxLayout(d->popup);
    popup_layout->setContentsMargins(10, 10, 10, 10);
    popup_layout->setSpacing(8);
    auto *picker_header = new QHBoxLayout;
    d->pickerTitle      = new QLabel(d->popup);
    d->pickerTitle->setStyleSheet(QStringLiteral("font-weight: 600; color: #f4c76b;"));
    d->clear = new QPushButton(QStringLiteral("卸下装备"), d->popup);
    d->clear->setObjectName(QStringLiteral("clearEquipment"));
    d->clear->setAutoDefault(false);
    picker_header->addWidget(d->pickerTitle, 1);
    picker_header->addWidget(d->clear);
    popup_layout->addLayout(picker_header);
    d->search = new QLineEdit(d->popup);
    d->search->setObjectName(QStringLiteral("equipmentSearch"));
    d->search->setPlaceholderText(QStringLiteral("搜索名称、属性或来源"));
    auto *body       = new QHBoxLayout;
    auto *candidates = new QVBoxLayout;
    candidates->addWidget(d->search);
    d->catalog = new QListWidget(d->popup);
    d->catalog->setObjectName(QStringLiteral("equipmentChoices"));
    d->catalog->setViewMode(QListView::IconMode);
    d->catalog->setIconSize(QSize(48, 48));
    d->catalog->setGridSize(QSize(80, 86));
    d->catalog->setResizeMode(QListView::Adjust);
    d->catalog->setMovement(QListView::Static);
    d->catalog->setWordWrap(true);
    d->catalog->setMouseTracking(true);
    d->catalog->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    d->catalog->setMinimumSize(0, 0);
    d->catalog->setStyleSheet(QStringLiteral("QListWidget { background: #1a2029; border: 1px solid #303a49; }"
                                             "QListWidget::item { color: #cbd5e1; padding: 2px; }"
                                             "QListWidget::item:selected { background: #3b4657; }"));
    connect(d->catalog, &QListWidget::itemClicked, this, &EquipmentPanel::EquipCandidate);
    const auto PREVIEW_ITEM = [this](QListWidgetItem *item) {
        d->preview->SetContentHtml(item ? ItemDetails(item->data(Qt::UserRole).toJsonObject(), d->activeSlot) : QString());
    };
    connect(d->catalog, &QListWidget::currentItemChanged, this, [PREVIEW_ITEM](QListWidgetItem *item) {
        PREVIEW_ITEM(item);
    });
    connect(d->catalog, &QListWidget::itemEntered, this, PREVIEW_ITEM);
    candidates->addWidget(d->catalog, 1);
    d->emptyResults = new QLabel(QStringLiteral("当前部位没有匹配装备"), d->popup);
    d->emptyResults->setAlignment(Qt::AlignCenter);
    candidates->addWidget(d->emptyResults);
    body->addLayout(candidates, 1);
    d->preview       = new desktop::EquipmentDetailCard(d->popup);
    d->preview->icon = [this](int id) {
        return RequestIcon(id).pixmap(32, 32);
    };
    d->preview->setObjectName(QStringLiteral("equipmentCandidateDetails"));
    d->preview->setOpenLinks(false);
    d->preview->setMinimumSize(0, 0);
    body->addWidget(d->preview, 3);
    popup_layout->addLayout(body, 1);
    auto *picker_hint = new QLabel(QStringLiteral("单击或回车选择 · Esc 取消"), d->popup);
    picker_hint->setStyleSheet(QStringLiteral("color: #9ca8b8; font-size: 11px;"));
    popup_layout->addWidget(picker_hint);
    for (QWidget *widget : std::initializer_list<QWidget *>{ d->catalog, d->search, d->preview, d->clear }) {
        d->popup->Watch(widget);
    }
    d->popup->acceptChoice = [this] {
        if (d->clear->hasFocus()) {
            d->clear->click();
        } else {
            EquipCandidate(d->catalog->currentItem());
        }
    };
    connect(d->search, &QLineEdit::textChanged, this, &EquipmentPanel::FilterCandidates);
    connect(d->clear, &QPushButton::clicked, this, [this] {
        const QString slot = d->activeSlot;
        d->popup->hide();
        SetSlotItem(slot, {});
    });

    const auto CREATE_ICON_LIST = [tabs](const QString &title) {
        auto *list = new QListWidget(tabs);
        list->setViewMode(QListView::IconMode);
        list->setIconSize(QSize(48, 48));
        list->setGridSize(QSize(112, 82));
        list->setResizeMode(QListView::Adjust);
        list->setMovement(QListView::Static);
        list->setSpacing(2);
        list->setStyleSheet(QStringLiteral("QListWidget { background: #1a2029; border: 1px solid #303a49; }"
                                           "QListWidget::item { color: #cbd5e1; padding: 2px; }"
                                           "QListWidget::item:selected { background: #3b4657; }"));
        tabs->addTab(list, title);
        return list;
    };
    d->skills  = CREATE_ICON_LIST(QStringLiteral("技能"));
    d->talents = CREATE_ICON_LIST(QStringLiteral("奇穴"));
    d->recipes = CREATE_ICON_LIST(QStringLiteral("秘籍"));
    root->addWidget(equipment_tab);
    root->addWidget(d->sourceLabel);
}

QString EquipmentPanel::FindSnapshotPath(const QString &requested)
{
    QStringList candidates;
    if (!requested.isEmpty()) {
        candidates << requested;
    }
    candidates << QDir::current().filePath(QStringLiteral("data/jx3box/tai_xu_jian_yi_latest.json"));
    QDir app_dir(QCoreApplication::applicationDirPath());
#ifdef Q_OS_MACOS
    candidates << app_dir.filePath(QStringLiteral("../Resources/data/jx3box/tai_xu_jian_yi_latest.json"));
#endif
    for (int i = 0; i < 5; ++i) {
        candidates << app_dir.filePath(QStringLiteral("data/jx3box/tai_xu_jian_yi_latest.json"));
        candidates << app_dir.filePath(QStringLiteral("../../data/jx3box/tai_xu_jian_yi_latest.json"));
        if (!app_dir.cdUp()) {
            break;
        }
    }
    for (const auto &candidate : candidates) {
        const QFileInfo INFO(candidate);
        if (INFO.exists() && INFO.isFile()) {
            return INFO.absoluteFilePath();
        }
    }
    return QString();
}

void EquipmentPanel::LoadLatestData(const QString &snapshot_path)
{
    d->loaded          = true;
    const QString PATH = FindSnapshotPath(snapshot_path);
    if (PATH.isEmpty()) {
        d->sourceLabel->setText(QStringLiteral("魔盒快照缺失"));
        return;
    }
    QFile file(PATH);
    if (!file.open(QIODevice::ReadOnly)) {
        d->sourceLabel->setText(QStringLiteral("快照读取失败"));
        return;
    }
    QJsonParseError error;
    const auto      document = QJsonDocument::fromJson(file.readAll(), &error);
    if (error.error != QJsonParseError::NoError || !document.isObject()) {
        d->sourceLabel->setText(QStringLiteral("快照格式错误"));
        return;
    }
    d->sourceLabel->setText(
        QStringLiteral("装备预览 · 魔盒 %1").arg(document.object().value(QStringLiteral("data_version")).toString()));
    PopulateFromSnapshot(document.object());
}

QString EquipmentPanel::SlotForItem(const QJsonObject &item)
{
    QString type = item.value(QStringLiteral("type")).toString();
    if (type == QStringLiteral("戒指")) {
        return QStringLiteral("戒指1");
    }
    if (type == QStringLiteral("腰坠") || type == QStringLiteral("项链")) {
        return type;
    }
    if (type == QStringLiteral("投掷")) {
        return QStringLiteral("特殊");
    }
    if (type.contains(QStringLiteral("短兵")) || type == QStringLiteral("武器")) {
        return QStringLiteral("武器");
    }
    if (!type.isEmpty()) {
        return type;
    }
    const QString NAME = item.value(QStringLiteral("name")).toString();
    if (NAME.contains(QStringLiteral("腰坠"))) {
        return QStringLiteral("腰坠");
    }
    if (NAME.contains(QStringLiteral("链"))) {
        return QStringLiteral("项链");
    }
    if (NAME.contains(QStringLiteral("戒"))) {
        return QStringLiteral("戒指1");
    }
    return QStringLiteral("特殊");
}

QString EquipmentPanel::IconUrl(int icon_id)
{
    return QStringLiteral("https://icon.jx3box.com/icon/%1.png").arg(icon_id);
}

void EquipmentPanel::PopulateFromSnapshot(const QJsonObject &snapshot)
{
    d->popup->hide();
    d->catalog->clear();
    d->effects->clear();
    d->skills->clear();
    d->talents->clear();
    d->recipes->clear();
    d->catalogData = snapshot.value(QStringLiteral("equipment_catalog")).toArray();

    const auto SKILL   = snapshot.value(QStringLiteral("skill")).toObject();
    const auto REMARKS = SKILL.value(QStringLiteral("remarks")).toArray();
    for (const auto &remark_value : REMARKS) {
        const auto REMARK = remark_value.toObject();
        for (const auto &skill_value : REMARK.value(QStringLiteral("forceSkills")).toArray()) {
            const auto ITEM = skill_value.toObject();
            AddIconItem(d->skills,
                        ITEM.value(QStringLiteral("skillName")).toString(),
                        ITEM.value(QStringLiteral("desc")).toString(),
                        ITEM.value(QStringLiteral("icon"))
                            .toObject()
                            .value(QStringLiteral("FileName"))
                            .toString()
                            .split('/')
                            .last()
                            .remove(".png")
                            .toInt());
        }
    }
    const auto talent = snapshot.value(QStringLiteral("talent")).toObject();
    for (const auto &level_value : talent.value(QStringLiteral("kungfuLevel")).toArray()) {
        for (const auto &talent_value : level_value.toObject().value(QStringLiteral("kungfuSkills")).toArray()) {
            const auto ITEM = talent_value.toObject();
            AddIconItem(d->talents,
                        QStringLiteral("%1\n第%2层")
                            .arg(ITEM.value(QStringLiteral("name")).toString())
                            .arg(ITEM.value(QStringLiteral("level")).toInt()),
                        ITEM.value(QStringLiteral("desc")).toString(),
                        ITEM.value(QStringLiteral("icon"))
                            .toObject()
                            .value(QStringLiteral("FileName"))
                            .toString()
                            .split('/')
                            .last()
                            .remove(".png")
                            .toInt());
        }
    }
    for (const auto &recipe_value : snapshot.value(QStringLiteral("recipe_catalog")).toArray()) {
        const auto ITEM = recipe_value.toObject();
        AddIconItem(d->recipes,
                    ITEM.value(QStringLiteral("name")).toString(),
                    ITEM.value(QStringLiteral("description")).toString(),
                    ITEM.value(QStringLiteral("icon_id")).toInt());
    }

    UpdateEffects();
}

void EquipmentPanel::EquipCandidate(QListWidgetItem *item)
{
    if (item == nullptr) {
        return;
    }
    const auto    object = item->data(Qt::UserRole).toJsonObject();
    const QString slot   = d->activeSlot;
    d->popup->hide();
    SetSlotItem(slot, object);
}

void EquipmentPanel::SetSlotItem(const QString &slot, const QJsonObject &item)
{
    if (!d->slotButtons.contains(slot)) {
        return;
    }
    const bool CHANGED = d->loadout.value(slot) != item;
    if (item.isEmpty()) {
        d->loadout.remove(slot);
    } else {
        d->loadout.insert(slot, item);
    }
    UpdateSlotButton(slot);
    d->selectedLabel->setText(slot + QStringLiteral("：") +
                              (item.isEmpty() ? QStringLiteral("未装备") : item.value("name").toString()));
    d->viewDetails->setEnabled(!item.isEmpty());
    d->details->SetContentHtml(ItemDetails(item, slot));
    UpdateEffects();
    if (CHANGED) {
        emit LoadoutChanged(Loadout());
    }
}

void EquipmentPanel::UpdateSlotButton(const QString &slot)
{
    auto *button = d->slotButtons.value(slot, nullptr);
    if (button == nullptr) {
        return;
    }
    const auto    ITEM    = d->loadout.value(slot);
    const QString NAME    = ITEM.value(QStringLiteral("name")).toString();
    const int     ICON_ID = ITEM.value(QStringLiteral("icon_id")).toInt();
    button->setText(slot);
    button->setAccessibleName(slot + QStringLiteral("：") + (NAME.isEmpty() ? QStringLiteral("未装备") : NAME));
    button->setToolTip(ItemDetails(ITEM, slot));
    button->setIcon(ITEM.isEmpty() ? QIcon(EmptySlotIcon(slot)) : RequestIcon(ICON_ID));
    button->setStyleSheet(
        QStringLiteral("QToolButton { background: #202631; border: 1px solid %1; border-radius: 4px; padding: 2px; }"
                       "QToolButton:hover, QToolButton:focus { background: #2b3442; border: 1px solid #f4c76b; }"
                       "QToolButton:checked { background: #34443e; border: 2px solid #85bfa8; }")
            .arg(ITEM.isEmpty() ? QStringLiteral("#536071") : QualityColor(ITEM.value("quality").toInt())));
}

void EquipmentPanel::OpenSlot(const QString &slot)
{
    if (d->popup->isVisible() && d->activeSlot == slot) {
        d->popup->hide();
        return;
    }
    d->popup->hide();
    d->detailsPopup->hide();
    d->activeSlot       = slot;
    const auto EQUIPPED = d->loadout.value(slot);
    d->selectedLabel->setText(slot + QStringLiteral("：") +
                              (EQUIPPED.isEmpty() ? QStringLiteral("未装备") : EQUIPPED.value("name").toString()));
    d->viewDetails->setEnabled(!EQUIPPED.isEmpty());
    d->details->SetContentHtml(ItemDetails(d->loadout.value(slot), slot));
    d->clear->setEnabled(d->loadout.contains(slot));
    const QSignalBlocker BLOCKER(d->search);
    d->search->clear();
    FilterCandidates();
    d->popup->Open(d->slotButtons.value(slot), { 980, 720 }, d->catalog, desktop::ChoicePopup::BESIDE);
    d->catalog->scrollToItem(d->catalog->currentItem());
}

void EquipmentPanel::FilterCandidates()
{
    d->catalog->clear();
    const QString    FILTER   = d->search->text().trimmed();
    const QString    type     = d->activeSlot == QStringLiteral("戒指2") ? QStringLiteral("戒指1") : d->activeSlot;
    QListWidgetItem *selected = nullptr;
    for (const auto &value : d->catalogData) {
        const auto ITEM = value.toObject();
        if (SlotForItem(ITEM) != type) {
            continue;
        }
        const QString DETAILS = ItemDetails(ITEM, d->activeSlot);
        QTextDocument searchable;
        searchable.setHtml(DETAILS);
        if (!FILTER.isEmpty() && !(searchable.toPlainText() + ITEM.value("magic_type").toString()).contains(FILTER, Qt::CaseInsensitive))
        {
            continue;
        }
        AddIconItem(d->catalog, ITEM.value("name").toString(), DETAILS, ITEM.value("icon_id").toInt());
        auto *entry = d->catalog->item(d->catalog->count() - 1);
        entry->setData(Qt::UserRole, ITEM);
        if (ITEM == d->loadout.value(d->activeSlot)) {
            selected = entry;
        }
    }
    d->pickerTitle->setText(QStringLiteral("%1 · %2 件候选").arg(d->activeSlot).arg(d->catalog->count()));
    d->emptyResults->setVisible(d->catalog->count() == 0);
    d->catalog->setCurrentItem((selected != nullptr) ? selected : d->catalog->item(0));
    if (d->catalog->count() == 0) {
        d->preview->SetContentHtml(QStringLiteral("<p>没有匹配的装备，请尝试其他名称或属性。</p>"));
    }
}

QString EquipmentPanel::ItemDetails(const QJsonObject &item, const QString &slot) const
{
    return desktop::EquipmentDetailCard::Html(item, Loadout().value("items").toObject(), slot);
}

void EquipmentPanel::UpdateEffects()
{
    d->effects->clear();
    QMap<QString, QList<QString>> sets;
    int                           sockets_count = 0;
    for (const auto &slot : kSlots) {
        const auto ITEM = d->loadout.value(slot);
        const auto NAME = ITEM.value("set").toObject().value("name").toString();
        if (!NAME.isEmpty()) {
            sets[NAME].append(slot);
        }
        const auto SOCKETS  = SocketLabels(ITEM.value("diamonds"));
        sockets_count      += SOCKETS.size();
        if (!SOCKETS.isEmpty()) {
            AddIconItem(d->effects, slot + QStringLiteral("镶嵌"), SOCKETS.join('\n'), ITEM.value("icon_id").toInt());
        }
    }
    QStringList summary;
    for (auto it = sets.cbegin(); it != sets.cend(); ++it) {
        const auto  ITEM    = d->loadout.value(it.value().first());
        const auto  BONUSES = ITEM.value("set").toObject().value("attributes").toObject();
        QStringList lines{ QStringLiteral("%1 · 已装备 %2 件").arg(it.key()).arg(it.value().size()) };
        for (auto bonus = BONUSES.begin(); bonus != BONUSES.end(); ++bonus) {
            lines << QStringLiteral("%1件：%2（%3）")
                         .arg(bonus.key(),
                              bonus.value().toString(),
                              bonus.key().toInt() > 0 && it.value().size() >= bonus.key().toInt()
                                  ? QStringLiteral("已激活")
                                  : QStringLiteral("未激活"));
        }
        summary << lines.first();
        AddIconItem(d->effects,
                    QStringLiteral("套装 %1 件").arg(it.value().size()),
                    lines.join('\n'),
                    ITEM.value("icon_id").toInt());
    }
    d->setSummary->setText(summary.isEmpty() ? QStringLiteral("尚未装备套装") : summary.join(QStringLiteral("；")));
    d->equippedCount->setText(QStringLiteral("%1 / 12").arg(d->loadout.size()));
    d->effectsToggle->setText(QStringLiteral("套装 %1 · 镶嵌 %2").arg(sets.size()).arg(sockets_count));
    d->effectsToggle->setVisible(d->effects->count() > 0);
    d->effectsContent->setVisible(d->effects->count() > 0 && d->effectsToggle->isChecked());
    d->effects->setVisible(d->effects->count() > 0);
    for (const auto &slot : kSlots) {
        d->slotButtons.value(slot)->setToolTip(ItemDetails(d->loadout.value(slot), slot));
    }
}

void EquipmentPanel::AddIconItem(QListWidget *list, const QString &name, const QString &description, int icon_id)
{
    auto *entry = new QListWidgetItem(name, list);
    entry->setToolTip(description);
    entry->setData(Qt::UserRole + 1, icon_id);
    entry->setIcon(RequestIcon(icon_id));
}

QJsonObject EquipmentPanel::Loadout() const
{
    QJsonObject result;
    QJsonObject items;
    for (auto it = d->loadout.cbegin(); it != d->loadout.cend(); ++it) {
        items.insert(it.key(), it.value());
    }
    result.insert(QStringLiteral("items"), items);
    return result;
}

QIcon EquipmentPanel::RequestIcon(int icon_id)
{
    if (icon_id <= 0) {
        return QIcon(PlaceholderIcon(0));
    }
    const QString KEY = QString::number(icon_id);
    if (d->iconCache.contains(KEY)) {
        return QIcon(d->iconCache.value(KEY));
    }
    const QString LOCAL_PATH = QStringLiteral(":/resources/images/JX3/Icons/%1.png").arg(icon_id);
    const QPixmap LOCAL(LOCAL_PATH);
    if (!LOCAL.isNull()) {
        d->iconCache.insert(KEY, LOCAL);
        return QIcon(LOCAL);
    }
    bool pending = false;
    for (auto it = d->replies.cbegin(); it != d->replies.cend(); ++it) {
        if (it.value() == KEY) {
            pending = true;
            break;
        }
    }
    if (!pending) {
        QNetworkRequest request(QUrl(IconUrl(icon_id)));
        request.setTransferTimeout(15000);
        auto *reply = d->network.get(request);
        d->replies.insert(reply, KEY);
    }
    return QIcon(PlaceholderIcon(icon_id));
}

void EquipmentPanel::IconDownloaded(QNetworkReply *reply)
{
    const QString KEY    = d->replies.take(reply);
    const QPixmap PIXMAP = QPixmap::fromImage(QImage::fromData(reply->readAll()));
    reply->deleteLater();
    if (PIXMAP.isNull()) {
        return;
    }
    d->iconCache.insert(KEY, PIXMAP);
    // Refresh only live entries: filtering or reloading may have deleted the original item.
    for (const auto *list : { d->catalog, d->effects, d->skills, d->talents, d->recipes }) {
        for (int row = 0; row < list->count(); ++row) {
            if (list->item(row)->data(Qt::UserRole + 1).toInt() == KEY.toInt()) {
                list->item(row)->setIcon(QIcon(PIXMAP));
            }
        }
    }
    for (auto it = d->slotButtons.cbegin(); it != d->slotButtons.cend(); ++it) {
        if (d->loadout.value(it.key()).value("icon_id").toInt() == KEY.toInt()) {
            it.value()->setIcon(QIcon(PIXMAP));
        }
    }
    d->details->RefreshIcon(KEY.toInt(), PIXMAP);
    d->preview->RefreshIcon(KEY.toInt(), PIXMAP);
}
