#ifndef JX3DPS_BUFF_BUFF_HPP
#define JX3DPS_BUFF_BUFF_HPP

#include "src/core/context.h"
#include "src/global/concepts.h"
#include "src/global/types.h"

namespace JX3DPS {

// 前向声明
template <typename T>
class Player;
struct Target;

/**
 * @brief BUFF基类 - 使用CRTP实现静态多态
 *
 * @tparam Derived 派生类类型
 *
 * 通过字段声明自动推导BUFF行为：
 * - interval: DOT间隔时间
 * - duration: BUFF持续时间
 * - max_stacks: 最大层数
 */
template <typename Derived>
class BuffImpl {
public:
    BuffImpl() = default;
    virtual ~BuffImpl() = default;

    /**
     * @brief 获取派生类引用
     */
    Derived& AsDerived() { return static_cast<Derived&>(*this); }
    const Derived& AsDerived() const { return static_cast<const Derived&>(*this); }

    /**
     * @brief BUFF添加
     */
    void Add(jx3id_t targetId, int stacks = 1) {
        // 调用派生类的Add方法（如果有）
        if constexpr (requires { std::declval<Derived>().AddImpl(targetId, stacks); }) {
            AsDerived().AddImpl(targetId, stacks);
        }
    }

    /**
     * @brief BUFF移除
     */
    void Remove(jx3id_t targetId, int stacks = 1) {
        if constexpr (requires { std::declval<Derived>().RemoveImpl(targetId, stacks); }) {
            AsDerived().RemoveImpl(targetId, stacks);
        }
    }

    /**
     * @brief BUFF清除
     */
    void Clear(jx3id_t targetId) {
        if constexpr (requires { std::declval<Derived>().ClearImpl(targetId); }) {
            AsDerived().ClearImpl(targetId);
        }
    }

    /**
     * @brief BUFF Tick（DOT跳）
     */
    void Tick(jx3id_t targetId) {
        if constexpr (requires { std::declval<Derived>().TickImpl(targetId); }) {
            AsDerived().TickImpl(targetId);
        }
    }

    /**
     * @brief 获取BUFF层数
     */
    int GetStacks(jx3id_t targetId) const {
        if constexpr (requires { std::declval<const Derived>().GetStacksImpl(targetId); }) {
            return AsDerived().GetStacksImpl(targetId);
        }
        return 0;
    }

    /**
     * @brief 注册事件处理器
     */
    template <typename... Args>
    void Register(Args&&... args) {
        // 事件注册占位符
        // TODO: 实现事件系统
    }

protected:
    // 内部状态（派生类可访问）
    struct BuffState {
        int stacks = 0;
        tick_t duration = 0;
        tick_t interval = 0;
    };

    hash_t<jx3id_t, BuffState> states_;  // 每个目标的BUFF状态
};

/**
 * @brief 检测BUFF是否有DOT属性
 */
template <typename T>
concept HasBuffInterval = requires(T t) {
    { T::interval } -> std::convertible_to<tick_t>;
};

/**
 * @brief 检测BUFF是否有持续时间
 */
template <typename T>
concept HasBuffDuration = requires(T t) {
    { T::duration } -> std::convertible_to<tick_t>;
};

/**
 * @brief 检测BUFF是否可堆叠
 */
template <typename T>
concept HasBuffStacks = requires(T t) {
    { T::max_stacks } -> std::convertible_to<int>;
};

} // namespace JX3DPS

#endif // JX3DPS_BUFF_BUFF_HPP
