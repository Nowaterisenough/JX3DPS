#include "common/builder.hpp"

namespace JX3DPS::runtime::season50 {
Book 冰心诀()
{
    Builder b("bxj", "冰心诀", R"(# 魔盒宏适配 https://www.jx3box.com/macro/111335
/cast 使用风腰坠
/cast 使用橙武
/cast [nobuff:化冰] 心鼓弦
/cast 繁音急节
/cast [nobuff:芳姿畅音] 剑影留痕
/cast [tbuff:急曲>=5] 江海凝光
/cast [skill_notin_cd:剑破虚空&bufftime:仙姿剑破<5] 芳姿畅音
/cast 剑破虚空
/cast [last_skill:剑破虚空&bufftime:仙姿玳弦<5&nobuff:芳姿畅音] 芳姿畅音
/cast [剑舞>0] 玳弦急曲
/cast 江海凝光
)");
    auto &school = b.definition();
    school.info = { school.code, "七秀", "根骨", "Lunar", 748, 20, 0 };
    school.display_order = 4;
    school.saved.macro_digest = "8507508384005949a430c52b94e006da0a260347fdca979cba63a22618e20ae1";
    school.precombat = true;
    b.preset(2301,
             { 6223, 1680, 73, 1875, 208, 1153, 4212, 0, 1092, 154 },
             { "剑气长江·6%伤害",
               "剑气长江·4%伤害",
               "剑气长江·3%会心",
               "江海凝光·3%会心",
               "江海凝光·2%会心",
               "玳弦急曲·5%伤害",
               "玳弦急曲·4%伤害",
               "玳弦急曲·3%伤害",
               "剑气长江·调息减1秒",
               "江海凝光·15%伤害（一）",
               "江海凝光·15%伤害（二）" });
    b.choices({ "明空", "新妆", "霜降", "盈袖", "流玉", "芳姿畅音", "化冰", "枕上", "钗燕", "灼桃" }, {}, {});
    b.default_talents({ "明空", "新妆", "霜降", "盈袖", "流玉", "芳姿畅音", "化冰", "枕上", "钗燕" });
    b.resource({ "剑舞", 0, 0, 5 });
    b.state({ 0, "满堂", 240, 0, 5 }, false, false, false);
    b.state({ 0, "繁音急节", 320, 0, 1 }, false, false, false);
    b.state({ 0, "急曲", 288, 48, 6 }, true, true, true);
    b.damage(0,
             "急曲(DOT)·1",
             {
                 1,
                 0,
                 137,
                 0,
                 6,
                 48,
                 false,
                 { .pve = 266 },
                 { { b.buff("满堂"), 1, { .critical = 800, .power = 21 }, "" },
                   { b.buff("满堂"), 2, { .critical = 1600, .power = 42 }, "" },
                   { b.buff("满堂"), 3, { .critical = 2400, .power = 63 }, "" },
                   { b.buff("满堂"), 4, { .critical = 3200, .power = 84 }, "" },
                   { b.buff("满堂"), 5, { .critical = 4000, .power = 105 }, "" },
                   { b.buff("繁音急节"), 0, { .attack = 461 }, "" } }
    });
    b.damage(40182,
             "剑破虚空",
             {
                 21,
                 10,
                 913,
                 0,
                 0,
                 0,
                 false,
                 { .pve = 266 },
                 { { b.buff("满堂"), 1, { .critical = 800, .power = 21 }, "" },
                   { b.buff("满堂"), 2, { .critical = 1600, .power = 42 }, "" },
                   { b.buff("满堂"), 3, { .critical = 2400, .power = 63 }, "" },
                   { b.buff("满堂"), 4, { .critical = 3200, .power = 84 }, "" },
                   { b.buff("满堂"), 5, { .critical = 4000, .power = 105 }, "" },
                   { b.buff("繁音急节"), 0, { .attack = 461 }, "" } }
    });
    b.damage(30524,
             "剑气长江",
             {
                 70,
                 7,
                 727,
                 0,
                 0,
                 0,
                 false,
                 { .pve = 266 },
                 { { b.buff("满堂"), 1, { .critical = 800, .power = 21 }, "" },
                   { b.buff("满堂"), 2, { .critical = 1600, .power = 42 }, "" },
                   { b.buff("满堂"), 3, { .critical = 2400, .power = 63 }, "" },
                   { b.buff("满堂"), 4, { .critical = 3200, .power = 84 }, "" },
                   { b.buff("满堂"), 5, { .critical = 4000, .power = 105 }, "" },
                   { b.buff("繁音急节"), 0, { .attack = 461 }, "" } }
    });
    b.damage(6559,
             "江海凝光",
             {
                 20,
                 2,
                 366,
                 0,
                 0,
                 0,
                 false,
                 { .pve = 266 },
                 { { b.buff("满堂"), 1, { .critical = 800, .power = 21 }, "" },
                   { b.buff("满堂"), 2, { .critical = 1600, .power = 42 }, "" },
                   { b.buff("满堂"), 3, { .critical = 2400, .power = 63 }, "" },
                   { b.buff("满堂"), 4, { .critical = 3200, .power = 84 }, "" },
                   { b.buff("满堂"), 5, { .critical = 4000, .power = 105 }, "" },
                   { b.buff("繁音急节"), 0, { .attack = 461 }, "" } }
    });

    b.dot("急曲", "急曲(DOT)·1");
    b.state({ 0, "枕上", 240, 0, 1 }, false);
    b.state({ 0, "待结算枕上", 0, 0, 1 }, false);
    b.stance_haste("枕上", 51, "急曲");
    b.state({ 0, "剑舞长空", 96, 0, 1 }, false);
    b.state({ 0, "霜降剑舞", 0, 0, 5 }, false);
    b.state({ 0, "化冰", 2880, 0, 1 }, false);
    b.state({ 0, "芳姿畅音", 80, 0, 1 }, false);
    b.state({ 0, "流玉", 160, 0, 2 }, false);
    b.state({ 0, "本次流玉", 0, 0, 1 }, false);
    b.state({ 0, "待结算钗燕", 0, 0, 1 }, false);
    b.state({ 0, "盈袖", 0, 0, 6 }, true);
    for (auto n : { "仙姿剑破", "仙姿玳弦", "仙姿剑影", "仙姿剑气" }) {
        b.state({ 0, n, 400, 0, 1 }, false);
    }
    b.damage(6234, "玳弦急曲", { 50, 2, 174, 0, 0, 0, false, { .pve = 266 }, {} });
    b.damage(6554, "玳弦急曲·一", { 11, 0, 77, 0, 0, 0, false, { .pve = 266 }, {} });
    b.damage(40289, "破·霜降", { 50, 10, 25, 0, 0, 0, false, { .pve = 266 }, {} });
    b.damage(24999, "化冰", { 2, 1, 121, 0, 0, 0, false, { .pve = 266 }, {} });
    b.damage(30532, "钗燕", { 13, 1, 485, 0, 0, 0, false, { .pve = 266 }, {} });
    b.damage(37317, "留芳仙姿·剑破", { 75, 0, 402, 0, 0, 0, false, { .pve = 266 }, {} });
    b.damage(37318, "留芳仙姿·玳弦", { 75, 0, 69, 0, 0, 0, false, { .pve = 266 }, {} });
    b.damage(37319, "留芳仙姿·剑气", { 75, 0, 161, 0, 0, 0, false, { .pve = 266 }, {} });
    b.damage(37320, "留芳仙姿·剑影", { 75, 0, 598, 0, 0, 0, false, { .pve = 266 }, {} });
    b.damage(33140, "盈袖", { 75, 0, 200, 0, 0, 0, false, { .pve = 266 }, {} });
    b.damage(33140, "盈袖·剑气", { 80, 0, 300, 0, 0, 0, false, { .pve = 266 }, {} });
    b.damage(33140, "盈袖·江海", { 85, 0, 420, 0, 0, 0, false, { .pve = 266 }, {} });
    const auto mark = [&](int skill) {
        return b.selected("钗燕",
                          { .op             = Op::RecordDistinct,
                            .slot           = b.buff("待结算钗燕"),
                            .amount         = skill,
                            .maximum        = 3,
                            .duration       = 960,
                            .clear_repeated = skill == 4 ? 0x1fu : 0x1ffu });
    };
    const auto hairpin       = b.when("待结算钗燕", b.hit("钗燕"));
    const auto clear_hairpin = b.remove("待结算钗燕");
    // 钗燕命中后给目标挂急曲。江海凝光可在自身命中后引爆急曲剩余跳数。
    b.post_damage("钗燕", { b.apply("急曲") });
    const auto consume_flower = [&](std::string_view n) {
        return b.when("盈袖", b.hit(n));
    };
    b.cast("繁音急节", 864, 24, 0, { .gcd_group = 1 }, { b.apply("繁音急节"), b.apply("满堂", 5), b.gain(0, 2) });
    b.cast("心鼓弦", 480, 24, 0, { .choice = "化冰" }, { b.apply("化冰") });
    b.cast("芳姿畅音", 288, 16, 0, { .choice = "芳姿畅音", .charges = 3, .gcd_group = 2 }, { b.apply("芳姿畅音") });
    b.cast("剑影留痕",
           240,
           0,
           0,
           {},
           { b.when("芳姿畅音", b.apply("仙姿剑影")),
             b.remove("芳姿畅音"),
             b.when("仙姿剑影", b.hit("留芳仙姿·剑影")),
             b.when("仙姿剑影", b.gain(0, 1)),
             mark(2),
             hairpin,
             clear_hairpin,
             b.selected("流玉", b.remove("流玉", 2)),
             b.selected("流玉", b.apply("流玉")),
             b.selected("流玉", b.when("繁音急节", b.apply("流玉"))),
             consume_flower("盈袖"),
             b.remove("盈袖") });
    b.cast("剑破虚空",
           160,
           24,
           0,
           {},
           { b.when("芳姿畅音", b.apply("仙姿剑破")),
             b.remove("芳姿畅音"),
             b.selected("明空", b.apply("剑舞长空")),
             b.when("仙姿剑破", b.hit("留芳仙姿·剑破")),
             b.when("仙姿剑破", b.gain(0, 1)),
             mark(0),
             hairpin,
             clear_hairpin,
             b.hit("剑破虚空"),
             b.gain(0, 1),
             b.apply("急曲"),
             consume_flower("盈袖"),
             b.remove("盈袖"),
             b.remove("满堂") });
    b.cast("剑气长江",
           80,
           24,
           0,
           {},
           { b.hit("剑气长江"),
             b.when("芳姿畅音", b.apply("仙姿剑气")),
             b.remove("芳姿畅音"),
             b.when("仙姿剑气", b.hit("留芳仙姿·剑气")),
             b.when("仙姿剑气", b.gain(0, 1)),
             mark(1),
             hairpin,
             clear_hairpin,
             b.gain(0, 1),
             consume_flower("盈袖·剑气"),
             b.remove("盈袖"),
             b.selected("明空", b.shorten("剑破虚空", 8)),
             b.remove("满堂") });
    b.cast("江海凝光",
           0,
           24,
           0,
           {},
           { b.hit("江海凝光"), b.remove("满堂"), consume_flower("盈袖·江海"), b.remove("盈袖"), b.burst("急曲"), mark(4), hairpin, clear_hairpin });
    for (auto [effect, tag] :
         {
             std::pair{ "留芳仙姿·剑破", 5 },
             std::pair{ "留芳仙姿·剑影", 6 },
             std::pair{ "留芳仙姿·剑气", 7 }
    })
    {
        b.post_damage(effect, { mark(tag), hairpin, clear_hairpin });
    }
    // 玳弦起手记下当前剑舞，并把流玉转存为「本次流玉」。
    // 引导期间流玉到期也保留本次强化；霜降按起手剑舞增加段数和系数。
    const auto spend        = b.resource_stacks("霜降剑舞", 0);
    const auto clear_shadow = b.remove("本次流玉");
    const auto petals       = b.selected("盈袖", b.stacks("急曲", 6, b.apply("盈袖")));
    const auto channel      = [&](bool echo, bool frost) {
        Action action{
            .resource             = 0,
            .pulses               = 3,
            .spacing              = echo ? 12 : 15,
            .channel_cost_step    = 1,
            .extra_pulses         = frost ? 1 : 0,
            .max_cost_steps       = frost ? 5 : 1,
            .pre_cast                = { spend,
                                     clear_shadow, b.when("流玉", b.apply("本次流玉")),
                                     b.remove("流玉"),
                                     mark(3),
                                     b.when("芳姿畅音", b.apply("仙姿玳弦")),
                                     b.remove("芳姿畅音") },
            .timing_resource      = 0,
            .empowered_spacing    = echo ? 7 : 9,
            .clear_on_channel_end = b.buff("本次流玉"),
            .refund_channel_point = b.buff("仙姿玳弦"),
            .distribute_pulses    = true
        };
        if (frost) {
            // 霜降完整收招时追加三次「破·霜降」；下方 impacts 列表则在每个引导段执行。
            action.post_cast.push_back(b.hit("破·霜降", 3));
        }
        if (!echo) {
            action.post_cast.push_back(b.apply("急曲"));
        }
        const std::initializer_list<Step> impacts{
            b.when("化冰", b.hit("化冰")),
            b.when("化冰", b.remove("满堂")),
            b.selected("新妆", b.hit("玳弦急曲·一")),
            b.hit("玳弦急曲"),
            b.remove("满堂", echo ? 2 : 1),
            petals,
            b.selected("盈袖", b.apply("急曲")),
            b.when("仙姿玳弦", b.hit("留芳仙姿·玳弦")),
            hairpin,
            clear_hairpin,
            b.when("待结算枕上", b.apply("枕上")),
            b.remove("待结算枕上")
        };
        if (!echo && !frost) {
            b.cast("玳弦急曲", 0, 24, 0, action, impacts);
        } else {
            b.variant("玳弦急曲", frost ? "霜降" : "新妆", 0, 0, action, impacts, frost && echo ? "新妆" : "");
        }
    };
    channel(false, false);
    channel(true, false);
    channel(false, true);
    channel(true, true);
    b.post_damage("留芳仙姿·玳弦", { mark(8) });
    // 枕上先在这些伤害会心时记一个待结算标记；玳弦在本段末、剑破等在收招时才把它转为增益。
    for (auto n : { "玳弦急曲", "剑破虚空", "剑气长江", "留芳仙姿·剑破", "留芳仙姿·玳弦", "留芳仙姿·剑影", "留芳仙姿·剑气" })
    {
        b.post_damage(n, { b.selected("枕上", b.on_crit("待结算枕上")) });
    }
    for (auto n : { "剑破虚空", "剑气长江", "剑影留痕" }) {
        b.post_cast(n, { b.when("待结算枕上", b.apply("枕上")), b.remove("待结算枕上") });
    }
    b.secondary_damage(25757, "气吞长江(DOT)", {1, 0, 1505, 0, 6, 48, false, {.pve = 266}});
    for (auto n :
         { "玳弦急曲", "玳弦急曲·一", "破·霜降", "化冰", "钗燕", "盈袖", "盈袖·剑气", "盈袖·江海", "留芳仙姿·剑破", "留芳仙姿·剑气", "留芳仙姿·玳弦", "留芳仙姿·剑影", "气吞长江(DOT)" })
    {
        b.pre_damage(n, { b.buff("繁音急节"), 0, { .attack = 461 } });
        for (int i = 1; i <= 5; i++) {
            b.pre_damage(n,
                       {
                           b.buff("满堂"),
                           i,
                           { .critical = 800 * i, .power = (i * 1024 + 25) / 50 }
            });
        }
    }
    for (auto n :
         { "玳弦急曲", "玳弦急曲·一", "破·霜降", "化冰", "钗燕", "盈袖", "盈袖·剑气", "盈袖·江海", "留芳仙姿·剑破", "留芳仙姿·剑气", "留芳仙姿·玳弦", "留芳仙姿·剑影", "剑破虚空", "剑气长江", "江海凝光", "急曲(DOT)·1", "气吞长江(DOT)" })
    {
        b.pre_damage(n,
                   {
                       b.buff("剑舞长空"),
                       0,
                       { .critical = 500, .power = 205 }
        });
        b.pre_damage(n, { b.buff("枕上"), 0, { .ignore = 512 } });
        b.pre_damage(n, { b.buff("化冰"), 0, { .strain_rating = 307 } });
        for (auto pose : { "仙姿剑破", "仙姿玳弦", "仙姿剑气", "仙姿剑影" }) {
            b.pre_damage(n, { b.buff(pose), 0, { .final_damage = 51 } });
        }
    }
    for (auto n : { "玳弦急曲", "玳弦急曲·一" }) {
        b.pre_damage(n, { b.buff("本次流玉"), 0, { .damage = 922 } });
        for (int i = 1; i <= 5; ++i) {
            b.pre_damage(n, { .aura = b.buff("霜降剑舞"), .stacks = i, .stats = {}, .choice = "霜降", .channel_scale = 1. + .1 * i });
        }
    }
    for (int i = 1; i <= 5; ++i) {
        b.pre_damage("破·霜降", { .aura = b.buff("霜降剑舞"), .stacks = i, .stats = {}, .choice = "霜降", .channel_scale = 1. + i });
    }
    // 套装同时提高玳弦急曲本体和新妆追加段的伤害，两条命中记录分别受益。
    b.set_bonus({ .damage = 102 }, { "玳弦急曲", "玳弦急曲·一" });
    b.recipe("剑气长江", "剑气长江·2%会心", { .critical = 200 }, { "剑气长江" });
    b.recipe("剑气长江", "剑气长江·3%会心", { .critical = 300 }, { "剑气长江" });
    b.recipe("剑气长江", "剑气长江·4%伤害", { .damage = 41 }, { "剑气长江" });
    b.recipe("剑气长江", "剑气长江·6%伤害", { .damage = 61 }, { "剑气长江" });
    b.recipe("江海凝光", "江海凝光·2%会心", { .critical = 200 }, { "江海凝光" });
    b.recipe("江海凝光", "江海凝光·3%会心", { .critical = 300 }, { "江海凝光" });
    b.recipe("玳弦急曲", "玳弦急曲·5%伤害", { .damage = 51 }, { "玳弦急曲", "玳弦急曲·一" });
    b.recipe("玳弦急曲", "玳弦急曲·4%伤害", { .damage = 41 }, { "玳弦急曲", "玳弦急曲·一" });
    b.recipe("玳弦急曲", "玳弦急曲·3%伤害", { .damage = 31 }, { "玳弦急曲", "玳弦急曲·一" });
    b.recipe("江海凝光", "江海凝光·15%伤害（一）", { .damage = 154 }, { "江海凝光" });
    b.recipe("江海凝光", "江海凝光·15%伤害（二）", { .damage = 154 }, { "江海凝光" });
    b.recipe("剑气长江", "剑气长江·调息减1秒", {}, {});
    b.adjust([](Prepared &p) {
        const auto &bk    = *p.config.book;
        auto        skill = [&](std::string_view n) {
            return std::ranges::find(bk.skills, n, &SkillDescriptor::name) - bk.skills.begin();
        };
        if (std::ranges::contains(p.config.talents, std::string_view{ "明空" })) {
            p.skills[skill("剑破虚空")].cooldown = 64;
        }
        if (std::ranges::contains(p.config.recipes, std::string_view{ "剑气长江·调息减1秒" })) {
            p.skills[skill("剑气长江")].cooldown -= 16;
        }
        if (p.config.precombat) {
            p.resources[0].initial = 5;
            if (std::ranges::contains(p.config.talents, std::string_view{ "化冰" })) {
                auto slot = static_cast<Slot>(std::ranges::find(bk.buffs, std::string_view{ "化冰" }, &BuffDescriptor::name) -
                                              bk.buffs.begin());
                p.opening.push_back({ Op::Add, slot });
            }
        }
    });
    b.state({0, "冽心", 96, 0, 1}, false);
    b.state({18512, "气吞长江", 288, 48, 3}, true, true, true);
    b.dot("气吞长江", "气吞长江(DOT)");
    // 使用橙武立即重置剑气长江、剑破虚空并开启冽心；窗口内再施放指定招式时缩短调息、回剑舞并挂气吞长江。
    b.cast("使用橙武", 800, 0, 0, {.choice = Legendary}, {b.reset("剑气长江"), b.reset("剑破虚空"), b.apply("冽心")});
    for (auto name : {"剑气长江", "剑破虚空"}) {
        b.post_cast(name, {b.when("冽心", b.shorten(name, 48)), b.when("冽心", b.gain(0, 1)), b.when("冽心", b.apply("气吞长江"))});
    }
    b.legendary_damage({ "玳弦急曲", "玳弦急曲·一", "江海凝光" });
    // 灼桃先按命中前的急曲层数判定本击双会；随后推进旧急曲两跳，再补一层。
    for (int layers = 1; layers <= 6; ++layers)
        b.pre_damage("剑气长江", {b.buff("急曲"), layers, {.critical = 1000 * layers, .power = 102 * layers}, "灼桃"});
    b.post_damage("剑气长江", {b.selected("灼桃", b.advance("急曲", 2)), b.selected("灼桃", b.apply("急曲"))});
    return b.finish();
}
} // namespace JX3DPS::runtime::season50
