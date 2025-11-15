#include "code_editor.h"

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
constexpr int EDITOR_BACKGROUND_R = 30;
constexpr int EDITOR_BACKGROUND_G = 30;
constexpr int EDITOR_BACKGROUND_B = 30;

constexpr int EDITOR_TEXT_R = 212;
constexpr int EDITOR_TEXT_G = 212;
constexpr int EDITOR_TEXT_B = 212;

constexpr int EDITOR_HIGHLIGHT_R = 38;
constexpr int EDITOR_HIGHLIGHT_G = 79;
constexpr int EDITOR_HIGHLIGHT_B = 120;

constexpr int CURRENT_LINE_R = 44;
constexpr int CURRENT_LINE_G = 44;
constexpr int CURRENT_LINE_B = 44;

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

constexpr int DEBUG_BG_R     = 255;
constexpr int DEBUG_BG_G     = 255;
constexpr int DEBUG_BG_B     = 0;
constexpr int DEBUG_BG_ALPHA = 40;

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

    const auto *menuOption = qstyleoption_cast<const QStyleOptionMenuItem *>(option);
    if (menuOption == nullptr) {
        QProxyStyle::drawControl(element, option, painter, widget);
        return;
    }

    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);

    // 绘制分隔符
    if (menuOption->menuItemType == QStyleOptionMenuItem::Separator) {
        painter->fillRect(option->rect, QColor(BACKGROUND_COLOR_R, BACKGROUND_COLOR_G, BACKGROUND_COLOR_B));
        int y = option->rect.center().y() + 1;
        painter->setPen(QColor(BORDER_COLOR_R, BORDER_COLOR_G, BORDER_COLOR_B));
        painter->drawLine(option->rect.left() + SEPARATOR_MARGIN, y, option->rect.right() - SEPARATOR_MARGIN, y);
        painter->restore();
        return;
    }

    // 绘制背景（圆角矩形）
    if ((menuOption->state & State_Selected) != 0) {
        painter->setPen(Qt::NoPen);
        painter->setBrush(QColor(HOVER_COLOR_R, HOVER_COLOR_G, HOVER_COLOR_B));
        QRect hoverRect =
            option->rect.adjusted(HOVER_RECT_MARGIN, HOVER_RECT_VERTICAL_MARGIN, -HOVER_RECT_MARGIN, -HOVER_RECT_VERTICAL_MARGIN);
        painter->drawRoundedRect(hoverRect, SMALL_RADIUS, SMALL_RADIUS);
    } else {
        painter->fillRect(option->rect, QColor(BACKGROUND_COLOR_R, BACKGROUND_COLOR_G, BACKGROUND_COLOR_B));
    }

    // 设置字体
    QFont font = QApplication::font();
    font.setPointSizeF(FONT_SIZE);
    painter->setFont(font);

    // 分离菜单文本和快捷键
    QString     fullText     = menuOption->text;
    QStringList parts        = fullText.split('\t');
    QString     menuText     = parts.value(0);
    QString     shortcutText = parts.value(1);

    QRect textRect = option->rect.adjusted(MENU_PADDING, 0, -MENU_PADDING, 0);

    bool isSelected = (menuOption->state & State_Selected) != 0;
    bool isEnabled  = (menuOption->state & State_Enabled) != 0;

    // 绘制菜单项文字
    if (!isEnabled) {
        painter->setPen(QColor(DISABLED_TEXT_COLOR_R, DISABLED_TEXT_COLOR_G, DISABLED_TEXT_COLOR_B));
    } else if (isSelected) {
        painter->setPen(QColor(WHITE_COLOR_RGB, WHITE_COLOR_RGB, WHITE_COLOR_RGB)); // hover 时纯白色
    } else {
        painter->setPen(QColor(NORMAL_TEXT_COLOR_RGB, NORMAL_TEXT_COLOR_RGB, NORMAL_TEXT_COLOR_RGB));
    }
    painter->drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, menuText);

    // 绘制快捷键
    if (!shortcutText.isEmpty()) {
        if (isSelected) {
            painter->setPen(QColor(WHITE_COLOR_RGB, WHITE_COLOR_RGB, WHITE_COLOR_RGB)); // hover 时纯白色
        } else {
            painter->setPen(QColor(SHORTCUT_TEXT_COLOR_RGB, SHORTCUT_TEXT_COLOR_RGB, SHORTCUT_TEXT_COLOR_RGB)); // 默认暗灰色
        }
        painter->drawText(textRect, Qt::AlignRight | Qt::AlignVCenter, shortcutText);
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
    QSize newSize = QProxyStyle::sizeFromContents(type, option, size, widget);

    if (type == CT_MenuItem) {
        const auto *menuOption = qstyleoption_cast<const QStyleOptionMenuItem *>(option);
        if (menuOption != nullptr && menuOption->menuItemType == QStyleOptionMenuItem::Separator) {
            return { newSize.width(), SEPARATOR_HEIGHT };
        }
        return QSize(newSize.width(), MENU_ITEM_HEIGHT);
    }

    return newSize;
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
        lineNumberArea(new LineNumberArea(editor)), tabWidth(4), showLineNumbers(true),
        currentLineColor(QColor(Qt::yellow).lighter(CURRENT_LINE_LIGHTER_VALUE)), syntaxHighlightingEnabled(true),
        syntaxType(CodeEditor::Cpp), cppHighlighter(nullptr), jx3MacroHighlighter(nullptr), currentDebugLine(-1),
        syntaxCheckEnabled(true), syntaxChecker(nullptr), syntaxCheckTimer(new QTimer(editor)), q_ptr(editor)
    {
        // 配置语法检测定时器：延迟500ms执行，避免频繁检测
        syntaxCheckTimer->setSingleShot(true);
        syntaxCheckTimer->setInterval(SYNTAX_CHECK_DELAY_MS);
    }

    LineNumberArea                           *lineNumberArea;
    int                                       tabWidth;
    bool                                      showLineNumbers;
    QColor                                    currentLineColor;
    bool                                      syntaxHighlightingEnabled;
    CodeEditor::SyntaxType                    syntaxType;
    QScopedPointer<CppSyntaxHighlighter>      cppHighlighter;
    QScopedPointer<JX3MacroSyntaxHighlighter> jx3MacroHighlighter;
    QSet<int>                                 breakpoints;      // 断点集合（行号从1开始）
    int                                       currentDebugLine; // 当前调试行（-1表示无）

    // 语法检测
    bool                          syntaxCheckEnabled; // 是否启用语法检测
    QScopedPointer<SyntaxChecker> syntaxChecker;      // 语法检测器
    QList<SyntaxError>            syntaxErrors;       // 当前语法错误列表
    QTimer                       *syntaxCheckTimer;   // 延迟检测定时器

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

    // 连接语法检测信号
    connect(this, &CodeEditor::textChanged, d->syntaxCheckTimer, static_cast<void (QTimer::*)()>(&QTimer::start));
    connect(d->syntaxCheckTimer, &QTimer::timeout, this, &CodeEditor::CheckSyntax);

    UpdateLineNumberAreaWidth(0);
    HighlightCurrentLine();

    // 设置等宽字体：英文 Monaco，中文霞鹜文楷等宽
    setFont(Resources::MonoFont());

    // 设置文档默认字体，确保中文字符能正确回退
    document()->setDefaultFont(Resources::MonoFont());

    // VSCode 深色主题配色
    QPalette palette = this->palette();
    palette.setColor(QPalette::Base, QColor(EDITOR_BACKGROUND_R, EDITOR_BACKGROUND_G, EDITOR_BACKGROUND_B)); // 编辑器背景 #1e1e1e
    palette.setColor(QPalette::Text, QColor(EDITOR_TEXT_R, EDITOR_TEXT_G, EDITOR_TEXT_B)); // 文本颜色 #d4d4d4
    palette.setColor(QPalette::Highlight, QColor(EDITOR_HIGHLIGHT_R, EDITOR_HIGHLIGHT_G, EDITOR_HIGHLIGHT_B)); // 选中背景 #264f78
    palette.setColor(QPalette::HighlightedText, QColor(WHITE_COLOR_RGB, WHITE_COLOR_RGB, WHITE_COLOR_RGB)); // 选中文本
    setPalette(palette);

    // 设置制表符宽度
    QFontMetrics metrics(Resources::MonoFont());
    setTabStopDistance(TAB_STOP * metrics.horizontalAdvance(' '));

    // 设置当前行高亮颜色为 VSCode 风格
    d->currentLineColor = QColor(CURRENT_LINE_R, CURRENT_LINE_G, CURRENT_LINE_B); // #2c2c2c

    // 默认启用 C++ 语法高亮
    d->cppHighlighter.reset(new CppSyntaxHighlighter(document()));

    // 默认启用 JX3Macro 语法检测器（因为默认是 C++，稍后会根据类型切换）
    d->syntaxChecker.reset(new JX3MacroSyntaxChecker(this));
}

CodeEditor::~CodeEditor() = default;

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
    int space = BREAKPOINT_AREA_WIDTH + (fontMetrics().horizontalAdvance(QLatin1Char('9')) * digits) + LINE_NUMBER_RIGHT_MARGIN;
    return space;
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

    QRect cr = contentsRect();
    d->lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), LineNumberAreaWidth(), cr.height()));
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
    QMap<int, QStringList> errorsByLine;
    for (const SyntaxError &error : d->syntaxErrors) {
        if (error.severity == SyntaxError::Error) {
            errorsByLine[error.line].append(error.message);
        }
    }

    if (errorsByLine.isEmpty()) {
        return;
    }

    // 绘制行尾错误信息
    QPainter painter(viewport());
    painter.setRenderHint(QPainter::Antialiasing);

    QFont errorFont = font();
    errorFont.setItalic(true);
    painter.setFont(errorFont);
    painter.setPen(QColor(ERROR_COLOR_R, ERROR_COLOR_G, ERROR_COLOR_B)); // 红色

    for (auto it = errorsByLine.constBegin(); it != errorsByLine.constEnd(); ++it) {
        int                lineNumber = it.key();
        const QStringList &messages   = it.value();

        QTextBlock block = document()->findBlockByLineNumber(lineNumber - 1);
        if (!block.isValid()) {
            continue;
        }

        // 计算行的绘制位置
        QRectF blockRect = blockBoundingGeometry(block).translated(contentOffset());
        int    top       = qRound(blockRect.top());
        int    height    = qRound(blockRect.height());

        // 计算行尾位置
        QString      lineText = block.text();
        QFontMetrics fm(font());
        int          textWidth = fm.horizontalAdvance(lineText);
        int          xPos      = textWidth + ERROR_ICON_X_OFFSET; // 距离行尾像素

        // 绘制错误信息
        QString errorText = "  // Error: " + messages.join("; ");
        painter.drawText(xPos, top, viewport()->width() - xPos, height, Qt::AlignLeft | Qt::AlignVCenter, errorText);
    }
}

void CodeEditor::HighlightCurrentLine()
{
    Q_D(CodeEditor);
    QList<QTextEdit::ExtraSelection> extraSelections;

    // 1. 绘制语法错误下划线（最底层）
    if (d->syntaxCheckEnabled && !d->syntaxErrors.isEmpty()) {
        for (const SyntaxError &error : d->syntaxErrors) {
            QTextBlock block = document()->findBlockByLineNumber(error.line - 1);
            if (!block.isValid()) {
                continue;
            }

            QTextEdit::ExtraSelection errorSelection;
            QTextCharFormat           format;

            if (error.severity == SyntaxError::Error) {
                format.setUnderlineColor(QColor(ERROR_COLOR_R, ERROR_COLOR_G, ERROR_COLOR_B)); // VSCode 红色 #ff4949
                format.setUnderlineStyle(QTextCharFormat::SpellCheckUnderline);                // 更粗的波浪线
            } else {
                format.setUnderlineColor(QColor(WARNING_COLOR_R, WARNING_COLOR_G, WARNING_COLOR_B)); // 橙色（警告）
                format.setUnderlineStyle(QTextCharFormat::SpellCheckUnderline);
            }

            errorSelection.format = format;
            errorSelection.cursor = QTextCursor(block);
            errorSelection.cursor.setPosition(block.position() + error.column);
            errorSelection.cursor.setPosition(block.position() + error.column + error.length, QTextCursor::KeepAnchor);
            extraSelections.append(errorSelection);
        }
    }

    // 2. 绘制调试行高亮（黄色背景）
    if (d->currentDebugLine > 0) {
        QTextBlock debugBlock = document()->findBlockByLineNumber(d->currentDebugLine - 1);
        if (debugBlock.isValid()) {
            QTextEdit::ExtraSelection debugSelection;
            debugSelection.format.setBackground(QColor(DEBUG_BG_R, DEBUG_BG_G, DEBUG_BG_B, DEBUG_BG_ALPHA)); // 浅黄色半透明 #ffff0028
            debugSelection.format.setProperty(QTextFormat::FullWidthSelection, true);
            debugSelection.cursor = QTextCursor(debugBlock);
            debugSelection.cursor.clearSelection();
            extraSelections.append(debugSelection);
        }
    }

    // 3. 绘制当前行高亮（深灰色背景）
    if (!isReadOnly()) {
        QTextEdit::ExtraSelection selection;
        selection.format.setBackground(d->currentLineColor);
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = textCursor();
        selection.cursor.clearSelection();
        extraSelections.append(selection);
    }

    setExtraSelections(extraSelections);
}

void CodeEditor::LineNumberAreaPaintEvent(QPaintEvent *event)
{
    Q_D(CodeEditor);
    QPainter painter(d->lineNumberArea);
    painter.setRenderHint(QPainter::Antialiasing, true);

    // VSCode 风格的行号区域背景 #1e1e1e
    painter.fillRect(event->rect(), QColor(30, 30, 30));

    QTextBlock block       = firstVisibleBlock();
    int        blockNumber = block.blockNumber();
    int        top         = qRound(blockBoundingGeometry(block).translated(contentOffset()).top());
    int        bottom      = top + qRound(blockBoundingRect(block).height());

    // 当前行号
    int currentBlockNumber = textCursor().blockNumber();

    // 断点圆圈参数
    constexpr int BREAKPOINT_SIZE   = 8; // 断点圆圈直径（更小更精致）
    constexpr int BREAKPOINT_MARGIN = 6; // 断点左边距

    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            int     lineNumber = blockNumber + 1; // 行号从1开始
            QString number     = QString::number(lineNumber);

            // 绘制断点（如果有）- VSCode 风格的红色圆圈
            if (d->breakpoints.contains(lineNumber)) {
                painter.setPen(Qt::NoPen);
                painter.setBrush(QColor(224, 78, 78)); // VSCode 断点红色 #e04e4e

                // 在行号左侧绘制圆形断点，稍微向下偏移以对齐行号中心
                int centerY = top + fontMetrics().height() / 2 + 2;
                painter.drawEllipse(QPoint(BREAKPOINT_MARGIN + BREAKPOINT_SIZE / 2, centerY), BREAKPOINT_SIZE / 2, BREAKPOINT_SIZE / 2);
            }

            // VSCode 风格：当前行行号高亮，其他行号灰色
            if (blockNumber == currentBlockNumber) {
                painter.setPen(QColor(212, 212, 212)); // 当前行 #d4d4d4
            } else {
                painter.setPen(QColor(133, 133, 133)); // 其他行 #858585
            }

            // 绘制调试行高亮（如果是当前调试行）- VSCode 风格：黄色背景
            int textX = BREAKPOINT_MARGIN + BREAKPOINT_SIZE + 4; // 断点后留4px间距

            if (d->currentDebugLine == lineNumber) {
                // 1. 绘制黄色背景高亮条（覆盖整个行号区域）
                painter.setPen(Qt::NoPen);
                painter.setBrush(QColor(255, 204, 0)); // VSCode 黄色 #ffcc00
                painter.drawRect(0, top, d->lineNumberArea->width(), fontMetrics().height());

                // 2. 重新绘制行号（黑色，覆盖在黄色背景上）
                painter.setPen(QColor(0, 0, 0)); // 黑色行号
                painter.drawText(textX, top, d->lineNumberArea->width() - textX - 6, fontMetrics().height(), Qt::AlignRight, number);

                // 3. 如果有断点，重新绘制断点（黑色，覆盖在黄色背景上）
                if (d->breakpoints.contains(lineNumber)) {
                    painter.setPen(Qt::NoPen);
                    painter.setBrush(QColor(0, 0, 0)); // 黑色断点
                    int centerY = top + fontMetrics().height() / 2 + 2;
                    painter.drawEllipse(QPoint(BREAKPOINT_MARGIN + BREAKPOINT_SIZE / 2, centerY), BREAKPOINT_SIZE / 2, BREAKPOINT_SIZE / 2);
                }
            } else {
                // 正常绘制行号（无调试高亮时）
                painter.drawText(textX, top, d->lineNumberArea->width() - textX - 6, fontMetrics().height(), Qt::AlignRight, number);
            }
        }

        block  = block.next();
        top    = bottom;
        bottom = top + qRound(blockBoundingRect(block).height());
        ++blockNumber;
    }
}

void CodeEditor::keyPressEvent(QKeyEvent *event)
{
    Q_D(CodeEditor);

    // Tab 键转换为空格
    if (event->key() == Qt::Key_Tab) {
        QTextCursor cursor = textCursor();
        cursor.insertText(QString(d->tabWidth, ' '));
        return;
    }

    // 回车时自动缩进
    if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
        QTextCursor cursor = textCursor();
        QTextBlock  block  = cursor.block();
        QString     text   = block.text();

        // 计算当前行的缩进
        int indent = 0;
        for (QChar ch : text) {
            if (ch == ' ') {
                indent++;
            } else if (ch == '\t') {
                indent += d->tabWidth;
            } else {
                break;
            }
        }

        // 如果当前行以 { 结尾，增加一级缩进
        QString trimmed = text.trimmed();
        if (trimmed.endsWith('{')) {
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
        QHelpEvent *helpEvent = static_cast<QHelpEvent *>(event);
        QTextCursor cursor    = cursorForPosition(helpEvent->pos());
        int         line      = cursor.blockNumber() + 1;
        int         column    = cursor.positionInBlock();

        // 查找当前位置的错误
        QString tooltipText;
        for (const SyntaxError &error : d->syntaxErrors) {
            if (error.line == line && column >= error.column && column < error.column + error.length) {
                if (!tooltipText.isEmpty()) {
                    tooltipText += "\n";
                }
                tooltipText += error.message;
            }
        }

        if (!tooltipText.isEmpty()) {
            QToolTip::showText(helpEvent->globalPos(), tooltipText, this);
        } else {
            QToolTip::hideText();
        }
        return true;
    }

    return QPlainTextEdit::event(event);
}

void CodeEditor::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu *menu = new QMenu(this);

    // 使用自定义样式代理（样式对象将由 QMenu 管理生命周期）
    MenuStyle *style = new MenuStyle();
    menu->setStyle(style);

    // 设置菜单属性
    menu->setAttribute(Qt::WA_DeleteOnClose);
    menu->setWindowFlags(menu->windowFlags() | Qt::NoDropShadowWindowHint);

    QTextCursor cursor       = textCursor();
    bool        hasSelection = cursor.hasSelection();
    bool        hasText      = !document()->isEmpty();
    QClipboard *clipboard    = QApplication::clipboard();
    bool        hasClipboard = !clipboard->text().isEmpty();

    // 剪切
    QAction *cutAction = menu->addAction("剪切\tCtrl+X");
    cutAction->setShortcut(QKeySequence::Cut);
    cutAction->setEnabled(hasSelection && !isReadOnly());
    connect(cutAction, &QAction::triggered, this, &CodeEditor::cut);

    // 复制
    QAction *copyAction = menu->addAction("复制\tCtrl+C");
    copyAction->setShortcut(QKeySequence::Copy);
    copyAction->setEnabled(hasSelection);
    connect(copyAction, &QAction::triggered, this, &CodeEditor::copy);

    // 粘贴
    QAction *pasteAction = menu->addAction("粘贴\tCtrl+V");
    pasteAction->setShortcut(QKeySequence::Paste);
    pasteAction->setEnabled(hasClipboard && !isReadOnly());
    connect(pasteAction, &QAction::triggered, this, &CodeEditor::paste);

    menu->addSeparator();

    // 全选
    QAction *selectAllAction = menu->addAction("全选\tCtrl+A");
    selectAllAction->setShortcut(QKeySequence::SelectAll);
    selectAllAction->setEnabled(hasText);
    connect(selectAllAction, &QAction::triggered, this, &CodeEditor::selectAll);

    // 格式化
    QAction *formatAction = menu->addAction("格式化\tShift+Alt+F");
    formatAction->setShortcut(QKeySequence("Shift+Alt+F"));
    formatAction->setEnabled(hasText && !isReadOnly());
    connect(formatAction, &QAction::triggered, this, &CodeEditor::FormatDocument);

    menu->addSeparator();

    // 撤销
    QAction *undoAction = menu->addAction("撤销\tCtrl+Z");
    undoAction->setShortcut(QKeySequence::Undo);
    undoAction->setEnabled(document()->isUndoAvailable() && !isReadOnly());
    connect(undoAction, &QAction::triggered, this, &CodeEditor::undo);

    // 重做
    QAction *redoAction = menu->addAction("重做\tCtrl+Y");
    redoAction->setShortcut(QKeySequence::Redo);
    redoAction->setEnabled(document()->isRedoAvailable() && !isReadOnly());
    connect(redoAction, &QAction::triggered, this, &CodeEditor::redo);

    menu->exec(event->globalPos());
    menu->deleteLater();
}

void CodeEditor::SetTabWidth(int spaces)
{
    Q_D(CodeEditor);
    d->tabWidth = spaces;
    QFontMetrics metrics(font());
    setTabStopDistance(spaces * metrics.horizontalAdvance(' '));
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
            case Cpp: d->cppHighlighter.reset(new CppSyntaxHighlighter(document())); break;
            case JX3Macro: d->jx3MacroHighlighter.reset(new JX3MacroSyntaxHighlighter(document())); break;
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
            case Cpp: d->cppHighlighter.reset(new CppSyntaxHighlighter(document())); break;
            case JX3Macro: d->jx3MacroHighlighter.reset(new JX3MacroSyntaxHighlighter(document())); break;
        }
    }

    // 切换语法检测器
    switch (type) {
        case JX3Macro: d->syntaxChecker.reset(new JX3MacroSyntaxChecker(this)); break;
        case Cpp:
            // C++ 暂时没有检测器
            d->syntaxChecker.reset(nullptr);
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
    QStringList controlKeywords = { "\\bif\\b",     "\\belse\\b", "\\bfor\\b",     "\\bwhile\\b", "\\bdo\\b",
                                    "\\bswitch\\b", "\\bcase\\b", "\\bdefault\\b", "\\bbreak\\b", "\\bcontinue\\b",
                                    "\\breturn\\b", "\\btry\\b",  "\\bcatch\\b",   "\\bthrow\\b", "\\bgoto\\b" };
    for (const QString &pattern : controlKeywords) {
        rule.pattern = QRegularExpression(pattern);
        rule.format  = keywordFormat;
        highlightingRules.append(rule);
    }

    // 类型关键字 - 蓝紫色 #569cd6
    QStringList typeKeywords = {
        "\\bchar\\b",     "\\bint\\b",     "\\bshort\\b",     "\\blong\\b",     "\\bfloat\\b",
        "\\bdouble\\b",   "\\bbool\\b",    "\\bvoid\\b",      "\\bsigned\\b",   "\\bunsigned\\b",
        "\\bauto\\b",     "\\bconst\\b",   "\\bvolatile\\b",  "\\bextern\\b",   "\\bstatic\\b",
        "\\bregister\\b", "\\bmutable\\b", "\\bconstexpr\\b", "\\bdecltype\\b", "\\btypename\\b"
    };
    for (const QString &pattern : typeKeywords) {
        rule.pattern = QRegularExpression(pattern);
        rule.format  = keywordFormat;
        highlightingRules.append(rule);
    }

    // 修饰符关键字 - 蓝紫色 #569cd6
    QStringList modifierKeywords = {
        "\\bclass\\b",        "\\bstruct\\b",           "\\bunion\\b",     "\\benum\\b",     "\\bnamespace\\b",
        "\\btemplate\\b",     "\\btypedef\\b",          "\\busing\\b",     "\\bpublic\\b",   "\\bprivate\\b",
        "\\bprotected\\b",    "\\bvirtual\\b",          "\\boverride\\b",  "\\bfinal\\b",    "\\bexplicit\\b",
        "\\bfriend\\b",       "\\binline\\b",           "\\boperator\\b",  "\\bnoexcept\\b", "\\bstatic_cast\\b",
        "\\bdynamic_cast\\b", "\\breinterpret_cast\\b", "\\bconst_cast\\b"
    };
    for (const QString &pattern : modifierKeywords) {
        rule.pattern = QRegularExpression(pattern);
        rule.format  = keywordFormat;
        highlightingRules.append(rule);
    }

    // new, delete, this, nullptr - 蓝紫色
    QStringList specialKeywords = { "\\bnew\\b",    "\\bdelete\\b", "\\bthis\\b",    "\\bnullptr\\b",
                                    "\\bsizeof\\b", "\\btypeid\\b", "\\bsignals\\b", "\\bslots\\b" };
    for (const QString &pattern : specialKeywords) {
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
        QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
        while (matchIterator.hasNext()) {
            QRegularExpressionMatch match = matchIterator.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }

    // 处理多行注释
    setCurrentBlockState(0);

    int startIndex = 0;
    if (previousBlockState() != 1) {
        startIndex = text.indexOf(commentStartExpression);
    }

    while (startIndex >= 0) {
        QRegularExpressionMatch match    = commentEndExpression.match(text, startIndex);
        int                     endIndex = match.capturedStart();
        int                     commentLength;
        if (endIndex == -1) {
            setCurrentBlockState(1);
            commentLength = text.length() - startIndex;
        } else {
            commentLength = endIndex - startIndex + match.capturedLength();
        }
        setFormat(startIndex, commentLength, multiLineCommentFormat);
        startIndex = text.indexOf(commentStartExpression, startIndex + commentLength);
    }
}

// ============================================================================
// Breakpoint Management
// ============================================================================

void CodeEditor::ToggleBreakpoint(int lineNumber)
{
    Q_D(CodeEditor);
    if (d->breakpoints.contains(lineNumber)) {
        RemoveBreakpoint(lineNumber);
    } else {
        SetBreakpoint(lineNumber, true);
    }
}

void CodeEditor::SetBreakpoint(int lineNumber, bool enabled)
{
    Q_D(CodeEditor);
    if (enabled && !d->breakpoints.contains(lineNumber)) {
        d->breakpoints.insert(lineNumber);
        emit BreakpointAdded(lineNumber);
        emit BreakpointToggled(lineNumber, true);
        d->lineNumberArea->update(); // 刷新行号区域
    }
}

void CodeEditor::RemoveBreakpoint(int lineNumber)
{
    Q_D(CodeEditor);
    if (d->breakpoints.remove(lineNumber)) {
        emit BreakpointRemoved(lineNumber);
        emit BreakpointToggled(lineNumber, false);
        d->lineNumberArea->update(); // 刷新行号区域
    }
}

void CodeEditor::ClearAllBreakpoints()
{
    Q_D(CodeEditor);
    QSet<int> oldBreakpoints = d->breakpoints;
    d->breakpoints.clear();
    for (int lineNumber : oldBreakpoints) {
        emit BreakpointRemoved(lineNumber);
    }
    d->lineNumberArea->update();
}

bool CodeEditor::HasBreakpoint(int lineNumber) const
{
    Q_D(const CodeEditor);
    return d->breakpoints.contains(lineNumber);
}

QSet<int> CodeEditor::GetBreakpoints() const
{
    Q_D(const CodeEditor);
    return d->breakpoints;
}

// ============================================================================
// 调试箭头管理
// ============================================================================

void CodeEditor::SetCurrentDebugLine(int lineNumber)
{
    Q_D(CodeEditor);
    d->currentDebugLine = lineNumber;
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
        QTextBlock block       = codeEditor->firstVisibleBlock();
        int        blockNumber = block.blockNumber();
        int        top = qRound(codeEditor->blockBoundingGeometry(block).translated(codeEditor->contentOffset()).top());
        int        bottom = top + qRound(codeEditor->blockBoundingRect(block).height());

        while (block.isValid()) {
            if (top <= event->pos().y() && event->pos().y() < bottom) {
                int lineNumber = blockNumber + 1; // 行号从1开始
                codeEditor->ToggleBreakpoint(lineNumber);
                break;
            }

            block  = block.next();
            top    = bottom;
            bottom = top + qRound(codeEditor->blockBoundingRect(block).height());
            ++blockNumber;
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

    // 命令格式 - 紫色 #c586c0 (/cast /fcast /scast /sfcast /switch等)
    commandFormat.setForeground(QColor(197, 134, 192));
    QStringList commandPatterns = { "/cast\\b",       "/fcast\\b",      "/scast\\b",         "/sfcast\\b",
                                    "/switch\\b", // 切换宏命令
                                    "/add_target\\b", "/set_target\\b", "/change_target\\b", "/add_buff\\b",
                                    "/clear_buff\\b", "/stop\\b",       "/continue\\b",      "/end\\b" };
    for (const QString &pattern : commandPatterns) {
        rule.pattern = QRegularExpression(pattern);
        rule.format  = commandFormat;
        highlightingRules.append(rule);
    }

    // macro 关键字 - 橙色 #ce9178 (用于宏名称标识)
    macroFormat.setForeground(QColor(206, 145, 120));
    rule.pattern = QRegularExpression("\\bmacro\\b");
    rule.format  = macroFormat;
    highlightingRules.append(rule);

    // 时间格式 - 浅绿色 #b5cea8 (例如 00:10.5)
    numberFormat.setForeground(QColor(181, 206, 168));
    rule.pattern = QRegularExpression("\\b[0-9]+:[0-9]+\\.[0-9]+\\b");
    rule.format  = numberFormat;
    highlightingRules.append(rule);

    // 数字 - 浅绿色 #b5cea8
    rule.pattern = QRegularExpression("\\b[0-9]+\\.?[0-9]*\\b");
    rule.format  = numberFormat;
    highlightingRules.append(rule);

    // 条件关键字 - 蓝紫色 #569cd6
    conditionFormat.setForeground(QColor(86, 156, 214));

    // 带冒号的关键字
    QStringList colonKeywords = {
        "buff",           "nobuff",       "bufftime", "qidian",  "energy",     "sun",      "moon",     "sun_power",
        "moon_power",     "skill_energy", "skill",    "noskill", "last_skill", "skill_cd", "npclevel", "nearby_enemy",
        "skill_notin_cd", "yaoxing",      "tbuff",    "tnobuff", "tbufftime",  "Duff",     "Buff",     "ebufftime"
    };
    for (const QString &keyword : colonKeywords) {
        rule.pattern = QRegularExpression("\\b" + keyword + "(?=\\s*:)");
        rule.format  = conditionFormat;
        highlightingRules.append(rule);
    }

    // 带比较运算符的关键字 (>, <, >=, <=, =)
    QStringList comparisonKeywords = { "qidian", "life", "mana", "rage", "tlife", "tmana", "trage", "nearby_enemy" };
    for (const QString &keyword : comparisonKeywords) {
        rule.pattern = QRegularExpression("\\b" + keyword + "(?=\\s*[><=])");
        rule.format  = conditionFormat;
        highlightingRules.append(rule);
    }

    // 带等号的关键字（macro 用于宏名称，name 用于条件判断）
    QStringList equalKeywords = { "id", "name", "stack_num", "duration", "distance", "shield", "level" };
    for (const QString &keyword : equalKeywords) {
        rule.pattern = QRegularExpression("\\b" + keyword + "(?=\\s*=)");
        rule.format  = conditionFormat;
        highlightingRules.append(rule);
    }

    // 独立关键字
    rule.pattern = QRegularExpression("\\bdead\\b");
    rule.format  = conditionFormat;
    highlightingRules.append(rule);

    // 操作符 - 橙色 #d4d4d4
    operatorFormat.setForeground(QColor(212, 212, 212));
    rule.pattern = QRegularExpression("[\\[\\]&|=><~:]");
    rule.format  = operatorFormat;
    highlightingRules.append(rule);

    // 技能名（中文） - VSCode函数标准黄 #dcdcaa
    skillNameFormat.setForeground(QColor(220, 220, 170));
    rule.pattern = QRegularExpression("[\\x{4e00}-\\x{9fa5}]+"); // 匹配中文字符
    rule.format  = skillNameFormat;
    highlightingRules.append(rule);

    // 注释格式 - 绿色 #6a9955 (必须最后添加以覆盖其他规则)
    commentFormat.setForeground(QColor(106, 153, 85));
}

void JX3MacroSyntaxHighlighter::highlightBlock(const QString &text)
{
    // 应用除注释外的所有规则
    for (const HighlightingRule &rule : highlightingRules) {
        QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
        while (matchIterator.hasNext()) {
            QRegularExpressionMatch match = matchIterator.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }

    // 最后处理注释，覆盖所有其他格式
    QRegularExpression              commentPattern("(//|##?)[^\n]*");
    QRegularExpressionMatchIterator commentIterator = commentPattern.globalMatch(text);
    while (commentIterator.hasNext()) {
        QRegularExpressionMatch match = commentIterator.next();
        setFormat(match.capturedStart(), match.capturedLength(), commentFormat);
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

void CodeEditor::CheckSyntax()
{
    Q_D(CodeEditor);

    if (!d->syntaxCheckEnabled || !d->syntaxChecker) {
        return;
    }

    // 执行语法检测
    d->syntaxErrors = d->syntaxChecker->Check(document());

    // 刷新高亮显示
    HighlightCurrentLine();
}

// ============================================================================
// JX3MacroSyntaxChecker Implementation
// ============================================================================

JX3MacroSyntaxChecker::JX3MacroSyntaxChecker(QObject *parent) : SyntaxChecker(parent) { }

QList<SyntaxError> JX3MacroSyntaxChecker::Check(QTextDocument *document)
{
    QList<SyntaxError> errors;

    for (QTextBlock block = document->begin(); block.isValid(); block = block.next()) {
        QString line       = block.text();
        int     lineNumber = block.blockNumber() + 1;

        // 跳过空行和注释
        QString trimmed = line.trimmed();
        if (trimmed.isEmpty() || trimmed.startsWith('#') || trimmed.startsWith("//")) {
            continue;
        }

        // 检测方括号匹配（优先级最高）
        CheckBrackets(line, lineNumber, errors);

        // 检测 macro 定义行
        if (trimmed.startsWith("macro ")) {
            CheckMacroLine(line, lineNumber, errors);
        }
        // 检测命令行
        else if (trimmed.startsWith('/'))
        {
            CheckCommandLine(line, lineNumber, errors);
        }

        // 检测多余空格（所有行）
        CheckExtraSpaces(line, lineNumber, errors);
    }

    return errors;
}

void JX3MacroSyntaxChecker::CheckCommandLine(const QString &line, int lineNumber, QList<SyntaxError> &errors)
{
    QString trimmed = line.trimmed();

    // 定义合法命令列表
    QStringList validCommands = { "/cast",       "/fcast",         "/scast",    "/sfcast",     "/switch", "/add_target",
                                  "/set_target", "/change_target", "/add_buff", "/clear_buff", "/stop",   "/continue",
                                  "/end" };

    // 提取命令（第一个单词）
    int     spaceIndex = trimmed.indexOf(' ');
    QString command    = (spaceIndex > 0) ? trimmed.left(spaceIndex) : trimmed;

    // 检查命令是否合法
    if (!validCommands.contains(command)) {
        int column = line.indexOf(command);
        errors.append(SyntaxError(lineNumber, column, command.length(), SyntaxError::Error, QString("未知命令: %1").arg(command)));
        return;
    }

    // 检查 cast 命令是否有技能参数
    if ((command == "/cast" || command == "/fcast" || command == "/scast" || command == "/sfcast")) {
        QString afterCommand = trimmed.mid(command.length()).trimmed();

        // 检查是否有条件表达式（方括号）
        int bracketStart = static_cast<int>(afterCommand.indexOf('['));
        int bracketEnd   = static_cast<int>(afterCommand.indexOf(']'));

        QString skillPart;
        if (bracketStart >= 0 && bracketEnd > bracketStart) {
            // 有条件表达式：技能名可能在方括号前或后
            QString beforeBracket = afterCommand.left(bracketStart).trimmed();
            QString afterBracket  = afterCommand.mid(bracketEnd + 1).trimmed();

            // 技能名在方括号前或后都可以
            if (!beforeBracket.isEmpty()) {
                skillPart = beforeBracket;
            } else if (!afterBracket.isEmpty()) {
                skillPart = afterBracket;
            }
        } else {
            // 没有条件表达式
            skillPart = afterCommand;
        }

        if (skillPart.isEmpty()) {
            int column = static_cast<int>(line.indexOf(command)) + command.length();
            errors.append(SyntaxError(lineNumber, column, 1, SyntaxError::Error, QString("%1 命令缺少技能名称").arg(command)));
        }
    }

    // 检测条件表达式
    CheckConditions(line, lineNumber, errors);
}

void JX3MacroSyntaxChecker::CheckMacroLine(const QString &line, int lineNumber, QList<SyntaxError> &errors)
{
    QString trimmed = line.trimmed();

    // macro 定义格式: macro 宏名称
    QRegularExpression      macroPattern("^macro\\s+(\\S+)(.*)$");
    QRegularExpressionMatch match = macroPattern.match(trimmed);

    if (!match.hasMatch()) {
        int column = line.indexOf("macro");
        errors.append(SyntaxError(lineNumber, column, 5, SyntaxError::Error, "macro 定义格式错误，正确格式: macro 宏名称"));
        return;
    }

    // 检查是否有多余内容
    QString extraContent = match.captured(2).trimmed();
    if (!extraContent.isEmpty()) {
        int column = line.indexOf(extraContent);
        errors.append(SyntaxError(lineNumber, column, extraContent.length(), SyntaxError::Warning, "macro 定义后有多余内容"));
    }
}

void JX3MacroSyntaxChecker::CheckConditions(const QString &line, int lineNumber, QList<SyntaxError> &errors)
{
    // 查找方括号内的条件表达式
    QRegularExpression              bracketPattern("\\[([^\\]]*)\\]");
    QRegularExpressionMatchIterator it = bracketPattern.globalMatch(line);

    while (it.hasNext()) {
        QRegularExpressionMatch match      = it.next();
        QString                 conditions = match.captured(1);

        // 检查条件是否为空
        if (conditions.trimmed().isEmpty()) {
            int column = match.capturedStart();
            errors.append(SyntaxError(lineNumber, column, match.capturedLength(), SyntaxError::Warning, "条件表达式为空"));
            continue;
        }

        // 检查中括号内是否包含空格（不允许）
        if (conditions.contains(' ')) {
            int spacePos = static_cast<int>(conditions.indexOf(' '));
            int column   = static_cast<int>(match.capturedStart()) + 1 + spacePos; // +1 跳过左括号
            errors.append(SyntaxError(lineNumber, column, 1, SyntaxError::Error, "条件表达式中不允许有空格"));
        }

        // 分割多个条件（用 & 或 | 分隔）
        QStringList condList = conditions.split(QRegularExpression("[&|]"));

        for (const QString &cond : condList) {
            if (cond.isEmpty()) {
                continue;
            }

            // 定义合法的条件关键字
            QStringList validKeywords = {
                "buff",         "nobuff",       "bufftime", "tbuff",   "tnobuff",    "tbufftime", "Buff",
                "Duff",         "ebufftime",    "qidian",   "energy",  "sun",        "moon",      "sun_power",
                "moon_power",   "skill_energy", "skill",    "noskill", "last_skill", "skill_cd",  "skill_notin_cd",
                "life",         "mana",         "rage",     "tlife",   "tmana",      "trage",     "npclevel",
                "nearby_enemy", "yaoxing",      "dead",     "id",      "name",       "stack_num", "duration",
                "distance",     "shield",       "level"
            };

            // 提取条件关键字（冒号或运算符之前的部分）
            QRegularExpression      keywordPattern("^(\\w+)[:><=~]");
            QRegularExpressionMatch kwMatch = keywordPattern.match(cond);

            if (kwMatch.hasMatch()) {
                QString keyword = kwMatch.captured(1);
                if (!validKeywords.contains(keyword)) {
                    int column = line.indexOf(keyword, match.capturedStart());
                    if (column >= 0) {
                        errors.append(SyntaxError(lineNumber,
                                                  column,
                                                  keyword.length(),
                                                  SyntaxError::Warning,
                                                  QString("未知条件关键字: %1").arg(keyword)));
                    }
                }
            }
        }
    }
}

void JX3MacroSyntaxChecker::CheckExtraSpaces(const QString &line, int lineNumber, QList<SyntaxError> &errors)
{
    // 检测行尾空格
    if (line.endsWith(' ') || line.endsWith('\t')) {
        int column = line.length() - 1;
        while (column > 0 && (line[column] == ' ' || line[column] == '\t')) {
            column--;
        }
        column++; // 指向第一个尾部空格

        errors.append(SyntaxError(lineNumber, column, line.length() - column, SyntaxError::Warning, "行尾有多余空格"));
    }

    // 检测多个连续空格（排除缩进）
    QString trimmed = line.trimmed();
    if (!trimmed.isEmpty()) {
        int     indentEnd = line.indexOf(trimmed);
        QString content   = line.mid(indentEnd);

        QRegularExpression              multiSpacePattern("  +"); // 2个或以上连续空格
        QRegularExpressionMatchIterator it = multiSpacePattern.globalMatch(content);

        while (it.hasNext()) {
            QRegularExpressionMatch match  = it.next();
            int                     column = indentEnd + match.capturedStart();
            errors.append(SyntaxError(lineNumber, column, match.capturedLength(), SyntaxError::Warning, "有多个连续空格"));
        }
    }
}

void JX3MacroSyntaxChecker::CheckBrackets(const QString &line, int lineNumber, QList<SyntaxError> &errors)
{
    int depth         = 0;
    int lastOpenIndex = -1;

    for (int i = 0; i < line.length(); ++i) {
        if (line[i] == '[') {
            depth++;
            lastOpenIndex = i;
        } else if (line[i] == ']') {
            depth--;
            if (depth < 0) {
                // 多余的右括号
                errors.append(SyntaxError(lineNumber, i, 1, SyntaxError::Error, "多余的右方括号 ']'"));
                return;
            }
        }
    }

    // 缺少右括号
    if (depth > 0 && lastOpenIndex >= 0) {
        errors.append(SyntaxError(lineNumber, lastOpenIndex, 1, SyntaxError::Error, "缺少右方括号 ']'"));
    }
}

// ============================================================================
// 代码格式化
// ============================================================================

void CodeEditor::FormatDocument()
{
    Q_D(CodeEditor);

    QString     text  = toPlainText();
    QStringList lines = text.split('\n');
    QStringList formattedLines;

    int     indentLevel = 0;
    QString indentString(d->tabWidth, ' ');

    for (const QString &line : lines) {
        QString trimmed = line.trimmed();

        // 跳过空行和注释行
        if (trimmed.isEmpty() || trimmed.startsWith('#') || trimmed.startsWith("//")) {
            formattedLines.append(trimmed);
            continue;
        }

        // 处理缩进
        QString formatted;

        // 如果以 macro 开头，不缩进
        if (trimmed.startsWith("macro ")) {
            indentLevel = 0;
            formatted   = FormatMacroLine(trimmed);
        }
        // 如果以命令开头，缩进一级并格式化
        else if (trimmed.startsWith('/'))
        {
            indentLevel = 1;
            formatted   = indentString + FormatCommandLine(trimmed);
        }
        // 时间格式（事件语句）
        else if (QRegularExpression("^\\d+:\\d+\\.\\d+").match(trimmed).hasMatch())
        {
            indentLevel = 0;
            formatted   = trimmed;
        }
        // 其他情况保持当前缩进
        else {
            formatted = QString(indentLevel * d->tabWidth, ' ') + trimmed;
        }

        formattedLines.append(formatted);
    }

    // 更新文档内容
    QTextCursor cursor = textCursor();
    cursor.beginEditBlock();
    cursor.select(QTextCursor::Document);
    cursor.insertText(formattedLines.join('\n'));
    cursor.endEditBlock();
}

QString CodeEditor::FormatCommandLine(const QString &line)
{
    QString trimmed = line.trimmed();

    // 提取命令部分（第一个单词）
    int firstSpace = static_cast<int>(trimmed.indexOf(' '));
    if (firstSpace < 0) {
        return trimmed; // 只有命令，没有参数
    }

    QString command      = trimmed.left(firstSpace);
    QString afterCommand = trimmed.mid(firstSpace + 1);

    // 查找条件表达式（方括号）
    int bracketStart = static_cast<int>(afterCommand.indexOf('['));

    if (bracketStart < 0) {
        // 没有条件表达式，只规范化空格：命令 参数
        return command + " " + afterCommand.trimmed();
    }

    // 有条件表达式
    QString beforeBracket = afterCommand.left(bracketStart).trimmed();
    QString fromBracket   = afterCommand.mid(bracketStart);

    // 找到右括号位置
    int bracketEnd = static_cast<int>(fromBracket.indexOf(']'));
    if (bracketEnd < 0) {
        // 没有右括号，返回原样（会被语法检测捕获）
        return command + " " + afterCommand.trimmed();
    }

    // 提取条件表达式和括号后的内容
    QString bracketContent = fromBracket.left(bracketEnd + 1);
    QString afterBracket   = fromBracket.mid(bracketEnd + 1).trimmed();

    // 格式化条件表达式（移除内部空格）
    QString formattedCondition = FormatConditionExpression(bracketContent);

    // 重组：/命令 [条件] 名称
    QStringList parts;
    parts.append(command);

    if (!beforeBracket.isEmpty()) {
        parts.append(beforeBracket); // 条件前的参数
    }

    parts.append(formattedCondition); // 条件

    if (!afterBracket.isEmpty()) {
        parts.append(afterBracket); // 条件后的参数
    }

    return parts.join(" ");
}

QString CodeEditor::FormatConditionExpression(const QString &expr)
{
    QString result = expr;

    // 查找所有方括号对
    QRegularExpression              bracketPattern(R"(\[([^\]]*)\])");
    QRegularExpressionMatchIterator it = bracketPattern.globalMatch(expr);

    // 从后向前替换（避免位置偏移）
    QList<QRegularExpressionMatch> matches;
    while (it.hasNext()) {
        matches.append(it.next());
    }

    for (int i = static_cast<int>(matches.size()) - 1; i >= 0; --i) {
        const QRegularExpressionMatch &match      = matches[i];
        QString                        conditions = match.captured(1);

        // 移除条件内的所有空格
        QString noSpaces = conditions;
        (void)noSpaces.remove(' ');

        // 替换原始表达式
        result.replace(static_cast<int>(match.capturedStart()), static_cast<int>(match.capturedLength()), "[" + noSpaces + "]");
    }

    return result;
}

QString CodeEditor::FormatMacroLine(const QString &line)
{
    QString trimmed = line.trimmed();

    // macro 定义格式: macro 宏名称
    QRegularExpression      macroPattern("^macro\\s+(\\S+)");
    QRegularExpressionMatch match = macroPattern.match(trimmed);

    if (match.hasMatch()) {
        return "macro " + match.captured(1);
    }

    return trimmed;
}
