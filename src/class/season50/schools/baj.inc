// Native event rules for 北傲诀. Timing is expressed in frames.
static Book baj()
{
    Builder b("baj", "北傲诀", R"(# 魔盒宏适配 https://www.jx3box.com/macro/84258
/cast 使用风腰坠
/cast 使用橙武
/cast [buff:四面楚歌可用] 四面楚歌
/cast [buff:秀明尘身&狂意>70&nobuff:疏狂] 西楚悲歌
/cast 上将军印
/cast [buff:秀明尘身&狂意<10] 龙骧虎步
/cast [buff:秀明尘身&狂意<40&nobuff:斩狂枭] 松烟竹雾
/cast [buff:秀明尘身&buff:龙骧&bufftime:斩狂枭<0.4] 松烟竹雾
/cast 雷走风切
/cast 破釜沉舟
/cast [buff:松烟竹雾&nobuff:临江&tbuff:闹须弥] 秀明尘身
/cast [buff:松烟竹雾&buff:临江&bufftime:临江<19.2] 雪絮金屏
/cast 闹须弥
/cast 碎江天
/cast 擒龙六斩
/cast 坚壁清野
/cast [buff:雪絮金屏&skill_notin_cd:上将军印] 秀明尘身
)");
    b.preset(2300,
             { 6809, 1242, 170, 1260, 208, 1772, 4212, 0, 1092 },
             { "刀啸风吟·5%伤害",
               "刀啸风吟·4%伤害",
               "刀啸风吟·4%会心",
               "刀啸风吟·3%会心",
               "雷走风切·5%伤害",
               "雷走风切·4%伤害",
               "雷走风切·4%会心",
               "雷走风切·3%伤害",
               "上将军印·4%会心",
               "上将军印·4%伤害",
               "上将军印·3%会心",
               "上将军印·3%伤害",
               "破釜沉舟·5%伤害",
               "破釜沉舟·4%会心",
               "破釜沉舟·4%伤害",
               "破釜沉舟·3%伤害", "碎江天·5%伤害", "碎江天·4%会心", "碎江天·4%伤害", "碎江天·3%伤害",
               "擒龙六斩·5%伤害", "擒龙六斩·回气劲", "擒龙六斩·4%伤害", "擒龙六斩·回狂意",
               "醉斩白蛇·5%伤害", "醉斩白蛇·4%会心", "醉斩白蛇·4%伤害", "醉斩白蛇·3%伤害" });
    b.choices({ "斩狂枭", "冥鼓", "分疆", "霜云", "碎影", "见尘", "临江", "龙息", "疏狂" }, {}, {});
    b.resource({ "狂意", 100, 0, 100 });
    b.resource({ "刀魂", 100, 0, 100 });
    b.resource({ "气劲", 100, 0, 100 });
    b.state({ 0, "秀明尘身", 0, 0, 1 }, false, false, false);
    b.state({ 0, "松烟竹雾", 0, 0, 1 }, false, false, false);
    b.state({ 0, "雪絮金屏", 0, 0, 1 }, false, false, false);
    b.state({ 0, "闹须弥", 384, 48, 1 }, true, true, true);
    b.state({ 0, "临江", 544, 0, 1 }, false, false, false);
    b.initial("秀明尘身");
    b.state({ 0, "斩狂枭", 160, 0, 1 }, false);

    b.damage(0, "闹须弥(DOT)", { 1, 0, 560, 0, 8, 48, true, { .pve = 174 }, { { b.buff("临江"), 0, { .attack = 154 }, "" } } });
    b.damage(16803,
             "上将军印",
             { 30, 1, 314, 1024, 0, 0, true, { .pve = 174 }, { { b.buff("临江"), 0, { .attack = 154, .pve = 809 }, "" } } });
    b.damage(16802,
             "上将军印·连续1",
             { 34, 1, 361, 1177, 0, 0, true, { .pve = 174 }, { { b.buff("临江"), 0, { .attack = 154, .pve = 809 }, "" } } });
    b.damage(16801,
             "上将军印·连续2",
             { 39, 1, 408, 1331, 0, 0, true, { .pve = 174 }, { { b.buff("临江"), 0, { .attack = 154, .pve = 809 }, "" } } });
    b.damage(16800,
             "上将军印·连续3",
             { 43, 1, 456, 1484, 0, 0, true, { .pve = 174 }, { { b.buff("临江"), 0, { .attack = 154, .pve = 809 }, "" } } });
    b.damage(17043,
             "上将军印·连续4",
             { 48, 1, 503, 1638, 0, 0, true, { .pve = 174 }, { { b.buff("临江"), 0, { .attack = 154, .pve = 809 }, "" } } });
    b.damage(20991,
             "破釜沉舟",
             { 35, 2, 794.6461314000001, 2048, 0, 0, true, { .pve = 174 }, { { b.buff("临江"), 0, { .attack = 154, .pve = 809 }, "" } } });
    b.damage(16610,
             "刀啸风吟",
             { 25, 2, 886.3731676800003, 0, 0, 0, true, { .pve = 174 }, { { b.buff("临江"), 0, { .attack = 154 }, "" } } });
    b.damage(16631, "雷走风切", { 17, 1, 125.44, 0, 0, 0, true, { .pve = 174 }, { { b.buff("临江"), 0, { .attack = 154 }, "" } } });
    b.state({ 0, "体态回能", 0, 16, 1 }, false);
    b.initial("体态回能");
    b.pulse("体态回能", { b.when("秀明尘身", b.gain(0, 1), true), b.when("松烟竹雾", b.gain(1, 1), true), b.when("雪絮金屏", b.gain(2, 1), true) });
    b.dot("闹须弥", "闹须弥(DOT)");
    b.cast("秀明尘身",
           48,
           0,
           0,
           { .forbidden = b.buff("秀明尘身") },
           { b.remove("松烟竹雾", 1000), b.remove("雪絮金屏", 1000), b.apply("秀明尘身", 1) });
    b.cast("松烟竹雾",
           48,
           0,
           0,
           { .forbidden = b.buff("松烟竹雾") },
           { b.remove("秀明尘身", 1000), b.remove("雪絮金屏", 1000), b.apply("松烟竹雾", 1) });
    b.cast("雪絮金屏",
           48,
           0,
           0,
           { .forbidden = b.buff("雪絮金屏") },
           { b.remove("秀明尘身", 1000), b.remove("松烟竹雾", 1000), b.apply("雪絮金屏", 1) });
    b.cast("闹须弥", 400, 24, 0, { .resource = 1, .cost = 10, .required = b.buff("松烟竹雾") }, { b.gain(1, -10), b.apply("闹须弥", 1) });
    b.state({0, "悬翎在握", 128, 0, 1}, false);
    b.state({0, "橙武破釜", 128, 0, 3}, false);
    b.state({0, "四面楚歌可用", 128, 0, 1}, false);
    Step debit = b.when("斩狂枭", b.gain(0, -10), true);
    debit.blocked_by = b.buff("橙武破釜");
    b.cast("破釜沉舟",
           144,
           24,
           0,
           { .resource = 0, .cost = 10, .required = b.buff("秀明尘身"), .charges = 2, .free_cost = b.buff("斩狂枭"), .shared_repayment = true, .weapon_free_cost = b.buff("橙武破釜") },
           { debit, b.hit("破釜沉舟"), b.when("斩狂枭", b.reset("破釜沉舟")) });
    b.cast("刀啸风吟", 128, 24, 24, { .resource = 2, .cost = 5, .required = b.buff("雪絮金屏") }, { b.gain(2, -5), b.hit("刀啸风吟", 1) });
    b.cast("雷走风切", 256, 24, 0, { .required = b.buff("秀明尘身"), .lock = 24, .charges = 3, .gcd_group = 2, .shared_repayment = true }, { b.hit("雷走风切", 1) });
    b.recipe("上将军印", "上将军印·2%会心", { .critical = 200 }, { "上将军印", "上将军印·连续1", "上将军印·连续2", "上将军印·连续3", "上将军印·连续4" });
    b.recipe("上将军印", "上将军印·2%伤害", { .damage = 21 }, { "上将军印", "上将军印·连续1", "上将军印·连续2", "上将军印·连续3", "上将军印·连续4" });
    b.recipe("上将军印", "上将军印·3%会心", { .critical = 300 }, { "上将军印", "上将军印·连续1", "上将军印·连续2", "上将军印·连续3", "上将军印·连续4" });
    b.recipe("上将军印", "上将军印·3%伤害", { .damage = 31 }, { "上将军印", "上将军印·连续1", "上将军印·连续2", "上将军印·连续3", "上将军印·连续4" });
    b.recipe("上将军印", "上将军印·4%会心", { .critical = 400 }, { "上将军印", "上将军印·连续1", "上将军印·连续2", "上将军印·连续3", "上将军印·连续4" });
    b.recipe("上将军印", "上将军印·4%伤害", { .damage = 41 }, { "上将军印", "上将军印·连续1", "上将军印·连续2", "上将军印·连续3", "上将军印·连续4" });
    b.recipe("刀啸风吟", "刀啸风吟·2%会心", { .critical = 200 }, { "刀啸风吟" });
    b.recipe("刀啸风吟", "刀啸风吟·3%会心", { .critical = 300 }, { "刀啸风吟" });
    b.recipe("刀啸风吟", "刀啸风吟·4%会心", { .critical = 400 }, { "刀啸风吟" });
    b.recipe("刀啸风吟", "刀啸风吟·4%伤害", { .damage = 41 }, { "刀啸风吟" });
    b.recipe("刀啸风吟", "刀啸风吟·5%伤害", { .damage = 51 }, { "刀啸风吟" });
    b.recipe("破釜沉舟", "破釜沉舟·2%会心", { .critical = 200 }, { "破釜沉舟" });
    b.recipe("破釜沉舟", "破釜沉舟·3%会心", { .critical = 300 }, { "破釜沉舟" });
    b.recipe("破釜沉舟", "破釜沉舟·3%伤害", { .damage = 31 }, { "破釜沉舟" });
    b.recipe("破釜沉舟", "破釜沉舟·4%会心", { .critical = 400 }, { "破釜沉舟" });
    b.recipe("破釜沉舟", "破釜沉舟·4%伤害", { .damage = 41 }, { "破釜沉舟" });
    b.recipe("破釜沉舟", "破釜沉舟·5%伤害", { .damage = 51 }, { "破釜沉舟" });
    b.recipe("雷走风切", "雷走风切·2%会心", { .critical = 200 }, { "雷走风切" });
    b.recipe("雷走风切", "雷走风切·3%会心", { .critical = 300 }, { "雷走风切" });
    b.recipe("雷走风切", "雷走风切·3%伤害", { .damage = 31 }, { "雷走风切" });
    b.recipe("雷走风切", "雷走风切·4%会心", { .critical = 400 }, { "雷走风切" });
    b.recipe("雷走风切", "雷走风切·4%伤害", { .damage = 41 }, { "雷走风切" });
    b.recipe("雷走风切", "雷走风切·5%伤害", { .damage = 51 }, { "雷走风切" });
    b.state({ 0, "冥鼓", 480, 0, 1 }, false);
    b.passive("破釜沉舟", "冥鼓", { .damage = 512 });
    b.phases("破釜沉舟", {}, { b.selected("冥鼓", b.apply("冥鼓")) });
    b.secondary_damage(39106, "背水沉舟", {5, 1, 786.9312, 0, 0, 0, true, {.pve = 174}});
    b.secondary_damage(34119, "沉舟(DOT)", {25, 0, 269, 0, 6, 48, true, {.pve = 174}});
    b.secondary_damage(42677, "霜云", {10, 5, 896, 1024, 0, 0, true, {.pve = 174}});
    for (auto hit : { "破釜沉舟", "雷走风切", "刀啸风吟", "闹须弥(DOT)", "上将军印", "上将军印·连续1", "上将军印·连续2", "上将军印·连续3", "上将军印·连续4", "背水沉舟", "沉舟(DOT)", "霜云" })
    {
        b.modifier(hit, { b.buff("冥鼓"), 0, { .ignore = 512 }, "冥鼓" });
    }
    b.set_bonus({ .damage = 102 }, { "破釜沉舟" });
    b.state({ 0, "碎江天机会", 0, 0, 3 }, false);
    b.damage(17092, "碎江天", { 30, 3, 880, 2048, 0, 0, true, { .pve = 174 }, {} });
    b.damage(38533, "碎影", { 5, 1, 950, 0, 0, 0, true, { .pve = 174 }, {} });
    b.after_cast("闹须弥", { b.selected("碎影", b.apply("碎江天机会")) });
    b.state({ 0, "龙息", 24, 0, 1 }, false);
    b.cast("碎江天",
           0,
           24,
           0,
           { .resource = 1, .cost = 5, .required = b.buff("碎江天机会"), .forbidden = b.buff("秀明尘身"), .also_forbidden = b.buff("雪絮金屏"), .alternate_gcd_aura = b.buff("龙息"), .alternate_gcd_group = 3 },
           { b.gain(1, -5), b.remove("碎江天机会"), b.hit("碎江天"), b.selected("碎影", b.when("闹须弥", b.hit("碎影"))) });
    b.damage(16774, "龙骧虎步·大刀一", { 16, 1, 192, 0, 0, 0, true, { .pve = 174 }, {} });
    b.damage(16775, "龙骧虎步·大刀二", { 16, 1, 192, 0, 0, 0, true, { .pve = 174 }, {} });
    b.damage(16097, "龙骧虎步·双刀", { 16, 1, 192, 0, 0, 0, true, { .pve = 174 }, {} });
    b.cast("龙骧虎步",
           240,
           0,
           0,
           { .forbidden = b.buff("雪絮金屏"), .chain_window = 96 },
           { b.when("松烟竹雾", b.hit("龙骧虎步·双刀")), b.gain(0, 30), b.gain(1, 30), b.gain(2, 30) });
    b.damage(19423,
             "上将军印·连续5",
             { 52, 1, 550, 1638, 0, 0, true, { .pve = 174 }, { { b.buff("临江"), 0, { .attack = 154, .pve = 809 }, {} } } });
    b.damage(19424,
             "上将军印·连续6",
             { 57, 1, 597, 1638, 0, 0, true, { .pve = 174 }, { { b.buff("临江"), 0, { .attack = 154, .pve = 809 }, {} } } });
    b.damage(32859, "上将军印·见尘", { 7, 0, 133, 0, 0, 0, true, { .pve = 174 }, {} });
    b.secondary_damage(25782, "上将军印·神兵", {0, 2, 25, 0, 0, 0, true, {.pve = 174}});
    b.damage(38537, "楚歌", { 5, 1, 416, 0, 0, 0, true, { .pve = 174 }, {} });
    b.state({11241, "上将连续命中", 35, 0, 6}, true);
    constexpr std::string_view cuts[]{"上将军印", "上将军印·连续1", "上将军印·连续2", "上将军印·连续3",
                                      "上将军印·连续4", "上将军印·连续5", "上将军印·连续6"};
    std::vector<Step> body_hits;
    for (int i = 0; i < 7; ++i) {
        b.on_hit(cuts[i], {b.selected(Legendary, b.proc("上将军印·神兵", 102))});
        const auto hit = b.hit(cuts[i]);
        body_hits.push_back(b.within(10, i ? b.stacks("上将连续命中", i, hit) : b.when("上将连续命中", hit, true)));
    }
    body_hits.push_back(b.within(10, b.apply("上将连续命中")));
    body_hits.push_back(b.within(10, b.selected("见尘", b.when("闹须弥", b.hit("上将军印·见尘")))));
    const auto spend = b.selected("斩狂枭", {Op::SpendForDuration, b.buff("斩狂枭"), 0});
    Action body{.resource = 0, .cost = 10, .required = b.buff("秀明尘身"), .pulses = 5, .spacing = 8,
                .start = {b.gain(0, -10), spend}, .fixed_cast_time = true};
    b.cast("上将军印", 240, 24, 0, body, body_hits);
    body.pulses = 7;
    body.spacing = 6;
    b.variant("上将军印", "分疆", 240, 0, body, body_hits);

    // 临江在施展时召唤灵体，排定的刀数不受角色后续动作影响。
    const auto summon = [&](std::initializer_list<int> offsets, std::string_view other = {}) {
        std::vector<Step> steps{b.gain(0, -10), b.apply("临江"), spend};
        int stage = 0;
        for (int frame : offsets) steps.push_back(b.later(cuts[stage++], frame));
        for (int frame : offsets) steps.push_back(b.selected("见尘", b.when("闹须弥", b.later("上将军印·见尘", frame))));
        b.variant("上将军印", "临江", 240, 0, {.resource = 0, .cost = 10, .required = b.buff("秀明尘身")}, std::move(steps), other);
    };
    summon({4, 12, 20, 24, 28});
    summon({4, 8, 12, 16, 20, 24, 28}, "分疆");
    b.on_hit("破釜沉舟", { b.when("斩狂枭", b.hit("楚歌")) });
    b.state({ 0, "龙骧", 80, 0, 1 }, false);
    b.after_cast("龙骧虎步", { b.apply("龙骧"), b.when("秀明尘身", b.cycle("龙骧虎步·大刀一", 2)) });
    b.state({ 0, "疏狂", 208, 0, 1 }, false);
    b.state({ 0, "擒龙连段", 0, 0, 6 }, false);
    b.state({ 0, "叠霜", 320, 0, 6 }, false);
    for (auto stance : { "秀明尘身", "雪絮金屏", "松烟竹雾" }) {
        b.on_cast(stance, { b.when("斩狂枭", { Op::RefundRemainingDuration, b.buff("斩狂枭"), 0 }),
                            b.remove("擒龙连段", 6), b.remove("龙息") });
    }
    b.damage(17006, "西楚悲歌", { 11, 2, 86, 1024, 0, 0, true, { .pve = 174 } });
    b.cast("西楚悲歌", 960, 16, 0, { .required = b.buff("秀明尘身"), .choice = "疏狂", .lock = 16, .charges = 2, .gcd_group = 4 },
           { b.apply("疏狂"), b.later("西楚悲歌", 2) });
    b.damage(16787, "坚壁清野·直伤", { 43, 2, 319, 0, 0, 0, true, { .pve = 174 } });
    b.damage(16794, "坚壁清野·残留", { 25, 2, 295, 0, 0, 0, true, { .pve = 174 } });
    b.cast("坚壁清野", 480, 24, 0, { .resource = 2, .cost = 10, .required = b.buff("雪絮金屏") },
           { b.gain(2, -10), b.later("坚壁清野·直伤", 2) });
    b.repeat_hits("坚壁清野", "坚壁清野·残留", 16, 16, 10);
    b.damage(16615, "醉斩白蛇", { 25, 2, 191, 0, 0, 0, true, { .pve = 174 } });
    b.cast("醉斩白蛇", 240, 24, 0, { .resource = 2, .cost = 20, .required = b.buff("雪絮金屏"), .pulses = 4, .spacing = 5, .channel_duration = 21, .weapon_free_cost = b.buff("悬翎在握") }, { b.hit("醉斩白蛇") });
    b.on_cast("醉斩白蛇", { b.when("悬翎在握", b.gain(2, -20), true) });
    b.cast("擒龙六斩", 0, 24, 0, { .resource = 1, .cost = 5, .required = b.buff("松烟竹雾"), .choice = "龙息" },
           { b.gain(1, -5), b.apply("擒龙连段"), b.apply("叠霜"), b.apply("龙息"), b.selected("擒龙六斩·回狂意", b.gain(0, 9)), b.selected("擒龙六斩·回气劲", b.gain(2, 9)) });
    constexpr std::string_view slash[] = { "惊燕式·一", "惊燕式·二", "逐鹰式·一", "逐鹰式·二", "控鹤式·一", "控鹤式·二",
                                           "起凤式·一", "起凤式·二", "腾蛟式·一", "腾蛟式·二", "擒龙式·一", "擒龙式·二" };
    constexpr int channel[] = { 157, 196, 157, 235, 196, 255, 235, 294, 275, 314, 275, 353 };
    b.recipe("擒龙六斩", "擒龙六斩·回狂意", {}, {});
    for (int i = 0; i < 12; ++i) {
        const auto hit = slash[i];
        b.damage(16933 + i, hit, { i < 10 ? 8 : 9, 1, double(channel[i]), 1024, 0, 0, true, { .pve = 174 } });
        b.after_cast("擒龙六斩", { b.stacks("擒龙连段", i / 2 + 1, b.hit(hit)) });
        b.recipe("擒龙六斩", "擒龙六斩·5%伤害", { .damage = 51 }, { hit });
        b.recipe("擒龙六斩", "擒龙六斩·回气劲", { .damage = 31 }, { hit });
        b.recipe("擒龙六斩", "擒龙六斩·4%会心", { .critical = 400 }, { hit });
        b.recipe("擒龙六斩", "擒龙六斩·4%伤害", { .damage = 41 }, { hit });
        for (int layers = 2; layers <= 6; ++layers) {
            const int bonus = (layers - 1) * 154;
            b.modifier(hit, { b.buff("叠霜"), layers, { .fragile = bonus } });
            b.modifier(hit, { b.buff("叠霜"), layers, { .fragile = bonus }, "霜云" });
        }
    }
    for (int stage : { 2, 4, 6 }) b.after_cast("擒龙六斩", { b.stacks("擒龙连段", stage, b.apply("碎江天机会")) });
    b.after_cast("擒龙六斩", { b.stacks("擒龙连段", 6, b.remove("叠霜", 6)), b.stacks("叠霜", 6, b.remove("叠霜", 6)),
                              b.stacks("擒龙连段", 6, b.remove("擒龙连段", 6)) });
    for (auto n : { "碎江天", "闹须弥", "龙骧虎步" }) b.after_cast(n, { b.remove("龙息") });
    b.recipe("碎江天", "碎江天·5%伤害", { .damage = 51 }, { "碎江天" });
    b.recipe("碎江天", "碎江天·4%会心", { .critical = 400 }, { "碎江天" });
    b.recipe("碎江天", "碎江天·4%伤害", { .damage = 41 }, { "碎江天" });
    b.recipe("碎江天", "碎江天·3%伤害", { .damage = 31 }, { "碎江天" });
    b.recipe("醉斩白蛇", "醉斩白蛇·5%伤害", { .damage = 51 }, { "醉斩白蛇" });
    b.recipe("醉斩白蛇", "醉斩白蛇·4%会心", { .critical = 400 }, { "醉斩白蛇" });
    b.recipe("醉斩白蛇", "醉斩白蛇·4%伤害", { .damage = 41 }, { "醉斩白蛇" });
    b.recipe("醉斩白蛇", "醉斩白蛇·3%伤害", { .damage = 31 }, { "醉斩白蛇" });
    for (auto n : { "上将军印·连续5", "上将军印·连续6" }) b.inherit_recipes(n, "上将军印");
    b.damage(16820, "霜风刀法", { 0, 0, 24, 1024, 0, 0, true, { .pve = 174 } });
    b.state({ 0, "霜风刀法计时", 0, 24, 1 }, false, true, true);
    b.pulse("霜风刀法计时", { b.hit("霜风刀法") });
    for (auto n : { "闹须弥", "龙骧虎步", "破釜沉舟", "上将军印", "擒龙六斩", "碎江天", "坚壁清野", "醉斩白蛇", "雷走风切", "刀啸风吟", "西楚悲歌" }) {
        b.on_cast(n, { b.when("霜风刀法计时", b.hit("霜风刀法"), true), b.when("霜风刀法计时", b.apply("霜风刀法计时"), true) });
    }
    b.adjust([](Prepared &p) {
        const auto &book = *p.config.book;
        const auto aura = [&](std::string_view n) { return static_cast<Slot>(std::ranges::find(book.buffs, n, &BuffDescriptor::name) - book.buffs.begin()); };
        if (p.config.precombat) p.opening = { { Op::Remove, aura("秀明尘身") }, { Op::Add, aura("松烟竹雾") } };
        for (size_t i = 0; i < book.hits.size(); ++i) {
            if (!book.hits[i].channel) continue;
            auto &mods = p.modifiers[i];
            for (const auto &[name, stats] : std::initializer_list<std::pair<std::string_view, Stats>>{
                     { "斩狂枭", { .damage = 307 } }, { "疏狂", { .attack = 307, .strain_rating = 154 } },
                     { "临江", { .attack = 154 } }, { "冥鼓", { .ignore = 512 } } }) {
                const auto slot = aura(name);
                if (std::ranges::none_of(mods, [&](const Modifier &m) { return m.aura == slot; })) mods.push_back({ slot, 0, stats });
            }
        }
    });
    b.state({34119, "沉舟", 288, 48, 1}, true, true, true);
    b.dot("沉舟", "沉舟(DOT)");
    b.cast("使用橙武", 800, 0, 0, {.choice = Legendary}, {b.reset("上将军印"), b.reset("醉斩白蛇"), b.reset("破釜沉舟"),
            b.gain(0, 100), b.gain(1, 100), b.gain(2, 100), b.apply("悬翎在握"), b.apply("橙武破釜", 3), b.apply("四面楚歌可用")});
    b.no_cooldown_during("破釜沉舟", "橙武破釜");
    b.no_cooldown_during("醉斩白蛇", "悬翎在握");
    b.modifier("醉斩白蛇", {b.buff("悬翎在握"), 0, {.damage = 1485}});
    b.after_cast("破釜沉舟", {b.when("橙武破釜", b.hit("背水沉舟")), b.when("橙武破釜", b.apply("沉舟")), b.remove("橙武破釜")});
    b.cast("四面楚歌", 0, 48, 0, {.required = b.buff("四面楚歌可用"), .choice = Legendary, .lock = 48},
           {b.remove("四面楚歌可用"), b.later("霜云", 2), b.later("霜云", 18), b.later("霜云", 30), b.later("霜云", 40), b.later("霜云", 47)});
    b.legendary_damage({ "醉斩白蛇" });
    return b.finish();
}
