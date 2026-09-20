#ifndef DESKTOP_COMBAT_EVENT_H
#define DESKTOP_COMBAT_EVENT_H

#include <QMetaType>
#include <QString>
#include <QVector>

struct CombatEvent {
    int frame = 0;
    int skill = 0;
    QString name;
    qint64 damage = 0;
    int outcome = 0;
    quint32 sequence = 0;
};

using CombatEvents = QVector<CombatEvent>;
Q_DECLARE_METATYPE(CombatEvents)

#endif
