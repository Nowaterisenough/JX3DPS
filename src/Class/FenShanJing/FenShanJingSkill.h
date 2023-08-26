/**
 * Project: JX3DPS
 * File: FenShanJingSkill.h
 * Description: 
 * Created Date: 2023-08-25 20:44:39
 * Author: 难为水
 * -----
 * Last Modified: 2023-08-26 09:43:43
 * Modified By: 难为水
 * -----
 * HISTORY:
 * Date      	By   	Comments
 * ----------	-----	----------------------------------------------------------
 */



#ifndef __JX3DPS_CLASS_FEN_SHAN_JING_SKILL_H__
#define __JX3DPS_CLASS_FEN_SHAN_JING_SKILL_H__

#include "Skill.h"
#include "FenShanJing.h"

namespace JX3DPS {

namespace FenShanJing {

namespace Skill {

class PoZhao : public JX3DPS::Skill
{
    SKILL_DEFAULT_FUNCTION(PoZhao)

public:
    void SubEffect();
};

class JieDao : public JX3DPS::Skill
{
    SKILL_DEFAULT_FUNCTION(JieDao)

public:
    void SubEffect();
};

class ZhanDao : public JX3DPS::Skill
{
    SKILL_DEFAULT_FUNCTION(ZhanDao)

public:
    void SubEffect();
};

class JueDao : public JX3DPS::Skill
{
    SKILL_DEFAULT_FUNCTION(JueDao)

public:
    void SubEffect();
};

class ShanDao : public JX3DPS::Skill
{
    SKILL_DEFAULT_FUNCTION(ShanDao)

public:
    void SubEffect();
};

class DunDao : public JX3DPS::Skill
{
    SKILL_DEFAULT_FUNCTION(DunDao)

public:
    void SubEffect();
};

class DunMeng : public JX3DPS::Skill
{
    SKILL_DEFAULT_FUNCTION(DunMeng)

public:
    void SubEffect();
};

class DunJi : public JX3DPS::Skill
{
    SKILL_DEFAULT_FUNCTION(DunJi)

public:
    void SubEffect();
};

class DunYa : public JX3DPS::Skill
{
    SKILL_DEFAULT_FUNCTION(DunYa)

public:
    void SubEffect();
};

class DunFei : public JX3DPS::Skill
{
    SKILL_DEFAULT_FUNCTION(DunFei)

public:
    void SubEffect();
};

class DunWu : public JX3DPS::Skill
{
    SKILL_DEFAULT_FUNCTION(DunWu)

public:
    void SubEffect();
};

class XueNu : public JX3DPS::Skill
{
    SKILL_DEFAULT_FUNCTION(XueNu)

public:
    void SubEffect();
};

class YeHuoLinGuang : public JX3DPS::Skill
{
    SKILL_DEFAULT_FUNCTION(YeHuoLinGuang)

public:
    void SubEffect();
};

class ZhenYunJieHui : public JX3DPS::Skill
{
    SKILL_DEFAULT_FUNCTION(ZhenYunJieHui)

public:
    void SubEffect();
};

} // namespace Skill

} // namespace FenShanJing

} // namespace JX3DPS

#endif // __JX3DPS_CLASS_FEN_SHAN_JING_SKILL_H__