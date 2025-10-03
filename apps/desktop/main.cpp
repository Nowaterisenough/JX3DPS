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

    // 定义宏信息
    struct MacroInfo {
        QString name;
        QColor color;
    };

    QList<MacroInfo> macros = {
        {"宏1-基础", QColor(100, 150, 255)},    // 蓝色
        {"宏2-太极", QColor(255, 150, 100)},    // 橙色
        {"宏3-爆发", QColor(255, 100, 150)},    // 粉色
        {"循环", QColor(150, 255, 100)}         // 绿色
    };

    // 模拟600秒的技能释放数据，平均1.5秒释放2-3个技能
    int totalDuration = 600000; // 600秒
    int currentMacroIndex = 0;
    int macroSwitchCounter = 0;

    for (int ms = 0; ms < totalDuration;) {
        // 每隔15-25秒切换宏
        if (macroSwitchCounter >= 10 + QRandomGenerator::global()->bounded(7)) {
            currentMacroIndex = (currentMacroIndex + 1) % macros.size();
            macroSwitchCounter = 0;
        }

        const auto &currentMacro = macros[currentMacroIndex];

        // 每次释放2-3个技能
        int skillCount = 2 + QRandomGenerator::global()->bounded(2); // 2或3个技能

        for (int i = 0; i < skillCount && ms < totalDuration; ++i) {
            Timeline::EventItem event;
            int skillIndex = QRandomGenerator::global()->bounded(skills.size());
            const auto &skill = skills[skillIndex];

            event.timestamp = ms;
            event.name = skill.name;
            event.icon = QPixmap(skill.iconPath);
            event.color = skill.color;
            event.damage = 10000 + QRandomGenerator::global()->bounded(50000);
            event.rollResult = QRandomGenerator::global()->bounded(3) + 1;  // 1=普通, 2=会心, 3=识破
            event.macroName = currentMacro.name;
            event.macroColor = currentMacro.color;
            events.append(event);

            // 同一批技能之间间隔50-150ms
            ms += 50 + QRandomGenerator::global()->bounded(100);
        }

        // 下一批技能间隔1.3-1.7秒（平均1.5秒）
        ms += 1300 + QRandomGenerator::global()->bounded(400);
        macroSwitchCounter++;
    }
    timeline->SetEvents(events);

    // 添加测试Buff数据（覆盖600秒）
    QVector<Timeline::BuffItem> buffs;

    // Buff 1: 破防 (20秒CD，持续15秒)
    Timeline::BuffItem buff1;
    buff1.name = "破防";
    buff1.icon = QPixmap(":/resources/images/JX3/Icons/2942.png");
    buff1.color = QColor(255, 100, 100, 180);
    for (int t = 0; t < totalDuration; t += 35000) {
        buff1.segments.append({t, qMin(t + 15000, totalDuration)});
    }
    buffs.append(buff1);

    // Buff 2: 无双 (30秒CD，持续20秒)
    Timeline::BuffItem buff2;
    buff2.name = "无双";
    buff2.icon = QPixmap(":/resources/images/JX3/Icons/2941.png");
    buff2.color = QColor(100, 255, 100, 180);
    for (int t = 5000; t < totalDuration; t += 50000) {
        buff2.segments.append({t, qMin(t + 20000, totalDuration)});
    }
    buffs.append(buff2);

    // Buff 3: 会心 (25秒CD，持续12秒)
    Timeline::BuffItem buff3;
    buff3.name = "会心";
    buff3.icon = QPixmap(":/resources/images/JX3/Icons/2936.png");
    buff3.color = QColor(100, 100, 255, 180);
    for (int t = 10000; t < totalDuration; t += 37000) {
        buff3.segments.append({t, qMin(t + 12000, totalDuration)});
    }
    buffs.append(buff3);

    // Buff 4: 加速 (60秒CD，持续30秒)
    Timeline::BuffItem buff4;
    buff4.name = "加速";
    buff4.icon = QPixmap(":/resources/images/JX3/Icons/2934.png");
    buff4.color = QColor(255, 255, 100, 180);
    for (int t = 0; t < totalDuration; t += 90000) {
        buff4.segments.append({t, qMin(t + 30000, totalDuration)});
    }
    buffs.append(buff4);

    // Buff 5: 外功 (持续覆盖)
    Timeline::BuffItem buff5;
    buff5.name = "外功";
    buff5.icon = QPixmap(":/resources/images/JX3/Icons/2933.png");
    buff5.color = QColor(255, 150, 50, 180);
    buff5.segments.append({0, totalDuration});
    buffs.append(buff5);

    // Buff 6: 橙武特效 (45秒CD，持续18秒)
    Timeline::BuffItem buff6;
    buff6.name = "橙武";
    buff6.icon = QPixmap(":/resources/images/JX3/Icons/2931.png");
    buff6.color = QColor(255, 165, 0, 180);
    for (int t = 20000; t < totalDuration; t += 63000) {
        buff6.segments.append({t, qMin(t + 18000, totalDuration)});
    }
    buffs.append(buff6);

    timeline->SetBuffs(buffs);

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