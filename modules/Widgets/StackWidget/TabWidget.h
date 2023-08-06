/**
 * Project: JX3DPS
 * File: TabWidget copy.h
 * Description:
 * Created Date: 2023-06-10 08:38:29
 * Author: 难为水
 * -----
 * Last Modified: 2023-06-29 20:34:35
 * Modified By: 难为水
 * -----
 * HISTORY:
 * Date      	By   	Comments
 * ----------	-----	----------------------------------------------------------
 */

#ifndef TABWIDGET_H
#define TABWIDGET_H

#include <QStringList>
#include <QWidget>

#include "Button.h"

class TWidget : public QWidget
{
    Q_OBJECT

public:
    TWidget(QWidget *parent = nullptr);

protected:
    virtual void paintEvent(QPaintEvent *event) override;
};

class TabButton : public QPushButton
{
    Q_OBJECT
    Q_PROPERTY(QColor color READ GetColor WRITE SetColor)
    Q_PROPERTY(int line READ GetLine WRITE SetLine)

public:
    TabButton(QWidget *parent = nullptr);

    QColor GetColor() const;
    void   SetColor(QColor color);

    int  GetLine() const;
    void SetLine(int line);

protected:
    void paintEvent(QPaintEvent *event) override;
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;

private:
    QColor m_color = QColor(COLOR_HIGHLIGHT);
    int    m_line  = 0;
};

class TabWidget : public QWidget
{
    Q_OBJECT

public:
    TabWidget(QWidget *parent = nullptr);

    void     AddTab(const QString &text);
    TWidget *Widget(int index);

protected:
    virtual void resizeEvent(QResizeEvent *event) override;

private:
    QList<QPair<TabButton *, TWidget *>> m_tabs;
};

class VTabButton : public QPushButton
{
    Q_OBJECT
    Q_PROPERTY(QColor color READ GetColor WRITE SetColor)
    Q_PROPERTY(int line READ GetLine WRITE SetLine)

public:
    VTabButton(QWidget *parent = nullptr);

    QColor GetColor() const;
    void   SetColor(QColor color);

    int  GetLine() const;
    void SetLine(int line);

    static int s_width;

protected:
    void paintEvent(QPaintEvent *event) override;
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;

private:
    QColor m_color = QColor(COLOR_HIGHLIGHT);
    int    m_line  = 0;
};

class VTabWidget : public QWidget
{
    Q_OBJECT

public:
    VTabWidget(QWidget *parent = nullptr);

    void     AddTab(const QString &text);
    TWidget *Widget(int index);

protected:
    virtual void resizeEvent(QResizeEvent *event) override;

private:
    QList<QPair<VTabButton *, TWidget *>> m_tabs;
};

class VerticalTabButton : public QPushButton
{
    Q_OBJECT
    Q_PROPERTY(QColor color READ GetColor WRITE SetColor)
    Q_PROPERTY(int line READ GetLine WRITE SetLine)

public:
    VerticalTabButton(QWidget *parent = nullptr);

    QColor GetColor() const;
    void   SetColor(QColor color);

    int  GetLine() const;
    void SetLine(int line);

protected:
    void paintEvent(QPaintEvent *event) override;
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;

private:
    QColor m_color = QColor(COLOR_HIGHLIGHT);
    int    m_line  = 0;
};

class VerticalTabWidget : public QWidget
{
    Q_OBJECT

    Q_PROPERTY(int dx READ GetDx WRITE SetDx)

public:
    VerticalTabWidget(QWidget *parent = nullptr);

    void     AddTab(const QString &text);
    TWidget *Widget(int index);
    void     Resize();
    void     AnimatedResize(int index);

    int  GetDx() const;
    void SetDx(int dx);

protected:
    virtual void resizeEvent(QResizeEvent *event) override;
    virtual void paintEvent(QPaintEvent *event) override;

private:
    QList<QPair<VerticalTabButton *, TWidget *>> m_tabs;

    int m_dx = 0;
    int m_lastIndex = 0; // 上一个选中的索引
    int m_currentIndex = 0; // 当前选中的索引
    bool m_flag = false; // 是否正在动画
};

#endif // TABWIDGET_H
