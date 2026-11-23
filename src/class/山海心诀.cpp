#include "common/builder.hpp"

namespace JX3DPS::runtime::season50 {

// 同一兽的唤灵印刷新时只更新持续时间，首次获得该兽的印才增加印数。
static void 获得唤灵印(EffectContext &frame, const Step &step)
{
    const Slot seal = step.slot + frame.Stacks(step.extra) - 1;
    if (!frame.Stacks(seal)) frame.ChangeResource(step.resource, 1);
    frame.SetAura(seal, 1, step.duration);
}

static void 消耗唤灵印(EffectContext &frame, const Step &step)
{
    int spent = 0;
    for (Slot i = step.slot; i < step.slot + step.maximum && spent < step.amount; ++i) {
        if (!frame.Stacks(i)) continue;
        frame.SetAura(i, -1);
        ++spent;
    }
    frame.ChangeResource(step.resource, -spent);
}

Book 山海心诀()
{
    Builder b("shxj", "山海心诀", R"(# 魔盒宏适配 https://www.jx3box.com/macro/107307
/cast 使用风腰坠
/cast [箭矢=8&buff:首次汇灵|last_skill:没石饮羽|last_skill:引风唤灵] 聚势摧霆
/cast 使用橙武
/cast [箭矢<3|buff:首次汇灵] 汇灵合契
/cast [箭矢>=7|箭矢=0] 引风唤灵
/cast [唤灵印<4] 弛律召野
/cast [箭矢=3|箭矢=0] 金乌见坠
/cast [箭矢=7] 弛风鸣角
/cast 没石饮羽
/cast [箭矢<=2] 聚势摧霆
/cast [箭矢=8|箭矢=3] 劲风簇
/cast [箭矢<8&箭矢>3] 饮羽簇
/cast [箭矢<3] 寒更晓箭
/cast 劲风簇
)");
    auto &school = b.definition();
    school.info = { school.code, "万灵", "身法", "Physics", 717, 51, 0 };
    school.display_order = 11;
    school.saved.macro_digest = "25102961b4d02df43acc2ddd8689efa3199977fb1253092eae348ca9b8594d73";
    school.saved.talents_v7 = { "聚势摧霆", "贯侯", "桑柘", "于狩", "佩弦" };
    school.saved.talents_v6 = { "聚势摧霆", "贯侯", "桑柘" };
    school.saved.added_recipes_v7 = { "饮羽簇·3%伤害" };
    school.saved.removed_recipes_v7 = { "饮羽簇·4%会心" };
    school.precombat = true;
    school.weapon_effect = "月弦激星";
    b.preset(2301,
             { 6045, 1611, 170, 2346, 208, 1153, 4212, 0, 1092 },
             { "劲风簇·3%伤害", "劲风簇·4%会心", "劲风簇·3%会心", "劲风簇·2%伤害",
               "饮羽簇·3%伤害", "饮羽簇·调息减24帧（一）", "饮羽簇·调息减24帧（二）", "饮羽簇·重置白羽",
               "引风唤灵·调息减1秒（一）", "引风唤灵·调息减1秒（二）", "引风唤灵·调息减1秒（三）", "引风唤灵·秘境不强仇",
               "汇灵合契·四层承契", "游雾乘云·调息减2秒", "游雾乘云·调息减3秒", "游雾乘云·获得承契" });
    b.choices({ "聚势摧霆", "贯侯", "桑柘", "于狩", "佩弦", "襄尺", "连珠", "领胡", "卢令", "朱厌" }, {}, { "月弦激星" });
    b.default_talents({ "聚势摧霆", "贯侯", "襄尺", "桑柘", "佩弦", "连珠", "领胡", "于狩", "卢令" });
    b.resource({ "箭矢", 8, 0, 8 });
    b.state({ 0, "橙武", 144, 0, 1 }, false, false, false);
    b.state({ 0, "橙武劲风", 128, 0, 1 }, false, false, false);
    b.state({ 0, "劲风簇追", 96, 0, 1 }, false, false, false);
    b.state({ 0, "饮羽簇追", 104, 0, 1 }, false, false, false);
    b.damage(35866,
             "劲风簇",
             { 180, 15, 185, 1024, 0, 0, true, { .pve = 133 }, { { .aura = b.buff("橙武劲风"), .stacks = 0, .stats = {}, .choice = "", .channel_scale = 1.3 } } });
    b.damage(39092, "饮羽簇", { 73, 1, 439, 2048, 0, 0, true, { .pve = 133 }, {} });
    b.secondary_damage(36580, "月弦激星", { 20, 2, 744, 0, 0, 0, true, { .pve = 133 }, {} });
    b.state({ 0, "金乌箭", 0, 0, 8 }, false);
    b.cast("使用橙武", 800, 0, 0, { .choice = "月弦激星" }, { b.gain(0, 8), b.apply("金乌箭", 8), b.apply("橙武", 1), b.apply("橙武劲风", 1), b.reset("饮羽簇") });
    b.cast("劲风簇",
           0,
           24,
           0,
           { .resource = 0, .cost = 1 },
           { b.hit("劲风簇", 1), b.remove("饮羽簇追", 1000), b.apply("劲风簇追", 1), b.gain(0, -1) });
    b.state({ 0, "佩弦", 160, 0, 1 }, false);
    b.cast("饮羽簇",
           160,
           24,
           32,
           { .resource = 0, .cost = 1, .instant = b.buff("橙武"), .cooldown_on_finish = true, .alternate_instant = b.buff("佩弦"), .precombat_cast = true },
           { b.remove("佩弦"),
             b.hit("饮羽簇", 1),
             b.when("橙武", b.hit("月弦激星", 1), false),
             b.remove("劲风簇追", 1000),
             b.apply("饮羽簇追", 1),
             b.gain(0, -1) });
    b.cast("寒更晓箭", 0, 24, 0, {}, { b.gain(0, 8) });
    b.recipe("劲风簇", "劲风簇·2%伤害", { .damage = 20 }, { "劲风簇" });
    b.recipe("劲风簇", "劲风簇·3%会心", { .critical = 300 }, { "劲风簇" });
    b.recipe("劲风簇", "劲风簇·3%伤害", { .damage = 31 }, { "劲风簇" });
    b.recipe("劲风簇", "劲风簇·4%会心", { .critical = 400 }, { "劲风簇" });
    b.recipe("饮羽簇", "饮羽簇·2%伤害", { .damage = 20 }, { "饮羽簇" });
    b.recipe("饮羽簇", "饮羽簇·3%会心", { .critical = 300 }, { "饮羽簇" });
    b.recipe("饮羽簇", "饮羽簇·3%伤害", { .damage = 31 }, { "饮羽簇" });
    b.recipe("饮羽簇", "饮羽簇·4%会心", { .critical = 400 }, { "饮羽簇" });
    b.state({ 0, "标鹄", 160, 0, 4 }, true);
    b.state({ 26856, "贯穿", 32, 8, 6 }, true, false, true);
    b.damage(36157, "标鹄", { 3, 2, 553, 0, 0, 0, true, { .pve = 133 }, {} });
    b.passive("标鹄", "贯侯", { .pve = 338 });
    b.damage(0, "贯穿(DOT)", { 1, 0, 152, 0, 4, 8, true, { .pve = 133 }, {} });
    b.dot("贯穿", "贯穿(DOT)");
    b.extra_tick("贯穿", "桑柘");
    b.damage(40347, "聚势摧霆", { 3, 1, 291, 0, 0, 0, true, { .pve = 2181 }, {} });
    b.damage(40720, "聚势摧霆·蓄力", { 3, 1, 154, 0, 0, 0, true, { .pve = 133 }, {} });
    // 劲风簇、饮羽簇和聚势摧霆命中后先查旧标鹄：已有四层就触发标鹄，再为本次命中叠层。
    for (auto arrow : { "劲风簇", "饮羽簇", "聚势摧霆" }) {
        b.post_damage(arrow, { b.stacks("标鹄", 4, b.hit("标鹄")), b.apply("标鹄") });
    }
    b.post_damage("标鹄", { b.remove("标鹄", 4), b.selected("贯侯", b.apply("标鹄")), b.apply("贯穿"), b.selected("桑柘", b.apply("贯穿")) });
    b.post_damage("聚势摧霆·蓄力", { b.apply("贯穿") });
    b.state({ 0, "白羽共调", 72, 0, 1 }, false);
    b.cast("弛风鸣角",
           0,
           24,
           0,
           { .resource = 0, .cost = 1, .required = b.buff("劲风簇追"), .forbidden = b.buff("白羽共调"), .pulses = 3, .spacing = 5, .pre_cast = { b.remove("劲风簇追") } },
           { b.at(0, 1, 8, b.hit("劲风簇")), b.gain(0, -1) });
    b.cast("没石饮羽",
           0,
           24,
           0,
           { .resource = 0, .cost = 3, .required = b.buff("饮羽簇追"), .forbidden = b.buff("白羽共调") },
           // 没石饮羽出手花三箭，三支饮羽簇分别排在 20、26、32 帧后命中。
           { b.remove("饮羽簇追"), b.gain(0, -3), b.later("饮羽簇", 20), b.later("饮羽簇", 26), b.later("饮羽簇", 32) });
    b.state({ 0, "聚势蓄力", 0, 0, 8 }, false);
    b.state({ 0, "聚势蓄箭", 0, 0, 8 }, false);
    // 聚势每 5 帧蓄一段，最多八段。每段都记蓄力，只有当段还有箭时才记蓄箭；释放时两个计数分别决定伤害。
    b.cast("聚势摧霆",
           240,
           24,
           0,
           { .choice = "聚势摧霆", .pulses = 8, .spacing = 5, .charges = 2, .gcd_group = 3, .cooldown_on_finish = true,
             .fixed_cast_time = true, .precombat_cast = true, .release_charged = true },
           { b.apply("聚势蓄力"), b.at(0, 1, 8, b.apply("聚势蓄箭")), b.gain(0, -1), b.remove("金乌箭") });
    b.set_bonus({ .damage = 102 }, { "饮羽簇" });

    b.cast("金乌见坠",
           240,
           24,
           0,
           { .charges = 3, .gcd_group = 1 },
           { b.resource_stacks("金乌箭", 0), b.selected("佩弦", b.when("橙武", b.apply("佩弦"), true)) });
    b.damage(40717, "于狩", { 3, 2, 300, 0, 0, 0, true, { .pve = 133 }, {} });
    b.pre_cast("没石饮羽",
              { b.selected("于狩", b.at(0, 3, 5, b.later("于狩", 20))),
                b.selected("于狩", b.at(0, 3, 5, b.later("于狩", 26))),
                b.selected("于狩", b.at(0, 3, 5, b.later("于狩", 32))) });
    b.passive("饮羽簇", "佩弦", { .ignore = 666 });
    b.post_damage("劲风簇", { b.when("金乌箭", b.apply("贯穿")), b.remove("金乌箭") });
    b.post_damage("饮羽簇", { b.when("金乌箭", b.apply("贯穿")), b.remove("金乌箭") });
    b.post_cast("寒更晓箭", { b.remove("金乌箭", 8) });
    b.state({ 0, "承契", 304, 0, 5 }, false);
    b.state({ 0, "连珠", 304, 0, 1 }, false);
    b.state({ 0, "领胡", 160, 0, 1 }, false);
    for (auto cast : { "弛风鸣角", "没石饮羽" }) b.pre_cast(cast, { b.apply("白羽共调"), b.selected("连珠", b.apply_for("白羽共调", 56)) });
    b.state({ 0, "首次汇灵", 0, 0, 1 }, false);
    b.initial("首次汇灵");
    b.resource({ "唤灵印", 0, 0, 6 });
    b.state({ 0, "唤灵次序", 0, 0, 6 }, false);
    constexpr std::string_view seals[]{"虎印", "鹰印", "猪印", "象印", "熊印", "狼印"};
    for (auto seal : seals) {
        b.state({0, seal, 0, 0, 1}, false);
        b.on_expiry(seal, {b.gain(1, -1)});
    }
    b.cast("弛律召野", 800, 24, 0, {}, { b.selected("领胡", b.shorten("弛律召野", 160)) });
    b.cast("引风唤灵", 368, 24, 0, {}, {});
    const Step spend_four{ .op = Op::School, .slot = b.buff(seals[0]), .amount = 4, .resource = 1, .maximum = 6, .effect = 消耗唤灵印 };
    b.damage(45107, "领胡", { 4, 4, 475, 1024, 0, 0, true, { .pve = 8325 } });
    b.cast("汇灵合契", 16, 24, 0, { .resource = 1, .cost = 4, .choice = "领胡", .free_cost = b.buff("首次汇灵"), .cooldown_aura = b.buff("首次汇灵"), .aura_cooldown = 480 },
           { b.when("首次汇灵", spend_four, true), b.remove("首次汇灵"),
             b.later("领胡", 36), b.later("领胡", 60), b.later("领胡", 84), b.later("领胡", 108) });
    b.post_damage("领胡", { b.apply("领胡") });
    b.post_cast("饮羽簇", { b.selected("连珠", b.shorten("饮羽簇", 16)) });
    b.charge_variant("聚势摧霆", "连珠", 192, 1);
    b.pre_cast("饮羽簇", { b.selected("饮羽簇·重置白羽", b.remove("白羽共调")) });
    b.post_cast("聚势摧霆", { b.selected("连珠", b.shorten("金乌见坠", 32)), b.selected("连珠", b.apply("连珠")) });
    // 承契先读刷新前的层数和剩余时长，再按本次召唤更新，重复召唤也走这次判断。
    const auto grant_contract = [&](std::string_view cast, std::string_view choice, int count) {
        for (int i = 0; i < count; ++i) {
            Step refund = b.stacks("承契", 5, b.shorten("金乌见坠", 48));
            refund.minimum_aura_time = 128;
            refund.choice = "襄尺";
            refund.other_choice = choice;
            b.post_cast(cast, { refund, b.selected(choice, b.apply("承契")) });
        }
    };
    grant_contract("没石饮羽", "于狩", 1);
    grant_contract("引风唤灵", "卢令", 1);
    grant_contract("汇灵合契", "汇灵合契·四层承契", 4);
    grant_contract("聚势摧霆", "连珠", 1);
    // 释放时，每层蓄力打一段本体，每层蓄箭另打一段蓄力伤害。两组段数结算完才清空计数。
    b.post_cast("聚势摧霆", { b.hits_per_stack("聚势摧霆", "聚势蓄力"), b.hits_per_stack("聚势摧霆·蓄力", "聚势蓄箭"),
                            b.remove("聚势蓄力", 8), b.remove("聚势蓄箭", 8) });
    b.damage(35894, "风矢", {0, 0, 24, 1024, 0, 0, true, {.damage = 205, .pve = 133}});
    b.state({0, "风矢计时", 0, 24, 1}, false);
    b.initial("风矢计时");
    b.pulse("风矢计时", {b.hit("风矢")});
    b.defer_swing("聚势摧霆", "风矢计时");
    b.pre_damage("标鹄", { b.buff("连珠"), 0, { .fragile = 409 } });
    b.school_modifier({ b.buff("连珠"), 0, { .attack = 154 } });
    b.school_modifier({ b.buff("领胡"), 0, { .damage = 307 } });
    b.school_modifier({ b.buff("承契"), 0, { .ignore = 205, .strain_rating = 102 }, "卢令" });
    for (int i = 1; i <= 5; ++i) b.school_modifier({ b.buff("承契"), i, { .damage = 41 * i } });
    // 召唤的宠兽各自持有一层对应的契约；弓手身上的承契另行计层。
    constexpr std::string_view attacks[]{"攻击-虎", "攻击-鹰", "重击", "践踏", "攻击-熊", "攻击-狼"};
    constexpr int ids[]{36112, 36113, 36057, 36056, 36114, 36111};
    constexpr int channels[]{1130, 828, 376, 828, 225, 376};
    for (int i = 0; i < 6; ++i) {
        b.secondary_damage(ids[i], attacks[i], {29, 2, channels[i], 0, 0, 0, true, {.damage = 41, .pve = 133}});
        b.pre_damage(attacks[i], { b.buff("领胡"), 0, { .damage = 307 } });
        b.pre_damage(attacks[i], { b.buff("连珠"), 0, { .attack = 154 } });
        b.passive(attacks[i], "卢令", { .ignore = 205, .strain_rating = 102 });
    }
    b.post_damage("攻击-鹰", {b.apply("贯穿", 4)});
    for (std::string_view cast : {"引风唤灵", "弛律召野"}) {
        // 朱厌延长弛律 20 秒；每只宠兽各自推进轮次、刷新对应印记并排入自己的命中帧。
        const int count = cast == "弛律召野" ? 3 : 1;
        for (int summon = 0; summon < count; ++summon) {
            const std::string_view choice = summon ? "朱厌" : "";
            auto emit = [&](Step step) { b.pre_cast(cast, {b.selected(choice, step)}); };
            emit(b.stacks("唤灵次序", 6, b.remove("唤灵次序", 6)));
            emit(b.apply("唤灵次序"));
            emit({.op = Op::School, .slot = b.buff(seals[0]), .extra = b.buff("唤灵次序"), .resource = 1,
                  .duration = cast == "弛律召野" ? 160 : 0, .effect = 获得唤灵印});
            for (int i = 0; i < 6; ++i) emit(b.stacks("唤灵次序", i + 1, b.later(attacks[i], i == 3 ? 36 : 16)));
            for (int delay : {21, 26}) emit(b.stacks("唤灵次序", 6, b.later("攻击-狼", delay)));
        }
    }
    b.timing_adjustment("朱厌", "弛律召野", 320);
    b.adjust([](Prepared &p) {
        const auto arrow = std::ranges::find(p.skills, "风矢", &SkillDescriptor::name) - p.skills.begin();
        p.opening.push_back({Op::DelayedHit, static_cast<Slot>(arrow), 0});
        for (auto n : { "寒更晓箭", "劲风簇" }) p.gcd_reduction[std::ranges::find(p.skills, n, &SkillDescriptor::name) - p.skills.begin()] = 8;
    });
    b.timing_recipe("引风唤灵", "引风唤灵·调息减1秒（一）", "引风唤灵", -16, 0);
    b.timing_recipe("引风唤灵", "引风唤灵·调息减1秒（二）", "引风唤灵", -16, 0);
    b.timing_recipe("引风唤灵", "引风唤灵·调息减1秒（三）", "引风唤灵", -16, 0);
    b.single_target_recipe("引风唤灵", "引风唤灵·秘境不强仇", "防御型宠兽在秘境取消强制仇恨。单目标木桩不结算仇恨。");
    b.recipe("汇灵合契", "汇灵合契·四层承契", {}, {});
    b.state({0, "游雾乘云", 160, 0, 1}, false);
    b.cast("游雾乘云", 800, 24, 0, { .resource = 1, .cost = 1, .forbidden = b.buff("游雾乘云") },
           { { .op = Op::School, .slot = b.buff(seals[0]), .amount = 1, .resource = 1, .maximum = 6, .effect = 消耗唤灵印 }, b.apply("游雾乘云") });
    grant_contract("游雾乘云", "游雾乘云·获得承契", 1);
    b.cast("下马", 1, 0, 0, { .required = b.buff("游雾乘云") }, { b.remove("游雾乘云") });
    b.timing_recipe("游雾乘云", "游雾乘云·调息减2秒", "游雾乘云", -32, 0);
    b.timing_recipe("游雾乘云", "游雾乘云·调息减3秒", "游雾乘云", -48, 0);
    b.recipe("游雾乘云", "游雾乘云·获得承契", {}, {});
    b.timing_recipe("饮羽簇", "饮羽簇·调息减24帧（一）", "饮羽簇", -24, 0);
    b.timing_recipe("饮羽簇", "饮羽簇·调息减24帧（二）", "饮羽簇", -24, 0);
    b.recipe("饮羽簇", "饮羽簇·重置白羽", {}, {});
    b.no_cooldown_during("饮羽簇", "橙武");
    b.legendary_damage({ "标鹄", "饮羽簇" });
    return b.finish();
}
} // namespace JX3DPS::runtime::season50
