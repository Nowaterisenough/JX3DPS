#ifndef JX3DPS_MACRO_H
#define JX3DPS_MACRO_H

#include <functional>
#include <memory>

#include "player.h"
#include "target.h"

template <typename T, typename... Args>
using tuple_t = std::tuple<T, Args...>;

namespace JX3DPS {

/**
 * @brief 宏
 *  /cast [condition] skill
 *  /change [condition] macro
 *  /use [condition] item
 */
class Macro
{
    void Run(Player &player, Targets &targets);

    enum NodeType
    {
        NODE_CONDITION,
        NODE_AND,
        NODE_OR
    };

    // 前向声明
    struct Node;

    // 树节点指针类型
    using NodePtr = std::unique_ptr<Node>;

    // 树节点结构
    struct Node
    {
        NodeType              type;
        NodePtr               left;
        NodePtr               right;
        std::function<bool()> condition; // 末端节点的条件函数

        // 构造函数
        Node(NodeType t, NodePtr l = nullptr, NodePtr r = nullptr) :
            type(t), left(std::move(l)), right(std::move(r))
        {
        }

        // 设置条件函数
        void SetCondition(std::function<bool()> cond) { condition = cond; }

        // 递归遍历条件树
        bool Evaluate()
        {
            switch (type) {
                case NODE_CONDITION: return condition();
                case NODE_AND: return left->Evaluate() && right->Evaluate();
                case NODE_OR: return left->Evaluate() || right->Evaluate();
                default: return true;
            }
        }
    };

    using cond_t = Node;

    inline thread_local static int index = 0; // 当前宏的索引

private:
    vector_t<vector_t<tuple_t<cond_t, int>>> _macros_vec;
};

} // namespace JX3DPS

#endif // JX3DPS_MACRO_H