/**
 * Project: JX3DPS
 * File: TabWidget copy.h
 * Description:
 * Created Date: 2023-06-10 08:38:29
 * Author: 难为水
 * -----
 * Last Modified: 2023-07-18 01:58:20
 * Modified By: 难为水
 * -----
 * HISTORY:
 * Date      	By   	Comments
 * ----------	-----	----------------------------------------------------------
 */

#ifndef COMBOBOX_H
#define COMBOBOX_H

#include <QComboBox>
#include <QLabel>
#include <QLayout>
#include <QListWidget>

class MarqueeLabel;
class Icon;

struct ItemInfo
{
    int         id = 0;
    std::string name;
    std::string icon;
    std::string desc;

    std::vector<std::pair<std::string, int>> subItems;
};

class ItemWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ItemWidget(const ItemInfo &itemInfo, QWidget *parent = nullptr);
    explicit ItemWidget(QWidget *parent = nullptr);

    void SetHovered(bool hovered);
    void SetSelected(bool selected);

    void SetComboBoxView();

    ItemInfo GetItemInfo() const;
    void     SetItemInfo(const ItemInfo &itemInfo);

signals:
    void Signal_Hovered(bool hovered);

protected:
    void paintEvent(QPaintEvent *event) override;
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;

private:
    bool m_hovered  = false;
    bool m_selected = false;

    bool m_view = false;

    ItemInfo m_itemInfo;

    QLabel       *m_nameText = nullptr;
    MarqueeLabel *m_descText = nullptr;
    Icon         *m_icon     = nullptr;

    QHBoxLayout *m_hLayout = nullptr;
    QVBoxLayout *m_vLayout = nullptr;
};

class ListWidget : public QListWidget
{
    Q_OBJECT

public:
    explicit ListWidget(QWidget *parent = nullptr);

    virtual QRect visualRect(const QModelIndex &index) const override;

protected:
    void wheelEvent(QWheelEvent *event) override;

private:
    QListWidgetItem *m_lastEnteredItem = nullptr;
};

enum class ComboBoxType
{
    ICON_MODE = 0,
    ICON_AND_NAME_MODE,
    DETAILED_MODE,
};

class SubComboBox : public QComboBox
{
    Q_OBJECT

public:
    SubComboBox(ComboBoxType type, QWidget *parent = nullptr);

    void     AddItem(const ItemInfo &itemInfo);
    void     SetItemSize(int width, int height);
    QWidget *View();

protected:
    void paintEvent(QPaintEvent *event) override;
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void showPopup() override;

private:
    QPixmap *m_pixmap  = nullptr;
    bool     m_hovered = false;
    int      m_width   = 0;
    int      m_height  = 0;
    QWidget *m_view    = nullptr;
};

class ComboBox : public QWidget
{
    Q_OBJECT

public:
    ComboBox(ComboBoxType type, QWidget *parent = nullptr);

    void AddItem(const ItemInfo &itemInfo);
    void SetItemSize(int width, int height);
    ItemInfo GetItemInfo() const;

protected:
    void paintEvent(QPaintEvent *event) override;
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;

private:
    SubComboBox *m_comboBox = nullptr;
    QLabel      *m_label    = nullptr;
    bool         m_hovered  = false;
    ComboBoxType m_type     = ComboBoxType::ICON_MODE;
};

#endif // COMBOBOX_H
