/**
 * Project: JX3DPS
 * File: ProgressBar.h
 * Description:
 * Created Date: 2023-06-30 23:40:42
 * Author: 难为水
 * -----
 * Last Modified: 2023-09-05 18:30:57
 * Modified By: 难为水
 * -----
 * HISTORY:
 * Date      	By   	Comments
 * ----------	-----	----------------------------------------------------------
 */

#ifndef __JX3DPS_IMPORT_WIDGET_H__
#define __JX3DPS_IMPORT_WIDGET_H__

#include <QDialog>

#include <nlohmann/json.hpp>

#include <Attribute/Attribute.hpp>

#include "Widget/Widget.h"

namespace JX3DPS {

namespace Simulator {

class ImportWidget : public Widget
{
    Q_OBJECT

public:
    ImportWidget(QWidget *parent = nullptr);

signals:
    void Signal_Import(nlohmann::ordered_json &json);

protected:
    void paintEvent(QPaintEvent *event) override;
};

} // namespace Simulator

} // namespace JX3DPS

#endif // __JX3DPS_IMPORT_WIDGET_H__