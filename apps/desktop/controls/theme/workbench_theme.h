#pragma once

#include <QString>
#include <utility>
#include "accent_color.h"

namespace desktop::theme {
// Sampled from the supplied Magpie screenshots; dimensions use Qt logical pixels.
inline constexpr const char *Canvas = "#1a1a1e";
inline constexpr const char *Surface = "#232327";
inline constexpr const char *Field = "#232327";
inline constexpr const char *Raised = "#2d2d33";
inline constexpr const char *Border = "#333339";
inline constexpr const char *Text = "#ededf1";
inline constexpr const char *Muted = "#92929d";
inline constexpr const char *Selection = "#27272c";
inline constexpr const char *Cyan = "#6fd99b";
inline constexpr const char *Amber = "#e6bc74";
inline constexpr const char *Success = "#6fd99b";
inline constexpr const char *Error = "#f08078";
inline constexpr int NavigationHeight = 52;
inline constexpr int TitleBarHeight = 30;

inline QString WorkbenchStyleSheet(const QColor &accent = DefaultAccent())
{
    auto sheet = QStringLiteral(R"(
        QWidget { accent-color: $accent; }
        QWidget#workbenchContent, QDialog#resultAnalysis, QWidget#simulationPage,
        QWidget#workspaceBody, QWidget#settingsPage, QWidget#aboutPage {
            background: $canvas;
        }
        QLabel, QCheckBox { color: $text; font-weight: 400; background: transparent; }
        QLabel[muted="true"] { color: $muted; }
        QLabel[sectionTitle="true"] { color: $muted; font-size: 12px; }
        QLabel[cardTitle="true"] { color: $text; font-weight: 600; }
        QWidget[statSection="true"] { background: $canvas; border: 0; }
        QWidget[statModule="true"] { background: $surface; border: 1px solid $border; border-radius: 12px; }
        QFrame#topNavigation { background: $canvas; border: 0; border-bottom: 1px solid $border; }
        QFrame[navigationChildren="true"] { background: $raised; border: 0; border-radius: 8px; }
        QPushButton[navItem="true"] {
            background: transparent; border: 1px solid transparent; border-radius: 8px;
            padding: 0 10px; text-align: center; color: #b6b6c0; font-size: 14px;
            min-height: 32px; max-height: 32px;
        }
        QPushButton[navItem="true"]:hover { background: #202024; color: $text; }
        QPushButton[navItem="true"]:checked {
            background: $surface; border-color: #2b2b30; color: $text; font-weight: 600;
        }
        QPushButton[navChild="true"] { border: 0; border-radius: 6px; padding: 0 10px; min-height: 27px; max-height: 27px; }
        QPushButton[navChild="true"]:checked { background: $surface; border: 0; }
        QLabel#breadcrumb { font-size: 15px; color: $text; }
        QGroupBox {
            background: $surface; border: 1px solid $border; border-radius: 12px;
            margin-top: 28px; padding-top: 0; font-weight: 600;
        }
        QGroupBox::title {
            subcontrol-origin: margin; subcontrol-position: top left;
            left: 2px; top: 0; color: $muted; font-size: 14px;
        }
        QGroupBox#configurationSchemes, QGroupBox#outputPanel { margin-top: 0; padding-top: 0; }
        QFrame[settingsCard="true"] { background: $surface; border: 1px solid $border; border-radius: 13px; }
        QFrame[settingsRow="true"] { background: transparent; border: 0; border-bottom: 1px solid #2c2c32; }
        QFrame[settingsLastRow="true"] { border-bottom: 0; }
        QTabWidget::pane { border: 0; background: $surface; top: 8px; border-radius: 8px; }
        QTabWidget#resultTabs::pane, QTabWidget#analysisViews::pane { background: $canvas; }
        QTextBrowser#resultDetails { border: 1px solid $border; border-radius: 12px; padding: 12px; }
        QTabBar { background: $raised; border: 0; border-radius: 8px; qproperty-drawBase: false; }
        QTabBar::tab { margin: 3px; padding: 4px 11px; background: transparent; color: $muted; border: 0; border-radius: 6px; }
        QTabBar::tab:selected { background: $surface; color: $text; }
        QTabBar::tab:!selected:hover { color: $text; }
        QTabBar[segmentedControl="true"] { background: transparent; border: 0; }
        QTabBar[segmentedControl="true"]::tab { background: transparent; border: 0; margin: 0; padding: 0; }
        QToolBar { background: transparent; border: 0; spacing: 4px; }
        QToolButton { background: transparent; color: #b6b6c0; border: 1px solid transparent; border-radius: 6px; padding: 4px 8px; font-weight: 400; }
        QToolButton:hover { background: $raised; color: $text; }
        QToolButton:pressed, QToolButton:checked { background: $raised; color: $text; }
        QToolButton#runMacroButton { background: $accent; color: $onAccent; font-weight: 600; padding: 5px 12px; }
        QToolButton#runMacroButton:hover { background: $accentHover; }
        QToolButton#runMacroButton:pressed { background: $accentPressed; }
        QToolButton#runMacroButton:disabled { background: $raised; color: #72727d; }
        QToolButton#legacyTalentToggle, QToolButton#clearTalentPreview { padding: 2px 3px; font-size: 12px; }
        QPushButton {
            min-height: 23px; padding: 2px 10px; border-radius: 7px;
            background: $surface; color: #b6b6c0; border: 1px solid $border; font-weight: 400;
        }
        QPushButton:hover { background: $raised; color: $text; border-color: #43434b; }
        QPushButton:pressed { background: $selection; }
        QPushButton#toggleResultDetails:checked { background: $raised; color: $text; }
        QPushButton:focus, QToolButton:focus { border-color: $accent; }
        QPushButton[primary="true"] { background: $accent; color: $onAccent; border: 0; font-weight: 600; }
        QPushButton[primary="true"]:hover { background: $accentHover; }
        QPushButton[primary="true"]:pressed { background: $accentPressed; }
        QPushButton[moduleAction="true"], QToolButton[moduleAction="true"] {
            min-height: 24px; max-height: 24px; padding: 0 9px; border: 1px solid $border;
            border-radius: 7px; background: $surface; color: $text; font-weight: 400;
        }
        QPushButton[moduleAction="true"]:hover, QToolButton[moduleAction="true"]:hover { background: $raised; border-color: #47474f; }
        QPushButton[moduleAction="true"]:pressed, QToolButton[moduleAction="true"]:pressed { background: $selection; }
        QPushButton[moduleAction="true"]:focus, QToolButton[moduleAction="true"]:focus { border-color: $accent; }
        QLabel[gainStatus="true"] { background: $raised; color: $muted; border: 0; border-radius: 4px; padding: 0 6px; }
        QPushButton:disabled, QToolButton:disabled { color: #56565f; border-color: #2c2c32; }
        QComboBox, QSpinBox, QDoubleSpinBox, QLineEdit {
            min-height: 24px; padding: 2px 9px; background: $field; color: $text;
            border: 1px solid $border; border-radius: 7px; font-weight: 400;
            selection-background-color: #414154; placeholder-text-color: #56565f;
        }
        QComboBox { padding-right: 24px; }
        QComboBox::drop-down { border: 0; width: 24px; }
        QSpinBox::up-button, QDoubleSpinBox::up-button { subcontrol-origin: border; subcontrol-position: top right; width: 18px; border: 0; background: transparent; }
        QSpinBox::down-button, QDoubleSpinBox::down-button { subcontrol-origin: border; subcontrol-position: bottom right; width: 18px; border: 0; background: transparent; }
        QComboBox QAbstractItemView { background: $surface; color: $text; selection-background-color: $raised; border: 1px solid $border; padding: 4px; }
        QComboBox:hover, QSpinBox:hover, QDoubleSpinBox:hover, QLineEdit:hover { border-color: #47474f; }
        QComboBox:focus, QSpinBox:focus, QDoubleSpinBox:focus, QLineEdit:focus { border-color: $accent; }
        QComboBox:disabled, QSpinBox:disabled, QDoubleSpinBox:disabled, QLineEdit:disabled { color: #72727d; }
        QScrollArea, QScrollArea > QWidget > QWidget { background: transparent; border: 0; }
        QScrollBar:vertical { background: transparent; width: 5px; margin: 2px 0; }
        QScrollBar:horizontal { background: transparent; height: 5px; margin: 0 2px; }
        QScrollBar::handle { background: #56565f; border-radius: 2px; }
        QScrollBar::handle:vertical { min-height: 28px; }
        QScrollBar::handle:horizontal { min-width: 28px; }
        QScrollBar::handle:hover { background: #72727d; }
        QScrollBar::add-line, QScrollBar::sub-line { width: 0; height: 0; border: 0; }
        QScrollBar::add-page, QScrollBar::sub-page { background: transparent; }
        QTableWidget, QTableView, QListWidget {
            background: $surface; alternate-background-color: $surface; color: $text;
            border: 0; gridline-color: #2c2c32; selection-background-color: $raised; selection-color: $text;
        }
        QTableView::item { padding: 5px 8px; border: 0; border-bottom: 1px solid #2c2c32; }
        QHeaderView::section { background: $surface; color: $muted; border: 0; border-bottom: 1px solid $border; padding: 8px; font-weight: 400; }
        QTextBrowser, QPlainTextEdit { background: $surface; color: $text; border: 0; border-radius: 8px; font-weight: 400; }
        QSplitter::handle { background: transparent; }
        QSplitter::handle:hover { background: $border; }
        QProgressBar { background: $raised; border: 0; border-radius: 4px; color: $muted; font-size: 11px; text-align: center; }
        QProgressBar::chunk { background: $accent; border-radius: 4px; }
        QMenu { background: $surface; color: $text; border: 1px solid $border; padding: 5px; }
        QMenu::item { padding: 6px 22px; border-radius: 5px; }
        QMenu::item:selected { background: $raised; }
        QMenu::separator { height: 1px; background: $border; margin: 4px; }
    )");
    const std::pair<const char *, const char *> tokens[] = {
        {"$canvas", Canvas}, {"$surface", Surface}, {"$field", Field}, {"$raised", Raised},
        {"$border", Border}, {"$text", Text}, {"$muted", Muted}, {"$selection", Selection}
    };
    const auto colors = MakeAccentColors(accent);
    sheet.replace("$accentHover", colors.hover.name()).replace("$accentPressed", colors.pressed.name())
        .replace("$onAccent", colors.text.name()).replace("$accent", colors.normal.name());
    for (const auto &[key, value] : tokens) sheet.replace(QString::fromLatin1(key), QString::fromLatin1(value));
    return sheet;
}
} // namespace desktop::theme
