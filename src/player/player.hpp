#ifndef JX3DPS_PLAYER_PLAYER_HPP
#define JX3DPS_PLAYER_PLAYER_HPP

#include "attribute.hpp"
#include "src/core/context.h"
#include "src/global/types.h"

namespace JX3DPS {

/**
 * @brief 玩家基类 - 使用CRTP模式
 *
 * @tparam DerivedPlayer 派生类型 (具体门派)
 */
template <typename DerivedPlayer>
class Player {
public:
    Player() = default;
    ~Player() = default;

    // ===== 属性系统 =====
    Attribute attribute;

    // ===== 资源管理 (子类可重写) =====

    // 生命
    pctf_t GetLifePercent() const { return m_lifePercent; }
    void SetLifePercent(pctf_t percent) { m_lifePercent = std::clamp(percent, 0.0, 1.0); }
    void AddLifePercent(pctf_t percent) { SetLifePercent(m_lifePercent + percent); }

    // 内力
    pctf_t GetManaPercent() const { return m_manaPercent; }
    void SetManaPercent(pctf_t percent) { m_manaPercent = std::clamp(percent, 0.0, 1.0); }
    void AddManaPercent(pctf_t percent) { SetManaPercent(m_manaPercent + percent); }

    // ===== BUFF管理 =====

    bool HasBuff(jx3id_t buffId) const { return m_buffs.find(buffId) != m_buffs.end(); }

    int GetBuffStack(jx3id_t buffId) const {
        auto it = m_buffs.find(buffId);
        return it != m_buffs.end() ? it->second : 0;
    }

    void AddBuff(jx3id_t buffId, int stack = 1) {
        m_buffs[buffId] += stack;
    }

    void RemoveBuff(jx3id_t buffId, int stack = 1) {
        auto it = m_buffs.find(buffId);
        if (it != m_buffs.end()) {
            it->second -= stack;
            if (it->second <= 0) {
                m_buffs.erase(it);
            }
        }
    }

    void ClearBuff(jx3id_t buffId) { m_buffs.erase(buffId); }

    // ===== 技能施放 =====

    template <typename Skill>
    void CastSkill() {
        if constexpr (requires { Skill::IsReady(); }) {
            if (Skill::IsReady()) {
                Skill::Cast();
            }
        }
    }

    void CastSkill(jx3id_t skillId, int level = 0) {
        // 通过ID查找并施放技能
        // TODO: 实现技能ID映射
    }

    // ===== 状态管理 =====

    bool IsCast() const { return m_cast; }
    void SetCast(bool cast) { m_cast = cast; }

    bool IsReCast() const { return m_reCast; }
    void SetReCast(bool reCast) { m_reCast = reCast; }

    bool IsStop() const { return m_stop; }
    void SetStop(bool stop) { m_stop = stop; }

    // ===== GCD管理 =====

    tick_t globalCooldownCurrent = 0;

    virtual tick_t GetNextGlobalCooldown() const {
        return globalCooldownCurrent > 0 ? globalCooldownCurrent : JX3DPS_INVALID_FRAMES;
    }

    virtual void UpdateGlobalCooldown(tick_t next) {
        globalCooldownCurrent -= next;
        if (globalCooldownCurrent < 0) {
            globalCooldownCurrent = 0;
        }
    }

    // ===== CRTP辅助 =====

    DerivedPlayer *AsDerived() { return static_cast<DerivedPlayer *>(this); }

    const DerivedPlayer *AsDerived() const { return static_cast<const DerivedPlayer *>(this); }

protected:
    // 基础资源
    pctf_t m_lifePercent = 1.0;
    pctf_t m_manaPercent = 1.0;

    // BUFF列表
    hash_t<jx3id_t, int> m_buffs;

    // 状态标记
    bool m_cast   = false;
    bool m_reCast = false;
    bool m_stop   = false;
};

} // namespace JX3DPS

#endif // JX3DPS_PLAYER_PLAYER_HPP
