/**
 * Project: JX3DPS
 * File: JX3DPSWidget.h
 * Description:
 * Created Date: 2023-08-06 06:45:29
 * Author: 难为水
 * -----
 * Last Modified: 2023-08-10 05:08:44
 * Modified By: 难为水
 * -----
 * HISTORY:
 * Date      	By   	Comments
 * ----------	-----	----------------------------------------------------------
 */

#pragma once

#include <nlohmann/json.hpp>

#include "Global/JX3.h"
#include "Widget/Widget.h"

namespace JX3DPS {

namespace Simulator {

class Widget : public ::Widget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

signals:
    void Signal_UpdateAttribute();
    void Signal_UpdateClassType(JX3DPS::ClassType classType);

private:
    void InitWidgetSetting(QWidget *parent = nullptr);
    void InitWidgetOut(QWidget *parent = nullptr);
    void InitWidgetAttribute(QWidget *parent = nullptr);
    void InitWidgetEquipEffects(QWidget *parent = nullptr);
    void InitWidgetGains(QWidget *parent = nullptr);
    void InitWidgetTalents(QWidget *parent = nullptr);
    void InitWidgetPermanents(QWidget *parent = nullptr);

    nlohmann::ordered_json m_config;
};

} // namespace Simulator

} // namespace JX3DPS
