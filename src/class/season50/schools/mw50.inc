// Level-50 base notes. Each accelerated DOT tick consumes one of the original ticks.
static Book mw50()
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
    b.cast("羽",
           96,
           24,
           0,
           { .free_cd = b.buff("神兵·羽"), .hasted_cd = true, .charges = 3, .range = 20, .stance_cooldown = true },
           { b.hit("羽"), b.when("神兵·羽", b.hit("神兵·羽")), b.remove("神兵·羽"), b.advance("商", 2), b.advance("角", 2) });
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
    b.phases("徵", { b.selected("豪情", b.hit("徵")) }, {});
    // Two notes meet on this target. Consume both snapshots before the next cast.
    for (auto [note, other] :
         {
             std::pair{ "商", "角" },
             std::pair{ "角", "商" }
    })
    {
        b.phases(note,
                 {},
                 { b.selected("青崖间", b.when(other, b.gain(0, 1))),
                   b.selected("青崖间", b.when(other, b.burst(note))),
                   b.selected("青崖间", b.burst(other)),
                   b.selected("师襄", b.resource_stacks("师襄", 0)) });
    }
    b.phases("羽", {}, { b.selected("青崖间", b.hit("青崖间")), b.gain(0, 1), b.selected("师襄", b.resource_stacks("师襄", 0)) });
    b.damage(18860, "变宫", { 170, 10, 501, 0, 0, 0, false, passive });
    b.inherit_recipes("变宫", "宫");
    b.damage(46614, "九衢闻曲", { 200, 25, 140, 0, 0, 0, false, passive });
    b.variant("宫",
              "鹿鸣",
              96,
              0,
              { .pulses = 9, .spacing = 8, .range = 20, .start = { b.hit("变宫"), b.at(0, 1, 9, b.hit("九衢闻曲")) }, .timing_resource = 0, .pulses_per_point = true, .distribute_pulses = true, .later_remainder = true },
              { b.at(0, 1, 9, b.hit("放鹿")), b.gain(0, -1), b.selected("师襄", b.resource_stacks("师襄", 0)) });
    b.damage(14227, "徵·凌冬", { 84, 8, 341, 0, 0, 0, false, { .pve = 543 }, {} });
    b.ranks("徵·凌冬", { 477, 668, 935, 1309, 1833, 2567 });
    b.variant("徵", "凌冬", 288, 0, { .pulses = 6, .spacing = 10, .range = 20, .start = { b.selected("豪情", b.hit("徵")) }, .distribute_pulses = true }, { b.ranked_pulse("徵·凌冬") });
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
    b.after_cast("羽", { b.selected("斩长鲸", b.apply("青莲歌")) });

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
        b.on_hit(sh.note,
                 { b.when("青莲歌", b.hit(sh.name, sh.note == "羽" ? 3 : 1)), b.when("青莲歌", b.build_charge("青莲消耗", 5, "长鲸可用")), b.remove("青莲歌") });
    }
    b.on_hit("变宫", { b.when("青莲歌", b.hit("青莲剑·宫")), b.when("青莲歌", b.build_charge("青莲消耗", 5, "长鲸可用")), b.remove("青莲歌") });
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
                b.on_hit(n, { b.when("青莲歌", b.apply(aura)), b.when("青莲歌", b.build_charge("青莲消耗", 5, "长鲸可用")), b.remove("青莲歌") });
            }
        } else {
            b.on_hit(ticks == 1 ? "青莲剑·商" : "青莲剑·角", { b.apply(aura) });
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
    // Rank one represents an unenhanced flower. Each note adds one rank.
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
    Step shadow{ .op = Op::SpendPairForAura, .slot = b.buff("超然"), .extra = b.effect("阳春白雪"), .neutral_buff = b.effect("高山流水") };
    b.cast("孤影化双", 960, 0, 0, { .choice = "超然", .hasted_cd = true }, { b.reset("羽"), shadow });
    b.pulse("超然", { b.when("阳春白雪", b.apply("雪花等级"), true), { .op = Op::ExtendAura, .slot = snow, .duration = 20 }, b.apply("雪花等级") });
    b.on_expiry("阳春白雪", { { Op::StackRankHit, b.effect("阳春白雪·曲"), b.buff("雪花等级") }, b.remove("雪花等级", 40) });
    for (auto note : { "商", "角", "宫", "变宫", "羽" }) b.on_hit(note, { b.when("阳春白雪", b.apply("雪花等级")) });
    b.on_hit("徵", { b.selected("无尽藏", b.when("阳春白雪", b.apply("雪花等级"))) });
    b.on_cast("徵", { b.selected("无尽藏", b.selected("豪情", b.when("阳春白雪", b.apply("雪花等级")))) });
    b.on_hit("徵·凌冬", { b.selected("无尽藏", b.when("阳春白雪", b.apply("雪花等级"))) });
    b.recipe("商", "商·5%会心", { .critical = 500 }, { "商", "商(DOT)" });
    for (auto hit : { "商", "角", "商(DOT)", "角(DOT)", "宫", "羽", "徵", "徵·凌冬", "放鹿", "青崖间", "神兵·羽", "变宫", "九衢闻曲", "阳春白雪·曲", "斩长鲸", "青莲剑·商", "青莲剑·角", "青莲剑·羽", "青莲剑·宫", "青莲剑·商(DOT)", "青莲剑·角(DOT)", "青莲剑·徵(DOT)" }) {
        b.passive(hit, "参连", { .ignore = 615 });
        for (int i = 1; i <= 9; ++i) {
            const int bonus = (i * 1024 + 25) / 50;
            b.modifier(hit,
                       {
                           b.buff("师襄"),
                           i,
                           { .power = bonus, .strain_rating = bonus },
                           "师襄"
            });
        }
    }
    b.legendary_damage({ "徵", "徵·凌冬" });
    return b.finish();
}
