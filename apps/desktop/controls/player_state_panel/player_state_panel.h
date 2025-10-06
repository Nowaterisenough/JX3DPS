#ifndef PLAYER_STATE_PANEL_H
#define PLAYER_STATE_PANEL_H

#include <QWidget>
#include <memory>

/**
 * @brief 玩家状态面板 - 显示调试时的玩家状态
 *
 * 数据驱动设计：
 * - 输入：PlayerState 结构体
 * - 输出：格式化的状态显示
 * - 无业务逻辑，纯UI展示
 */
class PlayerStatePanel : public QWidget
{
    Q_OBJECT

public:
    struct PlayerState {
        double lifePercent;
        double manaPercent;
        int qidian;
        int rage;
        int energy;
        int targetId;
        double targetLifePercent;
        int currentFrame;
        double currentSeconds;
        QString currentMacro;
        QString lastSkill;
    };

    explicit PlayerStatePanel(QWidget *parent = nullptr);
    ~PlayerStatePanel();

    void UpdateState(const PlayerState &state);
    void Clear();

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    struct Impl;
    std::unique_ptr<Impl> d;
};

#endif // PLAYER_STATE_PANEL_H
