/**
 * Project: JX3DPS
 * File: FenShanJingBuff.h
 * Description: 
 * Created Date: 2023-08-25 20:44:39
 * Author: 难为水
 * -----
 * Last Modified: 2023-08-26 09:44:35
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

class DieRen : public JX3DPS::Buff
{
    BUFF_DEFAULT_FUNCTION(DieRen)

public:
    void TriggerAdd(Id_t targetId, int stackNum);
    void TriggerQieYu(Id_t targetId);
    void TriggerHuanYue(Id_t targetId);

    void SubEffect(Id_t targetId, int stackNum);
    void SubEffectQieYun(Id_t targetId, int stackNum, int effectCount);
    void SubEffectAdd(Id_t targetId);
    void SubEffectClear(Id_t targetId);
};



} // namespace Buff

} // namespace FenShanJing

} // namespace JX3DPS

#endif // __JX3DPS_CLASS_FEN_SHAN_JING_BUFF_H__