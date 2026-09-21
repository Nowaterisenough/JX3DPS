#include "equipment_panel.h"
#include "equipment_detail_card.h"
#include "../choice_popup.h"

#include <QApplication>
#include <QButtonGroup>
#include <QCoreApplication>
#include <QDir>
#include <QDialog>
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
#include <QSignalBlocker>
#include <QShowEvent>
#include <QStandardPaths>
#include <QTabWidget>
#include <QTextBrowser>
#include <QToolButton>
#include <QUrl>
#include <QVBoxLayout>

#include <array>

struct EquipmentPanel::Impl
{
    QNetworkAccessManager network;
    QGridLayout *slotGrid = nullptr;
    QListWidget *catalog = nullptr;
    QListWidget *effects = nullptr;
    QListWidget *skills = nullptr;
    QListWidget *talents = nullptr;
    QListWidget *recipes = nullptr;
    QLabel *setSummary = nullptr;
    QLabel *sourceLabel = nullptr;
    QLabel *equippedCount = nullptr;
    QToolButton *effectsToggle = nullptr;
    QWidget *effectsContent = nullptr;
    desktop::ChoicePopup *popup = nullptr;
    desktop::ChoicePopup *detailsPopup = nullptr;
    QDialog *referenceDialog = nullptr;
    QLabel *selectedLabel = nullptr;
    QToolButton *viewDetails = nullptr;
    bool loaded = false;
    QLabel *pickerTitle = nullptr;
    QLabel *emptyResults = nullptr;
    QLineEdit *search = nullptr;
    QPushButton *clear = nullptr;
    desktop::EquipmentDetailCard *details = nullptr;
    desktop::EquipmentDetailCard *preview = nullptr;
    QString activeSlot;
    QHash<QString, QToolButton *> slotButtons;
    QHash<QString, QJsonObject> loadout;
    QHash<QString, QPixmap> iconCache;
    QHash<QNetworkReply *, QString> replies;
    QJsonArray catalogData;
};

namespace {

const QStringList kSlots = {
    QStringLiteral("帽子"), QStringLiteral("上衣"), QStringLiteral("腰带"),
    QStringLiteral("护腕"), QStringLiteral("下装"), QStringLiteral("鞋子"),
    QStringLiteral("项链"), QStringLiteral("腰坠"), QStringLiteral("戒指1"),
    QStringLiteral("戒指2"), QStringLiteral("武器"), QStringLiteral("特殊")
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
    painter.drawText(pixmap.rect(), Qt::AlignCenter, iconId ? QStringLiteral("装备") : QStringLiteral("空"));
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
        p.drawPolygon(QPolygon{{13, 8}, {7, 13}, {4, 22}, {11, 24}, {13, 18}, {13, 33},
                               {28, 33}, {28, 18}, {30, 24}, {36, 22}, {33, 13}, {27, 8}, {20, 13}});
        p.drawLine(20, 14, 20, 31);
    } else if (slot == QStringLiteral("下装")) {
        p.drawPolygon(QPolygon{{10, 8}, {30, 8}, {29, 33}, {22, 33}, {20, 21}, {18, 33}, {11, 33}});
        p.drawLine(11, 13, 29, 13);
    } else if (slot == QStringLiteral("鞋子")) {
        p.drawPolygon(QPolygon{{18, 8}, {29, 8}, {29, 31}, {8, 31}, {8, 26}, {17, 23}});
        p.drawLine(19, 13, 28, 13);
    } else if (slot == QStringLiteral("腰带")) {
        p.drawRoundedRect(QRectF(5, 15, 30, 12), 2, 2);
        p.drawRoundedRect(QRectF(16, 12, 9, 18), 2, 2);
    } else if (slot == QStringLiteral("护腕")) {
        p.drawPolygon(QPolygon{{12, 8}, {29, 11}, {27, 33}, {10, 29}});
        p.drawLine(13, 16, 28, 19);
        p.drawLine(12, 22, 27, 25);
    } else if (slot.startsWith(QStringLiteral("戒指"))) {
        p.drawEllipse(QRectF(10, 13, 20, 20));
        p.drawPolygon(QPolygon{{20, 6}, {26, 12}, {20, 18}, {14, 12}});
    } else if (slot == QStringLiteral("项链")) {
        p.drawArc(QRectF(8, 3, 24, 25), 180 * 16, 180 * 16);
        p.drawPolygon(QPolygon{{20, 25}, {25, 30}, {20, 36}, {15, 30}});
    } else if (slot == QStringLiteral("腰坠")) {
        p.drawLine(20, 5, 20, 12);
        p.drawRoundedRect(QRectF(12, 12, 16, 18), 5, 5);
        p.drawLine(16, 30, 14, 35);
        p.drawLine(24, 30, 26, 35);
    } else if (slot == QStringLiteral("武器")) {
        p.drawPolygon(QPolygon{{29, 6}, {30, 15}, {15, 30}, {10, 25}});
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
    if (value.isString() && !value.toString().isEmpty()) labels << value.toString();
    for (const auto &entry : value.toArray()) {
        const QString label = entry.isObject() ? entry.toObject().value("label").toString() : entry.toString();
        if (!label.isEmpty()) labels << label;
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
        painter.drawPolygon(QPolygon{QPoint(width() - 11, height() - 9),
                                     QPoint(width() - 3, height() - 9), QPoint(width() - 7, height() - 5)});
    }
};

} // namespace

EquipmentPanel::EquipmentPanel(QWidget *parent)
    : QWidget(parent)
    , d(std::make_unique<Impl>())
{
    setObjectName(QStringLiteral("equipmentPanel"));
    setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred);
    BuildUi();
    connect(&d->network, &QNetworkAccessManager::finished,
            this, &EquipmentPanel::IconDownloaded);
}

EquipmentPanel::~EquipmentPanel() = default;

void EquipmentPanel::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    if (!d->loaded) LoadLatestData();
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

    auto *titleRow = new QHBoxLayout();
    auto *title = new QLabel(QStringLiteral("太虚剑意"), this);
    title->setStyleSheet(QStringLiteral("font-size: 13px; font-weight: 600; color: #c4d9d2;"));
    d->equippedCount = new QLabel(QStringLiteral("0 / 12"), this);
    d->equippedCount->setObjectName(QStringLiteral("equippedCount"));
    d->equippedCount->setStyleSheet(QStringLiteral("color: #84979e; font-size: 11px;"));
    d->sourceLabel = new QLabel(QStringLiteral("读取中…"), this);
    d->sourceLabel->setWordWrap(true);
    d->sourceLabel->setStyleSheet(QStringLiteral("font-size: 10px; color: #8d99a8;"));
    titleRow->addWidget(title);
    titleRow->addWidget(d->equippedCount);
    titleRow->addStretch();
    auto *references = new QToolButton(this);
    references->setObjectName(QStringLiteral("equipmentReferences"));
    references->setText(QStringLiteral("资料"));
    references->setAutoRaise(true);
    references->setToolTip(QStringLiteral("查看太虚剑意的技能、奇穴与秘籍资料"));
    titleRow->addWidget(references);
    root->addLayout(titleRow);

    d->referenceDialog = new QDialog(this);
    d->referenceDialog->setObjectName(QStringLiteral("equipmentReferenceDialog"));
    d->referenceDialog->setWindowTitle(QStringLiteral("太虚剑意 · 魔盒资料"));
    d->referenceDialog->resize(640, 640);
    auto *referenceLayout = new QVBoxLayout(d->referenceDialog);
    auto *tabs = new QTabWidget(d->referenceDialog);
    referenceLayout->addWidget(tabs);
    connect(references, &QToolButton::clicked, this, [this] {
        d->referenceDialog->show();
        d->referenceDialog->raise();
        d->referenceDialog->activateWindow();
    });
    auto *equipmentTab = new QWidget(this);
    auto *equipmentLayout = new QVBoxLayout(equipmentTab);
    equipmentLayout->setContentsMargins(0, 0, 0, 0);
    equipmentLayout->setSpacing(6);

    auto *sidebar = new QVBoxLayout;
    sidebar->setSpacing(6);
    equipmentLayout->addLayout(sidebar);

    auto *slotsWidget = new QWidget(equipmentTab);
    slotsWidget->setObjectName(QStringLiteral("equipmentSlots"));
    slotsWidget->setToolTip(QStringLiteral("点击部位图标更换装备"));
    d->slotGrid = new QGridLayout(slotsWidget);
    d->slotGrid->setContentsMargins(0, 0, 0, 0);
    d->slotGrid->setHorizontalSpacing(4);
    d->slotGrid->setVerticalSpacing(3);
    for (const auto &[text, column] : {std::pair{QStringLiteral("防具"), 0},
                                       std::pair{QStringLiteral("首饰 / 兵器"), 3}}) {
        auto *label = new QLabel(text, slotsWidget);
        label->setStyleSheet(QStringLiteral("color: #8da4ac; font-size: 11px;"));
        label->setAlignment(Qt::AlignCenter);
        d->slotGrid->addWidget(label, 0, column, 1, 2);
    }
    auto *divider = new QFrame(slotsWidget);
    divider->setFixedWidth(1);
    divider->setStyleSheet(QStringLiteral("background: #2c3b43;"));
    d->slotGrid->addWidget(divider, 0, 2, 4, 1);
    auto *slotSelection = new QButtonGroup(this);
    slotSelection->setExclusive(true);
    // Keep stable slot identifiers while arranging armor and accessories in two columns each.
    const std::array<QPoint, 12> positions{{{0, 1}, {1, 1}, {1, 2}, {0, 2}, {0, 3}, {1, 3},
                                          {3, 1}, {4, 1}, {3, 2}, {4, 2}, {3, 3}, {4, 3}}};
    for (int i = 0; i < kSlots.size(); ++i) {
        const QString slot = kSlots.at(i);
        auto *cell = new QWidget(slotsWidget);
        auto *cellLayout = new QVBoxLayout(cell);
        cellLayout->setContentsMargins(0, 1, 0, 1);
        cellLayout->setSpacing(1);
        auto *button = new SlotButton(cell);
        button->setObjectName(QStringLiteral("equipmentSlot%1").arg(i));
        button->setProperty("equipmentSlot", slot);
        button->setToolButtonStyle(Qt::ToolButtonIconOnly);
        button->setIconSize(QSize(36, 36));
        button->setFixedSize(44, 44);
        button->setCheckable(true);
        slotSelection->addButton(button);
        button->setCursor(Qt::PointingHandCursor);
        cellLayout->addWidget(button, 0, Qt::AlignHCenter);
        auto *caption = new QLabel(slot, cell);
        caption->setStyleSheet(QStringLiteral("font-size: 11px; color: #b2c0c8;"));
        caption->setAlignment(Qt::AlignCenter);
        cellLayout->addWidget(caption);
        d->slotButtons.insert(slot, button);
        UpdateSlotButton(slot);
        connect(button, &QToolButton::clicked, this, [this, slot] { OpenSlot(slot); });
        d->slotGrid->addWidget(cell, positions[i].y(), positions[i].x());
    }
    sidebar->addWidget(slotsWidget);

    auto *selectionRow = new QHBoxLayout;
    d->selectedLabel = new QLabel(QStringLiteral("点击图标选择装备"), equipmentTab);
    d->selectedLabel->setObjectName(QStringLiteral("selectedEquipment"));
    d->selectedLabel->setWordWrap(true);
    d->selectedLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred);
    selectionRow->addWidget(d->selectedLabel, 1);
    d->viewDetails = new QToolButton(equipmentTab);
    d->viewDetails->setObjectName(QStringLiteral("viewEquipmentDetails"));
    d->viewDetails->setText(QStringLiteral("详情"));
    d->viewDetails->setAutoRaise(true);
    d->viewDetails->setEnabled(false);
    selectionRow->addWidget(d->viewDetails);
    sidebar->addLayout(selectionRow);
    d->detailsPopup = new desktop::ChoicePopup(this);
    d->detailsPopup->setObjectName(QStringLiteral("equipmentDetailsPopup"));
    auto *detailsLayout = new QVBoxLayout(d->detailsPopup);
    detailsLayout->setContentsMargins(1, 1, 1, 1);
    d->details = new desktop::EquipmentDetailCard(d->detailsPopup);
    d->details->icon = [this](int id) { return RequestIcon(id).pixmap(32, 32); };
    d->details->setObjectName(QStringLiteral("equipmentDetails"));
    d->details->setOpenLinks(false);
    d->details->setMinimumHeight(180);
    d->details->setHtml(ItemDetails({}, QStringLiteral("装备")));
    detailsLayout->addWidget(d->details);
    d->detailsPopup->Watch(d->details);
    connect(d->viewDetails, &QToolButton::clicked, this, [this] {
        if (d->detailsPopup->isVisible()) d->detailsPopup->hide();
        else d->detailsPopup->Open(d->viewDetails, {750, 900}, d->details, desktop::ChoicePopup::Beside);
    });

    d->effectsToggle = new QToolButton(this);
    d->effectsToggle->setObjectName(QStringLiteral("equipmentEffectsToggle"));
    d->effectsToggle->setCheckable(true);
    d->effectsToggle->setAutoRaise(true);
    d->effectsToggle->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    d->effectsToggle->setArrowType(Qt::RightArrow);
    sidebar->addWidget(d->effectsToggle);
    d->effectsContent = new QWidget(this);
    auto *effectLayout = new QVBoxLayout(d->effectsContent);
    effectLayout->setContentsMargins(0, 0, 0, 0);
    effectLayout->setSpacing(4);
    sidebar->addWidget(d->effectsContent);
    d->effectsContent->hide();
    connect(d->effectsToggle, &QToolButton::toggled, this, [this](bool expanded) {
        d->effectsToggle->setArrowType(expanded ? Qt::DownArrow : Qt::RightArrow);
        d->effectsContent->setVisible(expanded);
    });
    d->setSummary = new QLabel(QStringLiteral("尚未装备套装"), this);
    d->setSummary->setWordWrap(true);
    d->setSummary->setStyleSheet(QStringLiteral("color: #9ca8b8; font-size: 11px;"));
    effectLayout->addWidget(d->setSummary);
    d->effects = new QListWidget(this);
    d->effects->setViewMode(QListView::IconMode);
    d->effects->setIconSize(QSize(36, 36));
    d->effects->setObjectName(QStringLiteral("equipmentEffects"));
    d->effects->setGridSize(QSize(76, 58));
    d->effects->setResizeMode(QListView::Adjust);
    d->effects->setMovement(QListView::Static);
    d->effects->setMaximumHeight(68);
    d->effects->setStyleSheet(QStringLiteral(
        "QListWidget { background: #1a2029; border: 1px solid #303a49; }"
        "QListWidget::item { color: #cbd5e1; padding: 1px; }"));
    effectLayout->addWidget(d->effects);
    d->sourceLabel->setToolTip(QStringLiteral("装备选择用于预览；模拟属性以「属性」页为准。"));

    d->popup = new desktop::ChoicePopup(this);
    auto *popupLayout = new QVBoxLayout(d->popup);
    popupLayout->setContentsMargins(10, 10, 10, 10);
    popupLayout->setSpacing(8);
    auto *pickerHeader = new QHBoxLayout;
    d->pickerTitle = new QLabel(d->popup);
    d->pickerTitle->setStyleSheet(QStringLiteral("font-weight: 600; color: #f4c76b;"));
    d->clear = new QPushButton(QStringLiteral("卸下装备"), d->popup);
    d->clear->setObjectName(QStringLiteral("clearEquipment"));
    d->clear->setAutoDefault(false);
    pickerHeader->addWidget(d->pickerTitle, 1);
    pickerHeader->addWidget(d->clear);
    popupLayout->addLayout(pickerHeader);
    d->search = new QLineEdit(d->popup);
    d->search->setObjectName(QStringLiteral("equipmentSearch"));
    d->search->setPlaceholderText(QStringLiteral("搜索名称、属性或来源"));
    auto *body = new QHBoxLayout;
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
    d->catalog->setStyleSheet(QStringLiteral(
        "QListWidget { background: #1a2029; border: 1px solid #303a49; }"
        "QListWidget::item { color: #cbd5e1; padding: 2px; }"
        "QListWidget::item:selected { background: #3b4657; }"));
    connect(d->catalog, &QListWidget::itemClicked,
            this, &EquipmentPanel::EquipCandidate);
    const auto previewItem = [this](QListWidgetItem *item) {
        d->preview->setHtml(item ? ItemDetails(item->data(Qt::UserRole).toJsonObject(), d->activeSlot) : QString());
    };
    connect(d->catalog, &QListWidget::currentItemChanged, this, [previewItem](QListWidgetItem *item) { previewItem(item); });
    connect(d->catalog, &QListWidget::itemEntered, this, previewItem);
    candidates->addWidget(d->catalog, 1);
    d->emptyResults = new QLabel(QStringLiteral("当前部位没有匹配装备"), d->popup);
    d->emptyResults->setAlignment(Qt::AlignCenter);
    candidates->addWidget(d->emptyResults);
    body->addLayout(candidates, 1);
    d->preview = new desktop::EquipmentDetailCard(d->popup);
    d->preview->icon = [this](int id) { return RequestIcon(id).pixmap(32, 32); };
    d->preview->setObjectName(QStringLiteral("equipmentCandidateDetails"));
    d->preview->setOpenLinks(false);
    d->preview->setMinimumSize(0, 0);
    body->addWidget(d->preview, 3);
    popupLayout->addLayout(body, 1);
    auto *pickerHint = new QLabel(QStringLiteral("单击或回车选择 · Esc 取消"), d->popup);
    pickerHint->setStyleSheet(QStringLiteral("color: #9ca8b8; font-size: 11px;"));
    popupLayout->addWidget(pickerHint);
    for (QWidget *widget : std::initializer_list<QWidget *>{d->catalog, d->search, d->preview, d->clear})
        d->popup->Watch(widget);
    d->popup->acceptChoice = [this] {
        if (d->clear->hasFocus()) d->clear->click();
        else EquipCandidate(d->catalog->currentItem());
    };
    connect(d->search, &QLineEdit::textChanged, this, &EquipmentPanel::FilterCandidates);
    connect(d->clear, &QPushButton::clicked, this, [this] {
        const QString slot = d->activeSlot;
        d->popup->hide();
        SetSlotItem(slot, {});
    });

    const auto createIconList = [tabs](const QString &title) {
        auto *list = new QListWidget(tabs);
        list->setViewMode(QListView::IconMode);
        list->setIconSize(QSize(48, 48));
        list->setGridSize(QSize(112, 82));
        list->setResizeMode(QListView::Adjust);
        list->setMovement(QListView::Static);
        list->setSpacing(2);
        list->setStyleSheet(QStringLiteral(
            "QListWidget { background: #1a2029; border: 1px solid #303a49; }"
            "QListWidget::item { color: #cbd5e1; padding: 2px; }"
            "QListWidget::item:selected { background: #3b4657; }"));
        tabs->addTab(list, title);
        return list;
    };
    d->skills = createIconList(QStringLiteral("技能"));
    d->talents = createIconList(QStringLiteral("奇穴"));
    d->recipes = createIconList(QStringLiteral("秘籍"));
    root->addWidget(equipmentTab);
    root->addWidget(d->sourceLabel);
}

QString EquipmentPanel::FindSnapshotPath(const QString &requested)
{
    QStringList candidates;
    if (!requested.isEmpty()) candidates << requested;
    candidates << QDir::current().filePath(QStringLiteral("data/jx3box/tai_xu_jian_yi_latest.json"));
    QDir appDir(QCoreApplication::applicationDirPath());
    for (int i = 0; i < 5; ++i) {
        candidates << appDir.filePath(QStringLiteral("data/jx3box/tai_xu_jian_yi_latest.json"));
        candidates << appDir.filePath(QStringLiteral("../../data/jx3box/tai_xu_jian_yi_latest.json"));
        if (!appDir.cdUp()) break;
    }
    for (const auto &candidate : candidates) {
        const QFileInfo info(candidate);
        if (info.exists() && info.isFile()) return info.absoluteFilePath();
    }
    return QString();
}

void EquipmentPanel::LoadLatestData(const QString &snapshotPath)
{
    d->loaded = true;
    const QString path = FindSnapshotPath(snapshotPath);
    if (path.isEmpty()) {
        d->sourceLabel->setText(QStringLiteral("魔盒快照缺失"));
        return;
    }
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        d->sourceLabel->setText(QStringLiteral("快照读取失败"));
        return;
    }
    QJsonParseError error;
    const auto document = QJsonDocument::fromJson(file.readAll(), &error);
    if (error.error != QJsonParseError::NoError || !document.isObject()) {
        d->sourceLabel->setText(QStringLiteral("快照格式错误"));
        return;
    }
    d->sourceLabel->setText(QStringLiteral("装备预览 · 魔盒 %1").arg(document.object().value(QStringLiteral("data_version")).toString()));
    PopulateFromSnapshot(document.object());
}

QString EquipmentPanel::SlotForItem(const QJsonObject &item)
{
    const QString type = item.value(QStringLiteral("type")).toString();
    if (type == QStringLiteral("戒指")) return QStringLiteral("戒指1");
    if (type == QStringLiteral("腰坠") || type == QStringLiteral("项链")) return type;
    if (type == QStringLiteral("投掷")) return QStringLiteral("特殊");
    if (type.contains(QStringLiteral("短兵")) || type == QStringLiteral("武器")) return QStringLiteral("武器");
    if (!type.isEmpty()) return type;
    const QString name = item.value(QStringLiteral("name")).toString();
    if (name.contains(QStringLiteral("腰坠"))) return QStringLiteral("腰坠");
    if (name.contains(QStringLiteral("链"))) return QStringLiteral("项链");
    if (name.contains(QStringLiteral("戒"))) return QStringLiteral("戒指1");
    return QStringLiteral("特殊");
}

QString EquipmentPanel::IconUrl(int iconId)
{
    return QStringLiteral("https://icon.jx3box.com/icon/%1.png").arg(iconId);
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

    const auto skill = snapshot.value(QStringLiteral("skill")).toObject();
    const auto remarks = skill.value(QStringLiteral("remarks")).toArray();
    for (const auto &remarkValue : remarks) {
        const auto remark = remarkValue.toObject();
        for (const auto &skillValue : remark.value(QStringLiteral("forceSkills")).toArray()) {
            const auto item = skillValue.toObject();
            AddIconItem(d->skills,
                        item.value(QStringLiteral("skillName")).toString(),
                        item.value(QStringLiteral("desc")).toString(),
                        item.value(QStringLiteral("icon")).toObject().value(QStringLiteral("FileName")).toString().split('/').last().remove(".png").toInt());
        }
    }
    const auto talent = snapshot.value(QStringLiteral("talent")).toObject();
    for (const auto &levelValue : talent.value(QStringLiteral("kungfuLevel")).toArray()) {
        for (const auto &talentValue : levelValue.toObject().value(QStringLiteral("kungfuSkills")).toArray()) {
            const auto item = talentValue.toObject();
            AddIconItem(d->talents,
                        QStringLiteral("%1\n第%2层").arg(item.value(QStringLiteral("name")).toString()).arg(item.value(QStringLiteral("level")).toInt()),
                        item.value(QStringLiteral("desc")).toString(),
                        item.value(QStringLiteral("icon")).toObject().value(QStringLiteral("FileName")).toString().split('/').last().remove(".png").toInt());
        }
    }
    for (const auto &recipeValue : snapshot.value(QStringLiteral("recipe_catalog")).toArray()) {
        const auto item = recipeValue.toObject();
        AddIconItem(d->recipes,
                    item.value(QStringLiteral("name")).toString(),
                    item.value(QStringLiteral("description")).toString(),
                    item.value(QStringLiteral("icon_id")).toInt());
    }

    UpdateEffects();
}

void EquipmentPanel::EquipCandidate(QListWidgetItem *item)
{
    if (!item) return;
    const auto object = item->data(Qt::UserRole).toJsonObject();
    const QString slot = d->activeSlot;
    d->popup->hide();
    SetSlotItem(slot, object);
}

void EquipmentPanel::SetSlotItem(const QString &slot, const QJsonObject &item)
{
    if (!d->slotButtons.contains(slot)) return;
    const bool changed = d->loadout.value(slot) != item;
    if (item.isEmpty()) d->loadout.remove(slot);
    else d->loadout.insert(slot, item);
    UpdateSlotButton(slot);
    d->selectedLabel->setText(slot + QStringLiteral("：") + (item.isEmpty() ? QStringLiteral("未装备") : item.value("name").toString()));
    d->viewDetails->setEnabled(!item.isEmpty());
    d->details->setHtml(ItemDetails(item, slot));
    UpdateEffects();
    if (changed) emit LoadoutChanged(Loadout());
}

void EquipmentPanel::UpdateSlotButton(const QString &slot)
{
    auto *button = d->slotButtons.value(slot, nullptr);
    if (!button) return;
    const auto item = d->loadout.value(slot);
    const QString name = item.value(QStringLiteral("name")).toString();
    const int iconId = item.value(QStringLiteral("icon_id")).toInt();
    button->setText(slot);
    button->setAccessibleName(slot + QStringLiteral("：") + (name.isEmpty() ? QStringLiteral("未装备") : name));
    button->setToolTip(ItemDetails(item, slot));
    button->setIcon(item.isEmpty() ? QIcon(EmptySlotIcon(slot)) : RequestIcon(iconId));
    button->setStyleSheet(QStringLiteral(
        "QToolButton { background: #202631; border: 1px solid %1; border-radius: 4px; padding: 2px; }"
        "QToolButton:hover, QToolButton:focus { background: #2b3442; border: 1px solid #f4c76b; }"
        "QToolButton:checked { background: #34443e; border: 2px solid #85bfa8; }")
        .arg(item.isEmpty() ? QStringLiteral("#536071") : QualityColor(item.value("quality").toInt())));
}

void EquipmentPanel::OpenSlot(const QString &slot)
{
    if (d->popup->isVisible() && d->activeSlot == slot) {
        d->popup->hide();
        return;
    }
    d->popup->hide();
    d->detailsPopup->hide();
    d->activeSlot = slot;
    const auto equipped = d->loadout.value(slot);
    d->selectedLabel->setText(slot + QStringLiteral("：") + (equipped.isEmpty() ? QStringLiteral("未装备") : equipped.value("name").toString()));
    d->viewDetails->setEnabled(!equipped.isEmpty());
    d->details->setHtml(ItemDetails(d->loadout.value(slot), slot));
    d->clear->setEnabled(d->loadout.contains(slot));
    const QSignalBlocker blocker(d->search);
    d->search->clear();
    FilterCandidates();
    d->popup->Open(d->slotButtons.value(slot), {980, 720}, d->catalog, desktop::ChoicePopup::Beside);
    d->catalog->scrollToItem(d->catalog->currentItem());
}

void EquipmentPanel::FilterCandidates()
{
    d->catalog->clear();
    const QString filter = d->search->text().trimmed();
    const QString type = d->activeSlot == QStringLiteral("戒指2") ? QStringLiteral("戒指1") : d->activeSlot;
    QListWidgetItem *selected = nullptr;
    for (const auto &value : d->catalogData) {
        const auto item = value.toObject();
        if (SlotForItem(item) != type) continue;
        const QString details = ItemDetails(item, d->activeSlot);
        QTextDocument searchable;
        searchable.setHtml(details);
        if (!filter.isEmpty() && !(searchable.toPlainText() + item.value("magic_type").toString()).contains(filter, Qt::CaseInsensitive)) continue;
        AddIconItem(d->catalog, item.value("name").toString(), details, item.value("icon_id").toInt());
        auto *entry = d->catalog->item(d->catalog->count() - 1);
        entry->setData(Qt::UserRole, item);
        if (item == d->loadout.value(d->activeSlot)) selected = entry;
    }
    d->pickerTitle->setText(QStringLiteral("%1 · %2 件候选").arg(d->activeSlot).arg(d->catalog->count()));
    d->emptyResults->setVisible(d->catalog->count() == 0);
    d->catalog->setCurrentItem(selected ? selected : d->catalog->item(0));
    if (!d->catalog->count())
        d->preview->setHtml(QStringLiteral("<p>没有匹配的装备，请尝试其他名称或属性。</p>"));
}

QString EquipmentPanel::ItemDetails(const QJsonObject &item, const QString &slot) const
{
    return desktop::EquipmentDetailCard::Html(item, Loadout().value("items").toObject(), slot);
}

void EquipmentPanel::UpdateEffects()
{
    d->effects->clear();
    QMap<QString, QList<QString>> sets;
    int socketsCount = 0;
    for (const auto &slot : kSlots) {
        const auto item = d->loadout.value(slot);
        const auto name = item.value("set").toObject().value("name").toString();
        if (!name.isEmpty()) sets[name].append(slot);
        const auto sockets = SocketLabels(item.value("diamonds"));
        socketsCount += sockets.size();
        if (!sockets.isEmpty()) AddIconItem(d->effects, slot + QStringLiteral("镶嵌"), sockets.join('\n'), item.value("icon_id").toInt());
    }
    QStringList summary;
    for (auto it = sets.cbegin(); it != sets.cend(); ++it) {
        const auto item = d->loadout.value(it.value().first());
        const auto bonuses = item.value("set").toObject().value("attributes").toObject();
        QStringList lines{QStringLiteral("%1 · 已装备 %2 件").arg(it.key()).arg(it.value().size())};
        for (auto bonus = bonuses.begin(); bonus != bonuses.end(); ++bonus)
            lines << QStringLiteral("%1件：%2（%3）").arg(bonus.key(), bonus.value().toString(),
                bonus.key().toInt() > 0 && it.value().size() >= bonus.key().toInt() ? QStringLiteral("已激活") : QStringLiteral("未激活"));
        summary << lines.first();
        AddIconItem(d->effects, QStringLiteral("套装 %1 件").arg(it.value().size()), lines.join('\n'), item.value("icon_id").toInt());
    }
    d->setSummary->setText(summary.isEmpty() ? QStringLiteral("尚未装备套装") : summary.join(QStringLiteral("；")));
    d->equippedCount->setText(QStringLiteral("%1 / 12").arg(d->loadout.size()));
    d->effectsToggle->setText(QStringLiteral("套装 %1 · 镶嵌 %2").arg(sets.size()).arg(socketsCount));
    d->effectsToggle->setVisible(d->effects->count() > 0);
    d->effectsContent->setVisible(d->effects->count() > 0 && d->effectsToggle->isChecked());
    d->effects->setVisible(d->effects->count() > 0);
    for (const auto &slot : kSlots)
        d->slotButtons.value(slot)->setToolTip(ItemDetails(d->loadout.value(slot), slot));
}

void EquipmentPanel::AddIconItem(QListWidget *list, const QString &name, const QString &description, int iconId)
{
    auto *entry = new QListWidgetItem(name, list);
    entry->setToolTip(description);
    entry->setData(Qt::UserRole + 1, iconId);
    entry->setIcon(RequestIcon(iconId));
}

QJsonObject EquipmentPanel::Loadout() const
{
    QJsonObject result;
    QJsonObject items;
    for (auto it = d->loadout.cbegin(); it != d->loadout.cend(); ++it) items.insert(it.key(), it.value());
    result.insert(QStringLiteral("items"), items);
    return result;
}

QIcon EquipmentPanel::RequestIcon(int iconId)
{
    if (iconId <= 0) return QIcon(PlaceholderIcon(0));
    const QString key = QString::number(iconId);
    if (d->iconCache.contains(key)) return QIcon(d->iconCache.value(key));
    const QString localPath = QStringLiteral(":/resources/images/JX3/Icons/%1.png").arg(iconId);
    const QPixmap local(localPath);
    if (!local.isNull()) {
        d->iconCache.insert(key, local);
        return QIcon(local);
    }
    bool pending = false;
    for (auto it = d->replies.cbegin(); it != d->replies.cend(); ++it) {
        if (it.value() == key) { pending = true; break; }
    }
    if (!pending) {
        QNetworkRequest request(QUrl(IconUrl(iconId)));
        request.setTransferTimeout(15000);
        auto *reply = d->network.get(request);
        d->replies.insert(reply, key);
    }
    return QIcon(PlaceholderIcon(iconId));
}

void EquipmentPanel::IconDownloaded(QNetworkReply *reply)
{
    const QString key = d->replies.take(reply);
    const QPixmap pixmap = QPixmap::fromImage(QImage::fromData(reply->readAll()));
    reply->deleteLater();
    if (pixmap.isNull()) return;
    d->iconCache.insert(key, pixmap);
    // Refresh only live entries: filtering or reloading may have deleted the original item.
    for (auto *list : {d->catalog, d->effects, d->skills, d->talents, d->recipes})
        for (int row = 0; row < list->count(); ++row)
            if (list->item(row)->data(Qt::UserRole + 1).toInt() == key.toInt()) list->item(row)->setIcon(QIcon(pixmap));
    for (auto it = d->slotButtons.cbegin(); it != d->slotButtons.cend(); ++it)
        if (d->loadout.value(it.key()).value("icon_id").toInt() == key.toInt()) it.value()->setIcon(QIcon(pixmap));
    d->details->RefreshIcon(key.toInt(), pixmap);
    d->preview->RefreshIcon(key.toInt(), pixmap);
}
