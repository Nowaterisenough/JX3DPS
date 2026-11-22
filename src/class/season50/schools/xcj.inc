// Native event rules for 笑尘诀. Timing is expressed in frames.
static Book xcj()
{
    Builder b("xcj", "笑尘诀", R"(# 苍狂：苍龙起手、铺灼烧、时乘接狂龙 https://www.jx3box.com/macro/111035
/cast 使用风腰坠
/cast 使用橙武
/cast [skill_energy:落水打狗=2|last_skill:亢龙有悔&skill_notin_cd:蜀犬吠日&buff:苍龙] 落水打狗
/cast [last_skill:龙跃于渊&mana=1] 时乘六龙
/cast [mana=1&buff:苍龙&cd:蜀犬吠日>22&tbufftime:灼烧<20] 龙跃于渊
/cast [mana=1&buff:苍龙&cd:蜀犬吠日>22&tbufftime:灼烧>24] 时乘六龙
# 含弘将断时优先龙战，取自苍狂三键宏 https://www.jx3box.com/macro/110826
/cast [buff:狂龙乱舞&bufftime:含弘<2] 龙战于野
/cast [tbuff:灼烧] 狂龙乱舞
/cast [cd:蜀犬吠日<4] 苍龙戾天
/cast [buff:苍龙&醉意>0&nobuff:祭湘君] 祭湘君
/cast [nobuff:狂龙乱舞] 酒中仙
/cast [buff:苍龙&mana<0.3] 蜀犬吠日
/cast [cd:蜀犬吠日>10&mana<0.3&tbufftime:灼烧<15] 龙跃于渊
/cast [nobuff:狂龙乱舞&mana>0.36&cd:苍龙戾天<4] 龙战于野
/cast [nobuff:狂龙乱舞&mana>0.32] 亢龙有悔
/cast [nobuff:狂龙乱舞] 横打双獒
/cast [nobuff:狂龙乱舞] 拨狗朝天
)");
    b.preset(2300,
             { 6733, 1287, 193, 1260, 208, 1727, 4212, 0, 1092 },
             { "拨狗朝天·4%伤害", "拨狗朝天·5%会心", "拨狗朝天·4%会心", "拨狗朝天·回蓝3%",
               "横打双獒·5%伤害", "横打双獒·4%伤害", "横打双獒·4%会心", "横打双獒·3%伤害",
               "龙跃于渊·5%伤害", "龙跃于渊·4%伤害", "龙跃于渊·3%伤害", "龙跃于渊·无消耗内力",
               "亢龙有悔·5%伤害", "亢龙有悔·4%伤害", "亢龙有悔·7%会心", "亢龙有悔·含弘",
               "蜀犬吠日·5%伤害", "蜀犬吠日·5%会心", "蜀犬吠日·4%伤害", "蜀犬吠日·4%会心",
               "狂龙乱舞·5%伤害", "狂龙乱舞·4%伤害", "狂龙乱舞·3%伤害",
               "龙战于野·5%伤害", "龙战于野·3%会心", "龙战于野·4%伤害", "龙战于野·3%伤害",
               "恶狗拦路·5%伤害", "恶狗拦路·4%伤害", "恶狗拦路·3%伤害",
               "天下无狗·5%伤害", "天下无狗·4%伤害", "天下无狗·3%伤害",
               "龙腾五岳·5%会心", "龙腾五岳·4%伤害", "龙腾五岳·4%会心", "龙腾五岳·3%伤害",
               "笑醉狂·内力回复",
               "龙啸九天·3%伤害", "龙啸九天·4%伤害", "龙啸九天·5%伤害" });
    b.choices({ "雨龙", "自强", "越渊", "追远", "苍龙戾天", "祭湘君", "落水打狗", "酩酊", "坚冰" }, {}, {});
    b.resource({ .name = "内力", .initial = 100, .low = 0, .high = 100, .mana = true });
    b.state({ 0, "醉意已满", 0, 0, 1 }, false);
    b.resource({ "醉意", 0, 0, 3 });
    const auto mana = [](int quarters) {
        return Step{ Op::ManaQuarters, 0, quarters };
    };
    b.state({ 0, "昊破", 1120, 0, 1 }, false, false, false);
    b.state({ 0, "触类", 480, 0, 2 }, false, false, false);
    b.state({ 0, "含弘", 192, 0, 1 }, false, false, false);
    b.state({ 0, "酩酊大醉", 9600, 0, 1 }, false, false, false);
    b.state({ 0, "狂龙乱舞", 128, 0, 1 }, false, false, false);
    b.state({ 0, "祭湘君", 240, 0, 3 }, false, false, false);
    b.state({ 0, "祭湘君连击", 62, 0, 5 }, false, false, false);
    b.state({ 0, "连击", 80, 4, 100 }, false, false, true);
    b.state({ 0, "苍龙", 256, 0, 1 }, false, false, false);
    b.state({ 0, "苍龙腾空", 28, 0, 1 }, false, false, false);
    b.state({ 0, "落水打狗", 480, 0, 10 }, true, false, false);

    b.state({ 0, "横打双獒", 40, 0, 1 }, false, false, false);
    b.state({ 0, "灼烧", 0, 48, 1 }, true, true, true);
    b.damage(0, "灼烧(DOT)", { 1, 0, 1830, 0, 5, 48, true, {}, {} });
    b.damage(13524, "拨狗朝天·真实", { 70, 10, 429, 0, 0, 0, true, {}, {} });
    b.damage(6377, "横打双獒·一", { 90, 10, 242, 0, 0, 0, true, {}, {} });
    b.damage(6378, "横打双獒·二", { 120, 20, 323, 0, 0, 0, true, {}, {} });
    b.damage(6355, "龙跃于渊·一", { 3, 1, 163, 0, 0, 0, true, {}, {} });
    b.damage(6356, "龙跃于渊·二", { 3, 1, 116, 0, 0, 0, true, {}, {} });
    b.damage(6357, "龙跃于渊·三", { 3, 1, 140, 0, 0, 0, true, {}, {} });
    b.dot("灼烧", "灼烧(DOT)");
    b.cast("拨狗朝天",
           0,
           24,
           0,
           { .forbidden = b.buff("横打双獒"), .check_gcd_group = 1 },
           { mana(60), b.selected("拨狗朝天·回蓝3%", mana(12)), b.hit("拨狗朝天·真实"), b.remove("狂龙乱舞"), b.apply("横打双獒") });
    b.cast("横打双獒",
           0,
           24,
           0,
           { .required = b.buff("横打双獒"), .check_gcd_group = 1 },
           { mana(60), b.remove("横打双獒"), b.hit("横打双獒·一"), b.hit("横打双獒·二") });
    b.recipe("拨狗朝天", "拨狗朝天·2%会心", { .critical = 200 }, { "拨狗朝天·真实" });
    b.recipe("拨狗朝天", "拨狗朝天·3%伤害", { .damage = 31 }, { "拨狗朝天·真实" });
    b.recipe("拨狗朝天", "拨狗朝天·4%会心", { .critical = 400 }, { "拨狗朝天·真实" });
    b.recipe("拨狗朝天", "拨狗朝天·4%伤害", { .damage = 41 }, { "拨狗朝天·真实" });
    b.recipe("拨狗朝天", "拨狗朝天·5%会心", { .critical = 500 }, { "拨狗朝天·真实" });
    b.recipe("横打双獒", "横打双獒·3%会心", { .critical = 300 }, { "横打双獒·一", "横打双獒·二" });
    b.recipe("横打双獒", "横打双獒·3%伤害", { .damage = 31 }, { "横打双獒·一", "横打双獒·二" });
    b.recipe("横打双獒", "横打双獒·4%会心", { .critical = 400 }, { "横打双獒·一", "横打双獒·二" });
    b.recipe("横打双獒", "横打双獒·4%伤害", { .damage = 41 }, { "横打双獒·一", "横打双獒·二" });
    b.recipe("横打双獒", "横打双獒·5%伤害", { .damage = 51 }, { "横打双獒·一", "横打双獒·二" });
    b.recipe("龙跃于渊", "龙跃于渊·3%伤害", { .damage = 31 }, { "龙跃于渊·一", "龙跃于渊·三", "龙跃于渊·二" });
    b.recipe("龙跃于渊", "龙跃于渊·4%伤害", { .damage = 41 }, { "龙跃于渊·一", "龙跃于渊·三", "龙跃于渊·二" });
    b.recipe("龙跃于渊", "龙跃于渊·5%伤害", { .damage = 51 }, { "龙跃于渊·一", "龙跃于渊·三", "龙跃于渊·二" });
    b.extra_tick("灼烧", "雨龙", 6);
    b.passive("灼烧(DOT)", "雨龙", { .channel_multiplier = 1.67 });
    const std::array<std::string_view, 6> palms{ "亢龙有悔·一", "亢龙有悔·二", "亢龙有悔·三",
                                                 "亢龙有悔·四", "亢龙有悔·五", "亢龙有悔·六" };
    for (int i = 0; i < 5; ++i) {
        b.damage(6369 + i, palms[i], { 80, 10, 188.44937779245353, 0, 0, 0, true, {}, {} });
    }
    b.damage(6374, palms[5], { 1, 1, 509.5593639936, 0, 0, 0, true, {}, {} });
    b.state({6466, "青龙映夜", 96, 0, 1}, false);
    b.state({0, "神兵亢龙", 20, 0, 1}, false);
    b.cast("亢龙有悔",
           0,
           24,
           0,
           { .check_gcd_group = 1, .minimum_mana_quarters = 120, .weapon_free_cost = b.buff("青龙映夜") },
           { b.when("青龙映夜", mana(-400), true),
             b.later(palms[0], 1),
             b.later(palms[1], 2),
             b.later(palms[2], 3),
             b.later(palms[3], 4),
             b.later(palms[4], 6),
             b.later(palms[5], 19) });
    b.set_bonus({ .damage = 205 }, { "亢龙有悔·一", "亢龙有悔·二", "亢龙有悔·三", "亢龙有悔·四", "亢龙有悔·五", "亢龙有悔·六" });
    b.damage(38891, "追远", { 2, 1, 500, 0, 0, 0, true, {}, {} });
    for (auto n : { "拨狗朝天·真实", "横打双獒·一", "横打双獒·二" }) {
        b.on_hit(n, { b.selected("追远", b.hit("追远")) });
    }
    b.damage(46373, "落水打狗·命中", { 17, 1, 100, 0, 0, 0, true, {}, {} });
    b.damage(6353, "蜀犬吠日·一", { 5, 1, 91, 0, 0, 0, true, {}, {} });
    b.damage(6351, "蜀犬吠日·二", { 5, 1, 114, 0, 0, 0, true, {}, {} });
    b.damage(6352, "蜀犬吠日·三", { 5, 1, 122, 0, 0, 0, true, {}, {} });
    b.damage(6354, "蜀犬吠日·四", { 5, 1, 687, 0, 0, 0, true, {}, {} });
    b.damage(25201, "祭湘君", { 1, 1, 589, 0, 0, 0, true, {}, {} });
    b.damage(26703, "灵隼击", { 1, 1, 333, 0, 0, 0, true, {}, {} });
    b.damage(13510, "时乘六龙", { 14, 1, 400, 0, 0, 0, true, {}, {} });
    b.damage(6582, "狂龙乱舞·一", { 3, 1, 40, 0, 0, 0, true, {}, {} });
    b.damage(13512, "狂龙乱舞·二", { 3, 1, 34, 0, 0, 0, true, {}, {} });
    b.damage(6584, "狂龙乱舞·三", { 3, 1, 40, 0, 0, 0, true, {}, {} });
    b.damage(6585, "狂龙乱舞·四", { 3, 1, 40, 0, 0, 0, true, {}, {} });
    b.damage(6366, "龙战于野·一", { 5, 1, 323, 0, 0, 0, true, {}, {} });
    b.damage(6367, "龙战于野·二", { 5, 1, 300, 0, 0, 0, true, {}, {} });
    b.damage(6368, "龙战于野·三", { 5, 1, 866, 0, 0, 0, true, {}, {} });
    b.secondary_damage(25779, "龙战于野·神兵", {0, 2, 60, 0, 0, 0, true});
    b.damage(28819, "苍龙", { 16, 32, 584, 0, 0, 0, true, {}, {} });
    b.damage(37397, "苍龙戾天", { 16, 1, 2205, 0, 0, 0, true, {}, {} });
    b.damage(32898, "触类·醉影", { 15, 1, 500, 0, 0, 0, true, {}, {} });
    b.damage(32898, "酩酊·醉影", { 20, 2, 600, 0, 0, 0, true, {}, {} });
    b.damage(32898, "落水·醉影", { 20, 2, 1000, 0, 0, 0, true, {}, {} });
    b.cast("落水打狗",
           480,
           0,
           24,
           { .forbidden = b.buff("苍龙腾空"), .choice = "落水打狗", .charges = 2 },
           { b.hit("落水打狗·命中"), b.apply("落水打狗", 5) });
    b.on_hit("灼烧(DOT)", { b.when("落水打狗", b.hit("落水·醉影")), b.remove("落水打狗") });
    b.cast("苍龙戾天", 480, 24, 0, { .choice = "苍龙戾天", .lock = 28 }, { b.apply("苍龙腾空"), b.later("苍龙戾天", 28) });
    b.on_hit("苍龙戾天", { b.apply("苍龙") });
    b.passive("苍龙", {}, { .critical = 5000 });
    const Step detonate{ Op::AdvanceAndRestartTicks, b.buff("灼烧"), 2 };
    b.cast("龙跃于渊",
           256,
           24,
           0,
           { .charges = 2, .check_gcd_group = 1, .shared_repayment = true, .minimum_mana_quarters = 120 },
           { mana(-120), b.apply("灼烧"), b.later("龙跃于渊·一", 1), b.later("龙跃于渊·二", 8), b.later("龙跃于渊·三", 14) });
    b.cast("时乘六龙",
           240,
           24,
           0,
           { .lock = 24, .gcd_group = 1, .minimum_mana_quarters = 80 },
           { mana(-80), b.apply("狂龙乱舞"), b.later("时乘六龙", 8) });
    b.cast("狂龙乱舞",
           24,
           24,
           0,
           { .required = b.buff("狂龙乱舞"), .minimum_mana_quarters = 40 },
           { mana(-40), detonate, b.later("狂龙乱舞·一", 13), b.later("狂龙乱舞·二", 16), b.later("狂龙乱舞·三", 19), b.later("狂龙乱舞·四", 22) });
    b.cast("龙战于野",
           160,
           24,
           0,
           { .check_gcd_group = 1, .minimum_mana_quarters = 80 },
           { mana(-80), b.selected(Legendary, b.proc("龙战于野·神兵", 512)),
             b.hit("龙战于野·一"), b.hit("龙战于野·二"), b.hit("龙战于野·三") });
    for (auto n : { "亢龙有悔", "龙跃于渊", "时乘六龙", "狂龙乱舞", "龙战于野" }) {
        b.after_cast(n, { b.when("苍龙", b.hit("苍龙")) });
    }
    b.cast("蜀犬吠日",
           480,
           24,
           0,
           { .check_gcd_group = 1 },
           { mana(240), b.selected("自强", mana(160)), b.hit("蜀犬吠日·一"), b.hit("蜀犬吠日·二"), b.hit("蜀犬吠日·三"), b.hit("蜀犬吠日·四") });
    b.cast("祭湘君",
           0,
           24,
           0,
           { .resource = 1, .cost = 1, .forbidden = b.buff("苍龙腾空"), .choice = "祭湘君", .lock = 24, .gcd_group = 1 },
           { b.remove("祭湘君", 3), b.resource_stacks("祭湘君", 1), b.apply("酩酊大醉"), b.gain(1, -3), b.remove("醉意已满") });
    b.cast("酒中仙",
           16,
           24,
           0,
           { .forbidden = b.buff("醉意已满"), .cooldown_aura = b.buff("酩酊大醉"), .aura_cooldown = 480 },
           { b.gain(1, 1), mana(80), b.apply("昊破"), b.apply("触类"), b.selected("酩酊", b.apply("触类")), b.at(1, 3, 3, b.apply("醉意已满")) });
    b.after_cast("亢龙有悔",
                 { b.when("触类", b.hit("触类·醉影")),
                   b.selected("酩酊", b.when("触类", mana(20))),
                   b.remove("触类"),
                   b.selected("亢龙有悔·含弘", b.apply("含弘")) });
    b.pulse("连击",
            {
                { .op = Op::SpendStackGroups, .slot = b.buff("连击"), .amount = 5, .extra = b.effect("灵隼击") }
    });
    auto apply_buffs = [&](std::string_view n) {
        b.passive(n, "越渊", { .pve = 175, .ignore = 512 });
        b.modifier(n,
                   {
                       b.buff("昊破"),
                       0,
                       { .attack = 154, .critical = 2000, .power = 205 }
        });
        b.modifier(n, { b.buff("含弘"), 0, { .damage = 113 } });
    };
    for (auto n :
         { "亢龙有悔·一", "亢龙有悔·二", "亢龙有悔·三", "亢龙有悔·四", "亢龙有悔·五", "亢龙有悔·六", "拨狗朝天·真实",
           "时乘六龙",    "横打双獒·一", "横打双獒·二", "狂龙乱舞·一", "狂龙乱舞·二", "狂龙乱舞·三", "狂龙乱舞·四",
           "蜀犬吠日·一", "蜀犬吠日·二", "蜀犬吠日·三", "蜀犬吠日·四", "追远",        "龙战于野·一", "龙战于野·二",
           "龙战于野·三", "龙跃于渊·一", "龙跃于渊·二", "龙跃于渊·三" })
    {
        apply_buffs(n);
        b.on_hit(n,
                 { b.when("祭湘君", b.apply("祭湘君连击")),
                   b.at_least("祭湘君连击", 5, b.hits_per_stack("祭湘君", "祭湘君")),
                   b.at_least("祭湘君连击", 5, b.remove("祭湘君连击", 5)),
                   b.selected("坚冰", b.apply("连击")) });
    }
    for (auto n : { "灵隼击", "灼烧(DOT)", "祭湘君", "苍龙", "苍龙戾天", "落水·醉影", "落水打狗·命中", "触类·醉影", "酩酊·醉影", "龙战于野·神兵" })
    {
        apply_buffs(n);
    }
    b.recipe("拨狗朝天", "拨狗朝天·回蓝3%", {}, {});
    b.recipe("亢龙有悔",
             "亢龙有悔·5%伤害",
             { .damage = 51 },
             { "亢龙有悔·一", "亢龙有悔·三", "亢龙有悔·二", "亢龙有悔·五", "亢龙有悔·六", "亢龙有悔·四" });
    b.recipe("亢龙有悔",
             "亢龙有悔·4%伤害",
             { .damage = 41 },
             { "亢龙有悔·一", "亢龙有悔·三", "亢龙有悔·二", "亢龙有悔·五", "亢龙有悔·六", "亢龙有悔·四" });
    b.recipe("亢龙有悔",
             "亢龙有悔·7%会心",
             { .critical = 700 },
             { "亢龙有悔·一", "亢龙有悔·三", "亢龙有悔·二", "亢龙有悔·五", "亢龙有悔·六", "亢龙有悔·四" });
    b.recipe("亢龙有悔", "亢龙有悔·含弘", {}, {});
    b.recipe("龙跃于渊", "龙跃于渊·无消耗内力", {}, {});
    b.recipe("蜀犬吠日", "蜀犬吠日·5%伤害", { .damage = 51 }, { "蜀犬吠日·一", "蜀犬吠日·三", "蜀犬吠日·二", "蜀犬吠日·四" });
    b.recipe("蜀犬吠日", "蜀犬吠日·5%会心", { .critical = 500 }, { "蜀犬吠日·一", "蜀犬吠日·三", "蜀犬吠日·二", "蜀犬吠日·四" });
    b.recipe("蜀犬吠日", "蜀犬吠日·4%伤害", { .damage = 41 }, { "蜀犬吠日·一", "蜀犬吠日·三", "蜀犬吠日·二", "蜀犬吠日·四" });
    b.recipe("蜀犬吠日", "蜀犬吠日·4%会心", { .critical = 400 }, { "蜀犬吠日·一", "蜀犬吠日·三", "蜀犬吠日·二", "蜀犬吠日·四" });
    b.recipe("狂龙乱舞", "狂龙乱舞·5%伤害", { .damage = 51 }, { "狂龙乱舞·一", "狂龙乱舞·三", "狂龙乱舞·二", "狂龙乱舞·四" });
    b.recipe("狂龙乱舞", "狂龙乱舞·4%伤害", { .damage = 41 }, { "狂龙乱舞·一", "狂龙乱舞·三", "狂龙乱舞·二", "狂龙乱舞·四" });
    b.recipe("狂龙乱舞", "狂龙乱舞·3%伤害", { .damage = 31 }, { "狂龙乱舞·一", "狂龙乱舞·三", "狂龙乱舞·二", "狂龙乱舞·四" });
    b.recipe("龙战于野", "龙战于野·5%伤害", { .damage = 51 }, { "龙战于野·一", "龙战于野·三", "龙战于野·二" });
    b.recipe("龙战于野", "龙战于野·3%会心", { .critical = 300 }, { "龙战于野·一", "龙战于野·三", "龙战于野·二" });
    b.recipe("龙战于野", "龙战于野·4%伤害", { .damage = 41 }, { "龙战于野·一", "龙战于野·三", "龙战于野·二" });
    b.recipe("龙战于野", "龙战于野·3%伤害", { .damage = 31 }, { "龙战于野·一", "龙战于野·三", "龙战于野·二" });
    for (auto n : { "蜀犬吠日·一", "蜀犬吠日·二", "蜀犬吠日·三", "蜀犬吠日·四" }) {
        b.passive(n, "自强", { .damage = 205 });
    }
    b.adjust([](Prepared &p) {
        if (std::ranges::contains(p.config.talents, "落水打狗"))
            p.enabled_actions[std::ranges::find(p.skills, "天下无狗", &SkillDescriptor::name) - p.skills.begin()] = false;

        const auto &book = *p.config.book;
        auto        aura = [&](std::string_view n) {
            return static_cast<Slot>(std::ranges::find(book.buffs, n, &BuffDescriptor::name) - book.buffs.begin());
        };
        auto skill = [&](std::string_view n) {
            return static_cast<Slot>(std::ranges::find(book.skills, n, &SkillDescriptor::name) - book.skills.begin());
        };
        const bool reduced   = std::ranges::contains(p.config.talents, std::string_view{ "越渊" });
        const bool free_leap = std::ranges::contains(p.config.recipes, std::string_view{ "龙跃于渊·无消耗内力" });
        for (auto n : { "龙跃于渊", "亢龙有悔", "狂龙乱舞", "龙战于野", "时乘六龙" }) {
            const auto slot = skill(n);
            auto      &cost = p.mana_cost_quarters[slot];
            if (reduced) {
                cost = cost * 3 / 4;
            }
            if (n == std::string_view{ "龙跃于渊" } && free_leap) {
                cost = 0;
            }
            // 亢龙 checks the threshold then drains the entire bar.
            if (n != std::string_view{ "亢龙有悔" }) {
                p.impacts[slot].front().amount = -cost;
            }
        }
        const bool tipsy = std::ranges::contains(p.config.talents, std::string_view{ "酩酊" });
        if (tipsy) {
            for (auto &step : p.finishes[skill("亢龙有悔")]) {
                if (step.op == Op::Hit && step.slot == skill("触类·醉影")) {
                    step.slot = skill("酩酊·醉影");
                }
            }
        }
        if (p.config.precombat) {
            p.resources[1].initial = 3;
            p.opening.push_back({ Op::Add, aura("醉意已满") });
            p.opening.push_back({ .op = Op::Add, .slot = aura("昊破"), .duration = 560 });
            p.opening.push_back({ .op = Op::Add, .slot = aura("触类"), .amount = tipsy ? 2 : 1, .duration = 240 });
        }
    });
    // The chained palms share the remaining 蛟龙 cooldown; a follow-up consumes the previous opportunity.
    for (auto name : { "双龙取水", "龙游天地", "龙腾五岳" }) b.state({0, name, 560, 0, 1}, false);
    b.damage(13520, "恶狗拦路", {11,1,140,0,0,0,true});
    b.cast("恶狗拦路", 320, 24, 0, { .lock = 16, .range = 8 }, { b.later("恶狗拦路", 15) });
    b.damage(5528, "龙啸九天", {84,8,480,0,0,0,true});
    b.cast("龙啸九天", 640, 24, 0, { .lock = 33, .range = 6 }, { b.hit("龙啸九天") });
    b.cast("笑醉狂", 640, 0, 0, { .pulses = 3, .spacing = 16, .fixed_cast_time = true },
           { b.selected("笑醉狂·内力回复", mana(20)) });
    b.damage(6358, "蛟龙翻江·一", {10,0,469,0,0,0,true});
    b.damage(6359, "蛟龙翻江·二", {8,0,385,0,0,0,true});
    b.damage(13526, "蛟龙翻江·三", {51,4,228,0,0,0,true});
    b.damage(13527, "双龙取水", {15,1,1240,0,0,0,true});
    b.damage(6362, "龙游天地·一", {21,0,413,0,0,0,true});
    b.damage(6363, "龙游天地·二", {21,0,422,0,0,0,true});
    b.damage(13528, "龙游天地·三", {129,4,601,0,0,0,true});
    b.damage(13529, "龙腾五岳", {25,2,1784,0,0,0,true});
    b.cast("蛟龙翻江", 560, 24, 0, { .lock = 33, .range = 12, .minimum_mana_quarters = 60 },
           { mana(-60), b.remove("龙游天地"), b.remove("龙腾五岳"), b.apply("双龙取水"), b.hit("蛟龙翻江·一"), b.hit("蛟龙翻江·二"), b.hit("蛟龙翻江·三") });
    b.cast("双龙取水", 0, 24, 0, { .required = b.buff("双龙取水"), .lock = 23, .range = 12, .minimum_mana_quarters = 60 },
           { mana(-60), b.convert("双龙取水", "龙游天地"), b.hit("双龙取水") });
    b.cast("龙游天地", 0, 24, 0, { .required = b.buff("龙游天地"), .lock = 38, .range = 12, .minimum_mana_quarters = 60 },
           { mana(-60), b.convert("龙游天地", "龙腾五岳"), b.hit("龙游天地·一"), b.hit("龙游天地·二"), b.hit("龙游天地·三") });
    b.cast("龙腾五岳", 0, 24, 0, { .required = b.buff("龙腾五岳"), .lock = 46, .range = 12, .minimum_mana_quarters = 60 },
           { mana(-60), b.remove("龙腾五岳"), b.hit("龙腾五岳"), b.apply("灼烧") });
    for (auto name : { "蛟龙翻江", "双龙取水", "龙游天地", "龙腾五岳", "龙啸九天" }) b.after_cast(name, { b.when("苍龙", b.hit("苍龙")) });
    for (auto hit : { "恶狗拦路", "蛟龙翻江·一", "蛟龙翻江·二", "蛟龙翻江·三", "双龙取水", "龙游天地·一", "龙游天地·二", "龙游天地·三", "龙腾五岳", "龙啸九天" }) apply_buffs(hit);
    b.recipe("恶狗拦路", "恶狗拦路·5%伤害", { .damage = 51 }, { "恶狗拦路" });
    b.recipe("恶狗拦路", "恶狗拦路·4%伤害", { .damage = 41 }, { "恶狗拦路" });
    b.recipe("恶狗拦路", "恶狗拦路·3%伤害", { .damage = 31 }, { "恶狗拦路" });
    // A macro tap releases the first charge tier. 落水打狗 replaces the base button.
    b.damage(13503, "天下无狗·起手", {7,1,27,0,0,0,true});
    b.damage(6382, "天下无狗·收招", {7,1,27,0,0,0,true});
    b.cast("天下无狗", 128, 24, 0, { .range = 6 }, { b.hit("天下无狗·起手"), b.hit("天下无狗·收招") });
    for (auto n : { "天下无狗·起手", "天下无狗·收招" }) apply_buffs(n);
    b.recipe("天下无狗", "天下无狗·5%伤害", { .damage = 51 }, { "天下无狗·起手", "天下无狗·收招" });
    b.recipe("天下无狗", "天下无狗·4%伤害", { .damage = 41 }, { "天下无狗·起手", "天下无狗·收招" });
    b.recipe("天下无狗", "天下无狗·3%伤害", { .damage = 31 }, { "天下无狗·起手", "天下无狗·收招" });
    b.recipe("龙腾五岳", "龙腾五岳·5%会心", { .critical = 500 }, { "龙腾五岳" });
    b.recipe("龙腾五岳", "龙腾五岳·4%伤害", { .damage = 41 }, { "龙腾五岳" });
    b.recipe("龙腾五岳", "龙腾五岳·4%会心", { .critical = 400 }, { "龙腾五岳" });
    b.recipe("龙腾五岳", "龙腾五岳·3%伤害", { .damage = 31 }, { "龙腾五岳" });
    b.recipe("笑醉狂", "笑醉狂·内力回复", {}, {});
    b.recipe("龙啸九天", "龙啸九天·3%伤害", { .damage = 31 }, { "龙啸九天" });
    b.recipe("龙啸九天", "龙啸九天·4%伤害", { .damage = 41 }, { "龙啸九天" });
    b.recipe("龙啸九天", "龙啸九天·5%伤害", { .damage = 51 }, { "龙啸九天" });
    b.secondary_damage(32898, "醉影·一", {15, 1, 500, 0, 0, 0, true});
    apply_buffs("醉影·一");
    b.cast("使用橙武", 800, 0, 0, {.choice = Legendary}, {b.apply("青龙映夜")});
    b.no_cooldown_during("亢龙有悔", "青龙映夜");
    b.on_cast("亢龙有悔", {b.remove("神兵亢龙"), b.when("青龙映夜", b.apply("神兵亢龙"))});
    for (auto hit : palms) b.modifier(hit, {b.buff("神兵亢龙"), 0, {.damage = 451}});
    b.after_cast("亢龙有悔", {b.when("青龙映夜", b.hit("醉影·一"))});
    b.legendary_damage({ "拨狗朝天·真实", "亢龙有悔·一", "亢龙有悔·二", "亢龙有悔·三", "亢龙有悔·四", "亢龙有悔·五", "亢龙有悔·六" });
    return b.finish();
}
