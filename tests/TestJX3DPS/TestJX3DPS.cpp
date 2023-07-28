/**
 * Project: 
 * File: TestJX3DPS.cpp
 * Description: 
 * Created Date: 2023-07-27 20:53:06
 * Author: 难为水
 * -----
 * Last Modified: 2023-07-28 11:11:02
 * Modified By: 难为水
 * -----
 * CHANGELOG:
 * Date      	By     	Comments
 * ----------	-------	----------------------------------------------------------
 */



#include <fstream>

#include <gtest/gtest.h>
#include <nlohmann/json.hpp>

#include "JX3DPS.h"

int TestJX3DPSTaiXuJianYi()
{
    nlohmann::ordered_json json;
    std::ifstream  ifs("TestTaiXuJianYi.json");
    ifs >> json;
    ifs.close();

    char *buffer = new char[1024 * 1024 * 1024];

    int err = JX3DPSSimulate(json.dump().c_str(), buffer, nullptr, nullptr);

    delete buffer;

    return err;
}

TEST(JX3DPS, TaiXuJianYi)
{
    int err = TestJX3DPSTaiXuJianYi();
    EXPECT_EQ(err, 2);
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}