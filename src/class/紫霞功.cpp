#include "common/builder.hpp"

namespace JX3DPS::runtime::season50 {
Book 紫霞功()
{
    Builder b("zxg", "紫霞功", R"(# 魔盒宏适配 https://www.jx3box.com/macro/111458
/cast 使用风腰坠
/cast 使用橙武
/cast [nobuff:破苍穹] 破苍穹
/cast [last_skill:五方行尽] 紫气东来
/cast 六合独尊
/cast [气点>=1&buff:破苍穹] 五方行尽
/cast 破势二段
/cast [buff:飞剑&bufftime:飞剑<15] 破势
/cast [bufftime:飞剑>24] 万世不竭二段
/cast [nobuff:飞剑] 万世不竭
/cast [气点>9] 两仪化形
/cast 四象轮回
)");
    auto &school = b.definition();
    school.info = { school.code, "纯阳", "根骨", "Neutral", 727, 41, 0 };
    school.display_order = 19;
    school.saved.macro_digest = "795a4a975e43d752f6d9f82f983c74bc2c7ca72393d1c4e7a5de11bce0c78456";
    school.saved.talents_v7 = { "正气", "归元", "同尘" };
    school.saved.added_recipes_v7 = { "两仪回豆" };
    school.precombat = true;
    school.initial_qidian_option = true;
    school.weapon_effect = "神兵附伤";
    b.preset(2301,
             { 5919, 1633, 73, 2286, 208, 1153, 4212, 0, 1092 },
             { "两仪化形·5%伤害", "两仪化形·4%伤害", "两仪化形·4%会心",
               "四象轮回·4%会心", "四象轮回·3%会心", "四象轮回·运功减4帧（二）", "四象轮回·运功减4帧（一）",
               "两仪回豆",
               "太极无极·20%伤害", "太极无极·15%伤害（一）", "太极无极·15%伤害（二）", "太极无极·4%会心",
               "万世不竭·5%伤害", "万世不竭·4%伤害", "万世不竭·3%伤害" });
    b.choices({ "正气", "归元", "同尘", "浮生", "无外", "心固", "固本", "破势", "抱阳" }, { "两仪回豆" }, { "神兵附伤" });
    b.default_talents({ "浮生", "同尘", "无外", "归元", "心固", "固本", "破势", "正气", "抱阳" });
    b.resource({ "气点", 10, 0, 10 });
    b.state({ 2757, "紫气东来", 160, 16, 1 }, false, false, false);
    b.damage(896,
             "四象轮回",
             { 18, 2, 528, 0, 0, 0, false, { .pve = 686 }, {} });
    b.secondary_damage(
        25770,
        "四象轮回·神兵",
        { 0, 2, 65, 0, 0, 0, false, { .pve = 686 }, {} });
    b.damage(3439,
             "两仪化形·一",
             { 27, 2, 51, 0, 0, 0, false, { .pve = 686 }, {} });
    b.damage(3440,
             "两仪化形·二",
             { 54, 5, 103, 0, 0, 0, false, { .pve = 686 }, {} });
    b.damage(3441,
             "两仪化形·三",
             { 81, 8, 154, 0, 0, 0, false, { .pve = 686 }, {} });
    b.damage(3442,
             "两仪化形·四",
             { 109,
               1,
               206,
               0,
               0,
               0,
               false,
               { .pve = 686 },
               {} });
    b.damage(3443,
             "两仪化形·五",
             { 136,
               1,
               258,
               0,
               0,
               0,
               false,
               { .pve = 686 },
               {} });
    b.damage(3444,
             "两仪化形·六",
             { 163,
               1,
               309,
               0,
               0,
               0,
               false,
               { .pve = 686 },
               {} });
    b.damage(3445,
             "两仪化形·七",
             { 190,
               1,
               361,
               0,
               0,
               0,
               false,
               { .pve = 686 },
               {} });
    b.damage(3446,
             "两仪化形·八",
             { 218,
               2,
               413,
               0,
               0,
               0,
               false,
               { .pve = 686 },
               {} });
    b.damage(3447,
             "两仪化形·九",
             { 245,
               2,
               464,
               0,
               0,
               0,
               false,
               { .pve = 686 },
               {} });
    b.damage(3448,
             "两仪化形·十",
             { 272,
               2,
               516,
               0,
               0,
               0,
               false,
               { .pve = 686 },
               {} });
    b.pulse("紫气东来", { b.gain(0, 2) });
    b.state({ 0, "破苍穹", 384, 0, 3 }, false);
    b.cast("紫气东来", 912, 8, 0, { .lock = 8, .charges = 2, .gcd_group = 1 }, { b.gain(0, 10), b.apply("紫气东来", 1) });
    b.cast("四象轮回", 0, 24, 32, { .capture = b.buff("破苍穹") }, { b.hit("四象轮回", 1), b.gain(0, 2), b.selected("神兵附伤", b.proc("四象轮回·神兵", 307)) });
    b.cast("两仪化形", 0, 24, 0, { .resource = 0, .cost = 1, .capture = b.buff("破苍穹") }, { b.tier("两仪化形·一", 0), b.gain(0, -10), b.selected("两仪回豆", b.gain(0, 2)) });
    b.recipe_group("两仪回豆", "两仪化形");
    b.recipe("两仪化形",
             "两仪化形·2%会心",
             { .critical = 200 },
             { "两仪化形·一", "两仪化形·七", "两仪化形·三", "两仪化形·九", "两仪化形·二", "两仪化形·五", "两仪化形·八", "两仪化形·六", "两仪化形·十", "两仪化形·四" });
    b.recipe("两仪化形",
             "两仪化形·3%会心",
             { .critical = 300 },
             { "两仪化形·一", "两仪化形·七", "两仪化形·三", "两仪化形·九", "两仪化形·二", "两仪化形·五", "两仪化形·八", "两仪化形·六", "两仪化形·十", "两仪化形·四" });
    b.recipe("两仪化形",
             "两仪化形·3%伤害",
             { .damage = 31 },
             { "两仪化形·一", "两仪化形·七", "两仪化形·三", "两仪化形·九", "两仪化形·二", "两仪化形·五", "两仪化形·八", "两仪化形·六", "两仪化形·十", "两仪化形·四" });
    b.recipe("两仪化形",
             "两仪化形·4%会心",
             { .critical = 400 },
             { "两仪化形·一", "两仪化形·七", "两仪化形·三", "两仪化形·九", "两仪化形·二", "两仪化形·五", "两仪化形·八", "两仪化形·六", "两仪化形·十", "两仪化形·四" });
    b.recipe("两仪化形",
             "两仪化形·4%伤害",
             { .damage = 41 },
             { "两仪化形·一", "两仪化形·七", "两仪化形·三", "两仪化形·九", "两仪化形·二", "两仪化形·五", "两仪化形·八", "两仪化形·六", "两仪化形·十", "两仪化形·四" });
    b.recipe("两仪化形",
             "两仪化形·5%伤害",
             { .damage = 51 },
             { "两仪化形·一", "两仪化形·七", "两仪化形·三", "两仪化形·九", "两仪化形·二", "两仪化形·五", "两仪化形·八", "两仪化形·六", "两仪化形·十", "两仪化形·四" });
    b.recipe("四象轮回", "四象轮回·2%会心", { .critical = 200 }, { "四象轮回" });
    b.recipe("四象轮回", "四象轮回·3%会心", { .critical = 300 }, { "四象轮回" });
    b.recipe("四象轮回", "四象轮回·4%会心", { .critical = 400 }, { "四象轮回" });
    // 普通六合需维持引导，每秒一段；浮生在运功完成后留下剑域，此后移动或施放其他技能不撤销已排定的命中。
    b.damage(15209, "六合独尊", {146, 0, 246, 0, 0, 0, false, {.pve = 686}});
    b.passive("六合独尊", "浮生", {.channel_override = 506});
    b.cast("六合独尊", 0, 24, 0, {.pulses = 5, .spacing = 16, .range = 15}, {b.hit("六合独尊")});
    b.variant("六合独尊", "浮生", 320, 24, {.range = 15, .cooldown_on_finish = true, .precombat_cast = true},
              {b.hit("六合独尊"), b.later("六合独尊", 10), b.later("六合独尊", 20), b.later("六合独尊", 30), b.later("六合独尊", 40)});
    b.state({ 0, "颠越苍穹", 96, 16, 1 }, true);
    b.state({ 0, "颠越余次", 96, 0, 5 }, false);
    b.damage(36438, "颠越苍穹击", { 103, 10, 131, 0, 0, 0, false, { .pve = 686 + 4300 }, {} });
    b.pulse("颠越苍穹", { b.hit("颠越苍穹击") });
    constexpr std::array<std::string_view, 10> tiers{
        "五方行尽·一", "五方行尽·二", "五方行尽·三", "五方行尽·四", "五方行尽·五",
        "五方行尽·六", "五方行尽·七", "五方行尽·八", "五方行尽·九", "五方行尽·十"
    };
    constexpr int bases[]{ 5, 1, 1, 2, 2, 3, 3, 4, 4, 5 }, channels[]{ 11, 22, 33, 44, 56, 67, 78, 89, 100, 112 };
    for (int i = 0; i < 10; ++i) {
        b.damage(i < 5 ? 327 + i : 461 + i - 5, tiers[i], { bases[i], 0, channels[i], 0, 0, 0, false, { .pve = 686 }, {} });
    }
    b.cast("破苍穹", 0, 24, 16, { .choice = "归元" }, { b.apply("破苍穹") });
    b.cast("五方行尽", 320, 24, 0, { .resource = 0, .cost = 1 }, { b.tier(tiers.front(), 0), b.gain(0, -10) });
    b.variant("五方行尽",
              "正气",
              320,
              0,
              { .resource = 0, .cost = 1 },
              { b.tier(tiers.front(), 0),
                b.gain(0, -10),
                b.when("破苍穹", b.apply("颠越苍穹")),
                b.when("破苍穹", b.apply("颠越余次", 5)),
                b.remove("破苍穹"),
                b.when("颠越余次", b.hit("颠越苍穹击")),
                b.remove("颠越余次") });
    b.post_damage("六合独尊", { b.when("颠越余次", b.hit("颠越苍穹击")), b.remove("颠越余次") });
    // 同尘在六合独尊每段命中后叠一层，下一段才读到新层数。
    // 每层提高 512/1024 伤害，收招清层后重新挂一层。
    b.state({ 0, "同尘", 48, 0, 4 }, false);
    b.post_cast("六合独尊", { b.remove("同尘", 4), b.selected("同尘", b.apply("同尘")) });
    for (int i = 1; i <= 4; ++i) {
        b.pre_damage("六合独尊", { b.buff("同尘"), i, { .damage = i * 512 }, "同尘" });
    }
    b.post_damage("六合独尊", { b.selected("同尘", b.apply("同尘")) });
    b.state({ 0, "破势二段", 160, 0, 3 }, false);
    b.state({ 0, "飞剑", 424, 48, 1 }, false, false, true);
    constexpr std::string_view swords[]{ "飞剑·一", "飞剑·二", "飞剑·三", "飞剑·四", "飞剑·五" };
    for (int i = 0; i < 5; ++i) {
        b.damage(18649 + i, swords[i], { 5, 0, 71, 0, 0, 0, false, { .pve = 686 } });
        b.passive(swords[i], "破势", { .pve = 1331 });
    }
    // 飞剑每轮连发五击，第一击当帧命中，余下四击分别延迟 3、8、12、16 帧。
    b.pulse("飞剑", { b.hit(swords[0]), b.later(swords[1], 3), b.later(swords[2], 8), b.later(swords[3], 12), b.later(swords[4], 16) });
    b.cast("卸除飞剑", 1, 0, 0, {}, { b.remove("飞剑") });
    b.cast("万世不竭", 240, 24, 20, { .forbidden = b.buff("飞剑"), .also_forbidden = b.buff("破势二段") }, { b.apply("飞剑") });
    b.post_cast("紫气东来", { b.selected("固本", { .op = Op::ExtendAura, .slot = b.buff("飞剑"), .amount = 800, .condition = b.buff("破势二段"), .absent = true, .duration = 400 }) });
    b.damage(22014, "万世不竭·一", { 60, 1, 656, 0, 0, 0, false, { .pve = 686 } });
    b.passive("万世不竭·一", "破势", { .pve = 1331 });
    Hit remaining{ 9, 0, 612, 0, 0, 0, false, { .pve = 686 } };
    // 无外的攻击系数按飞剑剩余秒数分档。二段先按剩余时长打无外，
    // 再扣 240 帧飞剑时间，所以本次用的是扣除前的档位。
    for (int sec = 1; sec <= 50; ++sec) remaining.rank_channels.push_back(static_cast<int>(534 + 12 * sec * .89));
    b.damage(46142, "无外", std::move(remaining));
    Step time_hit{ Op::RemainingTimeHit, b.effect("无外"), 1, INVALID_SLOT, false, "无外", b.buff("飞剑") };
    b.cast("万世不竭二段", 0, 24, 0, { .required = b.buff("飞剑"), .minimum_aura_time = 240 }, { b.hit("万世不竭·一"), time_hit, b.remove("飞剑") });
    const Step spend{ .op = Op::ExtendAura, .slot = b.buff("飞剑"), .duration = -240 };
    b.variant("万世不竭二段", "无外", 0, 0, { .required = b.buff("飞剑"), .minimum_aura_time = 240 }, { b.hit("万世不竭·一"), time_hit, spend });
    for (auto cast : { "四象轮回", "两仪化形" }) {
        Step extend{ .op = Op::ExtendAura, .slot = b.buff("飞剑"), .amount = 800, .condition = b.buff("飞剑"), .choice = "抱阳", .duration = 96 };
        b.post_cast(cast, { b.captured(extend) });
    }
    b.post_cast("四象轮回", { b.selected("心固", b.gain(0, 3)) });
    b.set_bonus({ .damage = 102 }, { "飞剑·一", "飞剑·二", "飞剑·三", "飞剑·四", "飞剑·五", "万世不竭·一" });
    b.damage(46296, "破势", { 10, 1, 762, 0, 0, 0, false, { .pve = 2734 } });
    b.cast("破势", 960, 0, 0, { .required = b.buff("飞剑"), .choice = "破势" }, { b.remove("飞剑"), b.apply("破势二段", 3) });
    b.cast("破势二段", 0, 24, 0, { .required = b.buff("破势二段") }, { b.remove("破势二段"), b.hit("破势") });
    b.damage(2299, "太极无极", { 53, 46, 88, 0, 0, 0, false, { .pve = 686 } });
    b.cast("太极无极", 0, 24, 0, {}, { b.hit("太极无极") });
    b.state({ 0, "会神", 160, 0, 1 }, false);
    // 五方行尽在起手按气点决定会神持续时长：每两点加一秒，单数向上取整，再加基础五秒。
    for (int points = 1; points <= 10; ++points) b.pre_cast("五方行尽", { b.at(0, points, points, b.apply_for("会神", (5 + (points + 1) / 2) * 16)) });
    b.school_modifier({ b.buff("会神"), 0, { .uncapped_power = 328 } });
    b.school_modifier({ b.buff("破苍穹"), 0, { .critical = 500, .power = 102 } });
    b.school_modifier({ b.buff("破苍穹"), 0, { .critical = 500, .power = 102 }, "抱阳" });
    b.school_modifier({ b.buff("破苍穹"), 0, { .ignore = 615 }, "归元" });
    b.school_modifier({ b.buff("紫气东来"), 0, { .attack = 256, .critical = 2500, .uncapped_power = 256 } });
    b.school_modifier({ b.buff("紫气东来"), 0, { .strain_rating = 307 }, "固本" });
    b.adjust([](Prepared &p) {
        if (p.config.precombat) {
            const auto &buffs = p.config.book->buffs;
            auto aura = [&](std::string_view n) { return static_cast<Slot>(std::ranges::find(buffs, n, &BuffDescriptor::name) - buffs.begin()); };
            p.opening = { { .op = Op::Add, .slot = aura("破苍穹"), .amount = 2, .duration = 320 }, { .op = Op::Add, .slot = aura("飞剑"), .duration = 384 } };
        }
        auto skill = [&](std::string_view name) -> auto & { return *std::ranges::find(p.skills, name, &SkillDescriptor::name); };
        if (std::ranges::contains(p.config.talents, "心固")) skill("四象轮回").cast_time -= 4;
        if (!std::ranges::contains(p.config.talents, "固本")) skill("紫气东来").cooldown = 1200;
    });
    // 四象轮回的这本秘籍缩短 4 帧运功。它在施放阶段生效，直接改变命中到达的帧。
    b.timing_recipe("四象轮回", "四象轮回·运功减4帧（二）", "四象轮回", 0, -4);
    b.timing_recipe("四象轮回", "四象轮回·运功减4帧（一）", "四象轮回", 0, -4);
    b.recipe("太极无极", "太极无极·20%伤害", { .damage = 205 }, { "太极无极" });
    b.recipe("太极无极", "太极无极·15%伤害（一）", { .damage = 154 }, { "太极无极" });
    b.recipe("太极无极", "太极无极·15%伤害（二）", { .damage = 154 }, { "太极无极" });
    b.recipe("太极无极", "太极无极·4%会心", { .critical = 400 }, { "太极无极" });
    b.recipe("万世不竭", "万世不竭·5%伤害", { .damage = 51 }, { "万世不竭·一", "飞剑·一", "飞剑·二", "飞剑·三", "飞剑·四", "飞剑·五" });
    b.recipe("万世不竭", "万世不竭·4%伤害", { .damage = 41 }, { "万世不竭·一", "飞剑·一", "飞剑·二", "飞剑·三", "飞剑·四", "飞剑·五" });
    b.recipe("万世不竭", "万世不竭·3%伤害", { .damage = 31 }, { "万世不竭·一", "飞剑·一", "飞剑·二", "飞剑·三", "飞剑·四", "飞剑·五" });
    b.state({1916, "赤瞳照业", 160, 0, 3}, false);
    b.cast("使用橙武", 800, 0, 0, {.choice = Legendary}, {b.apply("飞剑"), b.apply("赤瞳照业", 3)});
    b.pre_damage("万世不竭·一", {b.buff("赤瞳照业"), 0, {.damage = 512}});
    b.post_cast("万世不竭二段", {b.when("赤瞳照业", b.apply("飞剑")), b.remove("赤瞳照业")});
    b.legendary_damage({ "四象轮回", "太极无极" });
    return b.finish();
}
} // namespace JX3DPS::runtime::season50
