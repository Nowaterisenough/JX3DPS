#include "code_editor.h"

#include <functional>
#include <optional>

#include <QApplication>
#include <QClipboard>
#include <QHelpEvent>
#include <QKeyEvent>
#include <QMenu>
#include <QMouseEvent>
#include <QPainter>
#include <QPaintEvent>
#include <QRegularExpression>
#include <QStyleOptionMenuItem>
#include <QTextBlock>
#include <QTimer>
#include <QToolTip>

#include "resources.h"

// ============================================================================
// MenuStyle Implementation
// ============================================================================

namespace {
// 颜色常量
constexpr int BACKGROUND_COLOR_R = 37;
constexpr int BACKGROUND_COLOR_G = 37;
constexpr int BACKGROUND_COLOR_B = 38;

constexpr int BORDER_COLOR_R = 69;
constexpr int BORDER_COLOR_G = 69;
constexpr int BORDER_COLOR_B = 69;

constexpr int HOVER_COLOR_R = 0;
constexpr int HOVER_COLOR_G = 120;
constexpr int HOVER_COLOR_B = 212;

constexpr int DISABLED_TEXT_COLOR_R = 101;
constexpr int DISABLED_TEXT_COLOR_G = 101;
constexpr int DISABLED_TEXT_COLOR_B = 101;

constexpr int WHITE_COLOR_RGB = 255;

constexpr int NORMAL_TEXT_COLOR_RGB = 204;

constexpr int SHORTCUT_TEXT_COLOR_RGB = 136;

// 尺寸常量
constexpr int   MENU_PADDING               = 10;
constexpr int   HOVER_RECT_MARGIN          = 2;
constexpr int   HOVER_RECT_VERTICAL_MARGIN = 1;
constexpr int   SMALL_RADIUS               = 3;
constexpr int   LARGE_RADIUS               = 6;
constexpr int   SEPARATOR_MARGIN           = 4;
constexpr int   SEPARATOR_HEIGHT           = 9;
constexpr int   MENU_ITEM_HEIGHT           = 26;
constexpr qreal FONT_SIZE                  = 10.0;
constexpr int   MENU_V_MARGIN              = 4;
constexpr int   MENU_H_MARGIN              = 0;

// 编辑器颜色常量
constexpr int EDITOR_BACKGROUND_R = 35;
constexpr int EDITOR_BACKGROUND_G = 35;
constexpr int EDITOR_BACKGROUND_B = 39;

constexpr int EDITOR_TEXT_R = 232;
constexpr int EDITOR_TEXT_G = 237;
constexpr int EDITOR_TEXT_B = 247;

constexpr int EDITOR_HIGHLIGHT_R = 35;
constexpr int EDITOR_HIGHLIGHT_G = 64;
constexpr int EDITOR_HIGHLIGHT_B = 97;

constexpr int CURRENT_LINE_R = 42;
constexpr int CURRENT_LINE_G = 42;
constexpr int CURRENT_LINE_B = 48;

// 编辑器尺寸常量
constexpr int TAB_STOP                 = 4;
constexpr int DIGIT_BASE               = 10;
constexpr int BREAKPOINT_LEFT_MARGIN   = 6;
constexpr int BREAKPOINT_CIRCLE_SIZE   = 8;
constexpr int BREAKPOINT_SPACING       = 4;
constexpr int LINE_NUMBER_RIGHT_MARGIN = 6;

// 语法错误相关颜色
constexpr int ERROR_COLOR_R = 255;
constexpr int ERROR_COLOR_G = 73;
constexpr int ERROR_COLOR_B = 73;

constexpr int WARNING_COLOR_R = 255;
constexpr int WARNING_COLOR_G = 165;
constexpr int WARNING_COLOR_B = 0;

constexpr int DEBUG_BG_R     = 216;
constexpr int DEBUG_BG_G     = 185;
constexpr int DEBUG_BG_B     = 121;
constexpr int DEBUG_BG_ALPHA = 28;

// 其他常量
constexpr int ERROR_ICON_X_OFFSET = 20;
} // namespace

MenuStyle::MenuStyle(QStyle *style) : QProxyStyle(style) { }

void MenuStyle::drawControl(ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    if (element != CE_MenuItem) {
        QProxyStyle::drawControl(element, option, painter, widget);
        return;
    }

    const auto *menu_option = qstyleoption_cast<const QStyleOptionMenuItem *>(option);
    if (menu_option == nullptr) {
        QProxyStyle::drawControl(element, option, painter, widget);
        return;
    }

    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);

    // 绘制分隔符
    if (menu_option->menuItemType == QStyleOptionMenuItem::Separator) {
        painter->fillRect(option->rect, QColor(BACKGROUND_COLOR_R, BACKGROUND_COLOR_G, BACKGROUND_COLOR_B));
        const int Y = option->rect.center().y() + 1;
        painter->setPen(QColor(BORDER_COLOR_R, BORDER_COLOR_G, BORDER_COLOR_B));
        painter->drawLine(option->rect.left() + SEPARATOR_MARGIN, Y, option->rect.right() - SEPARATOR_MARGIN, Y);
        painter->restore();
        return;
    }

    // 绘制背景（圆角矩形）
    if ((menu_option->state & State_Selected) != 0) {
        painter->setPen(Qt::NoPen);
        painter->setBrush(QColor(HOVER_COLOR_R, HOVER_COLOR_G, HOVER_COLOR_B));
        const QRect hover_rect =
            option->rect.adjusted(HOVER_RECT_MARGIN, HOVER_RECT_VERTICAL_MARGIN, -HOVER_RECT_MARGIN, -HOVER_RECT_VERTICAL_MARGIN);
        painter->drawRoundedRect(hover_rect, SMALL_RADIUS, SMALL_RADIUS);
    } else {
        painter->fillRect(option->rect, QColor(BACKGROUND_COLOR_R, BACKGROUND_COLOR_G, BACKGROUND_COLOR_B));
    }

    // 设置字体
    QFont font = QApplication::font();
    font.setPointSizeF(FONT_SIZE);
    painter->setFont(font);

    // 分离菜单文本和快捷键
    const QString     full_text     = menu_option->text;
    const QStringList PARTS         = full_text.split('\t');
    const QString     menu_text     = PARTS.value(0);
    const QString     shortcut_text = PARTS.value(1);

    const QRect text_rect = option->rect.adjusted(MENU_PADDING, 0, -MENU_PADDING, 0);

    const bool IS_SELECTED = (menu_option->state & State_Selected) != 0;
    const bool IS_ENABLED  = (menu_option->state & State_Enabled) != 0;

    // 绘制菜单项文字
    if (!IS_ENABLED) {
        painter->setPen(QColor(DISABLED_TEXT_COLOR_R, DISABLED_TEXT_COLOR_G, DISABLED_TEXT_COLOR_B));
    } else if (IS_SELECTED) {
        painter->setPen(QColor(WHITE_COLOR_RGB, WHITE_COLOR_RGB, WHITE_COLOR_RGB)); // hover 时纯白色
    } else {
        painter->setPen(QColor(NORMAL_TEXT_COLOR_RGB, NORMAL_TEXT_COLOR_RGB, NORMAL_TEXT_COLOR_RGB));
    }
    painter->drawText(text_rect, Qt::AlignLeft | Qt::AlignVCenter, menu_text);

    // 绘制快捷键
    if (!shortcut_text.isEmpty()) {
        if (IS_SELECTED) {
            painter->setPen(QColor(WHITE_COLOR_RGB, WHITE_COLOR_RGB, WHITE_COLOR_RGB)); // hover 时纯白色
        } else {
            painter->setPen(QColor(SHORTCUT_TEXT_COLOR_RGB, SHORTCUT_TEXT_COLOR_RGB, SHORTCUT_TEXT_COLOR_RGB)); // 默认暗灰色
        }
        painter->drawText(text_rect, Qt::AlignRight | Qt::AlignVCenter, shortcut_text);
    }

    painter->restore();
}

void MenuStyle::drawPrimitive(PrimitiveElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    if (element == PE_PanelMenu) {
        painter->save();
        painter->setRenderHint(QPainter::Antialiasing);
        painter->setPen(QColor(BORDER_COLOR_R, BORDER_COLOR_G, BORDER_COLOR_B));
        painter->setBrush(QColor(BACKGROUND_COLOR_R, BACKGROUND_COLOR_G, BACKGROUND_COLOR_B));
        painter->drawRoundedRect(option->rect.adjusted(0, 0, -1, -1), LARGE_RADIUS, LARGE_RADIUS);
        painter->restore();
        return;
    }

    QProxyStyle::drawPrimitive(element, option, painter, widget);
}

QSize MenuStyle::sizeFromContents(ContentsType type, const QStyleOption *option, const QSize &size, const QWidget *widget) const
{
    QSize new_size = QProxyStyle::sizeFromContents(type, option, size, widget);

    if (type == CT_MenuItem) {
        const auto *menu_option = qstyleoption_cast<const QStyleOptionMenuItem *>(option);
        if (menu_option != nullptr && menu_option->menuItemType == QStyleOptionMenuItem::Separator) {
            return { new_size.width(), SEPARATOR_HEIGHT };
        }
        return QSize(new_size.width(), MENU_ITEM_HEIGHT);
    }

    return new_size;
}

int MenuStyle::pixelMetric(PixelMetric metric, const QStyleOption *option, const QWidget *widget) const
{
    if (metric == PM_MenuVMargin) {
        return MENU_V_MARGIN;
    }
    if (metric == PM_MenuHMargin) {
        return MENU_H_MARGIN;
    }
    return QProxyStyle::pixelMetric(metric, option, widget);
}

// ============================================================================
// CodeEditorPrivate
// ============================================================================

class CodeEditorPrivate
{
public:
    explicit CodeEditorPrivate(CodeEditor *editor) :
        lineNumberArea(new LineNumberArea(editor)), currentLineColor(QColor(Qt::yellow).lighter(CURRENT_LINE_LIGHTER_VALUE)),
        cppHighlighter(nullptr), jx3MacroHighlighter(nullptr), syntaxCheckTimer(new QTimer(editor)), q_ptr(editor)
    {
        // 配置语法检测定时器：延迟500ms执行，避免频繁检测
        syntaxCheckTimer->setSingleShot(true);
        syntaxCheckTimer->setInterval(SYNTAX_CHECK_DELAY_MS);
    }

    LineNumberArea                           *lineNumberArea;
    int                                       tabWidth{ 4 };
    bool                                      showLineNumbers{ true };
    QColor                                    currentLineColor;
    bool                                      syntaxHighlightingEnabled{ true };
    CodeEditor::SyntaxType                    syntaxType{ CodeEditor::CPP };
    QScopedPointer<CppSyntaxHighlighter>      cppHighlighter;
    QScopedPointer<JX3MacroSyntaxHighlighter> jx3MacroHighlighter;
    QSet<int>                                 breakpoints; // 断点集合（行号从1开始）
    QVector<QTextBlock>                       breakpointBlocks;
    int                                       currentDebugLine{ -1 }; // 当前调试行（-1表示无）

    // 语法检测
    bool                                               syntaxCheckEnabled{ true };     // 是否启用语法检测
    std::optional<JX3MacroSyntaxChecker>               syntaxChecker{ std::in_place }; // 语法检测器
    QList<SyntaxError>                                 syntaxErrors;                   // 当前语法错误列表
    QList<QTextCursor>                                 searchMatches;
    std::function<QList<SyntaxError>(const QString &)> syntaxValidator;
    QTimer                                            *syntaxCheckTimer; // 延迟检测定时器

private:
    CodeEditor *q_ptr;
    Q_DECLARE_PUBLIC(CodeEditor)

    // 常量定义
    static constexpr int CURRENT_LINE_LIGHTER_VALUE = 160;
    static constexpr int SYNTAX_CHECK_DELAY_MS      = 500;
};

// ============================================================================
// CodeEditor Implementation
// ============================================================================

CodeEditor::CodeEditor(QWidget *parent) : QPlainTextEdit(parent), d_ptr(new CodeEditorPrivate(this))
{
    Q_D(CodeEditor);

    // 连接信号
    connect(this, &CodeEditor::blockCountChanged, this, &CodeEditor::UpdateLineNumberAreaWidth);
    connect(this, &CodeEditor::updateRequest, this, &CodeEditor::UpdateLineNumberArea);
    connect(this, &CodeEditor::cursorPositionChanged, this, &CodeEditor::HighlightCurrentLine);
    connect(document(), &QTextDocument::contentsChange, this, &CodeEditor::SyncBreakpoints);

    // 连接语法检测信号
    connect(this, &CodeEditor::textChanged, d->syntaxCheckTimer, qOverload<>(&QTimer::start));
    connect(d->syntaxCheckTimer, &QTimer::timeout, this, &CodeEditor::CheckSyntax);

    UpdateLineNumberAreaWidth(0);
    HighlightCurrentLine();

    // 设置等宽字体：英文 Monaco，中文霞鹜文楷等宽
    setFont(Resources::MonoFont());

    // 设置文档默认字体，确保中文字符能正确回退
    document()->setDefaultFont(Resources::MonoFont());

    QPalette palette = this->palette();
    palette.setColor(QPalette::Base, QColor(EDITOR_BACKGROUND_R, EDITOR_BACKGROUND_G, EDITOR_BACKGROUND_B));
    palette.setColor(QPalette::Text, QColor(EDITOR_TEXT_R, EDITOR_TEXT_G, EDITOR_TEXT_B));
    palette.setColor(QPalette::Highlight, QColor(EDITOR_HIGHLIGHT_R, EDITOR_HIGHLIGHT_G, EDITOR_HIGHLIGHT_B));
    palette.setColor(QPalette::HighlightedText, QColor(WHITE_COLOR_RGB, WHITE_COLOR_RGB, WHITE_COLOR_RGB)); // 选中文本
    setPalette(palette);

    // 设置制表符宽度
    const QFontMetrics METRICS(Resources::MonoFont());
    setTabStopDistance(TAB_STOP * METRICS.horizontalAdvance(' '));

    d->currentLineColor = QColor(CURRENT_LINE_R, CURRENT_LINE_G, CURRENT_LINE_B);

    // 默认启用 C++ 语法高亮
    d->cppHighlighter.reset(new CppSyntaxHighlighter(document()));
}

CodeEditor::~CodeEditor()
{
    // Destroying the highlighter can notify the document after the private
    // breakpoint storage has started to be destroyed.
    disconnect(document(), nullptr, this, nullptr);
}

int CodeEditor::LineNumberAreaWidth()
{
    Q_D(CodeEditor);
    if (!d->showLineNumbers) {
        return 0;
    }

    int digits = 1;
    int max    = qMax(1, blockCount());
    while (max >= DIGIT_BASE) {
        max /= DIGIT_BASE;
        ++digits;
    }

    // 为断点区域预留空间：左边距(6) + 圆圈(8) + 间距(4) + 行号 + 右边距(6)
    constexpr int BREAKPOINT_AREA_WIDTH = BREAKPOINT_LEFT_MARGIN + BREAKPOINT_CIRCLE_SIZE + BREAKPOINT_SPACING;
    const int SPACE = BREAKPOINT_AREA_WIDTH + (fontMetrics().horizontalAdvance(QLatin1Char('9')) * digits) + LINE_NUMBER_RIGHT_MARGIN;
    return SPACE;
}

void CodeEditor::UpdateLineNumberAreaWidth(int /* newBlockCount */)
{
    setViewportMargins(LineNumberAreaWidth(), 0, 0, 0);
}

void CodeEditor::UpdateLineNumberArea(const QRect &rect, int dy)
{
    Q_D(CodeEditor);
    if (dy != 0) {
        d->lineNumberArea->scroll(0, dy);
    } else {
        d->lineNumberArea->update(0, rect.y(), d->lineNumberArea->width(), rect.height());
    }

    if (rect.contains(viewport()->rect())) {
        UpdateLineNumberAreaWidth(0);
    }
}

void CodeEditor::resizeEvent(QResizeEvent *event)
{
    Q_D(CodeEditor);
    QPlainTextEdit::resizeEvent(event);

    const QRect CR = contentsRect();
    d->lineNumberArea->setGeometry(QRect(CR.left(), CR.top(), LineNumberAreaWidth(), CR.height()));
}

void CodeEditor::paintEvent(QPaintEvent *event)
{
    Q_D(CodeEditor);

    // 先调用基类绘制
    QPlainTextEdit::paintEvent(event);

    // 如果没有语法错误，直接返回
    if (!d->syntaxCheckEnabled || d->syntaxErrors.isEmpty()) {
        return;
    }

    // 按行号分组错误信息（仅显示 Error 级别）
    QMap<int, QStringList> errors_by_line;
    for (const SyntaxError &error : d->syntaxErrors) {
        if (error.severity == SyntaxError::SYNTAX_ERROR) {
            errors_by_line[error.line].append(error.message);
        }
    }

    if (errors_by_line.isEmpty()) {
        return;
    }

    // 绘制行尾错误信息
    QPainter painter(viewport());
    painter.setRenderHint(QPainter::Antialiasing);

    QFont error_font = font();
    error_font.setItalic(true);
    painter.setFont(error_font);
    painter.setPen(QColor(ERROR_COLOR_R, ERROR_COLOR_G, ERROR_COLOR_B)); // 红色

    for (auto it = errors_by_line.constBegin(); it != errors_by_line.constEnd(); ++it) {
        const int          LINE_NUMBER = it.key();
        const QStringList &messages    = it.value();

        const QTextBlock BLOCK = document()->findBlockByLineNumber(LINE_NUMBER - 1);
        if (!BLOCK.isValid()) {
            continue;
        }

        // 计算行的绘制位置
        const QRectF block_rect = blockBoundingGeometry(BLOCK).translated(contentOffset());
        const int    TOP        = qRound(block_rect.top());
        const int    height     = qRound(block_rect.height());

        // 计算行尾位置
        const QString      line_text = BLOCK.text();
        const QFontMetrics FM(font());
        const int          TEXT_WIDTH = FM.horizontalAdvance(line_text);
        const int          X_POS      = TEXT_WIDTH + ERROR_ICON_X_OFFSET; // 距离行尾像素

        // 绘制错误信息
        const QString error_text = "  // Error: " + messages.join("; ");
        painter.drawText(X_POS, TOP, viewport()->width() - X_POS, height, Qt::AlignLeft | Qt::AlignVCenter, error_text);
    }
}

void CodeEditor::HighlightCurrentLine()
{
    Q_D(CodeEditor);
    QList<QTextEdit::ExtraSelection> extra_selections;

    // 1. 绘制语法错误下划线（最底层）
    if (d->syntaxCheckEnabled && !d->syntaxErrors.isEmpty()) {
        for (const SyntaxError &error : d->syntaxErrors) {
            const QTextBlock BLOCK = document()->findBlockByLineNumber(error.line - 1);
            if (!BLOCK.isValid()) {
                continue;
            }

            QTextEdit::ExtraSelection error_selection;
            QTextCharFormat           format;

            if (error.severity == SyntaxError::SYNTAX_ERROR) {
                format.setUnderlineColor(QColor(ERROR_COLOR_R, ERROR_COLOR_G, ERROR_COLOR_B)); // VSCode 红色 #ff4949
                format.setUnderlineStyle(QTextCharFormat::SpellCheckUnderline);                // 更粗的波浪线
            } else {
                format.setUnderlineColor(QColor(WARNING_COLOR_R, WARNING_COLOR_G, WARNING_COLOR_B)); // 橙色（警告）
                format.setUnderlineStyle(QTextCharFormat::SpellCheckUnderline);
            }

            error_selection.format = format;
            error_selection.cursor = QTextCursor(BLOCK);
            error_selection.cursor.setPosition(BLOCK.position() + error.column);
            error_selection.cursor.setPosition(BLOCK.position() + error.column + error.length, QTextCursor::KeepAnchor);
            extra_selections.append(error_selection);
        }
    }

    // 2. 绘制调试行高亮（黄色背景）
    if (d->currentDebugLine > 0) {
        const QTextBlock DEBUG_BLOCK = document()->findBlockByLineNumber(d->currentDebugLine - 1);
        if (DEBUG_BLOCK.isValid()) {
            QTextEdit::ExtraSelection debug_selection;
            debug_selection.format.setBackground(QColor(DEBUG_BG_R, DEBUG_BG_G, DEBUG_BG_B, DEBUG_BG_ALPHA));
            debug_selection.format.setProperty(QTextFormat::FullWidthSelection, true);
            debug_selection.cursor = QTextCursor(DEBUG_BLOCK);
            debug_selection.cursor.clearSelection();
            extra_selections.append(debug_selection);
        }
    }

    // 3. 绘制当前行高亮（深灰色背景）
    if (!isReadOnly()) {
        QTextEdit::ExtraSelection selection;
        selection.format.setBackground(d->currentLineColor);
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = textCursor();
        selection.cursor.clearSelection();
        extra_selections.append(selection);
    }

    for (const auto &match : d->searchMatches) {
        QTextEdit::ExtraSelection selection;
        selection.cursor = match;
        selection.format.setBackground(QColor(103, 82, 29));
        extra_selections.append(selection);
    }
    setExtraSelections(extra_selections);
}

void CodeEditor::SetSearchMatches(const QList<QTextCursor> &matches)
{
    Q_D(CodeEditor);
    d->searchMatches = matches;
    HighlightCurrentLine();
}

void CodeEditor::LineNumberAreaPaintEvent(QPaintEvent *event)
{
    Q_D(CodeEditor);
    QPainter painter(d->lineNumberArea);
    painter.setRenderHint(QPainter::Antialiasing, true);

    painter.fillRect(event->rect(), palette().color(QPalette::Base));

    QTextBlock block        = firstVisibleBlock();
    int        block_number = block.blockNumber();
    int        top          = qRound(blockBoundingGeometry(block).translated(contentOffset()).top());
    int        bottom       = top + qRound(blockBoundingRect(block).height());

    // 当前行号
    const int CURRENT_BLOCK_NUMBER = textCursor().blockNumber();

    // 断点圆圈参数
    constexpr int BREAKPOINT_SIZE   = 8; // 断点圆圈直径（更小更精致）
    constexpr int BREAKPOINT_MARGIN = 6; // 断点左边距

    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            const int     LINE_NUMBER = block_number + 1; // 行号从1开始
            const QString NUMBER      = QString::number(LINE_NUMBER);

            // 绘制断点（如果有）- VSCode 风格的红色圆圈
            if (d->breakpoints.contains(LINE_NUMBER)) {
                painter.setPen(Qt::NoPen);
                painter.setBrush(QColor(224, 78, 78)); // VSCode 断点红色 #e04e4e

                // 在行号左侧绘制圆形断点，稍微向下偏移以对齐行号中心
                const int CENTER_Y = top + (fontMetrics().height() / 2) + 2;
                painter.drawEllipse(QPoint(BREAKPOINT_MARGIN + (BREAKPOINT_SIZE / 2), CENTER_Y), BREAKPOINT_SIZE / 2, BREAKPOINT_SIZE / 2);
            }

            // VSCode 风格：当前行行号高亮，其他行号灰色
            if (block_number == CURRENT_BLOCK_NUMBER) {
                painter.setPen(QColor(212, 212, 212)); // 当前行 #d4d4d4
            } else {
                painter.setPen(QColor(129, 143, 164));
            }

            // 绘制调试行高亮（如果是当前调试行）- VSCode 风格：黄色背景
            const int TEXT_X = BREAKPOINT_MARGIN + BREAKPOINT_SIZE + 4; // 断点后留4px间距

            if (d->currentDebugLine == LINE_NUMBER) {
                // 1. 绘制黄色背景高亮条（覆盖整个行号区域）
                painter.setPen(Qt::NoPen);
                painter.setBrush(QColor(216, 185, 121));
                painter.drawRect(0, top, d->lineNumberArea->width(), fontMetrics().height());

                // 2. 重新绘制行号（黑色，覆盖在黄色背景上）
                painter.setPen(QColor(0, 0, 0)); // 黑色行号
                painter.drawText(TEXT_X, top, d->lineNumberArea->width() - TEXT_X - 6, fontMetrics().height(), Qt::AlignRight, NUMBER);

                // 3. 如果有断点，重新绘制断点（黑色，覆盖在黄色背景上）
                if (d->breakpoints.contains(LINE_NUMBER)) {
                    painter.setPen(Qt::NoPen);
                    painter.setBrush(QColor(0, 0, 0)); // 黑色断点
                    const int CENTER_Y = top + (fontMetrics().height() / 2) + 2;
                    painter.drawEllipse(QPoint(BREAKPOINT_MARGIN + (BREAKPOINT_SIZE / 2), CENTER_Y), BREAKPOINT_SIZE / 2, BREAKPOINT_SIZE / 2);
                }
            } else {
                // 正常绘制行号（无调试高亮时）
                painter.drawText(TEXT_X, top, d->lineNumberArea->width() - TEXT_X - 6, fontMetrics().height(), Qt::AlignRight, NUMBER);
            }
        }

        block  = block.next();
        top    = bottom;
        bottom = top + qRound(blockBoundingRect(block).height());
        ++block_number;
    }
}

void CodeEditor::keyPressEvent(QKeyEvent *event)
{
    if (isReadOnly()) {
        QPlainTextEdit::keyPressEvent(event);
        return;
    }
    Q_D(CodeEditor);

    // Tab 键转换为空格
    if (event->key() == Qt::Key_Tab) {
        QTextCursor cursor = textCursor();
        cursor.insertText(QString(d->tabWidth, ' '));
        return;
    }

    // 回车时自动缩进
    if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
        QTextCursor      cursor = textCursor();
        const QTextBlock BLOCK  = cursor.block();
        const QString    text   = BLOCK.text();

        // 计算当前行的缩进
        int indent = 0;
        for (const QChar CH : text) {
            if (CH == ' ') {
                indent++;
            } else if (CH == '\t') {
                indent += d->tabWidth;
            } else {
                break;
            }
        }

        // 如果当前行以 { 结尾，增加一级缩进
        const QString TRIMMED = text.trimmed();
        if (TRIMMED.endsWith('{')) {
            indent += d->tabWidth;
        }

        QPlainTextEdit::keyPressEvent(event);
        cursor = textCursor();
        cursor.insertText(QString(indent, ' '));
        return;
    }

    QPlainTextEdit::keyPressEvent(event);
}

bool CodeEditor::event(QEvent *event)
{
    Q_D(CodeEditor);

    // 处理工具提示事件（鼠标悬停）
    if (event->type() == QEvent::ToolTip) {
        const auto       *help_event = static_cast<QHelpEvent *>(event);
        const QTextCursor CURSOR     = cursorForPosition(help_event->pos());
        const int         LINE       = CURSOR.blockNumber() + 1;
        const int         COLUMN     = CURSOR.positionInBlock();

        // 查找当前位置的错误
        QString tooltip_text;
        for (const SyntaxError &error : d->syntaxErrors) {
            if (error.line == LINE && COLUMN >= error.column && COLUMN < error.column + error.length) {
                if (!tooltip_text.isEmpty()) {
                    tooltip_text += "\n";
                }
                tooltip_text += error.message;
            }
        }

        if (!tooltip_text.isEmpty()) {
            QToolTip::showText(help_event->globalPos(), tooltip_text, this);
        } else {
            QToolTip::hideText();
        }
        return true;
    }

    return QPlainTextEdit::event(event);
}

void CodeEditor::contextMenuEvent(QContextMenuEvent *event)
{
    auto *menu = new QMenu(this);

    // 使用自定义样式代理（样式对象将由 QMenu 管理生命周期）
    auto *style = new MenuStyle();
    menu->setStyle(style);

    // 设置菜单属性
    menu->setAttribute(Qt::WA_DeleteOnClose);
    menu->setWindowFlags(menu->windowFlags() | Qt::NoDropShadowWindowHint);

    const QTextCursor CURSOR        = textCursor();
    const bool        HAS_SELECTION = CURSOR.hasSelection();
    const bool        has_text      = !document()->isEmpty();
    const QClipboard *clipboard     = QApplication::clipboard();
    const bool        HAS_CLIPBOARD = !clipboard->text().isEmpty();

    // 剪切
    QAction *cut_action = menu->addAction("剪切\tCtrl+X");
    cut_action->setShortcut(QKeySequence::Cut);
    cut_action->setEnabled(HAS_SELECTION && !isReadOnly());
    connect(cut_action, &QAction::triggered, this, &CodeEditor::cut);

    // 复制
    QAction *copy_action = menu->addAction("复制\tCtrl+C");
    copy_action->setShortcut(QKeySequence::Copy);
    copy_action->setEnabled(HAS_SELECTION);
    connect(copy_action, &QAction::triggered, this, &CodeEditor::copy);

    // 粘贴
    QAction *paste_action = menu->addAction("粘贴\tCtrl+V");
    paste_action->setShortcut(QKeySequence::Paste);
    paste_action->setEnabled(HAS_CLIPBOARD && !isReadOnly());
    connect(paste_action, &QAction::triggered, this, &CodeEditor::paste);

    menu->addSeparator();

    // 全选
    QAction *select_all_action = menu->addAction("全选\tCtrl+A");
    select_all_action->setShortcut(QKeySequence::SelectAll);
    select_all_action->setEnabled(has_text);
    connect(select_all_action, &QAction::triggered, this, &CodeEditor::selectAll);

    // 格式化
    QAction *format_action = menu->addAction("格式化\tShift+Alt+F");
    format_action->setShortcut(QKeySequence("Shift+Alt+F"));
    format_action->setEnabled(has_text && !isReadOnly());
    connect(format_action, &QAction::triggered, this, &CodeEditor::FormatDocument);

    menu->addSeparator();

    // 撤销
    QAction *undo_action = menu->addAction("撤销\tCtrl+Z");
    undo_action->setShortcut(QKeySequence::Undo);
    undo_action->setEnabled(document()->isUndoAvailable() && !isReadOnly());
    connect(undo_action, &QAction::triggered, this, &CodeEditor::undo);

    // 重做
    QAction *redo_action = menu->addAction("重做\tCtrl+Y");
    redo_action->setShortcut(QKeySequence::Redo);
    redo_action->setEnabled(document()->isRedoAvailable() && !isReadOnly());
    connect(redo_action, &QAction::triggered, this, &CodeEditor::redo);

    menu->exec(event->globalPos());
    menu->deleteLater();
}

void CodeEditor::SetTabWidth(int spaces)
{
    Q_D(CodeEditor);
    d->tabWidth = spaces;
    const QFontMetrics METRICS(font());
    setTabStopDistance(spaces * METRICS.horizontalAdvance(' '));
}

int CodeEditor::TabWidth() const
{
    Q_D(const CodeEditor);
    return d->tabWidth;
}

void CodeEditor::SetShowLineNumbers(bool show)
{
    Q_D(CodeEditor);
    d->showLineNumbers = show;
    d->lineNumberArea->setVisible(show);
    UpdateLineNumberAreaWidth(0);
}

bool CodeEditor::ShowLineNumbers() const
{
    Q_D(const CodeEditor);
    return d->showLineNumbers;
}

void CodeEditor::SetCurrentLineColor(const QColor &color)
{
    Q_D(CodeEditor);
    d->currentLineColor = color;
    HighlightCurrentLine();
}

QColor CodeEditor::CurrentLineColor() const
{
    Q_D(const CodeEditor);
    return d->currentLineColor;
}

void CodeEditor::SetSyntaxHighlighting(bool enable)
{
    Q_D(CodeEditor);
    d->syntaxHighlightingEnabled = enable;

    // 删除现有高亮器 (QScopedPointer自动管理)
    d->cppHighlighter.reset();
    d->jx3MacroHighlighter.reset();

    // 如果启用，根据类型创建高亮器
    if (enable) {
        switch (d->syntaxType) {
            case CPP: d->cppHighlighter.reset(new CppSyntaxHighlighter(document())); break;
            case J_X3_MACRO: d->jx3MacroHighlighter.reset(new JX3MacroSyntaxHighlighter(document())); break;
        }
    }
}

bool CodeEditor::SyntaxHighlighting() const
{
    Q_D(const CodeEditor);
    return d->syntaxHighlightingEnabled;
}

void CodeEditor::SetSyntaxType(SyntaxType type)
{
    Q_D(CodeEditor);
    if (d->syntaxType == type) {
        return;
    }

    d->syntaxType = type;

    // 删除旧的高亮器 (QScopedPointer自动管理)
    d->cppHighlighter.reset();
    d->jx3MacroHighlighter.reset();

    // 创建新的高亮器
    if (d->syntaxHighlightingEnabled) {
        switch (type) {
            case CPP: d->cppHighlighter.reset(new CppSyntaxHighlighter(document())); break;
            case J_X3_MACRO: d->jx3MacroHighlighter.reset(new JX3MacroSyntaxHighlighter(document())); break;
        }
    }

    // 切换语法检测器
    switch (type) {
        case J_X3_MACRO: d->syntaxChecker.emplace(); break;
        case CPP:
            // C++ 暂时没有检测器
            d->syntaxChecker.reset();
            break;
    }

    // 重新执行语法检测
    CheckSyntax();
}

CodeEditor::SyntaxType CodeEditor::GetSyntaxType() const
{
    Q_D(const CodeEditor);
    return d->syntaxType;
}

// ============================================================================
// CppSyntaxHighlighter Implementation
// ============================================================================

CppSyntaxHighlighter::CppSyntaxHighlighter(QTextDocument *parent) : QSyntaxHighlighter(parent)
{
    HighlightingRule rule;

    // VSCode C++ 主题配色方案
    // 参考：https://code.visualstudio.com/api/references/theme-color

    // 关键字格式 - 蓝紫色 #569cd6 (控制流关键字)
    keywordFormat.setForeground(QColor(86, 156, 214));
    const QStringList CONTROL_KEYWORDS = {
        "\\bif\\b",     "\\belse\\b", "\\bfor\\b",     "\\bwhile\\b", "\\bdo\\b",
        "\\bswitch\\b", "\\bcase\\b", "\\bdefault\\b", "\\bbreak\\b", "\\bcontinue\\b",
        "\\breturn\\b", "\\btry\\b",  "\\bcatch\\b",   "\\bthrow\\b", "\\bgoto\\b"
    };
    for (const QString &pattern : CONTROL_KEYWORDS) {
        rule.pattern = QRegularExpression(pattern);
        rule.format  = keywordFormat;
        highlightingRules.append(rule);
    }

    // 类型关键字 - 蓝紫色 #569cd6
    const QStringList TYPE_KEYWORDS = {
        "\\bchar\\b",     "\\bint\\b",     "\\bshort\\b",     "\\blong\\b",     "\\bfloat\\b",
        "\\bdouble\\b",   "\\bbool\\b",    "\\bvoid\\b",      "\\bsigned\\b",   "\\bunsigned\\b",
        "\\bauto\\b",     "\\bconst\\b",   "\\bvolatile\\b",  "\\bextern\\b",   "\\bstatic\\b",
        "\\bregister\\b", "\\bmutable\\b", "\\bconstexpr\\b", "\\bdecltype\\b", "\\btypename\\b"
    };
    for (const QString &pattern : TYPE_KEYWORDS) {
        rule.pattern = QRegularExpression(pattern);
        rule.format  = keywordFormat;
        highlightingRules.append(rule);
    }

    // 修饰符关键字 - 蓝紫色 #569cd6
    const QStringList MODIFIER_KEYWORDS = {
        "\\bclass\\b",        "\\bstruct\\b",           "\\bunion\\b",     "\\benum\\b",     "\\bnamespace\\b",
        "\\btemplate\\b",     "\\btypedef\\b",          "\\busing\\b",     "\\bpublic\\b",   "\\bprivate\\b",
        "\\bprotected\\b",    "\\bvirtual\\b",          "\\boverride\\b",  "\\bfinal\\b",    "\\bexplicit\\b",
        "\\bfriend\\b",       "\\binline\\b",           "\\boperator\\b",  "\\bnoexcept\\b", "\\bstatic_cast\\b",
        "\\bdynamic_cast\\b", "\\breinterpret_cast\\b", "\\bconst_cast\\b"
    };
    for (const QString &pattern : MODIFIER_KEYWORDS) {
        rule.pattern = QRegularExpression(pattern);
        rule.format  = keywordFormat;
        highlightingRules.append(rule);
    }

    // new, delete, this, nullptr - 蓝紫色
    const QStringList SPECIAL_KEYWORDS = { "\\bnew\\b",    "\\bdelete\\b", "\\bthis\\b",    "\\bnullptr\\b",
                                           "\\bsizeof\\b", "\\btypeid\\b", "\\bsignals\\b", "\\bslots\\b" };
    for (const QString &pattern : SPECIAL_KEYWORDS) {
        rule.pattern = QRegularExpression(pattern);
        rule.format  = keywordFormat;
        highlightingRules.append(rule);
    }

    // 类名/类型名格式 - 青绿色 #4ec9b0
    classFormat.setForeground(QColor(78, 201, 176));
    rule.pattern = QRegularExpression("\\b[A-Z][a-zA-Z0-9_]*\\b");
    rule.format  = classFormat;
    highlightingRules.append(rule);

    // 预处理器格式 - 紫色 #c586c0
    preprocessorFormat.setForeground(QColor(197, 134, 192));
    rule.pattern = QRegularExpression("^\\s*#.*");
    rule.format  = preprocessorFormat;
    highlightingRules.append(rule);

    // 字符串格式 - 橙褐色 #ce9178
    quotationFormat.setForeground(QColor(206, 145, 120));
    rule.pattern = QRegularExpression("\".*?\"|'.*?'");
    rule.format  = quotationFormat;
    highlightingRules.append(rule);

    // 函数名格式 - 黄色 #dcdcaa
    functionFormat.setForeground(QColor(220, 220, 170));
    rule.pattern = QRegularExpression("\\b[A-Za-z0-9_]+(?=\\s*\\()");
    rule.format  = functionFormat;
    highlightingRules.append(rule);

    // 数字格式 - 浅绿色 #b5cea8
    numberFormat.setForeground(QColor(181, 206, 168));
    rule.pattern = QRegularExpression("\\b[0-9]+\\.?[0-9]*[fFlLuU]*\\b|\\b0x[0-9a-fA-F]+\\b");
    rule.format  = numberFormat;
    highlightingRules.append(rule);

    // 单行注释格式 - 绿色 #6a9955
    singleLineCommentFormat.setForeground(QColor(106, 153, 85));
    singleLineCommentFormat.setFontItalic(true);
    rule.pattern = QRegularExpression("//[^\n]*");
    rule.format  = singleLineCommentFormat;
    highlightingRules.append(rule);

    // 多行注释格式 - 绿色 #6a9955
    multiLineCommentFormat.setForeground(QColor(106, 153, 85));
    multiLineCommentFormat.setFontItalic(true);

    // 多行注释的开始和结束表达式
    commentStartExpression = QRegularExpression("/\\*");
    commentEndExpression   = QRegularExpression("\\*/");
}

void CppSyntaxHighlighter::highlightBlock(const QString &text)
{
    // 应用所有规则
    for (const HighlightingRule &rule : highlightingRules) {
        QRegularExpressionMatchIterator match_iterator = rule.pattern.globalMatch(text);
        while (match_iterator.hasNext()) {
            const QRegularExpressionMatch MATCH = match_iterator.next();
            setFormat(MATCH.capturedStart(), MATCH.capturedLength(), rule.format);
        }
    }

    // 处理多行注释
    setCurrentBlockState(0);

    int start_index = 0;
    if (previousBlockState() != 1) {
        start_index = text.indexOf(commentStartExpression);
    }

    while (start_index >= 0) {
        const QRegularExpressionMatch MATCH          = commentEndExpression.match(text, start_index);
        const int                     END_INDEX      = MATCH.capturedStart();
        int                           comment_length = 0;
        if (END_INDEX == -1) {
            setCurrentBlockState(1);
            comment_length = text.length() - start_index;
        } else {
            comment_length = END_INDEX - start_index + MATCH.capturedLength();
        }
        setFormat(start_index, comment_length, multiLineCommentFormat);
        start_index = text.indexOf(commentStartExpression, start_index + comment_length);
    }
}

// ============================================================================
// Breakpoint Management
// ============================================================================

void CodeEditor::ToggleBreakpoint(int line_number)
{
    Q_D(CodeEditor);
    if (d->breakpoints.contains(line_number)) {
        RemoveBreakpoint(line_number);
    } else {
        SetBreakpoint(line_number, true);
    }
}

void CodeEditor::SetBreakpoint(int line_number, bool enabled)
{
    Q_D(CodeEditor);
    if (!enabled) {
        RemoveBreakpoint(line_number);
        return;
    }
    const auto BLOCK = document()->findBlockByNumber(line_number - 1);
    if (!BLOCK.isValid()) {
        return;
    }
    if (enabled && !d->breakpoints.contains(line_number)) {
        d->breakpoints.insert(line_number);
        d->breakpointBlocks.append(BLOCK);
        emit BreakpointAdded(line_number);
        emit BreakpointToggled(line_number, true);
        d->lineNumberArea->update(); // 刷新行号区域
    }
}

void CodeEditor::RemoveBreakpoint(int line_number)
{
    Q_D(CodeEditor);
    if (d->breakpoints.remove(line_number)) {
        for (int index = d->breakpointBlocks.size() - 1; index >= 0; --index) {
            if (d->breakpointBlocks[index].blockNumber() + 1 == line_number) {
                d->breakpointBlocks.removeAt(index);
            }
        }
        emit BreakpointRemoved(line_number);
        emit BreakpointToggled(line_number, false);
        d->lineNumberArea->update(); // 刷新行号区域
    }
}

void CodeEditor::ClearAllBreakpoints()
{
    Q_D(CodeEditor);
    const QSet<int> OLD_BREAKPOINTS = d->breakpoints;
    d->breakpoints.clear();
    d->breakpointBlocks.clear();
    for (const int LINE_NUMBER : OLD_BREAKPOINTS) {
        emit BreakpointRemoved(LINE_NUMBER);
    }
    d->lineNumberArea->update();
}

bool CodeEditor::HasBreakpoint(int line_number) const
{
    Q_D(const CodeEditor);
    return d->breakpoints.contains(line_number);
}

void CodeEditor::SyncBreakpoints()
{
    Q_D(CodeEditor);
    QSet<int> updated;
    for (int index = d->breakpointBlocks.size() - 1; index >= 0; --index) {
        const auto &block = d->breakpointBlocks[index];
        if (block.isValid()) {
            updated.insert(block.blockNumber() + 1);
        } else {
            d->breakpointBlocks.removeAt(index);
        }
    }
    const auto OLD = d->breakpoints;
    d->breakpoints = updated;
    for (const int LINE : OLD - updated) {
        emit BreakpointRemoved(LINE);
        emit BreakpointToggled(LINE, false);
    }
    for (const int LINE : updated - OLD) {
        emit BreakpointAdded(LINE);
        emit BreakpointToggled(LINE, true);
    }
    d->lineNumberArea->update();
}

QSet<int> CodeEditor::GetBreakpoints() const
{
    Q_D(const CodeEditor);
    return d->breakpoints;
}

// ============================================================================
// 调试箭头管理
// ============================================================================

void CodeEditor::SetCurrentDebugLine(int line_number)
{
    Q_D(CodeEditor);
    d->currentDebugLine = line_number;
    d->lineNumberArea->update(); // 重绘行号区域
    HighlightCurrentLine();      // 重绘代码区域高亮
}

void CodeEditor::ClearCurrentDebugLine()
{
    Q_D(CodeEditor);
    d->currentDebugLine = -1;
    d->lineNumberArea->update(); // 重绘行号区域
    HighlightCurrentLine();      // 重绘代码区域高亮
}

int CodeEditor::GetCurrentDebugLine() const
{
    Q_D(const CodeEditor);
    return d->currentDebugLine;
}

// ============================================================================
// LineNumberArea Mouse Event
// ============================================================================

void LineNumberArea::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        // 计算点击位置对应的行号
        QTextBlock block        = codeEditor->firstVisibleBlock();
        int        block_number = block.blockNumber();
        int        top = qRound(codeEditor->blockBoundingGeometry(block).translated(codeEditor->contentOffset()).top());
        int        bottom = top + qRound(codeEditor->blockBoundingRect(block).height());

        while (block.isValid()) {
            if (top <= event->pos().y() && event->pos().y() < bottom) {
                const int LINE_NUMBER = block_number + 1; // 行号从1开始
                codeEditor->ToggleBreakpoint(LINE_NUMBER);
                break;
            }

            block  = block.next();
            top    = bottom;
            bottom = top + qRound(codeEditor->blockBoundingRect(block).height());
            ++block_number;
        }
    }

    QWidget::mousePressEvent(event);
}

// ============================================================================
// JX3MacroSyntaxHighlighter Implementation
// ============================================================================

JX3MacroSyntaxHighlighter::JX3MacroSyntaxHighlighter(QTextDocument *parent) : QSyntaxHighlighter(parent)
{
    HighlightingRule rule;

    // VSCode 风格配色

    commandFormat.setForeground(QColor(112, 185, 255));
    const QStringList COMMAND_PATTERNS = {
        "/cast\\b",       "/fcast\\b",      "/scast\\b",      "/sfcast\\b",
        "/switch\\b", // 切换宏命令
        "/add_target\\b", "/set_target\\b", "/set_player\\b", "/change_target\\b", "/add_buff\\b", "/clear_buff\\b",
        "/stop\\b",       "/continue\\b",   "/end\\b"
    };
    for (const QString &pattern : COMMAND_PATTERNS) {
        rule.pattern = QRegularExpression(pattern);
        rule.format  = commandFormat;
        highlightingRules.append(rule);
    }

    macroFormat.setForeground(QColor(201, 173, 128));
    rule.pattern = QRegularExpression("\\bmacro\\b");
    rule.format  = macroFormat;
    highlightingRules.append(rule);

    numberFormat.setForeground(QColor(239, 188, 113));
    rule.pattern = QRegularExpression("\\b[0-9]+:[0-9]+\\.[0-9]+\\b");
    rule.format  = numberFormat;
    highlightingRules.append(rule);

    // 数字 - 浅绿色 #b5cea8
    rule.pattern = QRegularExpression("\\b[0-9]+\\.?[0-9]*\\b");
    rule.format  = numberFormat;
    highlightingRules.append(rule);

    conditionFormat.setForeground(QColor(95, 216, 223));

    // 带冒号的关键字
    const QStringList COLON_KEYWORDS = {
        "buff",           "nobuff",       "bufftime", "qidian",  "energy",     "sun",      "moon",     "sun_power",
        "moon_power",     "skill_energy", "skill",    "noskill", "last_skill", "skill_cd", "npclevel", "nearby_enemy",
        "skill_notin_cd", "yaoxing",      "tbuff",    "tnobuff", "tbufftime",  "Duff",     "Buff",     "ebufftime"
    };
    for (const QString &keyword : COLON_KEYWORDS) {
        rule.pattern = QRegularExpression("\\b" + keyword + "(?=\\s*:)");
        rule.format  = conditionFormat;
        highlightingRules.append(rule);
    }

    // 带比较运算符的关键字 (>, <, >=, <=, =)
    const QStringList COMPARISON_KEYWORDS = { "qidian", "life",  "mana",  "rage",
                                              "tlife",  "tmana", "trage", "nearby_enemy" };
    for (const QString &keyword : COMPARISON_KEYWORDS) {
        rule.pattern = QRegularExpression("\\b" + keyword + "(?=\\s*[><=])");
        rule.format  = conditionFormat;
        highlightingRules.append(rule);
    }

    // 带等号的关键字（macro 用于宏名称，name 用于条件判断）
    const QStringList EQUAL_KEYWORDS = { "id",     "name",  "stack_num",  "duration", "distance",
                                         "shield", "level", "vulnerable", "shadows",  "style" };
    for (const QString &keyword : EQUAL_KEYWORDS) {
        rule.pattern = QRegularExpression("\\b" + keyword + "(?=\\s*=)");
        rule.format  = conditionFormat;
        highlightingRules.append(rule);
    }

    // 独立关键字
    rule.pattern = QRegularExpression("\\bdead\\b");
    rule.format  = conditionFormat;
    highlightingRules.append(rule);

    operatorFormat.setForeground(QColor(160, 172, 192));
    rule.pattern = QRegularExpression("[\\[\\]&|=><~:]");
    rule.format  = operatorFormat;
    highlightingRules.append(rule);

    skillNameFormat.setForeground(QColor(244, 245, 247));
    rule.pattern = QRegularExpression("[\\x{4e00}-\\x{9fa5}]+"); // 匹配中文字符
    rule.format  = skillNameFormat;
    highlightingRules.append(rule);

    commentFormat.setForeground(QColor(132, 146, 166));
}

void JX3MacroSyntaxHighlighter::highlightBlock(const QString &text)
{
    // 应用除注释外的所有规则
    for (const HighlightingRule &rule : highlightingRules) {
        QRegularExpressionMatchIterator match_iterator = rule.pattern.globalMatch(text);
        while (match_iterator.hasNext()) {
            const QRegularExpressionMatch MATCH = match_iterator.next();
            setFormat(MATCH.capturedStart(), MATCH.capturedLength(), rule.format);
        }
    }

    // 最后处理注释，覆盖所有其他格式
    const QRegularExpression        COMMENT_PATTERN("(//|##?)[^\n]*");
    QRegularExpressionMatchIterator comment_iterator = COMMENT_PATTERN.globalMatch(text);
    while (comment_iterator.hasNext()) {
        const QRegularExpressionMatch MATCH = comment_iterator.next();
        setFormat(MATCH.capturedStart(), MATCH.capturedLength(), commentFormat);
    }
}

// ============================================================================
// 语法检测
// ============================================================================

void CodeEditor::SetSyntaxCheckEnabled(bool enable)
{
    Q_D(CodeEditor);
    d->syntaxCheckEnabled = enable;

    if (enable) {
        // 立即执行一次检测
        CheckSyntax();
    } else {
        // 清除所有错误标记
        d->syntaxErrors.clear();
        HighlightCurrentLine();
    }
}

bool CodeEditor::IsSyntaxCheckEnabled() const
{
    Q_D(const CodeEditor);
    return d->syntaxCheckEnabled;
}

QList<SyntaxError> CodeEditor::GetSyntaxErrors() const
{
    Q_D(const CodeEditor);
    return d->syntaxErrors;
}

void CodeEditor::SetSyntaxValidator(std::function<QList<SyntaxError>(const QString &)> validator)
{
    Q_D(CodeEditor);
    d->syntaxValidator = std::move(validator);
    CheckSyntax();
}

void CodeEditor::CheckSyntax()
{
    Q_D(CodeEditor);

    if (!d->syntaxCheckEnabled || (!d->syntaxValidator && !d->syntaxChecker)) {
        return;
    }

    // 执行语法检测
    d->syntaxErrors = d->syntaxValidator ? d->syntaxValidator(toPlainText()) : d->syntaxChecker->Check(document());
    emit SyntaxErrorsChanged(d->syntaxErrors.size());

    // 刷新高亮显示
    HighlightCurrentLine();
}

// ============================================================================
// JX3MacroSyntaxChecker Implementation
// ============================================================================

QList<SyntaxError> JX3MacroSyntaxChecker::Check(QTextDocument *document)
{
    QList<SyntaxError> errors;

    for (QTextBlock block = document->begin(); block.isValid(); block = block.next()) {
        const QString LINE        = block.text();
        const int     LINE_NUMBER = block.blockNumber() + 1;

        // 跳过空行和注释
        const QString TRIMMED = LINE.trimmed();
        if (TRIMMED.isEmpty() || TRIMMED.startsWith('#') || TRIMMED.startsWith("//")) {
            continue;
        }

        // 检测方括号匹配（优先级最高）
        CheckBrackets(LINE, LINE_NUMBER, errors);

        // 检测 macro 定义行
        if (TRIMMED.startsWith("macro ")) {
            CheckMacroLine(LINE, LINE_NUMBER, errors);
        }
        // 检测命令行
        else if (TRIMMED.startsWith('/'))
        {
            CheckCommandLine(LINE, LINE_NUMBER, errors);
        }

        // 检测多余空格（所有行）
        CheckExtraSpaces(LINE, LINE_NUMBER, errors);
    }

    return errors;
}

void JX3MacroSyntaxChecker::CheckCommandLine(const QString &line, int line_number, QList<SyntaxError> &errors)
{
    const QString TRIMMED = line.trimmed();

    // 定义合法命令列表
    const QStringList VALID_COMMANDS = {
        "/cast",          "/fcast",    "/scast",      "/sfcast", "/switch",   "/add_target", "/set_target",
        "/change_target", "/add_buff", "/clear_buff", "/stop",   "/continue", "/end"
    };

    // 提取命令（第一个单词）
    const int     SPACE_INDEX = TRIMMED.indexOf(' ');
    const QString COMMAND     = (SPACE_INDEX > 0) ? TRIMMED.left(SPACE_INDEX) : TRIMMED;

    // 检查命令是否合法
    if (!VALID_COMMANDS.contains(COMMAND)) {
        const int COLUMN = line.indexOf(COMMAND);
        errors.append(
            SyntaxError(line_number, COLUMN, COMMAND.length(), SyntaxError::SYNTAX_ERROR, QString("未知命令: %1").arg(COMMAND)));
        return;
    }

    // 检查 cast 命令是否有技能参数
    if ((COMMAND == "/cast" || COMMAND == "/fcast" || COMMAND == "/scast" || COMMAND == "/sfcast")) {
        const QString AFTER_COMMAND = TRIMMED.mid(COMMAND.length()).trimmed();

        // 检查是否有条件表达式（方括号）
        const int bracket_start = static_cast<int>(AFTER_COMMAND.indexOf('['));
        const int BRACKET_END   = static_cast<int>(AFTER_COMMAND.indexOf(']'));

        QString skill_part;
        if (bracket_start >= 0 && BRACKET_END > bracket_start) {
            // 有条件表达式：技能名可能在方括号前或后
            const QString before_bracket = AFTER_COMMAND.left(bracket_start).trimmed();
            const QString after_bracket  = AFTER_COMMAND.mid(BRACKET_END + 1).trimmed();

            // 技能名在方括号前或后都可以
            if (!before_bracket.isEmpty()) {
                skill_part = before_bracket;
            } else if (!after_bracket.isEmpty()) {
                skill_part = after_bracket;
            }
        } else {
            // 没有条件表达式
            skill_part = AFTER_COMMAND;
        }

        if (skill_part.isEmpty()) {
            const int COLUMN = static_cast<int>(line.indexOf(COMMAND)) + COMMAND.length();
            errors.append(SyntaxError(line_number, COLUMN, 1, SyntaxError::SYNTAX_ERROR, QString("%1 命令缺少技能名称").arg(COMMAND)));
        }
    }

    // 检测条件表达式
    CheckConditions(line, line_number, errors);
}

void JX3MacroSyntaxChecker::CheckMacroLine(const QString &line, int line_number, QList<SyntaxError> &errors)
{
    const QString TRIMMED = line.trimmed();

    // macro 定义格式: macro 宏名称
    const QRegularExpression      MACRO_PATTERN("^macro\\s+(\\S+)(.*)$");
    const QRegularExpressionMatch MATCH = MACRO_PATTERN.match(TRIMMED);

    if (!MATCH.hasMatch()) {
        const int COLUMN = line.indexOf("macro");
        errors.append(SyntaxError(line_number, COLUMN, 5, SyntaxError::SYNTAX_ERROR, "macro 定义格式错误，正确格式: macro 宏名称"));
        return;
    }

    // 检查是否有多余内容
    const QString extra_content = MATCH.captured(2).trimmed();
    if (!extra_content.isEmpty()) {
        const int COLUMN = line.indexOf(extra_content);
        errors.append(SyntaxError(line_number, COLUMN, extra_content.length(), SyntaxError::WARNING, "macro 定义后有多余内容"));
    }
}

void JX3MacroSyntaxChecker::CheckConditions(const QString &line, int line_number, QList<SyntaxError> &errors)
{
    // 查找方括号内的条件表达式
    const QRegularExpression        BRACKET_PATTERN("\\[([^\\]]*)\\]");
    QRegularExpressionMatchIterator it = BRACKET_PATTERN.globalMatch(line);

    while (it.hasNext()) {
        const QRegularExpressionMatch MATCH      = it.next();
        const QString                 CONDITIONS = MATCH.captured(1);

        // 检查条件是否为空
        if (CONDITIONS.trimmed().isEmpty()) {
            const int COLUMN = MATCH.capturedStart();
            errors.append(SyntaxError(line_number, COLUMN, MATCH.capturedLength(), SyntaxError::WARNING, "条件表达式为空"));
            continue;
        }

        // 检查中括号内是否包含空格（不允许）
        if (CONDITIONS.contains(' ')) {
            const int SPACE_POS = static_cast<int>(CONDITIONS.indexOf(' '));
            const int COLUMN    = static_cast<int>(MATCH.capturedStart()) + 1 + SPACE_POS; // +1 跳过左括号
            errors.append(SyntaxError(line_number, COLUMN, 1, SyntaxError::SYNTAX_ERROR, "条件表达式中不允许有空格"));
        }

        // 分割多个条件（用 & 或 | 分隔）
        const QStringList cond_list = CONDITIONS.split(QRegularExpression("[&|]"));

        for (const QString &cond : cond_list) {
            if (cond.isEmpty()) {
                continue;
            }

            // 定义合法的条件关键字
            const QStringList VALID_KEYWORDS = {
                "buff",         "nobuff",       "bufftime", "tbuff",   "tnobuff",    "tbufftime", "Buff",
                "Duff",         "ebufftime",    "qidian",   "energy",  "sun",        "moon",      "sun_power",
                "moon_power",   "skill_energy", "skill",    "noskill", "last_skill", "skill_cd",  "skill_notin_cd",
                "life",         "mana",         "rage",     "tlife",   "tmana",      "trage",     "npclevel",
                "nearby_enemy", "yaoxing",      "dead",     "id",      "name",       "stack_num", "duration",
                "distance",     "shield",       "level"
            };

            // 提取条件关键字（冒号或运算符之前的部分）
            const QRegularExpression      KEYWORD_PATTERN("^(\\w+)[:><=~]");
            const QRegularExpressionMatch KW_MATCH = KEYWORD_PATTERN.match(cond);

            if (KW_MATCH.hasMatch()) {
                const QString KEYWORD = KW_MATCH.captured(1);
                if (!VALID_KEYWORDS.contains(KEYWORD)) {
                    const int COLUMN = line.indexOf(KEYWORD, MATCH.capturedStart());
                    if (COLUMN >= 0) {
                        errors.append(SyntaxError(line_number,
                                                  COLUMN,
                                                  KEYWORD.length(),
                                                  SyntaxError::WARNING,
                                                  QString("未知条件关键字: %1").arg(KEYWORD)));
                    }
                }
            }
        }
    }
}

void JX3MacroSyntaxChecker::CheckExtraSpaces(const QString &line, int line_number, QList<SyntaxError> &errors)
{
    // 检测行尾空格
    if (line.endsWith(' ') || line.endsWith('\t')) {
        int column = line.length() - 1;
        while (column > 0 && (line[column] == ' ' || line[column] == '\t')) {
            column--;
        }
        column++; // 指向第一个尾部空格

        errors.append(SyntaxError(line_number, column, line.length() - column, SyntaxError::WARNING, "行尾有多余空格"));
    }

    // 检测多个连续空格（排除缩进）
    const QString TRIMMED = line.trimmed();
    if (!TRIMMED.isEmpty()) {
        const int     INDENT_END = line.indexOf(TRIMMED);
        const QString content    = line.mid(INDENT_END);

        const QRegularExpression        MULTI_SPACE_PATTERN("  +"); // 2个或以上连续空格
        QRegularExpressionMatchIterator it = MULTI_SPACE_PATTERN.globalMatch(content);

        while (it.hasNext()) {
            const QRegularExpressionMatch MATCH  = it.next();
            const int                     COLUMN = INDENT_END + MATCH.capturedStart();
            errors.append(SyntaxError(line_number, COLUMN, MATCH.capturedLength(), SyntaxError::WARNING, "有多个连续空格"));
        }
    }
}

void JX3MacroSyntaxChecker::CheckBrackets(const QString &line, int line_number, QList<SyntaxError> &errors)
{
    int depth           = 0;
    int last_open_index = -1;

    for (int i = 0; i < line.length(); ++i) {
        if (line[i] == '[') {
            depth++;
            last_open_index = i;
        } else if (line[i] == ']') {
            depth--;
            if (depth < 0) {
                // 多余的右括号
                errors.append(SyntaxError(line_number, i, 1, SyntaxError::SYNTAX_ERROR, "多余的右方括号 ']'"));
                return;
            }
        }
    }

    // 缺少右括号
    if (depth > 0 && last_open_index >= 0) {
        errors.append(SyntaxError(line_number, last_open_index, 1, SyntaxError::SYNTAX_ERROR, "缺少右方括号 ']'"));
    }
}

// ============================================================================
// 代码格式化
// ============================================================================

void CodeEditor::FormatDocument()
{
    if (isReadOnly()) {
        return;
    }
    Q_D(CodeEditor);

    const QString     text  = toPlainText();
    const QStringList LINES = text.split('\n');
    QStringList       formatted_lines;

    int           indent_level = 0;
    const QString INDENT_STRING(d->tabWidth, ' ');

    for (const QString &line : LINES) {
        const QString TRIMMED = line.trimmed();

        // 跳过空行和注释行
        if (TRIMMED.isEmpty() || TRIMMED.startsWith('#') || TRIMMED.startsWith("//")) {
            formatted_lines.append(TRIMMED);
            continue;
        }

        // 处理缩进
        QString formatted;

        // 如果以 macro 开头，不缩进
        if (TRIMMED.startsWith("macro ")) {
            indent_level = 0;
            formatted    = FormatMacroLine(TRIMMED);
        }
        // 如果以命令开头，缩进一级并格式化
        else if (TRIMMED.startsWith('/'))
        {
            indent_level = 1;
            formatted    = INDENT_STRING + FormatCommandLine(TRIMMED);
        }
        // 时间格式（事件语句）
        else if (QRegularExpression("^\\d+:\\d+\\.\\d+").match(TRIMMED).hasMatch())
        {
            indent_level = 0;
            formatted    = TRIMMED;
        }
        // 其他情况保持当前缩进
        else {
            formatted = QString(indent_level * d->tabWidth, ' ') + TRIMMED;
        }

        formatted_lines.append(formatted);
    }

    // 更新文档内容
    QTextCursor cursor = textCursor();
    cursor.beginEditBlock();
    cursor.select(QTextCursor::Document);
    cursor.insertText(formatted_lines.join('\n'));
    cursor.endEditBlock();
}

QString CodeEditor::FormatCommandLine(const QString &line)
{
    QString trimmed = line.trimmed();

    // 提取命令部分（第一个单词）
    const int FIRST_SPACE = static_cast<int>(trimmed.indexOf(' '));
    if (FIRST_SPACE < 0) {
        return trimmed; // 只有命令，没有参数
    }

    const QString COMMAND       = trimmed.left(FIRST_SPACE);
    const QString AFTER_COMMAND = trimmed.mid(FIRST_SPACE + 1);

    // 查找条件表达式（方括号）
    const int bracket_start = static_cast<int>(AFTER_COMMAND.indexOf('['));

    if (bracket_start < 0) {
        // 没有条件表达式，只规范化空格：命令 参数
        return COMMAND + " " + AFTER_COMMAND.trimmed();
    }

    // 有条件表达式
    const QString before_bracket = AFTER_COMMAND.left(bracket_start).trimmed();
    const QString from_bracket   = AFTER_COMMAND.mid(bracket_start);

    // 找到右括号位置
    const int BRACKET_END = static_cast<int>(from_bracket.indexOf(']'));
    if (BRACKET_END < 0) {
        // 没有右括号，返回原样（会被语法检测捕获）
        return COMMAND + " " + AFTER_COMMAND.trimmed();
    }

    // 提取条件表达式和括号后的内容
    const QString bracket_content = from_bracket.left(BRACKET_END + 1);
    const QString after_bracket   = from_bracket.mid(BRACKET_END + 1).trimmed();

    // 格式化条件表达式（移除内部空格）
    const QString FORMATTED_CONDITION = FormatConditionExpression(bracket_content);

    // 重组：/命令 [条件] 名称
    QStringList parts;
    parts.append(COMMAND);

    if (!before_bracket.isEmpty()) {
        parts.append(before_bracket); // 条件前的参数
    }

    parts.append(FORMATTED_CONDITION); // 条件

    if (!after_bracket.isEmpty()) {
        parts.append(after_bracket); // 条件后的参数
    }

    return parts.join(" ");
}

QString CodeEditor::FormatConditionExpression(const QString &expr)
{
    QString result = expr;

    // 查找所有方括号对
    const QRegularExpression        BRACKET_PATTERN(R"(\[([^\]]*)\])");
    QRegularExpressionMatchIterator it = BRACKET_PATTERN.globalMatch(expr);

    // 从后向前替换（避免位置偏移）
    QList<QRegularExpressionMatch> matches;
    while (it.hasNext()) {
        matches.append(it.next());
    }

    for (int i = static_cast<int>(matches.size()) - 1; i >= 0; --i) {
        const QRegularExpressionMatch &match      = matches[i];
        const QString                  CONDITIONS = match.captured(1);

        // 移除条件内的所有空格
        QString no_spaces = CONDITIONS;
        (void)no_spaces.remove(' ');

        // 替换原始表达式
        result.replace(static_cast<int>(match.capturedStart()), static_cast<int>(match.capturedLength()), "[" + no_spaces + "]");
    }

    return result;
}

QString CodeEditor::FormatMacroLine(const QString &line)
{
    QString trimmed = line.trimmed();

    // macro 定义格式: macro 宏名称
    const QRegularExpression      MACRO_PATTERN("^macro\\s+(\\S+)");
    const QRegularExpressionMatch MATCH = MACRO_PATTERN.match(trimmed);

    if (MATCH.hasMatch()) {
        return "macro " + MATCH.captured(1);
    }

    return trimmed;
}
