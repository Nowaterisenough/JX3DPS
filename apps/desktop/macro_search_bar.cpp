#include "macro_search_bar.h"

#include <algorithm>
#include <QCheckBox>
#include <QEvent>
#include <QLabel>
#include <QLineEdit>
#include <QPalette>
#include <QPushButton>
#include <QShortcut>
#include <QVBoxLayout>
#include "controls/code_editor/code_editor.h"

MacroSearchBar::MacroSearchBar(CodeEditor *editor, QWidget *parent) : QWidget(parent), m_editor(editor)
{
    setObjectName(QStringLiteral("macroSearchBar"));
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(6, 4, 6, 4);
    layout->setSpacing(4);
    auto *row = new QHBoxLayout;
    auto field = [&](const char *name, const QString &placeholder) {
        auto *input = new QLineEdit(this);
        input->setObjectName(QString::fromLatin1(name));
        input->setPlaceholderText(placeholder);
        input->setClearButtonEnabled(true);
        auto colors = input->palette();
        colors.setColor(QPalette::PlaceholderText, QColor("#98a7b8"));
        input->setPalette(colors);
        return input;
    };
    m_query = field("macroSearchQuery", QStringLiteral("输入查找文本"));
    row->addWidget(m_query, 1);
    m_case = new QCheckBox(QStringLiteral("区分大小写"), this);
    m_case->setObjectName(QStringLiteral("macroSearchCase"));
    row->addWidget(m_case);
    m_words = new QCheckBox(QStringLiteral("完整名称"), this);
    m_words->setObjectName(QStringLiteral("macroSearchWords"));
    m_words->setToolTip(QStringLiteral("匹配完整名称；下划线和间隔号均视为名称的一部分。"));
    row->addWidget(m_words);
    auto button = [&](const QString &text, const char *name, auto callback) {
        auto *control = new QPushButton(text, this);
        control->setObjectName(QString::fromLatin1(name));
        connect(control, &QPushButton::clicked, this, callback);
        return control;
    };
    row->addWidget(button(QStringLiteral("上一处"), "macroSearchPrevious", [this] { Find(true); }));
    row->addWidget(button(QStringLiteral("下一处"), "macroSearchNext", [this] { Find(); }));
    row->addWidget(button(QStringLiteral("关闭"), "macroSearchClose", [this] { Close(); }));
    layout->addLayout(row);
    m_replaceRow = new QWidget(this);
    auto *replacementLayout = new QHBoxLayout(m_replaceRow);
    replacementLayout->setContentsMargins(0, 0, 0, 0);
    m_replacement = field("macroReplacement", QStringLiteral("替换为（留空即删除）"));
    replacementLayout->addWidget(m_replacement, 1);
    m_replaceOne = button(QStringLiteral("替换"), "macroReplaceOne", [this] { ReplaceOne(); });
    m_replaceAll = button(QStringLiteral("全部替换"), "macroReplaceAll", [this] { ReplaceAll(); });
    replacementLayout->addWidget(m_replaceOne);
    replacementLayout->addWidget(m_replaceAll);
    layout->addWidget(m_replaceRow);
    m_status = new QLabel(this);
    m_status->setObjectName(QStringLiteral("macroSearchStatus"));
    m_status->setTextFormat(Qt::PlainText);
    m_status->setWordWrap(true);
    layout->addWidget(m_status);
    connect(m_query, &QLineEdit::textChanged, this, [this] { Refresh(); });
    connect(m_case, &QCheckBox::toggled, this, [this] { Refresh(); });
    connect(m_words, &QCheckBox::toggled, this, [this] { Refresh(); });
    connect(m_query, &QLineEdit::returnPressed, this, [this] { Find(); });
    connect(m_replacement, &QLineEdit::returnPressed, this, [this] { ReplaceOne(); });
    connect(m_editor, &QPlainTextEdit::textChanged, this, [this] { if (!m_replacing) Refresh(); });
    connect(m_editor, &QPlainTextEdit::cursorPositionChanged, this, [this] { if (!m_replacing) UpdateStatus(); });
    m_editor->installEventFilter(this);
    auto *escape = new QShortcut(QKeySequence(Qt::Key_Escape), this);
    escape->setContext(Qt::WidgetWithChildrenShortcut);
    connect(escape, &QShortcut::activated, this, &MacroSearchBar::Close);
    hide();
}

void MacroSearchBar::Open(bool replace)
{
    const auto selected = m_editor->textCursor().selectedText();
    if (!selected.isEmpty() && !selected.contains(QChar::ParagraphSeparator) && !selected.contains(QChar::LineSeparator))
        m_query->setText(selected);
    m_replaceRow->setVisible(replace);
    show();
    Refresh();
    m_query->setFocus();
    m_query->selectAll();
}

void MacroSearchBar::Close()
{
    hide();
    m_matches.clear();
    m_editor->SetSearchMatches({});
    m_editor->setFocus();
}

void MacroSearchBar::Refresh()
{
    m_matches.clear();
    if (isHidden()) return;
    QTextDocument::FindFlags flags;
    if (m_case->isChecked()) flags |= QTextDocument::FindCaseSensitively;
    if (!m_query->text().isEmpty()) {
        QTextCursor cursor(m_editor->document());
        auto nameCharacter = [](QChar ch) { return ch.isLetterOrNumber() || ch.isMark() || ch == '_' || ch == QChar(0x00b7); };
        while (!(cursor = m_editor->document()->find(m_query->text(), cursor, flags)).isNull()) {
            // Qt's word search splits aliases at underscores. Macro names keep
            // underscores and the middle dot inside a single name.
            if (m_words->isChecked() &&
                ((cursor.selectionStart() > 0 && nameCharacter(m_editor->document()->characterAt(cursor.selectionStart() - 1))) ||
                 nameCharacter(m_editor->document()->characterAt(cursor.selectionEnd())))) continue;
            m_matches.append(cursor);
        }
    }
    m_editor->SetSearchMatches(m_matches.mid(0, 1000));
    UpdateStatus();
    UpdateReadOnly();
}

void MacroSearchBar::UpdateStatus(const QString &prefix)
{
    if (isHidden()) return;
    int current = 0;
    const auto cursor = m_editor->textCursor();
    for (int i = 0; i < m_matches.size(); ++i)
        if (cursor.selectionStart() == m_matches[i].selectionStart() && cursor.selectionEnd() == m_matches[i].selectionEnd()) {
            current = i + 1; break;
        }
    auto text = m_query->text().isEmpty() ? QStringLiteral("输入查找文本。回车 / F3：下一处；Shift+F3：上一处。")
        : current ? QStringLiteral("第 %1 / %2 处匹配").arg(current).arg(m_matches.size())
                  : QStringLiteral("%1 处匹配").arg(m_matches.size());
    if (m_matches.size() > 1000) text += QStringLiteral("（高亮前 1000 处）");
    if (m_editor->isReadOnly()) text += QStringLiteral(" | 调试期间替换已锁定。");
    m_status->setText(prefix.isEmpty() ? text : prefix + QStringLiteral(" | ") + text);
}

void MacroSearchBar::UpdateReadOnly()
{
    const bool enabled = !m_editor->isReadOnly();
    m_replacement->setEnabled(enabled);
    m_replaceOne->setEnabled(enabled && !m_matches.isEmpty());
    m_replaceAll->setEnabled(enabled && !m_matches.isEmpty());
    UpdateStatus();
}

bool MacroSearchBar::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == m_editor && event->type() == QEvent::ReadOnlyChange) UpdateReadOnly();
    return QWidget::eventFilter(watched, event);
}

void MacroSearchBar::Find(bool backwards)
{
    if (isHidden()) { Open(); return; }
    if (m_matches.isEmpty()) return;
    const auto cursor = m_editor->textCursor();
    int index = -1;
    if (backwards) {
        for (int i = static_cast<int>(m_matches.size()) - 1; i >= 0; --i)
            if (m_matches[i].selectionEnd() <= cursor.selectionStart()) { index = i; break; }
    } else {
        for (int i = 0; i < m_matches.size(); ++i)
            if (m_matches[i].selectionStart() >= cursor.selectionEnd()) { index = i; break; }
    }
    const bool wrapped = index < 0;
    if (wrapped) index = backwards ? static_cast<int>(m_matches.size()) - 1 : 0;
    m_editor->setTextCursor(m_matches[index]);
    m_editor->centerCursor();
    UpdateStatus(wrapped ? QStringLiteral("已循环查找") : QString{});
}

void MacroSearchBar::ReplaceOne()
{
    if (m_editor->isReadOnly() || isHidden() || m_matches.isEmpty()) return;
    auto cursor = m_editor->textCursor();
    const auto match = std::find_if(m_matches.cbegin(), m_matches.cend(), [&](const QTextCursor &candidate) {
        return candidate.selectionStart() == cursor.selectionStart() && candidate.selectionEnd() == cursor.selectionEnd();
    });
    if (match == m_matches.cend()) { Find(); return; }
    m_replacing = true;
    cursor.beginEditBlock();
    cursor.insertText(m_replacement->text());
    cursor.endEditBlock();
    m_editor->setTextCursor(cursor);
    m_replacing = false;
    Refresh();
    Find();
    UpdateStatus(QStringLiteral("已替换 1 处"));
}

void MacroSearchBar::ReplaceAll()
{
    if (m_editor->isReadOnly() || isHidden() || m_matches.isEmpty()) return;
    const auto matches = m_matches;
    const auto selection = m_editor->textCursor();
    m_replacing = true;
    // Edit backwards in a single undo block, preserving unaffected text blocks
    // and their breakpoints instead of replacing the entire document.
    QTextCursor cursor(m_editor->document());
    cursor.beginEditBlock();
    for (auto it = matches.crbegin(); it != matches.crend(); ++it) {
        cursor.setPosition(it->selectionStart());
        cursor.setPosition(it->selectionEnd(), QTextCursor::KeepAnchor);
        cursor.insertText(m_replacement->text());
    }
    cursor.endEditBlock();
    m_editor->setTextCursor(selection);
    m_replacing = false;
    Refresh();
    UpdateStatus(QStringLiteral("已替换 %1 处").arg(matches.size()));
}
