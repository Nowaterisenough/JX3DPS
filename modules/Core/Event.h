/**
 * @Description : 
 * @Author      : NoWats
 * @Date        : 2022-02-04 12:07:17
 * @Update      : NoWats
 * @LastTime    : 2022-02-04 13:15:08
 * @FilePath    : \JX3DPS\modules\Core\Event.h
 */

#ifndef EVENT_H
#define EVENT_H

#include "Core/Global.h"

namespace JX3DPS {

class Event
{
public:
    Event(const std::list<std::string> &lines);
    ~Event();

    void    Cast();
    void    UpdateTime(const Frame_t frames);
    Frame_t GetNextTime();

    public_var : Events events;

private:
    int ParseEvent(const std::string &line);

    // 增加目标
    // [00:00] /add 0 102 6800
    void ParseAddTarget(Frame_t frames, const std::string &line);
    void AddTarget(const Param &param);

    // 转火目标
    // [00:00] /target 1
    void ParseChangeTarget(Frame_t frames, const std::string &line);
    void ChangeTarget(const Param &param);

    // 目标死亡
    // [00:00] /delete 1
    void ParseDeleteTarget(Frame_t frames, const std::string &line);
    void DeleteTarget(const Param &param);

    // 目标远离
    // [00:00] /detach 2
    void ParseDetachTarget(Frame_t frames, const std::string &line);
    void DetachTarget(const Param &param);

    // 目标回归
    // [00:00] /recovery 2
    void ParseRecoveryTarget(Frame_t frames, const std::string &line);
    void RecoveryTarget(const Param &param);

    // 停手
    // [00:00] /stop
    void ParseStop(Frame_t frames, const std::string &line);
    void Stop(const Param &param);

    // 继续
    // [00:00] /continue
    void ParseContinue(Frame_t frames, const std::string &line);
    void Continue(const Param &param);

    // 强制执行技能
    // [00:00] /cast ...
    void ParseFcast(Frame_t frames, const std::string &line);

    // 执行优先级技能
    // [00:00] /fcast ...
    void ParseCast(Frame_t frames, const std::string &line);

    // 添加buff
    // [00:00] /refresh target 1 叠刃
    void ParseRefreshBuff(Frame_t frames, const std::string &line);
    void RefreshBuff(const Param &param);
    void RefreshTBuff(const Param &param);

    // 清除buff
    // [00:00] /clean target 1 叠刃
    void ParseCleanBuff(Frame_t frames, const std::string &line);
    void CleanBuff(const Param &param);
    void CleanTBuff(const Param &param);

    // 设置
    // [00:00] /set target 1 tlife 0.8
    void ParseSet(Frame_t frames, const std::string &line);
    void Set(const Param &param);

    private_var : Player *m_player     = nullptr;
    TargetsMap           *m_targetsMap = nullptr;
    Stats                *m_stats      = nullptr;
    Settings             *m_settings   = nullptr;
    Macro                *m_macro      = nullptr;
};

} // namespace JX3DPS

#endif // EVENT_H
