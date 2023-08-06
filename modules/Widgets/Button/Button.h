/**
 * Project: JX3DPS
 * File: TabWidget copy.h
 * Description:
 * Created Date: 2023-06-10 08:38:29
 * Author: 难为水
 * -----
 * Last Modified: 2023-07-02 19:20:14
 * Modified By: 难为水
 * -----
 * HISTORY:
 * Date      	By   	Comments
 * ----------	-----	----------------------------------------------------------
 */

#ifndef BUTTON_H
#define BUTTON_H

#include <QColor>
#include <QPushButton>

#include "ThemeColors.h"

class TextButton : public QPushButton
{
    Q_OBJECT
    Q_PROPERTY(QColor color READ GetColor WRITE SetColor)

public:
    TextButton(QWidget *parent = nullptr);

    QColor GetColor() const;
    void   SetColor(QColor color);

protected:
    void paintEvent(QPaintEvent *event) override;
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;

private:
    QColor m_color = QColor(COLOR_INACTIVE);
};

class Button : public QPushButton
{
    Q_OBJECT
    Q_PROPERTY(QColor color READ GetColor WRITE SetColor)
    Q_PROPERTY(QColor textColor READ GetTextColor WRITE SetTextColor)

public:
    Button(QWidget *parent = nullptr);

    QColor GetColor() const;
    void   SetColor(QColor color);

    QColor GetTextColor() const;
    void   SetTextColor(QColor color);

    void SetButtonColor(const QColor &hover, const QColor &normal);

protected:
    void paintEvent(QPaintEvent *event) override;
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;

private:
    QColor m_textColor = QColor(193, 255, 203);
    QColor m_hoverColor = QColor(COLOR_BUTTON_BLUE_HOVER);
    QColor m_normalColor = QColor(COLOR_BUTTON_BLUE_NORMAL);
    QColor m_color     = m_normalColor;
};

class CloseButton : public QPushButton
{
    Q_OBJECT
    Q_PROPERTY(QColor color READ GetColor WRITE SetColor)
    Q_PROPERTY(QColor textColor READ GetTextColor WRITE SetTextColor)

public:
    CloseButton(QWidget *parent = nullptr);

    QColor GetColor() const;
    void   SetColor(QColor color);

    QColor GetTextColor() const;
    void   SetTextColor(QColor color);

protected:
    void paintEvent(QPaintEvent *event) override;
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;

private:
    QColor m_color     = QColor(COLOR_BACKGROUND_BASE);
    QColor m_textColor = QColor(COLOR_INACTIVE);
};

#endif // BUTTON_H
