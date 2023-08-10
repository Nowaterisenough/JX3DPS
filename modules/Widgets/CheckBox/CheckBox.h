/**
 * Project: JX3DPS
 * File: TabWidget copy.h
 * Description:
 * Created Date: 2023-06-10 08:38:29
 * Author: 难为水
 * -----
 * Last Modified: 2023-08-11 05:51:38
 * Modified By: 难为水
 * -----
 * HISTORY:
 * Date      	By   	Comments
 * ----------	-----	----------------------------------------------------------
 */

#ifndef __CHECK_BOX_H__
#define __CHECK_BOX_H__

#if defined _WIN32 || defined __CYGWIN__
#    if defined EXPORT_CHECK_BOX // CMake add_definitions
#        ifdef __GNUC__
#            define CHECK_BOX_API __attribute__((dllexport))
#        else
#            define CHECK_BOX_API __declspec(dllexport)
#        endif // __GNUC__
#    else
#        ifdef __GNUC__
#            define CHECK_BOX_API __attribute__((dllimport))
#        else
#            ifndef IMPORT_STATIC_CHECK_BOX_LIB
#                define CHECK_BOX_API __declspec(dllimport)
#            else
#                define CHECK_BOX_API
#            endif
#        endif // __GNUC__
#    endif     // EXPORT_CHECK_BOX
#    define CHECK_BOX_PRIVATE

#elif defined __GNUC__
#    if __GNUC__ >= 4
#        define CHECK_BOX_API     __attribute__((visibility("default")))
#        define CHECK_BOX_PRIVATE __attribute__((visibility("hidden")))
#    else
#        define CHECK_BOX_API
#        define CHECK_BOX_PRIVATE
#    endif // __GNUC__ >= 4

#elif defined __clang__
#    define CHECK_BOX_API     __attribute__((visibility("default")))
#    define CHECK_BOX_PRIVATE __attribute__((visibility("hidden")))

#else
#    error "Do not know how to export classes for this platform"
#endif // defined(_WIN32) || defined(__CYGWIN__)

#include <QCheckBox>
#include <QColor>
#include <QPixmap>



class CHECK_BOX_API CheckBox : public QCheckBox
{
    Q_OBJECT

public:
    struct ItemInfo
    {
        QString name;
        QString iconPath;
        QString description;
    };

    CheckBox(QWidget *parent = nullptr);

    void     SetItemInfo(const ItemInfo &itemInfo);
    ItemInfo GetItemInfo() const;

protected:
    virtual void paintEvent(QPaintEvent *event) override;
    virtual void enterEvent(QEnterEvent *event) override;
    virtual void leaveEvent(QEvent *event) override;
    virtual void mousePressEvent(QMouseEvent *event) override;

private:
    bool m_hovered = false;

    ItemInfo m_itemInfo;
};

class CHECK_BOX_API CheckBoxIcon : public QCheckBox
{
    Q_OBJECT

public:
    CheckBoxIcon(QWidget *parent = nullptr);

    void SetItemInfo(const CheckBox::ItemInfo &itemInfo);

protected:
    virtual void paintEvent(QPaintEvent *event) override;
    virtual void enterEvent(QEnterEvent *event) override;
    virtual void leaveEvent(QEvent *event) override;
    virtual void mousePressEvent(QMouseEvent *event) override;

private:
    bool m_hovered = false;

    QPixmap m_pixmap;

    CheckBox::ItemInfo m_itemInfo;
};

#endif // __CHECK_BOX_H__
