/**
 * Project: JX3DPS
 * File: TimeLine.h
 * Description:
 * Created Date: 2023-09-13 03:02:14
 * Author: 难为水
 * -----
 * Last Modified: 2023-09-15 06:01:02
 * Modified By: 难为水
 * -----
 * HISTORY:
 * Date      	By   	Comments
 * ----------	-----	----------------------------------------------------------
 */

#ifndef JX3DPS_TIME_LINE_H_
#define JX3DPS_TIME_LINE_H_

#include "Expression.h"

namespace JX3DPS {

class TimeLine
{
public:
    struct Info
    {
        std::string name;
        RollResult  rollResult;
        int         damage;
        NodeType    type;
    };

    using InfosList = std::list<std::pair<Frame_t, std::list<Info>>>;

public:
    static TimeLine &Instance()
    {
        static TimeLine instance;
        return instance;
    }

    inline static void SetMode(Options::Mode mode)
    {
        s_recordFunc       = RecordSkillVoid_;
        s_addTimeStampFunc = AddTimeStampVoid_;
        if (mode == Options::Mode::DEBUG) {
            s_recordFunc       = RecordSkill_;
            s_addTimeStampFunc = AddTimeStamp_;
        }
        Instance().Clear();
    }

    inline static void RecordSkill(const std::string &name, RollResult rollResult, int damage, NodeType type)
    {
        s_recordFunc(name, rollResult, damage, type);
    }

    inline static void AddTimeStamp(Frame_t frame) { s_addTimeStampFunc(frame); }

    InfosList &GetInfosList() { return m_infosList; }

    // 禁止拷贝构造函数和赋值运算符
    TimeLine(const TimeLine &)            = delete;
    TimeLine &operator=(const TimeLine &) = delete;

    inline static void RecordSkill_(const std::string &name, RollResult rollResult, int damage, NodeType type)
    {
        Info info;
        info.name       = name;
        info.rollResult = rollResult;
        info.damage     = damage;
        info.type       = type;
        Instance().GetInfosList().back().second.push_back(info);
    }

    inline static void RecordSkillVoid_(const std::string &name, RollResult rollResult, int damage, NodeType type)
    {
    }

    inline static void AddTimeStamp_(Frame_t frame)
    {
        if (Instance().GetInfosList().empty() || Instance().GetInfosList().back().first != frame)
        {
            Instance().GetInfosList().push_back(std::make_pair(frame, std::list<Info>()));
        }
    }

    inline static void AddTimeStampVoid_(Frame_t frame) { }

    static std::function<void(const std::string &name, RollResult rollResult, int damage, NodeType type)> s_recordFunc;

    static std::function<void(Frame_t frame)> s_addTimeStampFunc;

    void Clear() { m_infosList.clear(); }

private:

    TimeLine() { }

    ~TimeLine() { }

    InfosList m_infosList;
};

} // namespace JX3DPS

#endif // JX3DPS_TIME_LINE_H_