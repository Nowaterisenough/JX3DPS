#include "simulation_options.h"

#include <cmath>

#include <QCheckBox>
#include <QComboBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QFileDialog>
#include <QFormLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QJsonArray>
#include <QJsonDocument>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QRegularExpressionValidator>
#include <QSaveFile>
#include <QScrollArea>
#include <QSettings>
#include <QSpinBox>
#include <QStackedWidget>
#include <QTabWidget>
#include <QThread>
#include <QToolButton>
#include <QVBoxLayout>

#include "diagnostics.h"
#include "runtime_backend.h"
#include "game_icons.h"
#include "controls/equipment_panel/equipment_panel.h"

namespace mw = JX3DPS::runtime::mo_wen;
namespace tx = JX3DPS::runtime::tai_xu;
using namespace JX3DPS;

namespace {
QSpinBox *Integer(QFormLayout *form, const char *key, const QString &label, int value, int minimum, int maximum)
{
    auto *input = new QSpinBox;
    input->setObjectName(QString::fromLatin1(key));
    input->setRange(minimum, maximum);
    input->setValue(value);
    input->setGroupSeparatorShown(true);
    input->setProperty("defaultValue", value);
    form->addRow(label, input);
    return input;
}

QDoubleSpinBox *Decimal(QFormLayout *form, const char *key, const QString &label, double value, double minimum, double maximum, int decimals = 2)
{
    auto *input = new QDoubleSpinBox;
    input->setObjectName(QString::fromLatin1(key));
    input->setDecimals(decimals);
    input->setRange(minimum, maximum);
    input->setValue(value);
    input->setProperty("defaultValue", value);
    form->addRow(label, input);
    return input;
}
} // namespace

SimulationOptions::SimulationOptions(QWidget *parent) : QWidget(parent)
{
    Q_INIT_RESOURCE(desktop_data);
    auto *layout = new QGridLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(8);
    auto group = [&](const QString &title, const char *name, int row, int column, int rowSpan = 1, int columnSpan = 1) {
        auto *box = new QGroupBox(title, this);
        box->setObjectName(QString::fromLatin1(name));
        auto *content = new QVBoxLayout(box);
        content->setContentsMargins(10, 16, 10, 10);
        content->setSpacing(6);
        layout->addWidget(box, row, column, rowSpan, columnSpan);
        return content;
    };
    auto *settingsLayout = group(QStringLiteral("设置"), "runSettings", 0, 0, 2);
    auto *profileLayout  = group(QStringLiteral("配置"), "configurationSchemes", 0, 1);
    profileLayout->parentWidget()->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
    profileLayout->setAlignment(Qt::AlignTop);
    m_output              = group(QStringLiteral("输出"), "outputPanel", 1, 1);
    m_gains               = group(QStringLiteral("属性收益"), "gainsOverview", 2, 1);
    auto *permanentLayout = group(QStringLiteral("常驻增益"), "permanentPanel", 0, 2, 2);
    m_inputPanels         = { profileLayout->parentWidget(), permanentLayout->parentWidget() };
    SetupProfiles(profileLayout);
    m_specialization = new QComboBox(this);
    m_specialization->setObjectName(QStringLiteral("specialization"));
    m_specialization->addItems({ QStringLiteral("太虚剑意 · 纯阳"), QStringLiteral("莫问 · 长歌") });
    m_specialization->setAccessibleName(QStringLiteral("心法"));
    settingsLayout->addWidget(m_specialization);
    auto *runTabs = new QTabWidget(this);
    runTabs->setObjectName(QStringLiteral("runTabs"));
    settingsLayout->addWidget(runTabs, 1);
    m_inputPanels.push_back(runTabs);
    m_inputPanels.push_back(m_specialization);
    m_runActions = new QVBoxLayout;
    m_runActions->setContentsMargins(0, 0, 0, 0);
    settingsLayout->addLayout(m_runActions);
    auto *attributeTabs = new QTabWidget(this);
    attributeTabs->setObjectName(QStringLiteral("attributeTabs"));
    layout->addWidget(attributeTabs, 2, 0);
    auto *tabs = new QTabWidget(this);
    tabs->setObjectName(QStringLiteral("configurationTabs"));
    tabs->setMinimumHeight(200);
    layout->addWidget(tabs, 2, 2);
    m_inputPanels.push_back(tabs);
    auto page = [&](QTabWidget *owner, const QString &title) {
        auto *content = new QWidget;
        auto *form    = new QFormLayout(content);
        form->setContentsMargins(10, 10, 10, 10);
        form->setVerticalSpacing(3);
        form->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
        auto *scroll = new QScrollArea;
        scroll->setWidgetResizable(true);
        scroll->setFrameShape(QFrame::NoFrame);
        scroll->setWidget(content);
        owner->addTab(scroll, title);
        return form;
    };
    auto *run            = page(runTabs, QStringLiteral("模拟"));
    auto *target         = page(runTabs, QStringLiteral("目标与初始状态"));
    auto *attributes     = page(attributeTabs, QStringLiteral("属性"));
    auto *bonuses        = page(attributeTabs, QStringLiteral("加成"));
    m_inputPanels.push_back(attributes->parentWidget());
    m_inputPanels.push_back(bonuses->parentWidget());
    auto *equipmentPage = new QWidget;
    equipmentPage->setObjectName(QStringLiteral("equipmentPage"));
    auto *equipmentLayout = new QVBoxLayout(equipmentPage);
    equipmentLayout->setContentsMargins(7, 7, 7, 7);
    equipmentLayout->setSpacing(6);
    equipmentLayout->setAlignment(Qt::AlignTop);
    auto *equipmentPanel = new EquipmentPanel(equipmentPage);
    equipmentLayout->addWidget(equipmentPanel);
    auto equipmentEffects = [&] {
        auto *content = new QWidget(equipmentPage);
        content->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Maximum);
        auto *form = new QFormLayout(content);
        form->setContentsMargins(0, 0, 0, 0);
        form->setVerticalSpacing(6);
        form->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
        equipmentLayout->addWidget(content);
        m_inputPanels.push_back(content);
        return form;
    };
    auto *equipmentScroll = new QScrollArea;
    equipmentScroll->setObjectName(QStringLiteral("equipmentScroll"));
    equipmentScroll->setWidgetResizable(true);
    equipmentScroll->setFrameShape(QFrame::NoFrame);
    equipmentScroll->setWidget(equipmentPage);
    attributeTabs->addTab(equipmentScroll, QStringLiteral("配装"));
    auto *talentStack = new QStackedWidget;
    auto *recipeStack = new QStackedWidget;
    tabs->addTab(talentStack, QStringLiteral("奇穴"));
    tabs->addTab(recipeStack, QStringLiteral("秘籍"));
    auto effectPage = [](QStackedWidget *owner) {
        auto *content = new QWidget;
        auto *form    = new QFormLayout(content);
        form->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
        auto *scroll = new QScrollArea;
        scroll->setWidgetResizable(true);
        scroll->setFrameShape(QFrame::NoFrame);
        scroll->setWidget(content);
        owner->addWidget(scroll);
        return form;
    };
    auto *txTalents   = effectPage(talentStack);
    txTalents->setContentsMargins(3, 3, 3, 3);
    auto *txRecipes   = effectPage(recipeStack);
    auto *txEquipment = equipmentEffects();
    m_iterations      = Integer(run, "iterations", QStringLiteral("模拟次数"), 1000, 1, 1000000000);
    m_workers = Integer(run, "workers", QStringLiteral("工作线程"), std::max(1, QThread::idealThreadCount() / 2), 1, 256);
    m_seconds = Decimal(run, "seconds", QStringLiteral("战斗时长"), 300, 0.0625, 86400, 4);
    m_seconds->setSuffix(QStringLiteral(" 秒"));
    m_seconds->setSingleStep(0.0625);
    connect(m_seconds, &QDoubleSpinBox::editingFinished, this, [this] {
        m_seconds->setValue(std::llround(m_seconds->value() * 16.0) / 16.0);
    });
    m_seed = new QLineEdit(QStringLiteral("0"));
    m_seed->setObjectName(QStringLiteral("seed"));
    m_seed->setValidator(new QRegularExpressionValidator(QRegularExpression(QStringLiteral("[0-9]{1,20}")), m_seed));
    m_seed->setToolTip(QStringLiteral("相同种子可复现相同战斗；调试和单次记录复现其中第一场。"));
    run->addRow(QStringLiteral("随机种子"), m_seed);
    Integer(target, "qidian", QStringLiteral("初始气点"), 10, 0, 10);
    Integer(run, "delay", QStringLiteral("输入延迟"), 0, 0, 160)->setSuffix(QStringLiteral(" 帧"));
    Integer(target, "targetLevel", QStringLiteral("目标等级"), 124, 110, 130);
    Integer(target, "shield", QStringLiteral("目标防御"), 30000, 0, 10000000);
    Decimal(target, "targetLife", QStringLiteral("目标气血（%）"), 100, 1, 100);
    Decimal(target, "distance", QStringLiteral("目标距离"), 4, 0, 1000);
    m_bindings.push_back([](tx::Config &config, const QJsonObject &values) {
        config.initial_qidian  = values.value("qidian").toInt();
        config.delay           = values.value("delay").toInt();
        config.target.level    = values.value("targetLevel").toInt();
        config.target.shield   = values.value("shield").toInt();
        config.target.life     = values.value("targetLife").toDouble() / 100.0;
        config.target.distance = values.value("distance").toDouble();
    });
    const tx::Config defaults;
    auto attribute = [&](const char *key, const QString &label, value_t tx::Attributes::*member, int initial = -1) {
        auto *input =
            Integer(attributes, key, label, initial < 0 ? static_cast<int>(defaults.attributes.*member) : initial, 0, 1000000);
        m_bindings.push_back([key = input->objectName(), member](tx::Config &config, const QJsonObject &values) {
            config.attributes.*member = values.value(key).toInt();
        });
    };
    attribute("attackBase", QStringLiteral("基础攻击"), &tx::Attributes::attack_base);
    attribute("attackClass", QStringLiteral("心法攻击"), &tx::Attributes::attack_by_class);
    attribute("weapon", QStringLiteral("武器伤害"), &tx::Attributes::weapon);
    attribute("crit", QStringLiteral("会心等级"), &tx::Attributes::crit);
    attribute("critPower", QStringLiteral("会效等级"), &tx::Attributes::crit_power);
    attribute("overcomeBase", QStringLiteral("基础破防"), &tx::Attributes::overcome_base, 25000);
    attribute("overcomeClass", QStringLiteral("心法破防"), &tx::Attributes::overcome_by_class);
    attribute("strain", QStringLiteral("无双等级"), &tx::Attributes::strain);
    attribute("surplus", QStringLiteral("破招等级"), &tx::Attributes::surplus);
    attribute("haste", QStringLiteral("加速等级"), &tx::Attributes::haste);
    auto bonus = [&](const char *key, const QString &label, int tx::Attributes::*member, int scale = 1024, int maximum = 10000) {
        auto *input = Decimal(bonuses, key, label, 0, 0, maximum * 100.0 / scale, 3);
        input->setSuffix(QStringLiteral(" %"));
        m_bindings.push_back([key = input->objectName(), member, scale](tx::Config &config, const QJsonObject &values) {
            config.attributes.*member = static_cast<int>(std::llround(values.value(key).toDouble() * scale / 100.0));
        });
    };
    bonus("attackBonus", QStringLiteral("攻击加成"), &tx::Attributes::attack_percent);
    bonus("overcomeBonus", QStringLiteral("破防加成"), &tx::Attributes::overcome_percent);
    bonus("critBonus", QStringLiteral("会心几率加成"), &tx::Attributes::crit_basis_points, 10000);
    bonus("critPowerBonus", QStringLiteral("会效加成"), &tx::Attributes::crit_power_percent);
    bonus("hasteBonus", QStringLiteral("加速加成"), &tx::Attributes::haste_percent);
    bonus("damageBonus", QStringLiteral("伤害加成"), &tx::Attributes::damage_percent);
    bonus("strainBonus", QStringLiteral("无双加成"), &tx::Attributes::strain_percent);
    bonus("pveBonus", QStringLiteral("非侠士伤害加成"), &tx::Attributes::pve_percent);
    bonus("ignoreShield", QStringLiteral("无视防御"), &tx::Attributes::ignore_shield, 1024, 1024);
    Decimal(target, "vulnerable", QStringLiteral("目标易伤（%）"), 0, 0, 976.5625, 4);
    m_bindings.push_back([](tx::Config &config, const QJsonObject &values) {
        config.target.vulnerable = static_cast<int>(std::llround(values.value("vulnerable").toDouble() * 1024 / 100));
    });

    auto flags = [&](const QString                                           &title,
                     const char                                              *prefix,
                     std::initializer_list<std::pair<const char *, unsigned>> entries,
                     std::function<void(tx::Config &, unsigned)>              assign) {
        auto *group = new QGroupBox(title);
        auto *rows  = new QGridLayout(group);
        rows->setVerticalSpacing(5);
        const bool talent = QString::fromLatin1(prefix) == QStringLiteral("talent");
        const bool equipment =
            QString::fromLatin1(prefix) == QStringLiteral("equipment") || QString::fromLatin1(prefix) == QStringLiteral("team");
        const int                                 columns = talent ? 4 : 1;
        if (!talent && !equipment) group->setTitle(QString(title).replace(QStringLiteral("秘籍（最多四本）"), QStringLiteral("（最多四本）")));
        if (talent) {
            group->setTitle({});
            rows->setContentsMargins(4, 4, 4, 4);
            rows->setHorizontalSpacing(2);
            rows->setAlignment(Qt::AlignLeft | Qt::AlignTop);
        }
        int                                       index   = 0;
        std::vector<std::pair<QString, unsigned>> inputs;
        for (const auto &[label, flag] : entries) {
            auto *input = talent ? new desktop::IconCheckBox(QString::fromUtf8(label), desktop::Specialization::TaiXu, group)
                                 : new QCheckBox(QString::fromUtf8(label), group);
            input->setObjectName(QString::fromLatin1(prefix) + QString::number(flag));
            input->setProperty("defaultValue", false);
            rows->addWidget(input, index / columns, index % columns);
            ++index;
            inputs.emplace_back(input->objectName(), flag);
        }
        (talent ? txTalents : equipment ? txEquipment : txRecipes)->addRow(group);
        m_bindings.push_back([inputs, assign](tx::Config &config, const QJsonObject &values) {
            unsigned result = 0;
            for (const auto &[key, flag] : inputs) {
                if (values.value(key).toBool()) {
                    result |= flag;
                }
            }
            assign(config, result);
        });
    };
    flags(QStringLiteral("装备特效"),
          "equipment",
          {
              { "橙武特效",  tx::WeaponCW     },
              { "套装·剑鸣", tx::SetAttribute }
    },
          [](tx::Config &c, unsigned v) { c.equipment = v; });
    flags(QStringLiteral("团队增益"),
          "team",
          {
              { "游刃", tx::YouRen }
    },
          [](tx::Config &c, unsigned v) { c.team_effects = v; });
    auto *teamGroup = new QGroupBox(QStringLiteral("外部增益时间表（太虚）"));
    auto *teamRows = new QGridLayout(teamGroup);
    teamRows->addWidget(new QLabel(QStringLiteral("增益")), 0, 0);
    teamRows->addWidget(new QLabel(QStringLiteral("开始（秒）")), 0, 1);
    teamRows->addWidget(new QLabel(QStringLiteral("持续（秒）")), 0, 2);
    teamRows->addWidget(new QLabel(QStringLiteral("层数")), 0, 3);
    for (unsigned i = 0; i < tx::team::Count; ++i) {
        const auto &definition = tx::team::Definitions[i];
        const auto prefix = QStringLiteral("txTeam%1_").arg(i);
        auto *enabled = new QCheckBox(QString::fromUtf8(definition.name.data(), definition.name.size()));
        enabled->setObjectName(prefix + "enabled");
        enabled->setProperty("defaultValue", false);
        teamRows->addWidget(enabled, i + 1, 0);
        for (const auto &field : {QStringLiteral("start"), QStringLiteral("duration")}) {
            auto *input = new QDoubleSpinBox;
            input->setObjectName(prefix + field);
            input->setDecimals(4);
            input->setSingleStep(0.0625);
            input->setRange(field == "start" ? 0 : 0.0625, 86400);
            input->setValue(field == "start" ? 0 : definition.duration / 16.0);
            if (i == tx::team::HaoLingSanJun && field == "duration") input->setRange(60, 60);
            input->setProperty("defaultValue", input->value());
            teamRows->addWidget(input, i + 1, field == "start" ? 1 : 2);
        }
        auto *stacks = new QSpinBox;
        stacks->setObjectName(prefix + "stacks");
        stacks->setRange(1, definition.max_stacks);
        stacks->setProperty("defaultValue", 1);
        if (i == tx::team::HaoLingSanJun) {
            stacks->setMinimum(2);
            stacks->setSingleStep(2);
            stacks->setValue(48);
            stacks->setProperty("defaultValue", 48);
            stacks->setToolTip(QStringLiteral("旧版奇数层扣除不一致，当前支持偶数初始层数；30 秒后减半，60 秒结束，期间重复施加无效。"));
        }
        teamRows->addWidget(stacks, i + 1, 3);
        m_bindings.push_back([i, prefix](tx::Config &config, const QJsonObject &values) {
            if (values.value(prefix + "enabled").toBool())
                config.team_buffs.push_back({static_cast<tx::team::Kind>(i),
                    static_cast<tick_t>(std::llround(values.value(prefix + "start").toDouble() * 16)),
                    static_cast<tick_t>(std::llround(values.value(prefix + "duration").toDouble() * 16)),
                    values.value(prefix + "stacks").toInt()});
        });
    }
    txEquipment->addRow(teamGroup);
    flags(QStringLiteral("奇穴"),
          "talent",
          {
              { "叠刃",   tx::DieRenTalent  },
              { "无意",   tx::WuYi          },
              { "深埋",   tx::ShenMai       },
              { "若水",   tx::RuoShui       },
              { "风逝",   tx::FengShiTalent },
              { "无欲",   tx::WuYu          },
              { "环月",   tx::HuanYue       },
              { "切玉",   tx::QieYu         },
              { "心固",   tx::XinGu         },
              { "同根",   tx::TongGen       },
              { "负阴",   tx::FuYin         },
              { "故长",   tx::GuChang       },
              { "期声",   tx::QiSheng       },
              { "长生",   tx::ChangSheng    },
              { "玄门",   tx::XuanMen       },
              { "裂云",   tx::LieYun        },
              { "虚极",   tx::XuJi          },
              { "剑入",   tx::JianRu        },
              { "镜花影", tx::JingHuaTalent }
    },
          [](tx::Config &c, unsigned v) { c.talents = v; });
    flags(QStringLiteral("无我无剑秘籍（最多四本）"),
          "wuwu",
          {
              { "会心 +2%", tx::Crit2   },
              { "会心 +3%", tx::Crit3   },
              { "会心 +4%", tx::Crit4   },
              { "伤害 +3%", tx::Damage3 },
              { "伤害 +4%", tx::Damage4 },
              { "伤害 +5%", tx::Damage5 },
              { "气点",     tx::Qidian  }
    },
          [](tx::Config &c, unsigned v) { c.recipes[tx::WuWo] = v; });
    flags(QStringLiteral("八荒归元秘籍（最多四本）"),
          "bahuang",
          {
              { "伤害 +3%", tx::Damage3  },
              { "伤害 +4%", tx::Damage4  },
              { "伤害 +5%", tx::Damage5  },
              { "气点",     tx::Qidian   },
              { "冷却缩短", tx::Cooldown }
    },
          [](tx::Config &c, unsigned v) { c.recipes[tx::BaHuang] = v; });
    flags(QStringLiteral("三环套月秘籍（最多四本）"),
          "sanhuan",
          {
              { "会心 +2%", tx::Crit2    },
              { "会心 +3%", tx::Crit3    },
              { "会心 +4%", tx::Crit4    },
              { "伤害 +3%", tx::Damage3  },
              { "伤害 +4%", tx::Damage4  },
              { "伤害 +5%", tx::Damage5  },
              { "冷却缩短", tx::Cooldown }
    },
          [](tx::Config &c, unsigned v) { c.recipes[tx::SanHuan] = v; });
    flags(QStringLiteral("生太极秘籍"),
          "shengtaiji",
          {
              { "运功缩短一", tx::Prepare1 },
              { "运功缩短二", tx::Prepare2 },
              { "运功缩短三", tx::Prepare3 },
              { "冷却缩短",   tx::Cooldown }
    },
          [](tx::Config &c, unsigned v) { c.shengtaiji_recipes = v; });
    flags(QStringLiteral("人剑合一秘籍（最多四本）"),
          "renjian",
          {
              { "冷却缩短",    tx::Cooldown         },
              { "距离增加一",  tx::Range1           },
              { "距离增加二",  tx::Range2           },
              { "距离增加三",  tx::Range3           },
              { "伤害 +40%",   tx::Damage40         },
              { "伤害 +60%",   tx::Damage60         },
              { "持续伤害",    tx::Dot              },
              { "云中·碎星",   tx::YunZhongSuiXing  },
              { "云中·生太极", tx::YunZhongShengTai },
              { "云中·吞日月", tx::YunZhongTunRi    }
    },
          [](tx::Config &c, unsigned v) { c.renjian_recipes = v; });

    auto *mwTalents      = effectPage(talentStack);
    mwTalents->setContentsMargins(3, 3, 3, 3);
    auto *mwRecipes      = effectPage(recipeStack);
    auto *mwEquipment    = equipmentEffects();
    auto *initialShadows = Integer(target, "mwShadows", QStringLiteral("初始影子数量"), 0, 0, 6);
    m_style              = new QComboBox;
    m_style->setObjectName(QStringLiteral("mwStyle"));
    m_style->addItems({ QStringLiteral("阳春白雪"), QStringLiteral("高山流水") });
    m_style->setCurrentIndex(1);
    target->addRow(QStringLiteral("初始曲目"), m_style);
    auto mwFlags = [&](const QString                                           &title,
                       const char                                              *prefix,
                       std::initializer_list<std::pair<const char *, unsigned>> entries,
                       std::function<void(mw::Config &, unsigned)>              assign) {
        auto *group = new QGroupBox(title);
        auto *rows  = new QGridLayout(group);
        rows->setVerticalSpacing(5);
        const bool talent    = QString::fromLatin1(prefix) == QStringLiteral("mwTalent");
        const bool equipment = QString::fromLatin1(prefix) == QStringLiteral("mwEquipment") ||
                               QString::fromLatin1(prefix) == QStringLiteral("mwTeam");
        const int  columns   = talent ? 4 : 1;
        if (!talent && !equipment) group->setTitle(QString(title).replace(QStringLiteral("秘籍（最多四本）"), QStringLiteral("（最多四本）")));
        if (talent) {
            group->setTitle({});
            rows->setContentsMargins(4, 4, 4, 4);
            rows->setHorizontalSpacing(2);
            rows->setAlignment(Qt::AlignLeft | Qt::AlignTop);
        }
        int        index     = 0;
        std::vector<std::pair<QString, unsigned>> inputs;
        for (const auto &[label, flag] : entries) {
            const auto text  = QString::fromUtf8(label);
            auto *input = talent ? new desktop::IconCheckBox(text, desktop::Specialization::MoWen, group)
                                 : new QCheckBox(text.section(QStringLiteral("（"), 0, 0), group);
            if (!talent) input->setToolTip(text);
            input->setObjectName(QString::fromLatin1(prefix) + QString::number(flag));
            input->setProperty("defaultValue", false);
            rows->addWidget(input, index / columns, index % columns);
            ++index;
            inputs.emplace_back(input->objectName(), flag);
        }
        (talent ? mwTalents : equipment ? mwEquipment : mwRecipes)->addRow(group);
        m_mowenBindings.push_back([inputs, assign](mw::Config &config, const QJsonObject &values) {
            unsigned result = 0;
            for (const auto &[key, flag] : inputs) {
                if (values.value(key).toBool()) {
                    result |= flag;
                }
            }
            assign(config, result);
        });
    };
    mwFlags(QStringLiteral("装备特效"),
            "mwEquipment",
            {
                { "大附魔·鞋", mw::EnchantShoes },
                { "大附魔·腰", mw::EnchantBelt  },
                { "大附魔·腕", mw::EnchantWrist },
                { "套装·属性", mw::SetAttribute },
                { "套装·技能", mw::SetSkill     },
                { "橙武特效",  mw::WeaponCW     },
                { "水·斩流",   mw::WeaponWater  }
    },
            [](mw::Config &c, unsigned v) { c.equipment = v; });
    mwFlags(QStringLiteral("团队增益"),
            "mwTeam",
            {
                { "游刃", mw::YouRen   },
                { "精妙", mw::JingMiao }
    },
            [](mw::Config &c, unsigned v) { c.team_effects = v; });
    mwFlags(QStringLiteral("奇穴"),
            "mwTalent",
            {
                { "号钟",               mw::HaoZhong       },
                { "飞帆",               mw::FeiFan         },
                { "长清（旧版无效果）", mw::ChangQing      },
                { "弦风",               mw::XianFengTalent },
                { "流照",               mw::LiuZhaoTalent  },
                { "豪情",               mw::HaoQing        },
                { "师襄",               mw::ShiXiang       },
                { "知止",               mw::ZhiZhi         },
                { "刻梦",               mw::KeMeng         },
                { "争鸣",               mw::ZhengMing      },
                { "明津",               mw::MingJinTalent  },
                { "修期（旧版无效果）", mw::XiuQi          },
                { "云汉",               mw::YunHanTalent   },
                { "参连",               mw::CanLianTalent  },
                { "正律和鸣",           mw::ZhengLvHeMing  }
    },
            [](mw::Config &c, unsigned v) { c.talents = v; });
    mwFlags(QStringLiteral("宫秘籍（最多四本）"),
            "mwGong",
            {
                { "会心 +2%",   mw::GongCrit2    },
                { "会心 +3%",   mw::GongCrit3    },
                { "会心 +4%",   mw::GongCrit4    },
                { "伤害 +3%",   mw::GongDamage3  },
                { "伤害 +4%",   mw::GongDamage4  },
                { "运功缩短一", mw::GongPrepare1 },
                { "运功缩短二", mw::GongPrepare2 },
                { "距离增加",   mw::GongRange    }
    },
            [](mw::Config &c, unsigned v) { c.recipes[0] = v; });
    mwFlags(QStringLiteral("商秘籍（最多四本）"),
            "mwShang",
            {
                { "会心 +2%", mw::ShangCrit2   },
                { "会心 +3%", mw::ShangCrit3   },
                { "会心 +4%", mw::ShangCrit4   },
                { "伤害 +3%", mw::ShangDamage3 },
                { "伤害 +4%", mw::ShangDamage4 },
                { "伤害 +5%", mw::ShangDamage5 }
    },
            [](mw::Config &c, unsigned v) { c.recipes[1] = v; });
    mwFlags(QStringLiteral("徵秘籍（最多四本）"),
            "mwZhi",
            {
                { "会心 +2%", mw::ZhiCrit2   },
                { "会心 +3%", mw::ZhiCrit3   },
                { "会心 +4%", mw::ZhiCrit4   },
                { "伤害 +3%", mw::ZhiDamage3 },
                { "伤害 +4%", mw::ZhiDamage4 },
                { "距离增加", mw::ZhiRange   }
    },
            [](mw::Config &c, unsigned v) { c.recipes[2] = v; });
    mwFlags(QStringLiteral("羽秘籍（最多四本）"),
            "mwYu",
            {
                { "会心 +3%", mw::YuCrit3   },
                { "会心 +4%", mw::YuCrit4   },
                { "伤害 +3%", mw::YuDamage3 },
                { "伤害 +4%", mw::YuDamage4 }
    },
            [](mw::Config &c, unsigned v) { c.recipes[3] = v; });
    auto *permanentScroll = new QScrollArea;
    permanentScroll->setWidgetResizable(true);
    permanentScroll->setFrameShape(QFrame::NoFrame);
    auto *permanentStack = new QStackedWidget;
    permanentScroll->setWidget(permanentStack);
    permanentLayout->addWidget(permanentScroll);
    permanentScroll->setToolTip(QStringLiteral("沿用旧版常驻增益数值。动态游刃、精妙在配装中选择。"));
    QFile permanentFile(QStringLiteral(":/desktop/permanents.json"));
    if (!permanentFile.open(QIODevice::ReadOnly)) {
        throw std::runtime_error("缺少常驻增益数据资源");
    }
    const auto permanentData = QJsonDocument::fromJson(permanentFile.readAll()).object();
    const std::pair<const char *, const char *> categories[] = {
        { "TeamCore",        "阵法"     },
        { "FoodEnhance",     "增强食品" },
        { "FoodSupport",     "辅助食品" },
        { "MedEnhance",      "增强药品" },
        { "MedSupport",      "辅助药品" },
        { "HomeCook",        "家园炊事" },
        { "HomeWine",        "家园酿造" },
        { "WeaponWhetstone", "武器磨石" },
        { "Others",          "宴席"     }
    };
    for (int specialization = 0; specialization < 2; ++specialization) {
        auto *content = new QWidget;
        auto *grid = new QGridLayout(content);
        grid->setContentsMargins(0, 0, 0, 0);
        grid->setSpacing(8);
        grid->setAlignment(Qt::AlignTop | Qt::AlignLeft);
        permanentStack->addWidget(content);
        int position = 0;
        const auto groups = permanentData.value(QString::number(specialization)).toObject();
        for (const auto &[category, label] : categories) {
            const auto items = groups.value(QString::fromLatin1(category)).toArray();
            const bool feast = QString::fromLatin1(category) == "Others";
            auto       add   = [&](const QJsonArray &entries, const QString &suffix, const QString &title) {
                auto *cell = new QWidget;
                auto *cellLayout = new QVBoxLayout(cell);
                cellLayout->setContentsMargins(0, 0, 0, 0);
                cellLayout->setSpacing(2);
                auto *combo = new desktop::IconComboBox(title);
                combo->setObjectName(QStringLiteral("permanent%1%2%3").arg(specialization).arg(QString::fromLatin1(category), suffix));
                combo->setProperty("specialization", specialization);
                combo->addItem(QStringLiteral("无"), QJsonArray{});
                combo->setItemData(0, QStringLiteral("不使用此项增益"), Qt::ToolTipRole);
                for (const auto &value : entries) {
                    const auto entry = value.toObject();
                    const auto label = entry.value("Name").toString();
                    combo->addItem(desktop::GameIcon(entry.value("Icon").toInt()), label, entry.value("AttributeAdditional").toArray());
                    combo->setItemData(combo->count() - 1, label + '\n' + entry.value("Description").toString(), Qt::ToolTipRole);
                }
                combo->setToolTip(title + QStringLiteral("：无；点击图标选择"));
                cellLayout->addWidget(combo, 0, Qt::AlignHCenter);
                auto *caption = new QLabel(feast ? QStringLiteral("宴席") : title);
                caption->setAlignment(Qt::AlignCenter);
                caption->setStyleSheet(QStringLiteral("font-size: 11px; color: #aeb9c7;"));
                cellLayout->addWidget(caption);
                grid->addWidget(cell, position / 3, position % 3);
                ++position;
                m_permanents.push_back(combo);
            };
            if (feast) {
                for (int index = 0; index < items.size(); ++index) {
                    add({ items[index] }, QString::number(index), items[index].toObject().value("Name").toString());
                }
            } else {
                add(items, {}, QString::fromUtf8(label));
            }
        }
    }
    auto *importAttributes = new QPushButton(QStringLiteral("导入魔盒属性"));
    importAttributes->setObjectName(QStringLiteral("importAttributes"));
    attributes->insertRow(0, importAttributes);
    connect(importAttributes, &QPushButton::clicked, this, [this] {
        QDialog dialog(this);
        dialog.setWindowTitle(QStringLiteral("导入魔盒属性"));
        dialog.resize(620, 480);
        auto *rows = new QVBoxLayout(&dialog);
        auto *note = new QLabel(QStringLiteral("粘贴魔盒导出的属性对象。按当前心法读取外功或阴性内功属性；导入会替换属"
                                               "性并清空常驻增益选择，避免重复叠加。"));
        note->setWordWrap(true);
        rows->addWidget(note);
        auto *text = new QPlainTextEdit(&dialog);
        rows->addWidget(text);
        auto *status = new QLabel(&dialog);
        status->setWordWrap(true);
        rows->addWidget(status);
        auto *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
        buttons->button(QDialogButtonBox::Ok)->setText(QStringLiteral("导入"));
        buttons->button(QDialogButtonBox::Cancel)->setText(QStringLiteral("取消"));
        rows->addWidget(buttons);
        connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
        connect(buttons, &QDialogButtonBox::accepted, &dialog, [&, this] {
            QJsonParseError parse;
            const auto      json = QJsonDocument::fromJson(text->toPlainText().toUtf8(), &parse);
            QString         error;
            if (parse.error != QJsonParseError::NoError || !json.isObject()) {
                status->setText(QStringLiteral("请输入有效的属性对象。"));
            } else if (!ImportAttributes(json.object(), error)) {
                status->setText(error);
            } else {
                dialog.accept();
            }
        });
        dialog.exec();
    });
    Integer(bonuses, "importedMajor", QStringLiteral("已含身法 / 根骨"), 0, 0, 1000000)
        ->setToolTip(QStringLiteral("当前会心与心法攻击已包含此主属性；用于精确计算食品、药品增加的主属性收益。"));
    Integer(bonuses, "importedStrength", QStringLiteral("已含力道"), 41, 0, 1000000);
    auto updateClass = [=, this] {
        const bool moWen = Specialization() == desktop::Specialization::MoWen;
        talentStack->setCurrentIndex(moWen ? 1 : 0);
        recipeStack->setCurrentIndex(moWen ? 1 : 0);
        txEquipment->parentWidget()->setVisible(!moWen);
        mwEquipment->parentWidget()->setVisible(moWen);
        equipmentPanel->setVisible(!moWen);
        target->setRowVisible(findChild<QSpinBox *>("qidian"), !moWen);
        target->setRowVisible(initialShadows, moWen);
        target->setRowVisible(m_style, moWen);
        attributes->setRowVisible(findChild<QSpinBox *>("overcomeClass"), !moWen);
        permanentStack->setCurrentIndex(m_specialization->currentIndex());
        emit SpecializationChanged();
    };
    connect(m_specialization, &QComboBox::currentIndexChanged, this, updateClass);
    QSettings settings;
    for (auto *input : m_permanents) {
        input->setCurrentIndex(std::clamp(settings.value("simulation/" + input->objectName(), 0).toInt(), 0, input->count() - 1));
    }
    m_style->setCurrentIndex(settings.value("simulation/mwStyle", 1).toInt());
    m_specialization->setCurrentIndex(settings.value("simulation/specialization", 0).toInt() == 1 ? 1 : 0);
    updateClass();
    for (auto *input : findChildren<QSpinBox *>()) {
        input->setValue(settings.value("simulation/" + input->objectName(), input->value()).toInt());
    }
    for (auto *input : findChildren<QDoubleSpinBox *>()) {
        input->setValue(settings.value("simulation/" + input->objectName(), input->value()).toDouble());
    }
    for (auto *input : findChildren<QCheckBox *>()) {
        input->setChecked(settings.value("simulation/" + input->objectName(), false).toBool());
    }
    m_seed->setText(settings.value(QStringLiteral("simulation/seed"), QStringLiteral("0")).toString());
    auto *reset = findChild<QMenu *>(QStringLiteral("configurationMenu"))->addAction(QStringLiteral("重置配置"));
    reset->setObjectName(QStringLiteral("resetConfiguration"));
    layout->setColumnStretch(0, 5);
    layout->setColumnStretch(1, 5);
    layout->setColumnStretch(2, 4);
    layout->setRowStretch(2, 1);
    connect(reset, &QAction::triggered, this, [this] {
        for (auto *input : findChildren<QSpinBox *>()) {
            input->setValue(input->property("defaultValue").toInt());
        }
        for (auto *input : findChildren<QDoubleSpinBox *>()) {
            input->setValue(input->property("defaultValue").toDouble());
        }
        for (auto *input : findChildren<QCheckBox *>()) {
            input->setChecked(false);
        }
        m_seed->setText(QStringLiteral("0"));
        m_style->setCurrentIndex(1);
        for (auto *input : m_permanents) {
            input->setCurrentIndex(0);
        }
    });
}

void SimulationOptions::SetInputsEnabled(bool enabled)
{
    for (auto *panel : m_inputPanels) {
        panel->setEnabled(enabled);
    }
}

tx::Config SimulationOptions::Config() const
{
    return std::get<tx::Config>(ActiveConfigFromValues(Values(), desktop::Specialization::TaiXu));
}

tx::Config SimulationOptions::ConfigFromValues(const QJsonObject &values) const
{
    tx::Config config;
    for (const auto &assign : m_bindings) {
        assign(config, values);
    }
    return config;
}

desktop::Specialization SimulationOptions::Specialization() const
{
    return m_specialization->currentIndex() == 1 ? desktop::Specialization::MoWen : desktop::Specialization::TaiXu;
}

desktop::Config SimulationOptions::ActiveConfig() const
{
    return ActiveConfigFromValues(Values(), Specialization());
}

desktop::Config SimulationOptions::ActiveConfigFromValues(const QJsonObject &values, desktop::Specialization specialization) const
{
    auto  taiXu      = ConfigFromValues(values);
    auto &attr       = taiXu.attributes;
    int   majorAdded = 0, strengthBonus = 0;
    for (auto *input : m_permanents) {
        if (input->property("specialization").toInt() != static_cast<int>(specialization)) {
            continue;
        }
        const auto effects = input->itemData(values.value(input->objectName()).toInt()).value<QJsonArray>();
        for (const auto &effect : effects) {
            const auto object = effect.toObject();
            const auto type   = object.value("Type").toString();
            const auto amount = object.value("Value").toInt();
            if (type == QStringLiteral("基础攻击")) {
                attr.attack_base += amount;
            } else if (type == QStringLiteral("基础破防等级")) {
                attr.overcome_base += amount;
            } else if (type == QStringLiteral("会心等级")) {
                attr.crit += amount;
            } else if (type == QStringLiteral("无双")) {
                attr.strain += amount;
            } else if (type == QStringLiteral("破招值")) {
                attr.surplus += amount;
            } else if (type == QStringLiteral("加速等级")) {
                attr.haste += amount;
            } else if (type == QStringLiteral("身法") || type == QStringLiteral("根骨")) {
                majorAdded += amount;
            } else if (type == QStringLiteral("力道加成")) {
                strengthBonus += amount;
            } else if (type == QStringLiteral("基础攻击加成")) {
                attr.attack_percent += amount;
            } else if (type == QStringLiteral("无双加成")) {
                attr.strain_percent += amount;
            } else if (type == QStringLiteral("伤害加成")) {
                attr.damage_percent += amount;
            } else if (type == QStringLiteral("外功会效加成")) {
                attr.crit_power_percent += amount;
            } else if (type == QStringLiteral("外功破防加成")) {
                attr.overcome_percent += amount;
            } else if (type.endsWith(QStringLiteral("会心加成"))) {
                attr.crit_basis_points += amount;
            } else {
                throw std::invalid_argument("常驻增益包含不支持的属性");
            }
        }
    }
    const value_t major             = values.value("importedMajor").toInt();
    const auto    attackCoefficient = specialization == desktop::Specialization::MoWen ? 1895 : 1485;
    const auto    critCoefficient   = specialization == desktop::Specialization::MoWen ? 389 : 594;
    attr.attack_by_class      += (major + majorAdded) * attackCoefficient / 1024 - major * attackCoefficient / 1024;
    attr.crit                 += (major + majorAdded) * critCoefficient / 1024 - major * critCoefficient / 1024 +
                                 (major + majorAdded) * 655 / 1024 - major * 655 / 1024;
    const value_t strength     = values.value("importedStrength").toInt();
    const value_t newStrength  = strength * (1024 + strengthBonus) / 1024;
    attr.attack_base          += newStrength * 153 / 1024 - strength * 153 / 1024;
    attr.overcome_base        += newStrength * 307 / 1024 - strength * 307 / 1024;
    if (specialization == desktop::Specialization::TaiXu) {
        return taiXu;
    }
    mw::Config  config;
    const auto &a     = taiXu.attributes;
    config.attributes = {
        a.attack_base,
        a.attack_by_class,
        a.weapon,
        a.crit,
        a.crit_power,
        a.overcome_base,
        a.strain,
        a.surplus,
        a.haste,
        a.attack_percent,
        a.overcome_percent,
        a.haste_percent,
        a.crit_basis_points,
        a.crit_power_percent,
        a.damage_percent,
        a.strain_percent,
        a.pve_percent,
        a.ignore_shield
    };
    config.target = { taiXu.target.level, taiXu.target.shield, taiXu.target.vulnerable, taiXu.target.life, taiXu.target.distance };
    config.delay          = taiXu.delay;
    config.initial_yingzi = values.value("mwShadows").toInt();
    config.initial_style  = static_cast<mw::Style>(values.value("mwStyle").toInt());
    for (const auto &assign : m_mowenBindings) {
        assign(config, values);
    }
    return config;
}

runtime::BatchOptions SimulationOptions::Batch() const
{
    runtime::BatchOptions batch;
    batch.iterations = m_iterations->value();
    batch.workers    = m_workers->value();
    batch.duration   = static_cast<tick_t>(std::llround(m_seconds->value() * 16.0));
    bool valid       = false;
    batch.seed       = m_seed->text().toULongLong(&valid);
    if (!valid) {
        throw std::invalid_argument("随机种子须为 0 至 18446744073709551615 之间的整数。");
    }
    batch.damage_capacity   = static_cast<std::size_t>(batch.duration) * 16 + 64;
    batch.mutation_capacity = static_cast<std::size_t>(batch.duration) * 40 + 128;
    return batch;
}

void SimulationOptions::SaveSettings() const
{
    QSettings settings;
    for (auto *input : findChildren<QSpinBox *>()) {
        settings.setValue("simulation/" + input->objectName(), input->value());
    }
    for (auto *input : findChildren<QDoubleSpinBox *>()) {
        settings.setValue("simulation/" + input->objectName(), input->value());
    }
    for (auto *input : findChildren<QCheckBox *>()) {
        settings.setValue("simulation/" + input->objectName(), input->isChecked());
    }
    settings.setValue(QStringLiteral("simulation/seed"), m_seed->text());
    settings.setValue("simulation/specialization", m_specialization->currentIndex());
    settings.setValue("simulation/mwStyle", m_style->currentIndex());
    for (auto *input : m_permanents) {
        settings.setValue("simulation/" + input->objectName(), input->currentIndex());
    }
}

QJsonObject SimulationOptions::Values() const
{
    QJsonObject values;
    for (auto *input : findChildren<QSpinBox *>()) {
        values.insert(input->objectName(), input->value());
    }
    for (auto *input : findChildren<QDoubleSpinBox *>()) {
        values.insert(input->objectName(), input->value());
    }
    for (auto *input : findChildren<QCheckBox *>()) {
        values.insert(input->objectName(), input->isChecked());
    }
    // JSON numbers cannot portably preserve all uint64 seeds.
    values.insert(QStringLiteral("seed"), m_seed->text());
    values.insert("mwStyle", m_style->currentIndex());
    for (auto *input : m_permanents) {
        values.insert(input->objectName(), input->currentIndex());
    }
    return values;
}

QJsonObject SimulationOptions::Snapshot() const
{
    return {
        { "format",         "jx3dps.desktop.config"          },
        { "version",        2                                },
        { "specialization", m_specialization->currentIndex() },
        { "values",         Values()                         }
    };
}

bool SimulationOptions::ValidateSnapshot(const QJsonObject &snapshot, QString &error) const
{
    error.clear();
    if (snapshot.value("format") != QJsonValue("jx3dps.desktop.config") || snapshot.value("version") != QJsonValue(2) ||
        !snapshot.value("values").isObject())
    {
        error = QStringLiteral("不支持此配置格式或版本。");
        return false;
    }
    if (snapshot.value("specialization") != QJsonValue(0) && snapshot.value("specialization") != QJsonValue(1)) {
        error = QStringLiteral("心法编号无效。");
        return false;
    }
    const auto values = snapshot.value("values").toObject();
    if (values.value("mwStyle") != QJsonValue(0) && values.value("mwStyle") != QJsonValue(1)) {
        error = QStringLiteral("初始曲目无效。");
        return false;
    }
    if (values.keys() != Values().keys()) {
        error = QStringLiteral("配置缺少字段或包含未知字段。");
        return false;
    }
    auto invalid = [&](const QString &key) {
        error = QStringLiteral("配置值无效：%1").arg(desktop::FieldName(key));
        return false;
    };
    for (auto *input : findChildren<QSpinBox *>()) {
        const auto v = values.value(input->objectName());
        const auto n = v.toDouble();
        if (!v.isDouble() || !std::isfinite(n) || n < input->minimum() || n > input->maximum() || std::floor(n) != n) {
            return invalid(input->objectName());
        }
    }
    for (auto *input : findChildren<QDoubleSpinBox *>()) {
        const auto v     = values.value(input->objectName());
        const auto n     = v.toDouble();
        const auto scale = std::pow(10.0, input->decimals());
        if (!v.isDouble() || !std::isfinite(n) || n < input->minimum() || n > input->maximum() ||
            std::abs(n * scale - std::round(n * scale)) > 0.0001)
        {
            return invalid(input->objectName());
        }
    }
    for (auto *input : findChildren<QCheckBox *>()) {
        if (!values.value(input->objectName()).isBool()) {
            return invalid(input->objectName());
        }
    }
    for (auto *input : m_permanents) {
        const auto value = values.value(input->objectName());
        if (!value.isDouble() || value.toDouble() != value.toInt() || value.toInt() < 0 || value.toInt() >= input->count())
        {
            return invalid(input->objectName());
        }
    }
    const auto seed  = values.value("seed");
    bool       valid = false;
    seed.toString().toULongLong(&valid);
    if (!seed.isString() || !valid || !QRegularExpression(QStringLiteral("^[0-9]{1,20}$")).match(seed.toString()).hasMatch())
    {
        return invalid(QStringLiteral("seed"));
    }
    try {
        // Validate coupled rules (recipes, haste, etc.) before changing any control.
        std::visit([](const auto &config) { desktop::MakeRules(config); },
                   ActiveConfigFromValues(values, static_cast<desktop::Specialization>(snapshot.value("specialization").toInt())));
    } catch (const std::exception &exception) {
        error = desktop::Diagnostic(QString::fromUtf8(exception.what()));
        return false;
    }
    return true;
}

QJsonObject SimulationOptions::NormalizeSnapshot(const QJsonObject &snapshot) const
{
    auto withTeamDefaults = [this](QJsonObject result) {
        auto values = result.value("values").toObject();
        for (auto *input : findChildren<QWidget *>()) {
            const auto key = input->objectName();
            if (!key.startsWith("txTeam") || values.contains(key)) continue;
            if (qobject_cast<QCheckBox *>(input)) values.insert(key, false);
            else if (qobject_cast<QSpinBox *>(input) || qobject_cast<QDoubleSpinBox *>(input))
                values.insert(key, QJsonValue::fromVariant(input->property("defaultValue")));
        }
        result.insert("values", values);
        return result;
    };
    if (snapshot.value("format") != QJsonValue("jx3dps.tai-xu.config") || snapshot.value("version") != QJsonValue(1)) {
        return snapshot.value("format") == QJsonValue("jx3dps.desktop.config") && snapshot.value("version") == QJsonValue(2)
            ? withTeamDefaults(snapshot) : snapshot;
    }
    auto converted = snapshot;
    auto values    = snapshot.value("values").toObject();
    for (const auto &key : values.keys()) {
        if (key.startsWith("mw") || key.startsWith("permanent") || key.startsWith("imported")) {
            return snapshot;
        }
    }
    // Older Tai Xu snapshots predate the Mo Wen fields; fill only these new keys.
    for (auto *input : findChildren<QSpinBox *>()) {
        if (input->objectName().startsWith("mw")) {
            values.insert(input->objectName(), input->property("defaultValue").toInt());
        }
    }
    for (auto *input : findChildren<QCheckBox *>()) {
        if (input->objectName().startsWith("mw")) {
            values.insert(input->objectName(), false);
        }
    }
    values.insert("mwStyle", 1);
    values.insert("importedMajor", 0);
    values.insert("importedStrength", 41);
    for (auto *input : m_permanents) {
        values.insert(input->objectName(), 0);
    }
    converted.insert("format", "jx3dps.desktop.config");
    converted.insert("version", 2);
    converted.insert("specialization", 0);
    converted.insert("values", values);
    return withTeamDefaults(converted);
}

bool SimulationOptions::RestoreSnapshot(const QJsonObject &original, QString &error)
{
    const auto snapshot = NormalizeSnapshot(original);
    if (!ValidateSnapshot(snapshot, error)) {
        return false;
    }
    const auto values = snapshot.value("values").toObject();
    for (auto *input : findChildren<QSpinBox *>()) {
        input->setValue(values.value(input->objectName()).toInt());
    }
    for (auto *input : findChildren<QDoubleSpinBox *>()) {
        input->setValue(values.value(input->objectName()).toDouble());
    }
    for (auto *input : findChildren<QCheckBox *>()) {
        input->setChecked(values.value(input->objectName()).toBool());
    }
    m_seed->setText(values.value("seed").toString());
    m_style->setCurrentIndex(values.value("mwStyle").toInt());
    for (auto *input : m_permanents) {
        input->setCurrentIndex(values.value(input->objectName()).toInt());
    }
    m_specialization->setCurrentIndex(snapshot.value("specialization").toInt());
    return true;
}

bool SimulationOptions::ExportConfiguration(const QString &path, QString &error) const
{
    const auto snapshot = Snapshot();
    if (!ValidateSnapshot(snapshot, error)) {
        return false;
    }
    const auto bytes = QJsonDocument(snapshot).toJson();
    QSaveFile  file(path);
    if (!file.open(QIODevice::WriteOnly) || file.write(bytes) != bytes.size() || !file.commit()) {
        error = file.errorString();
        return false;
    }
    return true;
}

bool SimulationOptions::ImportConfiguration(const QString &path, QString &error)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        error = file.errorString();
        return false;
    }
    if (file.size() > 1024 * 1024) {
        error = QStringLiteral("配置文件超过 1 MiB。");
        return false;
    }
    const auto bytes = file.readAll();
    if (file.error() != QFileDevice::NoError) {
        error = file.errorString();
        return false;
    }
    QJsonParseError parse;
    const auto      document = QJsonDocument::fromJson(bytes, &parse);
    if (parse.error != QJsonParseError::NoError || !document.isObject()) {
        error = QStringLiteral("配置文件格式错误，请检查文件内容（位置 %1）。").arg(parse.offset);
        return false;
    }
    return RestoreSnapshot(document.object(), error);
}

bool SimulationOptions::ImportAttributes(const QJsonObject &attributes, QString &error)
{
    auto                              snapshot = Snapshot();
    auto                              values   = snapshot.value("values").toObject();
    const bool                        moWen    = Specialization() == desktop::Specialization::MoWen;
    const QString                     prefix   = moWen ? QStringLiteral("Lunar") : QStringLiteral("Physics");
    const QString                     majorKey = moWen ? QStringLiteral("Spirit") : QStringLiteral("Agility");
    const std::pair<QString, QString> fields[] = {
        { prefix + "AttackPowerBase",     "attackBase"       },
        { prefix + "CriticalStrike",      "crit"             },
        { prefix + "CriticalDamagePower", "critPower"        },
        { prefix + "Overcome",            "overcomeBase"     },
        { "SurplusValue",                 "surplus"          },
        { "Strain",                       "strain"           },
        { "Haste",                        "haste"            },
        { majorKey,                       "importedMajor"    },
        { "Strength",                     "importedStrength" },
        { "MeleeWeaponDamage",            "weapon"           }
    };
    auto read = [&](const QString &key, int &number) {
        const auto value = attributes.value(key);
        if (!value.isDouble() || !std::isfinite(value.toDouble()) || value.toDouble() < 0 ||
            value.toDouble() > 1000000 || std::floor(value.toDouble()) != value.toDouble())
        {
            error = QStringLiteral("魔盒属性缺少有效的整数项目：%1").arg(key);
            return false;
        }
        number = value.toInt();
        return true;
    };
    for (const auto &[source, destination] : fields) {
        int value = 0;
        if (!read(source, value)) {
            return false;
        }
        values.insert(destination, value);
    }
    int randomWeapon = 0;
    if (!read(QStringLiteral("MeleeWeaponDamageRand"), randomWeapon)) {
        return false;
    }
    values.insert("weapon", values.value("weapon").toInt() + randomWeapon / 2);
    const auto major = static_cast<value_t>(values.value("importedMajor").toInt());
    values.insert("attackClass", static_cast<int>(major * (moWen ? 1895 : 1485) / 1024));
    values.insert("overcomeClass", 0);
    for (const char *key :
         { "attackBonus", "overcomeBonus", "critBonus", "critPowerBonus", "hasteBonus", "damageBonus", "strainBonus", "ignoreShield" })
    {
        values.insert(key, 0);
    }
    values.insert("pveBonus", std::round((moWen ? 61 : 184) * 100000.0 / 1024) / 1000);
    for (auto *input : m_permanents) {
        values.insert(input->objectName(), 0);
    }
    snapshot.insert("values", values);
    return RestoreSnapshot(snapshot, error);
}

void SimulationOptions::SetupProfiles(QVBoxLayout *layout)
{
    auto *group    = layout->parentWidget();
    auto *rows     = layout;
    auto *profiles = new QComboBox(group);
    profiles->setObjectName(QStringLiteral("configurationProfile"));
    profiles->setEditable(true);
    profiles->setInsertPolicy(QComboBox::NoInsert);
    profiles->setAccessibleName(QStringLiteral("方案名称"));
    profiles->setToolTip(QStringLiteral("输入名称保存当前配置，或选择已有方案加载。"));
    profiles->lineEdit()->setMaxLength(100);
    const auto saved = QJsonDocument::fromJson(QSettings().value(QStringLiteral("simulation/profiles")).toByteArray()).object();
    profiles->addItems(saved.keys());
    profiles->setCurrentIndex(-1);
    profiles->setPlaceholderText(QStringLiteral("输入方案名称或选择已有方案"));
    profiles->lineEdit()->setPlaceholderText(QStringLiteral("输入方案名 / 选择已存方案"));
    auto placeholderPalette = profiles->lineEdit()->palette();
    placeholderPalette.setColor(QPalette::PlaceholderText, QColor(QStringLiteral("#94a5b0")));
    profiles->lineEdit()->setPalette(placeholderPalette);
    rows->addWidget(profiles);
    auto *buttons = new QHBoxLayout;
    auto  button  = [&](const QString &label, const char *name, auto callback) {
        auto *item = new QPushButton(label, group);
        item->setObjectName(QString::fromLatin1(name));
        buttons->addWidget(item);
        connect(item, &QPushButton::clicked, this, callback);
    };
    auto read = [] {
        return QJsonDocument::fromJson(QSettings().value(QStringLiteral("simulation/profiles")).toByteArray()).object();
    };
    auto write = [](const QJsonObject &data) {
        QSettings().setValue(QStringLiteral("simulation/profiles"), QJsonDocument(data).toJson(QJsonDocument::Compact));
    };
    auto report = [this](const QString &error) {
        QMessageBox::warning(this, QStringLiteral("配置"), error);
    };
    button(QStringLiteral("保存"), "saveProfile", [=, this] {
        const auto name = profiles->currentText().trimmed();
        if (name.isEmpty()) {
            report(QStringLiteral("请先输入方案名称。"));
            return;
        }
        QString    error;
        const auto snapshot = Snapshot();
        if (!ValidateSnapshot(snapshot, error)) {
            report(error);
            return;
        }
        auto data = read();
        data.insert(name, snapshot);
        write(data);
        if (profiles->findText(name) < 0) {
            profiles->addItem(name);
        }
        profiles->setCurrentText(name);
    });
    button(QStringLiteral("加载"), "loadProfile", [=, this] {
        const auto name = profiles->currentText().trimmed();
        QString    error;
        const auto data = read();
        if (!data.contains(name)) {
            report(QStringLiteral("请先选择已保存的方案。"));
            return;
        }
        if (!RestoreSnapshot(data.value(name).toObject(), error)) {
            report(error);
        }
    });
    auto *more = new QToolButton(group);
    more->setText(QStringLiteral("更多"));
    more->setPopupMode(QToolButton::InstantPopup);
    auto *menu = new QMenu(more);
    menu->setObjectName(QStringLiteral("configurationMenu"));
    more->setMenu(menu);
    buttons->addWidget(more);
    rows->addLayout(buttons);
    auto menuAction = [&](const QString &label, const char *name, auto callback) {
        auto *item = menu->addAction(label);
        item->setObjectName(QString::fromLatin1(name));
        connect(item, &QAction::triggered, this, callback);
    };
    menuAction(QStringLiteral("删除方案"), "deleteProfile", [=, this] {
        const auto name = profiles->currentText().trimmed();
        auto       data = read();
        if (!data.contains(name)) {
            return;
        }
        if (QMessageBox::question(this, QStringLiteral("删除方案"), QStringLiteral("确定删除“%1”？").arg(name)) != QMessageBox::Yes)
        {
            return;
        }
        data.remove(name);
        write(data);
        profiles->removeItem(profiles->findText(name));
    });
    menuAction(QStringLiteral("导入配置"), "importConfiguration", [=, this] {
        const auto path = QFileDialog::getOpenFileName(this, QStringLiteral("导入配置"), {}, QStringLiteral("配置文件 (*.json)"));
        if (path.isEmpty()) {
            return;
        }
        QString error;
        if (!ImportConfiguration(path, error)) {
            report(error);
        } else {
            profiles->setCurrentIndex(-1);
        }
    });
    menuAction(QStringLiteral("导出配置"), "exportConfiguration", [=, this] {
        const auto path =
            QFileDialog::getSaveFileName(this, QStringLiteral("导出配置"), QStringLiteral("心法配置.json"), QStringLiteral("配置文件 (*.json)"));
        if (path.isEmpty()) {
            return;
        }
        QString error;
        if (!ExportConfiguration(path, error)) {
            report(error);
        }
    });
    menu->addSeparator();
}
