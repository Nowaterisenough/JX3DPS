#include "common/builder.hpp"

namespace JX3DPS::runtime::season50 {

// 温寒朝零移动时，每抵消一点旧药性结算一次中和。例：原有 1 点温，
// 施加 2 点寒，抵消 1 点、触发一次中和，剩 1 点寒。
static void 中和(EffectContext &frame, const Step &step)
{
    const int old = frame.Resource(step.slot);
    frame.ChangeResource(step.slot, step.amount);
    const int  base   = old * step.amount < 0 ? std::min(std::abs(old), std::abs(step.amount)) : 0;
    const bool repeat = step.repeat_aura != INVALID_SLOT && frame.Stacks(step.repeat_aura);
    for (int i = 0; i < base; ++i) {
        if (step.neutral_buff != INVALID_SLOT) {
            frame.SetAura(step.neutral_buff, 1);
        }
        frame.Hit(step.extra);
        if (step.neutral_dot != INVALID_SLOT) {
            frame.SetAura(step.neutral_dot, 1);
        }
        if (repeat) {
            frame.Hit(step.extra);
        }
    }
}

static Step temperature(Builder &b, int delta, std::string_view event, std::string_view boost = {}, std::string_view dot = {})
{
    Step step{ .op = Op::School, .slot = 0, .amount = delta, .extra = b.effect(event), .effect = 中和 };
    if (!boost.empty()) {
        step.neutral_buff = b.buff(boost);
    }
    if (!dot.empty()) {
        step.neutral_dot = b.buff(dot);
    }
    return step;
}

Book 无方()
{
    Builder b("wf", "无方", R"(# 魔盒宏适配 https://www.jx3box.com/macro/111461
# 沾衣铺寒，香繁轮提前读川乌；普通轮等药雾结束再读
/cast 使用风腰坠
/cast 使用橙武
/cast [buff:千枝绽蕊&bufftime:紫伏<6&cd:沾衣未妨<6] 千枝伏藏
/cast [bufftime:紫伏<2&温寒<-2] 苍棘缚地
/cast [cd:苍棘缚地>19] 沾衣未妨
/cast [cd:沾衣未妨>13&mana>0.35&nobuff:千枝绽蕊] 千枝绽蕊
/cast [cd:苍棘缚地>19&bufftime:紫伏<2] 紫叶沉疴
/cast [温寒=-5&bufftime:紫伏>12&tbufftime:沾衣未妨<=2.25] 钩吻断肠
/cast [last_skill:钩吻断肠&bufftime:紫伏>14] 香繁饮露
/cast [buff:香繁饮露] 川乌射罔
/cast [温寒=5|温寒=2] 商陆缀寒
/cast [温寒<2&bufftime:紫伏>6&tnobuff:沾衣未妨] 川乌射罔
/cast [温寒>-5|bufftime:紫伏<12] 商陆缀寒
)");
    auto &school = b.definition();
    school.info = { school.code, "药宗", "根骨", "Poison", 707, 0, 61 };
    school.display_order = 14;
    school.saved.macro_digest = "372540b178db146ab4b2675aca6a4cacb862f56ac4c0271dfb4e11e5d7e4e466";
    school.saved.talents_v6 = { "相使" };
    school.precombat = true;
    school.weapon_effect = "鉴魄证身";
    school.raid.formation = "易筋阵（常驻）";
    b.preset(2301,
             { 5969, 1588, 113, 1831, 208, 2032, 5265, 0, 1092 },
             { "川乌射罔·4%会心", "川乌射罔·3%伤害", "川乌射罔·3%会心",
               "钩吻断肠·4%会心", "钩吻断肠·3%伤害", "钩吻断肠·3%会心", "钩吻断肠·2%伤害",
               "商陆缀寒·4%会心", "商陆缀寒·3%伤害", "商陆缀寒·3%会心",
               "且待时休·3%伤害", "且待时休·2%伤害",
               "银光照雪·5%会心", "银光照雪·4%会心", "银光照雪·3%伤害" });
    b.choices({ "相使", "紫伏", "凄骨", "灵荆", "甘遂", "六微", "熟地", "养荣", "香繁饮露", "岚因" }, {}, { "鉴魄证身" });
    b.default_talents({ "相使", "紫伏", "凄骨", "灵荆", "甘遂", "六微", "熟地", "养荣", "香繁饮露" });
    b.resource({ "温寒", 0, -5, 5 });
    b.state({ 0, "橙武", 96, 0, 1 }, false, false, false);
    b.state({ 0, "逆乱", 288, 32, 8 }, true, true, true);
    b.state({ 0, "相使", 288, 0, 1 }, false, false, false);
    b.state({ 0, "香繁饮露", 160, 0, 1 }, false);
    b.damage(0,
             "逆乱(DOT)",
             { 1, 0, 181, 0, 8, 32, false, { .pve = 379 }, { { b.buff("相使"), 0, { .attack = 154 }, "" } } });
    b.damage(28081, "无方中和", { 80, 8, 457, 0, 0, 0, false, { .pve = 379 }, { { b.buff("相使"), 0, { .attack = 154 }, "" } } });
    b.damage(27557, "川乌射罔", { 21, 1, 343, 0, 0, 0, false, { .pve = 379 }, { { b.buff("相使"), 0, { .attack = 154 }, "" } } });
    b.damage(27552, "商陆缀寒", { 111, 6, 253, 0, 0, 0, false, { .pve = 379 }, { { b.buff("相使"), 0, { .attack = 154 }, "" } } });
    b.secondary_damage(29695, "鉴魄证身", { 12, 1, 1276, 0, 0, 0, false, { .pve = 379 }, { { b.buff("相使"), 0, { .attack = 154 }, "" } } });
    b.damage(27555, "钩吻断肠", { 11, 1, 120, 0, 0, 0, false, { .pve = 379 }, { { b.buff("相使"), 0, { .attack = 154 }, "" } } });
    b.dot("逆乱", "逆乱(DOT)");
    b.cast("使用橙武", 800, 0, 0, { .choice = "鉴魄证身" }, { b.apply("橙武", 1), b.reset("川乌射罔") });
    Step extra_warm              = temperature(b, 1, "无方中和");
    extra_warm.empowered_channel = true;
    b.cast("川乌射罔",
           160,
           24,
           0,
           { .pulses = 4, .spacing = 9, .free_cd = b.buff("橙武"), .capture = b.buff("橙武"), .channel_buff = b.buff("香繁饮露"), .buff_pulses = 3 },
           { temperature(b, 1, "无方中和"),
             b.gain(1, 400),
             b.hit("川乌射罔", 1),
             b.captured(b.hit("商陆缀寒", 1)),
             b.captured(b.hit("鉴魄证身")),
             b.captured(temperature(b, -1, "无方中和")),
             extra_warm });
    b.cast("商陆缀寒", 0, 24, 24, {}, { b.hit("商陆缀寒", 1), temperature(b, -1, "无方中和") });
    b.cast("钩吻断肠",
           0,
           24,
           0,
           {},
           { b.selected("相使", b.apply("相使", 1)), temperature(b, 1, "无方中和"), b.hit("钩吻断肠", 1), b.apply("逆乱", 1) });
    b.base_action("川乌射罔").pre_cast = { b.captured(b.remove("橙武", 1000)) };
    b.recipe("商陆缀寒", "商陆缀寒·2%伤害", { .damage = 20 }, { "商陆缀寒" });
    b.recipe("商陆缀寒", "商陆缀寒·3%会心", { .critical = 300 }, { "商陆缀寒" });
    b.recipe("商陆缀寒", "商陆缀寒·3%伤害", { .damage = 31 }, { "商陆缀寒" });
    b.recipe("商陆缀寒", "商陆缀寒·4%会心", { .critical = 400 }, { "商陆缀寒" });
    b.recipe("川乌射罔", "川乌射罔·2%伤害", { .damage = 20 }, { "川乌射罔" });
    b.recipe("川乌射罔", "川乌射罔·3%会心", { .critical = 300 }, { "川乌射罔" });
    b.recipe("川乌射罔", "川乌射罔·3%伤害", { .damage = 31 }, { "川乌射罔" });
    b.recipe("川乌射罔", "川乌射罔·4%会心", { .critical = 400 }, { "川乌射罔" });
    b.recipe("钩吻断肠", "钩吻断肠·2%会心", { .critical = 200 }, { "钩吻断肠" });
    b.recipe("钩吻断肠", "钩吻断肠·2%伤害", { .damage = 20 }, { "钩吻断肠" });
    b.recipe("钩吻断肠", "钩吻断肠·3%会心", { .critical = 300 }, { "钩吻断肠" });
    b.recipe("钩吻断肠", "钩吻断肠·3%伤害", { .damage = 31 }, { "钩吻断肠" });
    b.recipe("钩吻断肠", "钩吻断肠·4%会心", { .critical = 400 }, { "钩吻断肠" });
    b.set_bonus({ .damage = 204 }, { "商陆缀寒", "川乌射罔" });
    b.state({ 0, "紫伏", 288, 0, 1 }, false);
    b.state({ 0, "凄骨", 960, 0, 1 }, false);
    const std::vector<Modifier> medicine{
        { b.buff("相使"), 0, { .attack = 154 }, {} }
    };
    b.damage(28434, "紫伏", { 3, 1, 238, 0, 0, 0, false, { .pve = 379 }, medicine });
    b.damage(0, "凄骨", { 10, 1, 600, 0, 0, 0, false, { .pve = 379 }, medicine });
    b.damage(27657, "苍棘缚地", { 13, 1, 297, 0, 0, 0, false, { .pve = 379 }, medicine });
    b.cast("紫叶沉疴", 416, 24, 0, { .choice = "紫伏" }, { b.apply("紫伏") });
    // 紫伏生效时，每次中和命中后追加紫伏伤害；中和本身还会返还内力。
    b.post_damage("无方中和", { b.when("紫伏", b.hit("紫伏")) });
    b.state({ 0, "苍棘蛰伏", 48, 0, 1 }, false);
    const std::initializer_list<Step> sprout{
        b.later("苍棘缚地", 8),
        b.later("苍棘缚地", 40),
        b.later("苍棘缚地", 72),
        b.later("苍棘缚地", 104),
        b.later("苍棘缚地", 136),
        b.selected("灵荆", b.later("苍棘缚地", 168)),
        b.selected("灵荆", b.later("苍棘缚地", 200)),
        b.selected("灵荆", b.later("苍棘缚地", 232))
    };
    // 苍棘蛰伏到期后才排入苍棘缚地的五次延迟命中；选灵荆时在队尾再加三次。
    b.on_expiry("苍棘蛰伏", sprout);
    b.cast("苍棘缚地", 400, 24, 16, { .cooldown_on_finish = true }, { b.apply("苍棘蛰伏") });
    b.passive("苍棘缚地", "灵荆", { .damage = 717 });
    b.post_damage("苍棘缚地", { b.selected("灵荆", b.apply("逆乱")) });
    b.post_cast("川乌射罔", { b.selected("凄骨", b.apply("凄骨")), b.selected("凄骨", b.hit("凄骨")), b.apply("逆乱") });
    b.damage(27579, "沾衣未妨", { 12, 1, 120, 0, 0, 0, false, { .pve = 379 }, medicine });
    b.cast("香繁饮露", 512, 0, 0, { .forbidden = b.buff("香繁饮露"), .choice = "香繁饮露", .charges = 2 }, { b.apply("香繁饮露") });
    b.state({ 0, "沾衣未妨", 65, 12, 1 }, true);
    b.state({ 0, "沾衣未妨·香繁", 41, 0, 1 }, true);
    b.tick_schedule("沾衣未妨", 17, 0);
    b.pulse("沾衣未妨", { temperature(b, -1, "无方中和"), b.hit("沾衣未妨"), b.when("沾衣未妨·香繁", temperature(b, -1, "无方中和")) });
    b.cast("沾衣未妨",
           256,
           0,
           0,
           {},
           { b.when("香繁饮露", b.apply("沾衣未妨"), true),
             b.when("香繁饮露", b.apply_for("沾衣未妨", 41)),
             b.when("香繁饮露", b.apply("沾衣未妨·香繁")),
             b.when("香繁饮露", b.shorten("沾衣未妨", 179)),
             b.remove("香繁饮露") });
    for (auto step : sprout) {
        b.pre_cast("沾衣未妨", { b.when("苍棘蛰伏", step) });
    }
    b.pre_cast("沾衣未妨", { b.remove("苍棘蛰伏") });
    b.pre_cast("川乌射罔", { b.when("香繁饮露", b.shorten("川乌射罔", 112)), b.remove("香繁饮露") });
    // 先复制川乌射罔的普通和强化引导配置，再给凄骨对应的招式改充能，保留两种引导各自的结算步骤。
    b.charge_variant("川乌射罔", "凄骨", 128, 2);
    b.damage(40208, "六微", { 2, 1, 804, 0, 0, 0, false, { .pve = 379 }, medicine });
    for (auto n : { "六微", "沾衣未妨", "苍棘缚地", "紫伏", "凄骨", "无方中和", "川乌射罔", "商陆缀寒", "钩吻断肠", "逆乱(DOT)", "鉴魄证身" })
    {
        b.pre_damage(n, { b.buff("凄骨"), 0, { .ignore = 717 }, "凄骨" });
    }
    // 内力满值记为 40000。每 4 帧自然回 7 点，
    // 正好是满内力的 0.0175%；绽蕊消耗和中和回蓝也用同一单位。
    b.resource({ .name = "内力", .initial = 40000, .low = 0, .high = 40000, .mana = true });
    b.state({ 22282, "千枝绽蕊", 0, 0, 1 }, false);
    b.state({ 0, "千枝伏藏", 0, 0, 1 }, false);
    b.state({ 0, "内力回复", 0, 4, 1 }, false);
    b.initial("内力回复");
    b.initial("千枝伏藏");
    b.cast("千枝绽蕊", 512, 0, 0, { .resource = 1, .cost = 2400, .forbidden = b.buff("千枝绽蕊") }, { b.remove("千枝伏藏"), b.apply("千枝绽蕊") });
    b.cast("千枝伏藏", 48, 0, 0, {}, { b.remove("千枝绽蕊"), b.apply("千枝伏藏") });
    b.pulse("内力回复",
            { b.when("千枝伏藏", b.gain(1, 100)),
              b.when("千枝绽蕊", b.gain(1, -500)),
              b.at(1, 0, 2399, b.when("千枝绽蕊", b.apply("千枝伏藏"))),
              b.at(1, 0, 2399, b.remove("千枝绽蕊")),
              b.gain(1, 7) });
    b.post_damage("无方中和", { b.gain(1, 100) });
    b.variant("商陆缀寒", "六微", 0, 24, { .timing_resource = 0, .instant_at = 5 }, { b.hit("商陆缀寒"), temperature(b, -1, "无方中和") });
    // 六微在出手时查药性极值：满温用商陆、满寒用钩吻，先打六微并反向改变两点温寒，再执行招式原有的一点变化。
    for (auto [spell, extreme, delta] :
         {
             std::tuple{ "商陆缀寒", 5,  -2 },
             std::tuple{ "钩吻断肠", -5, 2  }
    })
    {
        b.pre_cast(spell,
                  { b.selected("六微", b.at(0, extreme, extreme, b.hit("六微"))),
                    b.selected("六微", b.at(0, extreme, extreme, temperature(b, delta, "无方中和"))) });
    }
    b.post_cast("商陆缀寒", { b.selected("甘遂", b.shorten("紫叶沉疴", 16)), b.selected("甘遂", b.shorten("苍棘缚地", 16)) });
    b.adjust([](Prepared &p) {
        const auto &book = *p.config.book;
        const auto  aura = [&](std::string_view n) {
            return static_cast<Slot>(std::ranges::find(book.buffs, n, &BuffDescriptor::name) - book.buffs.begin());
        };
        const auto skill = [&](std::string_view n) {
            return std::ranges::find(book.skills, n, &SkillDescriptor::name) - book.skills.begin();
        };
        const auto has = [&](std::string_view n) {
            return std::ranges::contains(p.config.talents, n);
        };
        if (p.config.precombat) {
            p.resources[0].initial = -5;
        }
        const Slot flowers = has("养荣") ? aura("千枝绽蕊") : INVALID_SLOT;
        for (auto *lists : { &p.impacts, &p.pre_cast, &p.pulses }) {
            for (auto &steps : *lists) {
                for (auto &step : steps) {
                    if (step.effect == 中和) {
                        step.repeat_aura = flowers;
                        step.neutral_dot = aura("逆乱");
                    }
                }
            }
        }
        const auto merchant = skill("商陆缀寒");
        if (has("熟地")) {
            p.skills[merchant].cast_time -= 8;
            p.gcd_reduction[merchant]     = 8;
        }
        const auto channel = skill("川乌射罔");
        for (auto &step : p.pre_cast[channel]) {
            if (step.op == Op::ShortenCd) {
                step.amount = p.skills[channel].cooldown * 7 / 10;
            }
        }
    });
    b.damage(27584, "且待时休", {20, 1, 282, 0, 0, 0, false, { .pve = 379 }, medicine });
    b.cast("且待时休", 720, 24, 0, { .required = b.buff("逆乱"), .pulses = 4, .spacing = 16, .range = 15, .fixed_cast_time = true, .required_layers = 4 },
           { b.within(15, b.hit("且待时休")) });
    b.damage(28346, "银光照雪", {41, 2, 132, 0, 0, 0, false, { .pve = 379 }, medicine });
    b.cast("银光照雪", 160, 0, 0, { .range = 10 }, { b.hit("银光照雪") });
    b.recipe("且待时休", "且待时休·3%伤害", { .damage = 31 }, { "且待时休" });
    b.recipe("且待时休", "且待时休·2%伤害", { .damage = 20 }, { "且待时休" });
    b.recipe("银光照雪", "银光照雪·5%会心", { .critical = 500 }, { "银光照雪" });
    b.recipe("银光照雪", "银光照雪·4%会心", { .critical = 400 }, { "银光照雪" });
    b.recipe("银光照雪", "银光照雪·3%伤害", { .damage = 31 }, { "银光照雪" });
    b.legendary_damage({ "川乌射罔", "钩吻断肠" });
    // 岚因逐跳读取旧逆乱层数。低于四层必挂，高层逐跳掷 50% 概率。
    b.passive("沾衣未妨", "岚因", {.damage = 102});
    b.post_damage("沾衣未妨", {{.op = Op::School, .slot = b.buff("逆乱"), .choice = "岚因",
        .effect = [](EffectContext &f, const Step &s) {
            if (f.Stacks(s.slot) < 4 || f.Roll() % 2 == 0) f.SetAura(s.slot, 1);
        }}});
    return b.finish();
}
} // namespace JX3DPS::runtime::season50
