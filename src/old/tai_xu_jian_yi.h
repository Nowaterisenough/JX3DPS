#ifndef JX3DPS_TAI_XU_JIAN_YI_H
#define JX3DPS_TAI_XU_JIAN_YI_H

#include "Attribute.hpp"
#include "buff.hpp"
#include "common.h"
#include "jx3.h"
#include "sim.hpp"
#include "skill.hpp"

namespace JX3DPS {

struct 太虚剑意
{

    enum TalentId
    {
        无意,
        叠刃,
        无欲,
        裂云,
    };

    enum class RecipeId
    {
        碎星辰,
        云中剑·碎星辰,
    };

    enum class SkillId
    {
        无我无剑,
        八荒归元,
        三环套月,
        万剑归宗,
        人剑合一,
        三柴剑法,
        生太极,
        碎星辰,
        吞日月,
        紫气东来,
        破招,
    };

    class Player : public JX3DPS::Player<太虚剑意::Player>
    {
    public:
        Player();

        ~Player() = default;

        void AddQidian(int qidian) { _qidian = std::max(_qidian + qidian, 10); }

        void SetQidian(int qidian) { _qidian = qidian; }

        int GetQidian() const { return _qidian; }

    private:
        int _qidian = 10;
    };

    struct Skill
    {
        class 无我无剑 : public SkillImpl<无我无剑>
        {
        public:
            CONSTANT Tag TAG   = Tag(TARGET | DAMAGE | MELEE | INSTANT);
            CONSTANT int LEVEL = 10;
            CONSTANT int SUB   = 1;

            PROPERTY tick_t cooldown = 24; // 冷却时间
            PROPERTY tick_t *current_cooldown = &tls_sim.cache.skill_cooldown[0]; // 当前冷却时间
            // PROPERTY tick_t  prepare         = 0;                      // 预读条时间
            // PROPERTY tick_t *current_prepare = nullptr; // 当前预读条时间
            // PROPERTY tick_t  casting         = 0;       // 读条时间
            // PROPERTY tick_t *current_casting = nullptr; // 当前读条时间

            // PROPERTY int energy_count = 0; // 充能数
            // PROPERTY int energy_max   = 0; // 最大充能数

            PROPERTY int range = 4; // 施法距离
            PROPERTY int limit = 1; // 限制目标数目

            PROPERTY int    base_damage[LEVEL]        = { 0 };       // 基础伤害
            PROPERTY int    rand_damage[LEVEL]        = { 0 };       // 随机伤害
            PROPERTY int    physics_ap[LEVEL]         = { 0 };       // 物理系数
            PROPERTY pctn_t weapon_coefficient[LEVEL] = { PCT_100 }; // 武器系数

            PROPERTY pctn_t critical_strike_add       = 0; // 暴击率
            PROPERTY pctn_t critical_strike_power_add = 0; // 暴击伤害
            PROPERTY pctn_t damage_add                = 0; // 伤害加成
            PROPERTY pctn_t shield_ignore_add         = 0; // 无视防御

            无我无剑()
            {
                // Register(CAST_CONDITION,
                //          [this]() -> bool { return tls_sim.player.GetQidian() > 0; });

                Register(PRE_CAST, [this]() {
                    self.qidian = tls_sim.player.GetQidian();
                    tls_sim.player.SetQidian(0);
                });

                Register(POST_ROLL, [this]() {
                    constexpr int QIDIAN = 6;
                    if (self.qidian > QIDIAN) {
                        int level = self.qidian / 2;
                        tls_sim.player.CastSkill(破招, level);
                    }
                });

                Register(无意, PRE_CAST, [this]() {
                    constexpr int QIDIAN = 6;
                    if (self.qidian > QIDIAN) {
                        this->critical_strike_add       += PCT_10;
                        this->critical_strike_power_add += PCT_30;
                    }
                });

                Register(无意, POST_CAST, [this]() {
                    constexpr int QIDIAN = 6;
                    if (self.qidian > QIDIAN) {
                        this->critical_strike_add       -= PCT_10;
                        this->critical_strike_power_add -= PCT_30;
                    }
                });

                Register(叠刃, PRE_CAST, [this]() {
                    tls_sim.player.AddBuff(叠刃);
                    if (self.roll == DOUBLE) {
                        tls_sim.player.AddBuff(叠刃);
                    }
                });

                Register(无欲, POST_CAST, [this]() {
                    if (tls_sim.player.HasBuff(叠刃)) {
                        *八荒归元::current_cooldown -= self.qidian * JX3_FRAMES_PER_SECOND / 2;
                        *八荒归元::current_cooldown = std::max(*八荒归元::current_cooldown, 0);
                    }
                });
            }
        };

        class 八荒归元 : public SkillImpl<八荒归元>
        {
        public:
            CONSTANT int LEVEL = 10;
            CONSTANT int SUB   = 2;
            CONSTANT int LIMIT = 1;

            PROPERTY tick_t cooldown = JX3_FRAMES_PER_SECOND * 8; // 冷却时间
            PROPERTY tick_t *current_cooldown = &tls_sim.cache.skill_cooldown[1]; // 当前冷却时间
            // PROPERTY tick_t  prepare         = 0;                      // 预读条时间
            // PROPERTY tick_t *current_prepare = nullptr; // 当前预读条时间
            // PROPERTY tick_t  casting         = 0;       // 读条时间
            // PROPERTY tick_t *current_casting = nullptr; // 当前读条时间

            // PROPERTY int energy_count = 0; // 充能数
            // PROPERTY int energy_max   = 0; // 最大充能数

            PROPERTY int range = 6; // 施法距离

            PROPERTY int    base_damage[SUB][LEVEL]        = { 0 };       // 基础伤害
            PROPERTY int    rand_damage[SUB][LEVEL]        = { 0 };       // 随机伤害
            PROPERTY int    physics_ap[SUB][LEVEL]         = { 0 };       // 物理系数
            PROPERTY pctn_t weapon_coefficient[SUB][LEVEL] = { PCT_100 }; // 武器系数

            PROPERTY pctn_t critical_strike_add       = 0; // 暴击率
            PROPERTY pctn_t critical_strike_power_add = 0; // 暴击伤害
            PROPERTY pctn_t damage_add                = 0; // 伤害加成
            PROPERTY pctn_t shield_ignore_add         = 0; // 无视防御
        };

        class 三环套月 : public SkillImpl<三环套月>
        {
        public:
            CONSTANT int LEVEL = 1;
            CONSTANT int SUB   = 3;
            CONSTANT int LIMIT = 1;

            PROPERTY tick_t cooldown = JX3_FRAMES_PER_SECOND * 2; // 冷却时间
            PROPERTY tick_t *current_cooldown = &tls_sim.cache.skill_cooldown[1]; // 当前冷却时间
            // PROPERTY tick_t  prepare         = 0;                      // 预读条时间
            // PROPERTY tick_t *current_prepare = nullptr; // 当前预读条时间
            // PROPERTY tick_t  casting         = 0;       // 读条时间
            // PROPERTY tick_t *current_casting = nullptr; // 当前读条时间

            // PROPERTY int energy_count = 0; // 充能数
            // PROPERTY int energy_max   = 0; // 最大充能数

            PROPERTY int range = 4; // 施法距离

            PROPERTY int    base_damage[SUB]        = { 0 };       // 基础伤害
            PROPERTY int    rand_damage[SUB]        = { 0 };       // 随机伤害
            PROPERTY int    physics_ap[SUB]         = { 0 };       // 物理系数
            PROPERTY pctn_t weapon_coefficient[SUB] = { PCT_100 }; // 武器系数

            PROPERTY pctn_t critical_strike_add       = 0; // 暴击率
            PROPERTY pctn_t critical_strike_power_add = 0; // 暴击伤害
            PROPERTY pctn_t damage_add                = 0; // 伤害加成
            PROPERTY pctn_t shield_ignore_add         = 0; // 无视防御
        };

        class 万剑归宗 : public SkillImpl<万剑归宗>
        {
        public:
            CONSTANT int LEVEL = 1;
            CONSTANT int SUB   = 1;
            CONSTANT int LIMIT = 5;

            PROPERTY tick_t cooldown = JX3_FRAMES_PER_SECOND * 2; // 冷却时间
            PROPERTY tick_t *current_cooldown = &tls_sim.cache.skill_cooldown[1]; // 当前冷却时间
            // PROPERTY tick_t  prepare         = 0;                      // 预读条时间
            // PROPERTY tick_t *current_prepare = nullptr; // 当前预读条时间
            // PROPERTY tick_t  casting         = 0;       // 读条时间
            // PROPERTY tick_t *current_casting = nullptr; // 当前读条时间

            // PROPERTY int energy_count = 0; // 充能数
            // PROPERTY int energy_max   = 0; // 最大充能数

            PROPERTY int range = 4; // 施法距离

            PROPERTY int    base_damage        = { 0 };       // 基础伤害
            PROPERTY int    rand_damage        = { 0 };       // 随机伤害
            PROPERTY int    physics_ap         = { 0 };       // 物理系数
            PROPERTY pctn_t weapon_coefficient = { PCT_100 }; // 武器系数

            PROPERTY pctn_t critical_strike_add       = 0; // 暴击率
            PROPERTY pctn_t critical_strike_power_add = 0; // 暴击伤害
            PROPERTY pctn_t damage_add                = 0; // 伤害加成
            PROPERTY pctn_t shield_ignore_add         = 0; // 无视防御
        };

        class 人剑合一 : public SkillImpl<人剑合一>
        {
        public:
            CONSTANT int LEVEL = 1;
            CONSTANT int SUB   = 1;
            CONSTANT int LIMIT = 5;

            PROPERTY tick_t cooldown = JX3_FRAMES_PER_SECOND * 2; // 冷却时间
            PROPERTY tick_t *current_cooldown = &tls_sim.cache.skill_cooldown[1]; // 当前冷却时间
            // PROPERTY tick_t  prepare         = 0;                      // 预读条时间
            // PROPERTY tick_t *current_prepare = nullptr; // 当前预读条时间
            // PROPERTY tick_t  casting         = 0;       // 读条时间
            // PROPERTY tick_t *current_casting = nullptr; // 当前读条时间

            // PROPERTY int energy_count = 0; // 充能数
            // PROPERTY int energy_max   = 0; // 最大充能数

            PROPERTY int range = 4; // 施法距离

            PROPERTY int    base_damage        = { 0 };       // 基础伤害
            PROPERTY int    rand_damage        = { 0 };       // 随机伤害
            PROPERTY int    physics_ap         = { 0 };       // 物理系数
            PROPERTY pctn_t weapon_coefficient = { PCT_100 }; // 武器系数

            PROPERTY pctn_t critical_strike_add       = 0; // 暴击率
            PROPERTY pctn_t critical_strike_power_add = 0; // 暴击伤害
            PROPERTY pctn_t damage_add                = 0; // 伤害加成
            PROPERTY pctn_t shield_ignore_add         = 0; // 无视防御
        };

        class 三柴剑法 : public SkillImpl<三柴剑法>
        {
        public:
            CONSTANT int LEVEL = 1;
            CONSTANT int SUB   = 1;
            CONSTANT int LIMIT = 5;

            PROPERTY tick_t cooldown = JX3_FRAMES_PER_SECOND * 2; // 冷却时间
            PROPERTY tick_t *current_cooldown = &tls_sim.cache.skill_cooldown[1]; // 当前冷却时间
            // PROPERTY tick_t  prepare         = 0;                      // 预读条时间
            // PROPERTY tick_t *current_prepare = nullptr; // 当前预读条时间
            // PROPERTY tick_t  casting         = 0;       // 读条时间
            // PROPERTY tick_t *current_casting = nullptr; // 当前读条时间

            // PROPERTY int energy_count = 0; // 充能数
            // PROPERTY int energy_max   = 0; // 最大充能数

            PROPERTY int range = 4; // 施法距离

            PROPERTY int    base_damage        = { 0 };       // 基础伤害
            PROPERTY int    rand_damage        = { 0 };       // 随机伤害
            PROPERTY int    physics_ap         = { 0 };       // 物理系数
            PROPERTY pctn_t weapon_coefficient = { PCT_100 }; // 武器系数

            PROPERTY pctn_t critical_strike_add       = 0; // 暴击率
            PROPERTY pctn_t critical_strike_power_add = 0; // 暴击伤害
            PROPERTY pctn_t damage_add                = 0; // 伤害加成
            PROPERTY pctn_t shield_ignore_add         = 0; // 无视防御
        };

        class 生太极 : public SkillImpl<生太极>
        {
        public:
            CONSTANT int LEVEL = 1;
            CONSTANT int SUB   = 1;
            CONSTANT int LIMIT = 5;

            PROPERTY tick_t cooldown = JX3_FRAMES_PER_SECOND * 2; // 冷却时间
            PROPERTY tick_t *current_cooldown = &tls_sim.cache.skill_cooldown[1]; // 当前冷却时间
            PROPERTY tick_t  prepare         = 0;       // 预读条时间
            PROPERTY tick_t *current_prepare = nullptr; // 当前预读条时间
            // PROPERTY tick_t  casting         = 0;       // 读条时间
            // PROPERTY tick_t *current_casting = nullptr; // 当前读条时间

            // PROPERTY int energy_count = 0; // 充能数
            // PROPERTY int energy_max   = 0; // 最大充能数

            PROPERTY int range = 4; // 施法距离

            // PROPERTY int    base_damage        = { 0 };       // 基础伤害
            // PROPERTY int    rand_damage        = { 0 };       // 随机伤害
            // PROPERTY int    physics_ap         = { 0 };       // 物理系数
            // PROPERTY pctn_t weapon_coefficient = { PCT_100 }; // 武器系数

            // PROPERTY pctn_t critical_strike_add       = 0; // 暴击率
            // PROPERTY pctn_t critical_strike_power_add = 0; // 暴击伤害
            // PROPERTY pctn_t damage_add                = 0; // 伤害加成
            // PROPERTY pctn_t shield_ignore_add         = 0; // 无视防御
        };

        class 碎星辰 : public SkillImpl<碎星辰>
        {
        public:
            CONSTANT int LEVEL = 1;
            CONSTANT int SUB   = 1;
            CONSTANT int LIMIT = 5;

            PROPERTY tick_t cooldown = JX3_FRAMES_PER_SECOND * 2; // 冷却时间
            PROPERTY tick_t *current_cooldown = &tls_sim.cache.skill_cooldown[1]; // 当前冷却时间
            PROPERTY tick_t  prepare         = 0;       // 预读条时间
            PROPERTY tick_t *current_prepare = nullptr; // 当前预读条时间
            // PROPERTY tick_t  casting         = 0;       // 读条时间
            // PROPERTY tick_t *current_casting = nullptr; // 当前读条时间

            // PROPERTY int energy_count = 0; // 充能数
            // PROPERTY int energy_max   = 0; // 最大充能数

            PROPERTY int range = 4; // 施法距离

            // PROPERTY int    base_damage        = { 0 };       // 基础伤害
            // PROPERTY int    rand_damage        = { 0 };       // 随机伤害
            // PROPERTY int    physics_ap         = { 0 };       // 物理系数
            // PROPERTY pctn_t weapon_coefficient = { PCT_100 }; // 武器系数

            // PROPERTY pctn_t critical_strike_add       = 0; // 暴击率
            // PROPERTY pctn_t critical_strike_power_add = 0; // 暴击伤害
            // PROPERTY pctn_t damage_add                = 0; // 伤害加成
            // PROPERTY pctn_t shield_ignore_add         = 0; // 无视防御
        };

        class 吞日月 : public SkillImpl<吞日月>
        {
        public:
            CONSTANT int LEVEL = 1;
            CONSTANT int SUB   = 1;
            CONSTANT int LIMIT = 5;

            PROPERTY tick_t cooldown = JX3_FRAMES_PER_SECOND * 2; // 冷却时间
            PROPERTY tick_t *current_cooldown = &tls_sim.cache.skill_cooldown[1]; // 当前冷却时间
            PROPERTY tick_t  prepare         = 0;       // 预读条时间
            PROPERTY tick_t *current_prepare = nullptr; // 当前预读条时间
            // PROPERTY tick_t  casting         = 0;       // 读条时间
            // PROPERTY tick_t *current_casting = nullptr; // 当前读条时间

            // PROPERTY int energy_count = 0; // 充能数
            // PROPERTY int energy_max   = 0; // 最大充能数

            PROPERTY int range = 4; // 施法距离

            // PROPERTY int    base_damage        = { 0 };       // 基础伤害
            // PROPERTY int    rand_damage        = { 0 };       // 随机伤害
            // PROPERTY int    physics_ap         = { 0 };       // 物理系数
            // PROPERTY pctn_t weapon_coefficient = { PCT_100 }; // 武器系数

            // PROPERTY pctn_t critical_strike_add       = 0; // 暴击率
            // PROPERTY pctn_t critical_strike_power_add = 0; // 暴击伤害
            // PROPERTY pctn_t damage_add                = 0; // 伤害加成
            // PROPERTY pctn_t shield_ignore_add         = 0; // 无视防御
        };

        class 紫气东来 : public SkillImpl<紫气东来>
        {
        public:
            // PROPERTY tick_t cooldown = JX3_FRAMES_PER_SECOND * 2; // 冷却时间
            // PROPERTY tick_t *current_cooldown = &tls_sim.cache.skill_cooldown[1]; // 当前冷却时间

            PROPERTY int energy_count = 2; // 充能数
            PROPERTY int energy_max   = 2; // 最大充能数

            紫气东来()
            {
                Register(POST_CAST, [this]() {
                    tls_sim.player.SetQidian(10);
                    tls_sim.player.AddBuff(紫气东来);
                });
            }
        };
    };

    struct Buff
    {
        class 紫气东来 : public BuffImpl<紫气东来>
        {
        public:
            CONSTANT Tag TAG = Tag::SELF;

            PROPERTY tick_t  duration = JX3_FRAMES_PER_SECOND * 10; // 持续时间
            PROPERTY tick_t *current_duration = nullptr;            // 当前持续时间
            PROPERTY tick_t  interval         = JX3_FRAMES_PER_SECOND; // 间隔时间
            PROPERTY tick_t *current_interval = nullptr; // 当前间隔时间

            紫气东来()
            {
                Register(ADD, [this]() {
                    tls_sim.player.AddAttribute(Attribute::Type::CRITICAL_STRIKE, PCT_10);
                    tls_sim.player.AddAttribute(Attribute::Type::CRITICAL_STRIKE_POWER, PCT_20);
                });

                Register(EXPIRE, [this]() {
                    tls_sim.player.AddAttribute(Attribute::Type::CRITICAL_STRIKE, -PCT_10);
                    tls_sim.player.AddAttribute(Attribute::Type::CRITICAL_STRIKE_POWER, -PCT_20);
                });

                Register(TICKING, [this]() { tls_sim.player.AddQidian(2); });
            }
        };

        class 气场·碎星辰 : public BuffImpl<气场·碎星辰>
        {
        public:
            CONSTANT Tag TAG = Tag(Tag::THIRD | Tag::AOE);

            PROPERTY tick_t  duration = JX3_FRAMES_PER_SECOND * 10; // 持续时间
            PROPERTY tick_t *current_duration = nullptr;            // 当前持续时间
            PROPERTY tick_t  interval         = JX3_FRAMES_PER_SECOND; // 间隔时间
            PROPERTY tick_t *current_interval = nullptr; // 当前间隔时间

            气场·碎星辰()
            {
                Register(ADD, [this]() { tls_sim.player.AddBuff(碎星辰); });

                Register(TICKING, [this]() { tls_sim.player.AddBuff(碎星辰); });
            }
        };

        class 碎星辰 : public BuffImpl<碎星辰>
        {
        public:
            CONSTANT Tag TAG = Tag::SELF;

            PROPERTY tick_t  duration = JX3_FRAMES_PER_SECOND * 10; // 持续时间
            PROPERTY tick_t *current_duration = nullptr;            // 当前持续时间

            碎星辰()
            {
                Register(ADD, [this]() {
                    tls_sim.player.AddAttribute(Attribute::CRITICAL_STRIKE, PCT_10);
                });

                Register(EXPIRE, [this]() {
                    tls_sim.player.AddAttribute(Attribute::CRITICAL_STRIKE, -PCT_10);
                });
            }
        };

        class 叠刃 : public BuffImpl<叠刃>
        {
        public:
            CONSTANT Tag TAG = Tag::DOT;

            PROPERTY tick_t  duration = JX3_FRAMES_PER_SECOND * 10; // 持续时间
            PROPERTY tick_t *current_duration = nullptr;            // 当前持续时间
            PROPERTY tick_t  interval         = JX3_FRAMES_PER_SECOND; // 间隔时间
            PROPERTY tick_t *current_interval = nullptr; // 当前间隔时间

            PROPERTY int stack_num = 5; // 叠刃层数

            PROPERTY Snapshots snapshots;

            叠刃()
            {
                Register(裂云, [this]() { stack_num = 7; });

                Register(ADD, [this]() {
                    for (auto &[id, snapshot] : snapshots) {
                        if (snapshot.stack_num >= 4) {
                            tls_sim.targets[id].AddBuff(裂云隐藏);
                        }
                    }
                });
            }
        };

        class 云中剑·碎星辰 : public BuffImpl<云中剑·碎星辰>
        {
        public:
            CONSTANT Tag TAG = Tag(Tag::SELF | Tag::AOE | Tag::DAMAGE);

            PROPERTY tick_t  duration = JX3_FRAMES_PER_SECOND * 10; // 持续时间
            PROPERTY tick_t *current_duration = nullptr;            // 当前持续时间
            PROPERTY tick_t  interval         = JX3_FRAMES_PER_SECOND; // 间隔时间
            PROPERTY tick_t *current_interval = nullptr; // 当前间隔时间

            PROPERTY int limit = 5; // 限制目标数目
            PROPERTY int range = 4; // 施法距离
        };
    };
};

} // namespace JX3DPS

#endif // __JX3DPS_TAI_XU_JIAN_YI_H__