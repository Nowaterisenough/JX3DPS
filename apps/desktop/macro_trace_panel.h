#ifndef DESKTOP_MACRO_TRACE_PANEL_H
#define DESKTOP_MACRO_TRACE_PANEL_H

#include <QWidget>

#include "macro_step.h"

class QCheckBox;
class QComboBox;
class QHBoxLayout;
class QLabel;
class QLineEdit;
class QPushButton;
class QTableView;
class QTextBrowser;
class QResizeEvent;
class MacroStepModel;

class MacroTracePanel : public QWidget
{
    Q_OBJECT

public:
    explicit MacroTracePanel(QWidget *parent = nullptr);
    void Reset(const QString &source = {});
    void Append(const MacroSteps &steps);
    bool ExportCsv(const QString &path, QString &error) const;
signals:
    void SourceRequested(const QString &source, int line);

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    QString         m_source;
    MacroStepModel *m_model;
    QTableView     *m_table;
    QComboBox      *m_scope;
    QLineEdit      *m_search;
    QCheckBox      *m_follow;
    QLabel         *m_status;
    QTextBrowser   *m_details;
    QPushButton    *m_export;
    QPushButton    *m_locate;
    QHBoxLayout    *m_filters;
    QHBoxLayout    *m_actions;
    QWidget        *m_actionRow;
    bool            m_wide = false;
    void            Refresh(const MacroSteps &steps = {}, bool reset = false);
    void            ShowSelection();
    void            LocateSelection();
};

#endif
