#include "common/builder.hpp"

namespace JX3DPS::runtime::season50 {
Book 傲血战意()
{
    Builder b("axzy", "傲血战意", R"(# 魔盒宏适配 https://www.jx3box.com/macro/111255
/cast 使用风腰坠
/cast 使用橙武
/cast [nobuff:骑御&buff:牧云&skill_cd:撼如雷<1.5&战意>2] 任驰骋
/cast [buff:骑御] 撼如雷
/cast [buff:骑御&战意<3] 断魂刺
/cast [buff:骑御&nobuff:龙驭] 下马
/cast [buff:骑御&nobuff:夜征&skill_cd:撼如雷>2] 下马
/cast [nobuff:骑御] 突
/cast [nobuff:牧云] 龙吟
/cast [战意<3] 灭
/cast [nobuff:骑御&战意=2] 穿云
/cast [战意<4] 龙吟
/cast [战意>4] 龙牙
/cast [buff:骑御&战意=4&bufftime:夜征<1.5] 龙牙
/cast [战意=4] 穿云
/cast [buff:骑御&战意=4] 战八方
)");
    auto &school = b.definition();
    school.info = { school.code, "天策", "力道", "Physics", 563, 0, 205 };
    school.display_order = 2;
    school.saved.macro_digest = "ed87668ffe3d261042abd4c90eb5bf4fed7a3f1e6a4f9b78b598e638da21bcb0";
    school.saved.talents_v7 = { "龙血", "战心", "镇远", "风虎" };
    school.saved.talents_v6 = { "龙血", "战心" };
    school.weapon_effect = "神兵附伤";
    school.raid.provides_attack_buff = true;
    b.preset(2300,
             { 6860, 1264, 193, 1260, 208, 1750, 4212, 0, 1092 },
             { "穿云·3%会心", "穿云·2%会心", "穿云·4%伤害", "穿云·3%伤害", "龙吟·3%伤害", "龙吟·5%伤害", "龙吟·4%伤害", "龙牙·5%伤害", "龙牙·4%伤害", "龙牙·3%伤害", "龙牙·4%会心",
               "战八方·5%伤害（一）", "战八方·5%伤害（二）", "龙吟·调息减1秒", "灭·调息减1秒", "灭·3%会心", "灭·4%伤害", "灭·3%伤害",
               "突·4%伤害", "突·3%伤害", "断魂刺·4%伤害", "断魂刺·3%伤害", "断魂刺·调息减1秒（一）", "断魂刺·调息减1秒（二）" });
    b.choices({ "龙驭", "破楼兰", "镇远", "牧云", "驰骋", "夜征", "战心", "龙血", "风虎", "青阳" }, {}, { "神兵附伤" });
    b.default_talents({ "龙驭", "破楼兰", "镇远", "牧云", "驰骋", "夜征", "战心", "龙血", "风虎" });
    b.resource({ "战意", 5, 0, 5 });
    b.state({ 0, "激雷", 96, 0, 1 }, false, false, false);
    b.state({ 0, "流血", 224, 32, 2 }, true, true, true);
    b.state({ 0, "骑御", 0, 0, 1 }, false);
    b.state({ 0, "龙驭", 0, 0, 25 }, false);
    b.state({ 0, "骑乘龙牙", 1, 0, 1 }, false);
    b.state({ 0, "驰骋", 240, 0, 1 }, false);
    b.state({ 0, "牧云", 240, 0, 1 }, false);
    b.state({ 0, "夜征", 192, 0, 1 }, false);
    b.state({ 0, "破楼兰机会", 128, 0, 1 }, false);
    b.state({ 0, "破风", 224, 0, 1 }, true);
    b.damage(0,
             "流血(DOT)",
             { 1, 0, 962, 0, 7, 32, true, { .pve = 451 }, { { b.buff("激雷"), 0, { .attack = 205, .critical = 3000, .overcome = 205 }, "" } } });
    b.damage(14887,
             "战八方·一",
             { 9, 1, 290, 1024, 0, 0, true, { .pve = 451 }, { { b.buff("激雷"), 0, { .attack = 205, .critical = 3000, .overcome = 205 }, "" } } });
    b.damage(46148,
             "战八方·二三",
             { 9, 1, 290, 0, 0, 0, true, { .pve = 451 }, { { b.buff("激雷"), 0, { .attack = 205, .critical = 3000, .overcome = 205 }, "" } } });
    b.damage(18603,
             "龙吟",
             { 19, 1, 639, 1024, 0, 0, true, { .pve = 451 }, { { b.buff("激雷"), 0, { .attack = 205, .critical = 3000, .overcome = 205 }, "" } } });
    b.damage(18208,
             "穿云·一",
             { 16, 1, 427, 1024, 0, 0, true, { .pve = 451 }, { { b.buff("激雷"), 0, { .attack = 205, .critical = 3000, .overcome = 205 }, "" } } });
    b.damage(18207,
             "穿云·二",
             { 16, 1, 427, 1024, 0, 0, true, { .pve = 451 }, { { b.buff("激雷"), 0, { .attack = 205, .critical = 3000, .overcome = 205 }, "" } } });
    b.damage(18773,
             "龙牙",
             { 22, 1, 851, 1024, 0, 0, true, { .pve = 451 }, { { b.buff("激雷"), 0, { .attack = 205, .critical = 3000, .overcome = 205 }, "" } } });
    b.damage(15002,
             "龙牙·龙血",
             { 2, 1, 255, 1024, 0, 0, true, { .pve = 451 }, { { b.buff("激雷"), 0, { .attack = 205, .critical = 3000, .overcome = 205 }, "" } } });
    b.secondary_damage(
        25772,
        "龙牙·神兵",
        { 0, 2, 50, 0, 0, 0, true, { .pve = 451 }, { { b.buff("激雷"), 0, { .attack = 205, .critical = 3000, .overcome = 205 }, "" } } });
    b.dot("流血", "流血(DOT)");
    b.cast("撼如雷", 480, 0, 0, {}, { b.apply("激雷", 1) });
    b.variant("撼如雷", "夜征", 480, 0, {}, { b.apply("夜征") });
    b.cast("任驰骋", 640, 24, 12, { .forbidden = b.buff("骑御"), .charges = 2 },
           { b.apply("骑御"), b.apply("驰骋"), b.reset("断魂刺"), b.selected("龙驭", b.apply("龙驭", 6)) });
    b.charge_variant("任驰骋", "龙驭", 544, 2);
    b.cast("下马", 1, 0, 0, { .required = b.buff("骑御") }, { b.remove("骑御") });
    b.alias("下马", "骑御");
    b.cast("突", 288, 0, 0, {}, { b.remove("骑御"), b.hit("突") });
    b.cast("断魂刺", 400, 0, 0, { .required = b.buff("骑御") }, { b.gain(0, 3), b.hit("断魂刺") });
    b.cast("战八方", 0, 24, 0, {}, { b.hit("战八方·一", 1), b.later("战八方·二三", 4), b.later("战八方·二三", 8) });
    b.cast("龙吟", 128, 24, 0, {}, { b.hit("龙吟", 1), b.apply("流血", 1), b.gain(0, 2) });
    b.cast("穿云", 32, 24, 0, { .hasted_cd = true, .chain_window = 96 }, { b.cycle("穿云·一", 2), b.gain(0, 1) });
    b.cast("龙牙",
           24,
           24,
           0,
           { .resource = 0, .cost = 3, .hasted_cd = true, .capture = b.buff("夜征") },
           { b.hit("龙牙", 1),
             b.selected("龙血", b.when("流血", b.hit("龙牙·龙血", 1), false)),
             b.gain(0, -3),
             b.selected("神兵附伤", b.proc("龙牙·神兵", 256)) });
    // 穿云的会心秘籍同时作用于一段和二段，每一段独立判定会心。
    b.recipe("穿云", "穿云·2%会心", { .critical = 200 }, { "穿云·一", "穿云·二" });
    b.recipe("穿云", "穿云·3%会心", { .critical = 300 }, { "穿云·一", "穿云·二" });
    b.recipe("穿云", "穿云·3%伤害", { .damage = 31 }, { "穿云·一", "穿云·二" });
    b.recipe("穿云", "穿云·4%伤害", { .damage = 41 }, { "穿云·一", "穿云·二" });
    b.recipe("龙吟", "龙吟·3%伤害", { .damage = 31 }, { "龙吟" });
    b.recipe("龙吟", "龙吟·4%伤害", { .damage = 41 }, { "龙吟" });
    b.recipe("龙吟", "龙吟·5%伤害", { .damage = 51 }, { "龙吟" });
    b.recipe("龙牙", "龙牙·2%会心", { .critical = 200 }, { "龙牙", "龙牙·龙血" });
    b.recipe("龙牙", "龙牙·3%会心", { .critical = 300 }, { "龙牙", "龙牙·龙血" });
    b.recipe("龙牙", "龙牙·3%伤害", { .damage = 31 }, { "龙牙", "龙牙·龙血" });
    b.recipe("龙牙", "龙牙·4%会心", { .critical = 400 }, { "龙牙", "龙牙·龙血" });
    b.recipe("龙牙", "龙牙·4%伤害", { .damage = 41 }, { "龙牙", "龙牙·龙血" });
    b.recipe("龙牙", "龙牙·5%伤害", { .damage = 51 }, { "龙牙", "龙牙·龙血" });
    const std::vector<Modifier> thunder{
        { b.buff("激雷"), 0, { .attack = 205, .critical = 3000, .overcome = 205 }, {} }
    };
    b.damage(702, "灭", { 20, 1, 418, 1024, 0, 0, true, { .pve = 451 }, thunder });
    b.damage(701, "灭·斩杀", { 20, 1, 628, 1024, 0, 0, true, { .pve = 451 }, thunder });
    b.cast("灭",
           128,
           24,
           0,
           {},
           { b.life(5000, 10000, b.hit("灭")),
             b.life(-1, 5000, b.hit("灭·斩杀")),
             b.selected("战心", b.apply("流血")),
             b.selected("战心", b.gain(0, 3)) });
    b.set_bonus({ .damage = 51 }, { "龙牙·龙血", "龙牙" });
    b.state({ 0, "破军", 64, 0, 1 }, true);
    b.state({ 0, "风虎", 0, 0, 5 }, false);
    b.damage(44364, "破罡", { 16, 31, 300, 0, 0, 0, true, { .pve = 451 }, thunder });
    b.damage(24009, "破军", { 16, 31, 84, 1024, 0, 0, true, { .pve = 451 }, thunder });
    b.damage(431, "突", { 3, 1, 16, 1024, 0, 0, true, { .pve = 451 }, thunder });
    b.damage(409, "断魂刺", { 3, 1, 16, 1024, 0, 0, true, { .pve = 451 }, thunder });
    b.damage(0, "夜征", { 22, 1, 553, 1024, 0, 0, true, { .pve = 451 }, thunder });
    b.damage(6525, "灭·破楼兰·一", { 11, 1, 628, 1024, 0, 0, true, { .pve = 451 }, thunder });
    b.damage(6526, "灭·破楼兰·二", { 5, 6, 418, 256, 0, 0, true, { .pve = 451 }, thunder });
    b.pre_cast("龙吟", { b.selected("牧云", b.apply("牧云")), b.apply("破风") });
    // 破楼兰在灭收招时按目标气血选择追击，50% 及以下打一段。
    // 机会标记每次翻转，原先没有机会时重置灭的调息。
    b.post_cast("灭", { b.selected("战心", b.apply("破风")),
                         b.selected("破楼兰", b.life(-1, 5000, b.hit("灭·破楼兰·一"))),
                         b.selected("破楼兰", b.life(5000, 10000, b.hit("灭·破楼兰·二"))),
                         b.selected("破楼兰", b.when("破楼兰机会", b.reset("灭"), true)),
                         b.selected("破楼兰", { Op::ToggleAura, b.buff("破楼兰机会") }) });
    // 龙牙起手先给目标挂破军并打破罡；骑御与龙驭同时存在才保留骑乘标记，让这次龙牙读取易伤。
    b.pre_cast("龙牙", { b.apply("破军"), b.hit("破罡"), b.when("骑御", b.apply("骑乘龙牙")), b.when("龙驭", b.remove("骑乘龙牙"), true) });
    b.post_cast("龙牙", { b.when("骑乘龙牙", b.remove("龙驭")) });
    // 骑乘龙牙收招先扣一层龙驭；仍有龙驭且起手捕获了夜征时再打夜征，夜征命中后再扣一层。
    b.post_damage("夜征", { b.remove("龙驭") });
    b.post_cast("龙牙", { b.when("龙驭", b.remove("骑乘龙牙"), true),
                          b.captured(b.when("骑乘龙牙", b.hit("夜征"))), b.remove("骑乘龙牙"),
                          b.selected("龙驭", b.when("骑御", b.apply("龙驭"), true)) });
    b.pre_damage("龙牙", { b.buff("骑乘龙牙"), 0, { .fragile = 461 }, "龙驭" });
    // 列出的招式每次命中都检查目标的破军并追击一次，穿云等多段招式可逐段触发。
    for (auto n : { "穿云·一", "穿云·二", "龙吟", "龙牙", "灭", "灭·斩杀", "战八方·一", "战八方·二三", "破罡", "突", "断魂刺" }) {
        b.post_damage(n, { b.when("破军", b.hit("破军")) });
    }
    for (auto n : { "破罡", "破军" }) {
        b.passive(n, "镇远", { .fragile = 614 });
    }
    for (auto n : { "龙吟", "龙牙", "穿云", "灭" }) {
        b.pre_cast(n, { b.selected("风虎", b.resource_stacks("风虎", 0)) });
    }
    for (auto n : { "龙吟", "龙牙", "穿云·一", "穿云·二", "灭", "灭·斩杀", "龙牙·龙血", "灭·破楼兰·一", "灭·破楼兰·二" }) {
        for (int i = 1; i <= 5; ++i) {
            b.pre_damage(n, { b.buff("风虎"), i, { .damage = (i * 1024 + 10) / 20 }, "风虎" });
        }
    }
    b.damage(12, "梅花枪法", { 0, 0, 27, 1024, 0, 0, true, { .pve = 451 }, thunder });
    b.state({ 0, "梅花枪法计时", 0, 27, 1 }, false, true, true);
    b.tick_schedule("梅花枪法计时", 0, 1);
    b.defer_swing("任驰骋", "梅花枪法计时");
    b.pulse("梅花枪法计时", { b.hit("梅花枪法") });
    for (auto action : { "龙吟", "灭", "突", "龙牙", "战八方", "穿云", "断魂刺" }) {
        b.pre_cast(action, { b.when("梅花枪法计时", b.hit("梅花枪法"), true), b.when("梅花枪法计时", b.apply("梅花枪法计时"), true) });
    }
    b.secondary_damage(31031, "画角闻龙", {496, 52, 615, 0, 0, 0, true, {.pve = 451}, thunder});
    for (auto hit : { "灭", "灭·斩杀", "灭·破楼兰·一", "灭·破楼兰·二", "龙牙", "龙牙·龙血", "穿云·一", "穿云·二", "龙吟", "突", "断魂刺",
                      "战八方·一", "战八方·二三", "破军", "破罡", "夜征", "龙牙·神兵", "画角闻龙", "流血(DOT)", "梅花枪法" }) {
        b.passive(hit, {}, { .attack = 51 });
        // 团队满增益已维护破风减防时，公共结算跳过个人这份 280 点减防。
        b.pre_damage(hit, { .aura = b.buff("破风"), .stats = { .shield_flat = -280 }, .replaced_by_raid = true });
        b.pre_damage(hit, { b.buff("牧云"), 0, { .critical = 600, .power = 256, .strain_rating = 204 } });
        b.pre_damage(hit, { b.buff("驰骋"), 0, { .attack = 154 } });
        b.pre_damage(hit, { b.buff("驰骋"), 0, { .attack = 153 }, "驰骋" });
        b.pre_damage(hit, { b.buff("夜征"), 0, { .critical = 3000, .ignore = 717 } });
    }
    b.recipe("战八方", "战八方·5%伤害（一）", { .damage = 51 }, { "战八方·一", "战八方·二三" });
    b.recipe("战八方", "战八方·5%伤害（二）", { .damage = 51 }, { "战八方·一", "战八方·二三" });
    b.recipe("灭", "灭·3%会心", { .critical = 300 }, { "灭", "灭·斩杀", "灭·破楼兰·一", "灭·破楼兰·二" });
    b.recipe("灭", "灭·4%伤害", { .damage = 41 }, { "灭", "灭·斩杀", "灭·破楼兰·一", "灭·破楼兰·二" });
    b.recipe("灭", "灭·3%伤害", { .damage = 31 }, { "灭", "灭·斩杀", "灭·破楼兰·一", "灭·破楼兰·二" });
    b.recipe("突", "突·4%伤害", { .damage = 41 }, { "突" });
    b.recipe("突", "突·3%伤害", { .damage = 31 }, { "突" });
    b.recipe("断魂刺", "断魂刺·4%伤害", { .damage = 41 }, { "断魂刺" });
    b.recipe("断魂刺", "断魂刺·3%伤害", { .damage = 31 }, { "断魂刺" });
    b.timing_recipe("断魂刺", "断魂刺·调息减1秒（一）", "断魂刺", -16, 0);
    b.timing_recipe("断魂刺", "断魂刺·调息减1秒（二）", "断魂刺", -16, 0);
    b.timing_recipe("灭", "灭·调息减1秒", "灭", -16, 0);
    b.timing_recipe("龙吟", "龙吟·调息减1秒", "龙吟", -16, 0);
    b.adjust([](Prepared &p) {
        if (!std::ranges::contains(p.config.talents, "驰骋")) {
            const auto &book = *p.config.book;
            auto slot = std::ranges::find(book.skills, "灭", &SkillDescriptor::name) - book.skills.begin();
            auto horse = static_cast<Slot>(std::ranges::find(book.buffs, "骑御", &BuffDescriptor::name) - book.buffs.begin());
            p.pre_cast[slot].push_back({ Op::Remove, horse });
        }
    });
    b.state({1911, "寒锋凌日", 96, 0, 1}, false);
    b.cast("使用橙武", 800, 0, 0, {.choice = Legendary}, {b.apply("寒锋凌日"), b.gain(0, 5)});
    b.no_cooldown_during("龙吟", "寒锋凌日");
    b.post_cast("龙牙", {b.when("寒锋凌日", b.hit("画角闻龙")), b.when("寒锋凌日", b.gain(0, 5))});
    b.legendary_damage({ "穿云·一", "穿云·二", "龙吟" });
    // 灭命中时读取已有流血，每层打一段青阳；本次战心追加的流血要等灭本体结算后才挂。
    b.secondary_damage(32909, "青阳", {7, 1, 156, 0, 0, 0, true, {.pve = 451}});
    b.passive("流血(DOT)", "青阳", {.damage = 102});
    for (auto hit : {"灭", "灭·斩杀"}) b.post_damage(hit, {b.selected("青阳", b.hits_per_stack("青阳", "流血"))});
    return b.finish();
}
} // namespace JX3DPS::runtime::season50
