/**
 * @Description : JX3DPS APP 主函数
 * @Author      : NoWats
 * @Date        : 2022-01-26 22:28:25
 * @Update      : NoWats
 * @LastTime    : 2022-02-02 15:13:25
 * @FilePath    : \JX3DPS\app\main.cpp
 */

#pragma execution_character_set("utf-8")

#include <iostream>
#include <random>

#include "Version.h"
#include "Global.h"
#include "Common.h"
#include "Attr.h"

class wwwj
{
public:
    int GetNextFrames() const
    {
        return frames;
    }

    void UpdateFrames(int frames)
    {
        this->frames -= frames;
        if (this->frames < 0) {
            this->frames = 0;
        }
    }

    void Cast()
    {
        frames = s_frames;
        // std::cout << "5" << std::endl;
    }

private:
    static const int s_frames = 24;
    int              frames   = 0;
};

class bhgy
{
public:
    int GetNextFrames() const
    {
        return frames;
    }

    void UpdateFrames(int frames)
    {
        this->frames -= frames;
        if (this->frames < 0) {
            this->frames = 0;
        }
    }

    void Cast()
    {
        frames = s_frames;
        // std::cout << "8" << std::endl;
    }

private:
    static const int s_frames = 96;
    int              frames   = 0;
};
#include <regex>
int main(int argc, char *argv[])
{
    std::cout << JX3DPS::BRANCH << std::endl;
    std::cout << JX3DPS::VERSION << std::endl;

    std::string str{"tbuff:叠刃>4|tbuff:"};
    std::regex  reg("(tbuff:叠刃>4|tbuff:)"); //("([a-z_]+):(.+)(>=|<=|~=|<|>|=)([0-9]+)");

    std::smatch mat;
    if (std::regex_match(str, mat, reg)) {
        std::cout << mat[0];
    } else {
        std::cout << "none" << str;
    }

    return 0;

    wwwj w;
    bhgy b;
    int  qidian = 10;
    int  sumW   = 0;
    int  sumB   = 0;
    int  frames = 16 * 60 * 10 * 10000;
    while (frames > 0) {
        if (b.GetNextFrames() == 0 && qidian >= 7) {
            b.Cast();
            w.UpdateFrames(24);
            frames -= 24;
            qidian = 0;
            sumB++;
        } else if (w.GetNextFrames() == 0) {
            w.Cast();
            if (JX3DPS::Random(0.0, 100.0) < 0.7) {
                b.UpdateFrames(16);
                qidian += 2;
            }
            b.UpdateFrames(24);
            frames -= 24;
            qidian += 3;
            sumW++;
        } else {
            int nextFrames =
                b.GetNextFrames() <= w.GetNextFrames() ? b.GetNextFrames() : w.GetNextFrames();
            b.UpdateFrames(nextFrames);
            w.UpdateFrames(nextFrames);
            frames -= nextFrames;
            qidian += nextFrames;
        }
    }
    JX3DPS::Attr attr(JX3DPS::Class::TAI_XU_JIAN_YI);
    // std::cout << attr.GetAgilityBase();
    // float sum  = 0.0;
    // int   sumI = 0;
    // for (int i = 0; i < 10000; ++i) {
    //     sum += JX3DPS::Random(0.0, 100.0);
    //     sumI += JX3DPS::Random(0, 1024);
    // }
    // std::cout << sumB << std::endl << sumW << std::endl;
    return 0;
}
