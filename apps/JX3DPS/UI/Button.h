/**
 * Project: JX3DPS
 * File: TabWidget copy.h
 * Description:
 * Created Date: 2023-06-10 08:38:29
 * Author: 难为水
 * -----
 * Last Modified: 2023-06-18 18:18:29
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
    QColor m_color = QColor(139, 146, 154);
};



class GreenButton : public QPushButton
{
    Q_OBJECT
    Q_PROPERTY(QColor color READ GetColor WRITE SetColor)
    Q_PROPERTY(QColor textColor READ GetTextColor WRITE SetTextColor)

public:
    GreenButton(QWidget *parent = nullptr);

    QColor GetColor() const;
    void   SetColor(QColor color);

    QColor GetTextColor() const;
    void   SetTextColor(QColor color);

protected:
    void paintEvent(QPaintEvent *event) override;
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;

private:
    QColor m_color = QColor(54, 190, 62);
    QColor m_textColor = QColor(193, 255, 203);
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
    QColor m_color = QColor(23, 29, 37);
    QColor m_textColor = QColor(139, 146, 154);
};

#endif // BUTTON_H
