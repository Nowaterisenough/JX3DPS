/**
 * Project: JX3DPS
 * File: TestUI.h
 * Description: 
 * Created Date: 2023-08-09 03:12:14
 * Author: 难为水
 * -----
 * Last Modified: 2023-08-09 03:13:09
 * Modified By: 难为水
 * -----
 * HISTORY:
 * Date      	By   	Comments
 * ----------	-----	----------------------------------------------------------
 */

#include "Widget/Widget.h"

class TestUI : public Widget
{
    Q_OBJECT

public:
    TestUI(QWidget *parent = nullptr);
    ~TestUI();
};