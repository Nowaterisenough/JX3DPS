// Native event rules for 太玄经. Timing is expressed in frames.
static Book txj()
{
    Builder b("txj", "太玄经", R"(# 魔盒宏适配 https://www.jx3box.com/macro/68824
/cast 使用风腰坠
/cast 使用橙武
/cast [skill_notin_cd:起水卦&nobuff:世降共鸣] 起水卦
/cast [buff:水坎&buff:可施展祝由] 变卦
/cast [buff:火离&buff:世降共鸣] 变卦
/cast 祝由·火离
/cast [buff:世降共鸣&星运>=10] 御前星
/cast [nobuff:可施展祝由&星运>=200] 御前星
/cast [tnobuff:连极阵] 连极阵
/cast [tnobuff:灯魂] 奇门飞宫
/cast 镇星入舆
/cast 往者定
/cast 纵横三才
/cast 兵主逆
/cast 天斗旋
/cast 三星临
)");
    b.preset(2300,
             { 6657, 1448, 113, 1759, 208, 1290, 4212, 0, 1092 },
             { "三星临·5%伤害", "三星临·4%伤害", "三星临·4%会心", "三星临·3%伤害",
               "兵主逆·5%伤害", "兵主逆·4%伤害", "兵主逆·4%会心", "兵主逆·3%伤害",
               "天斗旋·5%伤害", "天斗旋·4%伤害", "天斗旋·4%会心", "天斗旋·调息减16帧",
               "鬼星开穴·5%伤害", "鬼星开穴·4%伤害", "鬼星开穴·4%会心", "鬼星开穴·3%会心",
               "返闭惊魂·调息减3秒", "返闭惊魂·调息减4秒", "返闭惊魂·延长灯魂",
               "奇门飞宫·调息减32帧", "奇门飞宫·调息减16帧",
               "起卦·调息减32帧", "起卦·调息减48帧",
               "变卦·顺序变卦" });
    b.choices({ "鬼遁", "御前星", "世降", "焚如", "连极阵", "知微", "顺祝", "亘天", "灵器", "镇星入舆", "荧入白" }, {}, { "神兵附伤" });
    b.default_talents({ "亘天", "灵器", "御前星", "顺祝", "鬼遁", "镇星入舆", "荧入白", "焚如", "世降" });
    b.resource({ "星运", 100, 0, 100 });
    b.state({ 32130, "水卦加速", 48, 0, 1 }, false);
    b.stance_haste("水卦加速", 103);
    b.state({ 0, "灯魂", 15984, 0, 1 }, true, false, false);
    b.state({ 0, "知微", 480, 32, 2 }, true, true, true);
    b.state({ 0, "连极阵", 160, 32, 1 }, true, false, false);
    b.state({ 0, "明灯", 192, 0, 1 }, false, false, false);
    b.damage(0, "知微(DOT)", { 1, 0, 1400, 0, 15, 32, false, { .pve = 154 }, {} });
    b.damage(28815, "纵横三才", { 9, 3, 506, 0, 0, 0, false, { .pve = 154 }, {} });
    b.damage(42423, "纵横三才·主动", { 3, 9, 920, 0, 0, 0, false, { .pve = 154 }, {} });
    b.damage(33236, "列宿游", { 38, 26, 2400, 0, 0, 0, false, { .pve = 154 }, {} });
    b.damage(24558, "三星临", { 188, 5, 330, 0, 0, 0, false, { .pve = 154 }, {} });
    b.secondary_damage(37599, "三星临·神兵", { 2, 2, 50, 0, 0, 0, false, { .pve = 154 }, {} });
    b.damage(24824, "天斗旋", { 242, 42, 942, 0, 0, 0, false, { .pve = 154 }, {} });
    b.damage(24811, "兵主逆", { 219, 18, 388, 0, 0, 0, false, { .pve = 154 }, {} });
    b.dot("知微", "知微(DOT)");
    b.pulse("连极阵",
            { b.selected("连极阵", b.within(33, b.at(0, 5, 100, b.hit("纵横三才", 1)))),
              b.within(33, b.at(0, 5, 100, b.selected("知微", b.apply("知微", 1)))),
              b.selected("连极阵", b.within(33, b.at(0, 5, 100, b.gain(0, -5)))) });
    b.cast("奇门飞宫", 272, 0, 0, { .charges = 3 }, { b.apply("灯魂", 1) });
    b.cast("连极阵", 48, 24, 0, { .choice = "连极阵" }, { b.apply("连极阵", 1) });
    b.cast("纵横三才", 96, 0, 0, { .required = b.buff("灯魂") }, { b.hit("纵横三才·主动", 1), b.selected("知微", b.apply("知微", 1)) });
    b.cast("列宿游", 400, 24, 0, {}, { b.hit("列宿游", 1), b.apply("明灯", 1) });
    b.cast("三星临", 0, 24, 0, {}, { b.hit("三星临", 1), b.advance("知微", 1), b.gain(0, 5), b.selected("神兵附伤", b.proc("三星临·神兵", 256)) });
    b.cast("天斗旋", 80, 24, 24, {}, { b.hit("天斗旋", 1), b.selected(Legendary, b.captured(b.hit("神兵·循环天转"))), b.advance("知微", 1), b.gain(0, 20) });
    b.cast("兵主逆", 80, 24, 0, { .pulses = 3, .spacing = 15, .distribute_pulses = true, .stance_cast = true }, { b.hit("兵主逆", 1), b.selected(Legendary, b.captured(b.hit("神兵·合灯"))), b.advance("知微", 1), b.gain(0, 5) });
    b.recipe("三星临", "三星临·2%会心", { .critical = 200 }, { "三星临" });
    b.recipe("三星临", "三星临·3%会心", { .critical = 300 }, { "三星临" });
    b.recipe("三星临", "三星临·3%伤害", { .damage = 31 }, { "三星临" });
    b.recipe("三星临", "三星临·4%会心", { .critical = 400 }, { "三星临" });
    b.recipe("三星临", "三星临·4%伤害", { .damage = 41 }, { "三星临" });
    b.recipe("三星临", "三星临·5%伤害", { .damage = 51 }, { "三星临" });
    b.recipe("兵主逆", "兵主逆·3%会心", { .critical = 300 }, { "兵主逆" });
    b.recipe("兵主逆", "兵主逆·3%伤害", { .damage = 31 }, { "兵主逆" });
    b.recipe("兵主逆", "兵主逆·4%会心", { .critical = 400 }, { "兵主逆" });
    b.recipe("兵主逆", "兵主逆·4%伤害", { .damage = 41 }, { "兵主逆" });
    b.recipe("兵主逆", "兵主逆·5%伤害", { .damage = 51 }, { "兵主逆" });
    b.recipe("天斗旋", "天斗旋·2%会心", { .critical = 200 }, { "天斗旋" });
    b.recipe("天斗旋", "天斗旋·3%会心", { .critical = 300 }, { "天斗旋" });
    b.recipe("天斗旋", "天斗旋·3%伤害", { .damage = 31 }, { "天斗旋" });
    b.recipe("天斗旋", "天斗旋·4%会心", { .critical = 400 }, { "天斗旋" });
    b.recipe("天斗旋", "天斗旋·4%伤害", { .damage = 41 }, { "天斗旋" });
    b.recipe("天斗旋", "天斗旋·5%伤害", { .damage = 51 }, { "天斗旋" });
    b.passive("兵主逆", "顺祝", { .damage = 102 });
    b.passive("天斗旋", "顺祝", { .damage = 102 });
    b.passive("天斗旋", "亘天", { .final_damage = 717 });
    b.variant("天斗旋", "亘天", 128, 16, { .hasted_cd = true, .charges = 3, .precombat_cast = true, .stance_cast = true, .stance_cooldown = true }, { b.hit("天斗旋"), b.selected(Legendary, b.captured(b.hit("神兵·循环天转"))), b.advance("知微", 1), b.gain(0, 20) });
    b.set_bonus({ .damage = 102 }, { "兵主逆", "天斗旋" });
    b.state({ 0, "水坎", 320, 0, 1 }, false);
    b.state({ 0, "山艮", 320, 0, 1 }, false);
    b.state({ 0, "免费变卦", 0, 0, 2 }, false);
    b.state({ 0, "荧入白", 320, 0, 1 }, false);
    b.state({ 0, "镇星", 0, 0, 1 }, false);
    b.state({ 0, "变卦前水坎", 1, 0, 1 }, false);
    b.state({ 0, "火离", 320, 0, 1 }, false);
    b.state({ 0, "可施展祝由", 320, 0, 1 }, false);
    b.state({ 0, "世降共鸣", 96, 0, 1 }, false);
    b.state({ 0, "祝由·火离", 416, 32, 1 }, true, false, true);
    b.damage(24454, "卦象·火离", { 7, 4, 240, 0, 0, 0, false, { .pve = 154 }, {} });
    b.damage(46241, "世降", { 10, 5, 968, 0, 0, 0, false, { .pve = 154 }, {} });
    b.damage(42520, "御前星", { 10, 10, 540, 0, 0, 0, false, { .pve = 154 }, {} });
    b.cast("起火卦", 400, 24, 0, {}, { b.apply("火离"), b.apply("可施展祝由") });
    b.cast("祝由·火离",
           0,
           24,
           0,
           { .required = b.buff("可施展祝由"), .forbidden = b.buff("水坎"), .gcd_group = 3, .also_forbidden = b.buff("山艮") },
           { b.remove("可施展祝由"), b.apply("祝由·火离"), b.selected("世降", b.apply("世降共鸣")) });
    b.damage(25031, "焚如·火离", { 10, 6, 360, 0, 0, 0, false, { .pve = 154 }, {} });
    b.pulse("祝由·火离", { b.hit("卦象·火离") });
    b.cast("御前星",
           80,
           0,
           0,
           { .resource = 0, .cost = 10, .choice = "御前星", .pulses = 30, .spacing = 4, .timing_resource = 0, .pulses_per_point = true, .points_per_pulse = 10, .distribute_pulses = true },
           { b.gain(0, -10), b.when("世降共鸣", b.hit("世降")), b.hit("御前星") });
    b.on_hit("世降",
             {
                 { Op::ProlongAura, b.buff("祝由·火离"), 32 }
    });
    b.adjust([](Prepared &p) {
        if (!std::ranges::contains(p.config.talents, "鬼遁")) p.stance_haste = 52;
        if (std::ranges::contains(p.config.talents, "御前星")) {
            p.enabled_actions[std::ranges::find(p.skills, "鬼星开穴", &SkillDescriptor::name) - p.skills.begin()] = false;
        }
        if (p.config.precombat) {
            const auto &buffs = p.config.book->buffs;
            auto aura = [&](std::string_view n) { return static_cast<Slot>(std::ranges::find(buffs, n, &BuffDescriptor::name) - buffs.begin()); };
            p.opening = { { Op::Add, aura("灯魂") }, { Op::Add, aura("水坎") }, { Op::Add, aura("可施展祝由") } };
            if (std::ranges::contains(p.config.talents, "镇星入舆")) {
                for (auto n : { "镇星·临", "镇星·兵", "镇星·斗" }) p.opening.push_back({ .op = Op::Add, .slot = aura(n), .duration = 240 });
            }
            if (std::ranges::contains(p.config.talents, "荧入白")) p.opening.push_back({ .op = Op::Add, .slot = aura("荧入白"), .duration = 160 });
            if (std::ranges::contains(p.config.talents, "鬼遁")) p.opening.push_back({ Op::Add, aura("免费变卦"), 2 });
        }
        if (std::ranges::contains(p.config.talents, std::string_view{ "鬼遁" })) {
            p.resources[0].initial = p.resources[0].high = 300;
        }
        if (std::ranges::contains(p.config.talents, std::string_view{ "焚如" })) {
            const auto &b = *p.config.book;
            auto aura = std::ranges::find(b.buffs, std::string_view{ "祝由·火离" }, &BuffDescriptor::name) - b.buffs.begin();
            p.pulses[aura][0].slot =
                static_cast<Slot>(std::ranges::find(p.skills, std::string_view{ "焚如·火离" }, &SkillDescriptor::name) -
                                  p.skills.begin());
        }
    });
    b.damage(25174, "灵器", { 3, 1, 260, 0, 0, 0, false, { .pve = 154 } });
    for (auto hit : { "三星临", "天斗旋", "兵主逆" }) b.on_hit(hit, { b.selected("灵器", b.when("灯魂", b.hit("灵器"))) });
    b.cast("起水卦", 400, 24, 0, { .gcd_group = 1 }, { b.remove("火离"), b.remove("山艮"), b.apply("水坎"), b.apply("可施展祝由"),
                                                         b.selected("鬼遁", b.apply("免费变卦", 2)), b.selected("荧入白", b.apply("荧入白")) });
    b.cast("变卦", 0, 8, 0, { .resource = 0, .cost = 50, .choice = "变卦·顺序变卦", .lock = 8, .gcd_group = 2, .capture = b.buff("火离"), .free_cost = b.buff("免费变卦") },
           { b.when("免费变卦", b.gain(0, -50), true), b.remove("免费变卦"), b.when("水坎", b.apply("变卦前水坎")), b.remove("水坎"), b.remove("火离"),
             b.when("山艮", b.apply("水坎")), b.remove("山艮"), b.captured(b.apply("山艮")), b.when("变卦前水坎", b.apply("火离")), b.remove("变卦前水坎"),
             b.selected("荧入白", b.apply("荧入白")) });
    b.damage(24373, "往者定", { 4, 3, 250, 0, 0, 0, false, { .pve = 154 } });
    b.damage(42509, "太白蚀昴", { 0, 0, 260, 0, 0, 0, false, { .pve = 154 } });
    b.cast("往者定", 224, 24, 0, {}, { b.hit("往者定") });
    b.repeat_hits("往者定", "太白蚀昴", 8, 8, 6);
    b.state({28367, "镇星·临", 480, 0, 1}, false);
    b.state({28368, "镇星·兵", 480, 0, 1}, false);
    b.state({28369, "镇星·斗", 480, 0, 1}, false);
    b.cast("镇星入舆", 400, 8, 0, { .required = b.buff("灯魂"), .choice = "镇星入舆", .lock = 8, .gcd_group = 4 },
           { b.apply("灯魂"), b.apply("连极阵"), b.apply("镇星·临"), b.apply("镇星·兵"), b.apply("镇星·斗") });
    for (auto [name, seal, extra, channel] : { std::tuple{"三星临", "镇星·临", "三星临·镇星环", 330},
            {"兵主逆", "镇星·兵", "兵主逆·镇星环", 388}, {"天斗旋", "镇星·斗", "天斗旋·镇星环", 942} }) {
        const int fixed = name == std::string_view("三星临") ? 188 : name == std::string_view("兵主逆") ? 219 : 242;
        const int spread = name == std::string_view("三星临") ? 5 : name == std::string_view("兵主逆") ? 18 : 42;
        b.damage(0, extra, {fixed, spread, channel, 0, 0, 0, false, { .pve = 154, .final_damage = 512 } });
        b.inherit_recipes(extra, name);
        b.on_hit(name, { b.when(seal, b.hit(extra)) });
        b.on_hit(extra, { b.apply("镇星"), b.selected("灵器", b.when("灯魂", b.hit("灵器"))) });
        b.after_cast(name, { b.remove(seal) });
        for (auto aura : { "水坎", "山艮", "火离", "荧入白", "可施展祝由" }) {
            Step prolong{ .op = Op::ProlongAura, .slot = b.buff(aura), .amount = 48, .choice = "顺祝", .duration = 480 };
            b.on_hit(name, { prolong });
            b.on_hit(extra, { prolong });
        }
    }
    for (auto n : { "三星临", "兵主逆", "天斗旋", "御前星" }) b.on_hit(n, { b.when("水坎", b.apply("水卦加速")) });
    b.secondary_damage(33588, "神兵·合灯", {120, 0, 1052, 0, 0, 0, false, {.pve = 154}});
    b.secondary_damage(33588, "神兵·循环天转", {120, 0, 1347, 0, 0, 0, false, {.pve = 154}});
    b.school_modifier({ b.buff("荧入白"), 0, { .ignore = 512, .strain_rating = 205 }, "荧入白" });
    b.school_modifier({ b.buff("镇星"), 0, { .attack = 184 } });
    b.modifier("御前星", { b.buff("山艮"), 0, { .final_damage = 154 } });
    b.timing_recipe("起卦", "起卦·调息减32帧", "起水卦", -32, 0);
    b.timing_recipe("起卦", "起卦·调息减48帧", "起水卦", -48, 0);
    b.timing_recipe("天斗旋", "天斗旋·调息减16帧", "天斗旋", -16, 0);
    b.damage(24870, "鬼星开穴", {26, 38, 742, 0, 0, 0, false, { .pve = 154 } });
    b.cast("鬼星开穴", 256, 24, 0, { .resource = 0, .cost = 50, .required = b.buff("灯魂") },
           { b.gain(0, -50), b.hit("鬼星开穴"), b.remove("灯魂"), b.remove("连极阵"), b.when("火离", b.apply("祝由·火离")) });
    b.cast("返闭惊魂", 608, 24, 0, { .gcd_group = 5 }, { b.apply("灯魂") });
    b.variant("返闭惊魂", "返闭惊魂·延长灯魂", 608, 0, { .gcd_group = 5 },
              { b.when("连极阵", b.apply("灯魂"), true), b.when("连极阵", { Op::ProlongAura, b.buff("灯魂"), 320 }) });
    b.recipe("鬼星开穴", "鬼星开穴·5%伤害", { .damage = 51 }, { "鬼星开穴" });
    b.recipe("鬼星开穴", "鬼星开穴·4%伤害", { .damage = 41 }, { "鬼星开穴" });
    b.recipe("鬼星开穴", "鬼星开穴·4%会心", { .critical = 400 }, { "鬼星开穴" });
    b.recipe("鬼星开穴", "鬼星开穴·3%会心", { .critical = 300 }, { "鬼星开穴" });
    b.timing_recipe("返闭惊魂", "返闭惊魂·调息减3秒", "返闭惊魂", -48, 0);
    b.timing_recipe("返闭惊魂", "返闭惊魂·调息减4秒", "返闭惊魂", -64, 0);
    b.recipe("返闭惊魂", "返闭惊魂·延长灯魂", {}, {});
    b.timing_recipe("奇门飞宫", "奇门飞宫·调息减32帧", "奇门飞宫", -32, 0);
    b.timing_recipe("奇门飞宫", "奇门飞宫·调息减16帧", "奇门飞宫", -16, 0);
    b.timing_recipe("起卦", "起卦·调息减32帧", "起火卦", -32, 0);
    b.timing_recipe("起卦", "起卦·调息减48帧", "起火卦", -48, 0);
    b.recipe("变卦", "变卦·顺序变卦", {}, {});
    b.state({0, "葛天八阕", 160, 0, 3}, false);
    b.cast("使用橙武", 800, 0, 0, {.choice = Legendary}, {b.apply("葛天八阕", 3)});
    b.no_cooldown_during("天斗旋", "葛天八阕", true, true);
    b.no_cooldown_during("兵主逆", "葛天八阕", false, true);
    for (auto name : {"兵主逆", "天斗旋"}) b.on_cast(name, {b.remove("葛天八阕")});
    b.legendary_damage({ "三星临", "兵主逆", "三星临·镇星环", "兵主逆·镇星环" });
    return b.finish();
}
