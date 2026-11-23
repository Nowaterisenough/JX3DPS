#include "common/builder.hpp"

namespace JX3DPS::runtime::season50 {

static void 绝刀耗怒(EffectContext &frame, const Step &step)
{
    // 绝刀按出手怒气选档：满足最低消耗后，每多 10 怒加一档，
    // 最多四档。临时绝返只覆盖这次绝刀；免耗时照常选档。
    const int  minimum  = frame.action->cost;
    const auto resource = frame.action->resource;
    const int  tier     = std::clamp((frame.Resource(resource) - minimum) / 10, 0, 4);
    if (tier) {
        frame.SetAura(step.extra, tier);
    }
    frame.Hit(step.slot);
    if (tier) {
        frame.SetAura(step.extra, -tier);
    }
    const auto free_cost = frame.action->weapon_free_cost;
    if (free_cost == INVALID_SLOT || !frame.Stacks(free_cost)) frame.ChangeResource(resource, -(minimum + tier * 10));
}

static void PrepareFenShan(Prepared &p)
{
    const auto &b     = *p.config.book;
    const auto  skill = [&](std::string_view n) {
        return static_cast<Slot>(std::ranges::find(b.skills, n, &SkillDescriptor::name) - b.skills.begin());
    };
    const auto buff = [&](std::string_view n) {
        return static_cast<Slot>(std::ranges::find(b.buffs, n, &BuffDescriptor::name) - b.buffs.begin());
    };
    const auto talent = [&](std::string_view n) {
        return std::ranges::find(p.config.talents, n) != p.config.talents.end();
    };
    const auto recipe = [&](std::string_view n) {
        return std::ranges::find(p.config.recipes, n) != p.config.recipes.end();
    };
    if (talent("登锋")) {
        const auto bleed = buff("流血");
        p.base_intervals[bleed] -= 16;
        p.dot_effects[bleed] = skill("流血·登锋(DOT)");
        p.dot_ticks[bleed] = b.hits[p.dot_effects[bleed]].ticks;
        p.aura_stack_caps[bleed] = 3;
    }
    p.resources[1].high = talent("朔荒") ? 120 : 40;
    if (p.config.precombat) {
        p.resources[0].initial = 100;
        p.resources[1].initial = p.resources[1].high;
        if (talent("阵云结晦") && talent("朔荒")) {
            Step next{ Op::Add, buff("可释放月照连营") };
            next.duration = 720;
            p.opening.push_back(next);
            next.slot = buff("朔荒·二段");
            p.opening.push_back(next);
        }
    }
    for (auto [action, book] : { std::pair{"斩刀", "斩刀·6尺内不突进"}, {"盾猛", "盾猛·6尺内不突进"} }) {
        p.ranges[skill(action)] = action == std::string_view("盾猛") ? 12 : 20;
        p.pre_cast[skill(action)].push_back({ Op::ApproachTarget, 0, recipe(book) ? 6 : 4 });
    }
    auto press = skill("盾压"), slam = skill("盾猛");
    p.skills[press].cooldown -= 16 * (recipe("盾压·调息-1秒（一）") + recipe("盾压·调息-1秒（二）"));
    if (talent("残裂")) {
        p.skills[slam].cooldown = 192;
    }
    if (talent("锋鸣")) {
        p.skills[skill("盾飞")].cooldown = 160;
    }
    if (talent("迸甲")) {
        p.skills[skill("血怒")].cooldown = 352;
    }
    const int duration = 16 * (10 + recipe("血怒·延长1秒（一）") + recipe("血怒·延长1秒（二）") + recipe("血怒·延长1秒（三）"));
    Step extension{ Op::ExtendAura, buff("血怒·惊涌") };
    extension.duration = duration;
    p.impacts[skill("血怒")].push_back(extension);
    const int chance = 3500 + 500 * (recipe("盾压·重置+5%（一）") + recipe("盾压·重置+5%（二）"));
    for (auto action : { slam, skill("盾刀"), skill("盾击") }) {
        p.impacts[action].push_back({ Op::RollResetCd, press, chance });
    }
    if (talent("斗塞")) {
        for (int i = 1; i <= 8; ++i) {
            Step refund{ Op::ShortenCd, slam, std::min(i, 6) * 16, buff("斗塞") };
            refund.exact_stacks = i;
            p.impacts[slam].push_back(refund);
        }
    }
}

Book 分山劲()
{
    Builder b("fsj", "分山劲", R"(# 魔盒宏适配 https://www.jx3box.com/macro/105778
# 紫武：盾姿满怒起手，战前预铺二档月照连营
/cast 使用风腰坠
/cast 使用橙武
/cast [nobuff:擎刀&buff:血怒·惊涌&bufftime:血怒·惊涌<4] 雁门迢递
/cast [nobuff:擎刀] 月照连营
/cast [武烈值>79] 阵云结晦
/cast [buff:擎刀&buff:血怒·惊涌&bufftime:血怒·惊涌<6] 雁门迢递
/cast [怒气<41] 盾回
/cast [buff:擎刀&nobuff:血怒·惊涌&skill_notin_cd:绝刀&怒气>79] 血怒
/cast 绝刀
/cast 盾猛
/cast 盾压
/cast 盾刀
/cast [怒气>89|怒气>64&buff:血怒·惊涌&bufftime:血怒·惊涌<6.6] 盾飞
/cast 卷雪刀
)");
    auto &school = b.definition();
    school.info = { school.code, "苍云", "身法", "Physics", 768, 0, 0 };
    school.display_order = 6;
    school.saved.macro_digest = "990005fb24c0416d58282c3176eeafec32684144834748cb5a77d96293930158";
    school.saved.talents_v6 = { "阵云结晦", "惊涌", "嗜血" };
    school.saved.added_recipes_v8 = { "盾击·三段回怒" };
    school.precombat = true;
    school.raid.penetration_perfume = true;
    school.raid.gear_score = 24030;
    b.preset(2298,
             { 8623, 1723, 252, 2057, 166, 2714, 8566, 0, 448 },
             { "绝刀·5%伤害", "绝刀·4%伤害", "绝刀·4%会心", "绝刀·怒气消耗-15",
               "斩刀·3%伤害", "斩刀·9%伤害", "斩刀·7%会心", "斩刀·6尺内不突进",
               "盾压·调息-1秒（一）", "盾压·调息-1秒（二）", "盾压·重置+5%（一）", "盾压·重置+5%（二）",
               "盾刀·三段回怒", "盾刀·5%伤害", "盾刀·4%伤害", "盾刀·3%伤害",
               "血怒·延长1秒（一）", "血怒·延长1秒（二）", "血怒·延长1秒（三）", "血怒·回怒+5",
               "盾飞·4%伤害", "盾飞·3%伤害", "盾飞·3%会心", "盾飞·2%会心",
               "盾猛·3%会心", "盾猛·3%伤害", "盾猛·4%伤害", "盾猛·6尺内不突进",
               "劫刀·5%伤害", "劫刀·4%会心", "劫刀·4%伤害", "劫刀·怒气消耗-5",
               "盾舞·4%会心", "盾舞·3%会心", "盾舞·5%伤害", "盾舞·4%伤害" });
    b.choices({ "阵云结晦", "绝霜", "惊涌", "偃守孤旌", "朔荒", "嗜血", "迸甲", "残裂", "斗塞", "锋鸣", "登锋" }, {}, {});
    b.default_talents({ "阵云结晦", "绝霜", "惊涌", "朔荒", "嗜血", "迸甲", "残裂", "斗塞", "锋鸣" });
    b.resource({ "怒气", 0, 0, 100, 0, 0, 1, 5, "阵云结晦" });
    b.resource({ "武烈值", 40, 0, 40 });
    b.state({ 0, "擎刀", 240, 0, 1 }, false);
    b.state({ 0, "盾飞", 240, 16, 1 }, false);
    b.state({ 0, "血怒·惊涌", 160, 0, 1 }, false);
    b.state({ 0, "嗜血", 192, 0, 1 }, false);
    b.state({ 0, "可释放月照连营", 1440, 0, 1 }, false);
    b.state({ 0, "可释放雁门迢递", 1440, 0, 1 }, false);
    b.state({ 0, "战绝", 144, 0, 1 }, false, false, false);
    b.state({ 0, "孤旌", 144, 48, 3 }, false, false, false);
    b.state({ 8249, "流血", 416, 32, 1 }, true, true, true);
    b.state({ 45804, "斗塞", 480, 0, 8 }, false);
    b.state({ 0, "锋鸣", 480, 0, 1 }, false);
    b.state({ 0, "虚弱", 384, 0, 1 }, true);
    b.state({ 0, "朔荒·二段", 1440, 0, 1 }, false);
    b.damage(29188,
             "流血(DOT)",
             {
                 1,
                 0,
                 122,
                 0,
                 13,
                 32,
                 true,
                 { .pve = 154 },
                 { { b.buff("孤旌"), 1, { .damage = 61 }, "" },
                   { b.buff("孤旌"), 2, { .damage = 123 }, "" },
                   { b.buff("孤旌"), 3, { .damage = 184 }, "" } }
    });
    b.damage(13044,
             "盾刀·一",
             {
                 80,
                 8,
                 225,
                 1024,
                 0,
                 0,
                 true,
                 { .pve = 154 },
                 { { b.buff("孤旌"), 1, { .damage = 61 }, "" },
                   { b.buff("孤旌"), 2, { .damage = 123 }, "" },
                   { b.buff("孤旌"), 3, { .damage = 184 }, "" } }
    });
    b.damage(13244,
             "盾刀·二",
             {
                 13,
                 1,
                 246,
                 1024,
                 0,
                 0,
                 true,
                 { .pve = 154 },
                 { { b.buff("孤旌"), 1, { .damage = 61 }, "" },
                   { b.buff("孤旌"), 2, { .damage = 123 }, "" },
                   { b.buff("孤旌"), 3, { .damage = 184 }, "" } }
    });
    b.damage(13060,
             "盾刀·三",
             {
                 13,
                 1,
                 269,
                 1024,
                 0,
                 0,
                 true,
                 { .pve = 154 },
                 { { b.buff("孤旌"), 1, { .damage = 61 }, "" },
                   { b.buff("孤旌"), 2, { .damage = 123 }, "" },
                   { b.buff("孤旌"), 3, { .damage = 184 }, "" } }
    });
    b.damage(19409,
             "盾压",
             {
                 20,
                 1,
                 575,
                 1024,
                 0,
                 0,
                 true,
                 { .pve = 154 },
                 { { b.buff("孤旌"), 1, { .damage = 61 }, "" },
                   { b.buff("孤旌"), 2, { .damage = 123 }, "" },
                   { b.buff("孤旌"), 3, { .damage = 184 }, "" } }
    });
    b.damage(13099,
             "盾猛",
             {
                 13,
                 1,
                 1003,
                 1024,
                 0,
                 0,
                 true,
                 { .pve = 154 },
                 { { b.buff("孤旌"), 1, { .damage = 61 }, "" },
                   { b.buff("孤旌"), 2, { .damage = 123 }, "" },
                   { b.buff("孤旌"), 3, { .damage = 184 }, "" } }
    });
    b.damage(13092,
             "斩刀",
             {
                 22,
                 0,
                 1197,
                 1024,
                 0,
                 0,
                 true,
                 { .pve = 154 },
                 { { b.buff("孤旌"), 1, { .damage = 61 }, "" },
                   { b.buff("孤旌"), 2, { .damage = 123 }, "" },
                   { b.buff("孤旌"), 3, { .damage = 184 }, "" } }
    });
    b.damage(28479,
             "劫刀",
             {
                 105,
                 8,
                 776,
                 1024,
                 0,
                 0,
                 true,
                 { .pve = 154 },
                 { { b.buff("孤旌"), 1, { .damage = 61 }, "" },
                   { b.buff("孤旌"), 2, { .damage = 123 }, "" },
                   { b.buff("孤旌"), 3, { .damage = 184 }, "" } }
    });
    b.pulse("孤旌", { b.gain(0, 100), b.remove("孤旌", 1) });
    b.dot("流血", "流血(DOT)");
    b.damage(13106, "盾击·一", {10, 1, 307, 1024, 0, 0, true, {.pve = 154, .ignore = 512}});
    b.damage(13160, "盾击·二", {10, 1, 307, 1024, 0, 0, true, {.pve = 154, .ignore = 512}});
    b.damage(13161, "盾击·三", {10, 1, 307, 1024, 0, 0, true, {.pve = 154, .ignore = 512}});
    b.secondary_damage(25780, "盾击·神兵", {20, 2, 50, 0, 0, 0, true, {.pve = 154}});
    b.damage(13047, "盾击", {});
    b.cast("盾击", 64, 16, 0, {.forbidden = b.buff("擎刀"), .chain_window = 32, .charges = 3, .range = 4},
           {b.cycle("盾击·一", 3), b.gain(0, 10), b.shorten("盾飞", 32), b.selected(Legendary, b.proc("盾击·神兵", 256))});
    // 盾击三段回怒由第三段命中触发，前两段不提前发放这 5 点怒气。
    b.recipe("盾击", "盾击·三段回怒", {}, {});
    b.post_damage("盾击·三", {b.selected("盾击·三段回怒", b.gain(0, 5))});
    b.cast("盾刀", 0, 16, 0, { .forbidden = b.buff("擎刀"), .chain_window = 48 }, { b.cycle("盾刀·一", 3), b.gain(0, 10) });
    b.cast("盾压", 192, 24, 0, { .forbidden = b.buff("擎刀") }, { b.hit("盾压", 1), b.gain(0, 20) });
    b.cast("盾猛", 240, 24, 0, { .forbidden = b.buff("擎刀") }, { b.hit("盾猛", 1), b.gain(0, 15) });
    b.cast("偃守孤旌",
           640,
           0,
           0,
           { .forbidden = b.buff("擎刀"), .choice = "偃守孤旌" },
           { b.apply("战绝", 1), b.apply_for("擎刀", 144), b.apply("孤旌", 3), b.gain(0, 100), b.reset("斩刀") });
    // 斩刀命中已有虚弱或流血的目标才施加流血，两者同时存在也只叠一次。
    Step bleed{.op = Op::School, .slot = b.buff("流血"), .extra = b.buff("虚弱"), .effect = [](EffectContext &frame, const Step &step) {
        if (frame.Stacks(step.slot) || frame.Stacks(step.extra)) frame.SetAura(step.slot, 1);
    }};
    b.cast("斩刀",
           160,
           24,
           0,
           { .resource = 0, .cost = 15, .required = b.buff("擎刀"), .free_cd = b.buff("战绝") },
           { b.hit("斩刀", 1), b.gain(0, -15), bleed, b.selected("登锋", b.stacks("流血", 3, b.gain(0, 15))) });
    b.cast("劫刀", 0, 24, 0, { .resource = 0, .cost = 10, .required = b.buff("擎刀") }, { b.hit("劫刀", 1), b.gain(0, -10) });
    b.recipe("劫刀", "劫刀·2%会心", { .critical = 200 }, { "劫刀" });
    b.recipe("劫刀", "劫刀·3%会心", { .critical = 300 }, { "劫刀" });
    b.recipe("劫刀", "劫刀·3%伤害", { .damage = 31 }, { "劫刀" });
    b.recipe("劫刀", "劫刀·4%会心", { .critical = 400 }, { "劫刀" });
    b.recipe("劫刀", "劫刀·4%伤害", { .damage = 41 }, { "劫刀" });
    b.recipe("劫刀", "劫刀·5%伤害", { .damage = 51 }, { "劫刀" });
    b.recipe("斩刀", "斩刀·2%会心", { .critical = 200 }, { "斩刀" });
    b.recipe("斩刀", "斩刀·3%伤害", { .damage = 31 }, { "斩刀" });
    b.recipe("斩刀", "斩刀·7%会心", { .critical = 700 }, { "斩刀" });
    b.recipe("斩刀", "斩刀·9%伤害", { .damage = 92 }, { "斩刀" });
    b.recipe("盾刀", "盾刀·2%会心", { .critical = 200 }, { "盾刀·一", "盾刀·三", "盾刀·二" });
    b.recipe("盾刀", "盾刀·3%会心", { .critical = 300 }, { "盾刀·一", "盾刀·三", "盾刀·二" });
    b.recipe("盾刀", "盾刀·3%伤害", { .damage = 31 }, { "盾刀·一", "盾刀·三", "盾刀·二" });
    b.recipe("盾刀", "盾刀·4%会心", { .critical = 400 }, { "盾刀·一", "盾刀·三", "盾刀·二" });
    b.recipe("盾刀", "盾刀·4%伤害", { .damage = 41 }, { "盾刀·一", "盾刀·三", "盾刀·二" });
    b.recipe("盾刀", "盾刀·5%伤害", { .damage = 51 }, { "盾刀·一", "盾刀·三", "盾刀·二" });
    b.recipe("盾压", "盾压·3%会心", { .critical = 300 }, { "盾压" });
    b.recipe("盾压", "盾压·4%会心", { .critical = 400 }, { "盾压" });
    b.recipe("盾压", "盾压·4%伤害", { .damage = 41 }, { "盾压" });
    b.recipe("盾压", "盾压·5%伤害", { .damage = 51 }, { "盾压" });
    b.recipe("盾猛", "盾猛·2%会心", { .critical = 200 }, { "盾猛" });
    b.recipe("盾猛", "盾猛·3%会心", { .critical = 300 }, { "盾猛" });
    b.recipe("盾猛", "盾猛·3%伤害", { .damage = 31 }, { "盾猛" });
    b.recipe("盾猛", "盾猛·4%伤害", { .damage = 41 }, { "盾猛" });
    b.state({ 9052, "绝返", 0, 0, 4 }, false);
    constexpr int finisher_channel = 668;
    b.damage(13075,
             "绝刀",
             {
                 18,
                 1,
                 finisher_channel,
                 1024,
                 0,
                 0,
                 true,
                 { .pve = 154, .ignore = 1024 },
                 {}
    });
    for (int tier = 1; tier <= 4; ++tier) {
        b.pre_damage("绝刀", { .aura = b.buff("绝返"), .stacks = tier, .stats = {}, .channel_scale = 1.0 + tier * 0.2 });
        // 嗜血把绝返的每档系数提升从 20% 改为 30%，按基础系数重新取整，覆盖普通档位。
        b.pre_damage("绝刀", {b.buff("绝返"), tier, {.channel_override = static_cast<int>(finisher_channel * (1.0 + tier * 0.3))}, "嗜血"});
    }
    for (int tier = 1; tier <= 3; ++tier) {
        b.pre_damage("绝刀", { b.buff("孤旌"), tier, { .damage = (tier * 6144 + 50) / 100 }, {} });
    }
    b.state({0, "边魂不寐", 64, 0, 1}, false);
    const Step spend{ .op = Op::School, .slot = b.effect("绝刀"), .extra = b.buff("绝返"), .effect = 绝刀耗怒 };
    b.cast("绝刀", 0, 24, 0, { .resource = 0, .cost = 25, .required = b.buff("擎刀"), .weapon_free_cost = b.buff("边魂不寐") }, { spend });
    b.variant("绝刀", "绝刀·怒气消耗-15", 0, 0, { .resource = 0, .cost = 10, .required = b.buff("擎刀"), .weapon_free_cost = b.buff("边魂不寐") }, { spend });
    b.variant("劫刀", "劫刀·怒气消耗-5", 0, 0, { .resource = 0, .cost = 5, .required = b.buff("擎刀") }, { b.hit("劫刀"), b.gain(0, -5) });
    b.recipe("绝刀", "绝刀·怒气消耗-15", {}, {});
    b.recipe("绝刀", "绝刀·5%伤害", { .damage = 51 }, { "绝刀" });
    b.recipe("绝刀", "绝刀·4%伤害", { .damage = 41 }, { "绝刀" });
    b.recipe("绝刀", "绝刀·4%会心", { .critical = 400 }, { "绝刀" });
    b.recipe("劫刀", "劫刀·怒气消耗-5", {}, {});
    b.recipe("劫刀", "劫刀·4%会心", { .critical = 400 }, { "劫刀" });
    for (auto recipe : { "盾压·调息-1秒（一）", "盾压·调息-1秒（二）", "盾压·重置+5%（一）", "盾压·重置+5%（二）" }) {
        b.recipe("盾压", recipe, {}, {});
    }
    for (auto recipe : { "血怒·延长1秒（一）", "血怒·延长1秒（二）", "血怒·延长1秒（三）", "血怒·回怒+5" }) {
        b.recipe("血怒", recipe, {}, {});
    }
    b.recipe("盾刀", "盾刀·三段回怒", {}, {});
    b.post_damage("盾刀·三", { b.selected("盾刀·三段回怒", b.gain(0, 5)) });
    b.damage(13463, "盾飞", { 4, 0, 64, 1024, 0, 0, true, { .pve = 154 }, {} });
    b.cast("盾飞",
           288,
           16,
           6,
           { .forbidden = b.buff("擎刀"), .charges = 3, .gcd_group = 1, .fixed_cast_time = true },
           { b.apply("擎刀"), b.apply("盾飞"), b.apply("虚弱"), b.hit("盾飞") });
    b.pulse("盾飞", { b.when("流血", b.apply("虚弱"), true), b.hit("盾飞") });
    b.base_action("盾飞").pre_cast = { b.selected("锋鸣", b.apply("锋鸣")) };
    b.recipe("盾飞", "盾飞·4%伤害", { .damage = 41 }, { "盾飞" });
    b.recipe("盾飞", "盾飞·3%伤害", { .damage = 31 }, { "盾飞" });
    b.recipe("盾飞", "盾飞·2%会心", { .critical = 200 }, { "盾飞" });
    b.recipe("盾飞", "盾飞·3%会心", { .critical = 300 }, { "盾飞" });
    b.cast("盾回", 0, 16, 2, { .required = b.buff("盾飞"), .gcd_group = 1, .fixed_cast_time = true }, { b.remove("盾飞"), b.remove("擎刀") });
    b.cast("血怒",
           400,
           8,
           0,
           { .choice = "惊涌", .lock = 8, .charges = 3, .gcd_group = 2 },
           { b.gain(0, 10), b.selected("血怒·回怒+5", b.gain(0, 5)), b.selected("迸甲", b.gain(1, 40)) });
    b.damage(30925, "阵云结晦", { 7, 0, 853, 1024, 0, 0, true, { .pve = 154 }, {} });
    b.damage(30926, "月照连营", { 9, 0, 1074, 1024, 0, 0, true, { .pve = 154 }, {} });
    b.damage(30857, "雁门迢递", { 12, 0, 1294, 1024, 0, 0, true, { .pve = 154 }, {} });
    b.cast("阵云结晦",
           0,
           24,
           0,
           { .resource = 1, .cost = 40, .forbidden = b.buff("可释放月照连营"), .choice = "阵云结晦", .also_forbidden = b.buff("可释放雁门迢递") },
           { b.gain(1, -40), b.hit("阵云结晦"), b.apply("可释放月照连营") });
    b.cast("月照连营", 0, 24, 0, { .required = b.buff("可释放月照连营") }, { b.remove("可释放月照连营"), b.hit("月照连营"), b.apply("可释放雁门迢递") });
    b.cast("雁门迢递", 0, 24, 0, { .required = b.buff("可释放雁门迢递") }, { b.remove("可释放雁门迢递"), b.hit("雁门迢递") });
    // 朔荒二段覆盖阵云结晦、月照连营和雁门迢递。
    // 三招各用自己的二档系数，共用 1997/1024 对非侠士增伤。
    for (auto [name, channel] :
         {
             std::pair{ "阵云结晦", 1493 },
             std::pair{ "月照连营", 1876 },
             std::pair{ "雁门迢递", 2266 }
    })
    {
        b.pre_damage(name,
                   {
                       b.buff("朔荒·二段"),
                       0,
                       { .pve = 1997, .channel_override = channel },
                       "朔荒"
        });
        b.passive(name, "阵云结晦", { .ignore = 1024 });
    }
    // 阵云结晦起手清旧标记；选朔荒且出手时武烈值至少 80 才记二段并额外扣 40。雁门迢递收招后清标记。
    b.base_action("阵云结晦").pre_cast = { b.remove("朔荒·二段"), b.selected("朔荒", b.at(1, 80, 120, b.apply("朔荒·二段"))), b.when("朔荒·二段", b.gain(1, -40)) };
    b.base_action("雁门迢递").post_cast = { b.remove("朔荒·二段") };
    b.damage(13039, "卷雪刀", { 0, 0, 24, 1024, 0, 0, true, { .pve = 154 }, {} });
    b.cast("卷雪刀", 24, 0, 0, { .range = 4 }, { b.hit("卷雪刀") });
    b.passive("盾压", "绝霜", { .damage = 1116 });
    for (auto hit : { "盾刀·一", "盾刀·二", "盾刀·三" }) {
        b.passive(hit, "绝霜", { .damage = 1147 });
    }
    // 盾刀收招叠一层斗塞，盾压叠两层；盾猛命中按当时层数增伤，收招再消耗斗塞并结算残裂回怒。
    b.base_action("盾刀").post_cast = { b.selected("斗塞", b.apply("斗塞")) };
    b.base_action("盾压").post_cast = { b.selected("斗塞", b.apply("斗塞", 2)) };
    b.passive("盾猛", "斗塞", { .ignore = 1024 });
    for (int stacks = 1; stacks <= 8; ++stacks) {
        b.pre_damage("盾猛", { b.buff("斗塞"), stacks, { .damage = (std::min(stacks, 6) * 3072 + 5) / 10 }, "斗塞" });
    }
    b.base_action("盾猛").post_cast = { b.selected("残裂", b.gain(0, 20)), b.remove("斗塞", 6) };
    b.passive("绝刀", "嗜血", { .critical = 1500, .power = 205 });
    b.post_damage("绝刀", { b.selected("嗜血", b.apply("嗜血")) });
    b.secondary_damage(34108, "裂伤(DOT)", {25, 0, 1045, 0, 6, 48, true, {.pve = 154}});
    for (auto hit : {"盾击·一", "盾击·二", "盾击·三", "盾击·神兵"}) {
        for (int stacks = 1; stacks <= 3; ++stacks)
            b.pre_damage(hit, {b.buff("孤旌"), stacks, {.damage = stacks == 1 ? 61 : stacks == 2 ? 123 : 184}});
        b.passive(hit, "惊涌", {.strain_rating = 205});
        b.pre_damage(hit, {b.buff("嗜血"), 0, {.damage = 51}, "嗜血"});
        b.pre_damage(hit, {b.buff("锋鸣"), 0, {.attack = 154}, "锋鸣"});
        b.pre_damage(hit, { .aura = b.buff("虚弱"), .stats = { .ignore = 51 }, .replaced_by_raid = true });
        b.pre_damage(hit, {b.buff("血怒·惊涌"), 0, {.attack = 102}, "惊涌"});
    }
    b.pre_damage("斩刀", { b.buff("血怒·惊涌"), 0, { .pve = 1024 }, "惊涌" });
    b.pre_damage("绝刀", { b.buff("血怒·惊涌"), 0, { .pve = 727 }, "惊涌" });
    for (auto hit :
         { "盾飞", "绝刀", "劫刀", "斩刀", "盾猛", "盾压", "盾刀·一", "盾刀·二", "盾刀·三", "流血(DOT)", "阵云结晦", "月照连营", "雁门迢递", "裂伤(DOT)" })
    {
        b.passive(hit, "惊涌", { .strain_rating = 205 });
        b.pre_damage(hit, { b.buff("嗜血"), 0, { .damage = 51 }, "嗜血" });
    }
    for (auto hit : { "盾猛", "盾压", "盾刀·一", "盾刀·二", "盾刀·三", "斩刀", "绝刀", "劫刀", "阵云结晦", "月照连营", "雁门迢递", "流血(DOT)", "裂伤(DOT)" })
    {
        b.pre_damage(hit, { b.buff("锋鸣"), 0, { .attack = 154 }, "锋鸣" });
    }
    for (auto hit :
         { "卷雪刀", "盾飞", "盾猛", "盾压", "盾刀·一", "盾刀·二", "盾刀·三", "绝刀", "斩刀", "劫刀", "阵云结晦", "月照连营", "雁门迢递", "流血(DOT)", "裂伤(DOT)" })
    {
        b.pre_damage(hit, { .aura = b.buff("虚弱"), .stats = { .ignore = 51 }, .replaced_by_raid = true });
        b.pre_damage(hit, { b.buff("血怒·惊涌"), 0, { .attack = 102 }, "惊涌" });
    }
    b.adjust(PrepareFenShan);
    b.set_bonus({ .damage = 102 }, { "盾压", "绝刀" });
    b.default_gear({ ClassSet, WindPendant, WaterWeapon, BeltEnchant, "昆吾·弦刃·英雄", "刃凌·英雄" });
    b.recipe("斩刀", "斩刀·6尺内不突进", {}, {});
    b.damage(38973, "盾舞", {32, 3, 156, 1024, 0, 0, true, { .pve = 154 } });
    b.cast("盾舞", 128, 16, 0, { .forbidden = b.buff("擎刀"), .pulses = 10, .spacing = 8, .range = 8, .distribute_pulses = true },
           { b.within(8, b.hit("盾舞")), b.within(8, b.gain(0, 1)) });
    b.recipe("盾舞", "盾舞·4%会心", { .critical = 400 }, { "盾舞" });
    b.recipe("盾舞", "盾舞·3%会心", { .critical = 300 }, { "盾舞" });
    b.recipe("盾舞", "盾舞·5%伤害", { .damage = 51 }, { "盾舞" });
    b.recipe("盾舞", "盾舞·4%伤害", { .damage = 41 }, { "盾舞" });
    b.recipe("盾猛", "盾猛·6尺内不突进", {}, {});
    b.state({25797, "裂伤", 288, 48, 3}, true, true, true);
    b.dot("裂伤", "裂伤(DOT)");
    // 使用橙武回 70 怒并重置盾压。边魂不寐期间盾压免调息、收招回 20 怒，绝刀收招挂裂伤。
    b.cast("使用橙武", 800, 0, 0, {.choice = Legendary}, {b.gain(0, 70), b.reset("盾压"), b.apply("边魂不寐")});
    b.no_cooldown_during("盾压", "边魂不寐");
    b.post_cast("盾压", {b.when("边魂不寐", b.gain(0, 20))});
    b.post_cast("绝刀", {b.when("边魂不寐", b.apply("裂伤"))});
    b.pre_damage("绝刀", {b.buff("边魂不寐"), 0, {.damage = 307}});
    b.legendary_damage({ "盾飞", "斩刀" });
    // 31385 二级流血使用自己的 26 跳、16 帧间隔和 41737 系数；沿用本门对流血的被动修正。
    auto enhanced_bleed = b.definition().hits[b.effect("流血(DOT)")];
    enhanced_bleed.channel = static_cast<int>(244.4904 * 1.8);
    enhanced_bleed.ticks = 26;
    enhanced_bleed.interval = 16;
    b.damage(41737, "流血·登锋(DOT)", std::move(enhanced_bleed));
    return b.finish();
}
} // namespace JX3DPS::runtime::season50
