#ifndef DESKTOP_RESULTS_PANEL_H
#define DESKTOP_RESULTS_PANEL_H

#include <optional>

#include <QDateTime>
#include <QJsonObject>
#include <QMap>
#include <QWidget>

#include "combat_event.h"
#include "simulation_result.h"

class QLabel;
class QPushButton;
class QTableWidget;
class QTextBrowser;
class QTabWidget;
class FightHotspotChart;
class FightFlameGraph;

namespace desktop {
class HistoryChart;
} // namespace desktop

class ResultsPanel : public QWidget
{
    Q_OBJECT

public:
    explicit ResultsPanel(QWidget *parent = nullptr);
    void    AddResult(const SimulationResult &result, const QString &source, const QJsonObject &configuration);
    void    ClearTrace(double seconds);
    void    AppendTrace(const CombatEvents &events);
    void    SetInputsEnabled(bool enabled);
    void    ShowSkillStatistics();
    void    ShowOverview();
    bool    ShowSection(const QString &id);
    QString CurrentSection() const;
    void    UseWorkspaceNavigation();
    bool    ExportSelected(const QString &path, QString &error) const;

signals:
    void InputsRequested(const QString &source, const QJsonObject &configuration);
    void SectionChanged();
    void HotspotRunRequested();
    void GainRunRequested();

private:
    struct Entry
    {
        quint64          id;
        QDateTime        time;
        SimulationResult result;
        QString          source;
        QJsonObject      configuration;
    };

    using Skill = SkillStatistics;
    QVector<Entry>         m_entries;
    std::optional<Entry>   m_baseline;
    QMap<int, Skill>       m_skills;
    quint64                m_nextId       = 1;
    double                 m_traceSeconds = 1;
    qint64                 m_traceDamage  = 0;
    quint64                m_traceCount   = 0;
    QTabWidget            *m_tabs;
    QTableWidget          *m_history;
    desktop::HistoryChart *m_chart;
    QTableWidget          *m_damage;
    QTableWidget          *m_gains;
    QLabel                *m_hotspotSummary;
    QWidget               *m_hotspotPage;
    QTabWidget            *m_hotspotViews;
    QPushButton           *m_collectHotspots;
    QPushButton           *m_runGains;
    FightFlameGraph       *m_flame;
    FightHotspotChart     *m_hotspotChart;
    QTableWidget          *m_hotspotTable;
    QTextBrowser          *m_details;
    QLabel                *m_traceSummary;
    QPushButton           *m_useInputs;
    QPushButton           *m_setBaseline;
    QPushButton           *m_clearBaseline;
    QPushButton           *m_export;
    bool                   m_inputsEnabled = true;

    const Entry *Selected() const;
    void         RefreshHistory();
    void         ShowDetails();
    void         RenderSkills(QVector<Skill> skills, double total_damage, double total_seconds);
    void         ShowResultSkills(const SimulationResult &result);
};

#endif
