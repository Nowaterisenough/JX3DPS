#include <QApplication>
#include <QDebug>
#include <QLabel>
#include <QRandomGenerator>
#include <QVBoxLayout>

#include "code_editor/code_editor.h"
#include "code_editor/debug_toolbar.h"
#include "controls/frameless/frameless.h"
#include "controls/theme/dark_style.h"
#include "controls/timeline/timeline.h"

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

    // 设置剑三宏示例代码（基于config配置的真实宏）
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

    // 创建时间轴控件
    Timeline *timeline = new Timeline(centralWidget);
    timeline->setMinimumHeight(200);

    // 添加测试数据
    QVector<Timeline::EventItem> events;

    // 剑纯技能 (技能名 + 图标ID)
    struct SkillData {
        QString name;
        QString iconPath;
        QColor color;
    };

    QList<SkillData> skills = {
        {"碎星辰", ":/resources/images/JX3/Icons/2942.png", QColor(255, 100, 100)},
        {"无我无剑", ":/resources/images/JX3/Icons/2941.png", QColor(100, 255, 100)},
        {"三环套月", ":/resources/images/JX3/Icons/2936.png", QColor(100, 100, 255)},
        {"八荒归元", ":/resources/images/JX3/Icons/2934.png", QColor(255, 255, 100)},
        {"吞日月", ":/resources/images/JX3/Icons/2933.png", QColor(255, 100, 255)},
        {"人剑合一", ":/resources/images/JX3/Icons/2931.png", QColor(100, 255, 255)},
        {"生太极", ":/resources/images/JX3/Icons/2927.png", QColor(200, 150, 100)},
        {"紫气东来", ":/resources/images/JX3/Icons/2926.png", QColor(150, 100, 200)}
    };

    // 模拟120秒的技能释放数据
    for (int ms = 0; ms < 120000; ms += 1000 + (QRandomGenerator::global()->bounded(2000))) {
        Timeline::EventItem event;
        int skillIndex = QRandomGenerator::global()->bounded(skills.size());
        const auto &skill = skills[skillIndex];

        event.timestamp = ms;
        event.name = skill.name;
        event.icon = QPixmap(skill.iconPath);
        event.color = skill.color;
        event.damage = 10000 + QRandomGenerator::global()->bounded(50000);
        event.rollResult = QRandomGenerator::global()->bounded(3) + 1;  // 1=普通, 2=会心, 3=识破
        events.append(event);
    }
    timeline->SetEvents(events);

    // 连接时间轴信号
    QObject::connect(timeline, &Timeline::EventClicked, [](int index, const Timeline::EventItem &item) {
        qDebug() << "Event clicked:" << index << item.name << "at" << item.timestamp << "ms";
    });
    QObject::connect(timeline, &Timeline::VisibleRangeChanged, [](int startMs, int endMs) {
        qDebug() << "Visible range:" << startMs << "-" << endMs << "ms";
    });

    // 设置布局（编辑器 + 时间轴，toolbar 是浮窗）
    QVBoxLayout *layout = new QVBoxLayout(centralWidget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(editor, 1);      // 编辑器占主要空间
    layout->addWidget(timeline, 0);    // 时间轴固定高度

    w.show();

    // 注意：调试工具栏默认隐藏，添加断点时会自动显示

    return QApplication::exec();
}