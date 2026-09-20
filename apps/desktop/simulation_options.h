#ifndef DESKTOP_SIMULATION_OPTIONS_H
#define DESKTOP_SIMULATION_OPTIONS_H

#include <functional>
#include <vector>

#include <QJsonObject>
#include <QWidget>

#include "src/class/tai_xu_jian_yi/runtime_data.hpp"
#include "src/core/runtime/batch.hpp"

#include "runtime_catalog.h"

class QSpinBox;
class QDoubleSpinBox;
class QLineEdit;
class QVBoxLayout;
class QComboBox;

class SimulationOptions : public QWidget
{
    Q_OBJECT

public:
    explicit SimulationOptions(QWidget *parent = nullptr);

    // Each form scrolls independently; wrapped labels must not expand the entire workspace.
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

    QVBoxLayout *RunActions() const { return m_runActions; }

    QVBoxLayout *OutputLayout() const { return m_output; }

    QVBoxLayout *GainsLayout() const { return m_gains; }

signals:
    void SpecializationChanged();

private:
    QJsonObject                     Values() const;
    JX3DPS::runtime::tai_xu::Config ConfigFromValues(const QJsonObject &values) const;
    desktop::Config ActiveConfigFromValues(const QJsonObject &values, desktop::Specialization specialization) const;
    QJsonObject     NormalizeSnapshot(const QJsonObject &snapshot) const;
    bool            ValidateSnapshot(const QJsonObject &snapshot, QString &error) const;
    void            SetupProfiles(QVBoxLayout *layout);
    QVBoxLayout    *m_runActions;
    QVBoxLayout    *m_output;
    QVBoxLayout    *m_gains;
    std::vector<QWidget *>                                                                   m_inputPanels;
    std::vector<std::function<void(JX3DPS::runtime::tai_xu::Config &, const QJsonObject &)>> m_bindings;
    std::vector<std::function<void(JX3DPS::runtime::mo_wen::Config &, const QJsonObject &)>> m_mowenBindings;
    QComboBox                                                                               *m_specialization;
    QComboBox                                                                               *m_style;
    std::vector<QComboBox *>                                                                 m_permanents;
    QSpinBox                                                                                *m_iterations;
    QSpinBox                                                                                *m_workers;
    QDoubleSpinBox                                                                          *m_seconds;
    QLineEdit                                                                               *m_seed;
};

#endif
