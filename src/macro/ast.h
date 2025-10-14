#ifndef JX3DPS_MACRO_AST_H
#define JX3DPS_MACRO_AST_H

#include "src/core/context.h"
#include "src/global/types.h"
#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace JX3DPS {

/**
 * @brief 比较运算符
 */
struct Comparator {
    struct LessThan {
        bool operator()(int a, int b) const { return a < b; }
    };

    struct LessEqual {
        bool operator()(int a, int b) const { return a <= b; }
    };

    struct Equal {
        bool operator()(int a, int b) const { return a == b; }
    };

    struct NotEqual {
        bool operator()(int a, int b) const { return a != b; }
    };

    struct GreaterThan {
        bool operator()(int a, int b) const { return a > b; }
    };

    struct GreaterEqual {
        bool operator()(int a, int b) const { return a >= b; }
    };

    struct Exists {
        bool operator()(int a) const { return a > 0; }
    };

    struct NotExists {
        bool operator()(int a) const { return a == 0; }
    };
};

/**
 * @brief 条件评估器基类
 */
class ConditionEvaluator {
public:
    virtual ~ConditionEvaluator() = default;
    virtual bool Evaluate() const = 0;
};

/**
 * @brief BUFF存在条件
 */
template <typename Comp>
class BuffExistsCondition : public ConditionEvaluator {
    jx3id_t m_buff_id;
    Comp    m_comp;

public:
    explicit BuffExistsCondition(jx3id_t buff_id) : m_buff_id(buff_id) {}

    bool Evaluate() const override {
        return m_comp(context.cache.buff_duration[0][m_buff_id]);
    }
};

/**
 * @brief 目标BUFF存在条件
 */
template <typename Comp>
class TBuffExistsCondition : public ConditionEvaluator {
    jx3id_t m_buff_id;
    Comp    m_comp;

public:
    explicit TBuffExistsCondition(jx3id_t buff_id) : m_buff_id(buff_id) {}

    bool Evaluate() const override {
        return m_comp(context.cache.buff_duration[context.targets.begin()->first][m_buff_id]);
    }
};

/**
 * @brief BUFF持续时间条件
 */
template <typename Comp>
class BuffDurationCondition : public ConditionEvaluator {
    jx3id_t m_buff_id;
    tick_t  m_duration;
    Comp    m_comp;

public:
    BuffDurationCondition(jx3id_t buff_id, tick_t duration)
        : m_buff_id(buff_id), m_duration(duration) {}

    bool Evaluate() const override {
        return m_comp(context.cache.buff_duration[0][m_buff_id], m_duration);
    }

    // 获取时间条件信息
    jx3id_t GetBuffId() const { return m_buff_id; }
    tick_t GetThreshold() const { return m_duration; }
    bool IsGreaterThan() const {
        return std::is_same_v<Comp, Comparator::GreaterThan> ||
               std::is_same_v<Comp, Comparator::GreaterEqual>;
    }
};

/**
 * @brief 目标BUFF持续时间条件
 */
template <typename Comp>
class TBuffDurationCondition : public ConditionEvaluator {
    jx3id_t m_buff_id;
    tick_t  m_duration;
    Comp    m_comp;

public:
    TBuffDurationCondition(jx3id_t buff_id, tick_t duration)
        : m_buff_id(buff_id), m_duration(duration) {}

    bool Evaluate() const override {
        return m_comp(context.cache.buff_duration[context.targets.begin()->first][m_buff_id], m_duration);
    }

    // 获取时间条件信息
    jx3id_t GetBuffId() const { return m_buff_id; }
    tick_t GetThreshold() const { return m_duration; }
    bool IsGreaterThan() const {
        return std::is_same_v<Comp, Comparator::GreaterThan> ||
               std::is_same_v<Comp, Comparator::GreaterEqual>;
    }
};

/**
 * @brief 技能冷却条件
 */
template <typename Comp>
class SkillCooldownCondition : public ConditionEvaluator {
    jx3id_t m_skill_id;
    tick_t  m_cooldown;
    Comp    m_comp;

public:
    SkillCooldownCondition(jx3id_t skill_id, tick_t cooldown)
        : m_skill_id(skill_id), m_cooldown(cooldown) {}

    bool Evaluate() const override {
        return m_comp(context.cache.skill_cooldown[m_skill_id], m_cooldown);
    }

    // 获取时间条件信息
    jx3id_t GetSkillId() const { return m_skill_id; }
    tick_t GetThreshold() const { return m_cooldown; }
    bool IsGreaterThan() const {
        return std::is_same_v<Comp, Comparator::GreaterThan> ||
               std::is_same_v<Comp, Comparator::GreaterEqual>;
    }
};

/**
 * @brief AST 节点类型
 */
enum class NodeType {
    CONDITION, // 条件节点
    ACTION,    // 动作节点
    AND,       // 与节点
    OR         // 或节点
};

/**
 * @brief 时间条件（用于生成关键帧）
 */
struct TimeCondition {
    enum class Type {
        BUFF_DURATION,   // bufftime
        TBUFF_DURATION,  // tbufftime
        SKILL_COOLDOWN   // skill_cd
    };

    Type    type;
    jx3id_t id;        // BUFF ID 或技能 ID
    tick_t  threshold; // 时间阈值
    bool    is_greater; // true = >, false = <

    TimeCondition(Type t, jx3id_t id_, tick_t threshold_, bool is_greater_)
        : type(t), id(id_), threshold(threshold_), is_greater(is_greater_) {}
};

/**
 * @brief AST 节点基类
 */
class AstNode {
public:
    virtual ~AstNode() = default;
    virtual bool Evaluate() const = 0;
    virtual std::string GetAction() const { return ""; }
    virtual NodeType GetType() const = 0;

    /**
     * @brief 提取所有时间条件（用于生成关键帧）
     */
    virtual void ExtractTimeConditions(std::vector<TimeCondition> &conditions) const {}
};

/**
 * @brief 条件节点
 */
class ConditionNode : public AstNode {
    std::unique_ptr<ConditionEvaluator> m_evaluator;

public:
    explicit ConditionNode(std::unique_ptr<ConditionEvaluator> evaluator)
        : m_evaluator(std::move(evaluator)) {}

    bool Evaluate() const override {
        return m_evaluator->Evaluate();
    }

    NodeType GetType() const override {
        return NodeType::CONDITION;
    }
};

/**
 * @brief 动作节点
 */
class ActionNode : public AstNode {
    std::string m_action;

public:
    explicit ActionNode(std::string action) : m_action(std::move(action)) {}

    bool Evaluate() const override {
        return true;
    }

    std::string GetAction() const override {
        return m_action;
    }

    NodeType GetType() const override {
        return NodeType::ACTION;
    }
};

/**
 * @brief 与节点 (AND)
 */
class AndNode : public AstNode {
    std::unique_ptr<AstNode> m_left;
    std::unique_ptr<AstNode> m_right;

public:
    AndNode(std::unique_ptr<AstNode> left, std::unique_ptr<AstNode> right)
        : m_left(std::move(left)), m_right(std::move(right)) {}

    bool Evaluate() const override {
        return m_left->Evaluate() && m_right->Evaluate();
    }

    std::string GetAction() const override {
        // 优先返回右节点的动作 (通常是技能名)
        std::string action = m_right->GetAction();
        if (!action.empty()) {
            return action;
        }
        return m_left->GetAction();
    }

    NodeType GetType() const override {
        return NodeType::AND;
    }
};

/**
 * @brief 或节点 (OR)
 */
class OrNode : public AstNode {
    std::unique_ptr<AstNode> m_left;
    std::unique_ptr<AstNode> m_right;

public:
    OrNode(std::unique_ptr<AstNode> left, std::unique_ptr<AstNode> right)
        : m_left(std::move(left)), m_right(std::move(right)) {}

    bool Evaluate() const override {
        return m_left->Evaluate() || m_right->Evaluate();
    }

    std::string GetAction() const override {
        // 根据条件返回对应的动作
        if (m_left->Evaluate()) {
            return m_left->GetAction();
        }
        return m_right->GetAction();
    }

    NodeType GetType() const override {
        return NodeType::OR;
    }
};

} // namespace JX3DPS

#endif // JX3DPS_MACRO_AST_H
