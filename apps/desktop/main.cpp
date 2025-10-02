#include <QApplication>
#include <QDebug>
#include <QLabel>
#include <QVBoxLayout>

#include "code_editor/code_editor.h"
#include "code_editor/debug_toolbar.h"
#include "controls/frameless/frameless.h"
#include "controls/theme/dark_style.h"

#include "resources.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // 启用高质量文本渲染（消除锯齿）
    QFont defaultFont = Resources::Font();
    defaultFont.setHintingPreference(QFont::PreferFullHinting);
    defaultFont.setStyleStrategy(QFont::PreferAntialias);

    Resources::InitResources();
    QApplication::setFont(defaultFont);

    // 设置深黑蓝色主题（包括 ToolTip 样式）
    QApplication::setStyle(new DarkStyle());

    Frameless w;
    w.SetupDefaultTitleBar();  // 使用默认的 titlebar
    w.setWindowTitle("JX3DPS - Code Editor");
    w.resize(1200, 800);

    // 获取中央控件
    QWidget *centralWidget = w.ContentWidget();

    // 创建调试工具栏（浮窗形式）
    DebugToolbar *debugToolbar = new DebugToolbar(&w);

    // 创建代码编辑器
    CodeEditor *editor = new CodeEditor(centralWidget);

    // 设置专用等宽字体（LXGWMono中文 + Monaco英文）
    editor->setFont(Resources::MonoFont());

    // 设置为剑三宏模式
    editor->SetSyntaxType(CodeEditor::JX3Macro);

    // 连接断点信号（添加断点时自动显示调试工具栏）
    QObject::connect(editor, &CodeEditor::BreakpointAdded, [debugToolbar](int lineNumber) {
        qDebug() << "Breakpoint added at line:" << lineNumber;
        debugToolbar->ShowFloating();  // 自动显示调试工具栏
    });
    QObject::connect(editor, &CodeEditor::BreakpointRemoved, [](int lineNumber) {
        qDebug() << "Breakpoint removed from line:" << lineNumber;
    });

    // 连接调试工具栏信号
    QObject::connect(debugToolbar, &DebugToolbar::ContinueClicked, [debugToolbar, editor]() {
        qDebug() << "Debug: Continue";
        debugToolbar->SetDebugState(DebugToolbar::Running);
        // 测试：显示调试箭头在第5行
        editor->SetCurrentDebugLine(5);
    });
    QObject::connect(debugToolbar, &DebugToolbar::PauseClicked, [debugToolbar, editor]() {
        qDebug() << "Debug: Pause";
        debugToolbar->SetDebugState(DebugToolbar::Paused);
        // 测试：显示调试箭头在第10行
        editor->SetCurrentDebugLine(10);
    });
    QObject::connect(debugToolbar, &DebugToolbar::StepOverClicked, [editor]() {
        qDebug() << "Debug: Step Over";
        // 测试：移动箭头到下一行
        int currentLine = editor->GetCurrentDebugLine();
        if (currentLine > 0) {
            editor->SetCurrentDebugLine(currentLine + 1);
        }
    });
    QObject::connect(debugToolbar, &DebugToolbar::StepIntoClicked, [editor]() {
        qDebug() << "Debug: Step Into";
        // 测试：显示箭头
        editor->SetCurrentDebugLine(3);
    });
    QObject::connect(debugToolbar, &DebugToolbar::StepOutClicked, [editor]() {
        qDebug() << "Debug: Step Out";
        // 测试：清除箭头
        editor->ClearCurrentDebugLine();
    });
    QObject::connect(debugToolbar, &DebugToolbar::RestartClicked, [debugToolbar]() {
        qDebug() << "Debug: Restart";
        debugToolbar->SetDebugState(DebugToolbar::Running);
    });
    QObject::connect(debugToolbar, &DebugToolbar::StopClicked, [debugToolbar]() {
        qDebug() << "Debug: Stop";
        debugToolbar->SetDebugState(DebugToolbar::Stopped);
    });

    // 设置剑三宏示例代码
    editor->setPlainText(R"(# 剑纯技能宏示例
# 参考JX3DPS模拟器宏语法

## 主循环宏 - 基础输出
macro 主循环
/scast [buff:无我无剑] 八荒归元
/scast [nobuff:无我无剑&skill_cd:八荒归元<8] 无我无剑
/scast [bufftime:无我无剑<2&tbuff:破>2] 三环套月
/scast [rage>=20] 人剑合一
/cast [tbuff:流血] 碎星辰
/cast [nobuff:太极] 太极无极
/cast 三柴剑法

## 爆发宏 - 高伤输出
macro 爆发
/fcast [buff:玄门&qidian>7] 两仪化形
/scast [buff:持盈] 八荒归元
/scast [buff:梦悠=4] 疾如风
/cast [skill_energy:盾飞>=2] 盾飞
/cast [nearby_enemy>2] 风来吴山

## 条件判定示例
macro 条件判定
/cast [life<0.3] 啸如虎
/cast [tlife<0.1] 闹须弥
/cast [mana<0.4] 碧水滔天
/cast [tbufftime:流血<2] 龙吟
/cast [skill_cd:八荒归元<4.5] 无我无剑

## 宏切换示例
macro 切换
/switch [buff:剑气] 爆发
/switch [nobuff:剑气] 平稳

## 事件语句示例（时间格式）
macro 事件序列
00:05.0 /add_target id=1 level=124 shield=27550
00:10.5 /set_target id=1 distance=4
00:30.0 /add_buff id=0 name=玄门 stack_num=3 duration=20
01:00.0 /change_target id=1
02:30.0 /set_target id=1 dead
03:00.0 /end
)");

    // 设置布局（只添加编辑器，toolbar 是浮窗）
    QVBoxLayout *layout = new QVBoxLayout(centralWidget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(editor);

    w.show();

    // 注意：调试工具栏默认隐藏，添加断点时会自动显示

    return QApplication::exec();
}