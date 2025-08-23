#include <chrono>
#include <cstdint>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <memory>
#include <random>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

#if defined(_WIN32)
#    include <windows.h>
#endif

using namespace std;

// ========================== Domain Model ==========================
struct Snapshot
{
    float                sun  = 0.0f;
    float                moon = 0.0f;
    std::vector<uint8_t> hasBuff;
    std::vector<float>   buffTime;

    bool hasBuffId(int id) const noexcept
    {
        return id >= 0 && id < (int)hasBuff.size() ? hasBuff[id] != 0 : false;
    }

    float buffTimeId(int id) const noexcept
    {
        if (id < 0 || id >= (int)buffTime.size()) {
            return std::numeric_limits<float>::infinity();
        }
        float t = buffTime[id];
        return t >= 0.0f ? t : std::numeric_limits<float>::infinity();
    }
};

struct Decision
{
    int  skillID = -1;
    bool forced  = false;
};

// 防优化汇总
static uint64_t volatile g_sink = 0;

// ========================== Bytecode VM ==========================
enum class Op : uint8_t
{
    PUSH_CONST_F,
    PUSH_CONST_B,
    LOAD_SUN,
    LOAD_MOON,
    LOAD_HASBUFF,
    LOAD_BUFTIME,
    LT_FF,
    GT_FF,
    LE_FF,
    GE_FF,
    EQ_FF,
    NE_FF,
    NOT_B,
    AND_SHORT,
    OR_SHORT,
    JUMP,
    RULE_TEST_END,
    SELECT,
    SELECT_FORCED,
    END
};

struct Instr
{
    Op    op{};
    float f = 0.0f;
    int   a = 0;
};

struct Program
{
    std::vector<Instr> code;
};

struct VM
{
    static Decision execute(const Program &p, const Snapshot &s) noexcept
    {
        float fstack[64];
        bool  bstack[64];
        int   fp = -1, bp = -1;
        auto  popF = [&]() {
            return fstack[fp--];
        };
        auto popB = [&]() {
            return bstack[bp--];
        };
        auto pushF = [&](float v) {
            fstack[++fp] = v;
        };
        auto pushB = [&](bool v) {
            bstack[++bp] = v;
        };
        Decision  out{};
        int       pc = 0;
        const int N  = (int)p.code.size();
        while (pc < N) {
            const Instr &ins = p.code[pc];
            switch (ins.op) {
                case Op::PUSH_CONST_F:
                    pushF(ins.f);
                    pc++;
                    break;
                case Op::PUSH_CONST_B:
                    pushB(ins.a != 0);
                    pc++;
                    break;
                case Op::LOAD_SUN:
                    pushF(s.sun);
                    pc++;
                    break;
                case Op::LOAD_MOON:
                    pushF(s.moon);
                    pc++;
                    break;
                case Op::LOAD_HASBUFF:
                    pushB(s.hasBuffId(ins.a));
                    pc++;
                    break;
                case Op::LOAD_BUFTIME:
                    pushF(s.buffTimeId(ins.a));
                    pc++;
                    break;
                case Op::LT_FF:
                {
                    float b = popF(), a = popF();
                    pushB(a < b);
                    pc++;
                } break;
                case Op::GT_FF:
                {
                    float b = popF(), a = popF();
                    pushB(a > b);
                    pc++;
                } break;
                case Op::LE_FF:
                {
                    float b = popF(), a = popF();
                    pushB(a <= b);
                    pc++;
                } break;
                case Op::GE_FF:
                {
                    float b = popF(), a = popF();
                    pushB(a >= b);
                    pc++;
                } break;
                case Op::EQ_FF:
                {
                    float b = popF(), a = popF();
                    pushB(a == b);
                    pc++;
                } break;
                case Op::NE_FF:
                {
                    float b = popF(), a = popF();
                    pushB(a != b);
                    pc++;
                } break;
                case Op::NOT_B:
                {
                    bool a = popB();
                    pushB(!a);
                    pc++;
                } break;
                case Op::AND_SHORT:
                {
                    bool a = popB();
                    if (!a) {
                        pushB(false);
                        pc += ins.a;
                    } else {
                        pc++;
                    }
                } break;
                case Op::OR_SHORT:
                {
                    bool a = popB();
                    if (a) {
                        pushB(true);
                        pc += ins.a;
                    } else {
                        pc++;
                    }
                } break;
                case Op::JUMP: pc += ins.a; break;
                case Op::RULE_TEST_END:
                {
                    bool ok = popB();
                    if (!ok) {
                        pc += ins.a;
                    } else {
                        pc++;
                    }
                } break;
                case Op::SELECT: out = { ins.a, false }; return out;
                case Op::SELECT_FORCED: out = { ins.a, true }; return out;
                case Op::END: return out;
            }
        }
        return out;
    }
};

// ========================== Parser / AST ==========================
enum class TokKind
{
    End,
    Ident,
    Number,
    Sym
};

struct Tok
{
    TokKind k;
    string  s;
};

struct Lexer
{
    string_view z;
    size_t      i = 0;

    explicit Lexer(string_view v) : z(v) { }

    static bool isIdentStart(char c) { return isalpha((unsigned char)c) || (c & 0x80); }

    static bool isIdent(char c) { return isalnum((unsigned char)c) || c == '_' || (c & 0x80); }

    Tok next()
    {
        while (i < z.size() && isspace((unsigned char)z[i])) {
            ++i;
        }
        if (i >= z.size()) {
            return { TokKind::End, "" };
        }
        char c = z[i];
        if (isdigit((unsigned char)c)) {
            size_t j = i;
            while (j < z.size() && (isdigit((unsigned char)z[j]) || z[j] == '.')) {
                j++;
            }
            string s(z.substr(i, j - i));
            i = j;
            return { TokKind::Number, s };
        }
        if (isIdentStart(c)) {
            size_t j = i;
            while (j < z.size() && isIdent(z[j])) {
                j++;
            }
            string s(z.substr(i, j - i));
            i = j;
            return { TokKind::Ident, s };
        }
        i++;
        return { TokKind::Sym, string(1, c) };
    }
};
enum class NodeKind
{
    Sun,
    Moon,
    HasBuff,
    BuffTime,
    ConstF,
    ConstB,
    Not,
    LT,
    GT,
    LE,
    GE,
    EQ,
    NE,
    And,
    Or
};

struct Node
{
    NodeKind         kind;
    float            f = 0.0f;
    int              a = 0;
    unique_ptr<Node> l, r;
};

enum : int
{
    SK_破穴   = 101,
    SK_骤风令 = 102,
    SK_抟风令 = 103,
    SK_引窍   = 104,
    SK_劈风令 = 105,
    SK_截阳   = 106,
    SK_断脉   = 107
};

enum : int
{
    BF_神门     = 201,
    BF_令聘骤风 = 202
};

static unordered_map<string, int> kSkill = {
    { "破穴",   101 },
    { "骤风令", 102 },
    { "抟风令", 103 },
    { "引窍",   104 },
    { "劈风令", 105 },
    { "截阳",   106 },
    { "断脉",   107 }
};
static unordered_map<string, int> kBuff = {
    { "神门",     201 },
    { "令聘骤风", 202 }
};

struct Parser
{
    Lexer lex;
    Tok   cur;

    explicit Parser(string_view s) : lex(s) { cur = lex.next(); }

    void eat() { cur = lex.next(); }

    bool accept(string_view sym)
    {
        if (cur.k == TokKind::Sym && cur.s == sym) {
            eat();
            return true;
        }
        return false;
    }

    [[noreturn]] void err(const string &m) { throw runtime_error("Parse error: " + m); }

    unique_ptr<Node> parse()
    {
        if (cur.k == TokKind::End) {
            auto n  = make_unique<Node>();
            n->kind = NodeKind::ConstB;
            n->a    = 1;
            return n;
        }
        auto n = parseOr();
        if (cur.k != TokKind::End) {
            err("unexpected token: " + cur.s);
        }
        return n;
    }

    unique_ptr<Node> parseOr()
    {
        auto n = parseAnd();
        while (accept("|")) {
            auto r  = parseAnd();
            auto p  = make_unique<Node>();
            p->kind = NodeKind::Or;
            p->l    = std::move(n);
            p->r    = std::move(r);
            n       = std::move(p);
        }
        return n;
    }

    unique_ptr<Node> parseAnd()
    {
        auto n = parseUnary();
        while (accept("&")) {
            auto r  = parseUnary();
            auto p  = make_unique<Node>();
            p->kind = NodeKind::And;
            p->l    = std::move(n);
            p->r    = std::move(r);
            n       = std::move(p);
        }
        return n;
    }

    unique_ptr<Node> parseUnary()
    {
        if (accept("!")) {
            auto u  = parseUnary();
            auto n  = make_unique<Node>();
            n->kind = NodeKind::Not;
            n->l    = std::move(u);
            return n;
        }
        auto lhs = parsePrimary();
        if (cur.k == TokKind::Sym && (cur.s == "<" || cur.s == ">" || cur.s == "=" || cur.s == "!")) {
            string op = cur.s;
            eat();
            if ((op == "<" || op == ">") && cur.k == TokKind::Sym && cur.s == "=") {
                op += "=";
                eat();
            } else if ((op == "=" || op == "!") && cur.k == TokKind::Sym && cur.s == "=") {
                op += "=";
                eat();
            }
            auto rhs = parsePrimary();
            auto n   = make_unique<Node>();
            if (op == "<") {
                n->kind = NodeKind::LT;
            } else if (op == ">") {
                n->kind = NodeKind::GT;
            } else if (op == "<=") {
                n->kind = NodeKind::LE;
            } else if (op == "%3E=") {
                n->kind = NodeKind::GE; // HTML-escaped safety if pasted; correct below if needed
            } else if (op == "==") {
                n->kind = NodeKind::EQ;
            } else if (op == "!=") {
                n->kind = NodeKind::NE;
            } else if (op == ">=") {
                n->kind = NodeKind::GE; // fix potential paste issue
            } else {
                err("unknown comparator " + op);
            }
            n->l = std::move(lhs);
            n->r = std::move(rhs);
            return n;
        }
        return lhs;
    }

    unique_ptr<Node> parsePrimary()
    {
        if (accept("(")) {
            auto n = parseOr();
            if (!accept(")")) {
                err("missing ')'");
            }
            return n;
        }
        if (cur.k == TokKind::Number) {
            float v = stof(cur.s);
            eat();
            auto n  = make_unique<Node>();
            n->kind = NodeKind::ConstF;
            n->f    = v;
            return n;
        }
        if (cur.k == TokKind::Ident) {
            string id = cur.s;
            eat();
            if (id == "sun") {
                auto n  = make_unique<Node>();
                n->kind = NodeKind::Sun;
                return n;
            }
            if (id == "moon") {
                auto n  = make_unique<Node>();
                n->kind = NodeKind::Moon;
                return n;
            }
            if (id == "nobuff" || id == "bufftime") {
                if (!accept(":")) {
                    err("expected ':' after " + id);
                }
                if (cur.k != TokKind::Ident) {
                    err("expected identifier after " + id + ":");
                }
                string name = cur.s;
                eat();
                if (id == "nobuff") {
                    auto n  = make_unique<Node>();
                    n->kind = NodeKind::HasBuff;
                    auto it = kBuff.find(name);
                    if (it == kBuff.end()) {
                        err("unknown buff " + name);
                    }
                    n->a       = it->second;
                    auto notn  = make_unique<Node>();
                    notn->kind = NodeKind::Not;
                    notn->l    = std::move(n);
                    return notn;
                } else {
                    auto n  = make_unique<Node>();
                    n->kind = NodeKind::BuffTime;
                    auto it = kBuff.find(name);
                    if (it == kBuff.end()) {
                        err("unknown buff " + name);
                    }
                    n->a = it->second;
                    return n;
                }
            }
            err("unknown identifier: " + id);
        }
        err("unexpected token in primary");
        return {};
    }
};

// ========================== Bytecode compiler (VM) ==========================
static void compileNodeToInstr(const Node *n, std::vector<Instr> &out)
{
    switch (n->kind) {
        case NodeKind::Sun: out.push_back({ Op::LOAD_SUN }); break;
        case NodeKind::Moon: out.push_back({ Op::LOAD_MOON }); break;
        case NodeKind::HasBuff:
        {
            Instr ins{ Op::LOAD_HASBUFF };
            ins.a = n->a;
            out.push_back(ins);
        } break;
        case NodeKind::BuffTime:
        {
            Instr ins{ Op::LOAD_BUFTIME };
            ins.a = n->a;
            out.push_back(ins);
        } break;
        case NodeKind::ConstF:
        {
            Instr ins{ Op::PUSH_CONST_F };
            ins.f = n->f;
            out.push_back(ins);
        } break;
        case NodeKind::ConstB:
        {
            Instr ins{ Op::PUSH_CONST_B };
            ins.a = n->a;
            out.push_back(ins);
        } break;
        case NodeKind::Not:
        {
            compileNodeToInstr(n->l.get(), out);
            out.push_back({ Op::NOT_B });
        } break;
        case NodeKind::LT:
        case NodeKind::GT:
        case NodeKind::LE:
        case NodeKind::GE:
        case NodeKind::EQ:
        case NodeKind::NE:
        {
            compileNodeToInstr(n->l.get(), out);
            compileNodeToInstr(n->r.get(), out);
            Op op = Op::LT_FF;
            switch (n->kind) {
                case NodeKind::LT: op = Op::LT_FF; break;
                case NodeKind::GT: op = Op::GT_FF; break;
                case NodeKind::LE: op = Op::LE_FF; break;
                case NodeKind::GE: op = Op::GE_FF; break;
                case NodeKind::EQ: op = Op::EQ_FF; break;
                case NodeKind::NE: op = Op::NE_FF; break;
                default: break;
            }
            out.push_back({ op });
        } break;
        case NodeKind::And:
        {
            compileNodeToInstr(n->l.get(), out);
            Instr andi{ Op::AND_SHORT };
            andi.a      = 0;
            int and_pos = (int)out.size();
            out.push_back(andi);
            int b_start = (int)out.size();
            compileNodeToInstr(n->r.get(), out);
            int b_len      = (int)out.size() - b_start;
            out[and_pos].a = b_len + 0;
        } break;
        case NodeKind::Or:
        {
            compileNodeToInstr(n->l.get(), out);
            Instr ori{ Op::OR_SHORT };
            ori.a      = 0;
            int or_pos = (int)out.size();
            out.push_back(ori);
            int b_start = (int)out.size();
            compileNodeToInstr(n->r.get(), out);
            int b_len     = (int)out.size() - b_start;
            out[or_pos].a = b_len + 0;
        } break;
    }
}

struct RuleSrc
{
    string cond;
    string skill;
    bool   forced  = false;
    int    srcLine = -1;
};

static Program compileRulesToProgram(const std::vector<RuleSrc> &rules)
{
    Program prog;
    for (const auto &r : rules) {
        vector<Instr> condCode;
        if (r.cond.empty()) {
            condCode.push_back({ Op::PUSH_CONST_B, 0.0f, 1 });
        } else {
            Parser ps(r.cond);
            auto   ast = ps.parse();
            compileNodeToInstr(ast.get(), condCode);
        }
        for (auto &ins : condCode) {
            prog.code.push_back(ins);
        }
        Instr testEnd{ Op::RULE_TEST_END };
        testEnd.a = 2;
        prog.code.push_back(testEnd);
        int skillId = -1;
        if (auto it = kSkill.find(r.skill); it != kSkill.end()) {
            skillId = it->second;
        }
        if (skillId < 0) {
            throw runtime_error("Unknown skill: " + r.skill);
        }
        Instr sel{ r.forced ? Op::SELECT_FORCED : Op::SELECT };
        sel.a = skillId;
        prog.code.push_back(sel);
    }
    prog.code.push_back({ Op::END });
    return prog;
}

// ========================== FlatFunc（扁平+递归求值） ==========================
enum class FCOp : uint8_t
{
    ConstB,
    Not,
    And,
    Or,
    HasBuff,
    BuffTimeCmp,
    FieldCmpConst,
    FieldCmpField
};
enum class FCCmp2 : uint8_t
{
    LT,
    GT,
    LE,
    GE,
    EQ,
    NE
};
enum class FCField2 : uint8_t
{
    Sun,
    Moon
};

struct FCNode
{
    FCOp op;
    int  l = -1, r = -1;

    union
    {
        struct
        {
            uint8_t bval;
        } uConst;

        struct
        {
            int id;
        } uHasBuff;

        struct
        {
            int     id;
            uint8_t cmp;
            float   rhs;
        } uBuffTimeCmp;

        struct
        {
            uint8_t field;
            uint8_t cmp;
            float   cval;
        } uFieldConst;

        struct
        {
            uint8_t field;
            uint8_t cmp;
            uint8_t ofield;
        } uFieldField;
    } data{};
};

struct FlatCond
{
    vector<FCNode> nodes;
    int            root = -1;
};

static int pushNode(vector<FCNode> &out, const FCNode &nd)
{
    out.push_back(nd);
    return (int)out.size() - 1;
}

static FCNode mkConstB(bool v)
{
    FCNode n{};
    n.op               = FCOp::ConstB;
    n.data.uConst.bval = v ? 1 : 0;
    return n;
}

static FCNode mkNot(int l)
{
    FCNode n{};
    n.op = FCOp::Not;
    n.l  = l;
    return n;
}

static FCNode mkAnd(int l, int r)
{
    FCNode n{};
    n.op = FCOp::And;
    n.l  = l;
    n.r  = r;
    return n;
}

static FCNode mkOr(int l, int r)
{
    FCNode n{};
    n.op = FCOp::Or;
    n.l  = l;
    n.r  = r;
    return n;
}

static FCNode mkHasBuff(int id)
{
    FCNode n{};
    n.op               = FCOp::HasBuff;
    n.data.uHasBuff.id = id;
    return n;
}

static FCNode mkBuffTimeCmp(int id, FCCmp2 c, float rhs)
{
    FCNode n{};
    n.op                    = FCOp::BuffTimeCmp;
    n.data.uBuffTimeCmp.id  = id;
    n.data.uBuffTimeCmp.cmp = (uint8_t)c;
    n.data.uBuffTimeCmp.rhs = rhs;
    return n;
}

static FCNode mkFieldCmpConst(FCField2 f, FCCmp2 c, float v)
{
    FCNode n{};
    n.op                     = FCOp::FieldCmpConst;
    n.data.uFieldConst.field = (uint8_t)f;
    n.data.uFieldConst.cmp   = (uint8_t)c;
    n.data.uFieldConst.cval  = v;
    return n;
}

static FCNode mkFieldCmpField(FCField2 f, FCCmp2 c, FCField2 of)
{
    FCNode n{};
    n.op                      = FCOp::FieldCmpField;
    n.data.uFieldField.field  = (uint8_t)f;
    n.data.uFieldField.cmp    = (uint8_t)c;
    n.data.uFieldField.ofield = (uint8_t)of;
    return n;
}

static optional<FCField2> asField2(const Node *n)
{
    if (n->kind == NodeKind::Sun) {
        return FCField2::Sun;
    }
    if (n->kind == NodeKind::Moon) {
        return FCField2::Moon;
    }
    return nullopt;
}

static FCCmp2 toCmp2(NodeKind k)
{
    switch (k) {
        case NodeKind::LT: return FCCmp2::LT;
        case NodeKind::GT: return FCCmp2::GT;
        case NodeKind::LE: return FCCmp2::LE;
        case NodeKind::GE: return FCCmp2::GE;
        case NodeKind::EQ: return FCCmp2::EQ;
        case NodeKind::NE: return FCCmp2::NE;
        default: return FCCmp2::EQ;
    }
}

static int buildFlatRec(const Node *n, vector<FCNode> &out)
{
    switch (n->kind) {
        case NodeKind::ConstB: return pushNode(out, mkConstB(n->a != 0));
        case NodeKind::Not:
        {
            int l = buildFlatRec(n->l.get(), out);
            return pushNode(out, mkNot(l));
        }
        case NodeKind::And:
        {
            int l = buildFlatRec(n->l.get(), out);
            int r = buildFlatRec(n->r.get(), out);
            return pushNode(out, mkAnd(l, r));
        }
        case NodeKind::Or:
        {
            int l = buildFlatRec(n->l.get(), out);
            int r = buildFlatRec(n->r.get(), out);
            return pushNode(out, mkOr(l, r));
        }
        case NodeKind::HasBuff: return pushNode(out, mkHasBuff(n->a));
        case NodeKind::BuffTime: return pushNode(out, mkConstB(true));
        case NodeKind::Sun:
        case NodeKind::Moon:
        case NodeKind::ConstF: return pushNode(out, mkConstB(false));
        case NodeKind::LT:
        case NodeKind::GT:
        case NodeKind::LE:
        case NodeKind::GE:
        case NodeKind::EQ:
        case NodeKind::NE:
        {
            if (n->l->kind == NodeKind::BuffTime && n->r->kind == NodeKind::ConstF) {
                return pushNode(out, mkBuffTimeCmp(n->l->a, toCmp2(n->kind), n->r->f));
            }
            if (auto lf = asField2(n->l.get()); lf && n->r->kind == NodeKind::ConstF) {
                return pushNode(out, mkFieldCmpConst(*lf, toCmp2(n->kind), n->r->f));
            }
            if (auto lf = asField2(n->l.get())) {
                if (auto rf = asField2(n->r.get())) {
                    return pushNode(out, mkFieldCmpField(*lf, toCmp2(n->kind), *rf));
                }
            }
            return pushNode(out, mkConstB(false));
        }
    }
    return pushNode(out, mkConstB(false));
}

struct FlatRule
{
    FlatCond cond;
    int      skillId = -1;
    bool     forced  = false;
};

static vector<FlatRule> compileRulesToFlat(const vector<RuleSrc> &rules)
{
    vector<FlatRule> out;
    out.reserve(rules.size());
    for (auto &r : rules) {
        FlatCond c;
        if (r.cond.empty()) {
            c.root = pushNode(c.nodes, mkConstB(true));
        } else {
            Parser ps(r.cond);
            auto   ast = ps.parse();
            c.root     = buildFlatRec(ast.get(), c.nodes);
        }
        int skillId = -1;
        if (auto it = kSkill.find(r.skill); it != kSkill.end()) {
            skillId = it->second;
        }
        if (skillId < 0) {
            throw runtime_error("Unknown skill: " + r.skill);
        }
        out.push_back({ std::move(c), skillId, r.forced });
    }
    return out;
}

static inline bool evalFC(const FlatCond &c, int idx, const Snapshot &s) noexcept
{
    const FCNode &nd = c.nodes[idx];
    switch (nd.op) {
        case FCOp::ConstB: return nd.data.uConst.bval != 0;
        case FCOp::Not: return !evalFC(c, nd.l, s);
        case FCOp::And:
        {
            if (!evalFC(c, nd.l, s)) {
                return false;
            }
            return evalFC(c, nd.r, s);
        }
        case FCOp::Or:
        {
            if (evalFC(c, nd.l, s)) {
                return true;
            }
            return evalFC(c, nd.r, s);
        }
        case FCOp::HasBuff: return s.hasBuffId(nd.data.uHasBuff.id);
        case FCOp::BuffTimeCmp:
        {
            float a = s.buffTimeId(nd.data.uBuffTimeCmp.id);
            switch ((FCCmp2)nd.data.uBuffTimeCmp.cmp) {
                case FCCmp2::LT: return a < nd.data.uBuffTimeCmp.rhs;
                case FCCmp2::GT: return a > nd.data.uBuffTimeCmp.rhs;
                case FCCmp2::LE: return a <= nd.data.uBuffTimeCmp.rhs;
                case FCCmp2::GE: return a >= nd.data.uBuffTimeCmp.rhs;
                case FCCmp2::EQ: return a == nd.data.uBuffTimeCmp.rhs;
                case FCCmp2::NE: return a != nd.data.uBuffTimeCmp.rhs;
            }
            return false;
        }
        case FCOp::FieldCmpConst:
        {
            float lhs = (nd.data.uFieldConst.field == (uint8_t)FCField2::Sun) ? s.sun : s.moon,
                  rhs = nd.data.uFieldConst.cval;
            switch ((FCCmp2)nd.data.uFieldConst.cmp) {
                case FCCmp2::LT: return lhs < rhs;
                case FCCmp2::GT: return lhs > rhs;
                case FCCmp2::LE: return lhs <= rhs;
                case FCCmp2::GE: return lhs >= rhs;
                case FCCmp2::EQ: return lhs == rhs;
                case FCCmp2::NE: return lhs != rhs;
            }
            return false;
        }
        case FCOp::FieldCmpField:
        {
            float lhs = (nd.data.uFieldField.field == (uint8_t)FCField2::Sun) ? s.sun : s.moon;
            float rhs = (nd.data.uFieldField.ofield == (uint8_t)FCField2::Sun) ? s.sun : s.moon;
            switch ((FCCmp2)nd.data.uFieldField.cmp) {
                case FCCmp2::LT: return lhs < rhs;
                case FCCmp2::GT: return lhs > rhs;
                case FCCmp2::LE: return lhs <= rhs;
                case FCCmp2::GE: return lhs >= rhs;
                case FCCmp2::EQ: return lhs == rhs;
                case FCCmp2::NE: return lhs != rhs;
            }
            return false;
        }
    }
    return false;
}

static Decision executeFlat(const vector<FlatRule> &rules, const Snapshot &s)
{
    for (auto &r : rules) {
        if (evalFC(r.cond, r.cond.root, s)) {
            return { r.skillId, r.forced };
        }
    }
    return {};
}

// ========================== Macro parsing from file ==========================

static std::vector<RuleSrc> parseMacroLines(const string &src)
{
    std::vector<RuleSrc> out;
    istringstream        iss(src);
    string               line;
    int                  lineNo = 1;

    auto trim = [](string &s) {
        auto l = s.find_first_not_of(" \t\r\n");
        auto r = s.find_last_not_of(" \t\r\n");
        if (l == string::npos) {
            s.clear();
            return;
        }
        s = s.substr(l, r - l + 1);
    };

    while (getline(iss, line)) {
        trim(line);
        if (line.empty()) {
            lineNo++;
            continue;
        }
        if (line.rfind("/cast", 0) != 0 && line.rfind("/fcast", 0) != 0) {
            lineNo++;
            continue;
        }
        bool   forced = line.rfind("/fcast", 0) == 0;
        string rest   = line.substr(forced ? 6 : 5);
        trim(rest);
        string cond, skill;
        if (!rest.empty() && rest[0] == '[') {
            auto rb = rest.find(']');
            if (rb == string::npos) {
                throw runtime_error("Missing ] at line " + to_string(lineNo));
            }
            cond  = rest.substr(1, rb - 1);
            skill = rest.substr(rb + 1);
        } else {
            cond  = "";
            skill = rest;
        }
        trim(skill);
        out.push_back({ cond, skill, forced, lineNo });
        lineNo++;
    }
    return out;
}

static string readFileAll(const string &path)
{
    ifstream ifs(path);
    if (!ifs) {
        throw runtime_error("Failed to open file: " + path);
    }
    stringstream ss;
    ss << ifs.rdbuf();
    return ss.str();
}

// ========================== Bench & util ==========================
struct BenchResult
{
    double total_ms        = 0.0;
    double ns_per_decision = 0.0;
    size_t decisions       = 0;
};

static inline void consumeDecision(const Decision &d) noexcept
{
    g_sink += (uint64_t)((uint32_t)(d.skillID < 0 ? 0 : d.skillID) ^ (uint32_t)(d.forced ? 0x9E'37'79'B9u : 0));
}

static BenchResult benchVM(const Program &prog, const vector<Snapshot> &inputs, int rounds)
{
    auto   t0  = chrono::high_resolution_clock::now();
    size_t cnt = 0;
    for (int r = 0; r < rounds; ++r) {
        for (const auto &s : inputs) {
            auto d = VM::execute(prog, s);
            consumeDecision(d);
            cnt++;
        }
    }
    auto   t1 = chrono::high_resolution_clock::now();
    double ms = chrono::duration<double, milli>(t1 - t0).count();
    double ns = (ms * 1e6) / cnt;
    return { ms, ns, cnt };
}

static BenchResult benchFlat(const vector<FlatRule> &rules, const vector<Snapshot> &inputs, int rounds)
{
    auto   t0  = chrono::high_resolution_clock::now();
    size_t cnt = 0;
    for (int r = 0; r < rounds; ++r) {
        for (const auto &s : inputs) {
            auto d = executeFlat(rules, s);
            consumeDecision(d);
            cnt++;
        }
    }
    auto   t1 = chrono::high_resolution_clock::now();
    double ms = chrono::duration<double, milli>(t1 - t0).count();
    double ns = (ms * 1e6) / cnt;
    return { ms, ns, cnt };
}

static vector<Snapshot> makeRandomInputs(size_t N, uint32_t seed = 42)
{
    vector<Snapshot> v;
    v.reserve(N);
    std::mt19937                          rng(seed);
    std::uniform_real_distribution<float> sunD(0.0f, 100.0f), moonD(0.0f, 100.0f);
    std::bernoulli_distribution           hasBuffD(0.4);
    std::uniform_real_distribution<float> btD(0.0f, 30.0f);
    for (size_t i = 0; i < N; ++i) {
        Snapshot s;
        s.sun  = sunD(rng);
        s.moon = moonD(rng);
        s.hasBuff.resize(300, 0);
        s.buffTime.resize(300, -1.0f);
        s.hasBuff[BF_神门] = hasBuffD(rng) ? 1 : 0;
        if (std::bernoulli_distribution(0.5)(rng)) {
            s.buffTime[BF_令聘骤风] = btD(rng);
        }
        v.push_back(std::move(s));
    }
    return v;
}

// ========================== Main ==========================
int main(int argc, char **argv)
{
#if defined(_WIN32)
    SetConsoleOutputCP(CP_UTF8);
    std::ios::sync_with_stdio(false);
#endif
    try {
        string macroPath = (argc >= 2) ? string(argv[1]) : string("macro.txt");
        cout << "Reading macro from: " << macroPath << "\n";
        string macroText = readFileAll(macroPath);

        auto rules = parseMacroLines(macroText);
        if (rules.empty()) {
            cerr << "No rules parsed. Ensure macro.txt has /cast or /fcast lines.\n";
            return 1;
        }
        cout << "Rules parsed: " << rules.size() << "\n";

        // Compile two backends
        auto prog      = compileRulesToProgram(rules);
        auto flatRules = compileRulesToFlat(rules);
        cout << "Program size: " << prog.code.size()
             << " instructions, Flat rules: " << flatRules.size() << "\n";

        // Sanity
        {
            Snapshot s;
            s.sun  = 50;
            s.moon = 50;
            s.hasBuff.resize(300, 0);
            s.buffTime.resize(300, -1.0f);
            s.hasBuff[BF_神门] = 0;
            auto d1            = VM::execute(prog, s);
            auto d2            = executeFlat(flatRules, s);
            cout << "Sanity -> VM:" << d1.skillID << " Flat:" << d2.skillID << "\n";
            consumeDecision(d1);
            consumeDecision(d2);
        }

        // Bench
        size_t inputN = 20000;
        int    rounds = 50;
        auto   inputs = makeRandomInputs(inputN, 42);

        cout << "\nBenchmarking...\n";
        auto rVM   = benchVM(prog, inputs, rounds);
        auto rFlat = benchFlat(flatRules, inputs, rounds);

        cout << fixed << setprecision(3);
        cout << "VM:       total " << rVM.total_ms << " ms, ns/decision " << rVM.ns_per_decision
             << ", decisions " << rVM.decisions << "\n";
        cout << "FlatFunc: total " << rFlat.total_ms << " ms, ns/decision " << rFlat.ns_per_decision
             << ", decisions " << rFlat.decisions << "\n";
        cout << "Speedup VM/Flat: "
             << (rFlat.ns_per_decision > 0 ? rVM.ns_per_decision / rFlat.ns_per_decision : 0)
             << "x ( >1 => VM slower )\n";
        cout << "g_sink=" << (unsigned long long)g_sink << "\n";

    } catch (const exception &e) {
        cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    return 0;
}
