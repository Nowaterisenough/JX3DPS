/**
 * Project: JX3DPS
 * File: FenShanJingBuff.h
 * Description: 
 * Created Date: 2023-08-25 20:44:39
 * Author: 难为水
 * -----
 * Last Modified: 2023-10-03 08:18:08
 * Modified By: 难为水
 * -----
 * HISTORY:
 * Date      	By   	Comments
 * ----------	-----	----------------------------------------------------------
 */



#ifndef __JX3DPS_CLASS_FEN_SHAN_JING_BUFF_H__
#define __JX3DPS_CLASS_FEN_SHAN_JING_BUFF_H__

#include "Buff.h"
#include "FenShanJing.h"

namespace JX3DPS {

namespace FenShanJing {

namespace Buff {

class DunFei : public JX3DPS::Buff
{
    BUFF_DEFAULT_FUNCTION(DunFei)

public:
    void TriggerAdd();
};

class XuRuo : public JX3DPS::Buff
{
    BUFF_DEFAULT_FUNCTION(XuRuo)

public:
    void TriggerAdd();
};

} // namespace Buff

} // namespace FenShanJing

} // namespace JX3DPS

#endif // __JX3DPS_CLASS_FEN_SHAN_JING_BUFF_H__