#include "common/builder.hpp"

namespace JX3DPS::runtime::season50 {

static void 乱洒续毒(EffectContext &frame, const Step &step)
{
    // 乱洒续毒每次处理两种毒。三毒全空时先兰摧再钟林；
    // 已有毒时优先补缺，同类按钟林、商阳、兰摧的顺序，余下名额用于刷新。
    std::array<Slot, 3> dots{step.slot, step.extra, step.neutral_dot};
    if (!frame.Stacks(dots[0]) && !frame.Stacks(dots[1]) && !frame.Stacks(dots[2])) {
        dots = {step.neutral_dot, step.slot, step.extra};
    } else {
        std::stable_partition(dots.begin(), dots.end(), [&](Slot dot) { return !frame.Stacks(dot); });
    }
    for (int i = 0; i < 2; ++i) {
        frame.SetAura(dots[i], 1);
        if (dots[i] == step.extra && step.neutral_buff != INVALID_SLOT) frame.SetAura(step.neutral_buff, 1);
    }
}

Book 花间游()
{
    Builder b("hjy", "花间游", R"(# 魔盒宏适配 https://www.jx3box.com/macro/111067
/cast 使用风腰坠
/cast 使用橙武
/cast 水月无间
/cast [last_skill:玉石俱焚] 乱洒青荷
/cast 墨海临源
/cast [buff:青荷] 阳明指
/cast [tbuff:兰摧玉折&tbufftime:兰摧玉折<13] 芙蓉并蒂
/cast [tnobuff:商阳指] 商阳指
/cast [tnobuff:钟林毓秀] 钟林毓秀
/cast [tnobuff:兰摧玉折] 兰摧玉折
/cast 玉石俱焚
/cast 快雪时晴
/cast 阳明指
)");
    auto &school = b.definition();
    school.info = { school.code, "万花", "元气", "Neutral", 686, 0, 82 };
    school.display_order = 9;
    school.saved.macro_digest = "16e3ec8e2fa00f8d0184bcfb81f8b87ab2cffbc5993045ef0b121b03e1a282dc";
    school.saved.talents_v7 = { "青歌", "雪弃", "丹鼎", "青冠", "墨海临源", "生风" };
    school.saved.talents_v6 = { "青歌", "雪弃", "丹鼎" };
    school.precombat = true;
    b.preset(2300, { 6606, 1540, 57, 1260, 208, 1474, 4212, 0, 1092 }, { "阳明指·4%伤害", "阳明指·3%伤害", "阳明指·3%会心", "阳明指·2%会心",
               "芙蓉并蒂·5%伤害", "芙蓉并蒂·4%伤害", "芙蓉并蒂·4%会心",
               "快雪时晴·5%伤害", "快雪时晴·4%伤害", "快雪时晴·3%伤害",
               "兰摧玉折·调息减8帧（一）", "兰摧玉折·调息减8帧（二）", "兰摧玉折·延长3秒", "兰摧玉折·运功减4帧",
               "商阳指·延长3秒",
               "钟林毓秀·运功减2帧（一）", "钟林毓秀·运功减2帧（二）", "钟林毓秀·运功减8帧", "钟林毓秀·延长3秒" });
    b.choices({ "青歌", "雪弃", "丹鼎", "青冠", "墨海临源", "生风", "流离", "钟灵", "渲青", "春寒" }, {}, {});
    b.default_talents({ "丹鼎", "青冠", "雪弃", "流离", "生风", "钟灵", "墨海临源", "渲青", "青歌" });
    b.resource({ "墨意", 0, 0, 60 });
    b.state({ 0, "青歌", 288, 0, 1 }, false, false, false);
    b.state({ 0, "商阳指", 288, 48, 1 }, true, true, true);
    b.state({ 0, "兰摧玉折", 288, 48, 1 }, true, true, true);
    b.state({ 0, "钟林毓秀", 288, 48, 1 }, true, true, true);
    b.damage(0,
             "商阳指(DOT)",
             { 50, 0, 740, 0, 6, 48, false, { .pve = 604 }, { { b.buff("青歌"), 0, { .attack = 154 }, "" } } });
    b.damage(0,
             "兰摧玉折(DOT)",
             { 30, 0, 882, 0, 6, 48, false, { .pve = 604 }, { { b.buff("青歌"), 0, { .attack = 154 }, "" } } });
    b.damage(0,
             "钟林毓秀(DOT)",
             { 38, 0, 804, 0, 6, 48, false, { .pve = 604 }, { { b.buff("青歌"), 0, { .attack = 154 }, "" } } });
    b.damage(6693, "商阳指", { 2, 0, 294, 0, 0, 0, false, { .pve = 604 }, { { b.buff("青歌"), 0, { .attack = 154 }, "" } } });
    b.damage(14941, "阳明指", { 180, 20, 509, 0, 0, 0, false, { .pve = 604 }, { { b.buff("青歌"), 0, { .attack = 154 }, "" } } });
    b.damage(182, "玉石俱焚", { 8, 1, 73, 0, 0, 0, false, { .pve = 604 }, { { b.buff("青歌"), 0, { .attack = 154 }, "" } } });
    b.dot("商阳指", "商阳指(DOT)");
    b.dot("兰摧玉折", "兰摧玉折(DOT)");
    b.dot("钟林毓秀", "钟林毓秀(DOT)");
    b.cast("商阳指", 0, 24, 0, {}, { b.hit("商阳指", 1), b.selected("青歌", b.apply("青歌", 1)), b.gain(0, 5), b.apply("商阳指", 1) });
    b.state({ 0, "流离", 0, 0, 1 }, false);
    b.state({ 0, "钟灵可运功", 288, 0, 1 }, false);
    b.cast("兰摧玉折", 96, 24, 28, {}, { b.apply("兰摧玉折", 1), b.gain(0, 5) });
    b.cast("钟林毓秀", 0, 24, 32, {}, { b.apply("钟林毓秀", 1), b.gain(0, 5) });
    b.state({ 0, "水月无间", 160, 0, 1 }, false);
    b.state({ 0, "布散畅和", 160, 0, 1 }, false);
    b.cast("水月无间", 960, 12, 0, { .gcd_group = 1 }, { b.apply("水月无间"), b.apply("布散畅和"), b.gain(0, 20) });
    b.cast("阳明指", 0, 24, 28, { .timing_resource = 0, .instant_at = 20, .precombat_cast = true },
           { b.hit("阳明指"), b.at(0, 20, 60, b.selected("流离", b.apply("流离"))), b.at(0, 20, 60, b.gain(0, -20)), b.gain(0, 5), b.when("青歌", b.gain(0, 5)) });
    b.cast("玉石俱焚",
           216,
           24,
           0,
           { .hasted_cd = true },
           { b.burst("钟林毓秀"), b.burst("商阳指"), b.burst("兰摧玉折"), b.gain(0, 5), b.hit("玉石俱焚", 1) });
    b.recipe("阳明指", "阳明指·2%会心", { .critical = 200 }, { "阳明指" });
    b.recipe("阳明指", "阳明指·3%会心", { .critical = 300 }, { "阳明指" });
    b.recipe("阳明指", "阳明指·3%伤害", { .damage = 31 }, { "阳明指" });
    b.recipe("阳明指", "阳明指·4%伤害", { .damage = 41 }, { "阳明指" });
    b.damage(6233, "快雪时晴", { 65, 10, 222, 0, 0, 0, false, { .pve = 604 }, { { b.buff("青歌"), 0, { .attack = 154 }, {} } } });
    b.cast("快雪时晴", 0, 24, 0, { .pulses = 5, .spacing = 9, .pre_cast = { b.gain(0, 5) } }, { b.hit("快雪时晴") });
    b.passive("快雪时晴", "雪弃", { .critical = 2000, .power = 205 });
    b.passive("快雪时晴", "丹鼎", { .damage = 819, .pve = 614 });
    b.variant("快雪时晴",
              "丹鼎",
              320,
              0,
              { .pulses = 7, .spacing = 6, .hasted_cd = true, .charges = 2, .pre_cast = { b.gain(0, 5) }, .channel_duration = 45 },
              { b.hit("快雪时晴") });
    b.set_bonus({ .damage = 102 }, { "快雪时晴" });
    b.set_bonus({}, { "钟林毓秀(DOT)" }, 1.15);
    b.state({ 0, "临源进度", 0, 0, 4 }, false);
    b.state({ 0, "临源充能", 0, 0, 3 }, false);
    b.damage(37270,
             "临源·一",
             {
                 34,
                 5,
                 2855,
                 0,
                 0,
                 0,
                 false,
                 { .pve = 604 + 1690 }, // 墨海临源在基础非侠士增伤以外，再加 1690/1024。
                 { { b.buff("青歌"), 0, { .attack = 154 }, "青歌" } }
    });
    // 墨海临源消耗一层充能，回 20 墨意并打临源·一；阳明、玉石和快雪在起手累计进度，每五次取得一层充能。
    b.cast("墨海临源", 0, 24, 0, { .required = b.buff("临源充能"), .choice = "墨海临源" }, { b.remove("临源充能"), b.gain(0, 20), b.hit("临源·一") });
    for (auto n : { "阳明指", "玉石俱焚", "快雪时晴" }) {
        b.pre_cast(n, { b.selected("墨海临源", b.build_charge("临源进度", 5, "临源充能")) });
    }
    for (auto n : { "商阳指", "阳明指", "玉石俱焚", "商阳指(DOT)", "兰摧玉折(DOT)", "钟林毓秀(DOT)", "快雪时晴", "临源·一" })
    {
        b.passive(n, "青冠", { .ignore = 666 });
    }
    for (auto n : { "兰摧玉折(DOT)", "钟林毓秀(DOT)" }) {
        b.passive(n, "生风", { .critical = 1000 });
    }
    b.variant("兰摧玉折", "生风", 96, 20, { .instant = b.buff("钟灵可运功"), .precombat_cast = true }, { b.apply("兰摧玉折"), b.gain(0, 5), b.shorten("兰摧玉折", 8) });
    b.variant("钟林毓秀", "生风", 0, 24, { .instant = b.buff("钟灵可运功"), .precombat_cast = true }, { b.apply("钟林毓秀"), b.gain(0, 10) });
    b.state({ 0, "钟灵", 288, 0, 1 }, false);
    for (auto cast : { "兰摧玉折", "钟林毓秀" }) b.pre_cast(cast, { b.when("钟灵可运功", b.selected("流离", b.apply("流离"))), b.when("钟灵可运功", b.gain(0, -20)), });
    for (auto cast : { "兰摧玉折", "钟林毓秀" }) b.post_cast(cast, { b.remove("钟灵可运功") });
    b.state({ 0, "青荷", 160, 0, 2 }, false);
    b.damage(45191, "渲青", { 15, 2, 1120, 0, 0, 0, false, { .pve = 604 } });
    b.damage(186, "芙蓉并蒂", { 7, 1, 261, 0, 0, 0, false, { .pve = 604 } });
    b.cast("芙蓉并蒂", 400, 24, 0, {}, { b.hit("芙蓉并蒂"), b.gain(0, 5), b.when("兰摧玉折", b.apply("兰摧玉折")),
                                           b.when("钟林毓秀", b.apply("钟林毓秀")), b.when("商阳指", b.apply("商阳指")) });
    // 乱洒青荷给一层青荷，选渲青再给一层。之后由阳明指的命中触发续毒，每次阳明只扣一层青荷。
    b.cast("乱洒青荷", 960, 0, 0, {}, { b.apply("青荷"), b.selected("渲青", b.apply("青荷")), b.reset("玉石俱焚"), b.gain(0, 20) });
    for (auto dot : { "商阳指", "兰摧玉折", "钟林毓秀" }) {
        // 流离在玉石起手逐种检查目标身上的三毒，每有一种就缩短玉石 20 帧调息；玉石自身随后才引爆。
        b.pre_cast("玉石俱焚", { b.selected("流离", b.when(dot, b.shorten("玉石俱焚", 20))),
                                b.selected("钟灵", b.when(dot, b.apply("钟灵"))), b.selected("钟灵", b.when(dot, b.apply("钟灵可运功"))) });
    }
    b.post_damage("阳明指", { b.when("青荷", { .op = Op::School, .slot = b.buff("钟林毓秀"), .extra = b.buff("商阳指"), .neutral_dot = b.buff("兰摧玉折"), .effect = 乱洒续毒 }),
                           b.selected("渲青", b.when("青荷", b.hit("渲青"))), b.selected("渲青", b.when("青荷", b.reset("玉石俱焚"))), b.remove("青荷") });
    // 这三本延长 3 秒的秘籍分别给对应 DOT 多一跳，跳伤间隔沿用原招式。
    b.extra_tick("商阳指", "商阳指·延长3秒");
    b.extra_tick("兰摧玉折", "兰摧玉折·延长3秒");
    b.extra_tick("钟林毓秀", "钟林毓秀·延长3秒");
    b.post_cast("玉石俱焚", { b.when("流离", b.apply("兰摧玉折")), b.when("流离", b.gain(0, 20)), b.remove("流离") });
    b.adjust([](Prepared &p) {
        if (p.config.precombat) p.resources[0].initial = 60;
        if (std::ranges::contains(p.config.talents, "青歌")) {
            const auto &book = *p.config.book;
            const auto note = std::ranges::find(p.skills, "阳明指", &SkillDescriptor::name) - p.skills.begin();
            for (auto &step : p.post_damage[note]) if (step.effect == 乱洒续毒)
                step.neutral_buff = static_cast<Slot>(std::ranges::find(book.buffs, "青歌", &BuffDescriptor::name) - book.buffs.begin());
        }
        if (std::ranges::contains(p.config.talents, "生风")) {
            for (auto n : { "兰摧玉折", "钟林毓秀" }) p.gcd_reduction[std::ranges::find(p.skills, n, &SkillDescriptor::name) - p.skills.begin()] = 8;
        }
    });
    b.secondary_damage(25768, "兰摧玉折·神兵", {0, 2, 100, 0, 0, 0, false, {.pve = 604}});
    b.passive("兰摧玉折·神兵", "青冠", {.ignore = 666});
    b.pre_damage("兰摧玉折·神兵", {b.buff("青歌"), 0, {.attack = 154}, "青歌"});
    b.school_modifier({ b.buff("布散畅和"), 0, { .attack = 307 } });
    b.school_modifier({ b.buff("钟灵"), 0, { .attack = 154 }, "钟灵" });
    b.school_modifier({ INVALID_SLOT, 0, { .strain_rating = 205 }, "青冠" });
    b.recipe("芙蓉并蒂", "芙蓉并蒂·5%伤害", { .damage = 51 }, { "芙蓉并蒂" });
    b.recipe("芙蓉并蒂", "芙蓉并蒂·4%伤害", { .damage = 41 }, { "芙蓉并蒂" });
    b.recipe("芙蓉并蒂", "芙蓉并蒂·4%会心", { .critical = 400 }, { "芙蓉并蒂" });
    b.recipe("快雪时晴", "快雪时晴·5%伤害", { .damage = 51 }, { "快雪时晴" });
    b.recipe("快雪时晴", "快雪时晴·4%伤害", { .damage = 41 }, { "快雪时晴" });
    b.recipe("快雪时晴", "快雪时晴·3%伤害", { .damage = 31 }, { "快雪时晴" });
    b.timing_recipe("兰摧玉折", "兰摧玉折·调息减8帧（一）", "兰摧玉折", -8, 0);
    b.timing_recipe("兰摧玉折", "兰摧玉折·调息减8帧（二）", "兰摧玉折", -8, 0);
    b.recipe("兰摧玉折", "兰摧玉折·延长3秒", {}, {});
    b.timing_recipe("兰摧玉折", "兰摧玉折·运功减4帧", "兰摧玉折", 0, -4);
    b.recipe("商阳指", "商阳指·延长3秒", {}, {});
    b.timing_recipe("钟林毓秀", "钟林毓秀·运功减2帧（一）", "钟林毓秀", 0, -2);
    b.timing_recipe("钟林毓秀", "钟林毓秀·运功减2帧（二）", "钟林毓秀", 0, -2);
    b.timing_recipe("钟林毓秀", "钟林毓秀·运功减8帧", "钟林毓秀", 0, -8);
    b.recipe("钟林毓秀", "钟林毓秀·延长3秒", {}, {});
    b.state({0, "烂柯", 64, 0, 1}, false);
    b.cast("使用橙武", 800, 0, 0, {.choice = Legendary}, {b.apply("烂柯")});
    b.no_cooldown_during("玉石俱焚", "烂柯");
    // 烂柯窗口内玉石先刷新已有三毒，再执行本体引爆；收招重新挂商阳、钟林，供下一轮使用。
    for (auto dot : {"商阳指", "钟林毓秀", "兰摧玉折"}) b.pre_cast("玉石俱焚", {b.when("烂柯", b.refresh_existing(dot))});
    b.post_cast("玉石俱焚", {b.when("烂柯", b.apply("商阳指")), b.when("烂柯", b.apply("钟林毓秀"))});
    b.post_cast("兰摧玉折", {b.selected(Legendary, b.proc("兰摧玉折·神兵", 256))});
    b.legendary_damage({ "阳明指", "快雪时晴" });
    // 春寒令乱洒青荷提前 20 秒转好，渲青的额外层数仍在施放时结算。
    b.timing_adjustment("春寒", "乱洒青荷", -320);
    return b.finish();
}
} // namespace JX3DPS::runtime::season50
