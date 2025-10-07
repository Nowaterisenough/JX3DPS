#ifndef CODE_EDITOR_H
#define CODE_EDITOR_H

#include <QPlainTextEdit>
#include <QSyntaxHighlighter>
#include <QRegularExpression>
#include <QProxyStyle>

class CodeEditorPrivate;
class LineNumberArea;
class SyntaxChecker;

/**
 * @brief 语法错误信息
 */
struct SyntaxError
{
    enum Severity {
        Warning,    // 警告（黄色波浪线）
        Error       // 错误（红色波浪线）
    };

    int      line;          // 行号（从1开始）
    int      column;        // 列号（从0开始）
    int      length;        // 错误范围长度
    Severity severity;      // 严重程度
    QString  message;       // 错误消息

    SyntaxError(int l, int c, int len, Severity sev, const QString &msg)
        : line(l), column(c), length(len), severity(sev), message(msg) {}
};

/**
 * @brief 自定义菜单样式代理
 */
class MenuStyle : public QProxyStyle
{
public:
    explicit MenuStyle(QStyle *style = nullptr);
    void drawControl(ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const override;
    void drawPrimitive(PrimitiveElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const override;
    QSize sizeFromContents(ContentsType type, const QStyleOption *option, const QSize &size, const QWidget *widget) const override;
    int pixelMetric(PixelMetric metric, const QStyleOption *option, const QWidget *widget) const override;
};

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

    // 代码格式化
    void         FormatDocument();                              // 格式化整个文档

    // 语法检测
    void         SetSyntaxCheckEnabled(bool enable);            // 启用/禁用语法检测
    bool         IsSyntaxCheckEnabled() const;                  // 是否启用语法检测
    QList<SyntaxError> GetSyntaxErrors() const;                 // 获取当前语法错误列表

signals:
    void BreakpointAdded(int lineNumber);                       // 断点添加信号
    void BreakpointRemoved(int lineNumber);                     // 断点移除信号
    void BreakpointToggled(int lineNumber, bool enabled);       // 断点切换信号

protected:
    void resizeEvent(QResizeEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void contextMenuEvent(QContextMenuEvent *event) override;
    bool event(QEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private slots:
    void UpdateLineNumberAreaWidth(int newBlockCount);
    void HighlightCurrentLine();
    void UpdateLineNumberArea(const QRect &rect, int dy);
    void CheckSyntax();                                         // 执行语法检测

private:
    friend class LineNumberArea;
    QScopedPointer<CodeEditorPrivate> d_ptr;
    Q_DECLARE_PRIVATE(CodeEditor)

    // 格式化辅助函数
    static QString FormatCommandLine(const QString &line);               // 格式化命令行
    static QString FormatConditionExpression(const QString &expr);       // 格式化条件表达式
    static QString FormatMacroLine(const QString &line);                 // 格式化宏定义行
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
    QTextCharFormat macroFormat;        // macro 关键字格式（宏名称标识）
    QTextCharFormat operatorFormat;     // 操作符 = > < & |
    QTextCharFormat numberFormat;       // 数字
    QTextCharFormat commentFormat;      // 注释
};

/**
 * @brief 语法检测器基类
 */
class SyntaxChecker : public QObject
{
    Q_OBJECT

public:
    explicit SyntaxChecker(QObject *parent = nullptr) : QObject(parent) {}
    virtual ~SyntaxChecker() = default;

    /**
     * @brief 检测文档中的语法错误
     * @param document 要检测的文档
     * @return 语法错误列表
     */
    virtual QList<SyntaxError> Check(QTextDocument *document) = 0;
};

/**
 * @brief JX3宏语法检测器
 *
 * 检测规则：
 * 1. 命令格式错误（/cast, /fcast 等）
 * 2. 条件语法错误（buff:, nobuff: 等）
 * 3. 多余的空格
 * 4. 缺失的参数
 * 5. 不匹配的方括号
 */
class JX3MacroSyntaxChecker : public SyntaxChecker
{
    Q_OBJECT

public:
    explicit JX3MacroSyntaxChecker(QObject *parent = nullptr);
    QList<SyntaxError> Check(QTextDocument *document) override;

private:
    // 检测单行命令
    void CheckCommandLine(const QString &line, int lineNumber, QList<SyntaxError> &errors);
    // 检测宏定义行
    void CheckMacroLine(const QString &line, int lineNumber, QList<SyntaxError> &errors);
    // 检测条件表达式
    void CheckConditions(const QString &line, int lineNumber, QList<SyntaxError> &errors);
    // 检测多余空格
    void CheckExtraSpaces(const QString &line, int lineNumber, QList<SyntaxError> &errors);
    // 检测方括号匹配
    void CheckBrackets(const QString &line, int lineNumber, QList<SyntaxError> &errors);
};

#endif // CODE_EDITOR_H
