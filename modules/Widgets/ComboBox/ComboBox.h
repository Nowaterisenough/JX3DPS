/**
 * Project: JX3DPS
 * File: TabWidget copy.h
 * Description:
 * Created Date: 2023-06-10 08:38:29
 * Author: 难为水
 * -----
 * Last Modified: 2023-08-12 02:20:06
 * Modified By: 难为水
 * -----
 * HISTORY:
 * Date      	By   	Comments
 * ----------	-----	----------------------------------------------------------
 */

#ifndef __COMBO_BOX_H__
#define __COMBO_BOX_H__

#if defined _WIN32 || defined __CYGWIN__
#    if defined EXPORT_COMBO_BOX // CMake add_definitions
#        ifdef __GNUC__
#            define COMBO_BOX_API __attribute__((dllexport))
#        else
#            define COMBO_BOX_API __declspec(dllexport)
#        endif // __GNUC__
#    else
#        ifdef __GNUC__
#            define COMBO_BOX_API __attribute__((dllimport))
#        else
#            ifndef IMPORT_STATIC_COMBO_BOX_LIB
#                define COMBO_BOX_API __declspec(dllimport)
#            else
#                define COMBO_BOX_API
#            endif
#        endif // __GNUC__
#    endif     // EXPORT_COMBO_BOX
#    define COMBO_BOX_PRIVATE

#elif defined __GNUC__
#    if __GNUC__ >= 4
#        define COMBO_BOX_API     __attribute__((visibility("default")))
#        define COMBO_BOX_PRIVATE __attribute__((visibility("hidden")))
#    else
#        define COMBO_BOX_API
#        define COMBO_BOX_PRIVATE
#    endif // __GNUC__ >= 4

#elif defined __clang__
#    define COMBO_BOX_API     __attribute__((visibility("default")))
#    define COMBO_BOX_PRIVATE __attribute__((visibility("hidden")))

#else
#    error "Do not know how to export classes for this platform"
#endif // defined(_WIN32) || defined(__CYGWIN__)

#include <QComboBox>
#include <QLabel>
#include <QLayout>
#include <QListWidget>

class MarqueeLabel;
class SubComboBox;
class ItemWidget;
class Icon;

class COMBO_BOX_API ComboBox : public QWidget
{

    Q_OBJECT

public:
    enum Type
    {
        DEFAULT,
        ICON,
        ICON_NAME,
        DETAILED,
    };

    struct ItemInfo
    {
        QString name;
        QString iconPath;
        QString description;
    };

    ComboBox(QWidget *parent = nullptr);

    void SetType(Type type);
    void SetView(const ComboBox::ItemInfo &itemInfo);
    void AddItem(const ItemInfo &itemInfo);
    void SetItemSize(int width, int height);
    ItemInfo GetItemInfo() const;
    void Clear();

signals:
    void Signal_CurrentItemChanged(const ComboBox::ItemInfo &itemInfo);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    Type         m_type        = DEFAULT;
    SubComboBox *m_subComboBox = nullptr;
    QString      m_name        = "";
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

class SubComboBox : public QComboBox
{
    Q_OBJECT

public:
    SubComboBox(QWidget *parent = nullptr);

    void               SetType(ComboBox::Type type);
    void               SetView(const ComboBox::ItemInfo &itemInfo);
    void               AddItem(const ComboBox::ItemInfo &itemInfo);
    void               SetItemSize(int width, int height);
    ComboBox::ItemInfo GetItemInfo() const;

signals:
    void Signal_CurrentItemChanged(const ComboBox::ItemInfo &itemInfo);

protected:
    void paintEvent(QPaintEvent *event) override;
    void showPopup() override;

private:
    ItemWidget *m_detailedView = nullptr;
    Icon       *m_icon         = nullptr;

    int m_width  = 0;
    int m_height = 0;
};

class ItemWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ItemWidget(const ComboBox::ItemInfo &itemInfo, QWidget *parent = nullptr);
    explicit ItemWidget(QWidget *parent = nullptr);

    void               SetView();
    void               SetItemInfo(const ComboBox::ItemInfo &itemInfo);
    void               SetSelected(bool selected);
    ComboBox::ItemInfo GetItemInfo() const;
    void               SetHovered(bool hovered);

protected:
    void paintEvent(QPaintEvent *event) override;
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;

private:
    ComboBox::ItemInfo m_itemInfo;

    QLabel       *m_nameText = nullptr;
    MarqueeLabel *m_descText = nullptr;
    Icon         *m_icon     = nullptr;

    bool m_hovered  = false;
    bool m_selected = false;

    QHBoxLayout *m_hLayout = nullptr;
    QVBoxLayout *m_vLayout = nullptr;
};

#endif // __COMBO_BOX_H__
