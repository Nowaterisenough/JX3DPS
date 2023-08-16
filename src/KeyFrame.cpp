/**
 * Project: JX3DPS
 * File: KeyFrame.cpp
 * Description:
 * Created Date: 2023-06-19 16:27:04
 * Author: 难为水
 * -----
 * Last Modified: 2023-08-16 18:05:33
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

// #define OLD_FRAMEWORK

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
        if (keyFrame.first != JX3DPS_INVALID_FRAMES_SET) {
            keyFrame.first -= next;
        }

        for (auto &[type, id] : keyFrame.second) {
            if (type == KeyFrameType::EVENT) { // 强制事件
                continue;
            } else if (type == KeyFrameType::SKILL) { // 技能
                player->skills[id]->UpdateKeyFrame(next);
            } else if (type == KeyFrameType::BUFF) { // buff
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
    ExprSkills       exprSkills = exprSkillsHash.at(EXPRESSION_SKILL_PLACE_HOLDERS_1);
    Frame_t          now        = 0;
    KeyFrameSequence checkedKeyFrameSequence;
    KeyFrameSequence unCheckedSkillKeyFrameSequence;
    KeyFrameSequence unCheckedBuffKeyFrameSequence;

#ifdef OLD_FRAMEWORK
    std::list<Id_t> skills;
#endif // OLD_FRAMEWORK
    while (true) {
        checkedKeyFrameSequence.clear();
        unCheckedSkillKeyFrameSequence.clear();
        unCheckedBuffKeyFrameSequence.clear();
#ifdef OLD_FRAMEWORK
        skills.clear();
#endif // OLD_FRAMEWORK

        // 更新关键帧序列
        Frame_t next  = keyFrameSequence.front().first;
        now          += next;
        if (now >= options.totalFrames) {
            return;
        }
        UpdateKeyFrameSequence(keyFrameSequence, player, next);

        // 执行首序关键帧列
        for (auto &[type, id] : keyFrameSequence.front().second) {
            if (type == KeyFrameType::EVENT) { // 强制事件
                exprEvents.front().second(player, targets);
                exprEvents.pop_front();
            } else if (type == KeyFrameType::SKILL) { // 技能
                // spdlog::debug("{} Trigger Skill {}", now, id);
                player->skills[id]->Trigger();
                KeyFrame keyFrame;
                keyFrame.first = 0;
                keyFrame.second.push_back(std::make_pair(type, id));
                unCheckedSkillKeyFrameSequence.push_back(keyFrame);
#ifdef OLD_FRAMEWORK
                skills.emplace_back(id);
#endif // OLD_FRAMEWORK

            } else if (type == KeyFrameType::BUFF) { // buff
                // spdlog::debug("{} Trigger Buff {}", now, id);
                player->buffs[id]->Trigger();
                KeyFrame keyFrame;
                keyFrame.first = 0;
                keyFrame.second.push_back(std::make_pair(type, id));
                unCheckedBuffKeyFrameSequence.push_back(keyFrame);
            }
        }

        // 去除首序关键帧列
        keyFrameSequence.pop_front();

        if (!player->IsStop()) {
            CastSkills(player, targets, exprSkillsHash, exprSkills, now);
        }

        // 检查后序关键帧因为事件、技能施放或buff刷新等原因的状态变化
        for (auto it = keyFrameSequence.begin(); it != keyFrameSequence.end();) {
            for (auto iter = it->second.begin(); iter != it->second.end();) {
                KeyFrame keyFrame;
                keyFrame.first = it->first;
                if (iter->first == KeyFrameType::SKILL) {
                    keyFrame.first = player->skills[iter->second]->GetNextKeyFrame();
                    keyFrame.second.push_back(std::make_pair(KeyFrameType::SKILL, iter->second));
#ifdef OLD_FRAMEWORK
                    skills.emplace_back(iter->second);
#endif // OLD_FRAMEWORK
                } else if (iter->first == KeyFrameType::BUFF) {
                    keyFrame.first = player->buffs[iter->second]->GetNextKeyFrame();
                    keyFrame.second.push_back(std::make_pair(KeyFrameType::BUFF, iter->second));
                }
                if (keyFrame.first != it->first) {
                    checkedKeyFrameSequence.push_back(keyFrame);
                    iter = it->second.erase(iter);
                    continue;
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

        for (auto &keyFrame : checkedKeyFrameSequence) {
            InsertKeyFrame(keyFrameSequence, keyFrame);
        }

        // 插入关键帧序列
        for (auto &[frame, key] : unCheckedSkillKeyFrameSequence) {
            KeyFrame keyFrame;
            keyFrame.first = player->skills[key.front().second]->GetNextKeyFrame();
            keyFrame.second.push_back(std::make_pair(KeyFrameType::SKILL, key.front().second));
            InsertKeyFrame(keyFrameSequence, keyFrame);
        }

        // 插入关键帧序列
        for (auto &[frame, key] : unCheckedBuffKeyFrameSequence) {
            KeyFrame keyFrame;
            keyFrame.first = player->buffs[key.front().second]->GetNextKeyFrame();
            keyFrame.second.push_back(std::make_pair(KeyFrameType::BUFF, key.front().second));
            InsertKeyFrame(keyFrameSequence, keyFrame);
        }

        Frame_t nextCD = keyFrameSequence.front().first;
#ifdef OLD_FRAMEWORK
        for (auto &id : skills) {
            Frame_t temp = player->skills[id]->GetCooldownCurrent();
            if (temp != 0) {
                nextCD = nextCD < temp ? nextCD : temp;
            }
        }
        KeyFrame keyFrame;
        keyFrame.first = nextCD;
        keyFrame.second.push_back(std::make_pair(KeyFrameType::EMPTY, Id_t::SKILL_DEFAULT));
        InsertKeyFrame(keyFrameSequence, keyFrame);
#else
        nextCD = nextCD < player->GetNextGlobalCooldown() ? nextCD : player->GetNextGlobalCooldown();
        nextCD = nextCD > options.framePrecision ? options.framePrecision : nextCD;
#endif // OLD_FRAMEWORK

        KeyFrame keyFrame;
        keyFrame.first = nextCD;
        keyFrame.second.push_back(std::make_pair(KeyFrameType::EMPTY, Id_t::SKILL_DEFAULT));
        InsertKeyFrame(keyFrameSequence, keyFrame);
    }
}

JX3DPS::Id_t JX3DPS::KeyFrame::CastSkills(Player *player, Targets *targets, ExprSkillsHash &exprSkillsHash, ExprSkills &exprSkills, Frame_t now)
{
    for (auto iter = exprSkills.begin(); iter != exprSkills.end();) {
        bool scast = iter->first.front().front()(player, targets);

        // 先判断冷却、距离和读条条件
        bool precondition = true;
        for (auto &exprIf : *std::next(iter->first.begin())) {
            if ((precondition = exprIf(player, targets)) == false) {
                break;
            }
        }
        if (precondition == false) {
            ++iter;
            if (scast) {
                return SKILL_DEFAULT;
            }
            continue; // 前置条件不满足，跳过此技能
        }

        // 判断技能施放条件
        bool castSuccess = true;
        for (auto it = std::next(std::next(iter->first.begin())); it != iter->first.end(); ++it)
        {
            for (auto &exprIf : *it) {
                if ((castSuccess = exprIf(player, targets)) == false) {
                    break;
                }
            }
            if (castSuccess) {
                break;
            }
        }

        if (castSuccess) {
            Id_t id = iter->second;
            if (id > SKILL_DEFAULT) { // 执行技能
                player->skills[id]->Cast();
                spdlog::debug("{:<8} {}", now *0.0625, JX3DPS_NAME.at(static_cast<int>(id)));
            } else if (id > TARGET_PLACE_HOLDERS_DEFAULT) { // 转火目标
                player->SetTargetId(id);
            } else if (id > EXPRESSION_SKILL_PLACE_HOLDERS_DEFAULT) { // 切换宏
                exprSkills = exprSkillsHash.at(id);
            }

            // 断读条
            if (std::next(iter->first.front().begin()) != iter->first.front().end()) {
                (*std::next(iter->first.front().begin()))(player, targets);
            }

            // scast执行成功
            if (scast && id > EXPRESSION_SKILL_PLACE_HOLDERS_END) {
                iter = exprSkills.erase(iter);
            }

            CastSkills(player, targets, exprSkillsHash, exprSkills, now);
            return SKILL_DEFAULT;
        } else if (scast) { // scast执行失败
            return SKILL_DEFAULT;
        }

        ++iter;
    }
    return SKILL_DEFAULT;
}
