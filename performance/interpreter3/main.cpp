#include <iostream>
#include <regex>
#include <string>
#include <vector>

#include <proxy/v4/proxy.h>

#include "Type.h"

using namespace JX3DPS;

struct alignas(32) FrameCache
{
    static constexpr size_t UNIT_SIZE = 8;
    static constexpr size_t TYPE_SIZE = 64;

    frame_t skill_cooldown[TYPE_SIZE] = {};
    frame_t skill_prepare[TYPE_SIZE]  = {};
    frame_t skill_casting[TYPE_SIZE]  = {};

    frame_t buff_cooldown[UNIT_SIZE][TYPE_SIZE] = {};
    frame_t buff_duration[UNIT_SIZE][TYPE_SIZE] = {};
    frame_t buff_interval[UNIT_SIZE][TYPE_SIZE] = {};
};

constexpr int PLAYER = 0;

struct Context
{
    int     bind_cooldown_index = 0;
    jx3id_t target_id           = 0;

    FrameCache frame_cache;

    frame_t skill_cooldown[64] = {};
    int     skill_energy[64]   = {};
    int     buff_stacks[8][64] = {};

    void Update(frame_t frame) noexcept;
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

template <typename Comparator>
class BuffExistsConditionProxy
{
    jx3id_t    _buff;
    Comparator _comp;

public:
    BuffExistsConditionProxy(jx3id_t buff) noexcept : _buff(buff) { }

    bool Evaluate() const noexcept
    {
        return _comp(context.frame_cache.buff_duration[PLAYER][_buff]);
    }
};

template <typename Comp>
using BuffExistsProxy = BuffExistsConditionProxy<Comp>;

template <typename Comparator>
class TBuffExistsConditionProxy
{
    jx3id_t    _buff;
    Comparator _comp;

public:
    TBuffExistsConditionProxy(jx3id_t buff) noexcept : _buff(buff) { }

    bool Evaluate() const noexcept
    {
        return _comp(context.frame_cache.buff_duration[context.target_id][_buff]);
    }
};

template <typename Comp>
using TBuffExistsProxy = TBuffExistsConditionProxy<Comp>;

template <typename Comparator>
class BuffDurationConditionProxy
{
private:
    jx3id_t    _buff;
    frame_t    _duration;
    Comparator _comp;

public:
    BuffDurationConditionProxy(jx3id_t buff, frame_t duration) noexcept :
        _buff(buff), _duration(duration)
    {
    }

    bool Evaluate() const noexcept
    {
        return _comp(context.frame_cache.buff_duration[PLAYER][_buff], _duration);
    }
};

template <typename Comp>
using BuffDurationProxy = BuffDurationConditionProxy<Comp>;

template <typename Comparator>
class TBuffDurationConditionProxy
{
private:
    jx3id_t    _buff;
    frame_t    _duration;
    Comparator _comp;

public:
    TBuffDurationConditionProxy(jx3id_t buff, frame_t duration) noexcept :
        _buff(buff), _duration(duration)
    {
    }

    bool Evaluate() const noexcept
    {
        return _comp(context.frame_cache.buff_duration[context.target_id][_buff], _duration);
    }
};

template <typename Comp>
using TBuffDurationProxy = TBuffDurationConditionProxy<Comp>;

template <typename Comparator>
class BuffStacksConditionProxy
{
private:
    jx3id_t    _buff;
    int        _stacks;
    Comparator _comp;

public:
    BuffStacksConditionProxy(jx3id_t buff, int stacks) noexcept : _buff(buff), _stacks(stacks) { }

    bool Evaluate() const noexcept { return _comp(context.buff_stacks[PLAYER][_buff], _stacks); }
};

template <typename Comp>
using BuffStacksProxy = BuffStacksConditionProxy<Comp>;

template <typename Comparator>
class TBuffStacksConditionProxy
{
private:
    jx3id_t    _buff;
    int        _stacks;
    Comparator _comp;

public:
    TBuffStacksConditionProxy(jx3id_t buff, int stacks) noexcept : _buff(buff), _stacks(stacks) { }

    bool Evaluate() const noexcept
    {
        return _comp(context.buff_stacks[context.target_id][_buff], _stacks);
    }
};

template <typename Comp>
using TBuffStacksProxy = TBuffStacksConditionProxy<Comp>;

template <typename Comparator>
class SkillCooldownConditionProxy
{
private:
    jx3id_t    _skill;
    frame_t    _cooldown;
    Comparator _comp;

public:
    SkillCooldownConditionProxy(jx3id_t skill, frame_t cooldown) noexcept :
        _skill(skill), _cooldown(cooldown)
    {
    }

    bool Evaluate() const noexcept { return _comp(context.skill_cooldown[_skill], _cooldown); }
};

template <typename Comp>
using SkillCooldownProxy = SkillCooldownConditionProxy<Comp>;

template <typename Comparator>
class SkillEnergyConditionProxy
{
private:
    jx3id_t    _skill;
    int        _stacks;
    Comparator _comp;

public:
    SkillEnergyConditionProxy(jx3id_t skill, int stacks) noexcept : _skill(skill), _stacks(stacks)
    {
    }

    bool Evaluate() const noexcept { return _comp(context.skill_energy[_skill], _stacks); }
};

template <typename Comp>
using SkillEnergyProxy = SkillEnergyConditionProxy<Comp>;

using BuffExists      = BuffExistsProxy<Exists>;
using BuffNotExists   = BuffExistsProxy<NotExists>;
using TBuffExists     = TBuffExistsProxy<Exists>;
using TBuffNotExists  = TBuffExistsProxy<NotExists>;
using BuffDurationLt  = BuffDurationProxy<LessThan>;
using BuffDurationLe  = BuffDurationProxy<LessEqual>;
using BuffDurationEq  = BuffDurationProxy<Equal>;
using BuffDurationNe  = BuffDurationProxy<NotEqual>;
using BuffDurationGt  = BuffDurationProxy<GreaterThan>;
using BuffDurationGe  = BuffDurationProxy<GreaterEqual>;
using TBuffDurationLt = TBuffDurationProxy<LessThan>;
using TBuffDurationLe = TBuffDurationProxy<LessEqual>;
using TBuffDurationEq = TBuffDurationProxy<Equal>;
using TBuffDurationNe = TBuffDurationProxy<NotEqual>;
using TBuffDurationGt = TBuffDurationProxy<GreaterThan>;
using TBuffDurationGe = TBuffDurationProxy<GreaterEqual>;
using BuffStacksLt    = BuffStacksProxy<LessThan>;
using BuffStacksLe    = BuffStacksProxy<LessEqual>;
using BuffStacksEq    = BuffStacksProxy<Equal>;
using BuffStacksNe    = BuffStacksProxy<NotEqual>;
using BuffStacksGt    = BuffStacksProxy<GreaterThan>;
using BuffStacksGe    = BuffStacksProxy<GreaterEqual>;
using TBuffStacksLt   = TBuffStacksProxy<LessThan>;
using TBuffStacksLe   = TBuffStacksProxy<LessEqual>;
using TBuffStacksEq   = TBuffStacksProxy<Equal>;
using TBuffStacksNe   = TBuffStacksProxy<NotEqual>;
using TBuffStacksGt   = TBuffStacksProxy<GreaterThan>;
using TBuffStacksGe   = TBuffStacksProxy<GreaterEqual>;
using SkillCooldownLt = SkillCooldownProxy<LessThan>;
using SkillCooldownLe = SkillCooldownProxy<LessEqual>;
using SkillCooldownEq = SkillCooldownProxy<Equal>;
using SkillCooldownNe = SkillCooldownProxy<NotEqual>;
using SkillCooldownGt = SkillCooldownProxy<GreaterThan>;
using SkillCooldownGe = SkillCooldownProxy<GreaterEqual>;
using SkillEnergyLt   = SkillEnergyProxy<LessThan>;
using SkillEnergyLe   = SkillEnergyProxy<LessEqual>;
using SkillEnergyEq   = SkillEnergyProxy<Equal>;
using SkillEnergyNe   = SkillEnergyProxy<NotEqual>;
using SkillEnergyGt   = SkillEnergyProxy<GreaterThan>;
using SkillEnergyGe   = SkillEnergyProxy<GreaterEqual>;

namespace spec {

PRO_DEF_MEM_DISPATCH(MemEvaluate, Evaluate);

struct Evaluator : pro::facade_builder::add_convention<MemEvaluate, bool() const>::build
{
};

PRO_DEF_MEM_DISPATCH(MemGetAction, GetAction);

struct Node :
    pro::facade_builder::add_convention<MemEvaluate, bool() const>::
        add_convention<MemGetAction, std::string() const>::build
{
};

} // namespace spec

enum class ErrorCode : uint8_t
{
    SUCCESS,
    INVALID_CONDITION,
    INVALID_MACRO_FORMAT,
};

class ConditionProxy
{
private:
    pro::proxy<spec::Evaluator> _proxy;

public:
    explicit ConditionProxy(pro::proxy<spec::Evaluator> proxy) : _proxy(std::move(proxy)) { }

    bool Evaluate() const noexcept { return _proxy->Evaluate(); }

    std::string GetAction() const { return ""; }
};

class ActionProxy
{
private:
    std::string _action;

public:
    explicit ActionProxy(std::string action) : _action(std::move(action)) { }

    bool Evaluate() const noexcept { return true; }

    std::string GetAction() const { return _action; }
};

class AndProxy
{
private:
    std::vector<pro::proxy<spec::Node>> _children;

public:
    explicit AndProxy(std::vector<pro::proxy<spec::Node>> children) : _children(std::move(children))
    {
    }

    bool Evaluate() const noexcept
    {
        for (const auto &child : _children) {
            if (!child->Evaluate()) {
                return false;
            }
        }
        return true;
    }

    std::string GetAction() const { return ""; }
};

class OrProxy
{
private:
    std::vector<pro::proxy<spec::Node>> _children;

public:
    explicit OrProxy(std::vector<pro::proxy<spec::Node>> children) : _children(std::move(children))
    {
    }

    bool Evaluate() const noexcept
    {
        for (const auto &child : _children) {
            if (child->Evaluate()) {
                return true;
            }
        }
        return false;
    }

    std::string GetAction() const { return ""; }
};

class MacroInterpreter
{
private:
    static inline const std::regex BUFF_EXISTS{ R"(buff:(\w+))" };
    static inline const std::regex NOBUFF_EXISTS{ R"(nobuff:(\w+))" };
    static inline const std::regex BUFF_DURATION{ R"(bufftime:(\w+)(>|>=|<|<=|==|!=)(\d+))" };
    static inline const std::regex BUFF_STACKS{ R"(buff:(\w+)(>|>=|<|<=|==|!=)(\d+))" };
    static inline const std::regex TBUFF_EXISTS{ R"(tbuff:(\w+))" };
    static inline const std::regex TNOBUFF_EXISTS{ R"(tnobuff:(\w+))" };
    static inline const std::regex TBUFF_DURATION{ R"(tbufftime:(\w+)(>|>=|<|<=|==|!=)(\d+))" };
    static inline const std::regex TBUFF_STACKS{ R"(tbuff:(\w+)(>|>=|<|<=|==|!=)(\d+))" };
    static inline const std::regex SKILL_CD{ R"(cd:(\w+)(>|>=|<|<=|==|!=)(\d+))" };
    static inline const std::regex SKILL_ENERGY{ R"(skill_energy:(\w+)(>|>=|<|<=|==|!=)(\d+))" };

    template <typename ProxyType, typename... Args>
    pro::proxy<spec::Node> MakeConditionProxy(Args... args)
    {
        auto &&proxy = ConditionProxy(pro::make_proxy<spec::Evaluator>(ProxyType(args...)));
        return pro::make_proxy<spec::Node>(std::move(proxy));
    }

    template <template <typename> class ProxyType>
    pro::proxy<spec::Node> ParseComparisonCondition(jx3id_t id, const std::string &op, int value)
    {
        if (op == "<") {
            return MakeConditionProxy<ProxyType<LessThan>>(id, value);
        }
        if (op == "<=") {
            return MakeConditionProxy<ProxyType<LessEqual>>(id, value);
        }
        if (op == "==") {
            return MakeConditionProxy<ProxyType<Equal>>(id, value);
        }
        if (op == "!=") {
            return MakeConditionProxy<ProxyType<NotEqual>>(id, value);
        }
        if (op == ">") {
            return MakeConditionProxy<ProxyType<GreaterThan>>(id, value);
        }
        if (op == ">=") {
            return MakeConditionProxy<ProxyType<GreaterEqual>>(id, value);
        }
        return nullptr;
    }

public:
    std::pair<pro::proxy<spec::Node>, ErrorCode> ParseMacro(const std::string &macro)
    {
        std::regex  macroPattern(R"(/cast\s*\[(.*?)\]\s*(\w+))");
        std::smatch match;
        if (!std::regex_match(macro, match, macroPattern)) {
            return { nullptr, ErrorCode::INVALID_MACRO_FORMAT };
        }

        std::string condition = match[1].str();
        std::string action    = match[2].str();

        if (condition.empty()) {
            return { pro::make_proxy<spec::Node>(ActionProxy(action)), ErrorCode::SUCCESS };
        }

        auto [conditionNode, error] = ParseCondition(condition);
        if (error != ErrorCode::SUCCESS) {
            return { nullptr, error };
        }

        std::vector<pro::proxy<spec::Node>> children;
        children.push_back(std::move(conditionNode));
        children.push_back(pro::make_proxy<spec::Node>(ActionProxy(action)));
        return { pro::make_proxy<spec::Node>(AndProxy(std::move(children))), ErrorCode::SUCCESS };
    }

    std::pair<pro::proxy<spec::Node>, ErrorCode> ParseCondition(const std::string &condition)
    {
        size_t orPos = condition.find('|');
        if (orPos != std::string::npos) {
            std::vector<pro::proxy<spec::Node>> children;
            auto [leftChild, leftError] = ParseCondition(condition.substr(0, orPos));
            if (leftError != ErrorCode::SUCCESS) {
                return { nullptr, leftError };
            }
            auto [rightChild, rightError] = ParseCondition(condition.substr(orPos + 1));
            if (rightError != ErrorCode::SUCCESS) {
                return { nullptr, rightError };
            }
            children.push_back(std::move(leftChild));
            children.push_back(std::move(rightChild));
            return { pro::make_proxy<spec::Node>(OrProxy(std::move(children))), ErrorCode::SUCCESS };
        }

        size_t andPos = condition.find('&');
        if (andPos != std::string::npos) {
            std::vector<pro::proxy<spec::Node>> children;
            auto [leftChild, leftError] = ParseCondition(condition.substr(0, andPos));
            if (leftError != ErrorCode::SUCCESS) {
                return { nullptr, leftError };
            }
            auto [rightChild, rightError] = ParseCondition(condition.substr(andPos + 1));
            if (rightError != ErrorCode::SUCCESS) {
                return { nullptr, rightError };
            }
            children.push_back(std::move(leftChild));
            children.push_back(std::move(rightChild));
            return { pro::make_proxy<spec::Node>(AndProxy(std::move(children))), ErrorCode::SUCCESS };
        }

        return ParseSingleCondition(condition);
    }

    std::pair<pro::proxy<spec::Node>, ErrorCode> ParseSingleCondition(const std::string &condition)
    {
        std::smatch match;

        if (std::regex_match(condition, match, BUFF_EXISTS)) {
            return { MakeConditionProxy<BuffExistsProxy<Exists>>(GetBuffId(match[1].str())), ErrorCode::SUCCESS };
        }

        if (std::regex_match(condition, match, NOBUFF_EXISTS)) {
            return { MakeConditionProxy<BuffExistsProxy<NotExists>>(GetBuffId(match[1].str())), ErrorCode::SUCCESS };
        }

        if (std::regex_match(condition, match, BUFF_DURATION)) {
            return { ParseComparisonCondition<BuffDurationProxy>(GetBuffId(match[1].str()),
                                                                 match[2].str(),
                                                                 std::stoi(match[3].str())),
                     ErrorCode::SUCCESS };
        }

        if (std::regex_match(condition, match, BUFF_STACKS)) {
            return { ParseComparisonCondition<BuffStacksProxy>(GetBuffId(match[1].str()),
                                                               match[2].str(),
                                                               std::stoi(match[3].str())),
                     ErrorCode::SUCCESS };
        }

        if (std::regex_match(condition, match, TBUFF_EXISTS)) {
            return { MakeConditionProxy<TBuffExistsProxy<Exists>>(GetBuffId(match[1].str())), ErrorCode::SUCCESS };
        }

        if (std::regex_match(condition, match, TNOBUFF_EXISTS)) {
            return { MakeConditionProxy<TBuffExistsProxy<NotExists>>(GetBuffId(match[1].str())), ErrorCode::SUCCESS };
        }

        if (std::regex_match(condition, match, TBUFF_DURATION)) {
            return { ParseComparisonCondition<TBuffDurationProxy>(GetBuffId(match[1].str()),
                                                                  match[2].str(),
                                                                  std::stoi(match[3].str())),
                     ErrorCode::SUCCESS };
        }

        if (std::regex_match(condition, match, TBUFF_STACKS)) {
            return { ParseComparisonCondition<TBuffStacksProxy>(GetBuffId(match[1].str()),
                                                                match[2].str(),
                                                                std::stoi(match[3].str())),
                     ErrorCode::SUCCESS };
        }

        if (std::regex_match(condition, match, SKILL_CD)) {
            return { ParseComparisonCondition<SkillCooldownProxy>(GetSkillId(match[1].str()),
                                                                  match[2].str(),
                                                                  std::stoi(match[3].str())),
                     ErrorCode::SUCCESS };
        }

        if (std::regex_match(condition, match, SKILL_ENERGY)) {
            return { ParseComparisonCondition<SkillEnergyProxy>(GetSkillId(match[1].str()),
                                                                match[2].str(),
                                                                std::stoi(match[3].str())),
                     ErrorCode::SUCCESS };
        }

        return { nullptr, ErrorCode::INVALID_CONDITION };
    }
};

int main()
{
    MacroInterpreter interpreter;

    std::vector<std::string> macros = { "/cast B", "/cast [buff:A&buff:F|cd:D<3] B", "/cast [nobuff:C|buff:A] D", "/cast [cd:B>2&nobuff:A] E" };

    const int EVALUATION_COUNT = 1000000;

    for (const auto &macro : macros) {
        std::cout << "Parsing macro: " << macro << std::endl;
        auto [node, error] = interpreter.ParseMacro(macro);
        if (error != ErrorCode::SUCCESS) {
            std::cout << "Error parsing macro: " << static_cast<int>(error) << std::endl;
            continue;
        }

        int successCount = 0;
        for (int i = 0; i < EVALUATION_COUNT; ++i) {
            if (node->Evaluate()) {
                ++successCount;
            }
        }

        std::cout << "Evaluation success rate: "
                  << (static_cast<double>(successCount) / EVALUATION_COUNT * 100) << "%" << std::endl;
        std::cout << "Action: " << node->GetAction() << std::endl << std::endl;
    }

    return 0;
}
