#ifndef DESKTOP_HISTORY_PANEL_H
#define DESKTOP_HISTORY_PANEL_H

#include <QWidget>

#include "combat_history.h"

class QLabel;
class QGridLayout;
class QFormLayout;

class HistoryPanel : public QWidget
{
    Q_OBJECT

public:
    explicit HistoryPanel(QWidget *parent = nullptr);
    void Clear();
    void Append(const desktop::HistoryChunk &chunk);
    void Inspect(double milliseconds);

    const desktop::CombatHistory &History() const { return m_history; }

private:
    desktop::CombatHistory m_history;
    int                    m_frame = -1;
    QLabel                *m_time, *m_status, *m_resources;
    QWidget               *m_content;
    QFormLayout           *m_attributes;
    QGridLayout           *m_self, *m_target, *m_skills;
    void                   Refresh();
};
#endif
