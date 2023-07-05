/**
 * Project: JX3DPS
 * File: KeyFrame.cpp
 * Description:
 * Created Date: 2023-06-19 16:27:04
 * Author: 难为水
 * -----
 * Last Modified: 2023-07-05 11:10:56
 * Modified By: 难为水
 * -----
 * HISTORY:
 * Date      	By   	Comments
 * ----------	-----	----------------------------------------------------------
 */

#include "KeyFrame.h"

#include <spdlog/spdlog.h>

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

void JX3DPS::KeyFrame::GenerateKeyFrameSequence(KeyFrameSequence &keyFrameSequence, Player *player, ExprEvents &exprEvents)
{
    // 强制事件
    for (auto &exprEvent : exprEvents) {
        Frame_t frame = exprEvent.first;
        if (!keyFrameSequence.empty() && frame == keyFrameSequence.back().first) {
            keyFrameSequence.back().second.push_back(std::pair<KeyFrameType, Id_t>(KeyFrameType::EVENT, 0));
        } else {
            std::list<std::pair<KeyFrameType, Id_t>> events;
            events.push_back(std::pair<KeyFrameType, Id_t>(KeyFrameType::EVENT, 0));
            keyFrameSequence.push_back(KeyFrame(frame, events));
        }
    }

    // 技能
    for (auto &skill : player->skills) {
        KeyFrame keyFrame;
        keyFrame.first = skill.second->GetNextKeyFrame();
        keyFrame.second.push_back(std::make_pair(KeyFrameType::SKILL, skill.first));
        InsertKeyFrame(keyFrameSequence, keyFrame);
    }
    // buff
    for (auto &buff : player->buffs) {
        KeyFrame keyFrame;
        keyFrame.first = buff.second->GetNextKeyFrame();
        keyFrame.second.push_back(std::make_pair(KeyFrameType::BUFF, buff.first));
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
        for (auto id : keyFrame.second) {
            if (id.first == KeyFrameType::EVENT) {        // 强制事件
                continue;
            } else if (id.first == KeyFrameType::SKILL) { // 技能
                player->skills[id.second]->UpdateKeyFrame(next);
            } else if (id.first == KeyFrameType::BUFF) {  // buff
                player->buffs[id.second]->UpdateKeyFrame(next);
            }
        }
    }
}

void JX3DPS::KeyFrame::KeyFrameAdvance(KeyFrameSequence &keyFrameSequence, Player *player, Targets *targets, ExprEvents &exprEvents, ExprSkills &exprSkills)
{
    while (player->end == false) {
        //spdlog::debug("{}",  player->attr->GetPhysicsCriticalStrikePercent());
        // auto rIter = keyFrameSequence.rbegin();  // 指向最后一个元素
        // ++rIter;
        // if (rIter->first <= 13850) {
        //     spdlog::info("KeyFrameAdvance: {}", rIter->first);
        // }

        // 更新关键帧序列
        Frame_t next = keyFrameSequence.front().first;
        UpdateKeyFrameSequence(keyFrameSequence, player, next);

        // 执行首序关键帧列
        KeyFrameSequence releasedKeyFrameSequence;
        std::list<Id_t>  skills;
        ExprEvents       exprEventSkills;
        for (auto &temp : keyFrameSequence.front().second) {
            if (temp.first == KeyFrameType::EVENT) {               // 强制事件
                if (!exprEvents.front().second(player, targets)) { // 执行失败，为技能，顺延执行
                    exprEventSkills.push_back(exprEvents.front());
                }
                exprEvents.pop_front();
            } else if (temp.first == KeyFrameType::SKILL) {                 // 技能
                if (player->skills[temp.second]->GetCooldown() != 0 &&
                        player->skills[temp.second]->GetEnergyCount() == -1 // 非充能技能prepareFrames为0
                    || player->skills[temp.second]->IsEnergy()) // 充能技能冷却为0
                {
                    spdlog::debug("Trigger\tskill\t{}", player->skills[temp.second]->GetName());
                    player->skills[temp.second]->Trigger();
                }
                skills.push_back(temp.second);
            } else { // buff
                spdlog::debug("Trigger\tbuff\t{}", player->buffs[temp.second]->GetName());
                player->buffs[temp.second]->Trigger();
                KeyFrame keyFrame;
                keyFrame.first = player->buffs[temp.second]->GetNextKeyFrame();
                keyFrame.second.push_back(std::make_pair(temp.first, temp.second));
                releasedKeyFrameSequence.push_back(keyFrame);
            }
        }

        // 去除首序关键帧列
        keyFrameSequence.pop_front();

        // 当有多个技能处于同一关键帧时，按照宏顺序依次施放
        ExprSkills tempSkills;
        for (auto &exprSkill : exprSkills) {
            for (auto &skill : skills) {
                if (exprSkill.second == skill) { // 当前帧的若干技能 根据宏顺序 重新组合成新宏
                    // 注意：如果当前帧的技能不在宏中，后序也不会再加入关键帧序列
                    // 注意：但是不在关键帧序列会引发事件中的技能帧没办法刷新，所以在事件技能处做处理
                    tempSkills.push_back(exprSkill);
                    break;
                }
            }
        }

        if (!player->stop) {
            CastSkill(player, targets, tempSkills);
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

        // 插入关键帧序列
        for (auto &keyFrame : releasedKeyFrameSequence) {
            InsertKeyFrame(keyFrameSequence, keyFrame);
        }

        // 检查已施放技能关键帧因为事件、技能施放或buff刷新等原因的状态变化，并插入关键帧序列
        KeyFrameSequence tempKeyFrameSequence;
        for (auto &exprSkill : tempSkills) {
            KeyFrame keyFrame;
            keyFrame.first = player->skills[exprSkill.second]->GetNextKeyFrame();
            keyFrame.second.push_back(std::make_pair(KeyFrameType::SKILL, exprSkill.second));
            if (keyFrame.first == 0) { // 技能冷却时间为0，但因不满足条件无法施放
                tempKeyFrameSequence.push_back(keyFrame);
            } else {
                InsertKeyFrame(keyFrameSequence, keyFrame);
            }
        }

        for (auto &keyFrame : tempKeyFrameSequence)
        { // 将技能冷却时间为0，但因不满足条件无法施放的技能插入到关键帧序列首位
            keyFrame.first = keyFrameSequence.front().first;
            InsertKeyFrame(keyFrameSequence, keyFrame);
        }

        if (!exprEventSkills.empty()) { // 执行失败，为技能，顺延执行
            for (int i = 0; i < exprEventSkills.size(); ++i) {
                KeyFrame keyFrame;
                keyFrame.first = keyFrameSequence.front().first;
                keyFrame.second.push_back(std::make_pair(KeyFrameType::EVENT, 0));
                InsertKeyFrame(keyFrameSequence, keyFrame);
            }
            int count = 0;
            // 查找关键帧序列第一帧有没有事件，如果没有，则将执行失败的技能插入到事件宏首位，如果有，则将执行失败的技能插入到相应的事件宏后
            for (auto &keyFrame : keyFrameSequence.front().second) {
                if (keyFrame.first == KeyFrameType::EVENT) {
                    count++;
                }
            }
            auto it = exprEvents.begin();
            std::advance(it, count);
            exprEvents.splice(it, exprEventSkills);
        }
    }
}