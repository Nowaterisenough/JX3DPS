#include "common/builder.hpp"

namespace JX3DPS::runtime::season50 {
// 气点每 2 点为一格。无我无剑按扣气前的气点选十档伤害之一。
Book 太虚剑意()
{
    // 魔盒 111525 长轴以玄应之气决定进人剑时机，以目标风逝层数决定何时打八荒。
    Builder b("tx", "太虚剑意", R"(# https://www.jx3box.com/bps/111525
/cast 使用风腰坠
/cast [buff:人剑合一&nobuff:紫气东来] 紫气东来
/cast [tbuff:风逝>3] 使用橙武
/cast [last_skill:吞日月] 碎星辰
/cast [玄应之气>44&nobuff:人剑合一] 人剑合一
/cast [tbuff:风逝>3|气点<6] 八荒归元
/cast [buff:人剑合一|气点>5] 无我无剑
/cast 吞日月
/cast 三环套月
)");
    auto &school = b.definition();
    school.info = { school.code, "纯阳", "身法", "Physics", 727, 41, 0 };
    school.display_order = 0;
    school.saved.macro_digest = "77fd56e5273e9ee1b658a9b607fb95166d181fe81c9c254e4916fc7d68738e27";
    school.saved.talents_v7 = { "负阴" };
    school.saved.talents_v6 = { "负阴" };
    school.saved.all_recipes_added_v7 = true;
    school.alias = "tx50";
    school.initial_qidian_option = true;
    school.raid.penetration_perfume = true;
    school.raid.gear_score = 24440;
    b.preset(1752, { 8784, 1243, 218, 3165, 859, 2762, 5644, 0, 469 }, { "三环套月·3%伤害", "三环套月·4%伤害", "三环套月·5%伤害", "三环套月·4%会心",
               "八荒归元·3%伤害", "八荒归元·4%伤害", "八荒归元·5%伤害", "八荒归元·调息减16帧",
               "无我无剑·3%伤害", "无我无剑·4%伤害", "无我无剑·5%伤害", "无我无剑·命中续气一格",
               "吞日月·移速提高30%",
               "人剑合一·40%伤害", "人剑合一·60%伤害", "人剑合一·调息减80帧", "人剑合一·持续伤害12秒" });
    b.choices({ "负阴", "玄门", "剑入", "无欲", "故长", "风逝", "切玉", "和光", "一彻", "裂云", "虚极", "若水", "归剑", "斩勘", "云游", "剑道万象" }, {});
    b.default_talents({ "玄门", "剑入", "负阴", "无欲", "故长", "风逝", "切玉", "和光", "一彻" });
    b.resource({ "气点", 10, 0, 10, 16, 1 });
    b.state({ 378, "碎星辰", 384, 0, 1 }, false);
    b.state({ 748, "叠刃", 384, 48, 5 }, true, true, true);
    b.resource({ .name = "玄应之气", .initial = 0, .low = 0, .high = 100, .macro_alias = "rage" });
    b.state({ 0, "玄应回气", 0, 16, 1 }, false);
    b.state({ 0, "人剑合一", 0, 16, 1 }, false);
    b.state({ 0, "生太极", 384, 192, 1 }, false);
    b.state({ 2757, "紫气东来", 160, 16, 1 }, false);
    b.state({ 0, "紫气·负阴", 160, 0, 1 }, false);
    b.state({ 31577, "剑入", 192, 0, 1 }, false);
    b.state({ 0, "灵剑", 160, 0, 1 }, true);
    b.state({ 29451, "风逝", 80, 0, 4 }, true);
    const Stats                 npc{ .pve = 686 };
    // 碎星辰给使用这组修正的攻击增加 5 个百分点会心和 100/1024 会效；
    // 负阴使会心总加成达到 10 个百分点、会效达到 204/1024，并加 102/1024 基础攻击。
    const std::vector<Modifier> field{
        { b.buff("碎星辰"), 0, { .critical = 500, .power = 100 },                {}     },
        { b.buff("碎星辰"), 0, { .attack = 102, .critical = 500, .power = 104 }, "负阴" }
    };
    const auto direct = [&](int id, std::string_view name, int base, int spread, int channel, int weapon) {
        b.damage(id, name, { base, spread, channel, weapon, 0, 0, true, npc, field });
    };
    direct(383, "无我无剑·一", 2, 0, 77, 2048);
    direct(386, "无我无剑·二", 4, 0, 154, 2048);
    direct(387, "无我无剑·三", 6, 0, 231, 2048);
    direct(388, "无我无剑·四", 8, 0, 308, 2048);
    direct(389, "无我无剑·五", 11, 1, 385, 2048);
    direct(390, "无我无剑·六", 13, 1, 462, 2048);
    direct(391, "无我无剑·七", 15, 1, 539, 2048);
    direct(392, "无我无剑·八", 17, 1, 616, 2048);
    direct(393, "无我无剑·九", 20, 2, 693, 2048);
    direct(394, "无我无剑·十", 22, 2, 770, 2048);
    direct(17689, "三环套月", 12, 1, 442, 1024);
    direct(14975, "万剑归宗", 16, 1, 358, 1024);
    direct(4954, "八荒归元", 7, 0, 548, 2048);
    direct(4954, "八荒归元·半血", 11, 1, 826, 2048);
    direct(4954, "八荒归元·斩杀", 23, 2, 1034, 2048);
    b.damage(600, "叠刃(DOT)", { 1, 0, 107, 0, 8, 48, true, npc, field });
    b.dot("叠刃", "叠刃(DOT)");
    b.cast("碎星辰", 160, 24, 16, {}, { b.apply("碎星辰") });
    b.cast("三环套月", 32, 24, 0, { .hasted_cd = true, .range = 4 }, { b.hit("三环套月", 3), b.gain(0, 2) });
    b.cast("万剑归宗", 192, 24, 0, { .range = 8 }, { b.hit("万剑归宗"), b.gain(0, 1) });
    // 无我无剑先打出当前气点对应的一击，再挂一层叠刃；这一击会心则多挂一层，最后清空气点。
    b.cast("无我无剑",
           0,
           24,
           0,
           { .resource = 0, .cost = 1, .range = 4 },
           { b.tier("无我无剑·一", 0), b.apply("叠刃"), b.on_crit("叠刃"), b.gain(0, -10) });
    b.cast("八荒归元",
           192,
           24,
           0,
           { .range = 12 },
           { b.life(5000, 10000, b.hit("八荒归元")),
             b.life(2000, 5000, b.hit("八荒归元·半血")),
             b.life(-1, 2000, b.hit("八荒归元·斩杀")),
             b.life(-1, 4000, b.burst("叠刃")),
             b.gain(0, 2) });
    b.recipe("三环套月", "三环套月·2%会心", { .critical = 200 }, { "三环套月" });
    b.recipe("三环套月", "三环套月·3%会心", { .critical = 300 }, { "三环套月" });
    b.recipe("三环套月", "三环套月·3%伤害", { .damage = 31 }, { "三环套月" });
    b.recipe("三环套月", "三环套月·4%会心", { .critical = 400 }, { "三环套月" });
    b.recipe("三环套月", "三环套月·4%伤害", { .damage = 41 }, { "三环套月" });
    b.recipe("三环套月", "三环套月·5%伤害", { .damage = 51 }, { "三环套月" });
    b.recipe("八荒归元", "八荒归元·3%伤害", { .damage = 31 }, { "八荒归元", "八荒归元·半血", "八荒归元·斩杀" });
    b.recipe("八荒归元", "八荒归元·4%伤害", { .damage = 41 }, { "八荒归元", "八荒归元·半血", "八荒归元·斩杀" });
    b.recipe("八荒归元", "八荒归元·5%伤害", { .damage = 51 }, { "八荒归元", "八荒归元·半血", "八荒归元·斩杀" });
    b.recipe("无我无剑",
             "无我无剑·2%会心",
             { .critical = 200 },
             { "无我无剑·一", "无我无剑·七", "无我无剑·三", "无我无剑·九", "无我无剑·二", "无我无剑·五", "无我无剑·八", "无我无剑·六", "无我无剑·十", "无我无剑·四" });
    b.recipe("无我无剑",
             "无我无剑·3%会心",
             { .critical = 300 },
             { "无我无剑·一", "无我无剑·七", "无我无剑·三", "无我无剑·九", "无我无剑·二", "无我无剑·五", "无我无剑·八", "无我无剑·六", "无我无剑·十", "无我无剑·四" });
    b.recipe("无我无剑",
             "无我无剑·3%伤害",
             { .damage = 31 },
             { "无我无剑·一", "无我无剑·七", "无我无剑·三", "无我无剑·九", "无我无剑·二", "无我无剑·五", "无我无剑·八", "无我无剑·六", "无我无剑·十", "无我无剑·四" });
    b.recipe("无我无剑",
             "无我无剑·4%会心",
             { .critical = 400 },
             { "无我无剑·一", "无我无剑·七", "无我无剑·三", "无我无剑·九", "无我无剑·二", "无我无剑·五", "无我无剑·八", "无我无剑·六", "无我无剑·十", "无我无剑·四" });
    b.recipe("无我无剑",
             "无我无剑·4%伤害",
             { .damage = 41 },
             { "无我无剑·一", "无我无剑·七", "无我无剑·三", "无我无剑·九", "无我无剑·二", "无我无剑·五", "无我无剑·八", "无我无剑·六", "无我无剑·十", "无我无剑·四" });
    b.recipe("无我无剑",
             "无我无剑·5%伤害",
             { .damage = 51 },
             { "无我无剑·一", "无我无剑·七", "无我无剑·三", "无我无剑·九", "无我无剑·二", "无我无剑·五", "无我无剑·八", "无我无剑·六", "无我无剑·十", "无我无剑·四" });
    direct(18121, "三柴剑法", 0, 0, 21, 1024);
    b.cast("三柴剑法", 21, 0, 0, { .hasted_cd = true, .range = 4 }, { b.hit("三柴剑法") });
    b.alias("三柴剑法", "sanchai");
    b.alias("无我无剑", "wuwu");
    b.alias("三环套月", "sanhuan");
    b.alias("万剑归宗", "wanjian");
    b.alias("八荒归元", "bahuang");
    b.alias("碎星辰", "suixing");
    b.set_bonus({ .damage = 102 },
                { "无我无剑·一", "无我无剑·二", "无我无剑·三", "无我无剑·四", "无我无剑·五", "无我无剑·六", "无我无剑·七", "无我无剑·八", "无我无剑·九", "无我无剑·十" });
    b.default_gear({ ClassSet, WindPendant, WaterWeapon, BeltEnchant, "昆吾·弦刃·英雄", "刃凌·英雄" });
    // 回气秘籍在无我无剑收招后返还 2 点。因此返还的一格留到下一招，不参与本次选档。
    b.recipe("无我无剑", "无我无剑·命中续气一格", {}, {});
    b.post_cast("无我无剑", { b.selected("无我无剑·命中续气一格", b.gain(0, 2)) });
    for (auto hit : { "八荒归元", "八荒归元·半血", "八荒归元·斩杀" }) b.passive(hit, "切玉", { .damage = 102 });
    b.charge_variant("八荒归元", "和光", 192, 2);
    // 风逝只替换伤害档。和光的双充能独立于风逝，准备阶段再应用调息秘籍。
    b.variant("八荒归元", "风逝", 192, 0, { .range = 12 }, { b.hit("八荒归元·斩杀"), b.life(-1, 4000, b.burst("叠刃")), b.gain(0, 2) });
    b.variant("八荒归元", "和光", 192, 0, {.charges = 2, .range = 12}, {b.hit("八荒归元·斩杀"), b.life(-1, 4000, b.burst("叠刃")), b.gain(0, 2)}, "风逝");
    b.variant("碎星辰", "故长", 0, 16, {}, { b.apply_for("碎星辰", 576) });
    b.timing_recipe("八荒归元", "八荒归元·调息减16帧", "八荒归元", -16, 0);
    b.state({0, "吞日月", 384, 0, 1}, false);
    b.state({0, "吞日月疾行", 384, 0, 1}, false);
    b.cast("吞日月", 160, 24, 16, { .range = 20 }, { b.apply("吞日月"), b.selected("吞日月·移速提高30%", b.apply("吞日月疾行")) });
    direct(38534, "人剑合一", 6, 0, 40, 1024);
    b.state({889, "人剑持续", 192, 48, 1}, true);
    b.damage(37453, "人剑合一(DOT)", {1, 0, 450, 0, 4, 48, true, npc, field});
    b.dot("人剑持续", "人剑合一(DOT)");
    b.cast("人剑合一", 288, 24, 0, { .range = 4 }, { b.hit("人剑合一"), b.selected("人剑合一·持续伤害12秒", b.apply("人剑持续")) });
    b.recipe("吞日月", "吞日月·移速提高30%", {}, {});
    b.recipe("人剑合一", "人剑合一·40%伤害", { .damage = 410 }, { "人剑合一" });
    b.recipe("人剑合一", "人剑合一·60%伤害", { .damage = 614 }, { "人剑合一" });
    b.timing_recipe("人剑合一", "人剑合一·调息减80帧", "人剑合一", -80, 0);
    b.recipe("人剑合一", "人剑合一·持续伤害12秒", {}, {});
    b.state({1915, "寒崖听籁", 96, 0, 1}, false);
    // 寒崖听籁期间八荒免调息；普通、半血、斩杀三个伤害条目都获得 461/1024 增伤。
    b.cast("使用橙武", 800, 0, 0, {.choice = Legendary}, {b.apply("寒崖听籁")});
    b.no_cooldown_during("八荒归元", "寒崖听籁");
    for (auto hit : {"八荒归元", "八荒归元·半血", "八荒归元·斩杀"}) b.pre_damage(hit, {b.buff("寒崖听籁"), 0, {.damage = 461}});
    b.legendary_damage({ "无我无剑·一", "无我无剑·二", "无我无剑·三", "无我无剑·四", "无我无剑·五", "无我无剑·六", "无我无剑·七", "无我无剑·八", "无我无剑·九", "无我无剑·十", "八荒归元", "八荒归元·半血", "八荒归元·斩杀" });
    // 增益气场每秒回 4 点玄应，两种气场重叠只回一次；人剑期间改为每秒扣 4 点。
    b.initial("玄应回气");
    b.pulse("玄应回气", {{.op = Op::School, .slot = b.buff("碎星辰"), .extra = b.buff("生太极"),
                          .blocked_by = b.buff("人剑合一"), .effect = [](EffectContext &f, const Step &s) {
                              if (f.Stacks(s.slot) || f.Stacks(s.extra)) f.ChangeResource(1, 4);
                          }}});
    b.pulse("人剑合一", {b.selected("玄门", b.gain(0, 10)), b.gain(1, -4), b.at(1, 0, 0, b.remove("人剑合一"))});
    // 淳和未分：处于自身生太极的目标每 12 秒获得一次叠刃，单木桩按气场中心到目标距离检查。
    b.pulse("生太极", {b.within(10, b.apply("叠刃"))});
    b.cast("生太极", 160, 24, 16, {}, {b.apply("生太极"), b.gain(0, 2)});
    b.variant("生太极", "玄门", 160, 16, {}, {b.apply_for("生太极", 464), b.gain(0, 2)});
    for (auto name : {"碎星辰", "吞日月"}) b.post_cast(name, {b.gain(0, 2)});
    // 人剑状态内气场瞬发；变体保持各自持续时间及秘籍效果。
    for (auto name : {"生太极", "碎星辰", "吞日月"}) b.base_action(name).alternate_instant = b.buff("人剑合一");
    for (auto &variant : school.variants)
        if (variant.skill.name == "生太极" || variant.skill.name == "碎星辰") variant.action.alternate_instant = b.buff("人剑合一");
    b.post_cast("人剑合一", {b.at(1, 20, 120, b.when("人剑合一", b.apply("人剑合一"), true))});
    b.cast("人剑合一·解", 1, 0, 0, {.required = b.buff("人剑合一"), .choice = "玄门"}, {b.remove("人剑合一"), b.gain(1, -120)});

    // 负阴在开紫气时读取碎星辰。无欲保留开招聚满气，只替换其后的每秒回气。
    b.cast("紫气东来", 1200, 8, 0, {.charges = 2, .gcd_group = 1}, {b.gain(0, 10), b.apply("紫气东来"),
           b.remove("紫气·负阴"), b.selected("负阴", b.when("碎星辰", b.apply("紫气·负阴"))), b.selected("剑入", b.apply("剑入"))});
    b.charge_variant("紫气东来", "若水", 1200, 3);
    b.pulse("紫气东来", {b.gain(0, 2)});
    direct(40752, "一彻", 8, 1, 260, 0);
    for (auto name : {"剑入", "剑入·引爆"}) {
        const bool burst = std::string_view(name) == "剑入·引爆";
        b.damage(burst ? 34694 : 34693, name, {7, 2, burst ? 850 : 765, 0, 0, 0, true, {.pve = 686 + 1536}, field});
    }
    // 八荒先处理场上旧剑。若再次开紫气已有新剑入，留给下一次八荒落剑。
    b.post_cast("八荒归元", {{.op = Op::School, .slot = b.buff("灵剑"), .choice = "剑入", .extra = b.buff("剑入"),
        .neutral_buff = b.effect("剑入"), .neutral_dot = b.effect("剑入·引爆"), .effect = [](EffectContext &f, const Step &s) {
            if (f.Stacks(s.slot)) { f.Hit(s.neutral_dot); f.SetAura(s.slot, -1); }
            else if (f.Stacks(s.extra)) { f.Hit(s.neutral_buff); f.SetAura(s.extra, -1); f.SetAura(s.slot, 1); }
        }}});
    b.pre_cast("无我无剑", {b.selected("风逝", b.at(0, 10, 10, b.when("人剑合一", b.shorten("八荒归元", 8)))),
                            b.selected("风逝", b.at(0, 10, 10, b.when("人剑合一", b.apply("风逝"))))});
    b.post_cast("无我无剑", {b.selected("一彻", b.when("人剑合一", b.hit("一彻")))});
    b.post_cast("八荒归元", {b.selected("切玉", b.when("紫气东来", b.life(4000, 7499, b.burst("叠刃")))), b.remove("风逝", 4)});
    for (auto name : {"八荒归元", "八荒归元·半血", "八荒归元·斩杀"}) {
        for (int n = 1; n <= 4; ++n) b.pre_damage(name, {b.buff("风逝"), n, {.damage = 512*n}, "风逝"});
    }
    b.passive("叠刃(DOT)", "虚极", {.damage = 461});
    b.passive("万剑归宗", "虚极", {.critical = 1000, .power = 102});
    b.post_damage("万剑归宗", {b.selected("虚极", b.apply("叠刃"))});
    // 斩勘按命中前目标身上的叠刃档位造成一段独立伤害，每次扣 4 点玄应。
    direct(42061, "天道剑气·斩勘", 8, 1, 79, 0);
    b.ranks("天道剑气·斩勘", {79,159,239,319,399,479,559});
    b.rank_bases("天道剑气·斩勘", {{8,1},{2,1},{3,2},{4,3},{4,4},{5,4},{6,5}});
    Step mark{.op = Op::School, .slot = b.effect("天道剑气·斩勘"), .condition = b.buff("人剑合一"), .choice = "斩勘",
              .extra = b.buff("叠刃"), .resource = 1, .minimum = 4, .maximum = 120, .effect = [](EffectContext &frame, const Step &step) {
                  if (const int layers = frame.Stacks(step.extra)) {
                      frame.ChangeResource(1, -4);
                      frame.Hit(step.slot, layers);
                      if (!frame.Resource(1)) frame.SetAura(step.condition, -1);
                  }
              }};
    for (auto name : {"三环套月", "万剑归宗", "八荒归元", "八荒归元·半血", "八荒归元·斩杀",
                      "无我无剑·一", "无我无剑·二", "无我无剑·三", "无我无剑·四", "无我无剑·五",
                      "无我无剑·六", "无我无剑·七", "无我无剑·八", "无我无剑·九", "无我无剑·十"}) b.post_damage(name, {mark});
    // 归剑以 20 玄应回收气场至自身；单桩模型中气场已经位于脚下。
    b.pre_cast("万剑归宗", {b.selected("归剑", b.at(1, 20, 120, b.gain(1, -20)))});
    // 自身气场召出的两种云中剑各自存在 8 秒、独立跳伤。云游的三环加成只取一次。
    b.state({31522, "云游", 128, 0, 1}, false);
    for (auto [field_name, cloud, id, radius] : {std::tuple{"碎星辰", "云中剑·碎星辰", 14983, 5}, {"吞日月", "云中剑·吞日月", 14984, 4}}) {
        b.state({id, cloud, 128, 16, 1}, false);
        direct(21979, cloud, 4, 1, 165, 0);
        b.post_cast("人剑合一", {b.when(field_name, b.apply(cloud)), b.selected("云游", b.when(field_name, b.apply("云游")))});
        b.pulse(cloud, {b.within(radius, b.hit(cloud))});
        b.post_damage(cloud, {b.selected("云游", b.when("紫气东来", b.apply("叠刃"))), b.selected("云游", b.when("紫气东来", b.gain(0, 2)))});
    }
    b.pre_damage("三环套月", {b.buff("云游"), 0, {.damage = 512}, "云游"});
    // 全心法增益在所有本门伤害（含人剑合一与其 DOT）登记完成后绑定。
    b.school_modifier({b.buff("碎星辰"), 0, {.ignore = 614}, "故长"});
    b.school_modifier({b.buff("紫气东来"), 0, {.attack = 256, .critical = 2500, .uncapped_power = 256}});
    b.school_modifier({b.buff("紫气东来"), 0, {.attack = 256, .strain_rating = 307}, "无欲"});
    b.school_modifier({b.buff("紫气·负阴"), 0, {.critical = -1500, .uncapped_power = 307}});
    for (int layers = 4; layers <= 7; ++layers)
        b.school_modifier({.aura = b.buff("叠刃"), .stacks = layers, .stats = {.power = 154}, .choice = "裂云", .max_distance = 15});
    b.base_action("三环套月").range_aura = b.base_action("无我无剑").range_aura = b.buff("人剑合一");
    b.base_action("三环套月").aura_range = b.base_action("无我无剑").aura_range = 8;
    b.adjust([](Prepared &p) {
        const auto &book = *p.config.book;
        auto skill = [&](std::string_view n) { return std::ranges::find(book.skills, n, &SkillDescriptor::name) - book.skills.begin(); };
        auto aura = [&](std::string_view n) { return std::ranges::find(book.buffs, n, &BuffDescriptor::name) - book.buffs.begin(); };
        auto chosen = [&](std::string_view n) { return std::ranges::contains(p.config.talents, n); };
        if (chosen("和光")) p.skills[skill("八荒归元")].cooldown -= 48;
        if (chosen("无欲")) p.pulses[aura("紫气东来")].clear();
        if (chosen("若水")) {
            p.skills[skill("紫气东来")].cooldown -= 240;
            for (auto &step : p.impacts[skill("紫气东来")])
                if (step.op == Op::Add && (step.slot == aura("紫气东来") || step.slot == aura("紫气·负阴"))) step.duration = 96;
        }
        if (chosen("玄门")) p.resources[1].high += 20;
        if (p.config.precombat) { p.resources[1].initial = p.resources[1].high; p.opening.push_back({Op::Add, static_cast<Slot>(aura("碎星辰"))}); }
        if (chosen("剑道万象"))
            for (auto n : {"云中剑·碎星辰", "云中剑·吞日月"}) p.base_intervals[aura(n)] = 8;
        if (chosen("裂云")) p.aura_stack_caps[aura("叠刃")] = 7;
        if (chosen("虚极")) p.skills[skill("万剑归宗")].cooldown -= 32;
        if (chosen("归剑")) { p.skills[skill("万剑归宗")].cooldown += 112; p.skills[skill("万剑归宗")].gcd = 0; }
    });
    return b.finish();
}
} // namespace JX3DPS::runtime::season50
