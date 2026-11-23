// Native event rules for 天罗诡道. Timing is expressed in frames.
static Book tlgd()
{
    Builder b("tlgd", "天罗诡道", R"(# 魔盒宏适配 https://www.jx3box.com/macro/109864
/cast 使用风腰坠
/cast 使用橙武
/cast [nobuff:机关存在] 千机变
/cast [tnobuff:重弩&skill_notin_cd:鬼斧神工] 重弩
/cast [tnobuff:重弩&buff:鬼斧弹药] 重弩
/cast [nobuff:鬼斧弹药&tnobuff:连弩&skill_cd:鬼斧神工>3] 连弩
/cast [buff:扬威] 心无旁骛
/cast 鬼斧神工
/cast [tnobuff:化血] 天女散花
/cast 天绝地灭
/cast [tbufftime:天绝地灭>=2.1875&tbufftime:化血>=2.1875&skill_energy:暴雨梨花针>=2] 暴雨梨花针
/cast [buff:扬威|bufftime:专注>17] 星离雨散
/cast [tbufftime:化血<4] 天女散花
/cast [tbufftime:天绝地灭>=2.1875] 暴雨梨花针
/cast [bufftime:专注>13] 暗藏杀机
/cast [神机>=50] 蚀肌弹
)");
    b.preset(2300, { 6353, 1219, 113, 2168, 208, 1290, 4212, 0, 10 }, { "蚀肌弹·5%伤害", "蚀肌弹·4%伤害", "蚀肌弹·运功减2帧（一）", "蚀肌弹·运功减2帧（二）",
               "暴雨梨花针·4%会心", "暴雨梨花针·5%伤害", "暴雨梨花针·4%伤害", "暴雨梨花针·气魄",
               "暗藏杀机·5%伤害", "暗藏杀机·4%伤害", "暗藏杀机·3%伤害", "千机变·人偶图绝章",
               "天绝地灭·5%伤害", "天绝地灭·4%伤害", "天绝地灭·3%伤害" });
    b.choices({ "化血迷心", "千机之威", "神威穿彻", "天风汲雨", "凝气淬机", "擘两分星", "横断天枢", "尽刑彻毒", "千秋万劫", "秋风散影", "星离雨散" }, {}, {});
    b.default_talents({ "横断天枢", "千机之威", "化血迷心", "凝气淬机", "擘两分星", "星离雨散", "秋风散影", "神威穿彻", "天风汲雨" });
    b.resource({ "神机", 100, 0, 100, 16, 10 });
    b.state({ 0, "机关存在", 3840, 16, 1 }, false, false, false);
    b.state({ 0, "连弩", 1920, 24, 1 }, true, false, false);
    b.state({ 0, "重弩", 1920, 48, 1 }, true, false, false);
    b.state({ 0, "鸩羽", 288, 0, 1 }, true, false, false);
    b.state({ 0, "扬威", 240, 0, 1 }, false, false, false);
    b.state({ 0, "鬼斧弹药", 1920, 0, 50 }, false, false, false);
    b.state({ 0, "化血", 384, 48, 1 }, true, true, true);
    b.secondary_damage(
        3401,
        "连弩·一",
        {
            3,
            1,
            306,
            0,
            0,
            0,
            false,
            { .pve = 20 },
            { { b.buff("机关存在"), 0, { .attack = 154, .ignore = 389 }, "千机之威" }, { b.buff("扬威"), 0, { .attack = 512 }, "" } }
    });
    b.secondary_damage(
        3404,
        "连弩·二",
        {
            5,
            1,
            540,
            0,
            0,
            0,
            false,
            { .pve = 20 },
            { { b.buff("机关存在"), 0, { .attack = 154, .ignore = 389 }, "千机之威" }, { b.buff("扬威"), 0, { .attack = 512 }, "" } }
    });
    b.secondary_damage(
        3819,
        "重弩·一",
        {
            5,
            0,
            415,
            0,
            0,
            0,
            false,
            { .pve = 20 },
            { { b.buff("机关存在"), 0, { .attack = 154, .ignore = 389 }, "千机之威" }, { b.buff("扬威"), 0, { .attack = 512 }, "" } }
    });
    b.secondary_damage(
        3824,
        "重弩·二",
        {
            7,
            0,
            745,
            0,
            0,
            0,
            false,
            { .pve = 20 },
            { { b.buff("机关存在"), 0, { .attack = 154, .ignore = 389 }, "千机之威" }, { b.buff("扬威"), 0, { .attack = 512 }, "" } }
    });
    b.ranks("重弩·一", { 415, 415, 415, 665 });
    b.ranks("重弩·二", { 745, 745, 745, 1193 });
    b.rank_bases("重弩·一", { {5,0}, {13,1}, {5,0}, {10,1} });
    b.rank_bases("重弩·二", { {7,0}, {19,1}, {7,0}, {15,1} });
    b.damage(0,
             "化血(DOT)",
             {
                 1,
                 0,
                 382,
                 0,
                 8,
                 48,
                 false,
                 { .pve = 20 },
                 { { b.buff("机关存在"), 0, { .attack = 154, .ignore = 389 }, "千机之威" },
                   { b.buff("扬威"), 0, { .attack = 512 }, "" } }
    });
    b.damage(3223,
             "孔雀翎",
             {
                 34,
                 3,
                 130,
                 0,
                 0,
                 0,
                 false,
                 { .pve = 20 },
                 { { b.buff("机关存在"), 0, { .attack = 154, .ignore = 389 }, "千机之威" },
                   { b.buff("扬威"), 0, { .attack = 512 }, "" } }
    });
    b.damage(3304,
             "化血迷心",
             {
                 2,
                 1,
                 388,
                 0,
                 0,
                 0,
                 false,
                 { .pve = 20 },
                 { { b.buff("机关存在"), 0, { .attack = 154, .ignore = 389 }, "千机之威" },
                   { b.buff("扬威"), 0, { .attack = 512 }, "" } }
    });
    b.damage(3105,
             "蚀肌弹",
             {
                 18,
                 2,
                 494,
                 1024,
                 0,
                 0,
                 false,
                 { .pve = 20 },
                 { { b.buff("机关存在"), 0, { .attack = 154, .ignore = 389 }, "千机之威" },
                   { b.buff("扬威"), 0, { .attack = 512 }, "" } }
    });
    b.pulse("连弩",
            { b.when("鬼斧弹药", b.hit("连弩·一", 1), true),
              b.when("鬼斧弹药", b.hit("连弩·二", 1), false),
              b.remove("鬼斧弹药", 1),
              b.gain(0, 10) });
    b.dot("化血", "化血(DOT)");
    b.cast("千机变", 128, 24, 16, { .resource = 0, .cost = 30 }, { b.gain(0, -30), b.apply("机关存在", 1) });
    b.cast("连弩", 48, 0, 0, { .required = b.buff("机关存在") }, { b.remove("重弩", 1000), b.apply("连弩", 1) });
    b.cast("重弩", 48, 0, 0, { .required = b.buff("机关存在") }, { b.remove("连弩", 1000), b.apply("重弩", 1) });
    b.cast("孔雀翎", 288, 24, 0, { .resource = 0, .cost = 20 }, { b.gain(0, -20), b.apply("鸩羽", 1), b.hit("孔雀翎", 1) });
    b.state({ 0, "化血急毒", 384, 16, 1 }, true, false, true);
    b.cast("蚀肌弹",
           0,
           24,
           28,
           { .resource = 0, .cost = 10 },
           { b.when("鸩羽", b.remove("化血急毒")), b.when("鸩羽", b.apply("化血")),
             b.hit("蚀肌弹", 1),
             b.gain(0, -10) });
    b.cast("鬼斧神工", 1440, 41, 24, { .resource = 0, .cost = 20 }, { b.apply("扬威", 1), b.apply("鬼斧弹药", 15), b.gain(0, -20) });
    b.recipe("蚀肌弹", "蚀肌弹·3%会心", { .critical = 300 }, { "蚀肌弹" });
    b.recipe("蚀肌弹", "蚀肌弹·3%伤害", { .damage = 31 }, { "蚀肌弹" });
    b.recipe("蚀肌弹", "蚀肌弹·4%会心", { .critical = 400 }, { "蚀肌弹" });
    b.recipe("蚀肌弹", "蚀肌弹·4%伤害", { .damage = 41 }, { "蚀肌弹" });
    b.recipe("蚀肌弹", "蚀肌弹·5%伤害", { .damage = 51 }, { "蚀肌弹" });
    b.secondary_damage(3480, "暴雨梨花针·额外", {621, 65, 191, 0, 0, 0, false, {.pve = 20}});
    b.damage(3228, "暴雨梨花针", { 52, 5, 165, 0, 0, 0, false, { .pve = 20 }, {} });
    b.cast("暴雨梨花针",
           128,
           24,
           0,
           { .resource = 0, .cost = 30, .pulses = 5, .spacing = 7, .charges = 2, .start = { b.gain(0, -30) } },
           { b.hit("暴雨梨花针") });
    b.set_bonus({ .damage = 102 }, { "连弩·一", "连弩·二", "重弩·一", "重弩·二" });
    b.state({ 0, "天绝布设", 24, 0, 1 }, true);
    b.state({ 0, "天绝地灭", 240, 48, 1 }, true);
    b.state({ 0, "汲雨", 400, 0, 4 }, false);
    b.damage(36502, "天绝地灭", { 10, 5, 70, 0, 0, 0, false, { .pve = 20 }, {} });
    b.damage(42484, "天风汲雨", { 1, 1, 184, 0, 0, 0, false, { .pve = 20 }, {} });
    b.cast("天绝地灭", 320, 24, 0, { .resource = 0, .cost = 40 }, { b.gain(0, -40), b.apply("天绝布设") });
    b.on_expiry("天绝布设", { b.apply("天绝地灭") });
    b.pulse("天绝地灭", { b.hit("天绝地灭") });
    b.on_hit("暴雨梨花针", { b.selected("天风汲雨", b.when("天绝地灭", b.hit("天风汲雨"))) });
    b.on_hit("天风汲雨", { b.apply("汲雨") });
    for (auto hit : { "暴雨梨花针", "暴雨梨花针·额外", "天绝地灭", "天风汲雨" }) {
        b.modifier(hit, { b.buff("机关存在"), 0, { .attack = 154, .ignore = 389 }, "千机之威" });
        b.modifier(hit, { b.buff("扬威"), 0, { .attack = 512 } });
    }
    b.variant("鬼斧神工",
              "神威穿彻",
              800,
              24,
              { .resource = 0, .cost = 20 },
              { b.apply_for("扬威", 160), b.apply("鬼斧弹药", 15), b.gain(0, -20) });
    for (auto n : { "天绝地灭", "天风汲雨", "暴雨梨花针", "暴雨梨花针·额外", "蚀肌弹", "化血(DOT)", "孔雀翎", "化血迷心", "连弩·一", "连弩·二", "重弩·一", "重弩·二" })
    {
        for (int i = 1; i <= 4; ++i) {
            b.modifier(n,
                       {
                           b.buff("汲雨"),
                           i,
                           { .overcome = (i * 1024 + 10) / 20, .strain_rating = (i * 1024 + 10) / 20 },
                           "天风汲雨"
            });
        }
    }
    b.state({ 3468, "心无旁骛", 160, 16, 1 }, false);
    b.state({ 0, "专注", 800, 0, 1 }, false);
    b.cast("心无旁骛", 1200, 0, 0, {}, { b.apply("心无旁骛"), b.gain(0, 100) });
    b.variant("心无旁骛", "凝气淬机", 800, 0, {}, { b.apply("心无旁骛"), b.apply("专注") });
    for (auto n : { "天绝地灭", "天风汲雨", "暴雨梨花针", "暴雨梨花针·额外", "蚀肌弹", "化血(DOT)", "孔雀翎", "化血迷心", "连弩·一", "连弩·二", "重弩·一", "重弩·二" })
    {
        b.modifier(n,
                   {
                       b.buff("心无旁骛"),
                       0,
                       { .critical = 1500, .power = 300 },
                       {}
        });
        b.modifier(n, { b.buff("专注"), 0, { .damage = 102 }, "凝气淬机" });
        b.resource_modifier(n, 0, 0, 79, { .damage = 205 }, "擘两分星");
    }
    b.resource_modifier("天绝地灭", 0, 21, 100, {}, "擘两分星", 1.1);
    b.state({ 0, "机关注能", 0, 0, 100 }, false);

    b.pulse("心无旁骛", { b.gain(0, 10) });
    b.pulse("机关存在", { b.selected("横断天枢", b.at(0, 31, 100, b.apply("机关注能"))), b.selected("横断天枢", b.at(0, 31, 100, b.gain(0, -5))) });
    b.damage(3313, "暗藏杀机", { 0, 5, 698, 0, 0, 0, false, { .pve = 20 } });
    b.cast("暗藏杀机", 128, 24, 0, { .charges = 3, .gcd_group = 2 }, { b.later("暗藏杀机", 24) });
    b.variant("暗藏杀机", "暗藏杀机·立即激活", 128, 0, { .charges = 3, .gcd_group = 2 }, { b.hit("暗藏杀机") });
    b.damage(3393, "天女散花", { 12, 1, 172, 0, 0, 0, false, { .pve = 20 } });
    b.cast("天女散花", 0, 24, 0, { .resource = 0, .cost = 20 }, { b.gain(0, -20), b.apply("化血"), b.later("天女散花", 24) });
    b.modifier("天女散花", { b.buff("化血"), 0, { .final_damage = 1024 } });
    b.damage(42664, "封喉", { 84, 10, 1536, 0, 0, 0, false, { .pve = 20 } });
    b.damage(42664, "星离雨散", { 112, 14, 1536, 0, 0, 0, false, { .pve = 20 } });
    b.cast("星离雨散", 96, 24, 0, { .resource = 0, .cost = 20, .choice = "星离雨散" },
           { b.gain(0, -20), b.hit("封喉"), b.later("星离雨散", 32) });
    b.state({ 0, "开火", 320, 0, 4 }, false);
    b.damage(18677, "千秋万劫", { 4, 4, 378, 0, 0, 0, false, { .pve = 20 } });
    b.cast("开火", 960, 24, 0, { .choice = "千秋万劫" }, { b.apply("开火", 4) });
    b.cast("千秋万劫", 0, 24, 0, { .required = b.buff("开火") }, { b.remove("开火"), b.hit("千秋万劫", 3) });
    b.damage(0, "化血·尽刑(DOT)", { 1, 0, 1148, 0, 24, 16, false, { .pve = 20 } });
    b.dot("化血急毒", "化血·尽刑(DOT)");
    b.on_cast("天女散花", { b.remove("化血急毒") });
    b.modifier("天女散花", { b.buff("化血急毒"), 0, { .final_damage = 1024 } });
    for (auto attack : { "蚀肌弹", "暴雨梨花针" }) {
        b.on_hit(attack, { b.selected("化血迷心", b.when("化血", b.hit("化血迷心"))),
                           b.selected("化血迷心", b.when("化血急毒", b.hit("化血迷心"))) });
    }

    b.on_hit("暗藏杀机", { b.selected("尽刑彻毒", { Op::CompressPeriodic, b.buff("化血"), 1, INVALID_SLOT, false, {}, b.buff("化血急毒") }) });
    b.resource_modifier("暗藏杀机", 0, 21, 100, {}, "擘两分星", 1.1);
    b.set_bonus({ .damage = 102 }, { "暗藏杀机" });
    b.on_hit("千秋万劫", { b.burst("化血"), b.burst("化血急毒") });
    b.on_hit("蚀肌弹", { b.when("鸩羽", b.reset("孔雀翎")) });
    // Lock on when switching to 重弩. Loading takes two seconds, subsequent attacks are three seconds apart.
    b.tick_schedule("重弩", 32, 0);
    // One lock-on consumes ten 注能. Each of the three individual bolts picks its own ammunition.
    b.pulse("重弩", { { .op = Op::CrossbowVolley, .slot = b.effect("重弩·一"), .extra = b.effect("重弩·二"),
                         .neutral_buff = b.buff("机关注能"), .neutral_dot = b.buff("鬼斧弹药") } });
    b.state({ 0, "秋风散影", 0, 0, 1 }, false);
    b.state({ 0, "气魄", 480, 0, 5 }, false);
    b.on_hit("暴雨梨花针", { b.selected("暴雨梨花针·气魄", b.apply("气魄")) });
    b.school_modifier({ b.buff("秋风散影"), 0, { .critical = 1000, .power = 102 } });
    for (auto n : { "暗藏杀机", "化血·尽刑(DOT)", "千秋万劫", "天女散花", "星离雨散", "封喉" }) {
        b.modifier(n, { b.buff("心无旁骛"), 0, { .critical = 1500, .power = 300 } });
        b.modifier(n, { b.buff("专注"), 0, { .damage = 102 }, "凝气淬机" });
        b.modifier(n, { b.buff("扬威"), 0, { .attack = 512 } });
        b.modifier(n, { b.buff("机关存在"), 0, { .attack = 154, .ignore = 389 }, "千机之威" });
        b.resource_modifier(n, 0, 0, 79, { .damage = 205 }, "擘两分星");
        for (int i = 1; i <= 4; ++i) b.modifier(n, { b.buff("汲雨"), i, { .overcome = i * 51, .strain_rating = i * 51 } });
    }
    b.adjust([](Prepared &p) {
        const auto &book = *p.config.book;
        if (std::ranges::contains(p.config.talents, "横断天枢")) {
            auto turret = std::ranges::find(book.buffs, "重弩", &BuffDescriptor::name) - book.buffs.begin();
            p.pulses[turret].front().amount = 2;
        }
        if (std::ranges::contains(p.config.talents, "秋风散影")) {
            p.distant_target_aura = static_cast<Slot>(std::ranges::find(book.buffs, "秋风散影", &BuffDescriptor::name) - book.buffs.begin());
            p.distant_threshold = 8;
            p.distant_linger = 160;
        }
        if (p.config.precombat) {
            auto buff = [&](std::string_view n) { return static_cast<Slot>(std::ranges::find(book.buffs, n, &BuffDescriptor::name) - book.buffs.begin()); };
            p.opening = { { Op::Add, buff("机关存在") }, { Op::Add, buff("连弩") } };
            if (std::ranges::contains(p.config.talents, "横断天枢")) p.opening.push_back({ Op::Add, buff("机关注能"), 50 });
            if (std::ranges::contains(p.config.talents, "星离雨散")) {
                // Author's two-second opener: 天绝 at -32, 天女 one hasted GCD later.
                const tick_t gcd = std::max(1, static_cast<int>(24 / p.haste));
                const auto trap = static_cast<Slot>(std::ranges::find(p.skills, "天绝地灭", &SkillDescriptor::name) - p.skills.begin());
                const auto flower = static_cast<Slot>(std::ranges::find(p.skills, "天女散花", &SkillDescriptor::name) - p.skills.begin());
                const bool charging = std::ranges::contains(p.config.talents, "横断天枢");
                p.resources[0].initial = 100 - 40 - 20 + 2 * (10 - (charging ? 5 : 0));
                if (charging) p.opening.push_back({ Op::Add, buff("机关注能"), 10 });
                p.opening_auras_elapsed = { { buff("天绝地灭"), 8 }, { buff("化血"), 32 - gcd } };
                p.opening_cooldowns.push_back({ trap, p.skills[trap].cooldown - 32 });
                p.opening_gcd = std::max(0, 2 * gcd - 32);
                p.opening.push_back({ Op::DelayedHit, flower, gcd - 8 });
            }
        }
        const auto aura = static_cast<Slot>(std::ranges::find(book.buffs, "气魄", &BuffDescriptor::name) - book.buffs.begin());
        const int major = p.config.base_major.value_or(book.default_major);
        for (size_t i = 0; i < book.hits.size(); ++i) {
            if (!book.hits[i].channel) continue;
            for (int layer = 1; layer <= 5; ++layer) {
                const int spunk = major * layer * 20 / 1024;
                p.modifiers[i].push_back({ aura, layer, { .critical = static_cast<int>((spunk * 225 / 1024) * 10000. / (9.609 * 990)),
                                                           .overcome_flat = spunk * 6 / 100, .attack_flat = spunk * 19 / 100, .class_attack_flat = spunk * 543 / 1024 } });
            }
        }
    });
    b.recipe("暴雨梨花针", "暴雨梨花针·4%会心", { .critical = 400 }, { "暴雨梨花针" });
    b.recipe("暴雨梨花针", "暴雨梨花针·5%伤害", { .damage = 51 }, { "暴雨梨花针" });
    b.recipe("暴雨梨花针", "暴雨梨花针·4%伤害", { .damage = 41 }, { "暴雨梨花针" });
    b.recipe("暴雨梨花针", "暴雨梨花针·气魄", {}, {});
    b.timing_recipe("蚀肌弹", "蚀肌弹·运功减2帧（一）", "蚀肌弹", 0, -2);
    b.timing_recipe("蚀肌弹", "蚀肌弹·运功减2帧（二）", "蚀肌弹", 0, -2);
    b.recipe("暗藏杀机", "暗藏杀机·5%伤害", { .damage = 51 }, { "暗藏杀机" });
    b.recipe("暗藏杀机", "暗藏杀机·4%伤害", { .damage = 41 }, { "暗藏杀机" });
    b.recipe("暗藏杀机", "暗藏杀机·3%伤害", { .damage = 31 }, { "暗藏杀机" });
    b.recipe("暗藏杀机", "暗藏杀机·立即激活", {}, {});
    b.single_target_recipe("千机变", "千机变·人偶图绝章", "千机弩始终锁定当前选中的敌对目标。单目标木桩中，锁定目标就是当前木桩。");
    b.recipe("天绝地灭", "天绝地灭·5%伤害", { .damage = 51 }, { "天绝地灭", "天风汲雨" });
    b.recipe("天绝地灭", "天绝地灭·4%伤害", { .damage = 41 }, { "天绝地灭", "天风汲雨" });
    b.recipe("天绝地灭", "天绝地灭·3%伤害", { .damage = 31 }, { "天绝地灭", "天风汲雨" });
    b.state({0, "回霆", 160, 0, 2}, false);
    b.state({0, "回霆·暴雨", 0, 0, 1}, false);
    b.cast("使用橙武", 800, 0, 0, {.choice = Legendary}, {b.reset("暴雨梨花针"), b.apply("回霆", 2)});
    b.on_cast("暴雨梨花针", {b.remove("回霆·暴雨"), b.when("回霆", b.apply("回霆·暴雨")), b.remove("回霆")});
    b.on_hit("暴雨梨花针", {b.when("回霆·暴雨", b.hit("暴雨梨花针·额外", 2))});
    b.after_cast("暴雨梨花针", {b.remove("回霆·暴雨")});
    b.legendary_damage({ "蚀肌弹", "天绝地灭" });
    return b.finish();
}
