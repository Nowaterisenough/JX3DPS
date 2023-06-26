/**
 * Project: JX3DPS
 * File: TabWidget copy.h
 * Description:
 * Created Date: 2023-06-10 08:38:29
 * Author: 难为水
 * -----
 * Last Modified: 2023-06-18 09:10:44
 * Modified By: 难为水
 * -----
 * HISTORY:
 * Date      	By   	Comments
 * ----------	-----	----------------------------------------------------------
 */

#ifndef COMBOBOX_H
#define COMBOBOX_H

#include <QComboBox>
#include <QPixmap>
#include <QStyledItemDelegate>

class ItemDelegateClass : public QStyledItemDelegate
{
    Q_OBJECT

public:
    explicit ItemDelegateClass(QObject *parent = nullptr);

    virtual void
    paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

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

    virtual QVariant
    headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
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

class ComboBoxTalent : public QComboBox
{
    Q_OBJECT

public:
    ComboBoxTalent(QWidget *parent = nullptr);

protected:
    virtual void paintEvent(QPaintEvent *event) override;
    virtual void enterEvent(QEnterEvent *event) override;
    virtual void showPopup() override;

signals:
    void Signal_CurrentTalentChanged(int index);

private:
    QPixmap *m_pixmap = nullptr;
};

#endif // COMBOBOX_H
