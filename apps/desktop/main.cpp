#include <QApplication>
#include <QVBoxLayout>
#include <QHBoxLayout>

#include "code_editor/code_editor.h"
#include "code_editor/debug_toolbar.h"
#include "controls/frameless/frameless.h"
#include "controls/theme/dark_style.h"
#include "controls/timeline/timeline.h"
#include "controls/player_state_panel/player_state_panel.h"
#include "test_data_generator.h"
#include "debug_session.h"
#include "debug_simulator.h"
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
    PlayerStatePanel *statePanel = new PlayerStatePanel(central);

    // 创建调试会话
    DebugSession *debugSession = new DebugSession(central);

    // 同步断点：编辑器 -> 调试会话
    QObject::connect(editor, &CodeEditor::BreakpointAdded, debugSession, &DebugSession::AddBreakpoint);
    QObject::connect(editor, &CodeEditor::BreakpointRemoved, debugSession, &DebugSession::RemoveBreakpoint);

    // 同步调试状态：调试会话 -> 编辑器 + 状态面板
    QObject::connect(debugSession, &DebugSession::LineChanged, editor, &CodeEditor::SetCurrentDebugLine);
    QObject::connect(debugSession, &DebugSession::StateChanged, [toolbar, statePanel](DebugSession::State state) {
        switch (state) {
            case DebugSession::Running:
                toolbar->SetDebugState(DebugToolbar::Running);
                break;
            case DebugSession::Paused:
                toolbar->SetDebugState(DebugToolbar::Paused);
                break;
            case DebugSession::Stopped:
            case DebugSession::Finished:
                toolbar->SetDebugState(DebugToolbar::Stopped);
                statePanel->Clear();
                break;
        }
    });

    // 更新状态面板：调试会话 -> 状态面板
    QObject::connect(debugSession, &DebugSession::DebugInfoUpdated, [statePanel](const DebugSession::DebugInfo &info) {
        PlayerStatePanel::PlayerState state;
        state.lifePercent = info.lifePercent;
        state.manaPercent = info.manaPercent;
        state.qidian = info.qidian;
        state.rage = info.rage;
        state.energy = info.energy;
        state.targetId = info.targetId;
        state.targetLifePercent = info.targetLifePercent;
        state.currentFrame = info.currentFrame;
        state.currentSeconds = info.currentSeconds;
        state.currentMacro = info.currentMacro;
        state.lastSkill = info.currentSkill;
        statePanel->UpdateState(state);
    });

    // 连接调试工具栏信号 -> 调试会话
    QObject::connect(toolbar, &DebugToolbar::ContinueClicked, [debugSession, editor]() {
        if (debugSession->GetState() == DebugSession::Stopped) {
            debugSession->Start(editor->toPlainText());
        } else {
            debugSession->Continue();
        }
    });
    QObject::connect(toolbar, &DebugToolbar::PauseClicked, debugSession, &DebugSession::Pause);
    QObject::connect(toolbar, &DebugToolbar::StepOverClicked, [debugSession, editor]() {
        if (debugSession->GetState() == DebugSession::Stopped) {
            debugSession->Start(editor->toPlainText());
        }
        debugSession->StepOver();
    });
    QObject::connect(toolbar, &DebugToolbar::StepIntoClicked, [debugSession, editor]() {
        if (debugSession->GetState() == DebugSession::Stopped) {
            debugSession->Start(editor->toPlainText());
        }
        debugSession->StepInto();
    });
    QObject::connect(toolbar, &DebugToolbar::StepOutClicked, [debugSession]() {
        debugSession->StepOut();
    });
    QObject::connect(toolbar, &DebugToolbar::RestartClicked, [debugSession, editor]() {
        debugSession->Stop();
        debugSession->Start(editor->toPlainText());
    });
    QObject::connect(toolbar, &DebugToolbar::StopClicked, [debugSession, editor]() {
        debugSession->Stop();
        editor->ClearCurrentDebugLine();
    });

    // 主布局：水平分割（左侧编辑器+时间轴，右侧状态面板）
    QHBoxLayout *mainLayout = new QHBoxLayout(central);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->setSpacing(10);

    // 左侧垂直布局
    QVBoxLayout *leftLayout = new QVBoxLayout();
    leftLayout->setSpacing(10);
    leftLayout->addWidget(editor, 1);
    leftLayout->addWidget(timeline);

    mainLayout->addLayout(leftLayout, 1);
    mainLayout->addWidget(statePanel);

    w.show();
    return app.exec();
}
