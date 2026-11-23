#include "common/builder.hpp"

namespace JX3DPS::runtime::season50 {

// 阳春白雪与高山流水都已转好，才能同时让两曲进入调息并获得超然。
static void 双曲超然(EffectContext &frame, const Step &step)
{
    const Slot first = step.extra, second = step.neutral_buff;
    if (frame.state.skill_ready_at[first] <= frame.state.now && frame.state.skill_ready_at[second] <= frame.state.now) {
        frame.StartCooldown(first);
        frame.StartCooldown(second);
        frame.SetAura(step.slot, 1);
    }
}


Book 莫问()
{
    Builder b("mw", "莫问", R"(# 魔盒宏适配 https://www.jx3box.com/macro/110213
/cast 使用风腰坠
/cast 使用橙武
/fcast [last_skill:宫] 羽
/fcast [last_skill:宫] 角
/cast [skill_energy:羽<1&skill_notin_cd:角] 孤影化双
/cast [buff:超然|skill_notin_cd:孤影化双] 羽
/cast [tnobuff:阳春白雪&nobuff:超然&skill_notin_cd:徵] 阳春白雪
/cast [tbuff:阳春白雪|buff:超然] 徵
/cast [曲风=9&skill_energy:阳春白雪=0] 徵
/cast 斩长鲸
/cast [曲风=9&tbuff:商] 宫
/cast [曲风<9&nobuff:青莲歌] 羽
/cast [tbuff:商] 角
/cast 商
)");
    auto &school = b.definition();
    school.info = { school.code, "长歌", "根骨", "Lunar", 737, 31, 0 };
    school.display_order = 1;
    school.saved.macro_digest = "704b3cbc7d19e8a619484738991b5e90fc3848b4b4eb286d75bb9c9aa8986316";
    school.saved.talents_v7 = { "斩长鲸", "鹿鸣", "青崖间", "参连", "凌冬", "豪情", "师襄" };
    school.saved.talents_v6 = { "鹿鸣", "青崖间", "参连", "凌冬", "豪情", "师襄" };
    school.precombat = true;
    school.alias = "mw50";
    school.weapon_effect = "神兵·羽";
    school.raid.formation = "莫问阵（常驻）";
    school.raid.hanxiao_stacks = 1;
    b.preset(2301,
             { 5902, 1656, 57, 2741, 225, 1153, 4212, 0, 559 },
             { "宫·4%伤害", "宫·4%会心", "宫·3%伤害", "宫·3%会心",
               "商·4%伤害", "商·3%伤害", "商·4%会心", "商·5%会心",
               "羽·4%伤害", "羽·3%伤害", "羽·4%会心", "羽·3%会心",
               "徵·4%伤害", "徵·3%伤害", "徵·4%会心", "徵·3%会心" });
    b.choices({ "斩长鲸", "鹿鸣", "青崖间", "参连", "凌冬", "豪情", "师襄", "超然", "无尽藏" }, {}, { "神兵·羽" });
    b.default_talents({ "鹿鸣", "青崖间", "参连", "凌冬", "超然", "斩长鲸", "豪情", "师襄", "无尽藏" });
    b.resource({ "曲风", 0, 0, 9 });
    b.state({ 0, "师襄", 0, 0, 9 }, false);
    b.state({ 9357, "商", 288, 48, 1 }, true, true, true);
    b.state({ 9361, "角", 288, 48, 1 }, true, true, true);
    b.state({ 0, "神兵·羽", 160, 0, 3 }, false);
    const Stats passive{ .pve = 31 };
    b.damage(14311, "商", { 95, 5, 470, 0, 0, 0, false, passive, {} });
    b.damage(14312, "角", { 95, 5, 470, 0, 0, 0, false, passive, {} });
    b.damage(14287, "商(DOT)", { 1, 0, 553, 0, 6, 48, false, passive, {} });
    b.damage(14291, "角(DOT)", { 1, 0, 538, 0, 6, 48, false, passive, {} });
    b.damage(14474, "宫", { 80, 10, 501, 0, 0, 0, false, passive, {} });
    b.damage(14227, "徵", { 84, 8, 341, 0, 0, 0, false, passive, {} });
    b.damage(14100, "羽", { 156, 19, 500, 0, 0, 0, false, passive, {} });
    b.secondary_damage(39128, "神兵·羽", { 0, 0, 2592, 0, 0, 0, false, passive, {} });
    b.damage(41831, "放鹿", { 10, 10, 1680, 0, 0, 0, false, passive, {} });
    b.damage(41831, "青崖间", { 10, 10, 1050, 0, 0, 0, false, passive, {} });
    b.dot("商", "商(DOT)");
    b.dot("角", "角(DOT)");
    b.cast("商", 0, 24, 0, { .range = 20 }, { b.hit("商"), b.apply("商") });
    b.cast("角", 0, 24, 0, { .range = 20 }, { b.hit("角"), b.apply("角") });
    b.cast("宫", 0, 24, 28, { .range = 20 }, { b.hit("宫") });
    b.cast("徵", 0, 24, 0, { .pulses = 3, .spacing = 15, .range = 20 }, { b.hit("徵") });
    b.cast("使用橙武", 800, 0, 0, { .choice = "神兵·羽" }, { b.reset("羽"), b.apply("神兵·羽", 3) });
    // 羽命中后，商、角各提前结算最多两跳，沿用各自的快照并扣除实际结算的跳数。
    b.cast("羽",
           96,
           24,
           0,
           { .free_cd = b.buff("神兵·羽"), .hasted_cd = true, .charges = 3, .range = 20, .stance_cooldown = true },
           { b.hit("羽"), b.when("神兵·羽", b.hit("神兵·羽")), b.remove("神兵·羽"), b.advance("商", 2), b.advance("角", 2) });
    // 商的伤害、会心秘籍同时作用于命中和后续持续伤害；下方宫、徵、羽分别列出各自受益的伤害条目。
    b.recipe("商", "商·2%会心", { .critical = 200 }, { "商", "商(DOT)" });
    b.recipe("商", "商·3%会心", { .critical = 300 }, { "商", "商(DOT)" });
    b.recipe("商", "商·3%伤害", { .damage = 31 }, { "商", "商(DOT)" });
    b.recipe("商", "商·4%会心", { .critical = 400 }, { "商", "商(DOT)" });
    b.recipe("商", "商·4%伤害", { .damage = 41 }, { "商", "商(DOT)" });
    b.recipe("商", "商·5%伤害", { .damage = 51 }, { "商", "商(DOT)" });
    b.recipe("宫", "宫·3%会心", { .critical = 300 }, { "宫" });
    b.recipe("宫", "宫·3%伤害", { .damage = 31 }, { "宫" });
    b.recipe("宫", "宫·4%会心", { .critical = 400 }, { "宫" });
    b.recipe("宫", "宫·4%伤害", { .damage = 41 }, { "宫" });
    b.recipe("徵", "徵·2%会心", { .critical = 200 }, { "徵" });
    b.recipe("徵", "徵·3%会心", { .critical = 300 }, { "徵" });
    b.recipe("徵", "徵·3%伤害", { .damage = 31 }, { "徵" });
    b.recipe("徵", "徵·4%会心", { .critical = 400 }, { "徵" });
    b.recipe("徵", "徵·4%伤害", { .damage = 41 }, { "徵" });
    b.recipe("羽", "羽·3%会心", { .critical = 300 }, { "羽" });
    b.recipe("羽", "羽·3%伤害", { .damage = 31 }, { "羽" });
    b.recipe("羽", "羽·4%会心", { .critical = 400 }, { "羽" });
    b.recipe("羽", "羽·4%伤害", { .damage = 41 }, { "羽" });
    b.alias("宫", "gong");
    b.alias("商", "shang");
    b.alias("角", "jue");
    b.alias("徵", "zhi");
    b.alias("羽", "yu");
    // 豪情在徵起手时立即打一击；常规引导段仍按自己的间隔结算。
    b.base_action("徵").pre_cast = { b.selected("豪情", b.hit("徵")) };
    // 青崖间在商、角收招时检查另一种持续效果。两者同在时回 1 点曲风，依次引爆本招和另一招的剩余跳数。
    for (auto [note, other] :
         {
             std::pair{ "商", "角" },
             std::pair{ "角", "商" }
    })
    {
        b.base_action(note).post_cast = { b.selected("青崖间", b.when(other, b.gain(0, 1))),
                   b.selected("青崖间", b.when(other, b.burst(note))),
                   b.selected("青崖间", b.burst(other)),
                   b.selected("师襄", b.resource_stacks("师襄", 0)) };
    }
    b.base_action("羽").post_cast = { b.selected("青崖间", b.hit("青崖间")), b.gain(0, 1), b.selected("师襄", b.resource_stacks("师襄", 0)) };
    b.damage(18860, "变宫", { 170, 10, 501, 0, 0, 0, false, passive });
    b.inherit_recipes("变宫", "宫");
    b.damage(46614, "九衢闻曲", { 200, 25, 140, 0, 0, 0, false, passive });
    b.variant("宫",
              "鹿鸣",
              96,
              0,
              { .pulses = 9, .spacing = 8, .range = 20, .pre_cast = { b.hit("变宫"), b.at(0, 1, 9, b.hit("九衢闻曲")) }, .timing_resource = 0, .pulses_per_point = true, .distribute_pulses = true, .later_remainder = true },
              { b.at(0, 1, 9, b.hit("放鹿")), b.gain(0, -1), b.selected("师襄", b.resource_stacks("师襄", 0)) });
    b.damage(14227, "徵·凌冬", { 84, 8, 341, 0, 0, 0, false, { .pve = 543 }, {} });
    b.ranks("徵·凌冬", { 477, 668, 935, 1309, 1833, 2567 });
    b.variant("徵", "凌冬", 288, 0, { .pulses = 6, .spacing = 10, .range = 20, .pre_cast = { b.selected("豪情", b.hit("徵")) }, .distribute_pulses = true }, { b.ranked_pulse("徵·凌冬") });
    b.inherit_recipes("徵·凌冬", "徵");
    b.set_bonus({ .damage = 102 }, { "徵", "徵·凌冬" });
    b.state({ 0, "青莲歌", 0, 0, 1 }, false);
    b.state({ 0, "青莲消耗", 0, 0, 4 }, false);
    b.state({ 0, "长鲸可用", 4800, 0, 2 }, false);
    b.damage(34676,
             "斩长鲸",
             {
                 4,
                 1,
                 14999,
                 0,
                 0,
                 0,
                 false,
                 { .pve = 31 },
                 {}
    });
    b.cast("斩长鲸", 0, 24, 0, { .required = b.buff("长鲸可用"), .choice = "斩长鲸", .range = 20 }, { b.remove("长鲸可用"), b.hit("斩长鲸") });
    b.charge_variant("羽", "斩长鲸", 160, 2);
    // 选斩长鲸后，羽收招给青莲歌；下方各音命中时才消耗青莲歌、追加剑音并累计长鲸进度。
    b.post_cast("羽", { b.selected("斩长鲸", b.apply("青莲歌")) });

    struct Shadow
    {
        std::string_view note, name;
        int              id, channel;
    };

    for (const auto &sh :
         std::array<Shadow, 4>{
             { { "商", "青莲剑·商", 42008, 568 }, { "角", "青莲剑·角", 42009, 580 }, { "羽", "青莲剑·羽", 42011, 208 }, { "宫", "青莲剑·宫", 42007, 688 } }
    })
    {
        b.secondary_damage(sh.id, sh.name, { 10, 10, sh.channel, 0, 0, 0, false, passive, {} });
        b.post_damage(sh.note,
                 { b.when("青莲歌", b.hit(sh.name, sh.note == "羽" ? 3 : 1)), b.when("青莲歌", b.build_charge("青莲消耗", 5, "长鲸可用")), b.remove("青莲歌") });
    }
    b.post_damage("变宫", { b.when("青莲歌", b.hit("青莲剑·宫")), b.when("青莲歌", b.build_charge("青莲消耗", 5, "长鲸可用")), b.remove("青莲歌") });
    for (auto [note, id, channel, ticks, interval] :
         {
             std::tuple{ "商", 31546, 568, 1, 16 },
             std::tuple{ "角", 31545, 580, 4, 16 },
             std::tuple{ "徵", 42010, 640, 8, 3  }
    })
    {
        const std::string_view aura =
            note == std::string_view("商")   ? "青莲剑·商(DOT)"
            : note == std::string_view("角") ? "青莲剑·角(DOT)"
                                             : "青莲剑·徵(DOT)";
        b.state({ id, aura, ticks * interval, interval, 1 }, true);
        b.secondary_damage(id, aura, { ticks == 8 ? 0 : 10, ticks == 8 ? 0 : 10, channel, 0, ticks, interval, false, passive, {} });
        b.dot(aura, aura);
        if (ticks == 8) {
            for (auto n : { "徵", "徵·凌冬" }) {
                b.post_damage(n, { b.when("青莲歌", b.apply(aura)), b.when("青莲歌", b.build_charge("青莲消耗", 5, "长鲸可用")), b.remove("青莲歌") });
            }
        } else {
            b.post_damage(ticks == 1 ? "青莲剑·商" : "青莲剑·角", { b.apply(aura) });
        }
    }
    b.state({ 0, "超然", 128, 16, 1 }, false);
    b.stance_haste("超然", 308);
    b.adjust([](Prepared &p) {
        if (p.config.precombat && std::ranges::contains(p.config.talents, "鹿鸣")) {
            p.resources[0].initial = 9;
            if (std::ranges::contains(p.config.talents, "师襄")) {
                auto &buffs = p.config.book->buffs;
                const auto slot = std::ranges::find(buffs, "师襄", &BuffDescriptor::name) - buffs.begin();
                p.opening.push_back({ Op::Add, static_cast<Slot>(slot), 9 });
            }
        }
    });
    b.state({ 34170, "阳春白雪", 80, 0, 1 }, true);
    // 雪花从一档起算。阳春白雪期间指定音命中提高档位，在阳春白雪到期时按当时的档位结算曲伤并清层。
    b.state({ 34172, "雪花等级", 0, 0, 40 }, true);
    Hit blossom{ 550, 40, 240, 0, 0, 0, false, passive };
    for (int rank = 1; rank <= 40; ++rank) blossom.rank_channels.push_back(rank <= 10 ? rank * 240 : 2400 + (rank - 10) * 200);
    b.damage(45777, "阳春白雪·曲", std::move(blossom));
    b.passive("阳春白雪·曲", "无尽藏", { .final_damage = 3072 });
    const auto snow = b.buff("阳春白雪");
    b.cast("阳春白雪", 384, 0, 0, { .charges = 2 },
           { b.when("阳春白雪", b.apply("雪花等级"), true), { .op = Op::ExtendAura, .slot = snow, .duration = 80 } });
    b.state({ 0, "高山流水", 192, 0, 1 }, false);
    b.cast("高山流水", 800, 0, 0, {}, { b.apply("高山流水") });
    Step shadow{ .op = Op::School, .slot = b.buff("超然"), .extra = b.effect("阳春白雪"), .neutral_buff = b.effect("高山流水"), .effect = 双曲超然 };
    b.cast("孤影化双", 960, 0, 0, { .choice = "超然", .hasted_cd = true }, { b.reset("羽"), shadow });
    b.pulse("超然", { b.when("阳春白雪", b.apply("雪花等级"), true), { .op = Op::ExtendAura, .slot = snow, .duration = 20 }, b.apply("雪花等级") });
    b.on_expiry("阳春白雪", { { Op::StackRankHit, b.effect("阳春白雪·曲"), b.buff("雪花等级") }, b.remove("雪花等级", 40) });
    for (auto note : { "商", "角", "宫", "变宫", "羽" }) b.post_damage(note, { b.when("阳春白雪", b.apply("雪花等级")) });
    b.post_damage("徵", { b.selected("无尽藏", b.when("阳春白雪", b.apply("雪花等级"))) });
    b.pre_cast("徵", { b.selected("无尽藏", b.selected("豪情", b.when("阳春白雪", b.apply("雪花等级")))) });
    b.post_damage("徵·凌冬", { b.selected("无尽藏", b.when("阳春白雪", b.apply("雪花等级"))) });
    b.recipe("商", "商·5%会心", { .critical = 500 }, { "商", "商(DOT)" });
    for (auto hit : { "商", "角", "商(DOT)", "角(DOT)", "宫", "羽", "徵", "徵·凌冬", "放鹿", "青崖间", "神兵·羽", "变宫", "九衢闻曲", "阳春白雪·曲", "斩长鲸", "青莲剑·商", "青莲剑·角", "青莲剑·羽", "青莲剑·宫", "青莲剑·商(DOT)", "青莲剑·角(DOT)", "青莲剑·徵(DOT)" }) {
        b.passive(hit, "参连", { .ignore = 614 });
        for (int i = 1; i <= 9; ++i) {
            const int bonus = (i * 1024 + 25) / 50;
            // 师襄读取本次命中或挂 DOT 当帧的曲风，回曲风与消耗都能即时改变后续伤害。
            b.resource_modifier(hit, 0, i, i, {.power = bonus, .strain_rating = bonus}, "师襄");
        }
    }
    b.legendary_damage({ "徵", "徵·凌冬" });
    return b.finish();
}
} // namespace JX3DPS::runtime::season50
