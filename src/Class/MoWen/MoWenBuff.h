/**
 * Project: JX3DPS
 * File: MoWenBuff.h
 * Description:
 * Created Date: 2023-08-01 00:55:43
 * Author: 难为水
 * -----
 * Last Modified: 2023-08-02 01:11:52
 * Modified By: 难为水
 * -----
 * HISTORY:
 * Date      	By   	Comments
 * ----------	-----	----------------------------------------------------------
 */

#ifndef __JX3DPS_CLASS_MO_WEN_BUFF_H__
#define __JX3DPS_CLASS_MO_WEN_BUFF_H__

#include "Buff.h"
#include "MoWen.h"

namespace JX3DPS {

namespace MoWen {

namespace Buff {

class Shang : public JX3DPS::Buff
{
    BUFF_DEFAULT_FUNCTION(Shang)

public:
    void TriggerAdd(Id_t targetId);

    void SubEffect(Id_t targetId);
};

class Jue : public JX3DPS::Buff
{
    BUFF_DEFAULT_FUNCTION(Jue)

public:
    void TriggerAdd(Id_t targetId);

    void SubEffect(Id_t targetId);
};

class XianFeng : public JX3DPS::Buff
{
    BUFF_DEFAULT_FUNCTION(XianFeng)

public:
    void TriggerAdd();
};

class XianFengBiaoJi : public JX3DPS::Buff
{
    BUFF_DEFAULT_FUNCTION(XianFengBiaoJi)

public:
    void TriggerAdd();
};

} // namespace Buff

} // namespace MoWen

} // namespace JX3DPS

#endif // __JX3DPS_CLASS_MO_WEN_BUFF_H__