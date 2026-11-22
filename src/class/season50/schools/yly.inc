// Native event rules for 幽罗引. Timing is expressed in frames.
static Book yly()
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
    b.state({ 0, "缠绞", 192, 0, 5 }, true, false, false);
    b.state({ 0, "钤束", 288, 0, 1 }, false, false, false);
    b.state({ 0, "千里急DOT", 80, 16, 1 }, true, true, true);
    b.damage(0,
             "千里急(DOT)",
             {
                 1,
                 0,
                 1200,
                 0,
                 5,
                 32,
                 false,
                 { .pve = 553 },
                 { { b.buff("缠绞"), 1, { .fragile = 51 }, "" },
                   { b.buff("缠绞"), 2, { .fragile = 102 }, "" },
                   { b.buff("缠绞"), 3, { .fragile = 154 }, "" },
                   { b.buff("缠绞"), 4, { .fragile = 205 }, "" },
                   { b.buff("缠绞"), 5, { .fragile = 256 }, "" },
                   { b.buff("钤束"), 0, { .attack = 154, .strain_rating = 307 }, "" } }
    });
    b.damage(42417,
             "应天长",
             {
                 15,
                 1,
                 1400,
                 1024,
                 0,
                 0,
                 false,
                 { .pve = 553 },
                 { { b.buff("缠绞"), 1, { .fragile = 51 }, "" },
                   { b.buff("缠绞"), 2, { .fragile = 102 }, "" },
                   { b.buff("缠绞"), 3, { .fragile = 154 }, "" },
                   { b.buff("缠绞"), 4, { .fragile = 205 }, "" },
                   { b.buff("缠绞"), 5, { .fragile = 256 }, "" },
                   { b.buff("钤束"), 0, { .attack = 154, .strain_rating = 307 }, "" } }
    });
    b.damage(41465,
             "挑丝",
             {
                 15,
                 1,
                 480,
                 0,
                 0,
                 0,
                 false,
                 { .pve = 553 },
                 { { b.buff("缠绞"), 1, { .fragile = 51 }, "" },
                   { b.buff("缠绞"), 2, { .fragile = 102 }, "" },
                   { b.buff("缠绞"), 3, { .fragile = 154 }, "" },
                   { b.buff("缠绞"), 4, { .fragile = 205 }, "" },
                   { b.buff("缠绞"), 5, { .fragile = 256 }, "" },
                   { b.buff("钤束"), 0, { .attack = 154, .strain_rating = 307 }, "" } }
    });
    b.damage(42265,
             "绊线",
             {
                 15,
                 1,
                 720,
                 0,
                 0,
                 0,
                 false,
                 { .pve = 553 },
                 { { b.buff("缠绞"), 1, { .fragile = 51 }, "" },
                   { b.buff("缠绞"), 2, { .fragile = 102 }, "" },
                   { b.buff("缠绞"), 3, { .fragile = 154 }, "" },
                   { b.buff("缠绞"), 4, { .fragile = 205 }, "" },
                   { b.buff("缠绞"), 5, { .fragile = 256 }, "" },
                   { b.buff("钤束"), 0, { .attack = 154, .strain_rating = 307 }, "" } }
    });
    b.damage(41486,
             "勾线",
             {
                 43,
                 2,
                 422,
                 0,
                 0,
                 0,
                 false,
                 { .pve = 553 },
                 { { b.buff("缠绞"), 1, { .fragile = 51 }, "" },
                   { b.buff("缠绞"), 2, { .fragile = 102 }, "" },
                   { b.buff("缠绞"), 3, { .fragile = 154 }, "" },
                   { b.buff("缠绞"), 4, { .fragile = 205 }, "" },
                   { b.buff("缠绞"), 5, { .fragile = 256 }, "" },
                   { b.buff("钤束"), 0, { .attack = 154, .strain_rating = 307 }, "" } }
    });
    b.ranks("勾线", { 422, 444, 466, 488, 510, 532, 554, 576, 598, 620 });
    b.damage(42976,
             "谐调",
             {
                 150,
                 1,
                 500,
                 1024,
                 0,
                 0,
                 false,
                 { .pve = 553 },
                 { { b.buff("缠绞"), 1, { .fragile = 51 }, "" },
                   { b.buff("缠绞"), 2, { .fragile = 102 }, "" },
                   { b.buff("缠绞"), 3, { .fragile = 154 }, "" },
                   { b.buff("缠绞"), 4, { .fragile = 205 }, "" },
                   { b.buff("缠绞"), 5, { .fragile = 256 }, "" },
                   { b.buff("钤束"), 0, { .attack = 154, .strain_rating = 307 }, "" } }
    });
    b.ranks("谐调", { 500, 1000, 2000 });
    b.dot("千里急DOT", "千里急(DOT)");
    b.cast("应天长", 480, 24, 0, { .choice = "应天长", .charges = 3 }, { b.gain(0, 20), b.hit("应天长", 1), b.apply("千里急DOT", 1) });
    b.cast("挑丝", 0, 24, 0, {}, { b.gain(0, 15), b.hit("挑丝", 1) });
    b.cast("绊线",
           0,
           24,
           0,
           { .resource = 0, .cost = 10 },
           { b.gain(0, -10), b.hit("绊线", 1), b.selected("钤束", b.apply("钤束", 1)), b.apply("缠绞", 1) });
    b.cast("勾线",
           288,
           0,
           0,
           { .resource          = 0,
             .cost              = 20,
             .pulses            = 4,
             .spacing           = 8,
             .channel_cost_step = 10,
             .extra_pulses      = 3,
             .max_cost_steps    = 2,
             .finisher          = b.effect("谐调"),
             .charges           = 2, .distribute_pulses = true },
           { b.ranked_pulse("勾线"), b.apply("缠绞", 1) });
    b.recipe("千里急", "千里急·2%会心", { .critical = 200 }, { "千里急(DOT)" });
    b.recipe("千里急", "千里急·3%会心", { .critical = 300 }, { "千里急(DOT)" });
    b.recipe("挑丝", "挑丝·2%会心", { .critical = 200 }, { "挑丝" });
    b.recipe("挑丝", "挑丝·4%伤害", { .damage = 41 }, { "挑丝" });
    b.recipe("挑丝", "挑丝·5%伤害", { .damage = 51 }, { "挑丝" });
    b.recipe("绊线", "绊线·2%会心", { .critical = 200 }, { "绊线" });
    b.recipe("绊线", "绊线·3%会心", { .critical = 300 }, { "绊线" });
    b.recipe("绊线", "绊线·4%伤害", { .damage = 41 }, { "绊线" });
    b.recipe("绊线", "绊线·5%伤害", { .damage = 51 }, { "绊线" });
    b.set_bonus({ .damage = 102 }, { "绊线" });
    b.damage(42098, "寄线", { 15, 1, 170, 0, 0, 0, false, { .pve = 553 }, {} });
    b.passive("寄线", "绞网", { .fragile = 1024 });
    b.passive("千里急(DOT)", "寄意", { .critical = 1500, .power = 157 });

    b.adjust([](Prepared &p) {
        const auto &book = *p.config.book;
        const auto group = std::ranges::find(book.buffs, "傀儡公调", &BuffDescriptor::name) - book.buffs.begin();
        for (auto &steps : p.starts) for (auto &step : steps)
            if (step.op == Op::Add && step.slot == group) step.duration = static_cast<int>(24 / p.haste);
        if (std::ranges::contains(p.config.talents, std::string_view{ "绞网" })) {
            p.resources[0].spent_per_point = 10;
            p.resources[0].spent_hit =
                static_cast<Slot>(std::ranges::find(p.skills, std::string_view{ "寄线" }, &SkillDescriptor::name) -
                                  p.skills.begin());
        }
    });
    b.state({ 0, "虚影", 160, 16, 1 }, true);
    b.state({ 0, "傀儡", 0, 0, 1 }, false);
    b.state({ 0, "傀儡公调", 24, 0, 1 }, false);
    b.state({ 0, "连环慢", 128, 0, 1 }, false);
    b.cast("落笼簿", 0, 16, 0, { .gcd_group = 2 }, { { Op::ToggleAura, b.buff("傀儡") } });
    b.variant("落笼簿", "叠拍", 160, 0, { .gcd_group = 2 },
              { b.apply_for("傀儡", 64), { Op::ShortenGcd, 3, 24 }, b.remove("傀儡公调") });
    b.cast("连环慢", 128, 4, 0, { .charges = 2, .gcd_group = 4, .cast_while_channeling = true }, { b.apply("连环慢") });
    b.state({ 0, "迭影", 80, 0, 1 }, false);
    b.damage(42576, "障幕虚影", { 15, 1, 280, 1024, 0, 0, false, { .pve = 553 } });
    b.cast("障幕虚影", 1440, 24, 0, { .gcd_group = 1 }, { b.apply("虚影"), b.hit("寄线", 5) });
    b.pulse("虚影", { b.hit("障幕虚影") });
    b.after_cast("挑丝", { b.selected("黄粱", b.shorten("障幕虚影", 48)) });
    b.damage(42021, "生地狱", { 150, 1, 196, 1024, 0, 0, false, { .pve = 553 } });
    b.ranks("生地狱", {197, 227, 256, 286, 316, 345});
    b.rank_bases("生地狱", {{150,1}, {170,1}, {190,1}, {210,1}, {230,1}, {250,1}});
    b.state({ 0, "生地狱·操控", 15, 0, 1 }, false);
    b.state({ 0, "生地狱·连环慢", 15, 0, 1 }, false);
    b.modifier("生地狱", { b.buff("生地狱·操控"), 0, { .channel_multiplier = 2.2 } });
    b.modifier("生地狱", { b.buff("生地狱·连环慢"), 0, { .channel_multiplier = 1.25 } });
    b.modifier("生地狱", { b.buff("钤束"), 0, { .attack = 154, .strain_rating = 307 } });
    b.passive("生地狱", "寄意", { .critical = 1500, .power = 150 });
    for (int i = 1; i <= 5; ++i) b.modifier("生地狱", { b.buff("缠绞"), i, { .fragile = (256 * i + 2) / 5 } });
    b.cast("生地狱", 480, 24, 0, { .required = b.buff("傀儡"), .gcd_group = 3 }, {});
    b.variant("生地狱", "迭影", 480, 0, { .gcd_group = 3 }, { b.apply("迭影") });
    b.on_cast("生地狱", { b.when("傀儡", b.apply("生地狱·操控")), b.when("连环慢", b.apply("生地狱·连环慢")), b.remove("连环慢") });
    for (int delay : {4, 9, 14}) {
        b.on_cast("生地狱", {{ .op = Op::DelayedHit, .slot = b.effect("生地狱"), .amount = delay, .extra = b.buff("缠绞"), .rank_offset = 1 }});
    }
    b.after_cast("生地狱", { b.remove("缠绞", 5) });
    b.after_cast("绊线", { b.when("迭影", b.gain(0, 10)), b.when("迭影", { Op::ShortenGcd, 0, 8 }) });
    b.after_cast("生地狱", { b.selected("寄意", b.shorten("勾线", 16)) });
    b.modifier("绊线", { b.buff("迭影"), 0, { .damage = 512 } });
    constexpr std::string_view skills[]{ "千里急·壹", "千里急·贰", "千里急·叁", "千里急·肆" };
    constexpr int channels[]{ 140, 210, 280, 420 };
    for (int i = 0; i < 4; ++i) b.damage(i == 3 ? 42384 : 41471 + i, skills[i], { 15, 1, double(channels[i]), i == 3 ? 1024 : 0, 0, 0, false, { .pve = 553 } });
    b.state({ 0, "千里急次序", 0, 0, 4 }, false);
    b.state({ 0, "自动千里急", 1, 0, 1 }, false);
    b.state({0, "天羊盏", 80, 0, 1}, false);
    b.state({32145, "刻木", 240, 48, 3}, true, true, true);
    b.damage(43081, "夹线", {15, 1, 2600, 0, 0, 0, false, {.pve = 553}});
    b.secondary_damage(43082, "刻木(DOT)", {1, 0, 1600, 0, 5, 48, false, {.pve = 553}});
    b.dot("刻木", "刻木(DOT)");
    b.cast("使用橙武", 960, 0, 0, {.choice = Legendary}, {b.apply("天羊盏")});
    b.cast("夹线", 0, 24, 0, {.resource = 0, .cost = 20, .required = b.buff("天羊盏"), .choice = Legendary},
           {b.gain(0, -20), b.gain(0, 10), b.hit("夹线"), b.apply("刻木")});
    for (auto cast : { "挑丝", "绊线", "勾线", "应天长", "夹线" }) {
        b.on_cast(cast, { b.when("傀儡公调", b.apply("自动千里急"), true), b.when("自动千里急", b.apply("千里急次序")),
                          b.when("自动千里急", { .op = Op::StackStageHit, .slot = b.effect("千里急·壹"), .extra = b.buff("千里急次序") }),
                          b.when("自动千里急", b.apply("傀儡公调")), b.selected("寄意", b.when("自动千里急", b.shorten("勾线", 16))),
                          b.at_least("千里急次序", 4, b.remove("千里急次序", 4)), b.remove("自动千里急") });
    }
    b.school_modifier({ INVALID_SLOT, 0, { .ignore = 614 }, "寡合" });
    b.school_modifier({ b.buff("虚影"), 0, { .damage = 307 }, "黄粱" });
    b.recipe("千里急", "千里急·3%系数", { .channel_multiplier = 1.03 }, { "千里急·壹", "千里急·贰", "千里急·叁", "千里急·肆", "千里急(DOT)" });
    b.recipe("千里急", "千里急·4%系数", { .channel_multiplier = 1.04 }, { "千里急·壹", "千里急·贰", "千里急·叁", "千里急·肆", "千里急(DOT)" });
    b.recipe("生地狱", "生地狱·3%伤害", { .damage = 31 }, { "生地狱" });
    b.recipe("生地狱", "生地狱·2%会心", { .critical = 200 }, { "生地狱" });
    b.recipe("生地狱", "生地狱·2%伤害", { .damage = 20 }, { "生地狱" });
    b.recipe("生地狱", "生地狱·3%会心", { .critical = 300 }, { "生地狱" });
    b.recipe("障幕虚影", "障幕虚影·3%伤害", { .damage = 31 }, { "障幕虚影" });
    b.recipe("障幕虚影", "障幕虚影·2%伤害", { .damage = 20 }, { "障幕虚影" });
    b.legendary_damage({ "勾线", "生地狱" });
    return b.finish();
}
