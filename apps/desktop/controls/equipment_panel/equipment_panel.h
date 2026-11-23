#ifndef JX3DPS_DESKTOP_EQUIPMENT_PANEL_H
#define JX3DPS_DESKTOP_EQUIPMENT_PANEL_H

#include <memory>

#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QPixmap>
#include <QWidget>

class QGridLayout;
class QLabel;
class QListWidget;
class QListWidgetItem;
class QNetworkReply;
class QToolButton;

#ifdef EQUIPMENT_PANEL_BUILD
#    define EQUIPMENT_PANEL_API Q_DECL_EXPORT
#else
#    define EQUIPMENT_PANEL_API Q_DECL_IMPORT
#endif

/**
 * @brief 魔盒驱动的中文配装面板。
 *
 * 快照在首次打开配装时读取，图标优先命中本地资源，缺失时异步从
 * icon.jx3box.com 获取。点击候选装备即可替换对应部位并发出完整配置。
 */
class EQUIPMENT_PANEL_API EquipmentPanel final : public QWidget
{
    Q_OBJECT

public:
    explicit EquipmentPanel(QWidget *parent = nullptr);
    ~EquipmentPanel() override;

    QJsonObject Loadout() const;
    void SetSeasonSchool(const QString &name, const QString &faction, const QString &major, const QString &kind, const QJsonObject &reference);

signals:
    void LoadoutChanged(const QJsonObject &loadout);

protected:
    void hideEvent(QHideEvent *event) override;

private slots:
    void EquipCandidate(QListWidgetItem *item);
    void IconDownloaded(QNetworkReply *reply);

private:
    struct Impl;
    std::unique_ptr<Impl> d;

    void           BuildUi();
    void           PopulateFromSnapshot(const QJsonObject &snapshot);
    void           SetSlotItem(const QString &slot, const QJsonObject &item);
    void           UpdateSlotButton(const QString &slot);
    void           OpenSlot(const QString &slot);
    void           FilterCandidates();
    void           UpdateEffects();
    QString        ItemDetails(const QJsonObject &item, const QString &slot) const;
    void           AddIconItem(QListWidget *list, const QString &name, const QString &description, int icon_id);
    QIcon          RequestIcon(int icon_id);
    static QString SlotForItem(const QJsonObject &item);
    static QString IconUrl(int icon_id);
};

#endif // JX3DPS_DESKTOP_EQUIPMENT_PANEL_H
