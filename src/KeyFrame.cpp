/**
 * Project: JX3DPS
 * File: KeyFrame.cpp
 * Description:
 * Created Date: 2023-06-19 16:27:04
 * Author: 难为水
 * -----
 * Last Modified: 2023-08-14 09:52:10
 * Modified By: 难为水
 * -----
 * HISTORY:
 * Date      	By   	Comments
 * ----------	-----	----------------------------------------------------------
 */

#include "KeyFrame.h"

#include <spdlog/spdlog.h>

#include "Buff.h"
#include "Player.h"
#include "Skill.h"
#include "Target.hpp"

void JX3DPS::KeyFrame::InsertKeyFrame(KeyFrameSequence &keyFrameSequence, KeyFrame &keyFrame)
{
    if (keyFrameSequence.empty()) {
        keyFrameSequence.push_back(keyFrame);
        return;
    }
    for (auto it = keyFrameSequence.begin(); it != keyFrameSequence.end(); ++it) {
        if (it->first == keyFrame.first) {
            it->second.insert(it->second.end(), keyFrame.second.begin(), keyFrame.second.end());
            return;
        } else if (it->first > keyFrame.first) {
            keyFrameSequence.insert(it, keyFrame);
            return;
        }
    }
    keyFrameSequence.push_back(keyFrame);
}

void JX3DPS::KeyFrame::GenerateKeyFrameSequence(KeyFrameSequence &keyFrameSequence,
                                                Player           *player,
                                                ExprEvents       &exprEvents,
                                                ExprSkillsHash   &exprSkillsHash)
{
    // 强制事件
    for (auto &[frame, exprIf] : exprEvents) {
        if (!keyFrameSequence.empty() && frame == keyFrameSequence.back().first) {
            keyFrameSequence.back().second.push_back(
                std::pair<KeyFrameType, Id_t>(KeyFrameType::EVENT, EXPRESSION_EVENT_PLACE_HOLDERS_DEFAULT));
        } else {
            std::list<std::pair<KeyFrameType, Id_t>> events;
            events.push_back(std::pair<KeyFrameType, Id_t>(KeyFrameType::EVENT, EXPRESSION_EVENT_PLACE_HOLDERS_DEFAULT));
            keyFrameSequence.push_back(KeyFrame(frame, events));
        }
    }

    // 技能
    for (auto &[id, skill] : player->skills) {
        // 判断技能是否在表达式中
        bool has = false;
        for (const auto &[macroId, exprSkills] : exprSkillsHash) {
            for (const auto &[exprIfs, i] : exprSkills) {
                if (i == id) {
                    has = true;
                    break;
                }
            }
            if (has) {
                break;
            }
        }
        if (!has) {
            continue;
        }

        KeyFrame keyFrame;
        keyFrame.first = skill->GetNextKeyFrame();
        keyFrame.second.push_back(std::make_pair(KeyFrameType::SKILL, id));
        InsertKeyFrame(keyFrameSequence, keyFrame);
    }
    // buff
    for (auto &[id, buff] : player->buffs) {
        KeyFrame keyFrame;
        keyFrame.first = buff->GetNextKeyFrame();
        keyFrame.second.push_back(std::make_pair(KeyFrameType::BUFF, id));
        InsertKeyFrame(keyFrameSequence, keyFrame);
    }
}

void JX3DPS::KeyFrame::UpdateKeyFrameSequence(KeyFrameSequence &keyFrameSequence, Player *player, Frame_t next)
{
    player->UpdateGlobalCooldown(next); // 更新公共冷却
    for (auto &keyFrame : keyFrameSequence) {
        if (keyFrame.first == JX3DPS_INVALID_FRAMES_SET) {
            continue;
        }
        keyFrame.first -= next;
        for (auto &[type, id] : keyFrame.second) {
            if (type == KeyFrameType::EVENT) {        // 强制事件
                continue;
            } else if (type == KeyFrameType::SKILL) { // 技能
                player->skills[id]->UpdateKeyFrame(next);
            } else if (type == KeyFrameType::BUFF) {  // buff
                player->buffs[id]->UpdateKeyFrame(next);
            }
        }
    }
}

void JX3DPS::KeyFrame::KeyFrameAdvance(
    KeyFrameSequence &keyFrameSequence,
    Player           *player,
    Targets          *targets,
    ExprEvents       &exprEvents,
    ExprSkillsHash   &exprSkillsHash,
    Options          &options)
{
    ExprSkills exprSkills = exprSkillsHash.at(EXPRESSION_SKILL_PLACE_HOLDERS_1);
    Frame_t    now        = 0;
    while (true) {

        // 更新关键帧序列
        Frame_t next  = keyFrameSequence.front().first;
        now          += next;
        if (now >= options.totalFrames) {
            return;
        }
        UpdateKeyFrameSequence(keyFrameSequence, player, next);

        // 执行首序关键帧列
        KeyFrameSequence releasedKeyFrameSequence;
        std::list<Id_t>  skills;
        for (auto &[type, id] : keyFrameSequence.front().second) {
            if (type == KeyFrameType::EVENT) { // 强制事件
                exprEvents.front().second(player, targets);
                exprEvents.pop_front();
            } else if (type == KeyFrameType::SKILL) { // 技能
                //spdlog::debug("{} Trigger Skill {}", now, id);
                player->skills[id]->Trigger();
                skills.push_back(id);
            } else { // buff
                //spdlog::debug("{} Trigger Buff {}", now, id);
                player->buffs[id]->Trigger();
                KeyFrame keyFrame;
                keyFrame.first = 0;
                keyFrame.second.push_back(std::make_pair(type, id));
                releasedKeyFrameSequence.push_back(keyFrame);
            }
        }

        // 去除首序关键帧列
        keyFrameSequence.pop_front();

        if (!player->IsStop()) {
            CastSkills(player, targets, exprSkillsHash, exprSkills);
        }

        // 检查已施放buff关键帧因为事件、技能施放或buff刷新等原因的状态变化
        for (auto &keyFrame : releasedKeyFrameSequence) {
            keyFrame.first = player->buffs[keyFrame.second.front().second]->GetNextKeyFrame();
        }

        // 检查后序关键帧因为事件、技能施放或buff刷新等原因的状态变化
        for (auto it = keyFrameSequence.begin(); it != keyFrameSequence.end();) {
            for (auto iter = it->second.begin(); iter != it->second.end();) {
                KeyFrame keyFrame;
                if (iter->first == KeyFrameType::EVENT) {
                    ++iter;
                    continue;
                } else if (iter->first == KeyFrameType::SKILL) {
                    keyFrame.first = player->skills[iter->second]->GetNextKeyFrame();
                    keyFrame.second.push_back(std::make_pair(KeyFrameType::SKILL, iter->second));
                } else { // buff
                    keyFrame.first = player->buffs[iter->second]->GetNextKeyFrame();
                    keyFrame.second.push_back(std::make_pair(KeyFrameType::BUFF, iter->second));
                }
                if (keyFrame.first != it->first) {
                    releasedKeyFrameSequence.push_back(keyFrame);
                    iter = it->second.erase(iter);
                } else {
                    ++iter;
                }
            }
            if (it->second.size() == 0) {
                it = keyFrameSequence.erase(it);
            } else {
                ++it;
            }
        }

        // 检查已施放技能关键帧因为事件、技能施放或buff刷新等原因的状态变化，并插入关键帧序列
        KeyFrameSequence tempKeyFrameSequence;
        for (auto &id : skills) {
            KeyFrame keyFrame;
            keyFrame.first = player->skills[id]->GetNextKeyFrame();
            keyFrame.second.push_back(std::make_pair(KeyFrameType::SKILL, id));
            if (keyFrame.first == 0) { // 技能冷却时间为0，但因不满足条件无法施放
                tempKeyFrameSequence.push_back(keyFrame); // 不能在此处插入关键帧序列，因为有可能会出现当前关键帧序列首位不正确的情况
            } else {
                releasedKeyFrameSequence.push_back(keyFrame);
            }
        }

        // 插入关键帧序列
        for (auto &keyFrame : releasedKeyFrameSequence) {
            InsertKeyFrame(keyFrameSequence, keyFrame);
        }

        // 将技能冷却时间为0，但因不满足条件无法施放的技能插入到关键帧序列首位
        for (auto &keyFrame : tempKeyFrameSequence) {
            keyFrame.first = keyFrameSequence.front().first;
            InsertKeyFrame(keyFrameSequence, keyFrame);
        }
    }
}

JX3DPS::Id_t JX3DPS::KeyFrame::CastSkills(Player *player, Targets *targets, ExprSkillsHash &exprSkillsHash, ExprSkills &exprSkills)
{
    for (auto iter = exprSkills.begin(); iter != exprSkills.end();) {
        bool scast = iter->first.front().front()(player, targets);

        // 先判断冷却、距离和读条条件
        bool precondition = false;
        for (auto &exprIf : *std::next(iter->first.begin())) {
            if ((precondition = exprIf(player, targets)) == false) {
                break;
            }
        }
        if (precondition == false) {
            ++iter;
            continue; // 前置条件不满足，跳过此技能
        }

        // 判断技能施放条件
        bool flag = true;
        for (auto it = std::next(std::next(iter->first.begin())); it != iter->first.end(); ++it)
        {
            for (auto &exprIf : *it) {
                if ((flag = exprIf(player, targets)) == false) {
                    break;
                }
            }
            if (flag) {
                Id_t id = iter->second;
                if (id > SKILL_DEFAULT) { // 执行技能
                    spdlog::debug("Cast Skill {}", id);
                    player->skills[id]->Cast();
                    if (scast) {
                        iter = exprSkills.erase(iter);
                    }
                } else if (id > TARGET_PLACE_HOLDERS_DEFAULT) { // 转火目标
                    player->SetTargetId(id);
                    if (scast) {
                        iter = exprSkills.erase(iter);
                    }
                    // 断读条
                    if (std::next(iter->first.front().begin()) != iter->first.front().end())
                    {
                        (*std::next(iter->first.front().begin()))(player, targets);
                    }
                    break; // 转火目标后，不再重新执行宏，避免因为一直满足转火条件导致死循环
                } else if (id > EXPRESSION_SKILL_PLACE_HOLDERS_DEFAULT) { // 切换宏
                    exprSkills = exprSkillsHash.at(id);
                }
                // 断读条
                if (std::next(iter->first.front().begin()) != iter->first.front().end()) {
                    (*std::next(iter->first.front().begin()))(player, targets);
                }

                CastSkills(player, targets, exprSkillsHash, exprSkills);
                return SKILL_DEFAULT;
            }
        }
        // scast执行失败
        if (!flag && scast) {
            return SKILL_DEFAULT;
        }
        ++iter;
    }
    return SKILL_DEFAULT;
}
