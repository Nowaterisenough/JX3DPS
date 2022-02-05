#include "Event.h"

#include <sstream>

#include "Core/Target.h"
#include "Core/Player.h"
#include "Core/Buff.h"
#include "Core/Macro.h"

namespace JX3DPS {

Event::Event(const std::list<std::string> &lines)
{
    for (auto it : lines) {
        ParseEvent(it);
    }
}

Event::~Event()
{

}

void Event::Cast()
{
    for (auto it : events) {
        if (it.first == 0) {
            (this->*it.second.eventFuncPtr)(it.second.param);
        } else {
            break;
        }
    }
}

void Event::UpdateTime(Frame_t frames)
{
    for (auto it : events) {
        it.first -= frames;
    }
}

Frame_t Event::GetNextTime()
{
    return events.front().first;
}

int Event::ParseEvent(const std::string &line)
{
    int minutes = stoi(line.substr(1, 2));
    int seconds = stoi(line.substr(4, 5));
    Frame_t time = (minutes * 60 + seconds) * 16;
    std::string instTpye = line.substr(9, 11);
    std::string instruction = line.substr(8);
    if (instTpye == "add") {
        ParseAddTarget(time, instruction);
    } else if (instTpye == "tar") {
        ParseChangeTarget(time, instruction);
    } else if (instTpye == "del") {
        ParseDeleteTarget(time, instruction);
    } else if (instTpye == "det") {
        ParseDetachTarget(time, instruction);
    } else if (instTpye == "rec") {
        ParseRecoveryTarget(time, instruction);
    } else if (instTpye == "sto") {
        ParseStop(time, instruction);
    } else if (instTpye == "con") {
        ParseContinue(time, instruction);
    } else if (instTpye == "cas") {
        ParseCast(time, instruction);
    } else if (instTpye == "fca") {
        ParseFcast(time, instruction);
    } else if (instTpye == "ref") {
        ParseRefreshBuff(time, instruction);
    } else if (instTpye == "cle") {
        ParseCleanBuff(time, instruction);
    } else if (instTpye == "set") {
        ParseSet(time, instruction);
    }
    return 0;
}

void Event::ParseAddTarget(Frame_t frames, const std::string &line)
{
    std::stringstream buf(line);
    int id = 0;
    int level = 0;
    int shield = 0;
    std::string unuse;
    buf >> unuse >> id >> level >> shield;
    Param param(id, level, shield, 0.0, 0.0);
    EventFuncPtr eventFuncPtr = &Event::AddTarget;
    EventFunc func(eventFuncPtr, param);
    events.push_back(std::pair<Frame_t, EventFunc>(frames, func));
}

void Event::AddTarget(const Param &param)
{
    (*m_targetsMap)[NORMAL].push_back(new Target(*m_player, param.int1st, param.int2nd, param.int3rd, m_settings->classType));
    m_stats->AddDamageStats(param.int1st, *m_player, *m_targetsMap);
}

void Event::ParseChangeTarget(Frame_t frames, const std::string &line)
{
    std::stringstream buf(line);
    Id_t id = 0;
    std::string unuse;
    buf >> unuse >> id;
    Param param;
    param.int1st = id;
    EventFuncPtr eventFuncPtr = &Event::ChangeTarget;
    EventFunc func(eventFuncPtr, param);
    events.push_back(std::pair<Frame_t, EventFunc>(frames, func));
}

void Event::ChangeTarget(const Param &param)
{
    Target *target;
    for (auto it : (*m_targetsMap)[NORMAL]) {
        if (it->GetId() == static_cast<Id_t>(param.int1st)) {
            target = it;
        }
    }
    (*m_targetsMap)[NORMAL].remove(target);
    (*m_targetsMap)[NORMAL].push_front(target);
}

void Event::ParseDeleteTarget(Frame_t frames, const std::string &line)
{
    std::stringstream buf(line);
    Id_t id = 0;
    std::string unuse;
    buf >> unuse >> id;
    Param param;
    param.int1st = id;
    EventFuncPtr eventFuncPtr = &Event::DeleteTarget;
    EventFunc func(eventFuncPtr, param);
    events.push_back(std::pair<Frame_t, EventFunc>(frames, func));
}

void Event::DeleteTarget(const Param &param)
{
    Target *target;
    for (auto it : (*m_targetsMap)[NORMAL]) {
        if (it->GetId() == static_cast<Id_t>(param.int1st)) {
            target = it;
        }
    }
    (*m_targetsMap)[NORMAL].remove(target);
    (*m_targetsMap)[DEAD].push_back(target);
}

void Event::ParseDetachTarget(Frame_t frames, const std::string &line)
{
    std::stringstream buf(line);
    Id_t id = 0;
    std::string unuse;
    buf >> unuse >> id;
    Param param;
    param.int1st = id;
    EventFuncPtr  eventFuncPtr = &Event::DetachTarget;
    EventFunc func(eventFuncPtr, param);
    events.push_back(std::pair<Frame_t, EventFunc>(frames, func));
}

void Event::DetachTarget(const Param &param)
{
    Target *target;
    for (auto it : (*m_targetsMap)[NORMAL]) {
        if (it->GetId() == static_cast<Id_t>(param.int1st)) {
            target = it;
        }
    }
    (*m_targetsMap)[NORMAL].remove(target);
    (*m_targetsMap)[DETACHED].push_back(target);
}

void Event::ParseRecoveryTarget(Frame_t frames, const std::string &line)
{
    std::stringstream buf(line);
    Id_t id = 0;
    std::string unuse;
    buf >> unuse >> id;
    Param param;
    param.int1st = id;
    EventFuncPtr eventFuncPtr = &Event::RecoveryTarget;
    EventFunc func(eventFuncPtr, param);
    events.push_back(std::pair<Frame_t, EventFunc>(frames, func));
}

void Event::RecoveryTarget(const Param &param)
{
    Target *target;
    for (auto it : (*m_targetsMap)[DETACHED]) {
        if (it->GetId() == static_cast<Id_t>(param.int1st)) {
            target = it;
        }
    }
    (*m_targetsMap)[DETACHED].remove(target);
    (*m_targetsMap)[NORMAL].push_back(target);
}

void Event::ParseStop(Frame_t frames, const std::string &line)
{
    Param param;
    EventFuncPtr eventFuncPtr = &Event::DetachTarget;
    EventFunc func(eventFuncPtr, param);
    events.push_back(std::pair<Frame_t, EventFunc>(frames, func));
}

void Event::Stop(const Param &param)
{
    m_player->SetStop(true);
}

void Event::ParseContinue(Frame_t frames, const std::string &line)
{
    Param param;
    EventFuncPtr eventFuncPtr = &Event::DetachTarget;
    EventFunc func(eventFuncPtr, param);
    events.push_back(std::pair<Frame_t, EventFunc>(frames, func));
}

void Event::Continue(const Param &param)
{
    m_player->SetStop(false);
}

void Event::ParseFcast(Frame_t frames, const std::string &line)
{
    m_macro->ParseToForceCast(frames, line);
}

void Event::ParseCast(Frame_t frames, const std::string &line)
{
    m_macro->ParseToForceCast(frames, line);
}

void Event::ParseRefreshBuff(Frame_t frames, const std::string &line)
{
    std::stringstream buf(line);
    std::string buffName;
    std::string unuse;
    std::string role;
    Id_t id = 0;
    Param param;
    EventFuncPtr eventFuncPtr;
    Id_t buffId = 0;
    buf >> unuse >> role;
    if (role == "Player") {
        buf >> buffName;
        eventFuncPtr = &Event::RefreshBuff;
        buffId = GetBuffId(buffName);
        param.int1st = buffId;
    } else {
        buf >> id >> buffName;
        eventFuncPtr = &Event::RefreshTBuff;
        param.int1st = id;
        buffId = GetBuffId(buffName);
        param.int2nd = buffId;
    }
    EventFunc func(eventFuncPtr, param);
    events.push_back(std::pair<Frame_t, EventFunc>(frames, func));
}

void Event::RefreshBuff(const Param &param)
{
    m_player->buffs[param.int1st]->Refresh();
}

void Event::RefreshTBuff(const Param &param)
{
    for (auto it : (*m_targetsMap)[NORMAL]) {
        if (it->GetId() == static_cast<Id_t>(param.int1st)) {
            it->buffs[param.int2nd]->Refresh();
        }
    }
    for (auto it : (*m_targetsMap)[DETACHED]) {
        if (it->GetId() == static_cast<Id_t>(param.int1st)) {
            it->buffs[param.int2nd]->Refresh();
        }
    }
}

void Event::ParseCleanBuff(Frame_t frames, const std::string &line)
{
    std::stringstream buf(line);
    std::string buffName;
    std::string unuse;
    std::string role;
    Id_t id = 0;
    Param param;
    EventFuncPtr eventFuncPtr;
    Id_t buffId = 0;
    buf >> unuse >> role;
    if (role == "Player") {
        buf >> buffName;
        eventFuncPtr = &Event::CleanBuff;
        buffId = GetBuffId(buffName);
        param.int1st = buffId;
    } else {
        buf >> id >> buffName;
        eventFuncPtr = &Event::CleanTBuff;
        param.int1st = id;
        buffId = GetBuffId(buffName);
        param.int2nd = buffId;
    }
    EventFunc func(eventFuncPtr, param);
    events.push_back(std::pair<Frame_t, EventFunc>(frames, func));
}

void Event::CleanBuff(const Param &param)
{
    m_player->buffs[param.int1st]->Clean(*m_targetsMap, *m_stats, CastType::FORCE);
}

void Event::CleanTBuff(const Param &param)
{
    for (auto it : (*m_targetsMap)[NORMAL]) {
        if (it->GetId() == static_cast<Id_t>(param.int1st)) {
            it->buffs[param.int2nd]->Clean(*m_targetsMap, *m_stats, CastType::FORCE);
        }
    }
    for (auto it : (*m_targetsMap)[DETACHED]) {
        if (it->GetId() == static_cast<Id_t>(param.int1st)) {
            it->buffs[param.int2nd]->Clean(*m_targetsMap, *m_stats, CastType::FORCE);
        }
    }
}

void Event::ParseSet(Frame_t frames, const std::string &line)
{
    std::stringstream buf(line);
    Id_t id = 0;
    Pct_t percent = 0.0;
    std::string unuse;
    buf >> unuse >> id >> unuse >> percent;
    Param param;
    param.int1st = id;
    param.double4th = percent;
    EventFuncPtr eventFuncPtr = &Event::Set;
    EventFunc func(eventFuncPtr, param);
    events.push_back(std::pair<Frame_t, EventFunc>(frames, func));
}

void Event::Set(const Param &param)
{
    for (auto it : (*m_targetsMap)[NORMAL]) {
        if (it->GetId() == static_cast<Id_t>(param.int1st)) {
            it->SetLifePercent(param.double4th);
        }
    }
    for (auto it : (*m_targetsMap)[DETACHED]) {
        if (it->GetId() == static_cast<Id_t>(param.int1st)) {
            it->SetLifePercent(param.double4th);
        }
    }
}

}
