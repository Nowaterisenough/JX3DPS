﻿/**
 * Project: JX3DPS
 * File: Widget.h
 * Description:
 * Created Date: 2023-06-07 01:15:59
 * Author: 难为水
 * -----
 * Last Modified: 2023-07-12 08:30:10
 * Modified By: 难为水
 * -----
 * HISTORY:
 * Date      	By   	Comments
 * ----------	-----	----------------------------------------------------------
 */

#ifndef WIDGET_H
#define WIDGET_H

#if defined _WIN32 || defined __CYGWIN__
#    if defined EXPORT_JX3DPSUI // CMake add_definitions
#        ifdef __GNUC__
#            define JX3DPSUI_API __attribute__((dllexport))
#        else
#            define JX3DPSUI_API __declspec(dllexport)
#        endif // __GNUC__
#    else
#        ifdef __GNUC__
#            define JX3DPSUI_API __attribute__((dllimport))
#        else
#            ifndef IMPORT_STATIC_JX3DPSUI_LIB
#                define JX3DPSUI_API __declspec(dllimport)
#            else
#                define JX3DPSUI_API
#            endif
#        endif // __GNUC__
#    endif     // EXPORT_JX3DPSUI
#    define JX3DPSUI_PRIVATE

#elif defined __GNUC__
#    if __GNUC__ >= 4
#        define JX3DPSUI_API     __attribute__((visibility("default")))
#        define JX3DPSUI_PRIVATE __attribute__((visibility("hidden")))
#    else
#        define JX3DPSUI_API
#        define JX3DPSUI_PRIVATE
#    endif // __GNUC__ >= 4

#elif defined __clang__
#    define JX3DPSUI_API     __attribute__((visibility("default")))
#    define JX3DPSUI_PRIVATE __attribute__((visibility("hidden")))

#else
#    error "Do not know how to export classes for this platform"
#endif // defined(_WIN32) || defined(__CYGWIN__)

#include <unordered_map>

#include <QWidget>

#include <nlohmann/json.hpp>

#include "Core/Attr.h"

class FramelessWidget;
class TalentWidget;
class ProgressBar;
class SecretCheckBox;
class CheckBox;

class JX3DPSUI_API Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);

    void SetProgress(double value);

signals:
    void Signal_UpdateAttr();
    void Signal_UpdateGains(const nlohmann::json &json);
    void Signal_UpdateStats(const nlohmann::json &json);
    void Signal_UpdateProgress(double value);

protected:
    void paintEvent(QPaintEvent *event);

private:
    FramelessWidget *m_frameless = nullptr;

    bool InitClass(JX3DPS::Class classType);
    void LoadSkills(int index);
    void LoadEvents(int index);

    void InitWidgetSetting(QWidget *parent = nullptr);
    void InitWidgetOut(QWidget *parent = nullptr);
    void InitWidgetAttr(QWidget *parent = nullptr);
    void InitWidgetEchant(QWidget *parent = nullptr);
    void InitWidgetAttrGain(QWidget *parent = nullptr);
    void InitWidgetTalent(QWidget *parent = nullptr);
    void InitWidgetSecret(QWidget *parent = nullptr);
    void InitWidgetSkill(QWidget *parent = nullptr);
    void InitWidgetSkillHelp(QWidget *parent = nullptr);
    void InitWidgetEvent(QWidget *parent = nullptr);
    void InitWidgetSetEffect(QWidget *parent = nullptr);

    QWidget *m_plainTextEditSkill    = nullptr;
    QWidget *m_plainTextEditEvent    = nullptr;
    QWidget *m_lineEditSimulateCount = nullptr;
    QWidget *m_lineEditDPS           = nullptr;
    QWidget *m_tabWidgetSecrets      = nullptr;
    QWidget *m_lineEditDelayMin      = nullptr;
    QWidget *m_lineEditDelayMax      = nullptr;
    QWidget *m_statsWidget           = nullptr;

    std::vector<TalentWidget *>                                  m_talentWidgets;
    std::unordered_map<std::string, std::list<SecretCheckBox *>> m_secretWidgets;
    std::vector<CheckBox *>                                      m_setEffectWidgets;

    JX3DPS::Attr m_attr;

    ProgressBar *m_progressBar = nullptr;

    nlohmann::json m_json;

    std::vector<std::pair<std::string, std::string>> m_skills;
    std::vector<std::pair<std::string, std::string>> m_events;
};

#endif // WIDGET_H
