#include "macro_editor.h"

#include <QAbstractItemView>
#include <QCompleter>
#include <QInputMethodEvent>
#include <QKeyEvent>
#include <QLayout>
#include <QScrollBar>
#include <QStandardItemModel>
#include <QTextBlock>
#include "src/class/tai_xu_jian_yi/runtime_data.hpp"

namespace {
namespace tx = JX3DPS::runtime::tai_xu;
enum class Scope { None, Command, Condition, Skill, Buff };
struct Context {
    Scope scope = Scope::None;
    int begin = 0, end = 0;
    QString prefix;
};
const QStringList Commands{"/cast", "/fcast", "/scast", "/sfcast"};
const QStringList BuffConditions{"buff", "nobuff", "tbuff", "tnobuff", "bufftime", "tbufftime"};
const QStringList SkillConditions{"cd", "skill_cd", "skill_notin_cd", "skill_energy", "last_skill", "notlast_skill"};

Context CompletionContext(const QString &text, int cursor)
{
    auto range = [&](Scope scope, int begin, int end) {
        while (begin < end && text[begin].isSpace()) ++begin;
        while (end > begin && text[end-1].isSpace()) --end;
        if (cursor < begin || cursor > end) return Context{};
        return Context{scope, begin, end, text.mid(begin, cursor - begin)};
    };
    int start = 0;
    while (start < text.size() && text[start].isSpace()) ++start;
    if (cursor < start || text.mid(start).startsWith('#') || text.mid(start).startsWith("//")) return {};
    int commandEnd = start;
    while (commandEnd < text.size() && !text[commandEnd].isSpace()) ++commandEnd;
    if (cursor <= commandEnd) return range(Scope::Command, start, commandEnd);
    if (!Commands.contains(text.mid(start, commandEnd - start))) return {};
    int body = commandEnd;
    while (body < text.size() && text[body].isSpace()) ++body;
    if (body < text.size() && text[body] == '[') {
        const int close = text.indexOf(']', body);
        if (close < 0 || cursor <= close) {
            if (cursor <= body) return {};
            int begin = body + 1;
            for (int i = begin; i < cursor; ++i) if (text[i] == '&' || text[i] == '|') begin = i + 1;
            int end = close < 0 ? static_cast<int>(text.size()) : close;
            for (int i = cursor; i < end; ++i) if (text[i] == '&' || text[i] == '|') { end = i; break; }
            int comparison = end;
            for (int i = begin; i < end; ++i) if (QStringLiteral("<>=!").contains(text[i])) { comparison = i; break; }
            int colon = text.indexOf(':', begin);
            if (colon < 0 || colon >= comparison) colon = -1;
            if (colon >= 0 && cursor > colon) {
                const auto kind = text.mid(begin, colon - begin).trimmed();
                return range(BuffConditions.contains(kind) ? Scope::Buff : SkillConditions.contains(kind) ? Scope::Skill : Scope::None,
                             colon + 1, comparison);
            }
            auto context = range(Scope::Condition, begin, colon >= 0 ? colon : comparison);
            // Include an existing colon when completing the condition keyword.
            if (colon >= 0) context.end = colon + 1;
            return context;
        }
        body = close + 1;
    }
    return range(Scope::Skill, body, static_cast<int>(text.size()));
}

QString Name(std::string_view text) { return QString::fromUtf8(text.data(), static_cast<int>(text.size())); }
}

MacroEditor::MacroEditor(QWidget *parent) : CodeEditor(parent)
{
    m_completer = new QCompleter(this);
    m_candidates = new QStandardItemModel(this);
    SetSyntaxType(CodeEditor::JX3Macro);
    m_completer->setObjectName(QStringLiteral("macroCompleter"));
    m_completer->setWidget(this);
    m_completer->setModel(m_candidates);
    m_completer->setCompletionRole(Qt::UserRole);
    m_completer->setCompletionMode(QCompleter::UnfilteredPopupCompletion);
    m_completer->setMaxVisibleItems(8);
    m_completer->popup()->setFont(font());
    m_completer->popup()->setStyleSheet(QStringLiteral(
        "QAbstractItemView { background: #20252b; color: #d4d4d4; border: 1px solid #48515e; padding: 4px; }"
        "QAbstractItemView::item { padding: 4px 8px; }"
        "QAbstractItemView::item:selected { background: #264f78; color: white; }"));
    connect(m_completer, QOverload<const QModelIndex &>::of(&QCompleter::activated), this, &MacroEditor::InsertCompletion);
    connect(this, &QPlainTextEdit::cursorPositionChanged, this, [this] {
        if (textCursor().position() != m_position) m_completer->popup()->hide();
    });
    connect(this, &QPlainTextEdit::textChanged, this, [this] { m_completer->popup()->hide(); });
}

void MacroEditor::SetSpecialization(desktop::Specialization specialization)
{
    m_specialization = specialization;
    m_completer->popup()->hide();
    m_candidates->clear();
}

void MacroEditor::Complete()
{
    if (parentWidget() && parentWidget()->layout()) parentWidget()->layout()->activate();
    UpdateCompletion(true);
}

void MacroEditor::UpdateCompletion(bool requested)
{
    m_completer->popup()->hide();
    if (isReadOnly() || textCursor().hasSelection()) return;
    const auto cursor = textCursor();
    const auto context = CompletionContext(cursor.block().text(), cursor.positionInBlock());
    if (context.scope == Scope::None || (!requested && context.prefix.isEmpty())) return;
    m_candidates->clear();
    auto add = [&](const QString &insertion, const QString &description) {
        if (!insertion.startsWith(context.prefix, Qt::CaseInsensitive)) return;
        auto *item = new QStandardItem(insertion + QStringLiteral("    ") + description);
        item->setData(insertion, Qt::UserRole);
        item->setToolTip(description);
        m_candidates->appendRow(item);
    };
    const auto catalog = desktop::Describe(m_specialization);
    if (context.scope == Scope::Command) {
        add("/cast", "每轮尝试施放"); add("/fcast", "允许打断引导");
        add("/scast", "每场只执行一次；成功前阻止后续行");
        add("/sfcast", "每场只执行一次；允许打断引导");
    } else if (context.scope == Scope::Condition) {
        for (const auto &kind : BuffConditions)
            add(kind + ':', kind.endsWith("time") ? "剩余秒数，例如 bufftime:玄门>1.5"
                : kind.contains("nobuff") ? "增益不存在，不带比较符" : "增益层数；buff:玄门 等价于 buff:玄门>0");
        for (const auto &kind : SkillConditions)
            add(kind + ':', kind.endsWith("last_skill") ? "上一技能名称，不带比较符"
                : kind == "skill_energy" ? "技能充能，例如 skill_energy:紫气东来>0" : "剩余冷却秒数");
        for (const auto &kind : {"qidian", "energy", "rage"}) add(kind, "资源数量，需要比较符");
        for (const auto &kind : {"life", "mana", "tlife"}) add(kind, "0 至 1 的比例，需要比较符");
    } else if (context.scope == Scope::Skill) {
        for (auto skill : catalog.castable) {
            add(Name(catalog.skills[skill].name), Name(catalog.skillAliases[skill]));
            add(Name(catalog.skillAliases[skill]), Name(catalog.skills[skill].name));
        }
    } else {
        for (std::size_t buff = 0; buff < catalog.buffs.size(); ++buff) {
            add(Name(catalog.buffs[buff].name), Name(catalog.buffAliases[buff]));
            add(Name(catalog.buffAliases[buff]), Name(catalog.buffs[buff].name));
        }
    }
    if (!m_candidates->rowCount()) return;
    if (!requested && m_candidates->rowCount() == 1 && m_candidates->item(0)->data(Qt::UserRole).toString() == context.prefix) return;
    m_revision = document()->revision();
    m_position = cursor.position();
    m_begin = cursor.block().position() + context.begin;
    m_end = cursor.block().position() + context.end;
    m_completer->setCompletionPrefix({});
    auto rect = cursorRect();
    rect.translate(viewport()->pos());
    rect.setWidth(std::min(720, std::max(320, m_completer->popup()->sizeHintForColumn(0) + 24)));
    m_completer->complete(rect);
    m_completer->popup()->setCurrentIndex(m_completer->completionModel()->index(0, 0));
}

void MacroEditor::InsertCompletion(const QModelIndex &index)
{
    const auto insertion = index.data(Qt::UserRole).toString();
    if (isReadOnly() || insertion.isEmpty() || document()->revision() != m_revision || textCursor().position() != m_position) return;
    auto cursor = textCursor();
    cursor.beginEditBlock();
    cursor.setPosition(m_begin);
    cursor.setPosition(m_end, QTextCursor::KeepAnchor);
    cursor.insertText(insertion);
    cursor.endEditBlock();
    setTextCursor(cursor);
    m_completer->popup()->hide();
}

void MacroEditor::keyPressEvent(QKeyEvent *event)
{
    if (m_completer->popup()->isVisible()) {
        switch (event->key()) {
        case Qt::Key_Enter: case Qt::Key_Return: case Qt::Key_Tab:
            InsertCompletion(m_completer->popup()->currentIndex()); event->accept(); return;
        case Qt::Key_Escape:
            m_completer->popup()->hide(); event->accept(); return;
        case Qt::Key_Backtab: event->ignore(); return;
        default: break;
        }
    }
    if (event->key() == Qt::Key_Space && event->modifiers() == Qt::ControlModifier) {
        Complete(); event->accept(); return;
    }
    CodeEditor::keyPressEvent(event);
    if (!isReadOnly() && !(event->modifiers() & (Qt::ControlModifier | Qt::AltModifier | Qt::MetaModifier)) &&
        (!event->text().isEmpty() || event->key() == Qt::Key_Backspace)) UpdateCompletion(false);
}

void MacroEditor::inputMethodEvent(QInputMethodEvent *event)
{
    CodeEditor::inputMethodEvent(event);
    if (!event->commitString().isEmpty()) UpdateCompletion(false);
    else m_completer->popup()->hide();
}

bool MacroEditor::event(QEvent *event)
{
    if (m_completer && (event->type() == QEvent::ReadOnlyChange || event->type() == QEvent::Hide ||
                        event->type() == QEvent::Move || event->type() == QEvent::Resize))
        m_completer->popup()->hide();
    return CodeEditor::event(event);
}
