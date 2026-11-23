#include "common/builder.hpp"

namespace JX3DPS::runtime::season50 {

// 引窍分别读取出手前的任脉、督脉，各取一半并受上限约束。
// 任脉决定引窍档位；有绝脉时，督脉消耗决定额外伤害，绝脉层数决定命中档位。
static void 引窍绝脉(EffectContext &frame, const Step &step)
{
    const int ren = std::min(frame.state.qidian / 2, step.maximum), du = std::min(frame.state.energy / 2, step.maximum);
    const int layers = frame.Stacks(step.neutral_buff);
    frame.ChangeResource(0, -ren);
    frame.ChangeResource(1, -du);
    frame.Hit(step.slot, ren + 1);
    if (layers) {
        frame.HitWithBonus(step.extra, layers, (du * 4096 + 50) / 100);
        frame.SetAura(step.neutral_buff, -layers);
    }
    // 神封把单脉消耗上限设为 40。任一脉耗到上限便双脉各回 10，并给自身挂神封。
    if (step.neutral_dot != INVALID_SLOT && (ren == step.maximum || du == step.maximum)) {
        frame.ChangeResource(0, 10);
        frame.ChangeResource(1, 10);
        frame.SetAura(step.neutral_dot, 1);
    }
}

static Step meridians(Builder &b, std::string_view hit, std::string_view rupture, std::string_view aura)
{
    Step step{ .op = Op::School, .slot = b.effect(hit), .effect = 引窍绝脉 };
    step.extra        = b.effect(rupture);
    step.neutral_buff = b.buff(aura);
    return step;
}

Book 周天功()
{
    Builder b("ztg", "周天功", R"(# 魔盒宏适配 https://www.jx3box.com/macro/110017
/cast 使用风腰坠
/cast 使用橙武
/cast 骤风劈风
/cast [任脉>=100&督脉>=100] 引窍
/cast [tnobuff:风萦] 劈风令
/cast [tbuff:骤风&tbufftime:骤风<1.8] 风徊絮转
/cast [tbuff:风徊絮转] 抟风令
/cast [skill_cd:抟风令<9&tnobuff:横驱风靡] 骤风令
/cast 劈风令
/cast [任脉>79] 引窍
/cast [督脉>79] 破穴
/cast [任脉<50] 断脉
/cast [督脉<80] 截阳
/cast 断脉
)");
    auto &school = b.definition();
    school.info = { school.code, "段氏", "元气", "Neutral", 625, 143, 0 };
    school.display_order = 18;
    school.saved.macro_digest = "2a4f19072df853c3f55db3762cd12dd1af79227e0813327fe6319a252963f908";
    school.saved.talents_v7 = { "封府", "风萦", "涣风" };
    school.saved.talents_v6 = { "封府" };
    school.precombat = true;
    school.weapon_effect = "神兵附伤";
    school.raid.formation = "易筋阵（常驻）";
    school.raid.penetration_perfume = true;
    school.raid.whetstone = "苍生·坠宵熔锭（攻击）";
    school.raid.dish = "创意料理（-气血+破防）";
    b.preset(2334,
             { 6393, 1424, 113, 778, 208, 1772, 4212, 0, 1714 },
             { "断脉·4%伤害", "断脉·4%会心", "断脉·5%伤害", "断脉·3%伤害",
               "截阳·4%伤害", "截阳·3%伤害", "截阳·4%会心", "截阳·5%伤害",
               "破穴·5%伤害", "破穴·4%伤害", "破穴·3%伤害", "破穴·4%会心",
               "引窍·5%伤害", "引窍·4%伤害", "引窍·调息减16帧（一）", "引窍·调息减16帧（二）",
               "劈风令·4%伤害", "劈风令·4%会心", "劈风令·3%会心", "劈风令·5%伤害",
               "抟风令·4%伤害", "抟风令·3%伤害", "抟风令·调息减2秒", "抟风令·调息减3秒" });
    b.choices({ "封府", "风萦", "涣风", "神封", "玉枕", "摧烟", "风徊絮转", "茫缈", "清激" }, {}, { "神兵附伤" });
    b.default_talents({ "风萦", "神封", "玉枕", "涣风", "摧烟", "风徊絮转", "茫缈", "封府", "清激" });
    b.resource({ "任脉", 0, 0, 100 });
    b.resource({ "督脉", 0, 0, 100 });
    b.state({ 0, "绝脉", 320, 0, 10 }, true, false, false);
    b.state({ 0, "封府断脉链", 1584, 0, 1 }, false, false, false);
    b.state({ 0, "封府截阳链", 1584, 0, 1 }, false, false, false);
    b.damage(38083, "破穴", { 2, 0, 720, 0, 0, 0, false, { .pve = 266 }, {} });
    b.damage(38084, "断脉", { 2, 0, 864, 0, 0, 0, false, { .pve = 266 }, {} });
    b.damage(44568, "封府", { 2, 0, 864, 0, 0, 0, false, { .pve = 266 }, {} });
    b.secondary_damage(39081, "断脉·神兵", { 20, 2, 65, 0, 0, 0, false, { .pve = 266 }, {} });
    b.damage(38090, "截阳", { 2, 0, 852, 0, 0, 0, false, { .pve = 266 }, {} });
    b.cast("破穴", 240, 24, 0, {}, { b.gain(0, 15), b.gain(1, 15), b.hit("破穴", 1) });
    b.cast("断脉",
           0,
           24,
           0,
           {},
           { b.gain(0, 15),
             b.hit("断脉", 1),
             b.when("封府截阳链", b.hit("封府", 1), false),
             b.remove("封府截阳链", 1000),
             b.selected("封府", b.apply("封府断脉链", 1)),
             b.selected("神兵附伤", b.proc("断脉·神兵", 256)) });
    b.cast("截阳",
           80,
           24,
           0,
           { .charges = 3 },
           { b.gain(1, 20),
             b.hit("截阳", 1),
             b.apply("绝脉", 3),
             b.when("封府断脉链", b.hit("封府", 1), false),
             b.remove("封府断脉链", 1000),
             b.selected("封府", b.apply("封府截阳链", 1)) });
    b.recipe("截阳", "截阳·2%会心", { .critical = 200 }, { "截阳" });
    b.recipe("截阳", "截阳·3%会心", { .critical = 300 }, { "截阳" });
    b.recipe("截阳", "截阳·3%伤害", { .damage = 31 }, { "截阳" });
    b.recipe("截阳", "截阳·4%会心", { .critical = 400 }, { "截阳" });
    b.recipe("截阳", "截阳·4%伤害", { .damage = 41 }, { "截阳" });
    b.recipe("截阳", "截阳·5%伤害", { .damage = 51 }, { "截阳" });
    b.recipe("断脉", "断脉·2%会心", { .critical = 200 }, { "断脉" });
    b.recipe("断脉", "断脉·3%会心", { .critical = 300 }, { "断脉" });
    b.recipe("断脉", "断脉·3%伤害", { .damage = 31 }, { "断脉" });
    b.recipe("断脉", "断脉·4%会心", { .critical = 400 }, { "断脉" });
    b.recipe("断脉", "断脉·4%伤害", { .damage = 41 }, { "断脉" });
    b.recipe("断脉", "断脉·5%伤害", { .damage = 51 }, { "断脉" });
    b.recipe("破穴", "破穴·2%会心", { .critical = 200 }, { "破穴" });
    b.recipe("破穴", "破穴·3%会心", { .critical = 300 }, { "破穴" });
    b.recipe("破穴", "破穴·3%伤害", { .damage = 31 }, { "破穴" });
    b.recipe("破穴", "破穴·4%会心", { .critical = 400 }, { "破穴" });
    b.recipe("破穴", "破穴·4%伤害", { .damage = 41 }, { "破穴" });
    b.recipe("破穴", "破穴·5%伤害", { .damage = 51 }, { "破穴" });
    // 每个引窍档位独立对应一个技能系数，按该档公式算完并截整后再登记。
    b.damage(38438, "引窍", {2, 0, 673, 0, 0, 0, false, {.pve = 266}});
    b.ranks("引窍", {673, 700, 727, 754, 781, 808, 835, 862, 889, 916, 943, 970, 997,
                    1024, 1051, 1078, 1105, 1132, 1159, 1186, 1213, 1240, 1267, 1294, 1321, 1347,
                    1374, 1401, 1428, 1455, 1482, 1509, 1536, 1563, 1590, 1617, 1644, 1671, 1698,
                    1725, 1752, 1779, 1806, 1833, 1860, 1887, 1914, 1941, 1968, 1994, 2021});
    b.damage(38085, "绝脉", {2, 0, 46, 0, 0, 0, false, {.pve = 266}});
    b.ranks("绝脉", {46, 93, 139, 186, 232, 279, 325, 372, 418, 465});
    b.cast("引窍", 0, 24, 0, {}, { meridians(b, "引窍", "绝脉", "绝脉") });
    b.set_bonus({ .damage = 102 }, { "引窍" });
    b.state({ 0, "风萦", 192, 0, 1 }, true);
    b.state({ 0, "骤风", 144, 12, 1 }, true);
    b.state({ 0, "涣风", 128, 0, 3 }, true);
    b.damage(37816, "劈风令", { 2, 0, 993, 0, 0, 0, false, { .pve = 266 }, {} });
    b.damage(0, "骤风", { 2, 0, 247, 0, 0, 0, false, { .pve = 266 }, {} });
    b.damage(38452, "风籁", { 7, 0, 315, 0, 0, 0, false, { .pve = 266 }, {} });
    b.damage(39340, "涣风", { 2, 0, 704, 0, 0, 0, false, { .pve = 266 }, {} });
    b.pre_damage("风籁", { b.buff("风萦"), 0, { .final_damage = 512 }, "风萦" });
    b.cast("劈风令",
           128,
           24,
           0,
           { .hasted_cd = true, .charges = 2 },
           { b.hit("劈风令"),
             b.selected("风萦", b.apply("风萦")),
             b.selected("涣风", b.hit("涣风")),
             b.selected("涣风", b.apply("涣风")),
             b.when("骤风", b.hit("风籁")) });
    b.state({ 0, "风场余跳", 144, 0, 10 }, true);
    b.cast("骤风令", 400, 24, 0, { .hasted_cd = true }, { b.apply("骤风"), b.apply("风场余跳", 10) });
    // 骤风每跳先看剩余次数：有余跳才打伤害，然后扣一层余跳；有风萦时，该次命中还会追加风籁。
    b.pulse("骤风", { b.when("风场余跳", b.hit("骤风")), b.remove("风场余跳") });
    b.post_damage("骤风", { b.when("风萦", b.hit("风籁")) });
    for (auto n : { "引窍", "绝脉" }) {
        for (int i = 1; i <= 3; ++i) {
            // 涣风每层增加引窍等指定伤害，按命中时层数读取；引窍收招再移除最多三层。
            b.pre_damage(n, { b.buff("涣风"), i, { .damage = (4096 * i + 5) / 10 }, "涣风" });
        }
    }
    b.post_cast("引窍", { b.remove("涣风", 3) });
    b.state({ 0, "风徊絮转", 64, 0, 1 }, true);
    b.state({ 0, "横驱风靡", 144, 12, 1 }, true);
    b.state({ 0, "神封", 480, 0, 1 }, false);
    b.damage(38477, "风徊絮转", { 2, 1, 1238, 0, 0, 0, false, { .pve = 266 } });
    b.damage(38886, "风徊絮转·抟", { 5, 3, 3231, 0, 0, 0, false, { .pve = 266 } });
    b.damage(38447, "抟风令", { 2, 0, 480, 0, 0, 0, false, { .pve = 266 } });
    b.damage(38453, "抟风令·爆", { 122, 0, 1536, 0, 0, 0, false, { .pve = 266 } });
    b.damage(0, "横驱风靡", { 0, 0, 421, 0, 0, 0, false, { .pve = 266 } });
    b.pulse("横驱风靡", { b.when("风场余跳", b.hit("横驱风靡")), b.remove("风场余跳") });
    b.post_damage("横驱风靡", { b.when("风萦", b.hit("风籁")) });
    b.cast("风徊絮转", 368, 8, 0, { .choice = "风徊絮转", .hasted_cd = true, .lock = 8, .gcd_group = 2 }, { b.apply("风徊絮转"), b.hit("风徊絮转") });
    b.cast("抟风令", 880, 24, 0, { .hasted_cd = true },
           { b.hit("抟风令"), b.later("抟风令·爆", 24), b.when("风徊絮转", b.hit("风徊絮转·抟")), b.remove("风徊絮转"),
             b.when("骤风", b.apply("横驱风靡")), b.when("骤风", b.apply("风场余跳", 10)), b.remove("骤风"), b.selected("摧烟", b.reset("劈风令")) });
    b.charge_variant("劈风令", "摧烟", 128, 3);
    b.post_cast("劈风令", { b.when("横驱风靡", b.hit("风籁")) });
    // 清激先改基础调息，再由本次施放读取加速，和两本调息秘籍使用同一个计时。
    b.timing_adjustment("清激", "抟风令", -160);
    b.passive("抟风令", "清激", { .damage = 1024 });
    for (auto n : { "骤风", "横驱风靡" }) b.passive(n, "茫缈", { .damage = 358 });
    b.state({ 0, "茫缈", 32, 0, 1 }, false);
    b.post_cast("骤风令", { b.selected("茫缈", b.apply("茫缈")) });
    b.cast("骤风劈风", 1, 0, 0, { .required = b.buff("茫缈") }, { b.remove("茫缈"), b.hit("劈风令"),
           b.selected("风萦", b.apply("风萦")), b.selected("涣风", b.hit("涣风")), b.selected("涣风", b.apply("涣风")), b.when("骤风", b.hit("风籁")) });
    b.state({ 0, "经脉循行", 0, 0, 1 }, false);
    for (auto cast : { "引窍", "截阳", "断脉", "破穴" }) {
        b.post_cast(cast, { b.at(0, 50, 100, b.apply("经脉循行")), b.at(1, 0, 49, b.remove("经脉循行")), b.at(0, 0, 49, b.remove("经脉循行")) });
    }
    b.school_modifier({ b.buff("经脉循行"), 0, { .ignore = 512 }, "玉枕" });
    b.post_cast("劈风令", { b.selected("涣风", { Op::ShortenRemainingCd, b.effect("抟风令"), 5 }) });
    b.post_cast("骤风劈风", { b.selected("涣风", { Op::ShortenRemainingCd, b.effect("抟风令"), 5 }) });
    for (auto tick : { "骤风", "横驱风靡" }) b.post_damage(tick, { b.when("风萦", { Op::ShortenRemainingCd, b.effect("抟风令"), 2 }) });
    b.adjust([](Prepared &p) {
        p.gcd_reduction[std::ranges::find(p.skills, "骤风令", &SkillDescriptor::name) - p.skills.begin()] = 8;
        if (p.config.precombat) p.resources[0].initial = p.resources[1].initial = 100;
        if (std::ranges::contains(p.config.talents, "神封")) {
            const auto &book = *p.config.book;
            auto &spend = p.impacts[std::ranges::find(book.skills, "引窍", &SkillDescriptor::name) - book.skills.begin()].front();
            spend.maximum = 40;
            spend.neutral_dot = static_cast<Slot>(std::ranges::find(book.buffs, "神封", &BuffDescriptor::name) - book.buffs.begin());
        }
    });
    b.school_modifier({ b.buff("神封"), 0, { .attack = 154, .strain_rating = 154 }, "神封" });
    b.recipe("引窍", "引窍·5%伤害", { .damage = 51 }, { "引窍" });
    b.recipe("引窍", "引窍·4%伤害", { .damage = 41 }, { "引窍" });
    // 两本引窍减调息秘籍各扣 16 帧。它们改的是同一按钮的调息时间。
    b.timing_recipe("引窍", "引窍·调息减16帧（一）", "引窍", -16, 0);
    b.timing_recipe("引窍", "引窍·调息减16帧（二）", "引窍", -16, 0);
    b.recipe("劈风令", "劈风令·4%伤害", { .damage = 41 }, { "劈风令", "风籁" });
    b.recipe("劈风令", "劈风令·4%会心", { .critical = 400 }, { "劈风令", "风籁" });
    b.recipe("劈风令", "劈风令·3%会心", { .critical = 300 }, { "劈风令", "风籁" });
    b.recipe("劈风令", "劈风令·5%伤害", { .damage = 51 }, { "劈风令", "风籁" });
    b.recipe("抟风令", "抟风令·4%伤害", { .damage = 41 }, { "抟风令" });
    b.recipe("抟风令", "抟风令·3%伤害", { .damage = 31 }, { "抟风令" });
    b.timing_recipe("抟风令", "抟风令·调息减2秒", "抟风令", -32, 0);
    b.timing_recipe("抟风令", "抟风令·调息减3秒", "抟风令", -48, 0);
    b.state({0, "龙影蔷微", 160, 0, 3}, false);
    b.cast("使用橙武", 800, 0, 0, {.choice = Legendary}, {b.gain(0, 100), b.gain(1, 100), b.apply("龙影蔷微", 3)});
    for (auto name : {"引窍", "破穴"}) {
        b.pre_damage(name, {b.buff("龙影蔷微"), 0, {.damage = 1024}});
        b.post_cast(name, {b.when("龙影蔷微", b.gain(0, 30)), b.when("龙影蔷微", b.gain(1, 30)), b.remove("龙影蔷微")});
    }
    b.legendary_damage({ "断脉", "引窍" });
    return b.finish();
}
} // namespace JX3DPS::runtime::season50
