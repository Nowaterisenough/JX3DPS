#include <proxy/proxy.h>

#include <functional>
#include <iostream>
#include <memory>
#include <random>
#include <regex>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#include "Type.h"

using namespace JX3DPS;

constexpr static size_t UNIT_SIZE = 8;
constexpr static size_t TYPE_SIZE = 64;

struct alignas(32) FrameCache
{
    tick_t skill_cooldown[TYPE_SIZE] = {};
    tick_t skill_prepare[TYPE_SIZE]  = {};
    tick_t skill_casting[TYPE_SIZE]  = {};

    tick_t buff_cooldown[UNIT_SIZE][TYPE_SIZE] = {};
    tick_t buff_duration[UNIT_SIZE][TYPE_SIZE] = {};
    tick_t buff_interval[UNIT_SIZE][TYPE_SIZE] = {};
};

constexpr int PLAYER = 0;

struct Context
{
    int  bind_cooldown_index = 0;
    jx3id_t target_id           = 0;

    FrameCache frame_cache; 

    tick_t skill_cooldown[TYPE_SIZE]         = {};
    int     skill_energy[TYPE_SIZE]           = {};
    int     buff_stacks[UNIT_SIZE][TYPE_SIZE] = {};

    void Update(tick_t tick) noexcept;
    void Reset() noexcept;
};

Context context;

jx3id_t GetBuffId(const std::string &buff)
{
    return 0;
}

jx3id_t GetSkillId(const std::string &skill)
{
    return 0;
}

struct LessThan
{
    bool operator()(int a, int b) const { return a < b; }
};

struct LessEqual
{
    bool operator()(int a, int b) const { return a <= b; }
};

struct Equal
{
    bool operator()(int a, int b) const { return a == b; }
};

struct NotEqual
{
    bool operator()(int a, int b) const { return a != b; }
};

struct GreaterThan
{
    bool operator()(int a, int b) const { return a > b; }
};

struct GreaterEqual
{
    bool operator()(int a, int b) const { return a >= b; }
};

struct Exists
{
    bool operator()(int a) const { return a > 0; }
};

struct NotExists
{
    bool operator()(int a) const { return a == 0; }
};

// 各种条件类模板 - 直接实现 facade 方法
template <typename Comparator>
class BuffExistsConditionProxy
{
    jx3id_t _buff;
    Comparator _comp;

public:
    BuffExistsConditionProxy(jx3id_t buff) noexcept : _buff(buff) {}

    bool Evaluate() const noexcept
    {
        return _comp(context.frame_cache.buff_duration[PLAYER][_buff]);
    }
    
    std::string GetAction() const noexcept { return ""; }
};

template <typename Comp>
using BuffExistsProxy = BuffExistsConditionProxy<Comp>;

template <typename Comparator>
class TBuffExistsConditionProxy
{
    jx3id_t _buff;
    Comparator _comp;

public:
    TBuffExistsConditionProxy(jx3id_t buff) noexcept : _buff(buff) {}

    bool Evaluate() const noexcept
    {
        return _comp(context.frame_cache.buff_duration[context.target_id][_buff]);
    }
    
    std::string GetAction() const noexcept { return ""; }
};

template <typename Comp>
using TBuffExistsProxy = TBuffExistsConditionProxy<Comp>;

template <typename Comparator>
class BuffDurationConditionProxy
{
private:
    jx3id_t _buff;
    tick_t _duration;
    Comparator _comp;

public:
    BuffDurationConditionProxy(jx3id_t buff, tick_t duration) noexcept :
        _buff(buff), _duration(duration) {}

    bool Evaluate() const noexcept
    {
        return _comp(context.frame_cache.buff_duration[PLAYER][_buff], _duration);
    }
    
    std::string GetAction() const noexcept { return ""; }
};

template <typename Comp>
using BuffDurationProxy = BuffDurationConditionProxy<Comp>;

template <typename Comparator>
class TBuffDurationConditionProxy
{
private:
    jx3id_t _buff;
    tick_t _duration;
    Comparator _comp;

public:
    TBuffDurationConditionProxy(jx3id_t buff, tick_t duration) noexcept :
        _buff(buff), _duration(duration) {}

    bool Evaluate() const noexcept
    {
        return _comp(context.frame_cache.buff_duration[context.target_id][_buff], _duration);
    }
    
    std::string GetAction() const noexcept { return ""; }
};

template <typename Comp>
using TBuffDurationProxy = TBuffDurationConditionProxy<Comp>;

template <typename Comparator>
class BuffStacksConditionProxy
{
private:
    jx3id_t _buff;
    int _stacks;
    Comparator _comp;

public:
    BuffStacksConditionProxy(jx3id_t buff, int stacks) noexcept : _buff(buff), _stacks(stacks) {}

    bool Evaluate() const noexcept { return _comp(context.buff_stacks[PLAYER][_buff], _stacks); }
    std::string GetAction() const noexcept { return ""; }
};

template <typename Comp>
using BuffStacksProxy = BuffStacksConditionProxy<Comp>;

template <typename Comparator>
class TBuffStacksConditionProxy
{
private:
    jx3id_t _buff;
    int _stacks;
    Comparator _comp;

public:
    TBuffStacksConditionProxy(jx3id_t buff, int stacks) noexcept : _buff(buff), _stacks(stacks) {}

    bool Evaluate() const noexcept
    {
        return _comp(context.buff_stacks[context.target_id][_buff], _stacks);
    }
    
    std::string GetAction() const noexcept { return ""; }
};

template <typename Comp>
using TBuffStacksProxy = TBuffStacksConditionProxy<Comp>;

template <typename Comparator>
class SkillCooldownConditionProxy
{
private:
    jx3id_t _skill;
    tick_t _cooldown;
    Comparator _comp;

public:
    SkillCooldownConditionProxy(jx3id_t skill, tick_t cooldown) noexcept :
        _skill(skill), _cooldown(cooldown) {}

    bool Evaluate() const noexcept { return _comp(context.skill_cooldown[_skill], _cooldown); }
    std::string GetAction() const noexcept { return ""; }
};

template <typename Comp>
using SkillCooldownProxy = SkillCooldownConditionProxy<Comp>;

template <typename Comparator>
class SkillEnergyConditionProxy
{
private:
    jx3id_t _skill;
    int _stacks;
    Comparator _comp;

public:
    SkillEnergyConditionProxy(jx3id_t skill, int stacks) noexcept : _skill(skill), _stacks(stacks) {}

    bool Evaluate() const noexcept { return _comp(context.skill_energy[_skill], _stacks); }
    std::string GetAction() const noexcept { return ""; }
};

template <typename Comp>
using SkillEnergyProxy = SkillEnergyConditionProxy<Comp>;

// 定义 facade（只定义一次）
namespace spec {
    PRO_DEF_MEM_DISPATCH(MemEvaluate, Evaluate);
    PRO_DEF_MEM_DISPATCH(MemGetAction, GetAction);
    
    struct AstNode : pro::facade_builder::add_convention<MemEvaluate, bool() const>
                                       ::add_convention<MemGetAction, std::string() const>::build {};
}

enum class NodeType : uint8_t
{
    CONDITION,
    ACTION,
    AND,
    OR
};

enum class ErrorCode
{
    SUCCESS,
    INVALID_CONDITION,
    INVALID_MACRO_FORMAT,
};

// 直接实现各种节点类，避免包装
struct ActionNodeImpl
{
    std::string _action;

    explicit ActionNodeImpl(std::string action) noexcept : _action(std::move(action)) {}

    bool Evaluate() const noexcept { return true; }
    std::string GetAction() const noexcept { return _action; }
};

struct AndNodeImpl
{
    pro::proxy<spec::AstNode> left;
    pro::proxy<spec::AstNode> right;

    AndNodeImpl(pro::proxy<spec::AstNode> left, pro::proxy<spec::AstNode> right) noexcept :
        left(std::move(left)), right(std::move(right)) {}

    bool Evaluate() const noexcept { return left->Evaluate() && right->Evaluate(); }

    std::string GetAction() const noexcept
    {
        std::string left_action = left->GetAction();
        if (!left_action.empty()) {
            return left_action;
        }
        return right->GetAction();
    }
};

struct OrNodeImpl
{
    pro::proxy<spec::AstNode> left;
    pro::proxy<spec::AstNode> right;

    OrNodeImpl(pro::proxy<spec::AstNode> left, pro::proxy<spec::AstNode> right) noexcept :
        left(std::move(left)), right(std::move(right)) {}

    bool Evaluate() const noexcept { return left->Evaluate() || right->Evaluate(); }

    std::string GetAction() const noexcept
    {
        if (left->Evaluate()) {
            return left->GetAction();
        }
        return right->GetAction();
    }
};