#ifndef CODE_EDITOR_H
#define CODE_EDITOR_H

#include <QPlainTextEdit>
#include <QSyntaxHighlighter>
#include <QRegularExpression>

class CodeEditorPrivate;
class LineNumberArea;

/**
 * @brief 代码编辑器控件
 *
 * 功能特性：
 * - 行号显示
 * - 当前行高亮
 * - 语法高亮（C/C++）
 * - 自动缩进
 * - 断点管理（VSCode 风格）
 * - 制表符支持
 */
class CodeEditor : public QPlainTextEdit
{
    Q_OBJECT

public:
    explicit CodeEditor(QWidget *parent = nullptr);
    ~CodeEditor() override;

    // 行号区域相关
    void LineNumberAreaPaintEvent(QPaintEvent *event);
    int  LineNumberAreaWidth();

    // 设置/获取制表符宽度（以空格数计算）
    void SetTabWidth(int spaces);
    int  TabWidth() const;

    // 设置/获取是否显示行号
    void SetShowLineNumbers(bool show);
    bool ShowLineNumbers() const;

    // 设置/获取当前行高亮颜色
    void   SetCurrentLineColor(const QColor &color);
    QColor CurrentLineColor() const;

    // 设置/获取是否启用语法高亮
    void SetSyntaxHighlighting(bool enable);
    bool SyntaxHighlighting() const;

    // 语法高亮类型
    enum SyntaxType {
        Cpp,        // C/C++
        JX3Macro    // 剑三宏
    };

    // 设置语法高亮类型
    void SetSyntaxType(SyntaxType type);
    SyntaxType GetSyntaxType() const;

    // 断点管理
    void         ToggleBreakpoint(int lineNumber);              // 切换断点
    void         SetBreakpoint(int lineNumber, bool enabled);   // 设置断点
    void         RemoveBreakpoint(int lineNumber);              // 移除断点
    void         ClearAllBreakpoints();                         // 清除所有断点
    bool         HasBreakpoint(int lineNumber) const;           // 检查是否有断点
    QSet<int>    GetBreakpoints() const;                        // 获取所有断点行号

    // 调试箭头管理（指示当前执行行）
    void         SetCurrentDebugLine(int lineNumber);           // 设置当前调试行
    void         ClearCurrentDebugLine();                       // 清除当前调试行
    int          GetCurrentDebugLine() const;                   // 获取当前调试行（-1表示无）

signals:
    void BreakpointAdded(int lineNumber);                       // 断点添加信号
    void BreakpointRemoved(int lineNumber);                     // 断点移除信号
    void BreakpointToggled(int lineNumber, bool enabled);       // 断点切换信号

protected:
    void resizeEvent(QResizeEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void UpdateLineNumberAreaWidth(int newBlockCount);
    void HighlightCurrentLine();
    void UpdateLineNumberArea(const QRect &rect, int dy);

private:
    friend class LineNumberArea;
    QScopedPointer<CodeEditorPrivate> d_ptr;
    Q_DECLARE_PRIVATE(CodeEditor)
};

/**
 * @brief 行号区域控件
 */
class LineNumberArea : public QWidget
{
public:
    explicit LineNumberArea(CodeEditor *editor) : QWidget(editor), codeEditor(editor)
    {
        setCursor(Qt::PointingHandCursor); // 设置鼠标指针为手型
    }

    QSize sizeHint() const override { return QSize(codeEditor->LineNumberAreaWidth(), 0); }

protected:
    void paintEvent(QPaintEvent *event) override { codeEditor->LineNumberAreaPaintEvent(event); }
    void mousePressEvent(QMouseEvent *event) override;

private:
    CodeEditor *codeEditor;
};

/**
 * @brief C/C++ 语法高亮器
 */
class CppSyntaxHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT

public:
    explicit CppSyntaxHighlighter(QTextDocument *parent = nullptr);

protected:
    void highlightBlock(const QString &text) override;

private:
    struct HighlightingRule
    {
        QRegularExpression pattern;
        QTextCharFormat    format;
    };
    QVector<HighlightingRule> highlightingRules;

    QRegularExpression commentStartExpression;
    QRegularExpression commentEndExpression;

    QTextCharFormat keywordFormat;
    QTextCharFormat classFormat;
    QTextCharFormat singleLineCommentFormat;
    QTextCharFormat multiLineCommentFormat;
    QTextCharFormat quotationFormat;
    QTextCharFormat functionFormat;
    QTextCharFormat numberFormat;
    QTextCharFormat preprocessorFormat;
};

/**
 * @brief 剑三宏语法高亮器
 */
class JX3MacroSyntaxHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT

public:
    explicit JX3MacroSyntaxHighlighter(QTextDocument *parent = nullptr);

protected:
    void highlightBlock(const QString &text) override;

private:
    struct HighlightingRule
    {
        QRegularExpression pattern;
        QTextCharFormat    format;
    };
    QVector<HighlightingRule> highlightingRules;

    QTextCharFormat commandFormat;      // 命令格式 /cast /fcast /switch
    QTextCharFormat conditionFormat;    // 条件格式 buff nobuff
    QTextCharFormat skillNameFormat;    // 技能名格式
    QTextCharFormat nameFormat;         // name 关键字格式
    QTextCharFormat operatorFormat;     // 操作符 = > < & |
    QTextCharFormat numberFormat;       // 数字
    QTextCharFormat commentFormat;      // 注释
};

#endif // CODE_EDITOR_H
