#pragma once

#include <cmath>
#include <functional>

#include <QAbstractButton>
#include <QButtonGroup>
#include <QFrame>
#include <QLabel>
#include <QPainter>
#include <QPainterPath>
#include <QPushButton>
#include <QScrollArea>
#include <QStackedWidget>
#include <QToolButton>
#include <QVBoxLayout>

#include "animated_navigation.h"
#include "workbench_theme.h"

namespace desktop {
enum class ShellIcon
{
    TERMINAL,
    FILE,
    LIBRARY,
    SLIDERS,
    NODES,
    SHIELD,
    CHART,
    TIMELINE,
    HELP,
    SETTINGS,
    PANEL,
    REFRESH
};

inline QIcon LineIcon(ShellIcon kind)
{
    QIcon icon;
    for (const auto MODE : { QIcon::Normal, QIcon::Active, QIcon::Selected }) {
        QPixmap image(40, 40);
        image.setDevicePixelRatio(2);
        image.fill(Qt::transparent);
        QPainter p(&image);
        p.setRenderHint(QPainter::Antialiasing);
        p.setPen(QPen(QColor(MODE == QIcon::Normal ? theme::Muted : theme::Text), 1.5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        p.setBrush(Qt::NoBrush);
        switch (kind) {
            case ShellIcon::TERMINAL:
                p.drawPolyline(QPolygonF{
                    { 4, 5  },
                    { 8, 9  },
                    { 4, 13 }
                });
                p.drawLine(QPointF(10, 14), QPointF(16, 14));
                break;
            case ShellIcon::FILE:
                p.drawRoundedRect(QRectF(4, 3, 12, 14), 1, 1);
                p.drawLine(7, 7, 13, 7);
                p.drawLine(7, 10, 13, 10);
                p.drawLine(7, 13, 11, 13);
                break;
            case ShellIcon::LIBRARY:
                p.drawPolyline(QPolygonF{
                    { 5,  3  },
                    { 15, 3  },
                    { 15, 16 },
                    { 4,  16 },
                    { 4,  13 },
                    { 12, 13 }
                });
                break;
            case ShellIcon::SLIDERS:
                p.drawLine(4, 5, 16, 5);
                p.drawLine(4, 10, 16, 10);
                p.drawLine(4, 15, 16, 15);
                p.setBrush(QColor(theme::Canvas));
                p.drawEllipse(QPointF(8, 5), 2, 2);
                p.drawEllipse(QPointF(13, 10), 2, 2);
                p.drawEllipse(QPointF(7, 15), 2, 2);
                break;
            case ShellIcon::NODES:
                p.drawPolyline(QPolygonF{
                    { 4,  10 },
                    { 8,  10 },
                    { 13, 4  },
                    { 16, 4  }
                });
                p.drawPolyline(QPolygonF{
                    { 8,  10 },
                    { 13, 16 },
                    { 16, 16 }
                });
                p.drawPolyline(QPolygonF{
                    { 14, 2 },
                    { 16, 4 },
                    { 14, 6 }
                });
                p.drawPolyline(QPolygonF{
                    { 14, 14 },
                    { 16, 16 },
                    { 14, 18 }
                });
                break;
            case ShellIcon::SHIELD:
                p.drawPolygon(QPolygonF{
                    { 10, 2  },
                    { 16, 5  },
                    { 16, 11 },
                    { 14, 15 },
                    { 10, 18 },
                    { 6,  15 },
                    { 4,  11 },
                    { 4,  5  }
                });
                break;
            case ShellIcon::CHART:
                p.drawLine(4, 15, 4, 8);
                p.drawLine(8, 15, 8, 4);
                p.drawLine(12, 15, 12, 10);
                p.drawLine(16, 15, 16, 6);
                break;
            case ShellIcon::TIMELINE:
                p.drawRoundedRect(QRectF(3, 3, 14, 5), 1, 1);
                p.drawRoundedRect(QRectF(3, 12, 14, 5), 1, 1);
                p.drawLine(6, 5, 9, 5);
                p.drawLine(9, 14, 14, 14);
                break;
            case ShellIcon::HELP:
                p.drawEllipse(QRectF(3, 3, 14, 14));
                p.drawArc(QRectF(7, 5, 6, 6), 0, 200 * 16);
                p.drawLine(10, 10, 10, 11);
                p.drawPoint(10, 14);
                break;
            case ShellIcon::SETTINGS:
            {
                QPolygonF gear;
                for (int i = 0; i < 24; ++i) {
                    const qreal A = i * 3.141592653589793 / 12;
                    const qreal R = i % 4 == 0 || i % 4 == 3 ? 8 : 6;
                    gear << QPointF(10 + (R * std::cos(A)), 10 + (R * std::sin(A)));
                }
                p.drawPolygon(gear);
                p.drawEllipse(QPointF(10, 10), 2.8, 2.8);
                break;
            }
            case ShellIcon::PANEL:
                p.drawRoundedRect(QRectF(3, 4, 14, 12), 2, 2);
                p.drawLine(8, 4, 8, 16);
                break;
            case ShellIcon::REFRESH:
                p.drawArc(QRectF(4, 4, 12, 12), 35 * 16, 290 * 16);
                p.drawPolyline(QPolygonF{
                    { 16, 3 },
                    { 16, 7 },
                    { 12, 7 }
                });
                break;
        }
        p.end();
        icon.addPixmap(image, MODE);
    }
    return icon;
}

class ToggleSwitch final : public QAbstractButton
{
public:
    explicit ToggleSwitch(QWidget *parent = nullptr) : QAbstractButton(parent)
    {
        setCheckable(true);
        setCursor(Qt::PointingHandCursor);
        setFocusPolicy(Qt::StrongFocus);
        setFixedSize(38, 24);
    }

protected:
    void paintEvent(QPaintEvent *) override
    {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);
        p.setPen(Qt::NoPen);
        p.setBrush(QColor(isChecked() ? theme::Text : theme::Raised));
        p.drawRoundedRect(QRectF(1, 2, 36, 20), 10, 10);
        p.setBrush(QColor(isChecked() ? theme::Canvas : "#ffffff"));
        p.drawEllipse(QRectF(isChecked() ? 20 : 3, 4, 16, 16));
        if (hasFocus()) {
            p.setPen(QPen(theme::AccentColor(this), 1));
            p.setBrush(Qt::NoBrush);
            p.drawRoundedRect(QRectF(.5, .5, 37, 23), 11, 11);
        }
    }
};

class MagpieShell final : public QWidget
{
public:
    QStackedWidget                      *pages;
    QVBoxLayout                         *bodyLayout;
    QHBoxLayout                         *headerLayout;
    QLabel                              *breadcrumb;
    QLabel                              *engineStatus;
    QToolButton                         *refresh;
    std::function<void(const QString &)> navigate;

    explicit MagpieShell(QWidget *parent = nullptr) : QWidget(parent)
    {
        auto *layout = new QVBoxLayout(this);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->setSpacing(0);
        auto *navigation = new QFrame(this);
        navigation->setObjectName(QStringLiteral("topNavigation"));
        navigation->setFixedHeight(theme::NavigationHeight);
        auto *navigation_layout = new QHBoxLayout(navigation);
        navigation_layout->setContentsMargins(18, 8, 18, 8);
        navigation_layout->addStretch();
        auto *items = new NavigationStrip(true, navigation);
        items->setObjectName(QStringLiteral("centeredNavigation"));
        auto *list = new QHBoxLayout(items);
        list->setContentsMargins(0, 0, 0, 0);
        list->setSpacing(4);
        AddItem(list, "simulation", QStringLiteral("模拟工作台"), ShellIcon::TERMINAL);
        character_ = AddItem(list, "character", QStringLiteral("角色配置"), ShellIcon::SLIDERS);
        AddItem(list, "macro", QStringLiteral("宏编辑与调试"), ShellIcon::FILE);
        analysis_ = AddItem(list, "analysis", QStringLiteral("结果分析"), ShellIcon::CHART);
        list->addSpacing(12);
        settings_ = AddItem(list, "settings", QStringLiteral("设置"), ShellIcon::SETTINGS);
        navigation_layout->addWidget(items);
        navigation_layout->addStretch();
        layout->addWidget(navigation);

        auto *body = new QWidget;
        body->setObjectName(QStringLiteral("workspaceBody"));
        bodyLayout = new QVBoxLayout(body);
        bodyLayout->setContentsMargins(18, 0, 18, 12);
        bodyLayout->setSpacing(12);
        auto *header = header_ = new QWidget;
        header->setFixedHeight(44);
        headerLayout = new QHBoxLayout(header);
        headerLayout->setContentsMargins(2, 0, 0, 0);
        headerLayout->setSpacing(10);
        breadcrumb = new QLabel;
        breadcrumb->setObjectName(QStringLiteral("breadcrumb"));
        headerLayout->addWidget(breadcrumb);
        const auto CHILDREN = [this](QFrame *&frame, const char *name) {
            frame = new NavigationStrip(false);
            frame->setObjectName(QString::fromLatin1(name));
            frame->setProperty("navigationChildren", true);
            auto *row = new QHBoxLayout(frame);
            row->setContentsMargins(3, 3, 3, 3);
            row->setSpacing(3);
            headerLayout->addWidget(frame);
            frame->hide();
            return row;
        };
        auto *character_row = CHILDREN(characterChildren_, "characterChildren");
        AddItem(character_row, "attributes", QStringLiteral("属性与配装"), ShellIcon::SLIDERS, true);
        AddItem(character_row, "talents", QStringLiteral("奇穴与秘籍"), ShellIcon::NODES, true);
        AddItem(character_row, "permanents", QStringLiteral("常驻增益"), ShellIcon::SHIELD, true);
        AddItem(character_row, "profiles", QStringLiteral("配置方案"), ShellIcon::LIBRARY, true);
        auto *analysis_row = CHILDREN(analysisChildren_, "analysisChildren");
        for (auto [key, text] :
             {
                 std::pair{ "statistics", "模拟历史"   },
                 { "skills",     "技能统计"   },
                 { "gains",      "属性收益"   },
                 { "hotspots",   "性能热点"   },
                 { "timeline",   "战斗时间轴" },
                 { "events",     "伤害事件"   },
                 { "buffs",      "增益分析"   }
        })
        {
            AddItem(analysis_row, key, QString::fromUtf8(text), ShellIcon::CHART, true);
        }
        auto *settings_row = CHILDREN(settingsChildren_, "settingsChildren");
        AddItem(settings_row, "general", QStringLiteral("常规"), ShellIcon::SETTINGS, true);
        AddItem(settings_row, "about", QStringLiteral("关于"), ShellIcon::SETTINGS, true);
        headerLayout->addStretch();
        engineStatus = new QLabel(QStringLiteral("<span style='color:#6fd99b'>●</span> &nbsp;本地引擎 · 就绪"));
        engineStatus->setObjectName(QStringLiteral("engineStatus"));
        engineStatus->setProperty("sectionTitle", true);
        headerLayout->addWidget(engineStatus);
        refresh = new QToolButton;
        refresh->setObjectName(QStringLiteral("checkWorkspace"));
        refresh->setIcon(LineIcon(ShellIcon::REFRESH));
        refresh->setToolTip(QStringLiteral("重新检查当前宏"));
        refresh->setAccessibleName(refresh->toolTip());
        refresh->setFixedSize(30, 30);
        headerLayout->addWidget(refresh);
        bodyLayout->addWidget(header);
        pages = new QStackedWidget;
        pages->setObjectName(QStringLiteral("workspacePages"));
        bodyLayout->addWidget(pages, 1);
        layout->addWidget(body, 1);
        Select(QStringLiteral("simulation"));
    }

    void Select(const QString &id)
    {
        const auto         KEY      = Destination(id);
        const QPushButton *selected = nullptr;
        for (auto *button : buttons_) {
            if (button->property("pageId").toString() == KEY) {
                selected = button;
            }
        }
        if (selected == nullptr) {
            return;
        }
        for (auto *button : buttons_) {
            button->setChecked(button == selected);
        }
        for (auto [group, parent] :
             {
                 std::pair{ characterChildren_, character_ },
                 { analysisChildren_,  analysis_  },
                 { settingsChildren_,  settings_  }
        })
        {
            const bool ACTIVE = group == selected->parentWidget();
            parent->setChecked(ACTIVE);
            group->setVisible(ACTIVE);
        }
        breadcrumb->setText(selected->text().toHtmlEscaped());
        breadcrumb->setVisible(!selected->property("navChild").toBool());
    }

    void SetCompactHeader(bool compact)
    {
        header_->setFixedHeight(compact ? 34 : 44);
        bodyLayout->setSpacing(compact ? 4 : 12);
    }

private:
    static QString Destination(const QString &id)
    {
        return id == "settings"    ? QStringLiteral("general")
               : id == "character" ? QStringLiteral("attributes")
               : id == "analysis"  ? QStringLiteral("statistics")
                                   : id;
    }

    QPushButton *AddItem(QBoxLayout *layout, const char *id, const QString &text, ShellIcon icon, bool child = false)
    {
        auto *button = new NavigationButton(text, child);
        button->setObjectName(QStringLiteral("nav_%1").arg(QString::fromLatin1(id)));
        button->setProperty("navItem", true);
        button->setProperty("navChild", child);
        button->setProperty("pageId", QString::fromLatin1(id));
        button->setAccessibleName(text);
        if (!child) {
            button->setIcon(LineIcon(icon));
            button->setIconSize(QSize(18, 18));
        }
        button->setCheckable(true);
        button->setFixedHeight(child ? 29 : 34);
        button->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
        layout->addWidget(button);
        static_cast<NavigationStrip *>(layout->parentWidget())->AddButton(button);
        buttons_.append(button);
        connect(button, &QPushButton::clicked, this, [this, key = QString::fromLatin1(id)] {
            Select(key);
            if (navigate) {
                navigate(Destination(key));
            }
        });
        return button;
    }

    QFrame              *characterChildren_{};
    QWidget             *header_;
    QFrame              *settingsChildren_{};
    QFrame              *analysisChildren_{};
    QPushButton         *analysis_;
    QPushButton         *character_;
    QPushButton         *settings_;
    QList<QPushButton *> buttons_;
};
} // namespace desktop
