#pragma once

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
#include <functional>
#include <cmath>
#include "workbench_theme.h"
#include "animated_navigation.h"

namespace desktop {
enum class ShellIcon { Terminal, File, Library, Sliders, Nodes, Shield, Chart, Timeline, Help, Settings, Panel, Refresh };

inline QIcon LineIcon(ShellIcon kind)
{
    QIcon icon;
    for (const auto mode : {QIcon::Normal, QIcon::Active, QIcon::Selected}) {
        QPixmap image(40, 40);
        image.setDevicePixelRatio(2);
        image.fill(Qt::transparent);
        QPainter p(&image);
        p.setRenderHint(QPainter::Antialiasing);
        p.setPen(QPen(QColor(mode == QIcon::Normal ? theme::Muted : theme::Text), 1.5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        p.setBrush(Qt::NoBrush);
        switch (kind) {
        case ShellIcon::Terminal:
            p.drawPolyline(QPolygonF{{4, 5}, {8, 9}, {4, 13}}); p.drawLine(QPointF(10, 14), QPointF(16, 14)); break;
        case ShellIcon::File:
            p.drawRoundedRect(QRectF(4, 3, 12, 14), 1, 1); p.drawLine(7, 7, 13, 7); p.drawLine(7, 10, 13, 10); p.drawLine(7, 13, 11, 13); break;
        case ShellIcon::Library:
            p.drawPolyline(QPolygonF{{5, 3}, {15, 3}, {15, 16}, {4, 16}, {4, 13}, {12, 13}}); break;
        case ShellIcon::Sliders:
            p.drawLine(4, 5, 16, 5); p.drawLine(4, 10, 16, 10); p.drawLine(4, 15, 16, 15);
            p.setBrush(QColor(theme::Canvas)); p.drawEllipse(QPointF(8, 5), 2, 2); p.drawEllipse(QPointF(13, 10), 2, 2); p.drawEllipse(QPointF(7, 15), 2, 2); break;
        case ShellIcon::Nodes:
            p.drawPolyline(QPolygonF{{4, 10}, {8, 10}, {13, 4}, {16, 4}});
            p.drawPolyline(QPolygonF{{8, 10}, {13, 16}, {16, 16}});
            p.drawPolyline(QPolygonF{{14, 2}, {16, 4}, {14, 6}});
            p.drawPolyline(QPolygonF{{14, 14}, {16, 16}, {14, 18}}); break;
        case ShellIcon::Shield:
            p.drawPolygon(QPolygonF{{10, 2}, {16, 5}, {16, 11}, {14, 15}, {10, 18}, {6, 15}, {4, 11}, {4, 5}}); break;
        case ShellIcon::Chart:
            p.drawLine(4, 15, 4, 8); p.drawLine(8, 15, 8, 4); p.drawLine(12, 15, 12, 10); p.drawLine(16, 15, 16, 6); break;
        case ShellIcon::Timeline:
            p.drawRoundedRect(QRectF(3, 3, 14, 5), 1, 1); p.drawRoundedRect(QRectF(3, 12, 14, 5), 1, 1);
            p.drawLine(6, 5, 9, 5); p.drawLine(9, 14, 14, 14); break;
        case ShellIcon::Help:
            p.drawEllipse(QRectF(3, 3, 14, 14)); p.drawArc(QRectF(7, 5, 6, 6), 0, 200 * 16); p.drawLine(10, 10, 10, 11); p.drawPoint(10, 14); break;
        case ShellIcon::Settings: {
            QPolygonF gear;
            for (int i = 0; i < 24; ++i) {
                const qreal a = i * 3.141592653589793 / 12;
                const qreal r = i % 4 == 0 || i % 4 == 3 ? 8 : 6;
                gear << QPointF(10 + r * std::cos(a), 10 + r * std::sin(a));
            }
            p.drawPolygon(gear); p.drawEllipse(QPointF(10, 10), 2.8, 2.8); break;
        }
        case ShellIcon::Panel:
            p.drawRoundedRect(QRectF(3, 4, 14, 12), 2, 2); p.drawLine(8, 4, 8, 16); break;
        case ShellIcon::Refresh:
            p.drawArc(QRectF(4, 4, 12, 12), 35 * 16, 290 * 16);
            p.drawPolyline(QPolygonF{{16, 3}, {16, 7}, {12, 7}}); break;
        }
        p.end();
        icon.addPixmap(image, mode);
    }
    return icon;
}

class ToggleSwitch final : public QAbstractButton {
public:
    explicit ToggleSwitch(QWidget *parent = nullptr) : QAbstractButton(parent) {
        setCheckable(true);
        setCursor(Qt::PointingHandCursor);
        setFocusPolicy(Qt::StrongFocus);
        setFixedSize(38, 24);
    }
protected:
    void paintEvent(QPaintEvent *) override {
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

class MagpieShell final : public QWidget {
public:
    QStackedWidget *pages;
    QVBoxLayout *bodyLayout;
    QHBoxLayout *headerLayout;
    QLabel *breadcrumb;
    QLabel *engineStatus;
    QToolButton *refresh;
    std::function<void(const QString &)> navigate;

    explicit MagpieShell(QWidget *parent = nullptr) : QWidget(parent) {
        auto *layout = new QVBoxLayout(this);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->setSpacing(0);
        auto *navigation = new QFrame(this);
        navigation->setObjectName(QStringLiteral("topNavigation"));
        navigation->setFixedHeight(theme::NavigationHeight);
        auto *navigationLayout = new QHBoxLayout(navigation);
        navigationLayout->setContentsMargins(18, 8, 18, 8);
        navigationLayout->addStretch();
        auto *items = new NavigationStrip(true, navigation);
        items->setObjectName(QStringLiteral("centeredNavigation"));
        auto *list = new QHBoxLayout(items);
        list->setContentsMargins(0, 0, 0, 0);
        list->setSpacing(4);
        AddItem(list, "simulation", QStringLiteral("模拟"), ShellIcon::Terminal, QStringLiteral("使用"));
        AddItem(list, "macro", QStringLiteral("循环宏"), ShellIcon::File, QStringLiteral("使用"));
        AddItem(list, "profiles", QStringLiteral("配置库"), ShellIcon::Library, QStringLiteral("使用"));
        character_ = AddItem(list, "character", QStringLiteral("角色配置"), ShellIcon::Sliders, QStringLiteral("角色"));
        AddItem(list, "statistics", QStringLiteral("模拟统计"), ShellIcon::Chart, QStringLiteral("观测"));
        AddItem(list, "timeline", QStringLiteral("时间轴"), ShellIcon::Timeline, QStringLiteral("观测"));
        AddItem(list, "buffs", QStringLiteral("增益分析"), ShellIcon::Shield, QStringLiteral("观测"));
        list->addSpacing(12);
        settings_ = AddItem(list, "settings", QStringLiteral("设置"), ShellIcon::Settings, QStringLiteral("设置"));
        navigationLayout->addWidget(items);
        navigationLayout->addStretch();
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
        auto children = [this](QFrame *&frame, const char *name) {
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
        auto *characterRow = children(characterChildren_, "characterChildren");
        AddItem(characterRow, "attributes", QStringLiteral("属性与收益"), ShellIcon::Sliders, QStringLiteral("角色"), true);
        AddItem(characterRow, "talents", QStringLiteral("奇穴与秘籍"), ShellIcon::Nodes, QStringLiteral("角色"), true);
        AddItem(characterRow, "permanents", QStringLiteral("常驻增益"), ShellIcon::Shield, QStringLiteral("角色"), true);
        auto *settingsRow = children(settingsChildren_, "settingsChildren");
        AddItem(settingsRow, "general", QStringLiteral("常规"), ShellIcon::Settings, QStringLiteral("设置"), true);
        AddItem(settingsRow, "about", QStringLiteral("关于"), ShellIcon::Settings, QStringLiteral("设置"), true);
        headerLayout->addStretch();
        engineStatus = new QLabel(QStringLiteral("<span style='color:#6fd99b'>●</span> &nbsp;本地引擎 · 就绪"));
        engineStatus->setObjectName(QStringLiteral("engineStatus"));
        engineStatus->setProperty("sectionTitle", true);
        headerLayout->addWidget(engineStatus);
        refresh = new QToolButton;
        refresh->setObjectName(QStringLiteral("checkWorkspace"));
        refresh->setIcon(LineIcon(ShellIcon::Refresh));
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

    void Select(const QString &id) {
        const auto key = Destination(id);
        for (auto *button : buttons_) button->setChecked(button->property("pageId").toString() == key);
        const bool settings = key == "general" || key == "about";
        const bool character = key == "attributes" || key == "talents" || key == "permanents";
        settings_->setChecked(settings);
        character_->setChecked(character);
        settingsChildren_->setVisible(settings);
        characterChildren_->setVisible(character);
        for (auto *button : buttons_) if (button->property("pageId").toString() == key) {
            breadcrumb->setText(QStringLiteral("<span style='color:#92929d'>%1 &nbsp;/&nbsp; </span><b>%2</b>")
                .arg(button->property("category").toString().toHtmlEscaped(), button->text().trimmed().toHtmlEscaped()));
            break;
        }
    }
    void SetCompactHeader(bool compact) {
        header_->setFixedHeight(compact ? 34 : 44);
        bodyLayout->setSpacing(compact ? 4 : 12);
    }
private:
    static QString Destination(const QString &id) {
        return id == "settings" ? QStringLiteral("general") : id == "character" ? QStringLiteral("attributes") : id;
    }
    QPushButton *AddItem(QBoxLayout *layout, const char *id, const QString &text, ShellIcon icon, const QString &category, bool child = false) {
        auto *button = new NavigationButton(text, child);
        button->setObjectName(QStringLiteral("nav_%1").arg(QString::fromLatin1(id)));
        button->setProperty("navItem", true);
        button->setProperty("navChild", child);
        button->setProperty("pageId", QString::fromLatin1(id));
        button->setProperty("category", category);
        button->setAccessibleName(text);
        if (!child) { button->setIcon(LineIcon(icon)); button->setIconSize(QSize(18, 18)); }
        button->setCheckable(true);
        button->setFixedHeight(child ? 29 : 34);
        button->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
        layout->addWidget(button);
        static_cast<NavigationStrip *>(layout->parentWidget())->AddButton(button);
        buttons_.append(button);
        connect(button, &QPushButton::clicked, this, [this, key = QString::fromLatin1(id)] {
            Select(key);
            if (navigate) navigate(Destination(key));
        });
        return button;
    }
    QFrame *characterChildren_;
    QWidget *header_;
    QFrame *settingsChildren_;
    QPushButton *character_;
    QPushButton *settings_;
    QList<QPushButton *> buttons_;
};
} // namespace desktop
