#ifndef DESKTOP_GAME_ICONS_H
#define DESKTOP_GAME_ICONS_H

#include <QCheckBox>
#include <QComboBox>
#include <QIcon>

#include "runtime_catalog.h"

class QListView;
class QKeyEvent;
class QMouseEvent;
class QPainter;

namespace desktop {
class ChoicePopup;
QIcon   GameIcon(int id);
QIcon   GameIcon(Specialization specialization, const QString &name);
QString GameDescription(Specialization specialization, const QString &name);
void    PaintGameIcon(QPainter &painter, const QRect &bounds, const QIcon &icon, bool grayscale = false);

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
    explicit IconComboBox(const QString &category, QWidget *parent = nullptr, bool has_empty_choice = true);

    QSize sizeHint() const override
    {
        const int extent = m_hasEmptyChoice ? 44 : 38;
        return { extent, extent };
    }

    QSize minimumSizeHint() const override { return sizeHint(); }

    void showPopup() override;
    void hidePopup() override;

    QListView *choiceView() const { return m_choices; }

protected:
    void paintEvent(QPaintEvent *) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private:
    ChoicePopup *m_popup;
    QListView   *m_choices;
    bool         m_hasEmptyChoice;
    void         AcceptChoice();
};
} // namespace desktop
#endif
