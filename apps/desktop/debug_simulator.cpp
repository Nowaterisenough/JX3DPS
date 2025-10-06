#include "debug_simulator.h"
#include <QDebug>
#include <QStringList>

// JX3DPS 核心头文件
#include "player.h"
#include "key_frame.h"
#include "regex.h"
#include "expression.h"
#include "target.hpp"
#include "skill.h"
#include "buff.h"
#include "class/tai_xu_jian_yi/tai_xu_jian_yi.h"
#include "global/defs.h"

struct DebugSimulator::Impl
{
    JX3DPS::Player *player = nullptr;
    JX3DPS::Targets *targets = nullptr;
    JX3DPS::ExprSkillsHash exprSkillsHash;
    JX3DPS::ExprEvents exprEvents;
    JX3DPS::KeyFrame::KeyFrameSequence keyFrameSequence;
    JX3DPS::Options options;

    // CastSkills 需要的状态
    JX3DPS::Id_t exprSkillsId = JX3DPS::EXPRESSION_SKILL_PLACE_HOLDERS_1;
    JX3DPS::Id_t lastExprSkillsId = JX3DPS::EXPRESSION_SKILL_PLACE_HOLDERS_1;
    JX3DPS::ExprSkills exprSkills;

    JX3DPS::Frame_t currentFrame = 0;
    bool finished = false;
    QString lastSkill;
    QString currentMacro;
    int currentMacroLine = 1;  // 当前宏行号（1-based）

    // 宏文本的所有行（用于行号映射）
    QStringList macroLines;
    // exprSkills 索引到源代码行号的映射（1-based）
    std::vector<int> exprSkillsLineMap;
};

DebugSimulator::DebugSimulator(QObject *parent)
    : QObject(parent)
    , d(std::make_unique<Impl>())
{
}

DebugSimulator::~DebugSimulator()
{
    Reset();
}

bool DebugSimulator::Initialize(const QString &macroText, QString &errorMessage)
{
    Reset();

    // 1. 解析宏文本
    QStringList lines = macroText.split('\n');
    d->macroLines = lines;  // 保存所有行用于行号跟踪
    d->currentMacroLine = 1;  // 重置行号
    std::list<std::pair<std::string, std::list<std::string>>> skills;

    QString currentMacroName;
    std::list<std::string> currentMacroLines;

    for (int i = 0; i < lines.size(); ++i) {
        QString trimmed = lines[i].trimmed();
        int lineNumber = i + 1;  // 1-based

        // 跳过空行和注释
        if (trimmed.isEmpty() || trimmed.startsWith('#')) {
            continue;
        }

        // 宏定义
        if (trimmed.startsWith("macro ")) {
            // 保存上一个宏
            if (!currentMacroName.isEmpty()) {
                skills.push_back({currentMacroName.toStdString(), currentMacroLines});
                currentMacroLines.clear();
            }
            currentMacroName = trimmed.mid(6).trimmed();
            d->currentMacro = currentMacroName;
        }
        // 宏指令
        else {
            currentMacroLines.push_back(trimmed.toStdString());
            // 记录这条指令对应的源代码行号
            d->exprSkillsLineMap.push_back(lineNumber);
        }
    }

    // 保存最后一个宏
    if (!currentMacroName.isEmpty()) {
        skills.push_back({currentMacroName.toStdString(), currentMacroLines});
    }

    // 调用 JX3DPS Regex 解析
    JX3DPS::Error_t err = JX3DPS::Regex::ParseToExprSkillsHash(skills, d->exprSkillsHash);
    if (err != JX3DPS::JX3DPS_SUCCESS) {
        errorMessage = QString("宏解析失败: 错误码 %1").arg(err);
        return false;
    }

    // 2. 创建Player（太虚剑意）
    d->player = JX3DPS::Player::PlayerFactoryGenerate(JX3DPS::ClassType::TAI_XU_JIAN_YI);
    d->player->Init();

    // 3. 创建Targets
    d->targets = new JX3DPS::Targets();
    d->player->SetTargets(d->targets);

    // 添加默认目标 (使用 Id_t 类型)
    JX3DPS::Id_t targetId = static_cast<JX3DPS::Id_t>(1);
    JX3DPS::Expression::AddTarget(d->player, d->targets, targetId, 124, 27550, 4.0);
    d->player->SetTargetId(targetId);

    // 4. 初始化选项
    d->options.totalFrames = 96000;  // 60分钟 * 16帧/秒 * 100
    d->options.delayMin = 0;
    d->options.delayMax = 0;

    // 5. 生成KeyFrameSequence
    JX3DPS::KeyFrame::GenerateKeyFrameSequence(
        d->keyFrameSequence,
        d->player,
        d->exprEvents,
        d->exprSkillsHash
    );

    // 6. 初始化 CastSkills 需要的状态
    d->exprSkillsId = JX3DPS::EXPRESSION_SKILL_PLACE_HOLDERS_1;
    d->lastExprSkillsId = JX3DPS::EXPRESSION_SKILL_PLACE_HOLDERS_1;

    // 检查 exprSkillsHash 是否包含默认ID
    if (d->exprSkillsHash.find(d->exprSkillsId) == d->exprSkillsHash.end()) {
        errorMessage = QString("ExprSkillsHash 不包含默认宏ID: %1").arg(static_cast<int>(d->exprSkillsId));
        qDebug() << errorMessage;
        qDebug() << "ExprSkillsHash 包含的ID数量:" << d->exprSkillsHash.size();
        return false;
    }

    d->exprSkills = d->exprSkillsHash.at(d->exprSkillsId);
    d->player->SetDelay(d->options.delayMin, d->options.delayMax);

    d->currentFrame = 0;
    d->finished = false;

    qDebug() << "调试模拟器初始化成功";
    qDebug() << "解析到" << skills.size() << "个宏";
    qDebug() << "生成" << d->keyFrameSequence.size() << "个关键帧";
    qDebug() << "ExprSkillsHash 大小:" << d->exprSkillsHash.size();

    return true;
}

bool DebugSimulator::StepOne()
{
    qDebug() << "\n===== StepOne 开始 =====";
    qDebug() << "finished:" << d->finished << ", player:" << (d->player != nullptr);
    qDebug() << "currentMacroLine:" << d->currentMacroLine;
    qDebug() << "keyFrameSequence.size():" << d->keyFrameSequence.size();

    if (d->finished || !d->player) {
        qDebug() << "StepOne: 已完成或player为空，退出";
        return false;
    }

    if (d->keyFrameSequence.empty()) {
        qDebug() << "StepOne: keyFrameSequence为空，标记为完成";
        d->finished = true;
        return false;
    }

    // 模拟 KeyFrameAdvance 的单步执行
    // 参考 key_frame.cpp:139-188

    // 1. 获取下一帧并更新时间
    JX3DPS::Frame_t next = d->keyFrameSequence.front().first;
    d->currentFrame += next;

    if (d->currentFrame >= d->options.totalFrames) {
        d->finished = true;
        return false;
    }

    // 2. 更新关键帧序列（所有技能/buff的冷却等）
    JX3DPS::KeyFrame::UpdateKeyFrameSequence(d->keyFrameSequence, d->player, next);

    // 3. 执行当前帧的所有事件
    for (auto &[type, id] : d->keyFrameSequence.front().second) {
        if (type == JX3DPS::KeyFrame::KeyFrameType::EVENT) {
            // 执行事件
            if (!d->exprEvents.empty()) {
                d->exprEvents.front().second(d->player, d->targets);
                d->exprEvents.pop_front();
            }
        } else if (type == JX3DPS::KeyFrame::KeyFrameType::SKILL) {
            // 触发技能
            qDebug() << "触发技能:" << static_cast<int>(id);
            d->player->skills[id]->Trigger();
            d->lastSkill = QString("技能ID:%1").arg(static_cast<int>(id));
        } else if (type == JX3DPS::KeyFrame::KeyFrameType::BUFF) {
            // 触发buff
            qDebug() << "触发Buff:" << static_cast<int>(id);
            d->player->buffs[id]->Trigger();
        }
    }

    // 4. 移除已处理的帧
    d->keyFrameSequence.pop_front();

    // 5. 调用 CastSkills 执行宏逻辑（关键！）
    if (!d->player->IsStop()) {
        qDebug() << "准备调用 CastSkills, frame=" << d->currentFrame
                 << ", exprSkillsId=" << static_cast<int>(d->exprSkillsId);

        try {
            JX3DPS::Id_t skillId = JX3DPS::KeyFrame::CastSkills(
                d->player,
                d->targets,
                d->exprSkillsHash,
                d->exprSkills,
                d->currentFrame,
                d->exprSkillsId,
                d->lastExprSkillsId,
                0
            );

            if (skillId != JX3DPS::SKILL_DEFAULT) {
                qDebug() << ">>> CastSkills 返回技能ID:" << static_cast<int>(skillId);
                d->lastSkill = QString("施放技能ID:%1").arg(static_cast<int>(skillId));

                // 找出是哪条 exprSkills 指令被执行了
                // 遍历 exprSkills，找到第一个满足条件且返回此技能ID的指令
                qDebug() << ">>> 开始匹配指令，exprSkills.size():" << d->exprSkills.size();
                qDebug() << ">>> exprSkillsLineMap.size():" << d->exprSkillsLineMap.size();
                int matchedIndex = -1;
                int index = 0;
                for (auto iter = d->exprSkills.begin(); iter != d->exprSkills.end(); ++iter, ++index) {
                    // 检查前置条件（冷却、距离、读条）
                    bool precondition = true;
                    for (auto &exprIf : *std::next(iter->first.begin())) {
                        if (!exprIf(d->player, d->targets)) {
                            precondition = false;
                            break;
                        }
                    }
                    if (!precondition) {
                        continue;
                    }

                    // 检查施放条件
                    bool castSuccess = false;
                    for (auto it = std::next(std::next(iter->first.begin())); it != iter->first.end(); ++it) {
                        castSuccess = true;
                        for (auto &exprIf : *it) {
                            if (!exprIf(d->player, d->targets)) {
                                castSuccess = false;
                                break;
                            }
                        }
                        if (castSuccess) {
                            break;
                        }
                    }

                    // 检查技能ID是否匹配
                    if (castSuccess && iter->second == skillId) {
                        matchedIndex = index;
                        break;
                    }
                }

                // 更新当前行号为匹配的指令所在的源代码行
                if (matchedIndex >= 0 && matchedIndex < static_cast<int>(d->exprSkillsLineMap.size())) {
                    d->currentMacroLine = d->exprSkillsLineMap[matchedIndex];
                    qDebug() << ">>> 匹配成功！索引" << matchedIndex << "-> 源代码行" << d->currentMacroLine;
                } else {
                    qDebug() << ">>> 匹配失败！matchedIndex:" << matchedIndex;
                }
            } else {
                qDebug() << ">>> CastSkills 返回 SKILL_DEFAULT（无技能施放）";
            }
        } catch (const std::exception &e) {
            qDebug() << "CastSkills 抛出异常:" << e.what();
            d->finished = true;
            return false;
        } catch (...) {
            qDebug() << "CastSkills 抛出未知异常";
            d->finished = true;
            return false;
        }
    }

    // 6. 检查后序关键帧的状态变化（参考 key_frame.cpp:191-230）
    // 技能施放后可能改变冷却时间，需要重新排列 KeyFrame
    JX3DPS::KeyFrame::KeyFrameSequence checkedKeyFrameSequence;

    for (auto it = d->keyFrameSequence.begin(); it != d->keyFrameSequence.end();) {
        for (auto iter = it->second.begin(); iter != it->second.end();) {
            JX3DPS::KeyFrame::KeyFrame keyFrame;
            keyFrame.first = it->first;

            if (iter->first == JX3DPS::KeyFrame::KeyFrameType::SKILL) {
                keyFrame.first = d->player->skills[iter->second]->GetNextKeyFrame();
                keyFrame.second.push_back(std::make_pair(JX3DPS::KeyFrame::KeyFrameType::SKILL, iter->second));
            } else if (iter->first == JX3DPS::KeyFrame::KeyFrameType::BUFF) {
                keyFrame.first = d->player->buffs[iter->second]->GetNextKeyFrame();
                keyFrame.second.push_back(std::make_pair(JX3DPS::KeyFrame::KeyFrameType::BUFF, iter->second));
            }

            if (keyFrame.first != it->first) {
                // 时间改变了，需要重新插入
                checkedKeyFrameSequence.push_back(keyFrame);
                iter = it->second.erase(iter);
                continue;
            } else {
                ++iter;
            }
        }

        if (it->second.empty()) {
            it = d->keyFrameSequence.erase(it);
        } else {
            ++it;
        }
    }

    // 7. 合并检查后的关键帧（参考 key_frame.cpp:232-253）
    for (auto &keyFrame : checkedKeyFrameSequence) {
        JX3DPS::KeyFrame::InsertKeyFrame(d->keyFrameSequence, keyFrame);
    }

    if (d->keyFrameSequence.empty()) {
        d->finished = true;
    }

    return true;
}

DebugSimulator::PlayerState DebugSimulator::GetPlayerState() const
{
    PlayerState state = {};

    if (d->player) {
        state.lifePercent = d->player->GetLifePercent();
        state.manaPercent = d->player->GetManaPercent();
        state.qidian = d->player->GetQidian();
        state.rage = d->player->GetRage();
        state.energy = d->player->GetEnergy();

        JX3DPS::Id_t targetId = d->player->GetTargetId();
        state.targetId = static_cast<int>(targetId);
        state.currentFrame = d->currentFrame;
        state.currentSeconds = d->currentFrame / 16.0;
        state.currentMacro = d->currentMacro;
        state.lastSkill = d->lastSkill;

        // 获取目标状态
        if (d->targets && d->targets->find(targetId) != d->targets->end()) {
            state.targetLifePercent = (*d->targets)[targetId]->GetLifePercent();
        }

        state.currentMacroLine = d->currentMacroLine;
    }

    return state;
}

bool DebugSimulator::IsFinished() const
{
    return d->finished;
}

void DebugSimulator::Reset()
{
    if (d->player) {
        delete d->player;
        d->player = nullptr;
    }

    if (d->targets) {
        for (auto &[id, target] : *d->targets) {
            delete target;
        }
        delete d->targets;
        d->targets = nullptr;
    }

    d->exprSkillsHash.clear();
    d->exprEvents.clear();
    d->keyFrameSequence.clear();
    d->currentFrame = 0;
    d->finished = false;
    d->lastSkill.clear();
    d->currentMacro.clear();
}
