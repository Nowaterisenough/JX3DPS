#include "common/builder.hpp"

namespace JX3DPS::runtime::season50 {
Book 易筋经()
{
    Builder b("yjj", "易筋经", R"(# 魔盒宏适配 https://www.jx3box.com/macro/110807
/cast 使用风腰坠
/cast 使用橙武
/cast [nobuff:擒龙&禅那<3] 擒龙诀
/cast [禅那>2&bufftime:罗汉金身<2] 罗汉金身
/cast [tnobuff:横扫六合DOT|tbufftime:横扫六合DOT<2] 横扫六合
/cast [禅那<3] 捉影式
/cast [buff:贪破>=3&nobuff:擒龙&禅那<3] 捕风式
/cast [禅那>2] 拿云式
/cast [禅那>2] 韦陀献杵
/cast 守缺式
/cast 横扫六合
/cast 捕风式
/cast 普渡四方
)");
    auto &school = b.definition();
    school.info = { school.code, "少林", "元气", "Solar", 666, 102, 0 };
    school.display_order = 16;
    school.saved.macro_digest = "ecb0a04cbe3b8220a5bc4a12c33f7c70242c1891a1229f56c81621767e55f838";
    school.precombat = true;
    b.preset(2300,
             { 6606, 1495, 193, 1489, 208, 1290, 4212, 0, 1092 },
             { "拿云式·5%伤害", "拿云式·4%伤害", "拿云式·3%伤害", "拿云式·命中回禅",
               "擒龙诀·不再回禅", "擒龙诀·延长一秒（一）", "擒龙诀·延长一秒（二）", "擒龙诀·延长一秒（三）",
               "普渡四方·4%伤害", "普渡四方·5%伤害", "普渡四方·调息减4帧（一）", "普渡四方·调息减4帧（二）",
               "横扫六合·50%伤害", "横扫六合·延长8秒",
               "守缺式·5%伤害", "守缺式·4%会心", "守缺式·4%伤害", "守缺式·3%伤害",
               "韦陀献杵·4%伤害", "韦陀献杵·5%伤害", "韦陀献杵·4%会心", "韦陀献杵·命中回禅",
               "捕风式·调息减半秒（一）", "捕风式·调息减半秒（二）", "捕风式·15%伤害", "捕风式·10%伤害" });
    b.choices({ "净果", "众嗔", "业因", "三生", "金刚日轮", "我闻", "身意", "布泽", "金刚怒目", "系珠" }, {}, {});
    b.default_talents({ "净果", "众嗔", "业因", "三生", "金刚日轮", "我闻", "身意", "布泽", "金刚怒目" });
    b.resource({ "禅那", 3, 0, 3 });
    b.state({ 0, "伏魔", 0, 0, 1 }, false);
    b.initial("伏魔");
    b.state({ 0, "身意", 128, 0, 1 }, false);
    b.state({ 0, "三禅出手", 0, 0, 1 }, false);
    b.state({ 0, "三生", 480, 0, 1 }, false);
    b.state({ 0, "横扫六合DOT", 192, 32, 2 }, true, true, true);
    b.state({ 0, "罗汉金身", 480, 0, 1 }, false, false, false);
    b.state({ 0, "贪破", 9600, 0, 6 }, false, false, false);
    b.damage(0, "横扫六合(DOT)", { 45, 0, 757, 0, 6, 32, false, { .pve = 174 }, { { b.buff("罗汉金身"), 0, { .overcome = 205 }, "" } } });
    b.damage(3810, "横扫六合", { 75, 0, 530, 0, 0, 0, false, { .pve = 174 }, { { b.buff("罗汉金身"), 0, { .overcome = 205 }, "" } } });
    b.damage(0,
             "横扫六合·武伤",
             {
                 0,
                 0,
                 16,
                 1024,
                 0,
                 0,
                 true,
                 { .attack = -1024, .pve = 174 },
                 {}
    });
    b.damage(3816, "守缺式", { 142, 5, 475, 0, 0, 0, false, { .pve = 174 }, { { b.buff("罗汉金身"), 0, { .overcome = 205 }, "" } } });
    b.damage(17641, "普渡四方", { 15, 1, 558, 0, 0, 0, false, { .pve = 174 }, { { b.buff("罗汉金身"), 0, { .overcome = 205 }, "" } } });
    b.damage(0,
             "普渡四方·武伤",
             {
                 0,
                 0,
                 16,
                 1024,
                 0,
                 0,
                 true,
                 { .attack = -1024, .pve = 174 },
                 {}
    });
    b.damage(3849, "韦陀献杵·一", { 6, 0, 203, 0, 0, 0, false, { .pve = 174 }, { { b.buff("罗汉金身"), 0, { .overcome = 205 }, "" } } });
    b.damage(3850, "韦陀献杵·二", { 12, 0, 406, 0, 0, 0, false, { .pve = 174 }, { { b.buff("罗汉金身"), 0, { .overcome = 205 }, "" } } });
    b.damage(3848, "韦陀献杵·三", { 105, 1, 609, 0, 0, 0, false, { .pve = 174 }, { { b.buff("罗汉金身"), 0, { .overcome = 205 }, "" } } });
    b.damage(0,
             "韦陀献杵·武伤",
             {
                 0,
                 0,
                 16,
                 2048,
                 0,
                 0,
                 true,
                 { .attack = -1024, .pve = 174 },
                 {}
    });
    b.damage(271, "降魔", { 360, 0, 295, 0, 0, 0, false, { .pve = 174 }, {} });
    b.damage(271, "降魔·拿云", { 420, 0, 418, 0, 0, 0, false, { .pve = 174 }, {} });
    b.damage(271, "降魔·清贪", { 70, 0, 208, 0, 0, 0, false, { .pve = 174 }, {} });
    b.damage(38615, "布泽", { 0, 0, 270, 0, 0, 0, false, { .pve = 174 }, {} });
    b.ranks("布泽", { 270, 560, 880, 1280, 1680, 2080 });
    for (int layers = 1; layers <= 6; ++layers) {
        // 每层贪破使降魔、降魔·拿云的伤害降低约 5%。逐层引爆使用独立的「降魔·清贪」伤害条目。
        for (auto n : { "降魔", "降魔·拿云" }) {
            b.pre_damage(n, { b.buff("贪破"), layers, { .fragile = -static_cast<int>(std::lround(layers * 0.05 * 1024)) } });
        }
    }
    b.dot("横扫六合DOT", "横扫六合(DOT)");
    b.independent_dot_hits("横扫六合DOT");
    b.cast("罗汉金身", 320, 0, 0, { .resource = 0, .cost = 1 }, { b.resource_aura("罗汉金身", 0), b.gain(0, -3) });
    b.cast("横扫六合", 144, 24, 0, {}, { b.hit("横扫六合", 1), b.hit("横扫六合·武伤", 1), b.apply("横扫六合DOT", 1) });
    b.cast("守缺式", 112, 24, 0, { .charges = 3 }, { b.hit("守缺式", 1) });
    b.cast("普渡四方", 0, 24, 0, {}, { b.hit("普渡四方", 1), b.hit("普渡四方·武伤", 1), b.gain(0, 1) });
    b.cast("韦陀献杵",
           0,
           24,
           0,
           { .resource = 0, .cost = 1 },
           { b.tier("韦陀献杵·一", 0), b.hit("韦陀献杵·武伤", 1), b.at(0, 3, 3, b.hit("降魔", 1)), b.at(0, 3, 3, b.apply("贪破", 1)) });
    b.recipe("守缺式", "守缺式·2%会心", { .critical = 200 }, { "守缺式" });
    b.recipe("守缺式", "守缺式·3%会心", { .critical = 300 }, { "守缺式" });
    b.recipe("守缺式", "守缺式·3%伤害", { .damage = 31 }, { "守缺式" });
    b.recipe("守缺式", "守缺式·4%会心", { .critical = 400 }, { "守缺式" });
    b.recipe("守缺式", "守缺式·4%伤害", { .damage = 41 }, { "守缺式" });
    b.recipe("守缺式", "守缺式·5%伤害", { .damage = 51 }, { "守缺式" });
    b.recipe("普渡四方", "普渡四方·3%伤害", { .damage = 31 }, { "普渡四方" });
    b.recipe("普渡四方", "普渡四方·4%伤害", { .damage = 41 }, { "普渡四方" });
    b.recipe("普渡四方", "普渡四方·5%伤害", { .damage = 51 }, { "普渡四方" });
    b.recipe("横扫六合", "横扫六合·50%伤害", { .damage = 512 }, { "横扫六合" });
    b.recipe("韦陀献杵", "韦陀献杵·2%会心", { .critical = 200 }, { "韦陀献杵·一", "韦陀献杵·三", "韦陀献杵·二" });
    b.recipe("韦陀献杵", "韦陀献杵·3%会心", { .critical = 300 }, { "韦陀献杵·一", "韦陀献杵·三", "韦陀献杵·二" });
    b.recipe("韦陀献杵", "韦陀献杵·3%伤害", { .damage = 31 }, { "韦陀献杵·一", "韦陀献杵·三", "韦陀献杵·二" });
    b.recipe("韦陀献杵", "韦陀献杵·4%会心", { .critical = 400 }, { "韦陀献杵·一", "韦陀献杵·三", "韦陀献杵·二" });
    b.recipe("韦陀献杵", "韦陀献杵·4%伤害", { .damage = 41 }, { "韦陀献杵·一", "韦陀献杵·三", "韦陀献杵·二" });
    b.recipe("韦陀献杵", "韦陀献杵·5%伤害", { .damage = 51 }, { "韦陀献杵·一", "韦陀献杵·三", "韦陀献杵·二" });
    b.set_bonus({ .damage = 102 }, { "韦陀献杵·一", "韦陀献杵·二", "韦陀献杵·三" });
    b.state({ 0, "拿云", 1584, 0, 3 }, false);
    b.state({ 0, "擒龙", 240, 0, 1 }, false);
    b.state({ 0, "擒龙回禅", 240, 24, 1 }, false);
    b.state({ 0, "果报", 960, 0, 8 }, false);
    b.state({ 0, "金刚日轮", 400, 0, 1 }, false);
    b.damage(32659, "果报", { 240, 15, 1432, 0, 0, 0, false, { .pve = 174 }, {} });
    b.damage(32656, "金刚龙爪功", { 0, 0, 390, 0, 0, 0, false, { .pve = 174 }, {} });
    b.damage(14951, "捕风式", { 6, 6, 150, 0, 0, 0, false, { .pve = 174 }, {} });
    b.damage(13682, "拿云式·一", { 8, 0, 218, 0, 0, 0, false, { .pve = 174 }, {} });
    b.damage(13684, "拿云式·二", { 16, 1, 438, 0, 0, 0, false, { .pve = 174 }, {} });
    b.damage(13686, "拿云式·三", { 46, 2, 659, 0, 0, 0, false, { .pve = 174 }, {} });
    for (auto n : { "拿云式·一", "拿云式·二", "拿云式·三", "韦陀献杵·一", "韦陀献杵·二", "韦陀献杵·三" }) {
        b.pre_damage(n, { b.buff("擒龙"), 0, { .damage = 307 }, "众嗔" });
    }
    b.cast("拿云式",
           0,
           24,
           0,
           { .resource = 0, .cost = 1, .required = b.buff("拿云"), .execute_below = 3000 },
           { b.tier("拿云式·一", 0), b.life(2999, 10000, b.remove("拿云")), b.at(0, 3, 3, b.apply("三禅出手")) });
    b.recipe("拿云式", "拿云式·命中回禅", {}, {});
    b.recipe("拿云式", "拿云式·5%伤害", { .damage = 51 }, { "拿云式·一", "拿云式·二", "拿云式·三" });
    b.recipe("拿云式", "拿云式·4%伤害", { .damage = 41 }, { "拿云式·一", "拿云式·二", "拿云式·三" });
    b.recipe("拿云式", "拿云式·3%伤害", { .damage = 31 }, { "拿云式·一", "拿云式·二", "拿云式·三" });
    b.cast("捕风式", 96, 24, 0, {}, { b.hit("捕风式"), b.gain(0, 1) });
    b.cast("捉影式", 320, 24, 0, { .gcd_group = 1 }, { b.gain(0, 1) });
    b.cast("擒龙诀", 960, 0, 0, { .charges = 2 }, { b.gain(0, 3), b.apply("擒龙"), b.apply("擒龙回禅"), b.selected("众嗔", b.apply("果报", 4)) });
    b.recipe("擒龙诀", "擒龙诀·不再回禅", {}, {});
    for (auto n : { "擒龙诀·延长一秒（一）", "擒龙诀·延长一秒（二）", "擒龙诀·延长一秒（三）" }) {
        b.recipe("擒龙诀", n, {}, {});
    }
    for (auto n : { "捕风式·调息减半秒（一）", "捕风式·调息减半秒（二）" }) {
        b.recipe("捕风式", n, {}, {});
    }
    b.recipe("横扫六合", "横扫六合·延长8秒", {}, {});
    b.recipe("韦陀献杵", "韦陀献杵·命中回禅", {}, {});
    b.recipe("捕风式", "捕风式·15%伤害", { .damage = 154 }, { "捕风式" });
    b.recipe("捕风式", "捕风式·10%伤害", { .damage = 102 }, { "捕风式" });
    b.post_cast("擒龙诀", { b.selected("擒龙诀·不再回禅", b.remove("擒龙回禅")) });
    b.pulse("擒龙回禅", { b.gain(0, 1) });
    b.post_cast("守缺式", { b.gain(0, 1), b.selected("身意", b.apply("身意")) });
    b.post_cast("横扫六合", { b.gain(0, 1), b.selected("金刚日轮", b.apply("金刚日轮")) });
    b.variant("罗汉金身",
              "三生",
              320,
              0,
              { .resource = 0, .cost = 1 },
              { b.resource_aura("罗汉金身", 0), b.at(0, 1, 2, b.gain(0, -3)), b.apply("三生") });
    for (auto n : { "捕风式", "守缺式", "拿云式" }) {
        b.pre_cast(n, { b.when("金刚日轮", b.hit("金刚龙爪功")) });
    }
    // 装上「擒龙诀·不再回禅」后，擒龙期间改由指定技能收招回 1 禅；果报则在自身命中后触发同一回禅效果。
    const auto extra_zen = b.selected("擒龙诀·不再回禅", b.when("擒龙", b.gain(0, 1)));
    for (auto n : { "普渡四方", "捕风式", "守缺式", "横扫六合", "捉影式" }) {
        b.post_cast(n, { extra_zen });
    }
    // 拿云、韦陀在收招时先扣禅。此时已有至少两层果报才打一次果报、返 1 禅并扣两层。
    for (auto n : { "拿云式", "韦陀献杵" }) {
        b.pre_cast(n, { b.selected("身意", b.apply("身意")) });
        b.post_cast(n,
                     { b.gain(0, -3),
                       b.at_least("果报", 2, b.hit("果报")),
                       b.at_least("果报", 2, b.gain(0, 1)),
                       b.at_least("果报", 2, b.remove("果报", 2)) });
    }
    b.post_damage("果报", { extra_zen });
    b.post_cast("韦陀献杵", { b.selected("韦陀献杵·命中回禅", b.gain(0, 1)), b.selected("众嗔", b.when("擒龙", b.apply("拿云"))) });
    b.post_cast("拿云式", { b.selected("拿云式·命中回禅", b.gain(0, 1)) });
    // 清贪排在本次果报判定之后。选业因会把旧贪破转成果报，本次拿云随后新增的贪破也留给后续招式。
    const auto clear_greed = [&](std::string_view spell, bool in_window) {
        auto gated = [&](Step step) {
            return in_window ? b.selected("众嗔", b.when("擒龙", step)) : step;
        };
        b.post_cast(spell,
                     { gated(b.hits_per_stack("降魔·清贪", "贪破")),
                       gated(b.selected("布泽", { Op::StackRankHit, b.effect("布泽"), b.buff("贪破") })),
                       gated(b.selected("业因", b.convert("贪破", "果报"))),
                       gated(b.remove("贪破", 6)) });
    };
    clear_greed("捕风式", false);
    clear_greed("拿云式", true);
    b.post_cast("拿云式", { b.when("三禅出手", b.hit("降魔·拿云")), b.when("三禅出手", b.apply("贪破")), b.remove("三禅出手") });
    // 净果每次判定有 30% 概率给拿云。横扫六合在循环外又登记一次，所以一次横扫会做两次独立判定。
    const Step opportunity = b.selected("净果", { Op::ChanceAura, b.buff("拿云"), 3000 });
    for (auto n : { "捕风式", "守缺式", "横扫六合", "普渡四方", "拿云式", "韦陀献杵" }) {
        b.post_cast(n, { opportunity });
    }
    b.post_cast("横扫六合", { opportunity });
    b.passive("横扫六合", "我闻", { .critical = 1000, .power = 205 });
    b.passive("横扫六合(DOT)", "我闻", { .critical = 1000, .power = 205 });
    for (auto n :
         { "横扫六合", "横扫六合(DOT)", "果报", "拿云式·一", "拿云式·二", "拿云式·三", "韦陀献杵·一", "韦陀献杵·二", "韦陀献杵·三", "守缺式", "捕风式", "普渡四方", "金刚龙爪功", "降魔", "降魔·拿云", "降魔·清贪", "布泽" })
    {
        b.pre_damage(n, { b.buff("伏魔"), 0, { .damage = 154 } });
        b.pre_damage(n, { b.buff("擒龙"), 0, { .attack = 204 } });
        b.pre_damage(n,
                   {
                       b.buff("罗汉金身"),
                       0,
                       { .attack = 205, .ignore = 614 },
                       "金刚怒目"
        });
        b.pre_damage(n,
                   {
                       b.buff("金刚日轮"),
                       0,
                       { .critical = 600, .power = 62 }
        });
        b.pre_damage(n,
                   {
                       b.buff("身意"),
                       0,
                       { .critical = 1000, .power = 102 }
        });
        b.pre_damage(n, { b.buff("三生"), 0, { .strain_rating = 307 } });
    }
    for (auto n : { "果报", "拿云式·一", "拿云式·二", "拿云式·三", "捕风式", "金刚龙爪功", "降魔", "降魔·拿云", "降魔·清贪", "布泽" })
    {
        b.pre_damage(n, { b.buff("罗汉金身"), 0, { .overcome = 205 } });
    }
    b.adjust([](Prepared &p) {
        const auto &book = *p.config.book;
        auto        aura = [&](std::string_view n) {
            return static_cast<Slot>(std::ranges::find(book.buffs, n, &BuffDescriptor::name) - book.buffs.begin());
        };
        auto skill = [&](std::string_view n) {
            return static_cast<Slot>(std::ranges::find(book.skills, n, &SkillDescriptor::name) - book.skills.begin());
        };
        auto talent = [&](std::string_view n) {
            return std::ranges::contains(p.config.talents, n);
        };
        auto recipe = [&](std::string_view n) {
            return std::ranges::contains(p.config.recipes, n);
        };
        if (p.config.precombat && talent("金刚日轮")) {
            p.opening.push_back({ .op = Op::Add, .slot = aura("金刚日轮"), .duration = 200 });
        }
        const int duration =
            (talent("系珠") ? 192 : 240) - 80 * talent("众嗔") +
            16 * (recipe("擒龙诀·延长一秒（一）") + recipe("擒龙诀·延长一秒（二）") + recipe("擒龙诀·延长一秒（三）"));
        for (auto &step : p.impacts[skill("擒龙诀")]) {
            if (step.op == Op::Add && (step.slot == aura("擒龙") || step.slot == aura("擒龙回禅"))) {
                step.duration = duration;
            }
        }
        p.skills[skill("捕风式")].cooldown -=
            8 * (recipe("捕风式·调息减半秒（一）") + recipe("捕风式·调息减半秒（二）"));
        if (talent("我闻")) {
            p.skills[skill("横扫六合")].cooldown -= 16;
        } else {
            p.aura_stack_caps[aura("横扫六合DOT")] = 1;
        }
        if (recipe("横扫六合·延长8秒")) {
            p.dot_ticks[aura("横扫六合DOT")] += 4;
        }
    });
    b.timing_recipe("普渡四方", "普渡四方·调息减4帧（一）", "普渡四方", -4, 0);
    b.timing_recipe("普渡四方", "普渡四方·调息减4帧（二）", "普渡四方", -4, 0);
    b.state({0, "万骨回澜", 96, 0, 1}, false);
    // 万骨回澜开启时回 3 禅，窗口内守缺免调息；韦陀的三个档位都获得增伤，施放结束再回 3 禅。
    b.cast("使用橙武", 800, 0, 0, {.choice = Legendary}, {b.gain(0, 3), b.apply("万骨回澜")});
    b.no_cooldown_during("守缺式", "万骨回澜");
    for (auto hit : {"韦陀献杵·一", "韦陀献杵·二", "韦陀献杵·三"}) b.pre_damage(hit, {b.buff("万骨回澜"), 0, {.damage = 1536}});
    b.post_cast("韦陀献杵", {b.when("万骨回澜", b.gain(0, 3))});
    b.legendary_damage({ "普渡四方", "守缺式" });
    // 系珠将擒龙诀的每层充能改为 25 秒，并将增益及其回禅计时一同改为 12 秒。
    b.charge_variant("擒龙诀", "系珠", 400, 2);
    b.school_modifier({b.buff("擒龙"), 0, {.critical = 1000}, "系珠"});
    return b.finish();
}
} // namespace JX3DPS::runtime::season50
