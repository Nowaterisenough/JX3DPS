#include "equipment_panel.h"

#include <QApplication>
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QHBoxLayout>
#include <QIcon>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QLabel>
#include <QListWidget>
#include <QNetworkReply>
#include <QPainter>
#include <QPushButton>
#include <QScrollArea>
#include <QStandardPaths>
#include <QTabWidget>
#include <QToolButton>
#include <QUrl>
#include <QVBoxLayout>

#include <functional>

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
    QHash<QString, QToolButton *> slotButtons;
    QHash<QString, QJsonObject> loadout;
    QHash<QString, QPixmap> iconCache;
    QHash<QString, QList<std::function<void(const QPixmap &)>>> iconWaiters;
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
    switch (quality) {
    case 5: return QStringLiteral("#ff80ff");
    case 4: return QStringLiteral("#ff9d3d");
    case 3: return QStringLiteral("#a335ee");
    case 2: return QStringLiteral("#1eff00");
    default: return QStringLiteral("#d8dee9");
    }
}

QPixmap PlaceholderIcon(int iconId)
{
    QPixmap pixmap(52, 52);
    pixmap.fill(QColor(48, 53, 62));
    QPainter painter(&pixmap);
    painter.setPen(QColor(160, 170, 185));
    painter.drawRect(1, 1, 50, 50);
    painter.drawText(pixmap.rect(), Qt::AlignCenter, QString::number(iconId));
    return pixmap;
}

} // namespace

EquipmentPanel::EquipmentPanel(QWidget *parent)
    : QWidget(parent)
    , d(std::make_unique<Impl>())
{
    setObjectName(QStringLiteral("equipmentPanel"));
    setMinimumWidth(330);
    setMaximumWidth(410);
    BuildUi();
    LoadLatestData();
    connect(&d->network, &QNetworkAccessManager::finished,
            this, &EquipmentPanel::IconDownloaded);
}

EquipmentPanel::~EquipmentPanel() = default;

void EquipmentPanel::BuildUi()
{
    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(10, 8, 10, 8);
    root->setSpacing(7);

    auto *titleRow = new QHBoxLayout();
    auto *title = new QLabel(QStringLiteral("太虚剑意 · 魔盒配装"), this);
    title->setStyleSheet(QStringLiteral("font-size: 14px; font-weight: 600; color: #f4c76b;"));
    d->sourceLabel = new QLabel(QStringLiteral("读取中…"), this);
    d->sourceLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    d->sourceLabel->setStyleSheet(QStringLiteral("font-size: 10px; color: #8d99a8;"));
    titleRow->addWidget(title);
    titleRow->addStretch();
    titleRow->addWidget(d->sourceLabel);
    root->addLayout(titleRow);

    auto *tabs = new QTabWidget(this);
    auto *equipmentTab = new QWidget(tabs);
    auto *equipmentLayout = new QVBoxLayout(equipmentTab);
    equipmentLayout->setContentsMargins(0, 0, 0, 0);
    equipmentLayout->setSpacing(7);

    auto *slotsWidget = new QWidget(equipmentTab);
    d->slotGrid = new QGridLayout(slotsWidget);
    d->slotGrid->setContentsMargins(0, 0, 0, 0);
    d->slotGrid->setSpacing(4);
    for (int i = 0; i < kSlots.size(); ++i) {
        const QString slot = kSlots.at(i);
        auto *button = new QToolButton(slotsWidget);
        button->setText(slot + QStringLiteral("\n未装备"));
        button->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        button->setIcon(PlaceholderIcon(0));
        button->setIconSize(QSize(48, 48));
        button->setMinimumSize(92, 76);
        button->setToolTip(slot);
        button->setStyleSheet(QStringLiteral(
            "QToolButton { color: #cbd5e1; background: #202631; border: 1px solid #384252; border-radius: 4px; padding: 2px; }"
            "QToolButton:hover { border-color: #f4c76b; background: #2b3442; }"));
        d->slotButtons.insert(slot, button);
        d->slotGrid->addWidget(button, i / 3, i % 3);
    }
    equipmentLayout->addWidget(slotsWidget);

    auto *setTitle = new QLabel(QStringLiteral("套装 / 附魔"), this);
    setTitle->setStyleSheet(QStringLiteral("font-weight: 600; color: #b8c5d6;"));
    equipmentLayout->addWidget(setTitle);
    d->setSummary = new QLabel(QStringLiteral("暂无套装数据"), this);
    d->setSummary->setWordWrap(true);
    d->setSummary->setStyleSheet(QStringLiteral("color: #9ca8b8; font-size: 11px;"));
    equipmentLayout->addWidget(d->setSummary);
    d->effects = new QListWidget(this);
    d->effects->setViewMode(QListView::IconMode);
    d->effects->setIconSize(QSize(36, 36));
    d->effects->setGridSize(QSize(128, 58));
    d->effects->setResizeMode(QListView::Adjust);
    d->effects->setMovement(QListView::Static);
    d->effects->setMaximumHeight(68);
    d->effects->setStyleSheet(QStringLiteral(
        "QListWidget { background: #1a2029; border: 1px solid #303a49; }"
        "QListWidget::item { color: #cbd5e1; padding: 1px; }"));
    equipmentLayout->addWidget(d->effects);

    auto *catalogTitle = new QLabel(QStringLiteral("魔盒候选装备（点击替换）"), this);
    catalogTitle->setStyleSheet(QStringLiteral("font-weight: 600; color: #b8c5d6;"));
    equipmentLayout->addWidget(catalogTitle);
    d->catalog = new QListWidget(this);
    d->catalog->setViewMode(QListView::IconMode);
    d->catalog->setIconSize(QSize(48, 48));
    d->catalog->setGridSize(QSize(96, 82));
    d->catalog->setResizeMode(QListView::Adjust);
    d->catalog->setMovement(QListView::Static);
    d->catalog->setSpacing(2);
    d->catalog->setMinimumHeight(170);
    d->catalog->setStyleSheet(QStringLiteral(
        "QListWidget { background: #1a2029; border: 1px solid #303a49; }"
        "QListWidget::item { color: #cbd5e1; padding: 2px; }"
        "QListWidget::item:selected { background: #3b4657; }"));
    connect(d->catalog, &QListWidget::itemClicked,
            this, &EquipmentPanel::EquipCandidate);
    equipmentLayout->addWidget(d->catalog, 1);

    tabs->addTab(equipmentTab, QStringLiteral("装备"));
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
    root->addWidget(tabs, 1);
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
    d->sourceLabel->setText(QStringLiteral("魔盒 %1").arg(document.object().value(QStringLiteral("data_version")).toString()));
    PopulateFromSnapshot(document.object());
}

QString EquipmentPanel::SlotForItem(const QJsonObject &item)
{
    const QString type = item.value(QStringLiteral("type")).toString();
    if (type == QStringLiteral("戒指")) return QStringLiteral("戒指1");
    if (type == QStringLiteral("腰坠") || type == QStringLiteral("项链")) return type;
    if (type.contains(QStringLiteral("短兵")) || type == QStringLiteral("投掷") || type == QStringLiteral("武器")) return QStringLiteral("武器");
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
    d->catalog->clear();
    d->effects->clear();
    d->skills->clear();
    d->talents->clear();
    d->recipes->clear();
    d->catalogData = snapshot.value(QStringLiteral("equipment_catalog")).toArray();

    const auto addIconItem = [this](QListWidget *list, const QString &name,
                                    const QString &description, int iconId) {
        auto *entry = new QListWidgetItem(name, list);
        entry->setToolTip(description);
        entry->setIcon(QIcon(PlaceholderIcon(iconId)));
        RequestIcon(iconId, [entry](const QPixmap &pixmap) {
            if (!pixmap.isNull()) entry->setIcon(QIcon(pixmap));
        });
    };
    const auto skill = snapshot.value(QStringLiteral("skill")).toObject();
    const auto remarks = skill.value(QStringLiteral("remarks")).toArray();
    for (const auto &remarkValue : remarks) {
        const auto remark = remarkValue.toObject();
        for (const auto &skillValue : remark.value(QStringLiteral("forceSkills")).toArray()) {
            const auto item = skillValue.toObject();
            addIconItem(d->skills,
                        item.value(QStringLiteral("skillName")).toString(),
                        item.value(QStringLiteral("desc")).toString(),
                        item.value(QStringLiteral("icon")).toObject().value(QStringLiteral("FileName")).toString().split('/').last().remove(".png").toInt());
        }
    }
    const auto talent = snapshot.value(QStringLiteral("talent")).toObject();
    for (const auto &levelValue : talent.value(QStringLiteral("kungfuLevel")).toArray()) {
        for (const auto &talentValue : levelValue.toObject().value(QStringLiteral("kungfuSkills")).toArray()) {
            const auto item = talentValue.toObject();
            addIconItem(d->talents,
                        QStringLiteral("%1\n第%2层").arg(item.value(QStringLiteral("name")).toString()).arg(item.value(QStringLiteral("level")).toInt()),
                        item.value(QStringLiteral("desc")).toString(),
                        item.value(QStringLiteral("icon")).toObject().value(QStringLiteral("FileName")).toString().split('/').last().remove(".png").toInt());
        }
    }
    for (const auto &recipeValue : snapshot.value(QStringLiteral("recipe_catalog")).toArray()) {
        const auto item = recipeValue.toObject();
        addIconItem(d->recipes,
                    item.value(QStringLiteral("name")).toString(),
                    item.value(QStringLiteral("description")).toString(),
                    item.value(QStringLiteral("icon_id")).toInt());
    }

    QHash<QString, QStringList> sets;
    QHash<QString, int> setIcons;
    for (const auto &value : d->catalogData) {
        const auto item = value.toObject();
        auto *entry = new QListWidgetItem(item.value(QStringLiteral("name")).toString(), d->catalog);
        entry->setData(Qt::UserRole, item);
        const int iconId = item.value(QStringLiteral("icon_id")).toInt();
        entry->setIcon(QIcon(PlaceholderIcon(iconId)));
        entry->setToolTip(item.value(QStringLiteral("name")).toString());
        RequestIcon(iconId, [entry](const QPixmap &pixmap) {
            if (!pixmap.isNull()) entry->setIcon(QIcon(pixmap));
        });
        const auto set = item.value(QStringLiteral("set")).toObject();
        const QString setName = set.value(QStringLiteral("name")).toString();
        if (!setName.isEmpty()) {
            sets[setName].append(item.value(QStringLiteral("name")).toString());
            setIcons.insert(setName, iconId);
        }
        const auto diamonds = item.value(QStringLiteral("diamonds")).toArray();
        for (const auto &diamond : diamonds) {
            auto *effect = new QListWidgetItem(QStringLiteral("宝石\n%1").arg(diamond.toString()), d->effects);
            effect->setIcon(QIcon(PlaceholderIcon(iconId)));
            RequestIcon(iconId, [effect](const QPixmap &pixmap) {
                if (!pixmap.isNull()) effect->setIcon(QIcon(pixmap));
            });
        }
    }
    QStringList summary;
    for (auto it = sets.cbegin(); it != sets.cend(); ++it) {
        QString text = QStringLiteral("%1（%2件候选）").arg(it.key()).arg(it.value().size());
        for (const auto &value : d->catalogData) {
            const auto set = value.toObject().value(QStringLiteral("set")).toObject();
            if (set.value(QStringLiteral("name")).toString() != it.key()) continue;
            const auto attrs = set.value(QStringLiteral("attributes")).toObject();
            QStringList bonuses;
            for (auto bonus = attrs.begin(); bonus != attrs.end(); ++bonus)
                bonuses << QStringLiteral("%1件：%2").arg(bonus.key(), bonus.value().toString());
            if (!bonuses.isEmpty()) text += QStringLiteral("\n") + bonuses.join(QStringLiteral("；"));
            break;
        }
        summary << text;
    }
    d->setSummary->setText(summary.isEmpty() ? QStringLiteral("当前候选暂无套装标签") : summary.join(QStringLiteral("\n")));
    for (auto it = sets.cbegin(); it != sets.cend(); ++it) {
        auto *setItem = new QListWidgetItem(QStringLiteral("套装\n%1").arg(it.key()), d->effects);
        const int iconId = setIcons.value(it.key());
        setItem->setIcon(QIcon(PlaceholderIcon(iconId)));
        RequestIcon(iconId, [setItem](const QPixmap &pixmap) {
            if (!pixmap.isNull()) setItem->setIcon(QIcon(pixmap));
        });
    }
}

void EquipmentPanel::EquipCandidate(QListWidgetItem *item)
{
    if (!item) return;
    const auto object = item->data(Qt::UserRole).toJsonObject();
    const QString slot = SlotForItem(object);
    if (slot == QStringLiteral("戒指1") && d->loadout.contains(QStringLiteral("戒指1"))) {
        SetSlotItem(QStringLiteral("戒指2"), object);
    } else {
        SetSlotItem(slot, object);
    }
    emit LoadoutChanged(Loadout());
}

void EquipmentPanel::SetSlotItem(const QString &slot, const QJsonObject &item)
{
    if (!d->slotButtons.contains(slot)) return;
    d->loadout.insert(slot, item);
    UpdateSlotButton(slot);
}

void EquipmentPanel::UpdateSlotButton(const QString &slot)
{
    auto *button = d->slotButtons.value(slot, nullptr);
    if (!button) return;
    const auto item = d->loadout.value(slot);
    const QString name = item.value(QStringLiteral("name")).toString();
    const int iconId = item.value(QStringLiteral("icon_id")).toInt();
    button->setText(slot + QStringLiteral("\n") + (name.isEmpty() ? QStringLiteral("未装备") : name.left(7)));
    button->setIcon(QIcon(PlaceholderIcon(iconId)));
    button->setStyleSheet(QStringLiteral(
        "QToolButton { color: %1; background: #202631; border: 1px solid #b58a43; border-radius: 4px; padding: 2px; }"
        "QToolButton:hover { background: #2b3442; }").arg(QualityColor(item.value(QStringLiteral("quality")).toInt())));
    RequestIcon(iconId, [button](const QPixmap &pixmap) {
        if (!pixmap.isNull()) button->setIcon(QIcon(pixmap));
    });
}

QJsonObject EquipmentPanel::Loadout() const
{
    QJsonObject result;
    QJsonObject items;
    for (auto it = d->loadout.cbegin(); it != d->loadout.cend(); ++it) items.insert(it.key(), it.value());
    result.insert(QStringLiteral("items"), items);
    return result;
}

void EquipmentPanel::RequestIcon(int iconId, const std::function<void(const QPixmap &)> &ready)
{
    if (iconId <= 0) return;
    const QString key = QString::number(iconId);
    const QString localPath = QStringLiteral(":/resources/images/JX3/Icons/%1.png").arg(iconId);
    const QPixmap local(localPath);
    if (!local.isNull()) {
        ready(local);
        d->iconCache.insert(key, local);
        return;
    }
    if (d->iconCache.contains(key)) {
        ready(d->iconCache.value(key));
        return;
    }
    d->iconWaiters[key].append(ready);
    bool pending = false;
    for (auto it = d->replies.cbegin(); it != d->replies.cend(); ++it) {
        if (it.value() == key) { pending = true; break; }
    }
    if (!pending) {
        auto *reply = d->network.get(QNetworkRequest(QUrl(IconUrl(iconId))));
        d->replies.insert(reply, key);
    }
}

void EquipmentPanel::IconDownloaded(QNetworkReply *reply)
{
    const QString key = d->replies.take(reply);
    const QPixmap pixmap = QPixmap::fromImage(QImage::fromData(reply->readAll()));
    reply->deleteLater();
    if (!pixmap.isNull()) d->iconCache.insert(key, pixmap);
    const auto waiters = d->iconWaiters.take(key);
    for (const auto &waiter : waiters) waiter(pixmap);
}
