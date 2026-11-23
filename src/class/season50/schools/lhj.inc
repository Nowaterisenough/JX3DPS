// Native event rules for 凌海诀. Timing is expressed in frames.
static Book lhj()
{
    Builder b("lhj", "凌海诀", R"(# 魔盒宏适配 https://www.jx3box.com/macro/110612
# 天地两页按体态切换，地面在下次起伞前放逐波、振翅
/cast 使用风腰坠
/cast 使用橙武
/cast 物化天行
/cast [bufftime:物化天行>26] 振翅图南
/cast [skill_cd:浮游天地<3.5&tnobuff:天行] 翼绝云天
/cast [nobuff:物化天行&nobuff:游仙] 振翅图南
/cast [nobuff:物化天行&skill_cd:浮游天地<2] 逐波灵游
/cast 浮游天地
/cast [buff:物化天行] 逐波灵游
/cast [buff:物化天行&skill_energy:溟海御波<2] 海运南冥
/cast [buff:物化天行] 溟海御波
/cast [nobuff:物化天行&buff:清源=3] 木落雁归
/cast [nobuff:物化天行] 海运南冥
/cast 击水三千
)");
    b.preset(2301,
             { 6299, 1588, 170, 2115, 208, 1153, 4631, 0, 1092 },
             { "振翅图南·5%伤害", "振翅图南·4%伤害", "振翅图南·3%伤害", "振翅图南·4%会心", "海运南冥·4%伤害",
               "海运南冥·3%伤害", "海运南冥·4%会心", "海运南冥·3%会心", "木落雁归·5%伤害", "木落雁归·4%伤害",
               "木落雁归·3%伤害", "木落雁归·4%会心", "翼绝云天·5%伤害", "翼绝云天·4%伤害", "翼绝云天·3%伤害",
               "翼绝云天·4%会心", "浮游天地·2%会心", "浮游天地·3%会心", "浮游天地·4%会心", "击水三千·5%伤害",
               "击水三千·4%伤害", "击水三千·3%伤害", "击水三千·4%会心" });
    b.choices({ "梦悠", "清源", "驰行", "游仙", "风驰", "抟空", "天行", "击浪", "青冥", "驾鸾" }, {}, {});
    b.default_talents({ "梦悠", "清源", "驰行", "游仙", "风驰", "抟空", "驾鸾", "击浪", "青冥" });
    b.state({ 0, "撑伞浮空", 32, 0, 1 }, false);
    b.state({ 0, "物化接近", 1, 0, 1 }, false);
    b.state({ 0, "地面", 0, 0, 1 }, false);
    b.initial("地面");
    b.state({ 0, "起伞", 20, 0, 1 }, false);
    b.state({ 0, "物化天行", 480, 0, 1 }, false);
    b.state({ 0, "抟空", 480, 0, 1 }, false);
    b.state({ 0, "太息", 240, 0, 1 }, false);
    b.state({ 0, "游仙", 240, 0, 1 }, false);
    b.state({ 0, "梦悠", 160, 0, 1 }, false);
    b.state({ 0, "风驰", 240, 0, 3 }, false);
    b.state({ 0, "振翅", 160, 0, 1 }, false);
    b.state({ 0, "驰行", 160, 0, 1 }, false);
    b.state({ 0, "清源", 0, 0, 3 }, false);
    b.state({ 0, "天行", 128, 0, 7 }, true);
    b.state({ 0, "伞击计时", 0, 24, 1 }, false);
    b.stance_haste("物化天行", 819, "伞击计时");
    b.tick_schedule("伞击计时", 0, 1);
    b.initial("伞击计时");
    b.state({ 0, "青冥", 192, 32, 1 }, true, true, true);
    b.damage(0, "青冥(DOT)", { 50, 0, 1138, 0, 8, 32, true, { .pve = 502 }, {} });
    b.damage(19766, "击水三千·一", { 120, 5, 321, 1024, 0, 0, true, { .pve = 502 }, {} });
    b.damage(19767, "击水三千·二", { 500, 15, 277, 1024, 0, 0, true, { .pve = 502 }, {} });
    b.damage(20014, "击水三千·三", { 150, 5, 308, 1024, 0, 0, true, { .pve = 502 }, {} });
    b.damage(20322, "溟海御波", { 162, 1, 648, 0, 0, 0, true, { .pve = 502 }, {} });
    b.damage(20685, "海运南冥·近", { 208, 1, 811, 0, 0, 0, true, { .pve = 502 }, {} });
    b.dot("青冥", "青冥(DOT)");
    b.accumulate_ticks("青冥", 12);
    b.damage(20323, "逐波灵游", { 83, 1, 376, 0, 0, 0, true, { .pve = 502 }, {} });
    b.damage(20052, "浮游天地", { 83, 1, 140, 2048, 0, 0, true, { .pve = 502 }, {} });
    b.damage(19819, "木落雁归", { 108, 1, 578, 2048, 0, 0, true, { .pve = 502 }, {} });
    b.secondary_damage(32478, "风驰", { 13, 1, 870, 0, 0, 0, true, { .pve = 502 }, {} });
    b.secondary_damage(31250, "振翅图南", { 45, 1, 96, 0, 0, 0, true, { .pve = 502 }, {} });
    b.secondary_damage(20016, "翼绝云天", { 100, 5, 153, 0, 0, 0, true, { .pve = 502 }, {} });
    b.secondary_damage(45100, "天行", { 3, 1, 303, 0, 0, 0, true, { .pve = 4598 }, {} });
    b.damage(19712, "飘遥伞击", { 0, 0, 24, 1024, 0, 0, true, { .pve = 502 }, {} });
    b.damage(19712, "飘遥伞击·物化", { 0, 0, 16, 1024, 0, 0, true, { .pve = 502 }, {} });
    b.pulse("伞击计时", { b.when("地面", b.hit("飘遥伞击")), b.when("物化天行", b.hit("飘遥伞击·物化")) });

    const Step retime{ Op::RetimePulse, b.buff("伞击计时") };
    b.cast("浮游天地",
           272,
           20,
           0,
           { .forbidden = b.buff("撑伞浮空"), .lock = 20, .gcd_group = 2, .also_forbidden = b.buff("物化天行") },
           { b.hit("浮游天地"),
             b.remove("地面"),
             b.apply("起伞"),
             b.apply("撑伞浮空"),
             b.reset("海运南冥"),
             b.reset("溟海御波"),
             b.reset("逐波灵游"),
             b.selected("抟空", b.apply("抟空")) });
    b.on_expiry("起伞", { b.apply("太息"), b.selected("游仙", b.remove("太息")), b.selected("游仙", b.apply("游仙")) });
    b.cast("物化天行",
           192,
           24,
           0,
           { .required = b.buff("撑伞浮空"), .forbidden = b.buff("起伞"), .charges = 2, .gcd_group = 2 },
           { b.remove("撑伞浮空"), b.apply("物化天行"), b.apply("物化接近"), retime });
    b.on_expiry("物化天行", { b.remove("抟空"), b.apply("地面"), retime });
    b.on_expiry("撑伞浮空", { b.remove("抟空"), b.apply("地面") });

    // Each hurricane retains its own half-second clock, even when two casts overlap.
    for (auto field : { "飓风一", "飓风二", "飓风三", "飓风四" }) {
        b.state({ 0, field, 96, 8, 1 }, true);
        b.pulse(field, { b.hit("振翅图南") });
    }
    Step spawn{ Op::SpawnField, b.buff("飓风一"), 4 };
    spawn.extra     = b.buff("振翅");
    spawn.duration  = 96;
    Step extend     = spawn;
    extend.op       = Op::ExtendLatestField;
    extend.duration = 48;
    extend.maximum  = 20;
    b.cast("振翅图南", 320, 32, 0, { .lock = 32, .gcd_group = 1 }, { spawn, b.selected("驰行", b.apply("驰行")) });
    b.charge_variant("振翅图南", "驾鸾", 400, 2);
    b.on_expiry("振翅", { b.remove("风驰", 3) });
    Step prolong_wind{ Op::ProlongAura, b.buff("风驰"), 48 };
    prolong_wind.duration = 240;
    Step prolong_palm{ Op::ProlongAura, b.buff("驰行"), 48 };
    prolong_palm.duration = 160;
    b.cast("击水三千",
           0,
           24,
           0,
           { .forbidden = b.buff("撑伞浮空"), .chain_window = 96, .ground_gcd = true },
           { b.cycle("击水三千·一", 3),
             b.selected("青冥", b.apply("青冥")),
             b.selected("击浪", b.within(30, extend)),
             b.selected("击浪", b.within(30, b.when("振翅", prolong_wind))),
             b.selected("击浪", b.within(30, b.when("振翅", prolong_palm))),
             b.selected("驾鸾", b.shorten("振翅图南", 32)) });
    b.cast("木落雁归",
           80,
           24,
           0,
           { .forbidden = b.buff("撑伞浮空"), .ground_gcd = true },
           { b.hit("木落雁归"), b.remove("清源", 3), b.selected("驾鸾", b.shorten("振翅图南", 32)) });
    for (auto spell : { "击水三千", "木落雁归" }) {
        b.on_cast(spell, { b.when("物化天行", b.remove("抟空")), b.when("物化天行", b.apply("地面")), b.remove("物化天行"), retime });
    }
    b.cast("溟海御波", 96, 24, 0, { .forbidden = b.buff("起伞"), .charges = 2 }, { b.hit("溟海御波"), b.advance("青冥", 1) });
    b.cast("海运南冥",
           80,
           24,
           0,
           { .forbidden = b.buff("起伞"), .free_cd = b.buff("抟空"), .capture = b.buff("抟空"), .also_forbidden = b.buff("物化接近") },
           { b.hit("海运南冥·近"), b.advance("青冥", 1), b.remove("抟空") });
    b.cast("逐波灵游", 240, 24, 0, { .forbidden = b.buff("起伞") }, { b.hit("逐波灵游") });
    const auto wind_proc = b.selected("梦悠", b.selected("风驰", b.when("梦悠", b.hit("风驰"))));
    b.on_cast("逐波灵游", { wind_proc, b.selected("梦悠", b.apply("梦悠")) });
    b.on_cast("海运南冥", { b.captured(wind_proc), b.captured(b.selected("梦悠", b.apply("梦悠"))) });
    b.on_hit("风驰", { b.when("振翅", b.apply("风驰")) });
    for (auto spell : { "逐波灵游", "海运南冥", "溟海御波" }) {
        b.after_cast(spell, { b.selected("清源", b.apply("清源")), b.when("天行", b.hit("天行")), b.remove("天行") });
    }
    b.cast("翼绝云天",
           384,
           32,
           0,
           { .lock = 32, .gcd_group = 1 },
           { b.hit("翼绝云天"), b.hit("翼绝云天"), b.later("翼绝云天", 64), b.later("翼绝云天", 128), b.later("翼绝云天", 192) });
    b.variant("翼绝云天",
              "天行",
              864,
              0,
              { .lock = 32, .charges = 2, .gcd_group = 1 },
              { b.apply("天行", 7), b.hit("翼绝云天"), b.hit("翼绝云天"), b.later("翼绝云天", 32), b.later("翼绝云天", 64), b.later("翼绝云天", 96) });
    b.recipe("击水三千", "击水三千·2%会心", { .critical = 200 }, { "击水三千·一", "击水三千·三", "击水三千·二" });
    b.recipe("击水三千", "击水三千·3%会心", { .critical = 300 }, { "击水三千·一", "击水三千·三", "击水三千·二" });
    b.recipe("击水三千", "击水三千·3%伤害", { .damage = 31 }, { "击水三千·一", "击水三千·三", "击水三千·二" });
    b.recipe("击水三千", "击水三千·4%会心", { .critical = 400 }, { "击水三千·一", "击水三千·三", "击水三千·二" });
    b.recipe("击水三千", "击水三千·4%伤害", { .damage = 41 }, { "击水三千·一", "击水三千·三", "击水三千·二" });
    b.recipe("击水三千", "击水三千·5%伤害", { .damage = 51 }, { "击水三千·一", "击水三千·三", "击水三千·二" });
    b.recipe("海运南冥", "海运南冥·2%会心", { .critical = 200 }, { "海运南冥·近" });
    b.recipe("海运南冥", "海运南冥·2%伤害", { .damage = 21 }, { "海运南冥·近" });
    b.recipe("海运南冥", "海运南冥·3%会心", { .critical = 300 }, { "海运南冥·近" });
    b.recipe("海运南冥", "海运南冥·3%伤害", { .damage = 31 }, { "海运南冥·近" });
    b.recipe("海运南冥", "海运南冥·4%会心", { .critical = 400 }, { "海运南冥·近" });
    b.recipe("海运南冥", "海运南冥·4%伤害", { .damage = 41 }, { "海运南冥·近" });
    b.set_bonus({ .damage = 102 }, { "海运南冥·近" });
    for (auto n : { "逐波灵游", "海运南冥·近", "溟海御波" }) {
        b.modifier(n, { b.buff("驰行"), 0, { .damage = 102 }, "驰行" });
        for (auto stance : { "撑伞浮空", "物化天行" }) {
            b.modifier(n,
                       {
                           b.buff(stance),
                           0,
                           { .critical = 2000, .power = 205 },
                           "游仙"
            });
        }
    }
    b.secondary_damage(25783, "木落雁归·神兵", {0, 2, 50, 0, 0, 0, true, {.pve = 502}});
    b.secondary_damage(26935, "御波驾澜(DOT)", {25, 0, 340, 0, 6, 48, true, {.pve = 502}});
    for (auto n :
         { "振翅图南", "风驰", "天行", "翼绝云天", "击水三千·一", "击水三千·二", "击水三千·三", "木落雁归", "海运南冥·近", "溟海御波", "逐波灵游", "浮游天地", "青冥(DOT)", "飘遥伞击", "飘遥伞击·物化", "御波驾澜(DOT)", "木落雁归·神兵" })
    {
        b.modifier(n, { b.buff("梦悠"), 0, { .ignore = 512 } });
        b.modifier(n, { b.buff("太息"), 0, { .attack = 102 } });
        b.modifier(n, { b.buff("游仙"), 0, { .attack = 307 } });
    }
    for (int layers = 1; layers <= 3; ++layers) {
        b.modifier("振翅图南", { b.buff("风驰"), layers, { .damage = (461 * layers - (layers == 3)) } });
        b.modifier("木落雁归", { b.buff("清源"), layers, { .damage = (410 * layers - (layers > 1)) } });
    }
    b.recipe("振翅图南", "振翅图南·5%伤害", { .damage = 51 }, { "振翅图南" });
    b.recipe("振翅图南", "振翅图南·4%伤害", { .damage = 41 }, { "振翅图南" });
    b.recipe("振翅图南", "振翅图南·3%伤害", { .damage = 31 }, { "振翅图南" });
    b.recipe("振翅图南", "振翅图南·4%会心", { .critical = 400 }, { "振翅图南" });
    b.recipe("木落雁归", "木落雁归·3%伤害", { .damage = 31 }, { "木落雁归" });
    b.recipe("木落雁归", "木落雁归·4%伤害", { .damage = 41 }, { "木落雁归" });
    b.recipe("木落雁归", "木落雁归·5%伤害", { .damage = 51 }, { "木落雁归" });
    b.recipe("木落雁归", "木落雁归·4%会心", { .critical = 400 }, { "木落雁归" });
    b.recipe("翼绝云天", "翼绝云天·3%伤害", { .damage = 31 }, { "翼绝云天", "天行" });
    b.recipe("翼绝云天", "翼绝云天·4%伤害", { .damage = 41 }, { "翼绝云天", "天行" });
    b.recipe("翼绝云天", "翼绝云天·5%伤害", { .damage = 51 }, { "翼绝云天", "天行" });
    b.recipe("翼绝云天", "翼绝云天·4%会心", { .critical = 400 }, { "翼绝云天", "天行" });
    b.recipe("浮游天地", "浮游天地·2%会心", { .critical = 200 }, { "浮游天地" });
    b.recipe("浮游天地", "浮游天地·3%会心", { .critical = 300 }, { "浮游天地" });
    b.recipe("浮游天地", "浮游天地·4%会心", { .critical = 400 }, { "浮游天地" });
    b.adjust([](Prepared &p) {
        const auto &book  = *p.config.book;
        const auto  skill = [&](std::string_view n) {
            return std::ranges::find(book.skills, n, &SkillDescriptor::name) - book.skills.begin();
        };
        const auto aura = [&](std::string_view n) {
            return static_cast<Slot>(std::ranges::find(book.buffs, n, &BuffDescriptor::name) - book.buffs.begin());
        };
        const auto has = [&](std::string_view n) {
            return std::ranges::contains(p.config.talents, n);
        };
        for (auto &step : p.impacts[skill("物化天行")]) {
            if (step.op == Op::Add && step.slot == aura("物化接近")) {
                step.duration = 3 + 24 - static_cast<int>(24 / p.haste);
            }
        }
        const auto wind = skill("振翅图南");
        if (has("梦悠")) {
            p.skills[wind].cooldown -= 64;
        }
        if (has("驰行")) {
            p.impacts[wind].front().duration = 160;
        }
        if (p.config.precombat) {
            p.opening.push_back({ Op::Remove, aura("地面") });
            p.opening.push_back({ .op = Op::Add, .slot = aura("撑伞浮空"), .duration = 32 });
            p.opening.push_back({ .op = Op::Add, .slot = has("游仙") ? aura("游仙") : aura("太息"), .duration = 120 });
            if (has("抟空")) {
                p.opening.push_back({ .op = Op::Add, .slot = aura("抟空"), .duration = 240 });
            }
        }
    });
    b.state({0, "孤游万里", 96, 0, 1}, false);
    b.state({19557, "御波驾澜", 288, 48, 3}, true, true, true);
    b.dot("御波驾澜", "御波驾澜(DOT)");
    b.cast("使用橙武", 800, 0, 0, {.choice = Legendary}, {b.apply("孤游万里")});
    for (auto name : {"木落雁归", "溟海御波", "逐波灵游"}) {
        b.no_cooldown_during(name, "孤游万里");
        b.after_cast(name, {b.when("孤游万里", b.apply("御波驾澜"))});
    }
    b.after_cast("木落雁归", {b.selected(Legendary, b.proc("木落雁归·神兵", 256))});
    b.legendary_damage({ "木落雁归", "翼绝云天" });
    return b.finish();
}
