/**
 * Project: JX3DPS
 * File: JX3DPSWidget.h
 * Description:
 * Created Date: 2023-08-06 06:45:29
 * Author: 难为水
 * -----
 * Last Modified: 2023-08-07 19:52:54
 * Modified By: 难为水
 * -----
 * HISTORY:
 * Date      	By   	Comments
 * ----------	-----	----------------------------------------------------------
 */

#include "Widget/Widget.h"

class JX3DPSWidget : public Widget
{
    Q_OBJECT

public:
    JX3DPSWidget(QWidget *parent = nullptr);
    ~JX3DPSWidget();

signals:
    void Signal_UpdateAttribute();

private:
    void InitWidgetSetting(QWidget *parent = nullptr);
    void InitWidgetOut(QWidget *parent = nullptr);
    void InitWidgetAttribute(QWidget *parent = nullptr);
    void InitWidgetEquipEffects(QWidget *parent = nullptr);
    void InitWidgetGains(QWidget *parent = nullptr);
    void InitWidgetTalents(QWidget *parent = nullptr);
    void InitWidgetPermanents(QWidget *parent = nullptr);
};