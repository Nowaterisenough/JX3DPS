/**
 * @Description : 宏解析
 * @Author      : NoWats
 * @Date        : 2022-02-02 16:30:03
 * @Update      : NoWats
 * @LastTime    : 2022-02-06 13:12:10
 * @FilePath    : \JX3DPS\modules\Core\Macro.h
 */

#ifndef MACRO_H
#define MACRO_H

#include <string>
#include <list>

#include "Common/ConstVal.h"

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

class Player;

class JX3DPS_CORE_API Macro
{
public:
    Macro();
    ~Macro();

    bool Parse(const std::list<std::string> &lines);

    bool Parse(const std::string &line);

private:
    bool ParseCondition(const std::string &str, Condition &condition);

    bool IsReady(const Params &params);
    bool IsNotCast(const Params &params);
    bool IsNotReCast(const Params &params);

    bool BuffStackCountLt(const Params &params);
    bool BuffStackCountLe(const Params &params);
    bool BuffStackCountEq(const Params &params);
    bool BuffStackCountNe(const Params &params);
    bool BuffStackCountGe(const Params &params);
    bool BuffStackCountGt(const Params &params);

    bool TBuffStackCountLt(const Params &params);
    bool TBuffStackCountLe(const Params &params);
    bool TBuffStackCountEq(const Params &params);
    bool TBuffStackCountNe(const Params &params);
    bool TBuffStackCountGe(const Params &params);
    bool TBuffStackCountGt(const Params &params);

    bool BuffEffectCountLt(const Params &params);
    bool BuffEffectCountLe(const Params &params);
    bool BuffEffectCountEq(const Params &params);
    bool BuffEffectCountNe(const Params &params);
    bool BuffEffectCountGe(const Params &params);
    bool BuffEffectCountGt(const Params &params);

    bool TBuffEffectCountLt(const Params &params);
    bool TBuffEffectCountLe(const Params &params);
    bool TBuffEffectCountEq(const Params &params);
    bool TBuffEffectCountNe(const Params &params);
    bool TBuffEffectCountGe(const Params &params);
    bool TBuffEffectCountGt(const Params &params);

    bool BuffTimeLt(const Params &params);
    bool BuffTimeLe(const Params &params);
    bool BuffTimeEq(const Params &params);
    bool BuffTimeNe(const Params &params);
    bool BuffTimeGe(const Params &params);
    bool BuffTimeGt(const Params &params);

    bool TBuffTimeLt(const Params &params);
    bool TBuffTimeLe(const Params &params);
    bool TBuffTimeEq(const Params &params);
    bool TBuffTimeNe(const Params &params);
    bool TBuffTimeGe(const Params &params);
    bool TBuffTimeGt(const Params &params);

    bool SkillCooldownLt(const Params &params);
    bool SkillCooldownLe(const Params &params);
    bool SkillCooldownEq(const Params &params);
    bool SkillCooldownNe(const Params &params);
    bool SkillCooldownGe(const Params &params);
    bool SkillCooldownGt(const Params &params);

    bool SkillEnergyLt(const Params &params);
    bool SkillEnergyLe(const Params &params);
    bool SkillEnergyEq(const Params &params);
    bool SkillEnergyNe(const Params &params);
    bool SkillEnergyGe(const Params &params);
    bool SkillEnergyGt(const Params &params);

    bool TLifeLt(const Params &params);
    bool TLifeLe(const Params &params);
    bool TLifeEq(const Params &params);
    bool TLifeNe(const Params &params);
    bool TLifeGe(const Params &params);
    bool TLifeGt(const Params &params);

    bool ManaLt(const Params &params);
    bool ManaLe(const Params &params);
    bool ManaEq(const Params &params);
    bool ManaNe(const Params &params);
    bool ManaGe(const Params &params);
    bool ManaGt(const Params &params);

    bool NearbyEnemyLt(const Params &params);
    bool NearbyEnemyLe(const Params &params);
    bool NearbyEnemyEq(const Params &params);
    bool NearbyEnemyNe(const Params &params);
    bool NearbyEnemyGe(const Params &params);
    bool NearbyEnemyGt(const Params &params);

    bool QidianLt(const Params &params);
    bool QidianLe(const Params &params);
    bool QidianEq(const Params &params);
    bool QidianNe(const Params &params);
    bool QidianGe(const Params &params);
    bool QidianGt(const Params &params);

    bool RageLt(const Params &params);
    bool RageLe(const Params &params);
    bool RageEq(const Params &params);
    bool RageNe(const Params &params);
    bool RageGe(const Params &params);
    bool RageGt(const Params &params);

    bool EnergyLt(const Params &params);
    bool EnergyLe(const Params &params);
    bool EnergyEq(const Params &params);
    bool EnergyNe(const Params &params);
    bool EnergyGe(const Params &params);
    bool EnergyGt(const Params &params);

    bool SunLt(const Params &params);
    bool SunLe(const Params &params);
    bool SunEq(const Params &params);
    bool SunNe(const Params &params);
    bool SunGe(const Params &params);
    bool SunGt(const Params &params);

    bool MoonLt(const Params &params);
    bool MoonLe(const Params &params);
    bool MoonEq(const Params &params);
    bool MoonNe(const Params &params);
    bool MoonGe(const Params &params);
    bool MoonGt(const Params &params);

    bool SunPower(const Params &params);
    bool MoonPower(const Params &params);

    bool BuffExist(const Params &params);
    bool NoBuffExist(const Params &params);
    bool TBuffExist(const Params &params);
    bool TNoBuffExist(const Params &params);

    bool LastSkill(const Params &params);

    Macros   m_publicMacros;
    Macros   m_otherMacros;
    Player  *m_player;
    Targets *m_targets;
};

} // namespace JX3DPS

#endif // MACRO_H
