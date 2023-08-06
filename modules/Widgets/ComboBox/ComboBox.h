/**
 * Project: JX3DPS
 * File: TabWidget copy.h
 * Description:
 * Created Date: 2023-06-10 08:38:29
 * Author: 难为水
 * -----
 * Last Modified: 2023-08-07 06:04:02
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
class Icon;

struct ComboBoxItemInfo
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
    explicit ItemWidget(const ComboBoxItemInfo &itemInfo, QWidget *parent = nullptr);
    explicit ItemWidget(QWidget *parent = nullptr);

    void SetHovered(bool hovered);
    void SetSelected(bool selected);

    void SetComboBoxView();

    ComboBoxItemInfo GetItemInfo() const;
    void     SetItemInfo(const ComboBoxItemInfo &itemInfo);

signals:
    void Signal_Hovered(bool hovered);

protected:
    void paintEvent(QPaintEvent *event) override;
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void focusOutEvent(QFocusEvent *event) override;

private:
    bool m_hovered  = false;
    bool m_selected = false;

    bool m_view = false;

    ComboBoxItemInfo m_itemInfo;

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

enum class COMBO_BOX_API ComboBoxType
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

    void     AddItem(const ComboBoxItemInfo &itemInfo);
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

class COMBO_BOX_API ComboBox : public QWidget
{
    Q_OBJECT

public:
    ComboBox(ComboBoxType type, QWidget *parent = nullptr);

    void     AddItem(const ComboBoxItemInfo &itemInfo);
    void     SetItemSize(int width, int height);
    ComboBoxItemInfo GetItemInfo() const;

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

#endif // __COMBO_BOX_H__
