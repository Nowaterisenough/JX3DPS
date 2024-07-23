#include <proxy.h>

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
    int  bind_cooldown_index = 0;
    id_t target_id           = 0;

    FrameCache frame_cache;

    frame_t skill_cooldown[TYPE_SIZE]         = {};
    int     skill_energy[TYPE_SIZE]           = {};
    int     buff_stacks[UNIT_SIZE][TYPE_SIZE] = {};

    void Update(frame_t frame) noexcept;
    void Reset() noexcept;
};

Context context;

id_t GetBuffId(const std::string &buff)
{
    return 0;
}

id_t GetSkillId(const std::string &skill)
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
    id_t       _buff;
    Comparator _comp;

public:
    BuffExistsConditionProxy(id_t buff) noexcept : _buff(buff) { }

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
    id_t       _buff;
    Comparator _comp;

public:
    TBuffExistsConditionProxy(id_t buff) noexcept : _buff(buff) { }

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
    id_t       _buff;
    frame_t    _duration;
    Comparator _comp;

public:
    BuffDurationConditionProxy(id_t buff, frame_t duration) noexcept :
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
    id_t       _buff;
    frame_t    _duration;
    Comparator _comp;

public:
    TBuffDurationConditionProxy(id_t buff, frame_t duration) noexcept :
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
    id_t       _buff;
    int        _stacks;
    Comparator _comp;

public:
    BuffStacksConditionProxy(id_t buff, int stacks) noexcept : _buff(buff), _stacks(stacks) { }

    bool Evaluate() const noexcept { return _comp(context.buff_stacks[PLAYER][_buff], _stacks); }
};

template <typename Comp>
using BuffStacksProxy = BuffStacksConditionProxy<Comp>;

template <typename Comparator>
class TBuffStacksConditionProxy
{
private:
    id_t       _buff;
    int        _stacks;
    Comparator _comp;

public:
    TBuffStacksConditionProxy(id_t buff, int stacks) noexcept : _buff(buff), _stacks(stacks) { }

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
    id_t       _skill;
    frame_t    _cooldown;
    Comparator _comp;

public:
    SkillCooldownConditionProxy(id_t skill, frame_t cooldown) noexcept :
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
    id_t       _skill;
    int        _stacks;
    Comparator _comp;

public:
    SkillEnergyConditionProxy(id_t skill, int stacks) noexcept : _skill(skill), _stacks(stacks) { }

    bool Evaluate() const noexcept { return _comp(context.skill_energy[_skill], _stacks); }
};

template <typename Comp>
using SkillEnergyProxy = SkillEnergyConditionProxy<Comp>;

namespace spec {

PRO_DEF_MEM_DISPATCH(MemEvaluate, Evaluate);

struct Evaluator : pro::facade_builder::add_convention<MemEvaluate, bool() const>::build
{
};

} // namespace spec

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

class Node
{
public:
    NodeType Type;

    explicit Node(NodeType type) : Type(type) { }

    virtual bool Evaluate() = 0;

    virtual std::string GetAction() const { return ""; }

    virtual ~Node() = default;
};

class ConditionNode : public Node
{
private:
    pro::proxy<spec::Evaluator> Proxy;

public:
    explicit ConditionNode(pro::proxy<spec::Evaluator> proxy) :
        Node(NodeType::CONDITION), Proxy(std::move(proxy))
    {
    }

    bool Evaluate() override { return Proxy->Evaluate(); }
};

class ActionNode : public Node
{
private:
    std::string Action;

public:
    explicit ActionNode(std::string action) : Node(NodeType::ACTION), Action(std::move(action)) { }

    bool Evaluate() override { return true; }

    std::string GetAction() const override { return Action; }
};

class AndNode : public Node
{
public:
    std::vector<std::unique_ptr<Node>> Children;

    AndNode() : Node(NodeType::AND) { }

    explicit AndNode(std::vector<std::unique_ptr<Node>> children) :
        Node(NodeType::AND), Children(std::move(children))
    {
    }

    bool Evaluate() override
    {
        for (const auto &child : Children) {
            if (!child->Evaluate()) {
                return false;
            }
        }
        return true;
    }
};

class OrNode : public Node
{
public:
    std::vector<std::unique_ptr<Node>> Children;

    OrNode() : Node(NodeType::OR) { }

    explicit OrNode(std::vector<std::unique_ptr<Node>> children) :
        Node(NodeType::OR), Children(std::move(children))
    {
    }

    bool Evaluate() override
    {
        for (const auto &child : Children) {
            if (child->Evaluate()) {
                return true;
            }
        }
        return false;
    }
};

class MacroInterpreter
{
private:
    inline static const std::regex buff_exists{ R"(buff:(\w+))" };
    inline static const std::regex nobuff_exists{ R"(nobuff:(\w+))" };
    inline static const std::regex buff_duration{ R"(bufftime:(\w+)(>|>=|<|<=|==|!=)(\d+))" };
    inline static const std::regex buff_stacks{ R"(buff:(\w+)(>|>=|<|<=|==|!=)(\d+))" };
    inline static const std::regex tbuff_exists{ R"(tbuff:(\w+))" };
    inline static const std::regex tnobuff_exists{ R"(tnobuff:(\w+))" };
    inline static const std::regex tbuff_duration{ R"(tbufftime:(\w+)(>|>=|<|<=|==|!=)(\d+))" };
    inline static const std::regex tbuff_stacks{ R"(tbuff:(\w+)(>|>=|<|<=|==|!=)(\d+))" };
    inline static const std::regex skill_cd{ R"(cd:(\w+)(>|>=|<|<=|==|!=)(\d+))" };
    inline static const std::regex skill_energy{ R"(skill_energy:(\w+)(>|>=|<|<=|==|!=)(\d+))" };

    template <typename ProxyType, typename... Args>
    std::pair<std::unique_ptr<Node>, ErrorCode> MakeConditionNode(Args... args)
    {
        return { std::make_unique<ConditionNode>(pro::make_proxy<spec::Evaluator>(ProxyType(args...))),
                 ErrorCode::SUCCESS };
    }

    template <template <typename> class ProxyType>
    std::pair<std::unique_ptr<Node>, ErrorCode> ParseComparisonCondition(id_t id, const std::string &op, int value)
    {
        if (op == "<") {
            return MakeConditionNode<ProxyType<LessThan>>(id, value);
        }
        if (op == "<=") {
            return MakeConditionNode<ProxyType<LessEqual>>(id, value);
        }
        if (op == "==") {
            return MakeConditionNode<ProxyType<Equal>>(id, value);
        }
        if (op == "!=") {
            return MakeConditionNode<ProxyType<NotEqual>>(id, value);
        }
        if (op == ">") {
            return MakeConditionNode<ProxyType<GreaterThan>>(id, value);
        }
        if (op == ">=") {
            return MakeConditionNode<ProxyType<GreaterEqual>>(id, value);
        }
        return { nullptr, ErrorCode::INVALID_CONDITION };
    }

public:
    std::pair<std::unique_ptr<Node>, ErrorCode> ParseMacro(const std::string &macro)
    {
        std::regex  macroPattern(R"(/cast\s*\[(.*?)\]\s*(\w+))");
        std::smatch match;
        if (!std::regex_match(macro, match, macroPattern)) {
            return { nullptr, ErrorCode::INVALID_MACRO_FORMAT };
        }

        std::string condition = match[1].str();
        std::string action    = match[2].str();

        if (condition.empty()) {
            return { std::make_unique<ActionNode>(action), ErrorCode::SUCCESS };
        }

        auto [conditionNode, error] = ParseCondition(condition);
        if (error != ErrorCode::SUCCESS) {
            return { nullptr, error };
        }

        auto andNode = std::make_unique<AndNode>();
        andNode->Children.push_back(std::move(conditionNode));
        andNode->Children.push_back(std::make_unique<ActionNode>(action));
        return { std::move(andNode), ErrorCode::SUCCESS };
    }

    std::pair<std::unique_ptr<Node>, ErrorCode> ParseCondition(const std::string &condition)
    {
        size_t orPos = condition.find('|');
        if (orPos != std::string::npos) {
            auto orNode                 = std::make_unique<OrNode>();
            auto [leftChild, leftError] = ParseCondition(condition.substr(0, orPos));
            if (leftError != ErrorCode::SUCCESS) {
                return { nullptr, leftError };
            }
            auto [rightChild, rightError] = ParseCondition(condition.substr(orPos + 1));
            if (rightError != ErrorCode::SUCCESS) {
                return { nullptr, rightError };
            }
            orNode->Children.push_back(std::move(leftChild));
            orNode->Children.push_back(std::move(rightChild));
            return { std::move(orNode), ErrorCode::SUCCESS };
        }

        size_t andPos = condition.find('&');
        if (andPos != std::string::npos) {
            auto andNode                = std::make_unique<AndNode>();
            auto [leftChild, leftError] = ParseCondition(condition.substr(0, andPos));
            if (leftError != ErrorCode::SUCCESS) {
                return { nullptr, leftError };
            }
            auto [rightChild, rightError] = ParseCondition(condition.substr(andPos + 1));
            if (rightError != ErrorCode::SUCCESS) {
                return { nullptr, rightError };
            }
            andNode->Children.push_back(std::move(leftChild));
            andNode->Children.push_back(std::move(rightChild));
            return { std::move(andNode), ErrorCode::SUCCESS };
        }

        return ParseSingleCondition(condition);
    }

    std::pair<std::unique_ptr<Node>, ErrorCode> ParseSingleCondition(const std::string &condition)
    {
        std::smatch match;

        if (std::regex_match(condition, match, buff_exists)) {
            return MakeConditionNode<BuffExistsProxy<Exists>>(GetBuffId(match[1].str()));
        }

        if (std::regex_match(condition, match, nobuff_exists)) {
            return MakeConditionNode<BuffExistsProxy<NotExists>>(GetBuffId(match[1].str()));
        }

        if (std::regex_match(condition, match, buff_duration)) {
            return ParseComparisonCondition<BuffDurationProxy>(GetBuffId(match[1].str()),
                                                               match[2].str(),
                                                               std::stoi(match[3].str()));
        }

        if (std::regex_match(condition, match, buff_stacks)) {
            return ParseComparisonCondition<BuffStacksProxy>(GetBuffId(match[1].str()),
                                                             match[2].str(),
                                                             std::stoi(match[3].str()));
        }

        if (std::regex_match(condition, match, tbuff_exists)) {
            return MakeConditionNode<TBuffExistsProxy<Exists>>(GetBuffId(match[1].str()));
        }

        if (std::regex_match(condition, match, tnobuff_exists)) {
            return MakeConditionNode<TBuffExistsProxy<NotExists>>(GetBuffId(match[1].str()));
        }

        if (std::regex_match(condition, match, tbuff_duration)) {
            return ParseComparisonCondition<TBuffDurationProxy>(GetBuffId(match[1].str()),
                                                                match[2].str(),
                                                                std::stoi(match[3].str()));
        }

        if (std::regex_match(condition, match, tbuff_stacks)) {
            return ParseComparisonCondition<TBuffStacksProxy>(GetBuffId(match[1].str()),
                                                              match[2].str(),
                                                              std::stoi(match[3].str()));
        }

        if (std::regex_match(condition, match, skill_cd)) {
            return ParseComparisonCondition<SkillCooldownProxy>(GetSkillId(match[1].str()),
                                                                match[2].str(),
                                                                std::stoi(match[3].str()));
        }

        if (std::regex_match(condition, match, skill_energy)) {
            return ParseComparisonCondition<SkillEnergyProxy>(GetSkillId(match[1].str()),
                                                              match[2].str(),
                                                              std::stoi(match[3].str()));
        }

        return { nullptr, ErrorCode::INVALID_CONDITION };
    }
};

int main()
{
    MacroInterpreter interpreter;

    std::vector<std::string> macros = { "/cast [buff:A&buff:F|cd:D<3] B", "/cast [nobuff:C|buff:A] D", "/cast [cd:B>2&nobuff:A] E" };

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
