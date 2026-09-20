#ifndef DESKTOP_GAME_ICONS_H
#define DESKTOP_GAME_ICONS_H

#include <QCheckBox>
#include <QComboBox>
#include <QIcon>

#include "runtime_catalog.h"

namespace desktop {
QIcon   GameIcon(int id);
QIcon   GameIcon(Specialization specialization, const QString &name);
QString GameDescription(Specialization specialization, const QString &name);

class IconCheckBox : public QCheckBox
{
public:
    IconCheckBox(const QString &name, Specialization specialization, QWidget *parent = nullptr);

    QSize sizeHint() const override { return { 44, 44 }; }

    QSize minimumSizeHint() const override { return sizeHint(); }

protected:
    void paintEvent(QPaintEvent *) override;

    bool hitButton(const QPoint &position) const override { return rect().contains(position); }
};

class IconComboBox : public QComboBox
{
public:
    explicit IconComboBox(const QString &category, QWidget *parent = nullptr);

    QSize sizeHint() const override { return { 48, 44 }; }

    QSize minimumSizeHint() const override { return sizeHint(); }

    void showPopup() override;

protected:
    void paintEvent(QPaintEvent *) override;
};
} // namespace desktop
#endif
