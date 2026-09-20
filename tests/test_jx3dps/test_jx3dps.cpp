/**
 * Project: JX3DPS
 * File: test_jx3dps.cpp
 * Description:
 * Created Date: 2023-07-27 20:53:06
 * Author: 难为水
 * -----
 * Last Modified: 2023-07-30 12:17:25
 * Modified By: 难为水
 * -----
 * CHANGELOG:
 * Date      	By     	Comments
 * ----------	-------	----------------------------------------------------------
 */

#include <fstream>
#include <string_view>

#include <gtest/gtest.h>
#include <nlohmann/json.hpp>

#include "jx3dps.h"
#include "src/class/tai_xu_jian_yi/tai_xu_jian_yi_data.h"
#include "src/core/context.h"
#include "src/equipment/equipment.hpp"
#include "src/macro/macro_interpreter.h"
#include "src/version.h"

int TestJX3DPSTaiXuJianYi()
{
    nlohmann::ordered_json  json;
    std::ifstream          ifs("test_tai_xu_jian_yi.json");
    ifs >> json;
    ifs.close();

    char *buffer = new char[1024 * 1024]();

    int err = jx3dps_simulate(json.dump().c_str(), buffer, nullptr, nullptr);

    delete[] buffer;

    return err;
}

TEST(JX3DPS, TaiXuJianYi)
{
    int err = TestJX3DPSTaiXuJianYi();
    EXPECT_EQ(err, 0);
}

TEST(JX3DPS, TaiXuJianYiCurrentData)
{
    using namespace JX3DPS;
    using namespace JX3DPS::太虚剑意;

    EXPECT_EQ(JX3_LEVEL_VERSION, 50);
    EXPECT_EQ(JX3_VERSION, JX3Version::CANG_SHENG_ZHU_SHI);
    EXPECT_NEAR(Data::ATTRIBUTE_SCALE.critical_strike, 95.1291, 1e-4);
    EXPECT_NEAR(Data::ATTRIBUTE_SCALE.overcome, 103.7817, 1e-4);
    EXPECT_NEAR(JX3_CRITICAL_STRIKE_PARAM * JX3_PLAYER_LEVEL, 95.1291, 1e-4);
    EXPECT_NEAR(JX3_OVERCOME_PARAM * JX3_PLAYER_LEVEL, 103.7817, 1e-4);
    EXPECT_NEAR(JX3_HASTE_RATE * JX3_PLAYER_LEVEL, 101.079, 1e-4);
    ASSERT_NE(Data::FindSkillCoefficient("三环套月"), nullptr);
    EXPECT_NEAR(Data::FindSkillCoefficient("三环套月")->attack, 0.307199, 1e-6);
    ASSERT_NE(Data::FindSkillCoefficient("叠刃"), nullptr);
    EXPECT_NEAR(Data::FindSkillCoefficient("叠刃")->attack, 0.021893, 1e-6);
}

TEST(JX3DPS, TaiXuJianYiApril2026Archive)
{
    using namespace JX3DPS::太虚剑意::April2026;

    EXPECT_STREQ(JX3DPS::太虚剑意::Data::APRIL_2026_DATA_VERSION, DATA_VERSION);
    EXPECT_EQ(JX3DPS::太虚剑意::Data::APRIL_2026_TALENT_VERSION, TALENT_VERSION);
    EXPECT_STREQ(DATA_VERSION, "2026-04-27");
    EXPECT_STREQ(RELEASE, "暗影千机");
    EXPECT_EQ(TALENT_VERSION, 20260427);
    EXPECT_EQ(OFFICIAL_CHANGELOG_ID, 91);
    EXPECT_EQ(APRIL_PATCH_ID, 1335493);
    ASSERT_EQ(TALENTS.size(), 26u);

    size_t passive_count = 0;
    size_t active_skill_count = 0;
    bool has_stack_talent = false;
    bool has_sword_potential = false;
    bool has_dao_ji = false;
    for (const auto &talent : TALENTS) {
        if (std::string_view(talent.type) == "talent") {
            ++passive_count;
        } else if (std::string_view(talent.type) == "skill") {
            ++active_skill_count;
        }
        has_stack_talent = has_stack_talent || (talent.id == 2689);
        has_sword_potential = has_sword_potential || (talent.id == 64136);
        has_dao_ji = has_dao_ji || (talent.id == 65187);
    }
    EXPECT_EQ(passive_count, 23u);
    EXPECT_EQ(active_skill_count, 3u);
    EXPECT_TRUE(has_stack_talent);
    EXPECT_TRUE(has_sword_potential);
    EXPECT_TRUE(has_dao_ji);
}

TEST(JX3DPS, BuffPresenceMatchesPositiveStackCondition)
{
    using namespace JX3DPS;

    context.Reset();
    MacroInterpreter interpreter;
    interpreter.SetBuffIdGetter([](const std::string &) { return 23367; });
    interpreter.SetSkillIdGetter([](const std::string &) { return 365; });

    auto [exists, exists_error] = interpreter.ParseMacro("/cast [buff:玄门] 无我无剑");
    auto [positive, positive_error] = interpreter.ParseMacro("/cast [buff:玄门>0] 无我无剑");
    ASSERT_EQ(exists_error, ParserError::SUCCESS);
    ASSERT_EQ(positive_error, ParserError::SUCCESS);

    context.SetBuffStack(0, 23367, 1);
    EXPECT_TRUE(exists->Evaluate());
    EXPECT_TRUE(positive->Evaluate());

    context.SetBuffStack(0, 23367, 0);
    EXPECT_FALSE(exists->Evaluate());
    EXPECT_FALSE(positive->Evaluate());
}

TEST(JX3DPS, EquipmentSnapshotUsesJx3BoxFields)
{
    using namespace JX3DPS;
    const nlohmann::json item = {
        {"id", "8_42406"}, {"source_id", 42406}, {"name", "雷鸣风怒·灵曜戒"},
        {"icon_id", 22894}, {"type", "戒指"}, {"quality", 4},
        {"attributes", nlohmann::json::array({
            {{"type", "atAgilityBase"}, {"label", "身法提高607"}},
            {{"type", "atPhysicsAttackPowerBase"}, {"label", "外功攻击提高1195"}},
            {{"type", "atPhysicsOvercomeBase"}, {"label", "外功破防等级提高4396"}}
        })},
        {"set", {{"name", "雷鸣风怒·灵曜"}, {"attributes", {{"2", "无双等级提高100"}}}}}
    };
    const auto loadout = EquipmentLoadout::FromJson({
        {"equipment", {{"items", nlohmann::json::array({item, item})}}}
    });
    Attribute attribute;
    loadout.ApplyTo(attribute);

    EXPECT_TRUE(loadout.equipped[static_cast<size_t>(EquipmentSlot::RING_1)]);
    EXPECT_TRUE(loadout.equipped[static_cast<size_t>(EquipmentSlot::RING_2)]);
    EXPECT_EQ(loadout.runtime.icons[static_cast<size_t>(EquipmentSlot::RING_1)], 22894);
    EXPECT_EQ(attribute.Get(Attribute::Type::AGILITY_BASE), 1214);
    EXPECT_EQ(attribute.Get(Attribute::Type::PHYSICS_ATTACK_POWER_BASE), 2390);
    EXPECT_EQ(attribute.Get(Attribute::Type::PHYSICS_OVERCOME_BASE), 8792);
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
