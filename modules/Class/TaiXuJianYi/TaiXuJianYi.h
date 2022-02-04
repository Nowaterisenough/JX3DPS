#ifndef TAIXUJIANYI_H
#define TAIXUJIANYI_H

#include "Core/Player.h"
#include "Core/Global.h"

namespace JX3DPS {

namespace TaiXuJianYi {

SkillMap InitSkillTaiXuJianYi();
BuffMap InitBuffTaiXuJianYi();

class TaiXuJianYi : public Player
{
public:
    TaiXuJianYi();
    ~TaiXuJianYi();

    /* 气点 */
    int UpdateQidian(int qidian = 0);
    int GetQidian();

    std::vector<Talent> m_talent;

private:
    /* 初始化 */
    void InitClass();

    /* 身法 */
    void UpdateAgility();

    int m_qidian;
    int m_qidianBefore;

};

}

}

#endif // TAIXUJIANYI_H
