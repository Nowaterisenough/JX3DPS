#ifndef DESKTOP_BUFF_PANEL_H
#define DESKTOP_BUFF_PANEL_H

#include <QWidget>
#include "buff_trace.h"
#include "controls/timeline/timeline.h"

class QComboBox;
class QLabel;
class QLineEdit;
class QPushButton;
class QTableWidget;

class BuffPanel : public QWidget {
    Q_OBJECT
public:
    explicit BuffPanel(QWidget *parent = nullptr);
    void Reset(int duration, desktop::Specialization specialization = desktop::Specialization::TaiXu);
    void Append(const BuffEvents &events);
    void Observe(int frame);
    const QVector<Timeline::BuffItem> &TimelineBuffs() const { return m_timeline; }
    bool ExportCsv(const QString &path, bool coverage, QString &error) const;

signals:
    void CoverageChanged();
    void DamageRequested(quint32 sequence);

private:
    BuffTrace m_trace;
    desktop::Specialization m_specialization = desktop::Specialization::TaiXu;
    QVector<Timeline::BuffItem> m_timeline;
    QLabel *m_status;
    QComboBox *m_scope;
    QLineEdit *m_filter;
    QTableWidget *m_coverage;
    QTableWidget *m_changes;
    QPushButton *m_exportCoverage;
    QPushButton *m_exportChanges;

    bool Matches(const BuffEvent &event) const;
    void Refresh();
    void FilterChanges();
};

#endif
