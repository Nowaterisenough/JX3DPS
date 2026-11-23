// Native event rules for 毒经. Timing is expressed in frames.
static Book dj()
{
    Builder b("dj", "毒经", R"(# 魔盒连缘爆发与循环 https://www.jx3box.com/macro/107317
/cast 使用风腰坠
/cast 使用橙武
/cast [nobuff:凤凰蛊] 凤凰蛊
/cast [nobuff:嗜蛊|buff:引魂&buff:令怖<2&skill_cd:连缘蛊<2] 灵蛊
/cast 灵蛇引
/cast [buff:不鸣] 蝎心
/cast [tbuff:蛇影DOT&tbuff:百足DOT&bufftime:引魂<11] 连缘蛊
/cast [buff:令怖>=6|nobuff:引魂&cd:灵蛇引<7] 千丝
/cast [nobuff:引魂&tbufftime:蛇影DOT<4&cd:灵蛇引>4] 蛇影
/cast [last_skill:蛇影|tnobuff:百足DOT&cd:灵蛇引>4] 百足
/cast [tbufftime:蟾啸DOT<2&nobuff:引魂&cd:灵蛇引>4] 蟾啸
/cast 蝎心
)");
    b.preset(2301,
             { 6172, 1680, 57, 1831, 208, 1197, 4212, 0, 1092 },
             { "百足·5%伤害",
               "千丝·5%伤害",
               "千丝·4%伤害",
               "千丝·3%伤害",
               "蛇影·4%会心",
               "蛇影·4%伤害",
               "蛇影·5%伤害",
               "蝎心·4%伤害",
               "蝎心·3%伤害",
               "百足·10%伤害（一）",
               "百足·10%伤害（二）",
               "百足·调息减1秒",
               "千丝·调息减1秒",
               "蛇影·额外一跳",
               "蝎心·运功减2帧（一）",
               "蝎心·运功减2帧（二）",
               "灵蛊·10%伤害（一）",
               "灵蛊·10%伤害（二）",
               "灵蛊·10%伤害（三）" });
    b.choices({ "连缘蛊", "残香", "不鸣", "嗜蛊", "荒息", "蛇悉", "令怖", "释灵", "蛛魄", "蛊虫狂暴", "悸心" }, {}, {});
    b.default_talents({ "连缘蛊", "残香", "不鸣", "嗜蛊", "荒息", "蛇悉", "令怖", "释灵", "蛛魄" });
    b.state({ 0, "蛇影DOT", 192, 32, 1 }, true, true, true);
    b.state({ 0, "百足DOT", 288, 32, 1 }, true, true, true);
    b.state({ 0, "蟾啸DOT", 224, 32, 1 }, true, true, true);
    b.state({ 0, "蛊虫狂暴", 256, 0, 1 }, false, false, false);
    b.state({ 0, "灵蛇存在", 4800, 0, 1 }, false, false, false);
    b.state({ 0, "灵蛇·幻击", 0, 192, 1 }, false, false, false);
    b.state({ 0, "引魂", 192, 0, 1 }, false, false, false);
    b.state({ 0, "灵蛇·普攻", 0, 64, 1 }, false);
    b.initial("灵蛇·普攻");
    b.initial("灵蛇·幻击");
    b.state({ 0, "圣蝎存在", 4800, 0, 1 }, false);
    b.state({ 0, "蛊虫存在", 4800, 0, 1 }, false);
    b.state({ 0, "圣蝎献祭", 128, 0, 1 }, false);
    b.state({ 0, "蛇悉", 96, 0, 2 }, false);
    b.state({ 0, "不鸣", 480, 0, 1 }, false);
    b.state({ 0, "嗜蛊", 640, 0, 1 }, false);
    b.state({ 0, "凤凰蛊", 2880, 0, 1 }, false);
    b.state({ 0, "荒息计数", 0, 0, 2 }, false);
    b.state({ 0, "令怖", 960, 0, 8 }, false);
    b.state({ 0, "残香", 480, 0, 2 }, false);
    b.state({ 0, "五毒齐备", 0, 0, 1 }, false);
    b.state({ 0, "释灵DOT", 128, 16, 1 }, true, false, true);
    b.damage(0,
             "蛇影(DOT)",
             {
                 1,
                 0,
                 573,
                 0,
                 6,
                 32,
                 false,
                 { .pve = 266 },
                 { { b.buff("蛊虫狂暴"), 0, { .damage = 102 }, "" }, { b.buff("引魂"), 0, { .attack = 512 }, "" } }
    });
    b.damage(0,
             "百足(DOT)",
             {
                 1,
                 0,
                 1222,
                 0,
                 9,
                 32,
                 false,
                 { .pve = 266 },
                 { { b.buff("蛊虫狂暴"), 0, { .damage = 102 }, "" }, { b.buff("引魂"), 0, { .attack = 512 }, "" } }
    });
    b.damage(0,
             "蟾啸(DOT)",
             {
                 1,
                 0,
                 720,
                 0,
                 7,
                 32,
                 false,
                 { .pve = 266 },
                 { { b.buff("蛊虫狂暴"), 0, { .damage = 102 }, "" }, { b.buff("引魂"), 0, { .attack = 512 }, "" } }
    });
    b.damage(2472,
             "攻击",
             {
                 4,
                 1,
                 258,
                 0,
                 0,
                 0,
                 false,
                 {},
                 { { b.buff("蛊虫狂暴"), 0, { .damage = 512 }, "" }, { b.buff("引魂"), 0, { .attack = 512 }, "" } }
    });
    b.pet_attack("攻击", 8700);
    b.damage(2477,
             "幻击",
             {
                 19,
                 2,
                 345,
                 0,
                 0,
                 0,
                 false,
                 {},
                 { { b.buff("蛊虫狂暴"), 0, { .damage = 512 }, "" }, { b.buff("引魂"), 0, { .attack = 512 }, "" } }
    });
    b.pet_attack("幻击", 8700);
    b.damage(21821,
             "千丝",
             {
                 45,
                 1,
                 1792,
                 0,
                 0,
                 0,
                 false,
                 { .pve = 266 },
                 { { b.buff("蛊虫狂暴"), 0, { .damage = 102 }, "" }, { b.buff("引魂"), 0, { .attack = 512 }, "" } }
    });
    b.damage(21303,
             "蛇影",
             {
                 20,
                 2,
                 664,
                 0,
                 0,
                 0,
                 false,
                 { .pve = 266 },
                 { { b.buff("蛊虫狂暴"), 0, { .damage = 102 }, "" }, { b.buff("引魂"), 0, { .attack = 512 }, "" } }
    });
    b.damage(13472,
             "百足",
             {
                 105,
                 10,
                 758,
                 0,
                 0,
                 0,
                 false,
                 { .pve = 266 },
                 { { b.buff("蛊虫狂暴"), 0, { .damage = 102 }, "" }, { b.buff("引魂"), 0, { .attack = 512 }, "" } }
    });
    b.damage(9331,
             "蝎心",
             {
                 51,
                 3,
                 616,
                 0,
                 0,
                 0,
                 false,
                 { .pve = 266 },
                 { { b.buff("蛊虫狂暴"), 0, { .damage = 102 }, "" }, { b.buff("引魂"), 0, { .attack = 512 }, "" } }
    });
    b.dot("蛇影DOT", "蛇影(DOT)");
    b.dot("百足DOT", "百足(DOT)");
    b.dot("蟾啸DOT", "蟾啸(DOT)");
    b.pulse("灵蛇·普攻", { b.when("灵蛇存在", b.hit("攻击")) });
    b.pulse("灵蛇·幻击", { b.when("灵蛇存在", b.hit("幻击")) });
    b.cast("灵蛇引",
           432,
           24,
           0,
           { .gcd_group = 1 },
           { b.remove("圣蝎存在"), b.apply("蛊虫存在"), b.apply("灵蛇存在"), b.apply("引魂"), b.selected("不鸣", b.apply("不鸣")) });
    b.cast("蛊虫狂暴", 1440, 24, 0, { .choice = "蛊虫狂暴", .gcd_group = 2 }, { b.apply("蛊虫狂暴") });
    b.cast("灵蛊", 320, 16, 0, { .charges = 3, .gcd_group = 3 }, { b.selected("嗜蛊", b.apply("嗜蛊")), b.selected("令怖", b.apply("令怖")) });
    b.cast("凤凰蛊", 480, 24, 0, { .choice = "荒息" }, { b.apply("凤凰蛊") });
    b.cast("圣蝎引", 432, 24, 0, {}, { b.remove("灵蛇存在"), b.apply("蛊虫存在"), b.apply("圣蝎存在") });
    b.cast("蛊虫献祭",
           480,
           24,
           0,
           { .required = b.buff("蛊虫存在"), .gcd_group = 4, .capture = b.buff("圣蝎存在") },
           { b.when("圣蝎存在", b.apply("圣蝎献祭")), b.remove("圣蝎存在"), b.remove("灵蛇存在"), b.remove("蛊虫存在") });
    b.variant("圣蝎引",
              "悸心",
              432,
              0,
              {},
              { b.remove("灵蛇存在"), b.remove("蛊虫存在"), b.remove("圣蝎存在"), b.apply("圣蝎献祭"), b.shorten("圣蝎引", 48) });
    b.variant("蛊虫献祭",
              "悸心",
              480,
              0,
              { .required = b.buff("蛊虫存在"), .gcd_group = 4 },
              { b.remove("圣蝎存在"), b.remove("灵蛇存在"), b.remove("蛊虫存在") });
    b.cast("千丝", 128, 24, 0, {}, { b.selected("蛛魄", b.later("千丝", 73)) });
    b.cast("蛇影", 0, 24, 0, {}, { b.apply("蛇影DOT") });
    b.variant("蛇影", "蛇悉", 96, 20, { .fixed_cast_time = true }, { b.hit("蛇影"), b.apply("蛇影DOT"), b.apply("蛇悉", 2) });
    b.cast("百足", 288, 24, 0, {}, { b.hit("百足"), b.apply("百足DOT") });
    b.cast("蟾啸", 192, 24, 0, {}, { b.apply("蟾啸DOT") });
    b.cast("蝎心", 0, 24, 28, { .instant = b.buff("蛇悉") }, { b.hit("蝎心"), b.remove("蛇悉") });
    b.recipe("千丝", "千丝·3%伤害", { .damage = 31 }, { "千丝" });
    b.recipe("千丝", "千丝·4%伤害", { .damage = 41 }, { "千丝" });
    b.recipe("千丝", "千丝·5%伤害", { .damage = 51 }, { "千丝" });
    b.recipe("百足", "百足·5%伤害", {}, { "百足", "百足(DOT)" }, 1.05);
    b.recipe("蛇影", "蛇影·2%会心", { .critical = 200 }, { "蛇影", "蛇影(DOT)" });
    b.recipe("蛇影", "蛇影·3%会心", { .critical = 300 }, { "蛇影", "蛇影(DOT)" });
    b.recipe("蛇影", "蛇影·4%会心", { .critical = 400 }, { "蛇影", "蛇影(DOT)" });
    b.recipe("蛇影", "蛇影·4%伤害", {}, { "蛇影", "蛇影(DOT)" }, 1.04);
    b.recipe("蛇影", "蛇影·5%伤害", {}, { "蛇影", "蛇影(DOT)" }, 1.05);
    b.recipe("蝎心", "蝎心·2%会心", { .critical = 200 }, { "蝎心" });
    b.recipe("蝎心", "蝎心·3%会心", { .critical = 300 }, { "蝎心" });
    b.recipe("蝎心", "蝎心·3%伤害", { .damage = 31 }, { "蝎心" });
    b.recipe("蝎心", "蝎心·4%伤害", { .damage = 41 }, { "蝎心" });
    b.passive("蝎心", "残香", { .damage = 205 });
    b.set_bonus({}, { "蛇影", "蛇影(DOT)" }, 1.1);
    b.set_bonus({ .damage = 204 }, { "蝎心" });
    b.state({ 0, "连缘起手毒数", 0, 0, 4 }, false);
    b.state({ 0, "连缘结束毒数", 0, 0, 4 }, false);
    b.damage(25044, "连缘蛊", { 4, 1, 230, 0, 0, 0, false, { .pve = 266 }, {} });
    for (int i = 1; i <= 4; ++i) {
        b.modifier("连缘蛊", { b.buff("连缘起手毒数"), i, { .damage = (8192 * i + 5) / 10 }, {} });
    }
    constexpr std::array<std::string_view, 5> ends{ "连缘蛊·额外·零", "连缘蛊·额外·一", "连缘蛊·额外·二", "连缘蛊·额外·三", "连缘蛊·额外·四" };
    constexpr std::array<int, 5> fixed{ 40, 66, 89, 111, 133 }, spread{ 4, 7, 9, 11, 14 }, cof{ 243, 487, 731, 975, 1219 };
    for (int i = 0; i < 5; ++i) {
        b.damage(0, ends[i], { fixed[i], spread[i], cof[i], 0, 0, 0, false, { .pve = 266 }, {} });
    }
    b.cast("连缘蛊", 432, 24, 0, { .choice = "连缘蛊", .pulses = 5, .spacing = 5, .distribute_pulses = true }, { b.hit("连缘蛊") });
    b.on_cast("连缘蛊", { b.remove("连缘起手毒数", 4) });
    b.after_cast("连缘蛊", { b.remove("连缘结束毒数", 4) });
    for (auto dot : { "蛇影DOT", "百足DOT", "蟾啸DOT", "释灵DOT" }) {
        b.on_cast("连缘蛊", { b.when(dot, b.apply("连缘起手毒数")) });
        b.after_cast("连缘蛊", { b.when(dot, b.apply("连缘结束毒数")) });
    }
    b.after_cast("连缘蛊", { b.when("连缘结束毒数", b.hit(ends[0]), true) });
    for (int i = 1; i <= 4; ++i) {
        b.after_cast("连缘蛊", { b.stacks("连缘结束毒数", i, b.hit(ends[i])) });
    }
    b.damage(37959, "不鸣", { 15, 1, 900, 0, 0, 0, false, { .pve = 266 }, {} });
    b.damage(38456, "虫魄", { 20, 2, 240, 0, 0, 0, false, { .pve = 266 }, {} });
    b.damage(42295, "令怖", { 2, 1, 64, 0, 0, 0, false, { .pve = 266 }, {} });
    b.damage(25019, "荒息", { 4, 1, 150, 0, 0, 0, false, { .pve = 225 }, {} });
    b.damage(38456, "残香", { 22, 2, 180, 0, 0, 0, false, { .pve = 266 }, {} });
    b.damage(37352, "释灵(DOT)", { 1, 0, 762, 0, 8, 16, false, { .pve = 266 }, {} });
    b.dot("释灵DOT", "释灵(DOT)");
    b.passive("令怖", {}, { .pve = 1843 });
    b.pet_attack("荒息", 8700);
    b.modifier("荒息", { b.buff("引魂"), 0, { .attack = 512 } });
    b.modifier("荒息", { b.buff("蛊虫狂暴"), 0, { .damage = 512 } });
    // Pets inherit their owner's attack and strain during 嗜蛊; the owner's shield ignore is personal.
    for (auto pet : { "攻击", "幻击", "荒息" }) {
        b.modifier(pet,
                   {
                       b.buff("嗜蛊"),
                       0,
                       { .attack = 154, .strain_rating = 307 }
        });
    }
    b.passive("攻击", {}, { .pve = 225 });
    b.passive("幻击", {}, { .pve = 225 });
    b.ranks("令怖", { 64, 128, 192, 256, 320, 384, 448, 512 });
    b.on_hit("不鸣", { b.apply("蛇影DOT"), b.apply("百足DOT"), b.apply("蟾啸DOT") });
    b.after_cast("蝎心",
                 { b.when("不鸣", b.hit("不鸣")),
                   b.remove("不鸣"),
                   b.selected("残香", b.extra("蛇影DOT")),
                   b.selected("残香", b.extra("百足DOT")),
                   b.selected("残香", b.extra("蟾啸DOT")),
                   b.selected("残香", b.apply("残香")),
                   b.selected("释灵", b.when("蛇影DOT", b.apply("释灵DOT"))) });
    for (auto spell : { "蛇影", "千丝", "百足", "蟾啸" }) {
        b.after_cast(spell, { b.when("残香", b.shorten(spell, 96)), b.when("残香", b.hit("残香")), b.remove("残香") });
    }
    // Five distinct poisons make one discharge. Repeated scorpion casts preserve the partial set.
    const auto mark = [&](int id) {
        return Step{ .op = Op::RecordDistinct, .slot = b.buff("五毒齐备"), .amount = id, .maximum = 5 };
    };
    const auto discharge = [&](auto append) {
        append(b.when("五毒齐备", b.hit("虫魄", 5)));
        for (int tick = 1; tick <= 5; ++tick) {
            auto follow  = b.later("令怖", tick * 4);
            follow.extra = b.buff("令怖");
            append(b.when("五毒齐备", follow));
        }
        append(b.when("五毒齐备", b.remove("令怖", 8)));
        append(b.remove("五毒齐备"));
    };
    b.state({0, "承瞑愿", 96, 0, 1}, false);
    b.secondary_damage(39036, "赤蝎", {0, 21, 2034, 0, 0, 0, false, {.pve = 266}});
    b.cast("使用橙武", 800, 0, 0, {.choice = Legendary}, {b.apply("承瞑愿")});
    constexpr std::array<std::string_view, 5> poisons{ "蝎心", "蛇影", "百足", "蟾啸", "千丝" };
    for (int i = 0; i < 5; ++i) {
        const auto n = poisons[i];
        b.after_cast(n, { mark(i) });
        discharge([&](Step effect) { b.after_cast(n, { effect }); });
        if (i >= 1 && i <= 3) {
            b.on_hit("不鸣", { mark(i) });
            discharge([&](Step effect) { b.on_hit("不鸣", { effect }); });
        }
    }
    for (auto dot : {"蛇影DOT", "百足DOT", "蟾啸DOT"}) b.after_cast("蝎心", {b.when("承瞑愿", b.refresh_existing(dot))});
    b.after_cast("蝎心", {b.when("承瞑愿", b.hit("赤蝎")), b.when("承瞑愿", {.op = Op::RecordPoisonFromDots, .slot = b.buff("五毒齐备"), .extra = b.buff("蛇影DOT"), .maximum = 5})});
    discharge([&](Step effect) { b.after_cast("蝎心", {effect}); });
    for (auto n : { "蝎心", "蛇影", "百足", "蟾啸", "千丝", "灵蛊" }) {
        b.after_cast(n,
                     { b.when("凤凰蛊", b.apply("荒息计数")),
                       b.at_least("荒息计数", 2, b.hit("荒息")),
                       b.at_least("荒息计数", 2, b.remove("荒息计数", 2)) });
    }
    for (auto n : { "连缘蛊", "连缘蛊·额外·零", "连缘蛊·额外·一", "连缘蛊·额外·二", "连缘蛊·额外·三", "连缘蛊·额外·四" })
    {
        b.on_hit(n,
                 { b.selected("令怖", b.apply("令怖")),
                   b.when("凤凰蛊", b.apply("荒息计数")),
                   b.at_least("荒息计数", 2, b.hit("荒息")),
                   b.at_least("荒息计数", 2, b.remove("荒息计数", 2)) });
    }
    // A scorpion sacrifice accelerates only the portion of an existing recovery that overlaps its window.
    for (auto n : { "圣蝎引", "灵蛇引", "蛊虫献祭", "灵蛊", "千丝", "百足", "蛇影", "蟾啸", "连缘蛊", "蛊虫狂暴", "凤凰蛊" })
    {
        Step refund{ .op = Op::SuppressCooldown, .slot = b.effect(n), .amount = 307, .extra = b.buff("圣蝎献祭") };
        b.after_cast("蛊虫献祭", { b.captured(refund) });
        b.after_cast("圣蝎引", { b.selected("悸心", refund) });
        if (n != std::string_view{ "蛊虫献祭" } && n != std::string_view{ "圣蝎引" }) {
            b.on_cast(n, { b.when("圣蝎献祭", refund) });
        }
    }
    for (auto n :
         { "百足(DOT)", "蛇影(DOT)", "蟾啸(DOT)", "蝎心", "千丝", "蛇影", "百足", "不鸣", "令怖", "残香", "虫魄", "释灵(DOT)", "连缘蛊", "连缘蛊·额外·零", "连缘蛊·额外·一", "连缘蛊·额外·二", "连缘蛊·额外·三", "连缘蛊·额外·四", "赤蝎" })
    {
        b.modifier(n,
                   {
                       b.buff("嗜蛊"),
                       0,
                       { .attack = 154, .ignore = 614, .strain_rating = 307 }
        });
    }
    for (auto n : { "不鸣", "令怖", "残香", "虫魄", "释灵(DOT)", "连缘蛊", "连缘蛊·额外·零", "连缘蛊·额外·一", "连缘蛊·额外·二", "连缘蛊·额外·三", "连缘蛊·额外·四", "赤蝎" })
    {
        b.modifier(n, { b.buff("引魂"), 0, { .attack = 512 } });
        b.modifier(n, { b.buff("蛊虫狂暴"), 0, { .damage = 102 } });
    }
    b.recipe("百足", "百足·10%伤害（一）", {}, { "百足", "百足(DOT)" }, 1.10);
    b.recipe("百足", "百足·10%伤害（二）", {}, { "百足", "百足(DOT)" }, 1.10);
    b.recipe("百足", "百足·调息减1秒", {}, {});
    b.recipe("千丝", "千丝·调息减1秒", {}, {});
    b.recipe("蛇影", "蛇影·额外一跳", {}, {});
    b.recipe("蝎心", "蝎心·运功减2帧（一）", {}, {});
    b.recipe("蝎心", "蝎心·运功减2帧（二）", {}, {});
    // The preset equips these three recipes. Its 灵蛊 applies a buff and has no direct damage event.
    b.recipe("灵蛊", "灵蛊·10%伤害（一）", {}, {});
    b.recipe("灵蛊", "灵蛊·10%伤害（二）", {}, {});
    b.recipe("灵蛊", "灵蛊·10%伤害（三）", {}, {});
    b.adjust([](Prepared &p) {
        auto &bk   = *p.config.book;
        auto  slot = [&](std::string_view n) {
            return std::ranges::find(bk.skills, n, &SkillDescriptor::name) - bk.skills.begin();
        };
        auto aura = [&](std::string_view n) {
            return static_cast<Slot>(std::ranges::find(bk.buffs, n, &BuffDescriptor::name) - bk.buffs.begin());
        };
        auto recipe = [&](std::string_view n) {
            return std::ranges::contains(p.config.recipes, n);
        };
        auto talent = [&](std::string_view n) {
            return std::ranges::contains(p.config.talents, n);
        };
        p.skills[slot("蝎心")].cast_time -= 2 * (recipe("蝎心·运功减2帧（一）") + recipe("蝎心·运功减2帧（二）"));
        p.skills[slot("百足")].cooldown  -= 16 * recipe("百足·调息减1秒");
        p.skills[slot("千丝")].cooldown  += 112 * talent("蛛魄") - 16 * recipe("千丝·调息减1秒");
        p.dot_ticks[aura("蛇影DOT")]     += 2 * talent("蛇悉") + recipe("蛇影·额外一跳");
        if (p.config.precombat && talent("荒息")) {
            p.opening.push_back({ .op = Op::Add, .slot = aura("凤凰蛊"), .duration = 2879 });
        }
    });
    b.legendary_damage({ "蝎心", "百足", "百足(DOT)" });
    return b.finish();
}
