#include "code_editor.h"

#include <QPainter>
#include <QTextBlock>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QRegularExpression>
#include "resources.h"

class CodeEditorPrivate
{
public:
    explicit CodeEditorPrivate(CodeEditor *q) :
        lineNumberArea(new LineNumberArea(q)),
        tabWidth(4),
        showLineNumbers(true),
        currentLineColor(QColor(Qt::yellow).lighter(160)),
        syntaxHighlightingEnabled(true),
        syntaxType(CodeEditor::Cpp),
        cppHighlighter(nullptr),
        jx3MacroHighlighter(nullptr),
        currentDebugLine(-1),
        q_ptr(q)
    {
    }

    LineNumberArea              *lineNumberArea;
    int                          tabWidth;
    bool                         showLineNumbers;
    QColor                       currentLineColor;
    bool                         syntaxHighlightingEnabled;
    CodeEditor::SyntaxType       syntaxType;
    QScopedPointer<CppSyntaxHighlighter>      cppHighlighter;
    QScopedPointer<JX3MacroSyntaxHighlighter> jx3MacroHighlighter;
    QSet<int>                    breakpoints;       // 断点集合（行号从1开始）
    int                          currentDebugLine;  // 当前调试行（-1表示无）

private:
    CodeEditor *q_ptr;
    Q_DECLARE_PUBLIC(CodeEditor)
};

// ============================================================================
// CodeEditor Implementation
// ============================================================================

CodeEditor::CodeEditor(QWidget *parent) :
    QPlainTextEdit(parent),
    d_ptr(new CodeEditorPrivate(this))
{
    Q_D(CodeEditor);

    // 连接信号
    connect(this, &CodeEditor::blockCountChanged, this, &CodeEditor::UpdateLineNumberAreaWidth);
    connect(this, &CodeEditor::updateRequest, this, &CodeEditor::UpdateLineNumberArea);
    connect(this, &CodeEditor::cursorPositionChanged, this, &CodeEditor::HighlightCurrentLine);

    UpdateLineNumberAreaWidth(0);
    HighlightCurrentLine();

    // 设置等宽字体：英文 Monaco，中文霞鹜文楷等宽
    setFont(Resources::MonoFont());

    // 设置文档默认字体，确保中文字符能正确回退
    document()->setDefaultFont(Resources::MonoFont());

    // VSCode 深色主题配色
    QPalette palette = this->palette();
    palette.setColor(QPalette::Base, QColor(30, 30, 30));           // 编辑器背景 #1e1e1e
    palette.setColor(QPalette::Text, QColor(212, 212, 212));        // 文本颜色 #d4d4d4
    palette.setColor(QPalette::Highlight, QColor(38, 79, 120));     // 选中背景 #264f78
    palette.setColor(QPalette::HighlightedText, QColor(255, 255, 255)); // 选中文本
    setPalette(palette);

    // 设置制表符宽度
    const int tabStop = 4;
    QFontMetrics metrics(Resources::MonoFont());
    setTabStopDistance(tabStop * metrics.horizontalAdvance(' '));

    // 设置当前行高亮颜色为 VSCode 风格
    d->currentLineColor = QColor(44, 44, 44); // #2c2c2c

    // 默认启用 C++ 语法高亮
    d->cppHighlighter.reset(new CppSyntaxHighlighter(document()));
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
    while (max >= 10) {
        max /= 10;
        ++digits;
    }

    // 为断点区域预留空间：左边距(6) + 圆圈(8) + 间距(4) + 行号 + 右边距(6)
    const int breakpointAreaWidth = 6 + 8 + 4;
    int space = breakpointAreaWidth + fontMetrics().horizontalAdvance(QLatin1Char('9')) * digits + 6;
    return space;
}

void CodeEditor::UpdateLineNumberAreaWidth(int /* newBlockCount */)
{
    setViewportMargins(LineNumberAreaWidth(), 0, 0, 0);
}

void CodeEditor::UpdateLineNumberArea(const QRect &rect, int dy)
{
    Q_D(CodeEditor);
    if (dy) {
        d->lineNumberArea->scroll(0, dy);
    } else {
        d->lineNumberArea->update(0, rect.y(), d->lineNumberArea->width(), rect.height());
    }

    if (rect.contains(viewport()->rect())) {
        UpdateLineNumberAreaWidth(0);
    }
}

void CodeEditor::resizeEvent(QResizeEvent *e)
{
    Q_D(CodeEditor);
    QPlainTextEdit::resizeEvent(e);

    QRect cr = contentsRect();
    d->lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), LineNumberAreaWidth(), cr.height()));
}

void CodeEditor::HighlightCurrentLine()
{
    Q_D(CodeEditor);
    QList<QTextEdit::ExtraSelection> extraSelections;

    // 优先绘制调试行高亮（黄色背景）
    if (d->currentDebugLine > 0) {
        QTextBlock debugBlock = document()->findBlockByLineNumber(d->currentDebugLine - 1);
        if (debugBlock.isValid()) {
            QTextEdit::ExtraSelection debugSelection;
            debugSelection.format.setBackground(QColor(255, 255, 0, 40)); // 浅黄色半透明 #ffff0028
            debugSelection.format.setProperty(QTextFormat::FullWidthSelection, true);
            debugSelection.cursor = QTextCursor(debugBlock);
            debugSelection.cursor.clearSelection();
            extraSelections.append(debugSelection);
        }
    }

    // 绘制当前行高亮（深灰色背景）
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
    const int breakpointSize = 8; // 断点圆圈直径（更小更精致）
    const int breakpointMargin = 6; // 断点左边距

    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            int lineNumber = blockNumber + 1; // 行号从1开始
            QString number = QString::number(lineNumber);

            // 绘制断点（如果有）- VSCode 风格的红色圆圈
            if (d->breakpoints.contains(lineNumber)) {
                painter.setPen(Qt::NoPen);
                painter.setBrush(QColor(224, 78, 78)); // VSCode 断点红色 #e04e4e

                // 在行号左侧绘制圆形断点，稍微向下偏移以对齐行号中心
                int centerY = top + fontMetrics().height() / 2 + 2;
                painter.drawEllipse(QPoint(breakpointMargin + breakpointSize / 2, centerY),
                                    breakpointSize / 2, breakpointSize / 2);
            }

            // VSCode 风格：当前行行号高亮，其他行号灰色
            if (blockNumber == currentBlockNumber) {
                painter.setPen(QColor(212, 212, 212)); // 当前行 #d4d4d4
            } else {
                painter.setPen(QColor(133, 133, 133)); // 其他行 #858585
            }

            // 绘制调试行高亮（如果是当前调试行）- VSCode 风格：黄色背景
            int textX = breakpointMargin + breakpointSize + 4; // 断点后留4px间距

            if (d->currentDebugLine == lineNumber) {
                // 1. 绘制黄色背景高亮条（覆盖整个行号区域）
                painter.setPen(Qt::NoPen);
                painter.setBrush(QColor(255, 204, 0)); // VSCode 黄色 #ffcc00
                painter.drawRect(0, top, d->lineNumberArea->width(), fontMetrics().height());

                // 2. 重新绘制行号（黑色，覆盖在黄色背景上）
                painter.setPen(QColor(0, 0, 0)); // 黑色行号
                painter.drawText(textX, top, d->lineNumberArea->width() - textX - 6,
                                 fontMetrics().height(), Qt::AlignRight, number);

                // 3. 如果有断点，重新绘制断点（黑色，覆盖在黄色背景上）
                if (d->breakpoints.contains(lineNumber)) {
                    painter.setPen(Qt::NoPen);
                    painter.setBrush(QColor(0, 0, 0)); // 黑色断点
                    int centerY = top + fontMetrics().height() / 2 + 2;
                    painter.drawEllipse(QPoint(breakpointMargin + breakpointSize / 2, centerY),
                                        breakpointSize / 2, breakpointSize / 2);
                }
            } else {
                // 正常绘制行号（无调试高亮时）
                painter.drawText(textX, top, d->lineNumberArea->width() - textX - 6,
                                 fontMetrics().height(), Qt::AlignRight, number);
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
        case Cpp:
            d->cppHighlighter.reset(new CppSyntaxHighlighter(document()));
            break;
        case JX3Macro:
            d->jx3MacroHighlighter.reset(new JX3MacroSyntaxHighlighter(document()));
            break;
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
        case Cpp:
            d->cppHighlighter.reset(new CppSyntaxHighlighter(document()));
            break;
        case JX3Macro:
            d->jx3MacroHighlighter.reset(new JX3MacroSyntaxHighlighter(document()));
            break;
        }
    }
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
    QStringList controlKeywords = {
        "\\bif\\b", "\\belse\\b", "\\bfor\\b", "\\bwhile\\b", "\\bdo\\b",
        "\\bswitch\\b", "\\bcase\\b", "\\bdefault\\b", "\\bbreak\\b", "\\bcontinue\\b",
        "\\breturn\\b", "\\btry\\b", "\\bcatch\\b", "\\bthrow\\b", "\\bgoto\\b"
    };
    for (const QString &pattern : controlKeywords) {
        rule.pattern = QRegularExpression(pattern);
        rule.format  = keywordFormat;
        highlightingRules.append(rule);
    }

    // 类型关键字 - 蓝紫色 #569cd6
    QStringList typeKeywords = {
        "\\bchar\\b", "\\bint\\b", "\\bshort\\b", "\\blong\\b", "\\bfloat\\b", "\\bdouble\\b",
        "\\bbool\\b", "\\bvoid\\b", "\\bsigned\\b", "\\bunsigned\\b", "\\bauto\\b",
        "\\bconst\\b", "\\bvolatile\\b", "\\bextern\\b", "\\bstatic\\b", "\\bregister\\b",
        "\\bmutable\\b", "\\bconstexpr\\b", "\\bdecltype\\b", "\\btypename\\b"
    };
    for (const QString &pattern : typeKeywords) {
        rule.pattern = QRegularExpression(pattern);
        rule.format  = keywordFormat;
        highlightingRules.append(rule);
    }

    // 修饰符关键字 - 蓝紫色 #569cd6
    QStringList modifierKeywords = {
        "\\bclass\\b", "\\bstruct\\b", "\\bunion\\b", "\\benum\\b", "\\bnamespace\\b",
        "\\btemplate\\b", "\\btypedef\\b", "\\busing\\b", "\\bpublic\\b", "\\bprivate\\b",
        "\\bprotected\\b", "\\bvirtual\\b", "\\boverride\\b", "\\bfinal\\b", "\\bexplicit\\b",
        "\\bfriend\\b", "\\binline\\b", "\\boperator\\b", "\\bnoexcept\\b",
        "\\bstatic_cast\\b", "\\bdynamic_cast\\b", "\\breinterpret_cast\\b", "\\bconst_cast\\b"
    };
    for (const QString &pattern : modifierKeywords) {
        rule.pattern = QRegularExpression(pattern);
        rule.format  = keywordFormat;
        highlightingRules.append(rule);
    }

    // new, delete, this, nullptr - 蓝紫色
    QStringList specialKeywords = {"\\bnew\\b", "\\bdelete\\b", "\\bthis\\b", "\\bnullptr\\b",
                                   "\\bsizeof\\b", "\\btypeid\\b", "\\bsignals\\b", "\\bslots\\b"};
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
        QRegularExpressionMatch match        = commentEndExpression.match(text, startIndex);
        int                     endIndex     = match.capturedStart();
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
        QTextBlock block = codeEditor->firstVisibleBlock();
        int blockNumber = block.blockNumber();
        int top = qRound(codeEditor->blockBoundingGeometry(block)
                         .translated(codeEditor->contentOffset()).top());
        int bottom = top + qRound(codeEditor->blockBoundingRect(block).height());

        while (block.isValid()) {
            if (top <= event->pos().y() && event->pos().y() < bottom) {
                int lineNumber = blockNumber + 1; // 行号从1开始
                codeEditor->ToggleBreakpoint(lineNumber);
                break;
            }

            block = block.next();
            top = bottom;
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
    QStringList commandPatterns = {
        "/cast\\b", "/fcast\\b", "/scast\\b", "/sfcast\\b",
        "/switch\\b",  // 切换宏命令
        "/add_target\\b", "/set_target\\b", "/change_target\\b",
        "/add_buff\\b", "/clear_buff\\b",
        "/stop\\b", "/continue\\b", "/end\\b"
    };
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
        "buff", "nobuff", "bufftime",
        "qidian", "energy", "sun", "moon", "sun_power", "moon_power",
        "skill_energy", "skill", "noskill", "last_skill", "skill_cd",
        "npclevel", "nearby_enemy", "skill_notin_cd", "yaoxing",
        "tbuff", "tnobuff", "tbufftime",
        "Duff", "Buff", "ebufftime"
    };
    for (const QString &keyword : colonKeywords) {
        rule.pattern = QRegularExpression("\\b" + keyword + "(?=\\s*:)");
        rule.format  = conditionFormat;
        highlightingRules.append(rule);
    }

    // 带比较运算符的关键字 (>, <, >=, <=, =)
    QStringList comparisonKeywords = {"qidian", "life", "mana", "rage", "tlife", "tmana", "trage", "nearby_enemy"};
    for (const QString &keyword : comparisonKeywords) {
        rule.pattern = QRegularExpression("\\b" + keyword + "(?=\\s*[><=])");
        rule.format  = conditionFormat;
        highlightingRules.append(rule);
    }

    // 带等号的关键字（macro 用于宏名称，name 用于条件判断）
    QStringList equalKeywords = {"id", "name", "stack_num", "duration", "distance", "shield", "level"};
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
    QRegularExpression commentPattern("(//|##?)[^\n]*");
    QRegularExpressionMatchIterator commentIterator = commentPattern.globalMatch(text);
    while (commentIterator.hasNext()) {
        QRegularExpressionMatch match = commentIterator.next();
        setFormat(match.capturedStart(), match.capturedLength(), commentFormat);
    }
}
