#ifndef DESKTOP_MACRO_SEARCH_BAR_H
#define DESKTOP_MACRO_SEARCH_BAR_H

#include <QTextCursor>
#include <QWidget>

class CodeEditor;
class QCheckBox;
class QLabel;
class QLineEdit;
class QPushButton;

class MacroSearchBar : public QWidget {
    Q_OBJECT
public:
    explicit MacroSearchBar(CodeEditor *editor, QWidget *parent = nullptr);
    void Open(bool replace = false);
    void Close();
    void Find(bool backwards = false);
protected:
    bool eventFilter(QObject *watched, QEvent *event) override;
private:
    CodeEditor *m_editor;
    QLineEdit *m_query, *m_replacement;
    QCheckBox *m_case, *m_words;
    QLabel *m_status;
    QWidget *m_replaceRow;
    QPushButton *m_replaceOne, *m_replaceAll;
    QList<QTextCursor> m_matches;
    bool m_replacing = false;
    void Refresh();
    void UpdateStatus(const QString &prefix = {});
    void UpdateReadOnly();
    void ReplaceOne();
    void ReplaceAll();
};

#endif
