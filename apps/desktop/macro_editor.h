#ifndef DESKTOP_MACRO_EDITOR_H
#define DESKTOP_MACRO_EDITOR_H

#include "controls/code_editor/code_editor.h"
#include "runtime_catalog.h"

class QCompleter;
class QStandardItemModel;

class MacroEditor : public CodeEditor {
    Q_OBJECT
public:
    explicit MacroEditor(QWidget *parent = nullptr);
    void Complete();
    void SetSpecialization(desktop::Specialization specialization);
protected:
    void keyPressEvent(QKeyEvent *event) override;
    void inputMethodEvent(QInputMethodEvent *event) override;
    bool event(QEvent *event) override;
private:
    desktop::Specialization m_specialization = desktop::Specialization::TaiXu;
    QCompleter *m_completer = nullptr;
    QStandardItemModel *m_candidates;
    int m_revision = -1, m_position = -1, m_begin = 0, m_end = 0;
    void UpdateCompletion(bool requested);
    void InsertCompletion(const QModelIndex &index);
};

#endif
