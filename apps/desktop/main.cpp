#include <QApplication>
#include <QVBoxLayout>

#include "code_editor/code_editor.h"
#include "code_editor/debug_toolbar.h"
#include "controls/frameless/frameless.h"
#include "controls/theme/dark_style.h"
#include "controls/timeline/timeline.h"
#include "test_data_generator.h"
#include "resources.h"

static void SetupApplication(QApplication &app)
{
    QFont defaultFont = Resources::Font();
    defaultFont.setHintingPreference(QFont::PreferFullHinting);
    defaultFont.setStyleStrategy(QFont::PreferAntialias);

    Resources::InitResources();
    QApplication::setFont(defaultFont);
    QApplication::setStyle(new DarkStyle());
}

static CodeEditor* CreateEditor(QWidget *parent, DebugToolbar *toolbar)
{
    CodeEditor *editor = new CodeEditor(parent);
    editor->setFont(Resources::MonoFont());
    editor->SetSyntaxType(CodeEditor::JX3Macro);

    QObject::connect(editor, &CodeEditor::BreakpointAdded, toolbar, &DebugToolbar::ShowFloating);

    editor->setPlainText(R"(# 剑纯技能宏示例
# 参考JX3DPS模拟器宏语法

## 主循环宏 - 基础输出
macro 循环
/cast 三柴剑法
/cast [buff:武器·橙武特效] 宏1
/scast 碎星辰
/scast [skill_notin_cd:无我无剑] 紫气东来
/scast [tlife:1<0.4] 腰坠·破防
/scast 无我无剑
/scast 三环套月
/scast 无我无剑
/scast 八荒归元
/scast 无我无剑
/scast 三环套月
/scast 无我无剑
/scast 吞日月
/scast 人剑合一
/switch [skill_cd:生太极<3&last_skill=人剑合一] 宏3
/switch [skill_cd:生太极<6&last_skill=人剑合一] 宏2
/switch [skill_cd:生太极>6&last_skill=人剑合一] 宏1

## 宏1 - 基础连招
macro 宏1
/cast 三柴剑法
/cast [buff:武器·橙武特效] 宏3
/scast 碎星辰
/scast 无我无剑
/scast 三环套月
/scast 无我无剑
/scast 八荒归元
/scast 无我无剑
/scast 三环套月
/scast 无我无剑
/scast 吞日月
/scast 人剑合一
/switch [skill_cd:生太极<6&last_skill=人剑合一] 宏2
/switch [skill_cd:生太极>6&last_skill=人剑合一] 宏1

## 宏2 - 太极循环
macro 宏2
/cast 三柴剑法
/cast [buff:武器·橙武特效] 宏3
/scast 三环套月
/scast 碎星辰
/scast 无我无剑
/scast 生太极
/scast 三环套月
/scast 无我无剑
/scast 八荒归元
/scast 吞日月
/scast 三环套月
/scast 人剑合一
/switch [tlife:1<0.4&last_skill=人剑合一&skill_energy:紫气东来>=1] 循环
/switch 宏1

## 宏切换示例
macro 切换
/switch [buff:武器·橙武特效] 宏3
/switch [skill_cd:生太极<3] 宏3
/switch [skill_cd:生太极<6] 宏2
/switch [skill_cd:生太极>6] 宏1

## 事件语句示例（时间格式）
macro 事件序列
00:05.0 /add_target id=1 level=124 shield=27550
00:10.5 /set_target id=1 distance=4
00:30.0 /add_buff id=0 name=玄门 stack_num=3 duration=20
01:00.0 /change_target id=1
02:30.0 /set_target id=1 dead
03:00.0 /end
)");

    return editor;
}

static Timeline* CreateTimeline(QWidget *parent)
{
    Timeline *timeline = new Timeline(parent);
    timeline->setMinimumHeight(200);
    timeline->SetEvents(TestDataGenerator::GenerateEvents());
    timeline->SetBuffs(TestDataGenerator::GenerateBuffs(600000));
    return timeline;
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    SetupApplication(app);

    Frameless w;
    w.SetupDefaultTitleBar();
    w.setWindowTitle("JX3DPS - Code Editor");
    w.resize(1200, 800);

    QWidget *central = w.ContentWidget();
    DebugToolbar *toolbar = new DebugToolbar(&w);
    CodeEditor *editor = CreateEditor(central, toolbar);
    Timeline *timeline = CreateTimeline(central);

    // 连接调试工具栏信号
    QObject::connect(toolbar, &DebugToolbar::ContinueClicked, [toolbar, editor]() {
        qDebug() << "Debug: Continue";
        toolbar->SetDebugState(DebugToolbar::Running);
        editor->SetCurrentDebugLine(5);
    });
    QObject::connect(toolbar, &DebugToolbar::PauseClicked, [toolbar, editor]() {
        qDebug() << "Debug: Pause";
        toolbar->SetDebugState(DebugToolbar::Paused);
        editor->SetCurrentDebugLine(10);
    });
    QObject::connect(toolbar, &DebugToolbar::StepOverClicked, [editor]() {
        qDebug() << "Debug: Step Over";
        int currentLine = editor->GetCurrentDebugLine();
        if (currentLine > 0) {
            editor->SetCurrentDebugLine(currentLine + 1);
        }
    });
    QObject::connect(toolbar, &DebugToolbar::StepIntoClicked, [editor]() {
        qDebug() << "Debug: Step Into";
        editor->SetCurrentDebugLine(3);
    });
    QObject::connect(toolbar, &DebugToolbar::StepOutClicked, [editor]() {
        qDebug() << "Debug: Step Out";
        editor->ClearCurrentDebugLine();
    });
    QObject::connect(toolbar, &DebugToolbar::RestartClicked, [toolbar]() {
        qDebug() << "Debug: Restart";
        toolbar->SetDebugState(DebugToolbar::Running);
    });
    QObject::connect(toolbar, &DebugToolbar::StopClicked, [toolbar]() {
        qDebug() << "Debug: Stop";
        toolbar->SetDebugState(DebugToolbar::Stopped);
    });

    QVBoxLayout *layout = new QVBoxLayout(central);
    layout->setContentsMargins(10, 10, 10, 10);
    layout->setSpacing(10);
    layout->addWidget(editor, 1);
    layout->addWidget(timeline);

    w.show();
    return app.exec();
}
