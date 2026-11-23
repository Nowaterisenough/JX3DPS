#include "common/builder.hpp"

namespace JX3DPS::runtime::season50 {

// 为目标依次填左、右弱点。两个位置都有弱点时保留原有的两个计时。
static void 添加弱点(EffectContext &frame, const Step &step)
{
    if (!frame.Stacks(step.slot)) {
        frame.SetAura(step.slot, 1);
    } else if (!frame.Stacks(step.extra)) {
        frame.SetAura(step.extra, 1);
    }
}

// 两侧都能击破时先消耗较早到期的一侧；到期帧相同则取左侧。移除弱点之后再记一次弱点击破伤害。
static void 击破弱点(EffectContext &frame, const Step &step)
{
    const Slot left = step.extra, right = step.neutral_buff;
    if (!frame.Stacks(left) && !frame.Stacks(right)) {
        return;
    }
    const Slot oldest =
        !frame.Stacks(right) || (frame.Stacks(left) && frame.state.target_buff_expires_at[left] <= frame.state.target_buff_expires_at[right])
            ? left
            : right;
    frame.SetAura(oldest, -1);
    frame.Hit(step.slot);
}

Book 孤锋诀()
{
    Builder b("gfj", "孤锋诀", R"(# 魔盒宏适配 https://www.jx3box.com/macro/109838
/cast 使用风腰坠
/cast 使用橙武
/cast [nobuff:双刀&last_skill:行云势] 灭影追风
/cast [锐意>=100] 断云势
/cast [buff:双刀&洄涛<2&buff:倾怒] 裂海横天
/cast [buff:双刀&洄涛>=2&nobuff:倾怒] 裂海横天
/cast [nobuff:双刀&buff:倾怒] 裂海横天
/cast [buff:截流] 沧浪三叠
/cast [buff:双刀&nobuff:截流] 横云断浪
/cast 孤锋破浪
/cast [buff:潋停|buff:潋行] 行云势
/cast 停云势
/cast 留客雨
/cast [nobuff:潋行] 决云势
/cast 行云势
)");
    auto &school = b.definition();
    school.info = { school.code, "刀宗", "力道", "Physics", 604, 164, 0 };
    school.display_order = 8;
    school.saved.macro_digest = "b8994be3458ba1faeea52f0f1e3b61e2e9b0b11836723031a8769130203257c3";
    school.saved.talents_v7 = { "流溯", "滔天", "裂海横天", "洄涛", "择行", "涣衍", "流岚", "潋风" };
    school.saved.talents_v6 = { "流溯", "滔天", "裂海横天" };
    school.saved.added_recipes_v7 = { "停云势·4%会心" };
    school.saved.removed_recipes_v7 = { "行云势·3%伤害" };
    school.precombat = true;
    school.weapon_effect = "橙武孤锋";
    school.raid.formation = "刀宗阵";
    school.raid.own_formation = "刀宗阵";
    school.raid.penetration_perfume = true;
    b.preset(2491,
             { 6583, 1469, 170, 1667, 225, 1687, 4246, 0, 1013 },
             { "行云势·距离提高2尺", "行云势·4%伤害", "行云势·5%伤害", "行云势·4%会心",
               "停云势·5%伤害", "停云势·4%伤害", "停云势·距离提高2尺", "停云势·4%会心",
               "决云势·3%伤害", "决云势·4%伤害", "决云势·3%会心", "决云势·2%会心",
               "断云势·4%伤害", "断云势·5%伤害", "断云势·4%会心", "断云势·3%会心",
               "孤锋破浪·4%伤害", "孤锋破浪·5%伤害", "孤锋破浪·3%会心", "孤锋破浪·4%会心",
               "留客雨·距离提高2尺", "留客雨·4%伤害", "留客雨·5%伤害", "留客雨·4%会心",
               "沧浪三叠·3%伤害", "沧浪三叠·4%伤害", "沧浪三叠·5%伤害", "沧浪三叠·4%会心",
               "横云断浪·4%伤害", "横云断浪·5%伤害", "横云断浪·3%会心", "横云断浪·4%会心",
               "触石雨·4%伤害", "触石雨·3%伤害" });
    b.choices({ "威声", "流溯", "滔天", "裂海横天", "洄涛", "择行", "涣衍", "流岚", "潋风", "强膂", "驭耀" }, {}, { "橙武孤锋" });
    b.default_talents({ "裂海横天", "流溯", "涣衍", "择行", "潋风", "洄涛", "流岚", "强膂", "滔天" });
    b.resource({ "锐意", 0, 0, 100 });
    b.state({ 0, "橙武", 96, 0, 1 }, false, false, false);
    b.state({ 0, "橙武孤锋", 672, 0, 8 }, false, false, false);
    b.state({ 0, "双刀", 0, 0, 1 }, false, false, false);
    b.state({ 0, "截流", 0, 0, 3 }, false);
    b.state({ 24219, "长溯", 4800, 0, 9 }, true);
    b.state({ 0, "灭影追风", 192, 0, 1 }, false, false, false);
    // 灭影追风同时开两个威声窗口：32 帧内影响双刀与界破，另有 192 帧的两层会心，用于后续孤锋破浪。
    b.state({ 0, "威声", 32, 0, 1 }, false, false, false);
    b.state({ 0, "威声会心", 192, 0, 2 }, false, false, false);
    b.damage(32149,
             "行云势·一",
             { 90, 5, 473, 1024, 0, 0, true, { .pve = 61 }, {} });
    b.damage(32150,
             "行云势·二",
             { 107, 6, 557, 1536, 0, 0, true, { .pve = 61 }, {} });
    b.damage(32151,
             "行云势·三",
             { 135, 7, 899, 2048, 0, 0, true, { .pve = 61 }, {} });
    b.damage(32154,
             "决云势",
             { 93, 6, 225, 1024, 0, 0, true, { .pve = 61 }, {} });
    b.damage(32153,
             "停云势",
             { 40, 1, 388, 2048, 0, 0, true, { .pve = 61 }, {} });
    b.damage(32167,
             "断云势",
             { 16, 1, 471, 2048, 0, 0, true, { .pve = 61 }, {} });
    b.damage(32348,
             "断云势·额外伤害",
             { 6, 0, 164, 2048, 0, 0, true, { .pve = 61 }, {} });
    b.damage(32235,
             "孤锋破浪",
             {
                 725,
                 25,
                 945,
                 3072,
                 0,
                 0,
                 true,
                 { .pve = 61 },
                 { { b.buff("橙武孤锋"), 0, { .damage = 1331 }, "" },
                   { b.buff("威声会心"), 0, { .critical = 1500 }, "威声" } }
    });
    b.damage(34695,
             "界破",
             { 16, 31, 535, 0, 0, 0, true, { .pve = 61 }, {} });
    b.cast("使用橙武", 800, 0, 0, { .choice = "橙武孤锋" }, { b.gain(0, 100), b.apply("双刀"), b.apply("橙武"), b.apply("橙武孤锋", 8) });
    b.cast("行云势", 0, 24, 0, { .forbidden = b.buff("双刀"), .chain_window = 160 }, { b.cycle("行云势·一", 3), b.gain(0, 5) });
    b.cast("决云势", 320, 24, 0, { .forbidden = b.buff("双刀"), .charges = 3 }, { b.hit("决云势", 1), b.gain(0, 25) });
    b.cast("停云势", 192, 24, 0, { .forbidden = b.buff("双刀") }, { b.hit("停云势", 1), b.gain(0, 10) });
    b.cast("断云势",
           0,
           24,
           0,
           { .resource = 0, .cost = 100, .forbidden = b.buff("双刀"), .free_cost = b.buff("橙武") },
           { b.hit("断云势", 1), b.life(7000, 10000, b.hit("断云势·额外伤害", 1)), b.when("橙武", b.gain(0, -100), true), b.apply("双刀", 1) });
    b.cast("灭影追风",
           320,
           0,
           0,
           {},
           { b.apply("灭影追风", 1), b.apply("双刀", 1), b.selected("威声", b.apply("威声", 1)), b.selected("威声", b.apply("威声会心", 2)) });
    // 孤锋破浪先打本体，再按目标已有长溯层数逐层追击，打完清长溯；威声会心在这次施放收尾才扣一层。
    b.cast("孤锋破浪",
           0,
           24,
           0,
           { .required = b.buff("双刀"), .gcd_group = 3 },
           { b.hit("孤锋破浪", 1),
             b.hits_per_stack("孤锋破浪", "长溯"),
             b.remove("长溯", 9),
             b.when("威声", b.later("界破", 32), false),
             b.when("威声", b.remove("双刀", 1000), true),
             b.remove("威声", 1000),
             b.remove("威声会心", 1) });
    b.recipe("停云势", "停云势·3%会心", { .critical = 300 }, { "停云势" });
    b.recipe("停云势", "停云势·4%会心", { .critical = 400 }, { "停云势" });
    b.recipe("停云势", "停云势·4%伤害", { .damage = 41 }, { "停云势" });
    b.recipe("停云势", "停云势·5%伤害", { .damage = 51 }, { "停云势" });
    b.recipe("决云势", "决云势·2%会心", { .critical = 200 }, { "决云势" });
    b.recipe("决云势", "决云势·3%会心", { .critical = 300 }, { "决云势" });
    b.recipe("决云势", "决云势·3%伤害", { .damage = 31 }, { "决云势" });
    b.recipe("决云势", "决云势·4%伤害", { .damage = 41 }, { "决云势" });
    b.recipe("孤锋破浪", "孤锋破浪·3%会心", { .critical = 300 }, { "孤锋破浪" });
    b.recipe("孤锋破浪", "孤锋破浪·4%会心", { .critical = 400 }, { "孤锋破浪" });
    b.recipe("孤锋破浪", "孤锋破浪·4%伤害", { .damage = 41 }, { "孤锋破浪" });
    b.recipe("孤锋破浪", "孤锋破浪·5%伤害", { .damage = 51 }, { "孤锋破浪" });
    b.recipe("断云势", "断云势·4%会心", { .critical = 400 }, { "断云势", "断云势·额外伤害" });
    b.recipe("断云势", "断云势·4%伤害", { .damage = 41 }, { "断云势", "断云势·额外伤害" });
    b.recipe("断云势", "断云势·5%伤害", { .damage = 51 }, { "断云势", "断云势·额外伤害" });
    b.recipe("行云势", "行云势·2%会心", { .critical = 200 }, { "行云势·一", "行云势·三", "行云势·二" });
    b.recipe("行云势", "行云势·3%会心", { .critical = 300 }, { "行云势·一", "行云势·三", "行云势·二" });
    b.recipe("行云势", "行云势·3%伤害", { .damage = 31 }, { "行云势·一", "行云势·三", "行云势·二" });
    b.recipe("行云势", "行云势·4%会心", { .critical = 400 }, { "行云势·一", "行云势·三", "行云势·二" });
    b.recipe("行云势", "行云势·4%伤害", { .damage = 41 }, { "行云势·一", "行云势·三", "行云势·二" });
    b.recipe("行云势", "行云势·5%伤害", { .damage = 51 }, { "行云势·一", "行云势·三", "行云势·二" });
    b.damage(32602, "沧浪三叠·一", { 20, 1, 317, 2048, 0, 0, true, { .pve = 61 }, {} });
    b.damage(32603, "沧浪三叠·二", { 26, 2, 351, 2560, 0, 0, true, { .pve = 61 }, {} });
    b.damage(32604, "沧浪三叠·三", { 28, 2, 400, 3072, 0, 0, true, { .pve = 61 }, {} });
    b.damage(44538, "滔天", { 1, 1, 640, 1024, 0, 0, true, { .pve = 61 }, {} });
    b.base_action("断云势").post_cast = { b.selected("流溯", b.apply("截流", 3)) };
    b.base_action("灭影追风").pre_cast = { b.selected("流溯", b.when("双刀", b.apply("截流", 3), true)) };
    b.state({ 0, "沧浪连段", 0, 0, 1 }, false);
    b.cast("沧浪三叠",
           0,
           24,
           0,
           { .required = b.buff("双刀"), .chain_window = 48, .gcd_group = 3 },
           { b.when("沧浪连段", b.cycle("沧浪三叠·一", 3), true),
             b.when("沧浪连段", b.hit("沧浪三叠·一")),
             b.when("沧浪连段", b.hit("沧浪三叠·二")),
             b.when("沧浪连段", b.hit("沧浪三叠·三")),
             b.remove("截流"),
             b.remove("沧浪连段") });
    // 橙武孤锋按实际命中逐击扣层，长溯追加的孤锋也消耗；同时选择裂海横天与滔天时每击可再触发滔天。
    b.post_damage("孤锋破浪", { b.remove("橙武孤锋"), b.selected("裂海横天", b.selected("滔天", b.hit("滔天"))) });
    b.set_bonus({ .damage = 102 }, { "孤锋破浪" });
    b.resource({ "洄涛", 0, 0, 24 });
    b.state({ 0, "寻隙检查", 0, 160, 1 }, false, true);
    b.state({ 0, "弱点·左", 320, 0, 1 }, true);
    b.state({ 0, "弱点·右", 320, 0, 1 }, true);
    b.state({ 0, "流岚", 960, 0, 1 }, false);
    b.state({ 0, "倾怒", 0, 0, 1 }, false);
    b.state({ 0, "潋停", 640, 0, 3 }, false);
    b.state({ 0, "潋行", 640, 0, 1 }, false);
    b.damage(0, "弱点击破", { 3, 1, 504, 1024, 0, 0, true, { .pve = 61 }, {} });
    b.damage(32246, "留客雨", { 16, 1, 200, 1024, 0, 0, true, { .pve = 61 }, {} });
    b.damage(44995, "潋风", { 1, 1, 400, 0, 0, 0, true, { .pve = 61 }, {} });
    b.initial("寻隙检查");
    b.tick_schedule("寻隙检查", 8, 5);
    b.pulse("寻隙检查",
            {
                { .op = Op::School, .slot = b.buff("弱点·左"), .extra = b.buff("弱点·右"), .effect = 添加弱点 }
    });
    Step check{ .op = Op::School, .slot = b.effect("弱点击破"), .extra = b.buff("弱点·左"), .effect = 击破弱点 };
    check.neutral_buff = b.buff("弱点·右");
    for (auto name : { "行云势", "断云势", "决云势", "停云势", "孤锋破浪", "沧浪三叠" }) {
        b.post_cast(name, { b.at(1, 0, 21, check) });
    }
    b.post_damage("弱点击破",
             { b.selected("裂海横天", b.gain(1, 1)),
               b.selected("涣衍", b.selected("裂海横天", b.gain(1, 1))),
               b.selected("流岚", b.apply("流岚")) });
    b.cast("裂海横天",
           160,
           8,
           0,
           {
               .choice    = "裂海横天",
               .lock      = 8,
               .charges   = 3,
               .gcd_group = 2
    },
           { b.selected("洄涛", b.when("倾怒", b.gain(1, 3), true)), { Op::ToggleAura, b.buff("倾怒") } });
    b.pre_cast("沧浪三叠", { b.when("倾怒", b.at(1, 2, 24, b.apply("沧浪连段"))), b.when("沧浪连段", b.gain(1, -2)) });
    for (auto hit : { "沧浪三叠·一", "沧浪三叠·二", "沧浪三叠·三" }) {
        b.post_damage(hit, { b.when("截流", b.apply("长溯")) });
    }
    b.cast("留客雨", 96, 24, 0, { .forbidden = b.buff("双刀"), .gcd_group = 1 }, { b.hit("留客雨") });
    b.post_cast("留客雨", { { Op::ShortenGcd, 0, 16 } });
    b.variant("停云势",
              "潋风",
              256,
              0,
              { .forbidden = b.buff("双刀"), .charges = 2 },
              { b.hit("停云势"), b.gain(0, 10), b.remove("潋行"), b.apply("潋停", 3) });
    b.post_cast("停云势", { b.selected("择行", b.gain(0, 20)) });
    b.variant("决云势", "择行", 320, 0, { .forbidden = b.buff("双刀"), .charges = 3 }, { b.hit("决云势") });
    b.post_cast("行云势",
                 { b.selected("择行", b.gain(0, 10)),
                   b.when("潋停", b.gain(0, 5)),
                   b.selected("择行", b.when("潋停", b.gain(0, 10))),
                   b.when("潋行", b.hit("潋风")),
                   b.stacks("潋停", 1, b.apply("潋行")),
                   b.remove("潋停") });
    for (auto hit : { "行云势·一", "行云势·二", "行云势·三" }) {
        b.pre_damage(hit, { b.buff("潋行"), 0, { .damage = 307 }, "潋风" });
    }
    b.secondary_damage(33239, "行云势·神兵", {20, 2, 60, 0, 0, 0, true, {.pve = 61}});
    for (auto hit :
         { "行云势·一", "行云势·二", "行云势·三", "停云势", "断云势", "断云势·额外伤害", "决云势", "留客雨", "孤锋破浪", "滔天", "潋风", "弱点击破", "沧浪三叠·一", "沧浪三叠·二", "沧浪三叠·三", "行云势·神兵" })
    {
        b.pre_damage(hit, { b.buff("流岚"), 0, { .ignore = 512 }, "流岚" });
    }
    b.adjust([](Prepared &p) {
        for (const auto &[skill, extra] : { std::pair{"行云势", "行云势·距离提高2尺"}, {"停云势", "停云势·距离提高2尺"}, {"留客雨", "留客雨·距离提高2尺"} }) {
            auto slot = std::ranges::find(p.skills, skill, &SkillDescriptor::name) - p.skills.begin();
            p.ranges[slot] = std::ranges::contains(p.config.recipes, extra) ? 6 : 4;
        }
        if (p.config.precombat) {
            const auto &buffs = p.config.book->buffs;
            auto aura = [&](std::string_view n) { return static_cast<Slot>(std::ranges::find(buffs, n, &BuffDescriptor::name) - buffs.begin()); };
            p.opening.push_back({ Op::Add, aura("双刀") });
            p.opening.push_back({ Op::Add, aura("截流"), 3 });
        }
        if (std::ranges::contains(p.config.talents, std::string_view{ "择行" })) {
            p.resources[0].high = 300;
        }
        if (!std::ranges::contains(p.config.talents, std::string_view{ "洄涛" })) {
            p.resources[1].high = 4;
        }
    });
    b.damage(32234, "横云断浪", { 72, 8, 432, 3072, 0, 0, true, { .pve = 61 } });
    b.cast("横云断浪", 240, 24, 0, { .required = b.buff("双刀"), .gcd_group = 3 }, { b.hit("横云断浪") });
    b.damage(32974, "云刀", { 0, 0, 16, 1024, 0, 0, true, { .pve = 61 } });
    b.state({ 0, "云刀计时", 0, 24, 1 }, false, true, true);
    b.pulse("云刀计时", { b.hit("云刀") });
    for (auto cast : { "行云势", "沧浪三叠", "决云势", "停云势", "孤锋破浪", "横云断浪", "留客雨" })
        b.pre_cast(cast, { b.when("云刀计时", b.hit("云刀"), true), b.when("云刀计时", b.apply("云刀计时"), true) });
    b.school_modifier({ b.buff("灭影追风"), 0, { .critical = 1000, .power = 100, .overcome = 102 } });
    b.school_modifier({ INVALID_SLOT, 0, { .strain_rating = 205 }, "强膂" });
    b.timing_recipe("停云势", "停云势·调息减16帧", "停云势", -16, 0);
    b.timing_recipe("停云势", "停云势·调息减32帧", "停云势", -32, 0);
    b.recipe("留客雨", "留客雨·3%伤害", { .damage = 31 }, { "留客雨" });
    b.recipe("留客雨", "留客雨·4%伤害", { .damage = 41 }, { "留客雨" });
    b.recipe("留客雨", "留客雨·5%伤害", { .damage = 51 }, { "留客雨" });
    b.recipe("留客雨", "留客雨·4%会心", { .critical = 400 }, { "留客雨" });
    b.recipe("断云势", "断云势·3%会心", { .critical = 300 }, { "断云势", "断云势·额外伤害" });
    b.recipe("沧浪三叠", "沧浪三叠·3%伤害", { .damage = 31 }, { "沧浪三叠·一", "沧浪三叠·二", "沧浪三叠·三" });
    b.recipe("沧浪三叠", "沧浪三叠·4%伤害", { .damage = 41 }, { "沧浪三叠·一", "沧浪三叠·二", "沧浪三叠·三" });
    b.recipe("沧浪三叠", "沧浪三叠·5%伤害", { .damage = 51 }, { "沧浪三叠·一", "沧浪三叠·二", "沧浪三叠·三" });
    b.recipe("沧浪三叠", "沧浪三叠·4%会心", { .critical = 400 }, { "沧浪三叠·一", "沧浪三叠·二", "沧浪三叠·三" });
    b.recipe("横云断浪", "横云断浪·4%伤害", { .damage = 41 }, { "横云断浪" });
    b.recipe("横云断浪", "横云断浪·5%伤害", { .damage = 51 }, { "横云断浪" });
    b.recipe("横云断浪", "横云断浪·3%会心", { .critical = 300 }, { "横云断浪" });
    b.recipe("横云断浪", "横云断浪·4%会心", { .critical = 400 }, { "横云断浪" });
    b.damage(32766, "触石雨", {18, 1, 200, 0, 0, 0, true, { .pve = 61 } });
    b.cast("触石雨", 560, 24, 0, { .range = 15 }, { b.hit("触石雨") });
    b.recipe("触石雨", "触石雨·4%伤害", { .damage = 41 }, { "触石雨" });
    b.recipe("触石雨", "触石雨·3%伤害", { .damage = 31 }, { "触石雨" });
    b.recipe("行云势", "行云势·距离提高2尺", {}, {});
    b.recipe("留客雨", "留客雨·距离提高2尺", {}, {});
    b.recipe("停云势", "停云势·距离提高2尺", {}, {});
    b.post_cast("行云势", {b.selected(Legendary, b.proc("行云势·神兵", 256))});
    b.legendary_damage({ "沧浪三叠·一", "沧浪三叠·二", "沧浪三叠·三", "孤锋破浪" });
    // 驭耀在行云势收招额外回复 6 点锐意。化劲只影响侠士目标。
    b.post_cast("行云势", {b.selected("驭耀", b.gain(0, 6))});
    return b.finish();
}
} // namespace JX3DPS::runtime::season50
