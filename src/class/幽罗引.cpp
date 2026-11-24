#include "common/builder.hpp"

namespace JX3DPS::runtime::season50 {
namespace {
constexpr Slot PuppetGcd = 3;
void 记录傀儡公调(EffectContext &f, const Step &s)
{
    f.SetAura(s.slot, 1, f.state.gcd_ready_at[PuppetGcd] - f.state.now);
}
}

Book 幽罗引()
{
    Builder b("yly", "幽罗引", R"(# 魔盒宏适配 https://www.jx3box.com/macro/105686
/cast 使用风腰坠
/cast 使用橙武
/cast [buff:天羊盏] 夹线
/cast [nobuff:连环慢] 连环慢
/cast 障幕虚影
/cast [tbuff:缠绞>3] 生地狱
/cast [bufftime:钤束<6] 绊线
/cast [tnobuff:千里急DOT] 应天长
/cast [心络>44] 勾线
/cast [心络>54] 绊线
/cast 挑丝
)");
    auto &school = b.definition();
    school.info = { school.code, "无相", "根骨", "Lunar", 717, 51, 0 };
    school.display_order = 17;
    school.saved.macro_digest = "cbfacdcf2dc3c05fab6bbd8589f6877a26e17b78268b84364921081cedaaf0fe";
    school.saved.talents_v7 = { "钤束", "绞网", "寄意" };
    school.saved.talents_v6 = { "钤束" };
    school.raid.formation = "易筋阵（常驻）";
    b.preset(2301,
             { 6248, 1611, 0, 1979, 208, 1153, 4212, 0, 1092 },
             { "绊线·5%伤害", "绊线·4%伤害", "绊线·3%会心", "绊线·2%会心",
               "千里急·3%会心", "千里急·2%会心", "千里急·3%系数", "千里急·4%系数",
               "挑丝·4%伤害", "挑丝·5%伤害", "挑丝·2%会心",
               "生地狱·3%伤害", "生地狱·2%会心", "生地狱·2%伤害", "生地狱·3%会心",
               "障幕虚影·3%伤害", "障幕虚影·2%伤害" });
    b.choices({ "钤束", "绞网", "寄意", "黄粱", "应天长", "鼓关", "寡合", "迭影", "谐调", "叠拍" }, {}, {});
    b.default_talents({ "叠拍", "钤束", "寄意", "黄粱", "应天长", "鼓关", "寡合", "迭影", "谐调" });
    b.resource({ .name = "心络", .initial = 100, .low = 0, .high = 100, .regen_interval = 16, .regen = 1, .first_regen = 8 });
    b.state({ 0, "傀儡", 160, 0, 1 }, false);
    const Slot puppet = b.buff("傀儡");
    b.state({ 0, "缠绞", 192, 0, 5 }, true, false, false);
    b.state({ 0, "钤束", 288, 0, 1 }, false, false, false);
    b.state({ 0, "千里急DOT", 80, 16, 1 }, true, true, true);
    b.damage(42702, "千里急(DOT)", { 1, 0, 1200, 0, 5, 32, false, { .pve = 553 } });
    b.damage(42417, "应天长", { 15, 1, 1400, 1024, 0, 0, false, { .pve = 553 } });
    b.damage(41465, "挑丝", { 15, 1, 480, 0, 0, 0, false, { .pve = 553 } });
    b.damage(42265, "绊线", { 15, 1, 720, 0, 0, 0, false, { .pve = 553 } });
    b.damage(41486, "勾线", { 43, 2, 422, 0, 0, 0, false, { .pve = 553 } });
    b.ranks("勾线", { 422, 444, 466, 488, 510, 532, 554, 576, 598, 620 });
    b.damage(42976, "谐调", { 150, 1, 500, 1024, 0, 0, false, { .pve = 553 } });
    b.ranks("谐调", { 500, 1000, 2000 });
    b.dot("千里急DOT", "千里急(DOT)");
    b.cast("应天长", 480, 24, 0, { .forbidden = puppet, .choice = "应天长", .charges = 3 }, { b.gain(0, 20), b.hit("应天长", 1) });
    b.cast("挑丝", 0, 24, 0, { .forbidden = puppet }, { b.gain(0, 15), b.hit("挑丝", 1) });
    b.cast("绊线",
           0,
           24,
           0,
           { .resource = 0, .cost = 10, .forbidden = puppet },
           { b.gain(0, -10), b.hit("绊线", 1), b.selected("钤束", b.apply("钤束", 1)), b.apply("缠绞", 1) });
    // 勾线按起手心络花 20／30／40 点，分别引导 4／7／10 段。
    // 每段先按段序打勾线，再叠缠绞；完整引导才打谐调。
    b.cast("勾线",
           288,
           0,
           0,
           { .resource          = 0,
             .cost              = 20,
             .forbidden         = puppet,
             .pulses            = 4,
             .spacing           = 8,
             .channel_cost_step = 10,
             .extra_pulses      = 3,
             .max_cost_steps    = 2,
             .charges           = 2, .distribute_pulses = true },
           { b.ranked_pulse("勾线"), b.apply("缠绞", 1) });
    b.recipe("挑丝", "挑丝·2%会心", { .critical = 200 }, { "挑丝" });
    b.recipe("挑丝", "挑丝·4%伤害", { .damage = 41 }, { "挑丝" });
    b.recipe("挑丝", "挑丝·5%伤害", { .damage = 51 }, { "挑丝" });
    // 勾线完整引导才打谐调，卸下奇穴后只结算各段勾线。
    auto harmony = b.base_action("勾线");
    harmony.finisher = b.effect("谐调");
    b.variant("勾线", "谐调", 288, 0, harmony, harmony.impact);
    b.recipe("绊线", "绊线·2%会心", { .critical = 200 }, { "绊线" });
    b.recipe("绊线", "绊线·3%会心", { .critical = 300 }, { "绊线" });
    b.recipe("绊线", "绊线·4%伤害", { .damage = 41 }, { "绊线" });
    b.recipe("绊线", "绊线·5%伤害", { .damage = 51 }, { "绊线" });
    b.damage(42098, "寄线", { 15, 1, 170, 0, 0, 0, false, { .pve = 553 }, {} });
    b.passive("寄线", "绞网", { .fragile = 1024 });

    b.adjust([](Prepared &p) {
        const auto &book = *p.config.book;
        if (!std::ranges::contains(p.config.talents, "鼓关")) {
            const auto order = std::ranges::find(book.buffs, "千里急次序", &BuffDescriptor::name) - book.buffs.begin();
            const auto third = std::ranges::find(book.skills, "千里急·叁", &SkillDescriptor::name) - book.skills.begin();
            p.post_damage[third].push_back({ Op::Remove, static_cast<Slot>(order), 3 });
        }
        // 绞网让心络每次支出时按每 10 点触发一次寄线，所以勾线的耗费档位也决定寄线次数。
        if (std::ranges::contains(p.config.talents, std::string_view{ "绞网" })) {
            p.resources[0].spent_per_point = 10;
            p.resources[0].spent_hit =
                static_cast<Slot>(std::ranges::find(p.skills, std::string_view{ "寄线" }, &SkillDescriptor::name) -
                                  p.skills.begin());
        }
    });
    b.state({ 0, "虚影", 160, 16, 1 }, true);
    b.state({ 0, "傀儡公调", 24, 0, 1 }, false);
    b.state({ 0, "连环慢", 128, 0, 1 }, false);
    b.state({ 0, "伶影驻场", 0, 0, 1 }, false);
    const Slot puppet_position = b.buff("伶影驻场");
    b.cast("落笼簿", 0, 16, 0, { .gcd_group = 2, .cast_while_channeling = true }, { { Op::ToggleAura, b.buff("傀儡") } });
    // 控制中再按落笼簿实际施展「合」。仅进入时开启叠拍调息、重置傀儡公调。
    const auto reset_puppet_gcd = b.when("傀儡", { Op::ResetGcd, PuppetGcd }, true);
    b.variant("落笼簿", "叠拍", 160, 0, { .gcd_group = 2, .cast_while_channeling = true, .bypass_cooldown_aura = b.buff("傀儡") },
              { reset_puppet_gcd, b.when("傀儡", b.remove("傀儡公调"), true), { .op = Op::ToggleAura, .slot = b.buff("傀儡"), .duration = 64 } });
    b.cast("落笼簿·合", 0, 16, 0, { .required = puppet, .gcd_group = 2, .cast_while_channeling = true }, { b.remove("傀儡") });
    // 连环慢强化下一式傀儡调。生地狱锁定三段，千里急在联动及应天长的 DOT 快照后消耗。
    b.cast("连环慢", 128, 4, 0, { .charges = 2, .gcd_group = 4, .cast_while_channeling = true },
           { b.apply("伶影驻场"), b.apply("连环慢") });
    b.state({ 0, "迭影", 80, 0, 1 }, false);
    b.damage(42576, "障幕虚影", { 15, 1, 280, 1024, 0, 0, false, { .pve = 553 } });
    b.cast("障幕虚影", 1440, 24, 0, { .gcd_group = 1 }, { b.apply("虚影"), b.hit("寄线", 5) });
    b.pulse("虚影", { b.hit("障幕虚影") });
    b.post_cast("挑丝", { b.selected("黄粱", b.shorten("障幕虚影", 48)) });
    b.damage(42021, "生地狱", { 150, 1, 196, 1024, 0, 0, false, { .pve = 553 } });
    b.ranks("生地狱", {197, 227, 256, 286, 316, 345});
    b.rank_bases("生地狱", {{150,1}, {170,1}, {190,1}, {210,1}, {230,1}, {250,1}});
    b.state({ 0, "生地狱·操控", 15, 0, 1 }, false);
    b.state({ 0, "生地狱·连环慢", 15, 0, 1 }, false);
    b.pre_damage("生地狱", { .aura = b.buff("生地狱·操控"), .stacks = 0, .stats = {}, .channel_scale = 2.2 });
    b.pre_damage("生地狱", { .aura = b.buff("生地狱·连环慢"), .stacks = 0, .stats = {}, .channel_scale = 1.25 });
    b.passive("生地狱", "寄意", { .critical = 1500, .power = 150 });
    b.cast("生地狱", 480, 24, 0, { .required = b.buff("傀儡"), .gcd_group = PuppetGcd, .cast_while_channeling = true }, {});
    b.variant("生地狱", "迭影", 480, 0, { .gcd_group = PuppetGcd, .cast_while_channeling = true }, { b.apply("迭影") });
    const Step record_gcd{ .op = Op::School, .slot = b.buff("傀儡公调"), .effect = 记录傀儡公调 };
    b.pre_cast("生地狱", { record_gcd, b.when("傀儡", b.apply("生地狱·操控")),
                         b.when("连环慢", b.apply("生地狱·连环慢")), b.remove("连环慢") });
    // 生地狱出手时按缠绞层数加一锁定三段档位，
    // 分别延迟 4、9、14 帧。收招清缠绞后，已经排定的档位仍有效。
    for (int delay : {4, 9, 14}) {
        b.pre_cast("生地狱", {{ .op = Op::DelayedHit, .slot = b.effect("生地狱"), .amount = delay, .extra = b.buff("缠绞"), .rank_offset = 1 }});
    }
    b.post_cast("生地狱", { b.remove("缠绞", 5) });
    b.post_cast("绊线", { b.when("迭影", b.gain(0, 10)), b.when("迭影", { Op::ShortenGcd, 0, 8 }) });
    b.post_cast("生地狱", { b.selected("寄意", b.shorten("勾线", 16)) });
    b.pre_damage("绊线", { b.buff("迭影"), 0, { .damage = 512 } });
    constexpr std::string_view skills[]{ "千里急·壹", "千里急·贰", "千里急·叁", "千里急·肆" };
    constexpr int channels[]{ 140, 210, 280, 420 };
    for (int i = 0; i < 4; ++i) b.damage(i == 3 ? 42384 : 41471 + i, skills[i], { 15, 1, channels[i], i == 3 ? 1024 : 0, 0, 0, false, { .pve = 553 } });
    b.state({ 0, "千里急次序", 0, 0, 4 }, false);
    b.state({ 0, "自动千里急", 1, 0, 1 }, false);
    b.state({0, "天羊盏", 80, 0, 1}, false);
    b.state({32145, "刻木", 240, 48, 3}, true, true, true);
    b.damage(43081, "夹线", {15, 1, 2600, 0, 0, 0, false, {.pve = 553}});
    b.secondary_damage(43082, "刻木(DOT)", {1, 0, 1600, 0, 5, 48, false, {.pve = 553}});
    b.dot("刻木", "刻木(DOT)");
    b.cast("使用橙武", 960, 0, 0, {.choice = Legendary}, {b.apply("天羊盏")});
    // 天羊盏期间夹线需要 20 心络，先扣 20 再回 10，
    // 随后命中并挂刻木；选绞网时，这笔支出触发两次寄线。
    b.cast("夹线", 0, 24, 0, {.resource = 0, .cost = 20, .required = b.buff("天羊盏"), .forbidden = puppet, .choice = Legendary},
           {b.gain(0, -20), b.gain(0, 10), b.hit("夹线"), b.apply("缠绞"), b.apply("刻木")});
    // 四式共用段序。鼓关在叁式后开放第四式手动射程，未选鼓关时在 prepare 中令叁式直接回到壹式。
    b.state({ 0, "鼓关待发", 0, 0, 1 }, false);
    b.post_damage("千里急·叁", { b.selected("鼓关", b.apply("鼓关待发")) });
    b.post_damage("千里急·肆", { b.remove("千里急次序", 4), b.remove("鼓关待发"), b.apply("伶影驻场") });
    for (auto name : skills) b.post_damage(name, { b.remove("连环慢"), b.selected("寄意", b.shorten("勾线", 16)) });
    const Step stage_hit{ .op = Op::StackStageHit, .slot = b.effect("千里急·壹"), .extra = b.buff("千里急次序") };
    b.cast("千里急", 0, 24, 0, { .required = puppet, .gcd_group = PuppetGcd, .range = 6, .cast_while_channeling = true,
                                 .range_aura = b.buff("鼓关待发"), .aura_range = 30, .range_origin = puppet_position },
           { record_gcd, b.apply("千里急次序"), stage_hit });

    // 联动与手动千里急、生地狱争用同一公调。勾线起手联动，其余本体招式先命中、上缠绞或钤束。
    const Step attempt{ .op = Op::School, .slot = b.buff("自动千里急"), .range = 6,
        .effect = [](EffectContext &f, const Step &s) {
            if (f.TryStartGcd(PuppetGcd, 24)) f.SetAura(s.slot, 1);
        }, .range_origin = puppet_position };
    const auto linked = [&](Step step) { return b.when("自动千里急", step); };
    const auto begin_link = { attempt, linked(record_gcd), linked(b.apply("千里急次序")) };
    const auto finish_link = { linked(stage_hit), b.remove("自动千里急") };
    b.pre_cast("勾线", begin_link);
    b.pre_cast("勾线", finish_link);
    for (std::string_view cast : { "挑丝", "绊线", "应天长", "夹线" }) {
        b.post_cast(cast, begin_link);
        // 先快照 DOT，再由千里急直伤消耗连环慢。联动未就绪时保留旧 DOT 和段序。
        if (cast == "应天长") b.post_cast(cast, { linked(b.apply("千里急DOT")) });
        b.post_cast(cast, finish_link);
    }
    // 钤束为角色面板增益，连寄线、虚影和 DOT 快照也读取。寡合只在未操控时提供无视防御。
    b.school_modifier({ b.buff("钤束"), 0, { .attack = 154, .strain_rating = 307 } });
    b.school_modifier({ INVALID_SLOT, 0, { .ignore = 614 }, "寡合" });
    b.school_modifier({ b.buff("傀儡"), 0, { .ignore = -614 }, "寡合" });
    // 缠绞只强化伶影的招式直伤。绊线、勾线负责叠层，谐调是独立附伤。
    for (auto name : { "千里急·壹", "千里急·贰", "千里急·叁", "千里急·肆", "生地狱" })
        for (int i = 1; i <= 5; ++i) b.pre_damage(name, { b.buff("缠绞"), i, { .fragile = (256 * i + 2) / 5 } });
    // 虚影是目标易伤，与迭影的招式增伤分开。黄粱将 205/1024 提升到 307/1024。
    for (auto name : { "挑丝", "绊线", "勾线", "夹线", "寄线", "千里急·壹", "千里急·贰", "千里急·叁", "千里急·肆", "生地狱" }) {
        b.pre_damage(name, { b.buff("虚影"), 0, { .fragile = 205 } });
        b.pre_damage(name, { b.buff("虚影"), 0, { .fragile = 102 }, "黄粱" });
    }
    const std::initializer_list<std::string_view> thousand = { "千里急·壹", "千里急·贰", "千里急·叁", "千里急·肆", "千里急(DOT)" };
    for (auto name : thousand) b.passive(name, "寄意", { .critical = 1500, .power = 157 });
    b.recipe("千里急", "千里急·2%会心", { .critical = 200 }, thousand);
    b.recipe("千里急", "千里急·3%会心", { .critical = 300 }, thousand);
    b.recipe("千里急", "千里急·3%系数", {}, thousand, 1.03);
    b.recipe("千里急", "千里急·4%系数", {}, thousand, 1.04);
    for (auto name : thousand) b.pre_damage(name, { .aura = b.buff("连环慢"), .channel_scale = 1.25 });
    for (auto name : skills) b.pre_damage(name, { .aura = b.buff("傀儡"), .channel_scale = 2.2 });
    b.set_bonus({ .damage = 102 }, { "绊线", "千里急·壹", "千里急·贰", "千里急·叁", "千里急·肆" });
    b.recipe("生地狱", "生地狱·3%伤害", { .damage = 31 }, { "生地狱" });
    b.recipe("生地狱", "生地狱·2%会心", { .critical = 200 }, { "生地狱" });
    b.recipe("生地狱", "生地狱·2%伤害", { .damage = 20 }, { "生地狱" });
    b.recipe("生地狱", "生地狱·3%会心", { .critical = 300 }, { "生地狱" });
    b.recipe("障幕虚影", "障幕虚影·3%伤害", { .damage = 31 }, { "障幕虚影" });
    b.recipe("障幕虚影", "障幕虚影·2%伤害", { .damage = 20 }, { "障幕虚影" });
    b.legendary_damage({ "勾线", "生地狱" });
    return b.finish();
}
} // namespace JX3DPS::runtime::season50
