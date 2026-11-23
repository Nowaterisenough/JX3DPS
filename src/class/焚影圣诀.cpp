#include "common/builder.hpp"

namespace JX3DPS::runtime::season50 {
Book 焚影圣诀()
{
    Builder b("fysj", "焚影圣诀", R"(# 魔盒宏适配 https://www.jx3box.com/macro/110739
/cast 使用风腰坠
/cast 使用橙武
/cast [buff:日月齐光] 光明相
/cast 生死劫·日
/cast 生死劫·月
/cast 净世破魔击·日
/cast 净世破魔击·月
/cast [buff:月芒|buff:日珥>3] 银月斩
/cast [nobuff:月芒] 烈日斩
/cast 驱夜断愁
/cast [buff:日珥] 赤日轮
/cast 幽月轮
)");
    auto &school = b.definition();
    school.info = { school.code, "明教", "元气", "Solar", 635, 133, 0 };
    school.display_order = 7;
    school.saved.macro_digest = "95f0522c3a722d81a671bfd3c4fb9368de4d0b5bed256bb9638c1e09266be0ec";
    school.saved.talents_v7 = { "崇光斩恶", "日月同辉", "明焰续夜", "夷数神光" };
    school.raid.penetration_perfume = true;
    school.raid.gear_score = 17785;
    b.preset(2363,
             { 7233, 1465, 113, 1582, 0, 2326, 4417, 0, 52 },
             { "净世破魔击·月破回灵", "净世破魔击·5%伤害", "净世破魔击·4%伤害", "净世破魔击·5%会心",
               "烈日斩·4%伤害", "烈日斩·5%伤害", "烈日斩·4%会心", "烈日斩·10%静止",
               "银月斩·5%会心", "银月斩·3%会心", "银月斩·4%会心",
               "赤日轮·4%伤害", "赤日轮·5%伤害", "赤日轮·3%伤害", "赤日轮·4%会心",
               "幽月轮·4%伤害", "幽月轮·3%伤害", "幽月轮·5%会心", "幽月轮·4%会心",
               "生死劫·3%伤害", "生死劫·4%伤害", "生死劫·5%伤害",
               "光明相·调息减160帧（一）", "光明相·调息减160帧（二）", "光明相·调息减160帧（三）",
               "驱夜断愁·4%伤害", "驱夜断愁·5%伤害", "驱夜断愁·5%会心", "驱夜断愁·4%会心" });
    b.choices({ "崇光斩恶", "日月同辉", "明焰续夜", "夷数神光", "无明业火", "诛邪镇魔", "靡业报劫", "双曜转辉", "日月齐光", "日月耀临", "净身明礼" }, {}, {});
    b.default_talents({ "日月同辉", "崇光斩恶", "诛邪镇魔", "靡业报劫", "双曜转辉", "日月齐光", "夷数神光", "日月耀临", "明焰续夜" });
    b.resource({ "日灵", 0, 0, 100 });
    b.resource({ "月灵", 0, 0, 100 });
    b.state({ 0, "烈日", 288, 0, 1 }, true, false, false);
    b.state({ 0, "银月斩DOT", 288, 32, 1 }, true, true, true);
    b.damage(0, "银月斩(DOT)", { 1, 0, 344, 0, 9, 32, false, { .pve = 317 }, {} });
    b.damage(19055,
             "武器伤害·1",
             {
                 0,
                 0,
                 16,
                 1024,
                 0,
                 0,
                 true,
                 { .attack = -1024, .pve = 317 },
                 {}
    });
    b.damage(19055,
             "武器伤害·2",
             {
                 0,
                 0,
                 16,
                 1024,
                 0,
                 0,
                 true,
                 { .attack = -1024, .pve = 317 },
                 {}
    });
    b.damage(19055,
             "武器伤害·5",
             {
                 0,
                 0,
                 16,
                 2048,
                 0,
                 0,
                 true,
                 { .attack = -1024, .pve = 317 },
                 {}
    });
    b.damage(19055,
             "武器伤害·6",
             {
                 0,
                 0,
                 16,
                 1024,
                 0,
                 0,
                 true,
                 { .attack = -1024, .pve = 317 },
                 {}
    });
    b.damage(19055,
             "武器伤害·7",
             {
                 0,
                 0,
                 16,
                 1024,
                 0,
                 0,
                 true,
                 { .attack = -1024, .pve = 317 },
                 {}
    });
    b.damage(19055,
             "武器伤害·8",
             {
                 0,
                 0,
                 16,
                 1024,
                 0,
                 0,
                 true,
                 { .attack = -1024, .pve = 317 },
                 {}
    });
    b.damage(4028, "赤日轮", { 13, 2, 510, 0, 0, 0, false, { .pve = 317 }, {} });
    b.damage(4029, "赤日轮二段", { 14, 2, 510, 0, 0, 0, false, { .pve = 317 }, {} });
    b.damage(4030, "赤日轮三段", { 15, 2, 636, 0, 0, 0, false, { .pve = 317 }, {} });
    b.damage(4024, "幽月轮", { 7, 7, 392, 0, 0, 0, false, { .pve = 317 }, {} });
    b.damage(4025,
             "幽月轮二段",
             {
                 7,
                 7,
                 392,
                 0,
                 0,
                 0,
                 false,
                 { .critical = 500, .pve = 317 },
                 {}
    });
    b.damage(4026,
             "幽月轮三段",
             {
                 7,
                 7,
                 448,
                 0,
                 0,
                 0,
                 false,
                 { .critical = 1000, .pve = 317 },
                 {}
    });
    b.damage(40088, "烈日斩", { 15, 1, 572, 0, 0, 0, false, { .pve = 317 }, {} });
    b.damage(40089, "银月斩", { 9, 1, 287, 0, 0, 0, false, { .pve = 317 }, {} });
    b.damage(14701, "净世破魔击·日", { 9, 1, 808, 0, 0, 0, false, { .pve = 317 }, {} });
    b.damage(4476, "净世破魔击·月", { 7, 2, 269, 0, 0, 0, false, { .pve = 317 }, {} });
    b.dot("银月斩DOT", "银月斩(DOT)");
    b.cast("赤日轮", 0, 16, 0, { .chain_window = 32 }, { b.cycle("赤日轮", 3), b.hit("武器伤害·1", 1), b.gain(0, 20) });
    b.cast("幽月轮", 0, 16, 0, { .chain_window = 32 }, { b.cycle("幽月轮", 3), b.hit("武器伤害·7", 1), b.gain(1, 20) });
    b.cast("烈日斩", 96, 16, 0, {}, { b.hit("烈日斩", 1), b.hit("武器伤害·2", 1), b.apply("烈日", 1), b.gain(0, 40) });
    b.cast("银月斩", 96, 16, 0, {}, { b.hit("银月斩", 1), b.hit("武器伤害·8", 1), b.apply("银月斩DOT", 1), b.gain(1, 40) });
    b.cast("净世破魔击·日", 0, 16, 0, { .resource = 0, .cost = 100 }, { b.hit("净世破魔击·日", 1), b.hit("武器伤害·5", 1), b.gain(0, -100) });
    b.cast("净世破魔击·月",
           0,
           16,
           0,
           { .resource = 1, .cost = 100 },
           { b.hit("净世破魔击·月", 1),
             b.hit("武器伤害·6", 1),
             b.later("净世破魔击·月", 1),
             b.later("武器伤害·6", 1),
             b.later("净世破魔击·月", 2),
             b.later("武器伤害·6", 2),
             b.gain(1, -100) });
    // 净世破魔击的双会与伤害秘籍同时覆盖日、月两种命中；月破回灵秘籍只在月破收招时另回 20 月灵。
    b.recipe("净世破魔击", "净世破魔击·3%会心", { .critical = 300 }, { "净世破魔击·日", "净世破魔击·月" });
    b.recipe("净世破魔击", "净世破魔击·3%伤害", { .damage = 31 }, { "净世破魔击·日", "净世破魔击·月" });
    b.recipe("净世破魔击", "净世破魔击·4%会心", { .critical = 400 }, { "净世破魔击·日", "净世破魔击·月" });
    b.recipe("净世破魔击", "净世破魔击·4%伤害", { .damage = 41 }, { "净世破魔击·日", "净世破魔击·月" });
    b.recipe("净世破魔击", "净世破魔击·5%会心", { .critical = 500 }, { "净世破魔击·日", "净世破魔击·月" });
    b.recipe("净世破魔击", "净世破魔击·5%伤害", { .damage = 51 }, { "净世破魔击·日", "净世破魔击·月" });
    b.recipe("幽月轮", "幽月轮·3%会心", { .critical = 300 }, { "幽月轮", "幽月轮三段", "幽月轮二段" });
    b.recipe("幽月轮", "幽月轮·3%伤害", { .damage = 31 }, { "幽月轮", "幽月轮三段", "幽月轮二段" });
    b.recipe("幽月轮", "幽月轮·4%会心", { .critical = 400 }, { "幽月轮", "幽月轮三段", "幽月轮二段" });
    b.recipe("幽月轮", "幽月轮·4%伤害", { .damage = 41 }, { "幽月轮", "幽月轮三段", "幽月轮二段" });
    b.recipe("幽月轮", "幽月轮·5%会心", { .critical = 500 }, { "幽月轮", "幽月轮三段", "幽月轮二段" });
    b.recipe("烈日斩", "烈日斩·3%会心", { .critical = 300 }, { "烈日斩" });
    b.recipe("烈日斩", "烈日斩·3%伤害", { .damage = 31 }, { "烈日斩" });
    b.recipe("烈日斩", "烈日斩·4%会心", { .critical = 400 }, { "烈日斩" });
    b.recipe("烈日斩", "烈日斩·4%伤害", { .damage = 41 }, { "烈日斩" });
    b.recipe("烈日斩", "烈日斩·5%伤害", { .damage = 51 }, { "烈日斩" });
    b.recipe("赤日轮", "赤日轮·3%会心", { .critical = 300 }, { "赤日轮", "赤日轮三段", "赤日轮二段" });
    b.recipe("赤日轮", "赤日轮·3%伤害", { .damage = 31 }, { "赤日轮", "赤日轮三段", "赤日轮二段" });
    b.recipe("赤日轮", "赤日轮·4%会心", { .critical = 400 }, { "赤日轮", "赤日轮三段", "赤日轮二段" });
    b.recipe("赤日轮", "赤日轮·4%伤害", { .damage = 41 }, { "赤日轮", "赤日轮三段", "赤日轮二段" });
    b.recipe("赤日轮", "赤日轮·5%伤害", { .damage = 51 }, { "赤日轮", "赤日轮三段", "赤日轮二段" });
    b.recipe("银月斩", "银月斩·3%会心", { .critical = 300 }, { "银月斩", "银月斩(DOT)" });
    b.recipe("银月斩", "银月斩·4%会心", { .critical = 400 }, { "银月斩", "银月斩(DOT)" });
    b.recipe("银月斩", "银月斩·5%会心", { .critical = 500 }, { "银月斩", "银月斩(DOT)" });
    b.set_bonus({ .damage = 102 }, { "净世破魔击·月", "净世破魔击·日" });
    b.state({ 0, "光明相", 1920, 0, 250 }, false);
    b.state({ 0, "日珥", 1920, 0, 18 }, false);
    b.state({ 0, "月芒", 1920, 0, 18 }, false);
    b.damage(4480, "驱夜断愁", { 95, 9, 814, 0, 0, 0, false, { .pve = 317 }, {} });
    b.damage(37336, "崇光斩恶", { 20, 24, 3285, 0, 0, 0, false, { .pve = 317 }, {} });
    b.damage(0, "崇光斩恶·月破魔", { 20, 24, 3285, 0, 0, 0, false, { .pve = 317 }, {} });
    b.cast("光明相", 1520, 0, 0, {}, { b.apply("光明相"), b.selected("夷数神光", b.apply("光明相", 2)) });
    b.cast("驱夜断愁", 400, 16, 0, { .choice = "明焰续夜" }, { b.hit("驱夜断愁"), b.when("日珥", b.gain(0, 40)), b.when("月芒", b.gain(1, 40)) });
    // 日珥与月芒按日月招式对称处理。选日月同辉时，日斩读月芒、月斩读日珥，把已有的对侧层数转为光明相。
    for (bool sun : { false, true }) {
        auto same = sun ? "日珥" : "月芒", opposite = sun ? "月芒" : "日珥", slash = sun ? "烈日斩" : "银月斩",
             spend = sun ? "净世破魔击·日" : "净世破魔击·月";
        b.post_cast(slash,
                     { b.selected("日月同辉", b.when(opposite, b.apply("光明相"))),
                       b.selected("日月同辉", b.convert(opposite, "光明相")),
                       b.selected("日月同辉", b.when("光明相", b.apply(same), true)) });
        // 光明相期间施放净世破魔击，收招回满对侧日月灵，再结算崇光斩恶的对应追击，最后移除光明相。
        b.post_cast(spend,
                     { b.selected("明焰续夜", b.at_least(same, 2, b.reset("驱夜断愁"))),
                       b.when("光明相", b.gain(sun ? 1 : 0, 100)),
                       b.selected("崇光斩恶", b.when("光明相", b.hit(sun ? "崇光斩恶" : "崇光斩恶·月破魔"))),
                       b.remove("光明相") });
    }
    b.recipe("净世破魔击", "净世破魔击·月破回灵", {}, {});
    b.post_cast("净世破魔击·月", { b.selected("净世破魔击·月破回灵", b.gain(1, 20)) });
    for (bool sun : { true, false }) {
        auto skill = sun ? "生死劫·日" : "生死劫·月";
        auto dot = sun ? "靡业报劫·日(DOT)" : "靡业报劫·月(DOT)";
        auto same = sun ? "日珥" : "月芒", opposite = sun ? "月芒" : "日珥";
        b.damage(sun ? 6257 : 6258, skill, { 7, 1, 67, 0, 0, 0, false, { .pve = 317 } });
        auto aura = sun ? "靡业报劫·日" : "靡业报劫·月";
        b.state({ 0, aura, 320, 32, 1 }, true);
        b.damage(0, dot, { 1, 0, 3317, 0, 10, 32, false, { .pve = 317 } });
        b.dot(aura, dot);
        b.cast(skill, 240, 16, 0, { .resource = static_cast<Slot>(sun ? 0 : 1), .cost = 100, .choice = "双曜转辉", .charges = 2 },
               { b.hit(skill), b.gain(sun ? 0 : 1, -100), b.gain(sun ? 1 : 0, 100), b.selected("明焰续夜", b.at_least(same, 2, b.reset("驱夜断愁"))), b.selected("日月同辉", b.when(opposite, b.apply("光明相"))),
                 b.selected("日月同辉", b.convert(opposite, "光明相")), b.selected("日月同辉", b.when("光明相", b.apply(same), true)),
                 b.convert(same, opposite), b.selected("崇光斩恶", b.when("光明相", b.hit("崇光斩恶"))), b.remove("光明相"), b.selected("靡业报劫", b.apply(aura)) });
    }
    b.state({0, "圣火渡", 160, 0, 4}, false);
    b.state({ 0, "诛邪日印", 40, 0, 1 }, true);
    // 日、月生死劫共用两层充能。圣火渡窗口外，按一侧也推进另一侧的充能调息，让宏看到相同的还层进度。
    b.pre_cast("生死劫·日", { b.when("圣火渡", { Op::SpendCharge, b.effect("生死劫·月") }, true) });
    b.pre_cast("生死劫·月", { b.when("圣火渡", { Op::SpendCharge, b.effect("生死劫·日") }, true) });
    b.state({ 0, "诛邪月印", 40, 0, 1 }, true);
    b.damage(0, "诛邪镇魔·日", { 20, 24, 648, 0, 0, 0, false, { .pve = 317 } });
    b.damage(0, "诛邪镇魔·月", { 20, 24, 648, 0, 0, 0, false, { .pve = 317 } });
    // 首次日或月破魔／生死劫留下同侧诛邪印；改用对侧命中时，原印触发诛邪镇魔的日、月两击并消失。
    for (bool sun : { true, false }) {
        const auto own = sun ? "诛邪日印" : "诛邪月印", other = sun ? "诛邪月印" : "诛邪日印";
        for (auto n : { sun ? "净世破魔击·日" : "净世破魔击·月", sun ? "生死劫·日" : "生死劫·月" }) {
            b.post_cast(n, { b.selected("诛邪镇魔", b.when(other, b.apply(own), true)), b.when(other, b.hit("诛邪镇魔·日")),
                              b.when(other, b.hit("诛邪镇魔·月")), b.remove(other) });
        }
    }
    b.school_modifier({ INVALID_SLOT, 0, { .ignore = 307, .strain_rating = 205 }, "日月耀临" });
    // 默认木桩按背后攻击计算，无明业火给这些伤害加 12 个百分点会心和 123/1024 会效。
    b.school_modifier({ INVALID_SLOT, 0, { .critical = 1200, .power = 123 }, "无明业火" });
    b.state({ 0, "日月齐光", 1920, 0, 1 }, false);
    for (bool sun : { true, false }) {
        const auto strike = sun ? "烈日斩" : "银月斩";
        b.post_cast(strike, { b.selected("日月齐光", b.at_least("光明相", 5, b.apply("日月齐光"))),
                               b.when("日月齐光", b.at(sun ? 0 : 1, 1, 99, b.gain(sun ? 0 : 1, 100))) });
        for (auto spend : { sun ? "净世破魔击·日" : "净世破魔击·月", sun ? "生死劫·日" : "生死劫·月" }) {
            b.post_cast(spend, { b.when("光明相", b.remove("日月齐光"), true) });
        }
    }
    b.school_modifier({ b.buff("日月齐光"), 0, { .attack = 256 }, "日月齐光" });
    for (int stacks = 1; stacks <= 18; ++stacks) {
        const Stats attack{ .attack = 51 * std::min(stacks, 5) };
        for (auto hit : { "烈日斩", "赤日轮", "赤日轮二段", "赤日轮三段", "生死劫·日", "净世破魔击·日", "靡业报劫·日(DOT)", "诛邪镇魔·日", "崇光斩恶", "崇光斩恶·月破魔" })
            b.pre_damage(hit, { b.buff("日珥"), stacks, attack, "日月齐光" });
        for (auto hit : { "银月斩", "银月斩(DOT)", "幽月轮", "幽月轮二段", "幽月轮三段", "生死劫·月", "净世破魔击·月", "靡业报劫·月(DOT)", "诛邪镇魔·月", "驱夜断愁" })
            b.pre_damage(hit, { b.buff("月芒"), stacks, attack, "日月齐光" });
    }
    for (auto n : { "烈日斩", "银月斩" }) b.charge_variant(n, "日月同辉", 240, 3);
    b.adjust([](Prepared &p) {
        if (std::ranges::contains(p.config.talents, "夷数神光")) {
            auto &skill = *std::ranges::find(p.skills, "光明相", &SkillDescriptor::name);
            skill.cooldown += 400;
        }
    });
    b.recipe("烈日斩", "烈日斩·10%静止", { .damage = 102 }, { "烈日斩" });
    b.recipe("生死劫", "生死劫·3%伤害", { .damage = 31 }, { "生死劫·日", "生死劫·月", "靡业报劫·日(DOT)", "靡业报劫·月(DOT)" });
    b.recipe("生死劫", "生死劫·4%伤害", { .damage = 41 }, { "生死劫·日", "生死劫·月", "靡业报劫·日(DOT)", "靡业报劫·月(DOT)" });
    b.recipe("生死劫", "生死劫·5%伤害", { .damage = 51 }, { "生死劫·日", "生死劫·月", "靡业报劫·日(DOT)", "靡业报劫·月(DOT)" });
    b.timing_recipe("光明相", "光明相·调息减160帧（一）", "光明相", -160, 0);
    b.timing_recipe("光明相", "光明相·调息减160帧（二）", "光明相", -160, 0);
    b.timing_recipe("光明相", "光明相·调息减160帧（三）", "光明相", -160, 0);
    b.recipe("驱夜断愁", "驱夜断愁·4%伤害", { .damage = 41 }, { "驱夜断愁" });
    b.recipe("驱夜断愁", "驱夜断愁·5%伤害", { .damage = 51 }, { "驱夜断愁" });
    b.recipe("驱夜断愁", "驱夜断愁·5%会心", { .critical = 500 }, { "驱夜断愁" });
    b.recipe("驱夜断愁", "驱夜断愁·4%会心", { .critical = 400 }, { "驱夜断愁" });
    b.cast("使用橙武", 800, 0, 0, {.choice = Legendary}, {b.apply("圣火渡", 4)});
    for (auto name : {"净世破魔击·日", "生死劫·日"}) b.post_cast(name, {b.when("圣火渡", b.gain(1, 100))});
    for (auto name : {"净世破魔击·月", "生死劫·月"}) b.post_cast(name, {b.when("圣火渡", b.gain(0, 100))});
    for (auto name : {"赤日轮", "幽月轮", "烈日斩", "银月斩", "净世破魔击·日", "净世破魔击·月", "生死劫·日", "生死劫·月"}) {
        b.no_cooldown_during(name, "圣火渡");
        b.post_cast(name, {b.remove("圣火渡")});
    }
    b.legendary_damage({ "驱夜断愁", "净世破魔击·日", "净世破魔击·月" });
    // 净身明礼按实际日／月斩触发各自的范围伤害，在单目标上各命中一次。
    for (auto [sun, slash, hit] : {std::tuple{true, "烈日斩", "净身明礼·日"}, {false, "银月斩", "净身明礼·月"}}) {
        b.secondary_damage(sun ? 40931 : 40932, hit, {3, 0, sun ? 179 : 101, 0, 0, 0, false, b.definition().hits[b.effect(slash)].passive});
        b.passive(slash, "净身明礼", {.damage = 256});
        b.post_damage(slash, {b.selected("净身明礼", b.hit(hit))});
    }
    return b.finish();
}
} // namespace JX3DPS::runtime::season50
