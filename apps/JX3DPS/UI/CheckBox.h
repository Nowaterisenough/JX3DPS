/**
 * Project: JX3DPS
 * File: TabWidget copy.h
 * Description:
 * Created Date: 2023-06-10 08:38:29
 * Author: 难为水
 * -----
 * Last Modified: 2023-07-01 01:54:46
 * Modified By: 难为水
 * -----
 * HISTORY:
 * Date      	By   	Comments
 * ----------	-----	----------------------------------------------------------
 */

#ifndef CHECKBOX_H
#define CHECKBOX_H

#include <QCheckBox>
#include <QColor>
#include <QPixmap>

#include "ThemeColors.h"

class CheckBox : public QCheckBox
{
    Q_OBJECT

public:
    CheckBox(QWidget *parent = nullptr);

protected:
    virtual void paintEvent(QPaintEvent *event) override;
    virtual void enterEvent(QEnterEvent *event) override;
    virtual void leaveEvent(QEvent *event) override;
    virtual void mousePressEvent(QMouseEvent *event) override;

private:
    QColor m_color = QColor(COLOR_BACKGROUND_HIGHLIGHT);
};

class SecretImage : public QWidget
{
    Q_OBJECT

public:
    SecretImage(const QPixmap &pixmap, QWidget *parent = nullptr);

    void SetChecked(bool checked);

protected:
    void paintEvent(QPaintEvent *event) override;
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

signals:
    void Signal_Checked(bool checked);

private:
    QPixmap *m_pixmap  = nullptr;
    bool     m_checked = false;
    bool     m_hovered = false;
};

struct SecretInfo
{
    int iconId = -1;

    std::string name;
    std::string desc;
};

class SecretCheckBox : public QWidget
{
    Q_OBJECT

public:
    SecretCheckBox(const SecretInfo &info, QWidget *parent = nullptr);

private:
    CheckBox    *m_checkBox = nullptr;
    SecretImage *m_image    = nullptr;
};

#endif // CHECKBOX_H
