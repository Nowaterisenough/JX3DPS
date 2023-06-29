/**
 * Project: JX3DPS
 * File: TabWidget copy.h
 * Description:
 * Created Date: 2023-06-10 08:38:29
 * Author: 难为水
 * -----
 * Last Modified: 2023-06-29 19:34:06
 * Modified By: 难为水
 * -----
 * HISTORY:
 * Date      	By   	Comments
 * ----------	-----	----------------------------------------------------------
 */

#ifndef COMBOBOX_H
#define COMBOBOX_H

#include <QComboBox>
#include <QListWidget>
#include <QPixmap>
#include <QStyledItemDelegate>

#include "Button.h"

class ItemDelegateClass : public QStyledItemDelegate
{
    Q_OBJECT

public:
    explicit ItemDelegateClass(QObject *parent = nullptr);

    virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

    virtual QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};

class TableModelClass : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit TableModelClass(QObject *parent = nullptr);

    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
};

class ComboBoxClass : public QComboBox
{
    Q_OBJECT

public:
    ComboBoxClass(QWidget *parent = nullptr);

protected:
    virtual void paintEvent(QPaintEvent *event) override;
    virtual void enterEvent(QEnterEvent *event) override;
    virtual void showPopup() override;

signals:
    void Signal_CurrentClassChanged(int index);

private:
    QPixmap *m_pixmap = nullptr;
};

struct TalentInfo
{
    int id     = -1;
    int iconId = -1;

    std::string name;
    std::string type;
    std::string desc;
};

class ItemWidgetTalent : public QWidget
{
    Q_OBJECT

public:
    explicit ItemWidgetTalent(const TalentInfo &talentInfo, QWidget *parent = nullptr);
    ~ItemWidgetTalent();

    void HoveredEvent(bool isHovered);
    void SetSelected(bool isSelected);

    QPixmap *GetIcon() const;
    QString  GetName() const;

protected:
    virtual void paintEvent(QPaintEvent *event) override;

    virtual void enterEvent(QEnterEvent *event) override;
    virtual void leaveEvent(QEvent *event) override;

private:
    bool m_isHovered = false;
    bool m_selected  = false;

    QPixmap *m_pixmap = nullptr;
    QString  m_name;
    QString  m_desc;
};

class ListWidgetTalent : public QListWidget
{
    Q_OBJECT

public:
    explicit ListWidgetTalent(QWidget *parent = nullptr);
    ~ListWidgetTalent();

    virtual QRect visualRect(const QModelIndex &index) const override;

protected:
    void wheelEvent(QWheelEvent *event) override;

    void paintEvent(QPaintEvent *event) override;
};

class ComboBoxTalent : public QComboBox
{
    Q_OBJECT

public:
    ComboBoxTalent(QWidget *parent = nullptr);

    void    AddItem(const TalentInfo &talentInfo);
    QString GetName() const;

protected:
    void paintEvent(QPaintEvent *event) override;
    void showPopup() override;
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;

signals:
    void Signal_CurrentTalentChanged(int index);

private:
    QPixmap *m_pixmap = nullptr;
    QString  m_name;
    bool     m_isHovered = false;
};

class TalentWidget : public QWidget
{
    Q_OBJECT

public:
    TalentWidget(QWidget *parent = nullptr);

    void AddItem(const TalentInfo &talentInfo);

private:
    ComboBoxTalent *m_comboBox = nullptr;
    TextButton     *m_button   = nullptr;
};

#endif // COMBOBOX_H
