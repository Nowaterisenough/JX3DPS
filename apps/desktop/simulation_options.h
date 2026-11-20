#ifndef DESKTOP_SIMULATION_OPTIONS_H
#define DESKTOP_SIMULATION_OPTIONS_H

#include <array>
#include <functional>
#include <vector>

#include <QJsonObject>
#include <QWidget>

#include "src/class/tai_xu_jian_yi/runtime_data.hpp"
#include "src/core/runtime/batch.hpp"

#include "attribute_gain_mode.h"
#include "runtime_catalog.h"

class QSpinBox;
class QDoubleSpinBox;
class QLineEdit;
class QVBoxLayout;
class QComboBox;
class QTabWidget;
class QDialog;
class QToolButton;
class QTabBar;
class QLabel;
class CodeEditor;
struct SimulationResult;

namespace desktop {
class TalentPanel;
class GameAttributePanel;
class AttributeGainsPanel;
} // namespace desktop

namespace desktop {
class FlowTimelineEditor;
} // namespace desktop

class SimulationOptions : public QWidget
{
    Q_OBJECT

public:
    explicit SimulationOptions(QWidget *parent = nullptr);
    ~SimulationOptions() override;

    // Wrapped descriptions must not expand the entire workspace.
    int heightForWidth(int) const override { return minimumSizeHint().height(); }

    JX3DPS::runtime::tai_xu::Config Config() const;
    desktop::Config                 ActiveConfig() const;
    desktop::Specialization         Specialization() const;
    JX3DPS::runtime::BatchOptions   Batch() const;
    void                            SaveSettings() const;
    QJsonObject                     Snapshot() const;
    bool                            RestoreSnapshot(const QJsonObject &snapshot, QString &error);
    bool                            ExportConfiguration(const QString &path, QString &error) const;
    bool                            ImportConfiguration(const QString &path, QString &error);
    bool                            ImportAttributes(const QJsonObject &attributes, QString &error);
    void                            SetInputsEnabled(bool enabled);
    void                            ShowAttributeGains();
    void                            ShowSection(const QString &section);
    QWidget                        *TakeOutputPanel();
    void                            SetAttributeGainResult(const SimulationResult &result);
    void                            ClearAttributeGains();
    desktop::GainComparisonMode     GainMode() const;
    bool                            AutoCalculateGains() const;
    void                            ShowEquipment();
    void                            CloseAuxiliaryWindows();
    bool                            CheckFlowMacro(QString &error) const;
    void                            FocusFlowMacro();

    QVBoxLayout *RunActions() const { return m_runActions; }

    QVBoxLayout *OutputLayout() const { return m_output; }

    QToolButton *GainsButton() const;

signals:
    void SpecializationChanged();

private:
    QJsonObject                     Values() const;
    JX3DPS::runtime::tai_xu::Config ConfigFromValues(const QJsonObject &values) const;
    desktop::Config ActiveConfigFromValues(const QJsonObject                            &values,
                                           desktop::Specialization                       specialization,
                                           std::span<const JX3DPS::runtime::TargetEvent> events = {}) const;
    QJsonObject     NormalizeSnapshot(const QJsonObject &snapshot) const;
    bool            ValidateSnapshot(const QJsonObject &snapshot, QString &error) const;
    void            SetupProfiles(QVBoxLayout *layout);
    void            RefreshGameAttributes();
    void            UpdateFlowMacro();
    void            UpdateTalentHeight();
    QVBoxLayout    *m_runActions;
    QVBoxLayout    *m_output;
    QWidget        *m_attributePane;
    desktop::GameAttributePanel              *m_gameAttributes;
    desktop::AttributeGainsPanel             *m_gameGains;
    QTabBar                                  *m_permanentsInPanel;
    QDialog                                  *m_attributeEditor;
    QDialog                                  *m_equipmentDialog;
    CodeEditor                               *m_flowEditor;
    desktop::FlowTimelineEditor              *m_flowTimeline;
    QLabel                                   *m_flowStatus;
    QJsonObject                               m_flowValues;
    std::vector<JX3DPS::runtime::TargetEvent> m_flowEvents;
    QString                                   m_flowSource;
    bool                                      m_flowInitialized = false;
    QJsonObject                               m_gainSnapshot;
    bool                                      m_refreshQueued  = false;
    QString                                   m_section        = QStringLiteral("simulation");
    bool                                      m_externalOutput = false;
    std::array<desktop::TalentPanel *, 2>     m_talentPanels{};
    std::vector<QWidget *>                    m_inputPanels;
    std::vector<std::function<void(JX3DPS::runtime::tai_xu::Config &, const QJsonObject &)>> m_bindings;
    std::vector<std::function<void(JX3DPS::runtime::mo_wen::Config &, const QJsonObject &)>> m_mowenBindings;
    QComboBox                                                                               *m_specialization;
    std::vector<QComboBox *>                                                                 m_permanents;
    QSpinBox                                                                                *m_iterations;
    QSpinBox                                                                                *m_workers;
    QDoubleSpinBox                                                                          *m_seconds;
    QLineEdit                                                                               *m_seed;
};

#endif
