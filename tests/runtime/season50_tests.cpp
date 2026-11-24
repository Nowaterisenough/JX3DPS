#include "src/class/common/runtime_rules.hpp"

#include "src/core/runtime/batch_statistics.hpp"
#include "src/core/runtime/attribute_gains.hpp"
#include "check.hpp"
#include <iostream>
#include <fstream>
#include <sstream>

#define JX3DPS LegacyReference
#include "legacy_reference/Damage.hpp"
#undef JX3DPS

using namespace JX3DPS;
namespace rt  = runtime;
namespace s50 = runtime::season50;

namespace {
struct Fight
{
    rt::CompiledMacro                               macro;
    s50::Config                                 cfg;
    std::unique_ptr<rt::Simulation<s50::Rules>> sim;

    Fight(std::string_view code, std::string_view source = {}, int crit = 0, int haste = 0)
    {
        cfg.book = s50::Find(code);
        CHECK(cfg.book != nullptr);
        cfg.talents.assign(cfg.book->default_talents.begin(), cfg.book->default_talents.end());
        cfg.attributes.crit  = crit;
        cfg.attributes.haste = haste;
        auto [m, errors]     = rt::MacroCompiler::Compile(source.empty() ? cfg.book->default_macro : source,
                                                          rt::CatalogMacroOptions(cfg.book->Catalog()));
        CHECK(errors.empty());
        macro = std::move(m);
        prepare();
    }

    void prepare()
    {
        sim = std::make_unique<rt::Simulation<s50::Rules>>(macro.program, rt::LoadSpecialization(cfg), 4096, 8192);
    }

    void run(int frames, std::uint64_t seed = 42)
    {
        sim->Start(frames, seed);
        sim->Run();
    }

    std::vector<rt::DamageIntent> hits(std::string_view name) const
    {
        std::vector<rt::DamageIntent> out;
        for (const auto &h : sim->Log().Intents()) {
            if (cfg.book->skills[h.skill].name == name) {
                out.push_back(h);
            }
        }
        return out;
    }

    std::vector<tick_t> frames(std::string_view name) const
    {
        std::vector<tick_t> out;
        for (const auto &h : hits(name)) {
            out.push_back(h.frame);
        }
        return out;
    }
};

void CheckAllBooks()
{
    for (const auto code : {"tx", "mw", "axzy", "baj", "bxj", "dj", "fsj", "fysj", "gfj", "hjy",
                            "lhj", "shxj", "txj", "tlgd", "wf", "xcj", "yjj", "yly", "ztg", "zxg"})
        CHECK(s50::Find(code) != nullptr);
    for (const auto &b : s50::Books()) {
        Fight f(b.code);
        f.run(960);
        CHECK(f.sim->TotalDamage() > 0);
        auto expected = f.sim->Log().Intents();
        auto state    = f.sim->Log().Mutations();
        f.run(960, 99);
        f.run(960);
        CHECK(expected == f.sim->Log().Intents());
        CHECK(state == f.sim->Log().Mutations());
        rt::Simulation<s50::Rules, true> debug(f.macro.program, rt::LoadSpecialization(f.cfg), 4096, 8192);
        debug.Start(960, 42);
        while (!debug.Finished()) {
            debug.Continue();
        }
        CHECK(expected == debug.Log().Intents());
        CHECK(state == debug.Log().Mutations());
        CHECK(debug.TotalDamage() == f.sim->TotalDamage());
        for (const auto &hit : expected) {
            CHECK(hit.frame >= 0 && hit.frame < 960);
        }
    }
}

void CheckDotAndCastOrder()
{
    Fight f("hjy", "/cast [tnobuff:商阳指] 商阳指\n/cast 阳明指");
    f.run(289);
    CHECK((f.frames("商阳指(DOT)") == std::vector<tick_t>{ 48, 96, 144, 192, 240, 288 }));
    CHECK(f.frames("阳明指").front() == 52);
    Fight burst("hjy", "/cast [tnobuff:商阳指] 商阳指\n/cast 玉石俱焚");
    burst.run(49);
    const auto ticks = burst.hits("商阳指(DOT)");
    CHECK(ticks.size() == 1);
    CHECK(ticks[0].frame == 24 && ticks[0].sub == 6);
    Fight clipped("hjy", "/cast 阳明指");
    clipped.run(28);
    CHECK(clipped.sim->Log().Size() == 0);
}

void CheckDamageHooksAndDotSnapshot()
{
    Fight fight("hjy", "/scast 商阳指");
    auto book = *fight.cfg.book;
    const auto options = rt::CatalogMacroOptions(book.Catalog());
    const auto cast = options.resolve_skill("商阳指");
    const auto aura = options.resolve_buff("流离");
    const auto dot = book.auras[options.resolve_buff("商阳指")].damage;
    // A one-cast bonus is captured when the DOT is applied, then removed at cast completion.
    book.actions[cast].pre_cast.push_back({s50::Op::Add, aura});
    book.actions[cast].post_cast.push_back({s50::Op::Remove, aura});
    book.hits[dot].modifiers.push_back({.aura = aura, .stats = {.attack_flat = 256}});
    fight.cfg.book = &book;
    fight.prepare();
    fight.run(100);
    const auto &versions = fight.sim->GetRules().Versions();
    auto ticks = fight.hits("商阳指(DOT)");
    CHECK(ticks.size() == 2);
    for (const auto &tick : ticks) {
        CHECK(versions[tick.snapshot_version].attack_flat == 256);
        CHECK(versions[tick.live_version].attack_flat == 0);
    }
    // Each tick can change live state before damage and undo it after the hit.
    book.hits[dot].pre_damage.push_back({s50::Op::Add, aura});
    book.hits[dot].post_damage.push_back({s50::Op::Remove, aura});
    fight.prepare();
    fight.run(100);
    ticks = fight.hits("商阳指(DOT)");
    CHECK(ticks.size() == 2);
    const auto &updated = fight.sim->GetRules().Versions();
    for (const auto &tick : ticks) {
        CHECK(updated[tick.snapshot_version].attack_flat == 256);
        CHECK(updated[tick.live_version].attack_flat == 256);
    }
    CHECK(fight.sim->GetState().self_buff_stacks[aura] == 0);
}

void CheckTaiXuTalentWindows()
{
    Fight blade("tx", "/cast 八荒归元");
    for (bool winds : {false, true}) {
        for (bool charges : {false, true}) {
            blade.cfg.talents.clear();
            if (winds) blade.cfg.talents.emplace_back("风逝");
            if (charges) blade.cfg.talents.emplace_back("和光");
            blade.prepare();
            blade.run(4800);
            CHECK(blade.hits(winds ? "八荒归元·斩杀" : "八荒归元").size() == (charges ? 35 : 25));
        }
    }
    Fight ignore("tx", "/scast 碎星辰\n/scast 人剑合一");
    ignore.cfg.talents = {"故长"};
    ignore.cfg.recipes = {"人剑合一·持续伤害12秒"};
    ignore.prepare();
    ignore.run(160);
    for (auto name : {"人剑合一", "人剑合一(DOT)"}) {
        const auto hits = ignore.hits(name);
        CHECK(!hits.empty());
        for (auto h : hits) CHECK(ignore.sim->GetRules().Versions()[h.snapshot_version].ignore == 614);
    }
    Fight swords("tx", "/scast 紫气东来\n/scast 八荒归元\n/scast 紫气东来\n/scast 八荒归元\n/cast 八荒归元");
    swords.cfg.talents = {"剑入", "和光"};
    swords.prepare();
    swords.run(145);
    CHECK((swords.frames("剑入") == std::vector<tick_t>{0, 144}));
    CHECK((swords.frames("剑入·引爆") == std::vector<tick_t>{24}));
    Fight stance("tx", "/scast 碎星辰\n/scast [rage>=20] 人剑合一\n/cast [buff:人剑合一&气点=10] 无我无剑");
    stance.cfg.talents = {"玄门", "一彻", "风逝"};
    stance.prepare();
    stance.run(180);
    CHECK(stance.frames("人剑合一").size() == 1);
    CHECK(!stance.frames("一彻").empty());
    CHECK(stance.frames("一彻") == stance.frames("无我无剑·十"));
    CHECK(stance.sim->GetState().energy < 20);
}

void CheckConditionalTalentEffects()
{
    for (bool selected : {false, true}) {
        Fight puppet("yly", "/cast 勾线\n/cast 挑丝");
        puppet.cfg.talents = selected ? std::vector<std::string>{"鼓关", "谐调"} : std::vector<std::string>{};
        puppet.prepare();
        puppet.run(1200);
        CHECK(!puppet.hits("勾线").empty());
        CHECK(!puppet.hits("千里急·肆").empty() == selected);
        CHECK(!puppet.hits("谐调").empty() == selected);
        Fight venom("dj", "/scast 蝎心");
        venom.cfg.talents = selected ? std::vector<std::string>{"蝎毒"} : std::vector<std::string>{};
        venom.prepare();
        venom.run(200);
        CHECK(venom.hits("蝎心").size() == (selected ? 0 : 1));
        CHECK(venom.hits("蝎心(DOT)").size() == (selected ? 6 : 0));
        if (selected) CHECK(venom.frames("蝎心(DOT)").front() == 32);
    }
    Fight refresh("txj", "/scast 起火卦\n/scast 祝由·火离\n/scast 奇门飞宫\n/scast 鬼星开穴");
    refresh.cfg.talents.clear();
    refresh.prepare();
    refresh.run(480);
    auto refreshed = refresh.frames("卦象·火离");
    CHECK(refreshed.size() == 14 && refreshed[0] == 0 && refreshed[1] == 24);
    for (int i = 1; i <= 12; ++i) CHECK(refreshed[i + 1] == i * 32);
    Fight extend("txj", "/scast 起火卦\n/scast 祝由·火离\n/scast 御前星");
    extend.cfg.talents = {"御前星", "世降"};
    extend.prepare();
    extend.run(960);
    auto extended = extend.frames("卦象·火离");
    const auto extensions = extend.frames("世降").size();
    CHECK(extensions > 0 && extended.size() == 13 + extensions);
    for (std::size_t i = 0; i < extended.size(); ++i) CHECK(extended[i] == static_cast<tick_t>(i * 32));
    for (bool enhanced : {false, true}) {
        Fight bleed("fsj", "/scast 盾飞\n/scast 斩刀");
        bleed.cfg.talents = enhanced ? std::vector<std::string>{"登锋"} : std::vector<std::string>{};
        bleed.cfg.initial_qidian = 100;
        bleed.prepare();
        bleed.run(640);
        auto frames = bleed.frames(enhanced ? "流血·登锋(DOT)" : "流血(DOT)");
        CHECK(frames.size() == (enhanced ? 26 : 13));
        CHECK(frames[1] - frames[0] == (enhanced ? 16 : 32));
    }
    for (auto [haste, spacing] : {std::pair{0, 32}, {928, 29}, {1452, 27}}) {
        Fight fire("txj", "/scast 起火卦\n/scast 祝由·火离", 0, haste);
        fire.cfg.talents = {"焚如"};
        fire.prepare();
        fire.run(480);
        auto frames = fire.frames("焚如·火离");
        CHECK(frames.size() == 13);
        for (int i = 0; i < 13; ++i) CHECK(frames[i] == i * spacing);
        CHECK(fire.frames("卦象·火离").empty());
    }
}

void CheckNovemberChanges()
{
    Fight fsj("fsj", "/cast 偃守孤旌\n/cast 斩刀");
    fsj.cfg.talents = {"偃守孤旌", "阵云结晦"};
    fsj.prepare();
    fsj.run(145);
    const auto hits = fsj.hits("斩刀");
    CHECK(hits.size() == 6);
    constexpr std::array<int, 6> bonus{ 184, 184, 123, 123, 61, 61 };
    for (std::size_t i = 0; i < hits.size(); ++i) {
        CHECK(hits[i].frame == static_cast<tick_t>(24 * i));
        const auto &stats = fsj.sim->GetRules().Versions()[hits[i].snapshot_version];
        CHECK(stats.damage == bonus[i]);
        CHECK(stats.pve == 154);
    }
    Fight gfj("gfj", "/cast 灭影追风\n/cast 孤锋破浪");
    gfj.cfg.talents.push_back("威声");
    gfj.prepare();
    gfj.run(193);
    const auto slashes = gfj.hits("孤锋破浪");
    CHECK(slashes.size() == 2);
    CHECK(slashes[0].frame == 0 && slashes[1].frame == 24);
    for (auto h : slashes) {
        CHECK(gfj.sim->GetRules().Versions()[h.snapshot_version].critical == 2500);
    }
    CHECK((gfj.frames("界破") == std::vector<tick_t>{ 32 }));
}

void CheckResources()
{
    Fight qi("zxg", "/cast [qidian>=1] 两仪化形\n/cast 四象轮回");
    qi.cfg.talents.clear();
    qi.prepare();
    qi.run(58);
    CHECK((qi.frames("两仪化形·十") == std::vector<tick_t>{ 0 }));
    CHECK((qi.frames("两仪化形·二") == std::vector<tick_t>{ 56 }));
    Fight gou("yly", "/cast 勾线");
    gou.run(85);
    CHECK(gou.frames("勾线").size() == 10);
    CHECK(gou.frames("谐调").front() == 80);
    // The second charge starts the next channel at frame 80.
    CHECK(gou.sim->GetState().qidian == 25);
}

void CheckUpdatedLegacySchools()
{
    Fight lamps("txj", "/cast [tnobuff:连极阵] 连极阵");
    lamps.cfg.talents       = { "连极阵", "知微" };
    lamps.cfg.target_events = {
        { 60,  {}, 4000 },
        { 100, {}, 400  }
    };
    lamps.prepare();
    lamps.run(161);
    CHECK((lamps.frames("纵横三才") == std::vector<tick_t>{ 32, 128, 160 }));
    CHECK(lamps.frames("知微(DOT)").front() == 64);
    Fight crit("tx", "/cast [qidian>=10] 无我无剑", 100000);
    crit.run(1);
    CHECK(crit.hits("无我无剑·十").front().outcome == RollResult::DOUBLE);
    CHECK(crit.sim->GetState().target_buff_stacks[1] == 2);
    Fight normal("tx", "/cast [qidian>=10] 无我无剑");
    normal.run(1);
    CHECK(normal.sim->GetState().target_buff_stacks[1] == 1);
    Fight notes("mw", "/cast [tnobuff:商] 商\n/cast 羽");
    notes.run(49);
    CHECK((notes.frames("商(DOT)") == std::vector<tick_t>{ 24, 24, 48, 48, 48 }));
    Fight recharge("mw", "/cast 羽");
    recharge.cfg.talents.clear();
    recharge.prepare();
    recharge.run(145);
    CHECK((recharge.frames("羽") == std::vector<tick_t>{ 0, 24, 48, 96 }));
}

void CheckEquipmentEvents()
{
    Fight medicine("wf", "/cast 使用橙武\n/cast 川乌射罔");
    medicine.run(37);
    CHECK(medicine.hits("鉴魄证身").empty());
    medicine.cfg.equipment = { "鉴魄证身" };
    medicine.prepare();
    medicine.run(37);
    CHECK((medicine.frames("商陆缀寒") == std::vector<tick_t>{ 9, 18, 27, 36 }));
    CHECK(medicine.frames("鉴魄证身") == medicine.frames("商陆缀寒"));
    Fight interrupted("wf", "/cast 使用橙武\n/cast [buff:橙武] 川乌射罔\n/fcast 钩吻断肠");
    interrupted.cfg.equipment = { "鉴魄证身" };
    interrupted.prepare();
    interrupted.run(37);
    CHECK((interrupted.frames("商陆缀寒") == std::vector<tick_t>{ 9, 18 }));
    CHECK(interrupted.frames("鉴魄证身") == interrupted.frames("商陆缀寒"));
    Fight proc("zxg", "/cast 四象轮回");
    proc.cfg.equipment = { "神兵附伤" };
    proc.prepare();
    proc.run(3200);
    const auto main = proc.frames("四象轮回"), extra = proc.frames("四象轮回·神兵");
    CHECK(!extra.empty() && extra.size() < main.size());
    CHECK(std::ranges::includes(main, extra));
    proc.run(3200, 43);
    CHECK(extra != proc.frames("四象轮回·神兵"));
    Fight pet("dj", "/cast [nobuff:灵蛇存在] 灵蛇引");
    pet.run(193);
    CHECK((pet.frames("攻击") == std::vector<tick_t>{ 64, 128, 192 }));
    CHECK((pet.frames("幻击") == std::vector<tick_t>{ 192 }));
    Fight weapon("mw", "/cast 使用橙武\n/cast 羽");
    weapon.cfg.equipment     = { "神兵·羽" };
    weapon.cfg.target.level  = 50;
    weapon.cfg.target.shield = 0;
    weapon.prepare();
    weapon.run(100);
    const auto added = weapon.hits("神兵·羽");
    CHECK(added.size() == 3);
    for (const auto &hit : added) {
        CHECK(weapon.sim->GetRules().Reduce(hit) == 17199);
    }
    Fight temperature("wf", "/cast [温寒<=-1] 钩吻断肠\n/cast 商陆缀寒");
    temperature.run(26);
    CHECK(temperature.hits("无方中和").size() == 1);
}

void CheckRecipeScope()
{
    Fight f("zxg", "/cast [气点>=8] 两仪化形\n/cast 四象轮回");
    f.cfg.recipes = { "四象轮回·2%会心", "两仪化形·5%伤害" };
    f.prepare();
    f.run(90);
    const auto &rolls = f.sim->GetRules().Versions();
    const auto  four  = rolls[f.hits("四象轮回").front().snapshot_version];
    const auto  two   = rolls[f.hits("两仪化形·十").front().snapshot_version];
    CHECK(four.critical == 200 && four.damage == 0);
    CHECK(two.critical == 0 && two.damage == 51);
    f.cfg.recipes = { "两仪回豆", "两仪化形·5%伤害", "两仪化形·4%伤害", "两仪化形·3%会心", "两仪化形·2%会心" };
    bool rejected = false;
    try {
        f.prepare();
    } catch (const std::invalid_argument &) {
        rejected = true;
    }
    CHECK(rejected);
}

void CheckLegendaryWeapons()
{
    Fight strikes("tx", "/cast 三柴剑法");
    for (const auto &tier : s50::LegendaryWeapons) {
        strikes.cfg.equipment = {std::string(tier.name)};
        strikes.prepare();
        strikes.run(140);
        const auto hits = strikes.hits("三柴剑法");
        CHECK(hits.size() >= 7);
        const auto &versions = strikes.sim->GetRules().Versions();
        for (int i = 0; i < 7; ++i) CHECK(versions[hits[i].snapshot_version].strain_flat == std::min(i, 5) * tier.strain_per_stack);
    }
    strikes.cfg.target_events = {{22, {}, 2000}, {160, {}, 400}};
    strikes.prepare();
    strikes.run(161);
    CHECK(strikes.sim->GetRules().Versions()[strikes.hits("三柴剑法").back().snapshot_version].strain_flat == 0);

    Fight bolts("tlgd", "/cast 使用橙武\n/cast 暴雨梨花针");
    bolts.cfg.equipment = {"橙武·960品"};
    bolts.prepare();
    bolts.run(36);
    CHECK((bolts.frames("暴雨梨花针·额外") == std::vector<tick_t>{7,7,14,14,21,21,28,28,35,35}));
    Fight interrupted("tlgd", "/cast 使用橙武\n/cast 暴雨梨花针\n/fcast 天女散花");
    interrupted.cfg.equipment = bolts.cfg.equipment;
    interrupted.prepare();
    interrupted.run(35);
    CHECK((interrupted.frames("暴雨梨花针·额外") == std::vector<tick_t>{7,7,14,14,21,21}));
}

void CheckInstanceEquipment()
{
    Fight fight("tx", "/cast 三柴剑法");
    fight.cfg.equipment = {"晚山青", "见天心", "落潮平", "疏雨落", "无观囊·加速攻击"};
    fight.cfg.attributes.overcome = 1000;
    fight.cfg.target_events = {{40, 9000, {}}};
    fight.prepare();
    fight.run(349);
    const auto normal = fight.hits("三柴剑法");
    const auto &versions = fight.sim->GetRules().Versions();
    CHECK(versions[normal.front().snapshot_version].attack_flat == 0);
    CHECK(versions[normal[1].snapshot_version].attack_flat == 189);
    CHECK(versions[normal[2].snapshot_version].overcome_flat == 0);
    CHECK(versions[normal[3].snapshot_version].overcome_flat == 313);
    CHECK(versions[normal[3].snapshot_version].critical > versions[normal[2].snapshot_version].critical);
    CHECK((fight.frames("巽·晚山青") == std::vector<tick_t>{0,168,336}));
    CHECK(fight.sim->GetRules().Reduce(fight.hits("巽·晚山青").front()) == 36800);
    fight.cfg.equipment.push_back("陌微尘");
    bool rejected = false;
    try { fight.prepare(); } catch (const std::invalid_argument &) { rejected = true; }
    CHECK(rejected);
}

void CheckAdaptiveEquipmentGains()
{
    Fight fight("tx", "/cast 三柴剑法");
    fight.cfg.attributes.crit = 3100;
    fight.cfg.attributes.overcome = 3000;
    const rt::BatchOptions options{16, 42, 480, 2, 4096, 8192};
    constexpr int index = static_cast<int>(rt::AttributeGainKind::OVERCOME);
    for (const auto *item : {"见天心", "无观冠·适应之力"}) {
        fight.cfg.equipment = {item};
        rt::BatchControl control;
        auto increments = rt::DefaultGainIncrements;
        increments[index] = 208;
        const auto gains = rt::RunAttributeGainsControlled(fight.macro.program, options, control, fight.cfg, rt::detail::TotalDamageReducer{}, increments);
        const auto changed = rt::ApplyAttributeGain(fight.cfg, index, increments[index]);
        const auto direct = rt::RunBatch(fight.macro.program, options, rt::LoadSpecialization(changed));
        CHECK(gains.gains[index] && gains.gains[index]->checksum == direct.checksum);
        CHECK(control.completed == options.iterations * rt::GainSimulationStages(fight.cfg));
    }
}

void CheckFullRaid()
{
    Fight dot("hjy", "/cast [tnobuff:商阳指] 商阳指");
    dot.cfg.raid_full = true;
    dot.cfg.talents.clear();
    dot.prepare();
    dot.run(242);
    const auto ticks = dot.hits("商阳指(DOT)");
    CHECK(ticks.size() == 5);
    const auto &versions = dot.sim->GetRules().Versions();
    CHECK(versions[ticks.front().snapshot_version].strain_flat == 11400);
    CHECK(ticks.front().snapshot_version == ticks.back().snapshot_version);
    CHECK(versions[ticks.back().live_version].strain_flat == 7000);

    Fight fight("tx", "/cast 三柴剑法");
    fight.cfg.raid_full = true;
    fight.cfg.gear_score = 17188;
    fight.prepare();
    fight.run(1600);
    const auto procs = fight.hits("逐云寒蕊");
    CHECK(procs.size() == 24);
    for (std::size_t i = 0; i < procs.size(); ++i) {
        CHECK(procs[i].frame / 800 == i / 12 && procs[i].frame % 800 < 480);
        CHECK(procs[i].base == 1800 && procs[i].outcome == RollResult::HIT);
        if (i) CHECK(procs[i].frame - procs[i - 1].frame >= 16);
    }
    fight.run(1600);
    CHECK(fight.hits("逐云寒蕊") == procs);
}

void CheckRaidGear()
{
    Fight f("tx", "/cast 使用风腰坠\n/cast 三柴剑法", 100000);
    f.cfg.equipment = { "刃凌·英雄", "昆吾·弦刃·英雄", std::string(s50::BeltEnchant), std::string(s50::WaterWeapon), std::string(s50::WindPendant) };
    f.prepare();
    f.run(3200);
    const auto &rules = f.sim->GetRules();
    auto        main  = f.hits("三柴剑法");
    CHECK(!main.empty());
    CHECK(rules.Versions().at(main[0].snapshot_version).attack_flat == 0);
    CHECK(rules.Versions().at(main[1].snapshot_version).attack_flat == 22);
    CHECK(rules.Versions().at(main.back().snapshot_version).attack_flat == 220);
    bool belt = false;
    for (const auto &hit : main) {
        const auto stats = rules.Versions().at(hit.snapshot_version);
        CHECK(stats.overcome_flat == ((hit.frame % 2880 < 240) ? 931 : 0));
        belt |= stats.final_damage == 10 || stats.final_damage == 51;
    }
    CHECK(belt);
    for (auto [name, damage] :
         {
             std::pair{ "刃凌·英雄",      10531 },
             std::pair{ "昆吾·弦刃·英雄", 15796 }
    })
    {
        const auto hits = f.hits(name);
        CHECK(hits.size() > 1);
        tick_t previous = -160;
        for (const auto &hit : hits) {
            CHECK(hit.frame - previous >= 160);
            CHECK(hit.outcome == RollResult::HIT && rules.Reduce(hit) == damage);
            previous = hit.frame;
        }
    }
    f.cfg.attributes.crit = 0;
    f.prepare();
    f.run(3200);
    CHECK(f.hits("刃凌·英雄").empty());
    f.cfg.equipment.emplace_back("刃凌");
    bool rejected = false;
    try {
        f.prepare();
    } catch (const std::invalid_argument &) {
        rejected = true;
    }
    CHECK(rejected);
}

void CheckSpeedDotAndTrialGear()
{
    Fight fight("tx", "/cast 三柴剑法");
    fight.cfg.equipment = {"动寒星", "影凋零", "无观囊·伤害14档"};
    for (const auto [haste, count] : {std::pair{0,12}, {443,14}, {444,15}, {928,17}, {1451,20}, {2024,22}, {3000,22}}) {
        fight.cfg.attributes.haste = haste;
        fight.prepare();
        fight.run(640);
        const auto ticks = fight.hits("速·震");
        CHECK(ticks.size() == count);
        for (int i = 0; i < count; ++i) {
            CHECK(ticks[i].frame == (i + 1) * 16);
            CHECK(ticks[i].outcome == RollResult::HIT && fight.sim->GetRules().Reduce(ticks[i]) == 1100);
        }
    }
    fight.cfg.attributes.haste = 0;
    fight.cfg.target_events = {{1, {}, 2000}, {640, {}, 400}};
    fight.prepare();
    fight.run(656);
    CHECK(fight.frames("速·震").size() == 12);
    fight.run(657);
    CHECK(fight.frames("速·震").back() == 656);
    CHECK((fight.frames("无观·荒·14档") == std::vector<tick_t>{0, 640}));
    CHECK(fight.sim->GetRules().Reduce(fight.hits("无观·荒·14档").front()) == 31395);

    for (const auto *shoe : {"无观鞋·会心", "无观鞋·破防"}) {
        fight.cfg.equipment = {shoe, "晚山青", "无观囊·伤害13档"};
        fight.cfg.player_life = .75;
        fight.cfg.target_events = {{21, {}, {}, {}, 3000}, {42, {}, {}, {}, 1501}, {42, {}, {}, {}, 1501},
                                   {63, {}, {}, 10000, 3001}, {84, {}, {}, 7500}, {105, {}, {}, 7501}};
        fight.prepare();
        fight.run(365);
        const auto normal = fight.hits("三柴剑法");
        for (const auto &hit : normal) {
            const auto &stats = fight.sim->GetRules().Versions()[hit.snapshot_version];
            const bool active = (hit.frame > 0 && hit.frame < 63) || hit.frame > 336;
            if (shoe == std::string_view("无观鞋·破防")) CHECK(stats.overcome_flat == (active ? 974 : 0));
            else CHECK(stats.critical == (active ? static_cast<int>(974 * 10000. / (9.609 * 990)) : 0));
        }
        CHECK(fight.frames("巽·晚山青").front() == 105);
        CHECK(fight.sim->GetRules().Reduce(fight.hits("无观·荒·13档").front()) == 26910);
        CHECK(fight.sim->GetState().life == .7501);
        CHECK(fight.sim->StartingState().life == .75);
    }
}

void CheckShieldBash()
{
    Fight fight("fsj", "/cast 盾击");
    fight.cfg.talents.clear();
    fight.cfg.initial_qidian = 0;
    fight.cfg.recipes = {"盾击·三段回怒"};
    fight.prepare();
    fight.run(64);
    CHECK((fight.frames("盾击·一") == std::vector<tick_t>{0}));
    CHECK((fight.frames("盾击·二") == std::vector<tick_t>{16}));
    CHECK((fight.frames("盾击·三") == std::vector<tick_t>{32}));
    CHECK(fight.sim->GetState().qidian == 35);
    fight.run(4096);
    const auto first = fight.frames("盾击·一");
    CHECK(first[1] == 64);
    CHECK(fight.hits("盾击·神兵").empty());
    fight.cfg.equipment = {"橙武·1240品"};
    fight.prepare();
    fight.run(4096);
    const auto procs = fight.frames("盾击·神兵");
    CHECK(!procs.empty() && procs.size() < first.size());
    for (auto frame : procs) CHECK(std::ranges::contains(first, frame) || frame == 16 || frame == 32);
}

void CheckGeneralSealAndDragonProcs()
{
    Fight seal("baj", "/cast 上将军印");
    constexpr std::string_view cuts[]{"上将军印", "上将军印·连续1", "上将军印·连续2", "上将军印·连续3",
                                      "上将军印·连续4", "上将军印·连续5", "上将军印·连续6"};
    for (bool spirit : {false, true}) {
        for (bool extended : {false, true}) {
            seal.cfg.talents.clear();
            if (spirit) seal.cfg.talents.emplace_back("临江");
            if (extended) seal.cfg.talents.emplace_back("分疆");
            seal.prepare();
            seal.run(48);
            const std::vector<int> frames = spirit ? (extended ? std::vector{4,8,12,16,20,24,28} : std::vector{4,12,20,24,28}) :
                                                     (extended ? std::vector{6,12,18,24,30,36,42} : std::vector{8,16,24,32,40});
            for (std::size_t i = 0; i < std::size(cuts); ++i) {
                const auto hits = seal.hits(cuts[i]);
                CHECK(hits.size() == (i < frames.size() ? 1u : 0u));
                if (!hits.empty()) {
                    CHECK(hits[0].frame == frames[i]);
                    CHECK(seal.sim->GetRules().Versions().at(hits[0].snapshot_version).attack == (spirit ? 154 : 0));
                }
            }
            CHECK(seal.hits("上将军印·神兵").empty());
        }
    }

    // A missed swing neither advances the target's stage nor rolls the weapon.
    seal.cfg.talents.clear();
    seal.cfg.target_events = {{16, {}, 1001}, {24, {}, 1000}};
    seal.cfg.equipment = {"橙武·1240品"};
    seal.prepare();
    int procs = 0;
    for (int seed = 1; seed <= 256; ++seed) {
        seal.run(41, seed);
        CHECK((seal.frames(cuts[0]) == std::vector<tick_t>{8}));
        CHECK((seal.frames(cuts[1]) == std::vector<tick_t>{24}));
        for (auto frame : seal.frames("上将军印·神兵")) {
            CHECK(frame == 8 || frame == 24 || frame == 32 || frame == 40);
            ++procs;
        }
    }
    CHECK(procs > 65 && procs < 140);
    Fight interrupted("baj", "/cast 上将军印\n/fcast 破釜沉舟");
    interrupted.cfg.talents.clear();
    interrupted.prepare();
    interrupted.run(48);
    CHECK(!interrupted.hits("破釜沉舟").empty() && interrupted.hits(cuts[3]).empty());
    interrupted.cfg.talents = {"临江"};
    interrupted.prepare();
    interrupted.run(48);
    CHECK((interrupted.frames(cuts[4]) == std::vector<tick_t>{28}));

    Fight dragon("xcj", "/cast 龙战于野");
    dragon.cfg.talents.clear();
    dragon.prepare();
    dragon.run(1);
    CHECK(dragon.hits("龙战于野·神兵").empty());
    dragon.cfg.equipment = {"橙武·960品"};
    dragon.prepare();
    procs = 0;
    for (int seed = 1; seed <= 256; ++seed) {
        dragon.run(1, seed);
        CHECK(dragon.hits("龙战于野·三").size() == 1);
        const auto hits = dragon.hits("龙战于野·神兵");
        CHECK(hits.size() <= 1);
        procs += hits.size();
    }
    CHECK(procs > 96 && procs < 160);
}

void CheckPermanents()
{
    s50::Config cfg;
    cfg.book       = s50::Find("gfj");
    cfg.attributes = cfg.book->default_attributes;
    cfg.permanents = {
        { "药品增强", "巴山凝神丹·精（会心)" },
        { "阵法",     "刀宗阵"               }
    };
    const auto prepared = s50::Prepare(cfg);
    CHECK(prepared->config.attributes.crit == cfg.attributes.crit + 203);
    CHECK(prepared->config.attributes.overcome_percent == 256);
    CHECK(prepared->config.attributes.strain_percent == 20);
    CHECK(prepared->config.attributes.attack_percent == 50);
    cfg.book      = s50::Find("mw");
    bool rejected = false;
    try {
        s50::Prepare(cfg);
    } catch (const std::invalid_argument &) {
        rejected = true;
    }
    CHECK(rejected);
}

void CheckOldVersion4Coefficients()
{
    // Exercise every declared channel/rank against the unchanged historical implementation
    // with its own season constants, including boundaries absent from the default rotations.
    const auto compare = [](int channel, int ticks, int interval) {
        CHECK(rt::DirectAttackCoefficient(channel, 10) == LegacyReference::PhysicsDamageCoefficient(channel, 0));
        CHECK(rt::DirectAttackCoefficient(channel, 12) == LegacyReference::MagicDamageCoefficient(channel, 0));
        if (ticks) {
            CHECK(rt::DotAttackCoefficient(channel, ticks, interval, 10) == LegacyReference::PhysicsDotDamageCoefficient(channel, ticks, interval));
            CHECK(rt::DotAttackCoefficient(channel, ticks, interval, 12) == LegacyReference::MagicDotDamageCoefficient(channel, ticks, interval));
        }
    };
    for (int channel : {0, 1, 15, 16, 17, 263}) {
        compare(channel, 3, 11);
        compare(channel, 7, 39);
    }
    for (const auto &book : s50::Books()) {
        for (const auto &hit : book.hits) {
            compare(hit.channel, hit.ticks, hit.interval);
            for (int rank : hit.rank_channels) compare(rank, hit.ticks, hit.interval);
        }
    }

    Fight snake("dj", "/cast 蛇影");
    snake.cfg.talents = {"蛇悉"};
    snake.cfg.recipes = {"蛇影·5%伤害", "蛇影·4%伤害"};
    snake.cfg.equipment = {std::string(s50::ClassSet)};
    snake.prepare();
    snake.run(90);
    // The two books and the set modify the integer in registration order, even if selected in reverse.
    for (auto [name, expected] : {std::pair{"蛇影", 796}, {"蛇影(DOT)", 686}}) {
        const auto hits = snake.hits(name);
        CHECK(!hits.empty());
        for (const auto &hit : hits)
            CHECK(snake.sim->GetRules().Versions()[hit.snapshot_version].channel_override == expected);
    }

    // The rank is captured at launch; both live channel bonuses are applied at impact.
    Fight puppet("yly", "/cast 生地狱");
    auto preset = *puppet.cfg.book;
    for (std::string_view name : {"傀儡", "连环慢", "缠绞"}) {
        const auto aura = std::ranges::find(preset.buffs, name, &rt::BuffDescriptor::name);
        preset.initial_auras.push_back(static_cast<rt::Slot>(aura - preset.buffs.begin()));
    }
    puppet.cfg.book = &preset;
    puppet.cfg.talents.clear();
    puppet.prepare();
    puppet.run(15);
    const auto delayed = puppet.hits("生地狱");
    CHECK(delayed.size() == 3);
    for (const auto &hit : delayed) {
        CHECK(hit.level == 2);
        // Rank two: 227 -> 499 -> 623.
        CHECK(puppet.sim->GetRules().Versions()[hit.snapshot_version].channel_override == 623);
    }
}

// Reference site, 2026-11-20. One noncritical hit at midpoint base damage.
void CheckOnlineHitValues()
{
    struct Sample
    {
        std::string_view code, name;
        int              damage;
    };

    constexpr Sample samples[] = {
        { "mw",   "羽",            1813 },
        { "axzy", "灭",            2409 },
        { "baj",  "雷走风切",      538  },
        { "bxj",  "剑破虚空",      3870 },
        { "dj",   "蝎心",          2632 },
        { "fsj",  "盾猛",          4239 },
        { "fysj", "烈日斩",        2519 },
        { "gfj",  "行云势·一",     1916 },
        { "hjy",  "阳明指",        2868 },
        { "lhj",  "击水三千·一",   1901 },
        { "shxj", "劲风簇",        923  },
        { "txj",  "三星临",        1385 },
        { "tlgd", "蚀肌弹",        1749 },
        { "wf",   "商陆缀寒",      1241 },
        { "xcj",  "拨狗朝天·真实", 1589 },
        { "yjj",  "普渡四方",      2194 },
        { "yly",  "挑丝",          2488 },
        { "ztg",  "断脉",          3648 },
        { "zxg",  "四象轮回",      2969 },
    };
    for (const auto &sample : samples) {
        s50::Config config;
        config.book              = s50::Find(sample.code);
        config.attributes.weapon = 100;
        auto skill               = std::ranges::find(config.book->skills, sample.name, &rt::SkillDescriptor::name);
        CHECK(skill != config.book->skills.end());
        auto        slot   = static_cast<rt::Slot>(skill - config.book->skills.begin());
        const auto &effect = config.book->hits.at(slot);
        auto        stats  = effect.passive;
        if (sample.code == "axzy") {
            // The site's isolated skill includes its two permanent Tiance effects.
            stats.attack      += 51;
            stats.shield_flat -= 280;
        }
        rt::DamageIntent hit;
        hit.skill = slot;
        hit.level = hit.sub = 1;
        hit.base            = effect.fixed + effect.spread / 2;
        CHECK(std::abs(s50::Damage(config, hit, stats, stats) - sample.damage) <= 1);
    }
}

void CheckPresetMechanics()
{
    Fight bow("shxj", "/cast 聚势摧霆");
    bow.run(41);
    CHECK(bow.hits("聚势摧霆").size() == 8);
    CHECK(bow.hits("聚势摧霆·蓄力").size() == 8);
    CHECK(bow.hits("标鹄").size() == 2);
    CHECK(bow.sim->GetState().qidian == 0);
    Fight second("shxj", "/cast [箭矢=8] 饮羽簇\n/cast 没石饮羽");
    second.run(65);
    CHECK((second.frames("饮羽簇") == std::vector<tick_t>{ 32, 52, 58, 64 }));

    Fight channel("mw", "/cast 徵");
    channel.run(61);
    CHECK((channel.frames("徵·凌冬") == std::vector<tick_t>{ 10, 20, 30, 40, 50, 60 }));
    const auto hits = channel.hits("徵·凌冬");
    for (int i = 1; i < 6; ++i) {
        CHECK(channel.sim->GetRules().Reduce(hits[i]) > channel.sim->GetRules().Reduce(hits[i - 1]));
    }

    Fight spear("fsj", "/cast 雁门迢递\n/cast 月照连营\n/cast 阵云结晦");
    spear.run(97);
    CHECK((spear.frames("阵云结晦") == std::vector<tick_t>{ 0 }));
    CHECK((spear.frames("月照连营") == std::vector<tick_t>{ 24 }));
    CHECK((spear.frames("雁门迢递") == std::vector<tick_t>{ 48 }));
}

void CheckFenShanRage()
{
    // 盾飞开始时可以接预铺的月照，盾猛受飞盾限制，只有切姿态时才造成盾飞首击。
    Fight throw_shield("fsj", "/cast 盾飞\n/cast 盾猛\n/cast 月照连营\n/cast 雁门迢递");
    throw_shield.cfg.precombat = true;
    throw_shield.prepare();
    throw_shield.run(25);
    CHECK((throw_shield.frames("月照连营") == std::vector<tick_t>{0}));
    CHECK((throw_shield.frames("雁门迢递") == std::vector<tick_t>{24}));
    CHECK((throw_shield.frames("盾飞") == std::vector<tick_t>{6, 22}));
    CHECK(throw_shield.hits("盾猛").empty());
    const auto &mutations = throw_shield.sim->Log().Mutations();
    const auto first_stack = [&](std::string_view name) {
        return std::ranges::find_if(mutations, [&](const auto &m) {
            return m.kind == rt::MutationKind::BUFF && m.c > 0 && throw_shield.cfg.book->buffs[m.slot].name == name;
        });
    };
    const auto weakness = first_stack("虚弱"), stance = first_stack("擎刀");
    CHECK(weakness != mutations.end() && weakness->frame == 2 && weakness->target);
    CHECK(stance != mutations.end() && stance->frame == 6 && !stance->target);

    Fight two("fsj", "/cast 盾飞\n/cast 绝刀");
    two.cfg.initial_qidian = 100;
    two.cfg.recipes        = { "绝刀·怒气消耗-15" };
    two.prepare();
    two.run(47);
    CHECK((two.frames("绝刀") == std::vector<tick_t>{ 6, 30 }));
    CHECK(two.sim->GetState().qidian == 0);
    two.cfg.recipes.clear();
    two.prepare();
    two.run(47);
    CHECK(two.hits("绝刀").size() == 2);
    // 100 -> 35 (-65), then 35 -> 0 (-35); only the first blow is full tier.
    const auto  hits     = two.hits("绝刀");
    const auto &versions = two.sim->GetRules().Versions();
    CHECK(versions[hits[0].snapshot_version].channel_override > versions[hits[1].snapshot_version].channel_override);
    Fight opening("fsj", "/cast 月照连营\n/cast 阵云结晦\n/cast 雁门迢递");
    opening.cfg.precombat = true;
    opening.prepare();
    opening.run(49);
    CHECK((opening.frames("月照连营") == std::vector<tick_t>{ 0 }));
    CHECK((opening.frames("雁门迢递") == std::vector<tick_t>{ 24 }));
    CHECK((opening.frames("阵云结晦") == std::vector<tick_t>{ 48 }));
    CHECK(opening.sim->GetState().energy == 40);
    CHECK(two.cfg.book->hits[two.hits("绝刀").front().skill].passive.ignore == 1024);
    Fight slam("fsj", "/cast 盾猛");
    slam.cfg.talents = { "残裂" };
    slam.prepare();
    slam.run(193);
    CHECK((slam.frames("盾猛") == std::vector<tick_t>{ 0, 192 }));
    CHECK(slam.sim->GetState().qidian == 70);
    Fight charge("fsj", "/cast 血怒");
    charge.cfg.talents = { "惊涌", "迸甲", "朔荒" };
    charge.cfg.recipes = { "血怒·延长1秒（一）", "血怒·回怒+5" };
    charge.prepare();
    charge.run(17);
    CHECK(charge.sim->GetState().qidian == 45);
    CHECK(charge.sim->GetState().energy == 120);
}

void CheckRotationTriggers()
{
    Fight ink("hjy", "/cast 墨海临源\n/cast 阳明指");
    ink.run(141);
    CHECK(ink.hits("阳明指").size() == 5);
    // The fifth Yangming now has 20 ink available, so it is instant and its GCD ends at 136.
    CHECK((ink.frames("临源·一") == std::vector<tick_t>{ 136 }));
    Fight lotus("mw", "/cast 羽");
    lotus.cfg.talents = { "斩长鲸" };
    lotus.prepare();
    lotus.run(161);
    CHECK((lotus.frames("羽") == std::vector<tick_t>{ 0, 24, 160 }));
    CHECK((lotus.frames("青莲剑·羽") == std::vector<tick_t>{ 24, 24, 24, 160, 160, 160 }));
    Fight clouds("gfj", "/cast 沧浪三叠");
    clouds.run(200);
    CHECK(clouds.hits("沧浪三叠·一").empty());
    CHECK(clouds.hits("弱点击破").empty());
    Fight thread("yly", "/cast 勾线");
    thread.cfg.talents = { "绞网" };
    thread.prepare();
    thread.run(1);
    CHECK(thread.hits("寄线").size() == 4);
}

bool report_replays = false;
std::string_view report_school;

struct Button
{
    tick_t           frame;
    std::string_view name;
    bool             grant_opportunity = false;
    int              pulses = 0;
};

// Arm a macro wake for each recorded input. The normal cast path can still reject a button.
class InputReplay : public s50::Rules
{
    std::span<const Button>        inputs_;
    std::size_t                    next_ = 0, rejected_ = 0;
    tick_t                         first_rejection_ = -1;
    bool                           native_rolls_;
    bool                           first_diagnostic_ = false;
    rt::DeadlineQueue::TimerId     wake_{};
    std::shared_ptr<s50::Prepared> replay_data_;

public:
    InputReplay(std::shared_ptr<s50::Prepared> data, std::span<const Button> inputs, bool native_rolls = false, bool diagnostic = false) :
        Rules(data), inputs_(inputs), native_rolls_(native_rolls), first_diagnostic_(diagnostic), replay_data_(data)
    {
    }

    template <class E>
    void Setup(E &e)
    {
        Rules::Setup(e);
        wake_ = e.Timers().Create({ rt::TimerKind::MACRO_WAKE }, 2);
    }

    template <class E>
    void Reset(E &e, std::uint64_t seed)
    {
        next_ = rejected_ = 0;
        first_rejection_ = -1;
        Rules::Reset(e, seed);
    }

    template <class E>
    bool Cast(E &e, const rt::MacroLine &line)
    {
        CHECK(next_ < inputs_.size());
        const auto &button = inputs_[next_];
        if (e.GetState().now < button.frame) {
            e.Timers().ArmAt(wake_, button.frame);
            return false;
        }
        CHECK(e.GetState().now == button.frame);
        if (!native_rolls_ && replay_data_->config.book->code == "yjj") {
            // Condition only the random opportunity on the recorded buttons. All spends, gates and cooldowns run normally.
            bool granted = false;
            for (auto &step : replay_data_->post_cast[line.action_slot]) {
                if (step.op == s50::Op::ChanceAura) {
                    step.amount = button.grant_opportunity && !granted ? 10000 : 0;
                    granted     = true;
                }
            }
            CHECK(!button.grant_opportunity || granted);
        }
        if (!Rules::Cast(e, line, button.pulses)) {
            if (!rejected_) first_rejection_ = button.frame;
            ++rejected_;
            if (first_diagnostic_) {
                first_diagnostic_ = false;
                const auto &s = e.GetState();
                std::cout << "first rejection frame=" << button.frame << " button=" << button.name << " resource=" << s.qidian << "/" << s.energy
                          << " cooldown=" << s.skill_ready_at[line.action_slot] << " gcd=" << s.gcd_ready_at[replay_data_->actions[line.action_slot]->gcd_group]
                          << " channel=" << s.casting_skill << " prepare=" << s.preparing_skill << '\n';
            }
            if (!native_rolls_) {
                std::cerr << button.frame << " rejected " << button.name << '\n';
                CHECK(false);
            }
            // Consume this input in the test driver; do not substitute an action or apply its effects.
        }
        ++next_;
        return true;
    }

    std::size_t rejected() const { return rejected_; }
    tick_t first_rejection() const { return first_rejection_; }

    std::size_t consumed() const { return next_; }
};

void PublishedLoadout(s50::Config &cfg)
{
    const auto code = cfg.book->code;
    auto swap = [](auto &values, std::string_view from, std::string_view to) {
        auto it = std::ranges::find(values, from);
        CHECK(it != values.end());
        *it = to;
    };
    if (code == "baj") swap(cfg.recipes, "擒龙六斩·回气劲", "擒龙六斩·4%会心");
    if (code == "lhj") swap(cfg.talents, "驾鸾", "天行");
    if (code == "fysj") swap(cfg.talents, "诛邪镇魔", "无明业火");
    if (code == "yly") swap(cfg.talents, "叠拍", "绞网");
    if (code == "tlgd") {
        swap(cfg.talents, "星离雨散", "千秋万劫");
        swap(cfg.talents, "化血迷心", "尽刑彻毒");
        swap(cfg.recipes, "千机变·人偶图绝章", "暗藏杀机·立即激活");
    }
    if (code == "ztg") {
        std::erase(cfg.recipes, "抟风令·调息减2秒");
        std::erase(cfg.recipes, "抟风令·调息减3秒");
    }
    if (code == "gfj") {
        swap(cfg.recipes, "行云势·距离提高2尺", "行云势·3%伤害");
        swap(cfg.recipes, "留客雨·距离提高2尺", "留客雨·3%伤害");
        swap(cfg.recipes, "停云势·距离提高2尺", "停云势·调息减16帧");
        swap(cfg.recipes, "停云势·4%会心", "停云势·调息减32帧");
    }
    if (code == "shxj") swap(cfg.recipes, "饮羽簇·3%伤害", "饮羽簇·4%会心");
}

std::shared_ptr<s50::Prepared> PreparePublishedReplay(const s50::Config &cfg)
{
    auto data = std::make_shared<s50::Prepared>(*s50::Prepare(cfg));
    if (cfg.book->code == "shxj") {
        // 紫武_襄尺_桑柘 explicitly requests six distinct seals at the start of its recorded pull.
        data->resources[1].initial = 6;
        for (auto name : {"虎印", "鹰印", "猪印", "象印", "熊印", "狼印"}) {
            const auto slot = std::ranges::find(cfg.book->buffs, name, &rt::BuffDescriptor::name) - cfg.book->buffs.begin();
            data->opening.push_back({s50::Op::Add, static_cast<rt::Slot>(slot)});
        }
    }
    return data;
}

void CheckYouLuoPuppet()
{
    auto run = [](s50::Config cfg, std::span<const Button> buttons, tick_t end, int blocked, auto check) {
        std::string macro;
        for (const auto &button : buttons) macro += "/scast " + std::string(button.name) + "\n";
        auto [compiled, errors] = rt::MacroCompiler::Compile(macro, rt::CatalogMacroOptions(cfg.book->Catalog()));
        CHECK(errors.empty());
        rt::Simulation<InputReplay> sim(compiled.program, InputReplay(PreparePublishedReplay(cfg), buttons, true));
        sim.Start(end, 42);
        sim.Run();
        CHECK(sim.GetRules().consumed() == buttons.size());
        CHECK(sim.GetRules().rejected() == blocked);
        check(sim);
    };
    Fight f("yly");
    auto &cfg = f.cfg;
    cfg.recipes = {"千里急·2%会心", "千里急·3%会心"};
    cfg.equipment = {std::string(s50::ClassSet)};
    cfg.target_events = {{.frame = 72, .distance_hundredths = 1200}};
    // Both directions of the shared GCD, a blocked DOT refresh and a puppet left behind by a moving target.
    constexpr Button chain[]{{0,"绊线"}, {0,"生地狱"}, {0,"连环慢"}, {24,"应天长"}, {48,"生地狱"},
                             {48,"应天长"}, {72,"挑丝"}, {96,"连环慢"}, {96,"挑丝"}};
    run(cfg, chain, 106, 1, [&](const auto &sim) {
        std::vector<tick_t> stages, dots;
        const auto &versions = sim.GetRules().Versions();
        for (const auto &hit : sim.Log().Intents()) {
            const auto name = cfg.book->skills[hit.skill].name;
            const auto &stats = versions[hit.snapshot_version];
            if (name.starts_with("千里急·")) {
                stages.push_back(hit.frame);
                CHECK(stats.attack == 154 && stats.critical == 2000 && stats.damage == 102);
                if (hit.frame == 0) CHECK(stats.fragile == 51);
            } else if (name == "千里急(DOT)") {
                dots.push_back(hit.frame);
                CHECK(stats.channel_override == 1500 && stats.fragile == 0 && stats.damage == 0);
                CHECK(versions[hit.live_version].channel_override == -1);
            } else if (name == "绊线") {
                CHECK(stats.fragile == 0 && stats.attack == 0);
            }
        }
        CHECK((stages == std::vector<tick_t>{0, 24, 96}));
        CHECK((dots == std::vector<tick_t>{40, 56, 72, 88, 104}));
    });

    cfg.target_events.clear();
    // Returning to the player is possible during the entry cooldown and preserves its deadline.
    constexpr Button control[]{{0,"落笼簿"}, {0,"千里急"}, {16,"落笼簿"}, {24,"挑丝"}, {48,"落笼簿"},
                               {160,"落笼簿"}, {160,"千里急"}, {176,"落笼簿·合"}};
    run(cfg, control, 177, 1, [&](const auto &sim) {
        std::vector<tick_t> stages;
        for (const auto &hit : sim.Log().Intents()) {
            if (!cfg.book->skills[hit.skill].name.starts_with("千里急·")) continue;
            stages.push_back(hit.frame);
            CHECK(sim.GetRules().Versions()[hit.snapshot_version].ignore == (hit.frame == 24 ? 614 : 0));
        }
        CHECK((stages == std::vector<tick_t>{0, 24, 160}));
    });

    constexpr Button weave[]{{0,"勾线"}, {16,"落笼簿"}, {16,"千里急"}, {32,"落笼簿"}, {176,"落笼簿"}};
    run(cfg, weave, 177, 0, [&](const auto &sim) {
        int pulses = 0;
        for (const auto &hit : sim.Log().Intents()) if (cfg.book->skills[hit.skill].name == "勾线") ++pulses;
        CHECK(pulses == 10);
    });
}

// Diagnostic replays consume every supplied button once. Missing actions and blocked inputs
// are reported separately, without moving them to another frame or inventing damage.
void InspectInputs(const char *code, int haste, int last, const char *filename)
{
    const auto *book = s50::Find(code);
    CHECK(book && last > 0);
    std::ifstream input(filename);
    CHECK(input.good());
    std::vector<std::pair<int, std::string>> rows;
    std::map<std::string, int> missing;
    int frame;
    std::string name;
    std::vector<int> limits;
    std::string row;
    while (std::getline(input, row)) {
        std::istringstream fields(row);
        int limit = 0;
        CHECK(fields >> frame >> name);
        fields >> limit;
        CHECK(frame >= 0 && frame <= last && (rows.empty() || frame >= rows.back().first));
        if (std::ranges::any_of(book->castable, [&](auto slot) { return book->skills[slot].name == name; })) { rows.emplace_back(frame, name); limits.push_back(limit); }
        else ++missing[name];
    }
    CHECK(input.eof() && !rows.empty());
    std::string macro;
    std::vector<Button> buttons;
    for (const auto &[at, skill] : rows) {
        macro += "/scast " + skill + "\n";
        buttons.push_back({at, skill, false, limits[buttons.size()]});
    }
    auto [compiled, errors] = rt::MacroCompiler::Compile(macro, rt::CatalogMacroOptions(book->Catalog()));
    CHECK(errors.empty());
    s50::Config cfg;
    cfg.book = book;
    cfg.attributes = book->default_attributes;
    cfg.attributes.haste = haste;
    cfg.precombat = true;
    cfg.talents.assign(book->default_talents.begin(), book->default_talents.end());
    cfg.recipes.assign(book->default_recipes.begin(), book->default_recipes.end());
    PublishedLoadout(cfg);
    cfg.equipment.assign(book->default_equipment.begin(), book->default_equipment.end());
    auto prepared = PreparePublishedReplay(cfg);
    rt::Simulation<InputReplay> sim(compiled.program, InputReplay(prepared, buttons, true, true));
    double total = 0, blocked = 0;
    for (int run = 0; run < 128; ++run) {
        sim.Start(last + 1, rt::IterationSeed(42, run));
        sim.Run();
        CHECK(sim.GetRules().consumed() == rows.size());
        total += sim.TotalDamage();
        blocked += sim.GetRules().rejected();
    }
    std::cout << code << " known=" << rows.size() << " blocked=" << blocked / 128 << " DPS=" << std::lround(total * 16 / (128 * last));
    for (const auto &[skill, count] : missing) std::cout << " missing[" << skill << "]=" << count;
    std::cout << '\n';
    std::map<std::string_view, std::pair<int, uint64_t>> observed;
    for (const auto &hit : sim.Log().Intents()) {
        auto &[count, hash] = observed[book->skills[hit.skill].name];
        if (!count++) hash = 14695981039346656037ull;
        hash = (hash ^ static_cast<uint64_t>(hit.frame)) * 1099511628211ull;
    }
    for (const auto &[name, row] : observed) std::cout << name << " " << row.first << " " << std::hex << row.second << std::dec << '\n';
}

void CheckInputReplay(std::string_view                       code,
                      std::span<const Button>                inputs,
                      int                                    haste,
                      tick_t                                 last,
                      std::string_view                       primary,
                      std::uint64_t                          expected_hash,
                      const std::map<std::string_view, int> &expected,
                      bool                                   default_loadout = false)
{
    if (!report_school.empty() && report_school != code) {
        return;
    }
    const auto &book = *s50::Find(code);
    s50::Config cfg;
    cfg.book             = &book;
    cfg.attributes       = book.default_attributes;
    cfg.attributes.haste = haste;
    cfg.precombat        = true;
    cfg.talents.assign(book.default_talents.begin(), book.default_talents.end());
    cfg.recipes.assign(book.default_recipes.begin(), book.default_recipes.end());
    if (!default_loadout) PublishedLoadout(cfg);
    std::string macro;
    for (const auto &button : inputs) {
        macro += "/scast " + std::string(button.name) + "\n";
    }
    auto [compiled, errors] = rt::MacroCompiler::Compile(macro, rt::CatalogMacroOptions(book.Catalog()));
    CHECK(errors.empty());
    auto replay = PreparePublishedReplay(cfg);
    s50::Action published_thread;
    if (code == "yly") {
        rt::Simulation<InputReplay> native(compiled.program, InputReplay(replay, inputs, true));
        double total = 0;
        const int runs = report_replays ? 128 : 1;
        for (int i = 0; i < runs; ++i) {
            native.Start(last + 1, rt::IterationSeed(42, i));
            native.Run();
            CHECK(native.GetRules().consumed() == inputs.size());
            CHECK(native.GetRules().first_rejection() == 294);
            CHECK(native.GetRules().rejected() == 5);
            total += native.TotalDamage();
        }
        if (report_replays) std::cout << "yly native: " << native.GetRules().rejected() << " unavailable buttons, first at frame 294, "
                                     << std::lround(total * 16 / (runs * last)) << " DPS\n";
        // The published simulator accepts 绊线 at eight 心络. Relax only its eligibility
        // in this second replay; its ten-point spend and all subsequent effects still run.
        auto slot = std::ranges::find(book.skills, "绊线", &rt::SkillDescriptor::name) - book.skills.begin();
        published_thread = *replay->actions[slot];
        published_thread.cost = 0;
        replay->actions[slot] = &published_thread;
    }
    if (code == "bxj") {
        // The online channel trace grants 枕上 after every eligible hit. Keep the native
        // damage rolls independent while conditioning only this haste/ignore window.
        for (auto &list : replay->post_damage) {
            for (auto &step : list) {
                if (step.op == s50::Op::AddOnCrit && book.buffs[step.slot].name == "待结算枕上") {
                    step.op = s50::Op::Add;
                }
            }
        }
    }
    rt::Simulation<InputReplay> sim(compiled.program, InputReplay(replay, inputs));
    sim.Start(last + 1, 42);
    sim.Run();
    CHECK(sim.GetRules().consumed() == inputs.size());
    if (code == "shxj") {
        static constexpr tick_t expected_wind[]{
#include "fixtures/shxj_wind_frames.inc"
        };
        std::vector<tick_t> wind;
        for (const auto &hit : sim.Log().Intents()) if (book.skills[hit.skill].name == "风矢") wind.push_back(hit.frame);
        auto diff = std::ranges::mismatch(wind, expected_wind);
        if (diff.in1 != wind.end() && diff.in2 != std::end(expected_wind))
            std::cerr << "风矢 " << *diff.in1 << " expected " << *diff.in2 << '\n';
        CHECK(std::ranges::equal(wind, expected_wind));
        const std::map<std::string_view, std::vector<tick_t>> pet_frames{
            {"攻击-虎", {16, 1662, 3057, 4662}}, {"攻击-鹰", {378, 1702, 3378, 4702}},
            {"重击", {699, 1983, 3699}}, {"践踏", {759, 2324, 3759}}, {"攻击-熊", {1020, 2625, 4020}},
            {"攻击-狼", {1341, 1346, 1351, 2665, 2670, 2675, 4341, 4346, 4351}}
        };
        for (const auto &[name, frames] : pet_frames) {
            std::vector<tick_t> actual;
            for (const auto &h : sim.Log().Intents()) if (book.skills[h.skill].name == name) actual.push_back(h.frame);
            CHECK(actual == frames);
        }
    }
    std::map<std::string_view, int> hits;
    for (const auto &hit : sim.Log().Intents()) {
        ++hits[book.skills[hit.skill].name];
    }
    // Check the ordered damage frames too. Reaching the right total with misplaced ticks would pass a count check.
    std::uint64_t frame_hash = 14695981039346656037ull;
    for (const auto &hit : sim.Log().Intents()) {
        if (book.skills[hit.skill].name == primary) {
            frame_hash = (frame_hash ^ static_cast<std::uint64_t>(hit.frame)) * 1099511628211ull;
        }
    }
    if (frame_hash != expected_hash) {
        std::cerr << code << " hits " << hits[primary] << " frame hash " << std::hex << frame_hash << std::dec << '\n';
    }
    CHECK(frame_hash == expected_hash);
    for (const auto &[name, count] : expected) {
        if (hits[name] != count) {
            std::cerr << code << " " << name << " expected " << count << ", got " << hits[name] << '\n';
            CHECK(false);
        }
    }
    if (report_replays) {
        constexpr int                                                       fights = 128;
        double                                                              damage = 0;
        std::map<std::string_view, std::pair<std::uint64_t, std::uint64_t>> totals;
        for (int i = 0; i < fights; ++i) {
            sim.Start(last + 1, rt::IterationSeed(42, i));
            sim.Run();
            damage += sim.TotalDamage([&](const rt::DamageIntent &h, value_t dealt) {
                auto &[count, amount] = totals[book.skills[h.skill].name];
                ++count;
                amount += dealt;
            });
        }
        std::cout << code << (default_loadout ? " (author build) " : " ") << last << " frames, " << primary << " " << hits[primary] << " hits, "
                  << std::lround(damage * 16 / (fights * last)) << " DPS (" << fights << " fights)\n";
        for (const auto &[name, values] : totals) {
            if (!report_school.empty() || name == primary) {
                std::cout << "  " << name << "  " << double(values.first) / fights << " hits, "
                          << std::lround(double(values.second) / values.first) << " per hit\n";
            }
        }
        if (report_school == "hjy") {
            const auto slot = std::ranges::find(book.skills, primary, &rt::SkillDescriptor::name) - book.skills.begin();
            auto old = std::make_shared<s50::Prepared>(*replay);
            old->passive[slot].pve -= 1690 - 1536; // Public 3.0.5 uses +150% NPC damage. Live talent uses +165%.
            rt::Simulation<InputReplay> legacy(compiled.program, InputReplay(old, inputs));
            std::map<int, std::pair<int, std::uint64_t>> bins;
            for (int i = 0; i < fights; ++i) {
                legacy.Start(last + 1, rt::IterationSeed(42, i));
                legacy.Run();
                legacy.TotalDamage([&](const rt::DamageIntent &h, value_t dealt) {
                    if (h.skill != slot) return;
                    auto &[count, amount] = bins[legacy.GetRules().Versions()[h.snapshot_version].attack];
                    ++count;
                    amount += dealt;
                });
            }
            for (const auto &[attack, bin] : bins)
                std::cout << "  public 150% NPC bonus, attack=" << attack << ": " << bin.first / fights << " hits, "
                          << std::lround(double(bin.second) / bin.first) << " per hit\n";
        }
        if (report_school == "bxj") {
            const auto slot = std::ranges::find(book.skills, primary, &rt::SkillDescriptor::name) - book.skills.begin();
            for (std::string_view aura : { "本次流玉", "霜降剑舞", "枕上", "化冰", "繁音急节", "剑舞长空" }) {
                auto  isolated = std::make_shared<s50::Prepared>(*replay);
                auto &mods     = isolated->modifiers[slot];
                std::erase_if(mods, [&](const auto &m) {
                    return m.aura != rt::INVALID_SLOT && book.buffs[m.aura].name == aura;
                });
                rt::Simulation<InputReplay> trial(compiled.program, InputReplay(isolated, inputs));
                std::uint64_t               count = 0, amount = 0;
                for (int i = 0; i < fights; ++i) {
                    trial.Start(last + 1, rt::IterationSeed(42, i));
                    trial.Run();
                    trial.TotalDamage([&](const rt::DamageIntent &h, value_t d) {
                        if (h.skill == slot) {
                            ++count;
                            amount += d;
                        }
                    });
                }
                CHECK(count == fights * hits[primary]);
                std::cout << "  without " << aura << " on " << primary << ": " << std::lround(double(amount) / count) << " per hit\n";
            }
        }
        if (code == "yjj") {
            rt::Simulation<InputReplay> native(compiled.program,
                                               InputReplay(std::make_shared<s50::Prepared>(*s50::Prepare(cfg)), inputs, true));
            double fruit = 0, missed = 0, total = 0;
            for (int i = 0; i < fights; ++i) {
                native.Start(last + 1, rt::IterationSeed(42, i));
                native.Run();
                CHECK(native.GetRules().consumed() == inputs.size());
                missed += native.GetRules().rejected();
                total  += native.TotalDamage();
                for (const auto &h : native.Log().Intents()) {
                    if (book.skills[h.skill].name == primary) {
                        ++fruit;
                    }
                }
            }
            std::cout << code << " independent rolls: " << fruit / fights << " hits, " << missed / fights
                      << " unavailable buttons, " << std::lround(total * 16 / (fights * last)) << " DPS\n";
        }
    }
}

void CheckWindAndMedicineTriggers()
{
    Fight wind("lhj", "/scast 振翅图南\n/scast 振翅图南\n/scast 击水三千\n/scast 击水三千");
    wind.cfg.talents = { "驰行", "驾鸾", "击浪" };
    wind.prepare();
    wind.run(260);
    auto ticks = wind.frames("振翅图南");
    CHECK(ticks.size() == 43 && ticks.front() == 8 && ticks.back() == 216);
    wind.cfg.target.distance = 31;
    wind.prepare();
    wind.run(260);
    CHECK(wind.frames("振翅图南").size() == 40);
    Fight bare("lhj", "/scast 振翅图南\n/cast 击水三千");
    bare.cfg.talents.clear();
    bare.prepare();
    bare.run(250);
    CHECK(bare.frames("振翅图南").size() == 12);

    Fight herb("wf", "/scast 千枝绽蕊\n/scast 香繁饮露\n/scast 川乌射罔");
    herb.cfg.initial_qidian = -5;
    herb.cfg.talents        = { "香繁饮露", "养荣" };
    herb.prepare();
    herb.run(28);
    CHECK((herb.frames("川乌射罔") == std::vector<tick_t>{ 9, 18, 27 }));
    CHECK(herb.hits("无方中和").size() == 10);
    herb.cfg.talents = { "香繁饮露" };
    herb.prepare();
    herb.run(28);
    CHECK(herb.hits("无方中和").size() == 5);
    Fight mist("wf", "/scast 香繁饮露\n/scast 沾衣未妨");
    mist.cfg.initial_qidian = 5;
    mist.prepare();
    mist.run(66);
    CHECK((mist.frames("沾衣未妨") == std::vector<tick_t>{ 17, 29, 41 }));
    CHECK(mist.hits("无方中和").size() == 5);
    Fight defense("wf", "/scast 川乌射罔\n/scast 钩吻断肠\n/scast 商陆缀寒");
    defense.cfg.talents = { "六微", "凄骨" };
    defense.prepare();
    defense.run(80);
    auto neutral_bonus = defense.hits("六微");
    CHECK(neutral_bonus.size() == 1);
    CHECK(defense.sim->GetRules().Versions()[neutral_bonus[0].live_version].ignore == 717);
}

void CheckBurnAndKarma()
{
    Fight burn("xcj", "/scast 龙跃于渊\n/scast 时乘六龙\n/scast 狂龙乱舞");
    burn.cfg.talents = { "雨龙" };
    burn.prepare();
    burn.run(460);
    // At frame 24 two of eleven ticks are consumed. The next natural tick is 72, not 48.
    CHECK((burn.frames("灼烧(DOT)") == std::vector<tick_t>{ 24, 24, 72, 120, 168, 216, 264, 312, 360, 408, 456 }));
    burn.cfg.talents.clear();
    burn.prepare();
    burn.run(460);
    CHECK((burn.frames("灼烧(DOT)") == std::vector<tick_t>{ 24, 24, 72, 120, 168 }));

    Fight refresh("xcj", "/scast 龙跃于渊\n/scast 龙跃于渊");
    refresh.cfg.talents = { "雨龙" };
    refresh.prepare();
    refresh.run(560);
    const auto topped_up = refresh.frames("灼烧(DOT)");
    CHECK(topped_up.size() == 11 && topped_up.front() == 48 && topped_up.back() == 528);

    Fight debt("xcj", "/cast 龙跃于渊");
    debt.cfg.recipes = { "龙跃于渊·无消耗内力" };
    debt.prepare();
    debt.run(560);
    CHECK((debt.frames("龙跃于渊·一") == std::vector<tick_t>{ 1, 25, 513, 537 }));

    Fight ordering("yjj", "/scast 韦陀献杵\n/scast 捉影式\n/scast 守缺式\n/scast 擒龙诀\n/scast 拿云式\n/scast 捕风式");
    ordering.cfg.talents     = { "众嗔", "业因" };
    ordering.cfg.target.life = 0.25;
    ordering.prepare();
    ordering.run(100);
    CHECK(ordering.hits("果报").size() == 1);
    CHECK(ordering.hits("降魔·拿云").size() == 1);
    CHECK(ordering.hits("降魔·清贪").size() == 2);
    auto slot = std::ranges::find(ordering.cfg.book->buffs, std::string_view{ "果报" }, &rt::BuffDescriptor::name) -
                ordering.cfg.book->buffs.begin();
    CHECK(ordering.sim->GetState().self_buff_stacks[slot] == 4);
    ordering.cfg.talents = { "众嗔" };
    ordering.prepare();
    ordering.run(100);
    CHECK(ordering.sim->GetState().self_buff_stacks[slot] == 2);
    ordering.cfg.talents = { "业因" };
    ordering.prepare();
    ordering.run(100);
    CHECK(ordering.hits("果报").empty());
}

void CheckPoisonAndIceWindows()
{
    Fight venom("dj", "/scast 蛇影\n/scast 蝎心\n/scast 蝎心\n/scast 蝎心");
    venom.cfg.talents = { "蛇悉", "残香", "释灵" };
    venom.cfg.recipes = { "蝎心·运功减2帧（一）", "蝎心·运功减2帧（二）" };
    venom.prepare();
    venom.run(97);
    CHECK((venom.frames("蝎心") == std::vector<tick_t>{ 24, 48, 96 }));
    CHECK((venom.frames("释灵(DOT)") == std::vector<tick_t>{ 40, 56, 72, 88 }));
    CHECK(venom.frames("幻击").empty());
    Fight charges("dj", "/scast 灵蛊\n/scast 灵蛊\n/scast 灵蛊");
    charges.cfg.talents = { "嗜蛊", "令怖" };
    charges.prepare();
    const auto gu = std::ranges::find(charges.cfg.book->skills, std::string_view{ "灵蛊" }, &rt::SkillDescriptor::name) -
                    charges.cfg.book->skills.begin();
    charges.run(320);
    CHECK(charges.sim->GetState().skill_energy[gu] == 0);
    charges.run(321);
    CHECK(charges.sim->GetState().skill_energy[gu] == 1);
    CHECK(charges.hits("蝎心").empty());

    Fight sacrifice("dj", "/scast 灵蛇引\n/scast 圣蝎引\n/scast 蛊虫献祭\n/scast 灵蛇引");
    sacrifice.cfg.talents.clear();
    sacrifice.prepare();
    sacrifice.run(394);
    auto slot = std::ranges::find(sacrifice.cfg.book->skills, std::string_view{ "灵蛇引" }, &rt::SkillDescriptor::name) -
                sacrifice.cfg.book->skills.begin();
    CHECK(sacrifice.sim->GetState().skill_ready_at[slot] == 394);
    CHECK(sacrifice.hits("幻击").empty() && sacrifice.hits("攻击").empty());
    sacrifice.run(449);
    CHECK((sacrifice.frames("攻击") == std::vector<tick_t>{ 448 }));

    // The first channel crosses the ten-second 流玉 expiry. Its entire eight-pulse cast keeps the bonus.
    Fight ice("bxj", "/scast 剑影留痕\n/scast 江海凝光\n/scast 江海凝光\n/scast 江海凝光\n/scast 江海凝光\n/scast 江海凝光\n/scast 江海凝光\n/scast 玳弦急曲\n/scast 玳弦急曲");
    ice.cfg.talents        = { "新妆", "流玉", "霜降" };
    ice.cfg.initial_qidian = 5;
    ice.prepare();
    ice.run(250);
    auto ticks = ice.hits("玳弦急曲");
    CHECK(ticks.size() == 11 && ticks[0].frame == 151 && ticks[7].frame == 200);
    for (size_t i = 0; i < ticks.size(); ++i) {
        const auto &v = ice.sim->GetRules().Versions()[ticks[i].snapshot_version];
        CHECK(v.damage == (i < 8 ? 922 : 0));
        const int base = ice.cfg.book->hits[ticks[i].skill].channel;
        CHECK(v.channel_override == (i < 8 ? base * 3 / 2 : -1));
    }
    Fight clipped("bxj", "/scast 剑影留痕\n/scast 玳弦急曲\n/fcast 江海凝光");
    clipped.cfg.talents        = { "霜降", "新妆", "流玉" };
    clipped.cfg.initial_qidian = 5;
    clipped.prepare();
    clipped.run(60);
    const auto latch = std::ranges::find(clipped.cfg.book->buffs, std::string_view{ "本次流玉" }, &rt::BuffDescriptor::name) -
                       clipped.cfg.book->buffs.begin();
    CHECK(clipped.hits("玳弦急曲").size() == 3);
    CHECK(clipped.hits("破·霜降").empty() && !clipped.sim->GetState().self_buff_stacks[latch]);

    for (int crit : { 0, 100000 }) {
        Fight pillow("bxj", "/scast 玳弦急曲", crit);
        pillow.cfg.talents = { "枕上" };
        pillow.prepare();
        pillow.run(46);
        const auto hits = pillow.hits("玳弦急曲");
        CHECK(hits.size() == 3);
        const auto &versions = pillow.sim->GetRules().Versions();
        CHECK(versions[hits[0].snapshot_version].ignore == 0);
        CHECK(versions[hits[1].snapshot_version].ignore == (crit ? 512 : 0));
    }
}

void CheckChargedArrows()
{
    Fight early("shxj", "/fcast [箭矢=6] 引风唤灵\n/cast 聚势摧霆");
    early.cfg.talents = {"聚势摧霆", "连珠"};
    early.prepare();
    early.run(36);
    CHECK((early.frames("聚势摧霆") == std::vector<tick_t>{10, 10}));
    CHECK(early.frames("聚势摧霆·蓄力") == early.frames("聚势摧霆"));
    CHECK((early.frames("风矢") == std::vector<tick_t>{0, 24}));
    CHECK(early.sim->GetState().qidian == 6);
}

void CheckStarRainPoison()
{
    Fight dry("tlgd", "/cast 蚀肌弹");
    dry.cfg.talents = { "化血迷心" };
    dry.prepare();
    dry.run(80);
    CHECK(!dry.hits("蚀肌弹").empty() && dry.hits("化血迷心").empty());

    Fight poison("tlgd", "/cast [tnobuff:化血] 天女散花\n/cast 星离雨散\n/cast 暴雨梨花针\n/cast 蚀肌弹");
    poison.cfg.talents = { "化血迷心", "星离雨散" };
    poison.prepare();
    poison.run(118);
    CHECK((poison.frames("天女散花") == std::vector<tick_t>{24}));
    CHECK((poison.frames("封喉") == std::vector<tick_t>{24}));
    CHECK((poison.frames("星离雨散") == std::vector<tick_t>{56}));
    CHECK((poison.frames("化血(DOT)") == std::vector<tick_t>{48, 96}));
    CHECK(poison.frames("化血迷心") == poison.frames("暴雨梨花针"));

    Fight acid("tlgd", "/cast [tnobuff:化血] 天女散花\n/cast 蚀肌弹");
    acid.cfg.talents = { "化血迷心" };
    acid.prepare();
    acid.run(121);
    CHECK(!acid.hits("蚀肌弹").empty() && acid.frames("化血迷心") == acid.frames("蚀肌弹"));
    CHECK((acid.frames("化血(DOT)") == std::vector<tick_t>{48, 96}));

    Fight burst("tlgd", "/cast 开火\n/cast [last_skill:开火] 天女散花\n/cast 千秋万劫");
    burst.cfg.talents = { "千秋万劫" };
    burst.prepare();
    burst.run(73);
    const auto ticks = burst.hits("化血(DOT)");
    CHECK(ticks.size() == 1 && ticks.front().frame == 48 && ticks.front().sub == 8);
}

// Star Rain, retail 1031. The public JCL exports six buff combinations for 75 rank-four hits.
// This fixture weights independently simulated volleys; it does not supply a fictional button timeline.
void CheckStarRainBolts(bool report = false)
{
    Fight fight("tlgd", "/cast [神机<0] 蚀肌弹");
    const auto &b = *fight.cfg.book;
    fight.cfg.attributes = b.default_attributes;
    fight.cfg.talents.assign(b.default_talents.begin(), b.default_talents.end());
    fight.cfg.recipes.assign(b.default_recipes.begin(), b.default_recipes.end());
    struct Window { int count, spirit, rain; bool attack, focus; };
    // The 39+15 and 12+3 rows carry identical tags. All 75 include 擘两分星.
    constexpr Window windows[]{ {54,5,4,true,true}, {15,5,4,false,false}, {5,0,0,true,true}, {1,1,1,true,true} };
    const auto aura = [&](std::string_view n) { return static_cast<rt::Slot>(std::ranges::find(b.buffs, n, &rt::BuffDescriptor::name) - b.buffs.begin()); };
    const auto second = std::ranges::find(b.skills, "重弩·二", &rt::SkillDescriptor::name) - b.skills.begin();
    const auto ordinary = std::ranges::find(b.skills, "重弩·一", &rt::SkillDescriptor::name) - b.skills.begin();
    // Exactly eight yards is close. Re-entering long range cancels a pending expiry.
    fight.cfg.target.distance = 8;
    fight.cfg.target_events = { { 16, {}, 801 }, { 24, {}, 800 }, { 100, {}, 1200 }, { 160, {}, 400 } };
    fight.prepare();
    for (auto [frames, expected] : { std::pair{16, 0}, {17, 1}, {185, 1}, {320, 1}, {321, 0} }) {
        fight.run(frames);
        CHECK(fight.sim->GetState().self_buff_stacks[aura("秋风散影")] == expected);
    }
    fight.cfg.target_events.clear();
    for (bool horizontal : {false, true}) {
        auto cfg = fight.cfg;
        if (!horizontal) std::erase(cfg.talents, "横断天枢");
        auto prepared = std::make_shared<s50::Prepared>(*s50::Prepare(cfg));
        prepared->resources[0].initial = 0;
        prepared->opening = { {s50::Op::Add, aura("重弩")}, {s50::Op::Add, aura("机关注能"), 9}, {s50::Op::Add, aura("鬼斧弹药")} };
        rt::Simulation<s50::Rules> sim(fight.macro.program, s50::Rules(prepared));
        sim.Start(81, 42);
        sim.Run();
        const auto &hits = sim.Log().Intents();
        CHECK(hits.size() == 2 && hits[0].skill == second && hits[1].skill == ordinary);
        CHECK(hits[0].frame == 32 && hits[1].frame == 80 && hits[0].level == (horizontal ? 2 : 1));
    }
    double weighted = 0;
    for (const auto &w : windows) {
        auto prepared = std::make_shared<s50::Prepared>(*s50::Prepare(fight.cfg));
        prepared->resources[0].initial = 20;
        const auto add = [&](std::string_view n, int count = 1) { if (count) prepared->opening.push_back({s50::Op::Add,aura(n),count}); };
        add("机关存在"); add("重弩"); add("机关注能", 10); add("鬼斧弹药", 2); add("专注"); add("秋风散影");
        add("气魄", w.spirit); add("汲雨", w.rain); add("扬威", w.attack); add("心无旁骛", w.focus);
        rt::Simulation<s50::Rules> sim(fight.macro.program, s50::Rules(prepared));
        constexpr int iterations = 2048;
        std::uint64_t total = 0;
        value_t min_normal = 1LL << 40, max_critical = 0;
        for (int i = 0; i < iterations; ++i) {
            sim.Start(33, rt::IterationSeed(42, i)); sim.Run();
            const auto &hits = sim.Log().Intents();
            CHECK(hits.size() == 3 && hits[0].skill == second && hits[1].skill == second && hits[2].skill == ordinary);
            for (const auto &h : hits) { CHECK(h.frame == 32 && h.level == 4); if (h.skill == second) {
                auto value = sim.GetRules().Reduce(h); total += value;
                if (h.outcome == RollResult::DOUBLE) { max_critical = std::max(max_critical, value); }
                else min_normal = std::min(min_normal, value);
            } }
        }
        if (report) std::cout << w.count << " shots: " << min_normal << " normal, " << max_critical << " critical, average " << total / (2 * iterations) << "\n";
        weighted += w.count * double(total) / (2 * iterations);
    }
    CHECK(std::abs(weighted / 75 - 20725) < 150);
    if (report) std::cout << "75 weighted rank-four bolts, average " << std::lround(weighted / 75) << " (online 20725)\n";
}

void CheckPublishedInputs()
{
    static constexpr Button mw[]{
#include "fixtures/mw_inputs.inc"
    };
    CheckInputReplay("mw", mw, 445, 4478, "放鹿", 0x0b6c6160e7a1a789ull,
                     { { "放鹿", 88 }, { "阳春白雪·曲", 13 }, { "斩长鲸", 8 }, { "徵", 15 }, { "徵·凌冬", 90 }, { "九衢闻曲", 24 } });
    static constexpr Button hjy[]{
#include "fixtures/hjy_inputs.inc"
    };
    CheckInputReplay("hjy", hjy, 928, 4796, "临源·一", 0x5f5060a002dc23cull,
                     { { "临源·一", 26 }, { "阳明指", 75 }, { "玉石俱焚", 40 }, { "渲青", 10 } });
    static constexpr Button txj[]{
#include "fixtures/txj_inputs.inc"
    };
    CheckInputReplay("txj", txj, 0, 4797, "世降", 0x1f1f929155f27ae9ull,
                     { { "世降", 213 }, { "灵器", 284 }, { "往者定", 17 } });
    static constexpr Button yly[]{
#include "fixtures/yly_inputs.inc"
    };
    CheckInputReplay("yly", yly, 928, 3246, "勾线", 0x9daeeef0b31af6e9ull,
                     { { "勾线", 161 }, { "谐调", 17 }, { "生地狱", 18 }, { "绊线", 68 } });
    static constexpr Button shxj[]{
#include "fixtures/shxj_inputs.inc"
    };
    CheckInputReplay("shxj", shxj, 928, 4894, "聚势摧霆", 0x7587606d7f2e4d45ull,
                     { { "聚势摧霆", 160 }, { "聚势摧霆·蓄力", 8 }, { "领胡", 23 }, { "饮羽簇", 157 } });
    static constexpr Button zxg[]{
#include "fixtures/zxg_inputs.inc"
    };
    CheckInputReplay("zxg", zxg, 928, 4698, "颠越苍穹击", 0xf521ff3e4b64ea57ull,
                     { { "颠越苍穹击", 154 }, { "无外", 49 }, { "破势", 15 } });
    static constexpr Button gfj[]{
#include "fixtures/gfj_inputs.inc"
    };
    CheckInputReplay("gfj", gfj, 978, 4867, "孤锋破浪", 0xc89ae0d19159478full,
                     { { "孤锋破浪", 268 }, { "滔天", 268 }, { "行云势·三", 37 } });
    static constexpr Button fysj[]{
#include "fixtures/fysj_inputs.inc"
    };
    CheckInputReplay("fysj", fysj, 10, 4627, "崇光斩恶", 0x436066832641fb10ull,
                     { { "崇光斩恶", 38 }, { "崇光斩恶·月破魔", 36 }, { "驱夜断愁", 52 } });
    CheckInputReplay("fysj", fysj, 10, 4627, "崇光斩恶", 0x436066832641fb10ull,
                     { { "崇光斩恶", 38 }, { "崇光斩恶·月破魔", 36 }, { "驱夜断愁", 52 } }, true);
    static constexpr Button ztg[]{
#include "fixtures/ztg_inputs.inc"
    };
    CheckInputReplay("ztg", ztg, 1452, 5106, "风籁", 0x7298bae8a25283fdull, { { "风籁", 324 }, { "抟风令", 12 }, { "引窍", 33 } });
    static constexpr Button axzy[]{
#include "fixtures/axzy_inputs.inc"
    };
    CheckInputReplay("axzy", axzy, 928, 4806, "龙牙", 0x5150dd48abe2e82cull,
                     { { "龙牙", 89 }, { "破军", 333 }, { "破罡", 89 }, { "夜征", 48 }, { "灭·破楼兰·二", 58 }, { "龙牙·龙血", 89 } });
    static constexpr Button baj[]{
#include "fixtures/baj_inputs.inc"
    };
    CheckInputReplay("baj", baj, 445, 4809, "破釜沉舟", 0x6df1132a3f7f477dull,
                     { { "破釜沉舟", 80 }, { "楚歌", 80 }, { "上将军印·见尘", 84 }, { "碎江天", 51 }, { "碎影", 51 }, { "惊燕式·一", 20 }, { "起凤式·二", 20 }, { "坚壁清野·残留", 100 } });
    static constexpr Button wind[]{
#include "fixtures/lhj_inputs.inc"
    };
    static constexpr Button medicine[]{
#include "fixtures/wf_inputs.inc"
    };
    static constexpr Button xcj[]{
#include "fixtures/xcj_inputs.inc"
    };
    static constexpr Button dj[]{
#include "fixtures/dj_inputs.inc"
    };
    CheckInputReplay(
        "dj",
        dj,
        928,
        4366,
        "蝎心",
        0xaae8fca023e50eebull,
        {
            { "蝎心",      136 },
            { "不鸣",      10  },
            { "释灵(DOT)", 270 },
            { "虫魄",      100 },
            { "令怖",      50  },
            { "残香",      50  },
            { "攻击",      68  },
            { "幻击",      22  },
            { "百足(DOT)", 265 },
            { "蛇影(DOT)", 264 },
            { "蟾啸(DOT)", 271 }
    });
    static constexpr Button bxj[]{
#include "fixtures/bxj_inputs.inc"
    };
    CheckInputReplay(
        "bxj",
        bxj,
        948,
        4952,
        "玳弦急曲",
        0x8f08c363f2387ecfull,
        {
            { "玳弦急曲",      368 },
            { "玳弦急曲·一",   368 },
            { "化冰",          356 },
            { "钗燕",          94  },
            { "留芳仙姿·玳弦", 271 },
            { "留芳仙姿·剑破", 46  },
            { "剑破虚空",      65  }
    });
    CheckInputReplay("xcj",
                     xcj,
                     445,
                     3015,
                     "灼烧(DOT)",
                     0xe30e211e32d81d9bull,
                     {
                         { "灼烧(DOT)",   100 },
                         { "苍龙",        64  },
                         { "落水·醉影",   35  },
                         { "祭湘君",      72  },
                         { "灵隼击",      88  },
                         { "狂龙乱舞·一", 25  }
    });
    static constexpr Button yjj[]{
#include "fixtures/yjj_inputs.inc"
    };
    CheckInputReplay("yjj",
                     yjj,
                     928,
                     4813,
                     "果报",
                     0x96eee06acc5b1084ull,
                     {
                         { "果报",          72  },
                         { "降魔·清贪",     121 },
                         { "拿云式·三",     97  },
                         { "布泽",          52  },
                         { "横扫六合(DOT)", 320 },
                         { "金刚龙爪功",    152 }
    });
    CheckInputReplay(
        "lhj",
        wind,
        474,
        4787,
        "振翅图南",
        0xf146fbdbc36f7a28ull,
        {
            { "振翅图南",    859 },
            { "青冥(DOT)",   263 },
            { "风驰",        37  },
            { "天行",        49  },
            { "翼绝云天",    35  },
            { "浮游天地",    18  },
            { "逐波灵游",    36  },
            { "溟海御波",    36  },
            { "海运南冥·近", 72  },
            { "击水三千·一", 35  },
            { "击水三千·二", 34  },
            { "击水三千·三", 18  },
            { "木落雁归",    18  }
    });
    CheckInputReplay(
        "wf",
        medicine,
        928,
        4813,
        "无方中和",
        0x278dbdf3979c4044ull,
        {
            { "无方中和",  407 },
            { "紫伏",      407 },
            { "六微",      51  },
            { "逆乱(DOT)", 165 },
            { "商陆缀寒",  149 },
            { "川乌射罔",  169 },
            { "凄骨",      45  },
            { "沾衣未妨",  85  },
            { "苍棘缚地",  135 },
            { "钩吻断肠",  23  }
    });
}
} // namespace

int main(int argc, char **argv)
{
    if (argc == 2 && std::string_view(argv[1]) == "--report-crossbow") { CheckStarRainBolts(true); return 0; }
    if (argc == 6 && std::string_view(argv[1]) == "--inspect-inputs") {
        InspectInputs(argv[2], std::stoi(argv[3]), std::stoi(argv[4]), argv[5]);
        return 0;
    }
    if ((argc == 2 || argc == 3) && std::string_view(argv[1]) == "--report-replays") {
        report_replays = true;
        if (argc == 3) {
            report_school = argv[2];
        }
        CheckPublishedInputs();
        return 0;
    }
    CheckStarRainBolts();
    CheckStarRainPoison();
    CheckChargedArrows();
    CheckPoisonAndIceWindows();
    CheckBurnAndKarma();
    CheckWindAndMedicineTriggers();
    CheckPublishedInputs();
    CheckYouLuoPuppet();
    CheckAllBooks();
    CheckDotAndCastOrder();
    CheckDamageHooksAndDotSnapshot();
    CheckTaiXuTalentWindows();
    CheckConditionalTalentEffects();
    CheckNovemberChanges();
    CheckResources();
    CheckUpdatedLegacySchools();
    CheckEquipmentEvents();
    CheckRaidGear();
    CheckLegendaryWeapons();
    CheckInstanceEquipment();
    CheckAdaptiveEquipmentGains();
    CheckFullRaid();
    CheckSpeedDotAndTrialGear();
    CheckShieldBash();
    CheckGeneralSealAndDragonProcs();
    CheckPermanents();
    CheckRecipeScope();
    CheckOnlineHitValues();
    CheckOldVersion4Coefficients();
    CheckPresetMechanics();
    CheckFenShanRage();
    CheckRotationTriggers();
}
