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

class JX3DPS_CORE_API Macro
{
public:
    Macro();
    ~Macro();

    bool Parse(const std::list<std::string> &lines);

    bool Parse(const std::string &line);

private:
    bool ParseCondition(const std::string &condition);

    static bool IsReady(const Param &param, Player &player);
    static bool IsNotCast(const Param &param);
    static bool IsNotReCast(const Param &param);

    static bool BuffStackNumLt(const Param &param);
    static bool BuffStackNumLe(const Param &param);
    static bool BuffStackNumEq(const Param &param);
    static bool BuffStackNumNe(const Param &param);
    static bool BuffStackNumGe(const Param &param);
    static bool BuffStackNumGt(const Param &param);

    static bool TBuffStackNumLt(const Param &param);
    static bool TBuffStackNumLe(const Param &param);
    static bool TBuffStackNumEq(const Param &param);
    static bool TBuffStackNumNe(const Param &param);
    static bool TBuffStackNumGe(const Param &param);
    static bool TBuffStackNumGt(const Param &param);

    static bool BuffEffectNumLt(const Param &param);
    static bool BuffEffectNumLe(const Param &param);
    static bool BuffEffectNumEq(const Param &param);
    static bool BuffEffectNumNe(const Param &param);
    static bool BuffEffectNumGe(const Param &param);
    static bool BuffEffectNumGt(const Param &param);

    static bool TBuffEffectNumLt(const Param &param);
    static bool TBuffEffectNumLe(const Param &param);
    static bool TBuffEffectNumEq(const Param &param);
    static bool TBuffEffectNumNe(const Param &param);
    static bool TBuffEffectNumGe(const Param &param);
    static bool TBuffEffectNumGt(const Param &param);

    static bool BuffTimeLt(const Param &param);
    static bool BuffTimeLe(const Param &param);
    static bool BuffTimeEq(const Param &param);
    static bool BuffTimeNe(const Param &param);
    static bool BuffTimeGe(const Param &param);
    static bool BuffTimeGt(const Param &param);

    static bool TBuffTimeLt(const Param &param);
    static bool TBuffTimeLe(const Param &param);
    static bool TBuffTimeEq(const Param &param);
    static bool TBuffTimeNe(const Param &param);
    static bool TBuffTimeGe(const Param &param);
    static bool TBuffTimeGt(const Param &param);

    static bool SkillCooldownLt(const Param &param);
    static bool SkillCooldownLe(const Param &param);
    static bool SkillCooldownEq(const Param &param);
    static bool SkillCooldownNe(const Param &param);
    static bool SkillCooldownGe(const Param &param);
    static bool SkillCooldownGt(const Param &param);

    static bool SkillEnergyLt(const Param &param);
    static bool SkillEnergyLe(const Param &param);
    static bool SkillEnergyEq(const Param &param);
    static bool SkillEnergyNe(const Param &param);
    static bool SkillEnergyGe(const Param &param);
    static bool SkillEnergyGt(const Param &param);

    static bool TLifeLt(const Param &param);
    static bool TLifeLe(const Param &param);
    static bool TLifeEq(const Param &param);
    static bool TLifeNe(const Param &param);
    static bool TLifeGe(const Param &param);
    static bool TLifeGt(const Param &param);

    static bool ManaLt(const Param &param);
    static bool ManaLe(const Param &param);
    static bool ManaEq(const Param &param);
    static bool ManaNe(const Param &param);
    static bool ManaGe(const Param &param);
    static bool ManaGt(const Param &param);

    static bool NearbyEnemyLt(const Param &param);
    static bool NearbyEnemyLe(const Param &param);
    static bool NearbyEnemyEq(const Param &param);
    static bool NearbyEnemyNe(const Param &param);
    static bool NearbyEnemyGe(const Param &param);
    static bool NearbyEnemyGt(const Param &param);

    static bool QidianLt(const Param &param);
    static bool QidianLe(const Param &param);
    static bool QidianEq(const Param &param);
    static bool QidianNe(const Param &param);
    static bool QidianGe(const Param &param);
    static bool QidianGt(const Param &param);

    static bool RageLt(const Param &param);
    static bool RageLe(const Param &param);
    static bool RageEq(const Param &param);
    static bool RageNe(const Param &param);
    static bool RageGe(const Param &param);
    static bool RageGt(const Param &param);

    static bool EnergyLt(const Param &param);
    static bool EnergyLe(const Param &param);
    static bool EnergyEq(const Param &param);
    static bool EnergyNe(const Param &param);
    static bool EnergyGe(const Param &param);
    static bool EnergyGt(const Param &param);

    static bool SunLt(const Param &param);
    static bool SunLe(const Param &param);
    static bool SunEq(const Param &param);
    static bool SunNe(const Param &param);
    static bool SunGe(const Param &param);
    static bool SunGt(const Param &param);

    static bool MoonLt(const Param &param);
    static bool MoonLe(const Param &param);
    static bool MoonEq(const Param &param);
    static bool MoonNe(const Param &param);
    static bool MoonGe(const Param &param);
    static bool MoonGt(const Param &param);

    static bool SunPower(const Param &param);
    static bool MoonPower(const Param &param);

    static bool BuffExist(const Param &param);
    static bool NoBuffExist(const Param &param);
    static bool TBuffExist(const Param &param);
    static bool TNoBuffExist(const Param &param);

    static bool LastSkill(const Param &param);

    Macros      m_publicMacros;
    Macros      m_otherMacros;
    ForceMacros m_forceMacros;
};

} // namespace JX3DPS

#endif // MACRO_H
