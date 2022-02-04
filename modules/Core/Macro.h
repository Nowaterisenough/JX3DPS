/**
 * @Description : 宏解析
 * @Author      : NoWats
 * @Date        : 2022-02-02 16:30:03
 * @Update      : NoWats
 * @LastTime    : 2022-02-04 01:35:51
 * @FilePath    : \JX3DPS\modules\Core\Macro.h
 */

#ifndef MACRO_H
#define MACRO_H

#include <string>
#include <list>

#if defined _WIN32 || defined __CYGWIN__
#    if defined EXPORT_JX3DPS_CORE // CMake add_definitions
#        ifdef __GNUC__
#            define JX3DPS_CORE_API __attribute__(dllexport)
#        else
#            define JX3DPS_CORE_API __declspec(dllexport)
#        endif // __GNUC__
#    else
#        ifdef __GNUC__
#            define JX3DPS_CORE_API __attribute__(dllimport)
#        else
#            define JX3DPS_CORE_API __declspec(dllimport)
#        endif // __GNUC__
#    endif     // COLOR_GRADIENT_SLIDER_API
#    define JX3DPS_CORE_PRIVATE

#elif defined __GNUC__
#    if __GNUC__ >= 4
#        define JX3DPS_CORE_API     __attribute__((visibility("default")))
#        define JX3DPS_CORE_PRIVATE __attribute__((visibility("hidden")))
#    else
#        define JX3DPS_CORE_API
#        define JX3DPS_CORE_PRIVATE
#    endif // __GNUC__ >= 4

#elif defined __clang__
#    define JX3DPS_CORE_API     __attribute__((visibility("default")))
#    define JX3DPS_CORE_PRIVATE __attribute__((visibility("hidden")))

#else
#    error "Do not know how to export classes for this platform"
#endif // defined(_WIN32) || defined(__CYGWIN__)

namespace JX3DPS {

struct Param
{
    int int1st;
    int int2nd;
    int int3rd;
    double double4th;
    double double5th;

    Param()
    {}

    Param(int int1st, int int2nd, int int3rd, double double4th, double double5th) :
        int1st(int1st), int2nd(int2nd), int3rd(int3rd), double4th(double4th), double5th(double5th)
    {
    }
};

using ConditionFuncPtr = bool(Macro::*)(const Param &param);

struct ConditionFunc
{
    ConditionFuncPtr macroFuncPtr;
    Param param;

    ConditionFunc()
    {
    }

    ConditionFunc(ConditionFuncPtr conditionFuncPtr) :
        conditionFuncPtr(conditionFuncPtr)
    {
    }

    ConditionFunc(ConditionFuncPtr conditionFuncPtr, const Param &param) :
        conditionFuncPtr(conditionFuncPtr), param(param)
    {
    }
};

using Macros = std::list<std::pair<std::list<std::list<ConditionFunc>>, Id_t>>;

using ForceMacros = std::list<std::pair<Frame_t, std::pair<std::list<std::list<ConditionFunc>>, Id_t>>>;

class JX3DPS_CORE_API Macro
{
public:
    Macro();
    ~Macro();

    bool Parse(const std::list<std::string> &lines);

    bool Parse(const std::string &line);

private:
    bool ParseCondition(const std::string &condition);

    bool IsReady(const Param &param);
    bool IsNotCast(const Param &param);
    bool IsNotReCast(const Param &param);

    bool BuffStackNumLt(const Param &param);
    bool BuffStackNumLe(const Param &param);
    bool BuffStackNumEq(const Param &param);
    bool BuffStackNumNe(const Param &param);
    bool BuffStackNumGe(const Param &param);
    bool BuffStackNumGt(const Param &param);

    bool TBuffStackNumLt(const Param &param);
    bool TBuffStackNumLe(const Param &param);
    bool TBuffStackNumEq(const Param &param);
    bool TBuffStackNumNe(const Param &param);
    bool TBuffStackNumGe(const Param &param);
    bool TBuffStackNumGt(const Param &param);

    bool BuffEffectNumLt(const Param &param);
    bool BuffEffectNumLe(const Param &param);
    bool BuffEffectNumEq(const Param &param);
    bool BuffEffectNumNe(const Param &param);
    bool BuffEffectNumGe(const Param &param);
    bool BuffEffectNumGt(const Param &param);

    bool TBuffEffectNumLt(const Param &param);
    bool TBuffEffectNumLe(const Param &param);
    bool TBuffEffectNumEq(const Param &param);
    bool TBuffEffectNumNe(const Param &param);
    bool TBuffEffectNumGe(const Param &param);
    bool TBuffEffectNumGt(const Param &param);

    bool BuffTimeLt(const Param &param);
    bool BuffTimeLe(const Param &param);
    bool BuffTimeEq(const Param &param);
    bool BuffTimeNe(const Param &param);
    bool BuffTimeGe(const Param &param);
    bool BuffTimeGt(const Param &param);

    bool TBuffTimeLt(const Param &param);
    bool TBuffTimeLe(const Param &param);
    bool TBuffTimeEq(const Param &param);
    bool TBuffTimeNe(const Param &param);
    bool TBuffTimeGe(const Param &param);
    bool TBuffTimeGt(const Param &param);

    bool SkillCooldownLt(const Param &param);
    bool SkillCooldownLe(const Param &param);
    bool SkillCooldownEq(const Param &param);
    bool SkillCooldownNe(const Param &param);
    bool SkillCooldownGe(const Param &param);
    bool SkillCooldownGt(const Param &param);

    bool SkillEnergyLt(const Param &param);
    bool SkillEnergyLe(const Param &param);
    bool SkillEnergyEq(const Param &param);
    bool SkillEnergyNe(const Param &param);
    bool SkillEnergyGe(const Param &param);
    bool SkillEnergyGt(const Param &param);

    bool TLifeLt(const Param &param);
    bool TLifeLe(const Param &param);
    bool TLifeEq(const Param &param);
    bool TLifeNe(const Param &param);
    bool TLifeGe(const Param &param);
    bool TLifeGt(const Param &param);

    bool ManaLt(const Param &param);
    bool ManaLe(const Param &param);
    bool ManaEq(const Param &param);
    bool ManaNe(const Param &param);
    bool ManaGe(const Param &param);
    bool ManaGt(const Param &param);

    bool NearbyEnemyLt(const Param &param);
    bool NearbyEnemyLe(const Param &param);
    bool NearbyEnemyEq(const Param &param);
    bool NearbyEnemyNe(const Param &param);
    bool NearbyEnemyGe(const Param &param);
    bool NearbyEnemyGt(const Param &param);

    bool QidianLt(const Param &param);
    bool QidianLe(const Param &param);
    bool QidianEq(const Param &param);
    bool QidianNe(const Param &param);
    bool QidianGe(const Param &param);
    bool QidianGt(const Param &param);

    bool RageLt(const Param &param);
    bool RageLe(const Param &param);
    bool RageEq(const Param &param);
    bool RageNe(const Param &param);
    bool RageGe(const Param &param);
    bool RageGt(const Param &param);

    bool EnergyLt(const Param &param);
    bool EnergyLe(const Param &param);
    bool EnergyEq(const Param &param);
    bool EnergyNe(const Param &param);
    bool EnergyGe(const Param &param);
    bool EnergyGt(const Param &param);

    bool SunLt(const Param &param);
    bool SunLe(const Param &param);
    bool SunEq(const Param &param);
    bool SunNe(const Param &param);
    bool SunGe(const Param &param);
    bool SunGt(const Param &param);

    bool MoonLt(const Param &param);
    bool MoonLe(const Param &param);
    bool MoonEq(const Param &param);
    bool MoonNe(const Param &param);
    bool MoonGe(const Param &param);
    bool MoonGt(const Param &param);

    bool SunPower(const Param &param);
    bool MoonPower(const Param &param);

    bool BuffExist(const Param &param);
    bool NoBuffExist(const Param &param);
    bool TBuffExist(const Param &param);
    bool TNoBuffExist(const Param &param);

    bool LastSkill(const Param &param);

    Macros m_publicMacros;
    Macros m_otherMacros;
    ForceMacros m_forceMacros;
};

} // namespace JX3DPS

#endif // MACRO_H
