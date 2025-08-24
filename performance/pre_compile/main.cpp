// main.cpp — JX3 宏解析/执行（VM/Flat/Closures）快版本
// 特性：
// - | 与 & 无优先级，右结合解析：A&B|C => A&(B|C)
// - 支持比较：=、>、<、>=、<=、~=（~= 表示不等于），兼容 ==、!=
// - 时间为整型 tick；sun/moon/rage/qidian/energy 均为整型
// - last_skill 为整型 ID
// - 三后端：VM（字节码）、Flat（预编译 FCNode IR）、Closures（谓词）一致

#include <chrono>
#include <cstdint>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <memory>
#include <optional>
#include <random>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>
#include <cctype>
#include <climits>

#if defined(_WIN32)
#    include <windows.h>
#endif

using namespace std;

// ========================== Domain Model ==========================
struct Snapshot
{
    // 百分比（0~1）
    float life = 1.0f;
    float mana = 1.0f;

    // 整型资源/点数
    int sun    = 0;  // 日灵/任脉
    int moon   = 0;  // 月魂/督脉
    int rage   = 0;
    int qidian = 0;
    int energy = 0;

    // 充盈判定
    bool sun_power  = false;
    bool moon_power = false;

    // 自身与目标 buff 与剩余时间（tick，-1 表示无；对外用 INT_MAX 代表“无”）
    vector<uint8_t> hasBuff;
    vector<int>     buffTime;
    vector<uint8_t> tHasBuff;
    vector<int>     tBuffTime;

    // 充能技能层数与上限：skillId -> 层数
    unordered_map<int,int> skill_energy;
    unordered_map<int,int> skill_energy_max;

    // 自身“技能/奇穴”：skillId -> 是否拥有
    unordered_map<int,bool> self_skill;

    // 最后一次成功释放的技能（整型 ID）
    int last_skill = -1;

    // 目标强度等级与周围敌人数
    int npclevel     = 1;
    int nearby_enemy = 0;

    // 技能是否不在 CD：skillId -> bool
    unordered_map<int,bool> skill_notin_cd;

    // 药性（-5~5）
    int yaoxing = 0;

    // 接口
    bool hasBuffId(int id) const noexcept
    {
        return id >= 0 && id < (int)hasBuff.size() ? hasBuff[id] != 0 : false;
    }
    int buffTimeId(int id) const noexcept
    {
        if (id < 0 || id >= (int)buffTime.size()) return INT_MAX;
        int t = buffTime[id];
        return t >= 0 ? t : INT_MAX;
    }
    bool thasBuffId(int id) const noexcept
    {
        return id >= 0 && id < (int)tHasBuff.size() ? tHasBuff[id] != 0 : false;
    }
    int tbuffTimeId(int id) const noexcept
    {
        if (id < 0 || id >= (int)tBuffTime.size()) return INT_MAX;
        int t = tBuffTime[id];
        return t >= 0 ? t : INT_MAX;
    }
    bool hasSelfSkill(int id) const noexcept
    {
        if (auto it = self_skill.find(id); it != self_skill.end()) return it->second;
        return false;
    }
    bool skillNotInCD(int id) const noexcept
    {
        if (auto it = skill_notin_cd.find(id); it != skill_notin_cd.end()) return it->second;
        return true; // 未知默认可用
    }
    int skillEnergy(int id) const noexcept
    {
        if (auto it = skill_energy.find(id); it != skill_energy.end()) return it->second;
        return 0;
    }
    int skillEnergyMax(int id) const noexcept
    {
        if (auto it = skill_energy_max.find(id); it != skill_energy_max.end()) return it->second;
        return 0;
    }
};

struct Decision
{
    int  skillID = -1;
    bool forced  = false;
    bool operator==(const Decision &o) const noexcept { return skillID == o.skillID && forced == o.forced; }
    bool operator!=(const Decision &o) const noexcept { return !(*this == o); }
};

// 防优化
static uint64_t volatile g_sink = 0;

// ========================== Name Mappings（示例，可扩展） ==========================
static unordered_map<string,int> kSkill = {
    { "破穴",   101 }, { "骤风令", 102 }, { "抟风令", 103 },
    { "引窍",   104 }, { "劈风令", 105 }, { "截阳",   106 },
    { "断脉",   107 },
    { "太极无极", 20001 }, { "振翅图南", 20002 }, { "疾如风", 20003 },
    { "赤日轮", 20004 }, { "幽月轮", 20005 },
    { "生死劫", 20006 }, { "净世破魔击", 20007 },
    { "盾飞", 20008 }, { "西楚悲歌", 20009 }, { "云飞玉皇", 20010 },
    { "钟灵毓秀", 20011 }, { "闹须弥", 20012 },
    { "风来吴山", 20013 }, { "撼如雷", 20014 },
    { "川乌射罔", 20015 }
};
static unordered_map<string,int> kBuff = {
    { "神门",     201 },
    { "令聘骤风", 202 },
    { "梦悠", 301 }, { "牧云", 302 }, { "太极", 303 }, { "流血", 304 }
};

static int nameToSkillId(const string &nameOrNum)
{
    bool allDigit = !nameOrNum.empty();
    for (char c : nameOrNum) {
        if (!isdigit((unsigned char)c)) { allDigit = false; break; }
    }
    if (allDigit) return stoi(nameOrNum);
    if (auto it = kSkill.find(nameOrNum); it != kSkill.end()) return it->second;
    return -1;
}
static int nameToBuffId(const string &name)
{
    if (auto it = kBuff.find(name); it != kBuff.end()) return it->second;
    return -1;
}

// ========================== VM（优化字节码） ==========================
enum class Op : uint8_t
{
    // 常量
    PUSH_CONST_F,    // float 常数
    PUSH_CONST_I,    // int 常数
    PUSH_CONST_B,

    // 字段加载
    LOAD_LIFE, LOAD_MANA,     // float
    LOAD_SUN_I, LOAD_MOON_I,  // int
    LOAD_RAGE_I, LOAD_QIDIAN_I, LOAD_ENERGY_I, // int
    LOAD_NPCLEVEL_I, LOAD_NEARBY_ENEMY_I,      // int
    LOAD_YAOXING_I,                              // int
    LOAD_LAST_SKILL_I,                           // int

    // 技能充能
    LOAD_SKILL_ENERGY_I,       // a=skillId -> int
    LOAD_SKILL_ENERGY_MAX_I,   // a=skillId -> int

    // bufftime（tick）
    LOAD_BUFTIME_I,            // a=buffId -> int
    LOAD_T_BUFTIME_I,          // a=buffId -> int

    // 比较（float）
    LT_FF, GT_FF, LE_FF, GE_FF, EQ_FF, NE_FF,
    // 比较（int）
    LT_II, GT_II, LE_II, GE_II, EQ_II, NE_II,

    // 布尔
    NOT_B, AND_BB, OR_BB,

    // 布尔测试
    TEST_HASBUFF, TEST_NOBUFF,
    TEST_T_HASBUFF, TEST_T_NOBUFF,
    TEST_SUN_POWER, TEST_MOON_POWER,
    TEST_SELF_SKILL, TEST_SELF_NOSKILL,
    TEST_SKILL_NOTIN_CD,

    // 控制流
    RULE_TEST_END,
    SELECT, SELECT_FORCED,
    END
};

enum class CmpOp : uint8_t { LT, GT, LE, GE, EQ, NE };

struct Instr
{
    Op    op{};
    float f   = 0.0f;
    int   a   = 0;
    CmpOp cmp = CmpOp::LT;
};

struct Program { vector<Instr> code; };

static inline bool doCmpF(float a, float b, CmpOp c) noexcept
{
    switch (c) {
        case CmpOp::LT: return a < b;
        case CmpOp::GT: return a > b;
        case CmpOp::LE: return a <= b;
        case CmpOp::GE: return a >= b;
        case CmpOp::EQ: return a == b;
        case CmpOp::NE: return a != b;
    }
    return false;
}
static inline bool doCmpI(int a, int b, CmpOp c) noexcept
{
    switch (c) {
        case CmpOp::LT: return a < b;
        case CmpOp::GT: return a > b;
        case CmpOp::LE: return a <= b;
        case CmpOp::GE: return a >= b;
        case CmpOp::EQ: return a == b;
        case CmpOp::NE: return a != b;
    }
    return false;
}

struct VM
{
    static Decision execute(const Program &p, const Snapshot &s) noexcept
    {
        float fstack[64]; int istack[64]; bool bstack[64];
        int fp=-1, ip=-1, bp=-1;
        auto pushF=[&](float v){ fstack[++fp]=v; };
        auto popF=[&](){ return fstack[fp--]; };
        auto pushI=[&](int v){ istack[++ip]=v; };
        auto popI=[&](){ return istack[ip--]; };
        auto pushB=[&](bool v){ bstack[++bp]=v; };
        auto popB=[&](){ return bstack[bp--]; };

        Decision out{};
        int pc=0, N=(int)p.code.size();
        while (pc<N) {
            const Instr &ins=p.code[pc];
            switch (ins.op) {
                case Op::PUSH_CONST_F: pushF(ins.f); pc++; break;
                case Op::PUSH_CONST_I: pushI(ins.a); pc++; break;
                case Op::PUSH_CONST_B: pushB(ins.a!=0); pc++; break;

                case Op::LOAD_LIFE: pushF(s.life); pc++; break;
                case Op::LOAD_MANA: pushF(s.mana); pc++; break;

                case Op::LOAD_SUN_I: pushI(s.sun); pc++; break;
                case Op::LOAD_MOON_I: pushI(s.moon); pc++; break;
                case Op::LOAD_RAGE_I: pushI(s.rage); pc++; break;
                case Op::LOAD_QIDIAN_I: pushI(s.qidian); pc++; break;
                case Op::LOAD_ENERGY_I: pushI(s.energy); pc++; break;

                case Op::LOAD_NPCLEVEL_I: pushI(s.npclevel); pc++; break;
                case Op::LOAD_NEARBY_ENEMY_I: pushI(s.nearby_enemy); pc++; break;
                case Op::LOAD_YAOXING_I: pushI(s.yaoxing); pc++; break;
                case Op::LOAD_LAST_SKILL_I: pushI(s.last_skill); pc++; break;

                case Op::LOAD_SKILL_ENERGY_I: pushI(s.skillEnergy(ins.a)); pc++; break;
                case Op::LOAD_SKILL_ENERGY_MAX_I: pushI(s.skillEnergyMax(ins.a)); pc++; break;

                case Op::LOAD_BUFTIME_I: pushI(s.buffTimeId(ins.a)); pc++; break;
                case Op::LOAD_T_BUFTIME_I: pushI(s.tbuffTimeId(ins.a)); pc++; break;

                case Op::LT_FF: { float b=popF(), a=popF(); pushB(a<b); pc++; } break;
                case Op::GT_FF: { float b=popF(), a=popF(); pushB(a>b); pc++; } break;
                case Op::LE_FF: { float b=popF(), a=popF(); pushB(a<=b); pc++; } break;
                case Op::GE_FF: { float b=popF(), a=popF(); pushB(a>=b); pc++; } break;
                case Op::EQ_FF: { float b=popF(), a=popF(); pushB(a==b); pc++; } break;
                case Op::NE_FF: { float b=popF(), a=popF(); pushB(a!=b); pc++; } break;

                case Op::LT_II: { int b=popI(), a=popI(); pushB(a<b); pc++; } break;
                case Op::GT_II: { int b=popI(), a=popI(); pushB(a>b); pc++; } break;
                case Op::LE_II: { int b=popI(), a=popI(); pushB(a<=b); pc++; } break;
                case Op::GE_II: { int b=popI(), a=popI(); pushB(a>=b); pc++; } break;
                case Op::EQ_II: { int b=popI(), a=popI(); pushB(a==b); pc++; } break;
                case Op::NE_II: { int b=popI(), a=popI(); pushB(a!=b); pc++; } break;

                case Op::NOT_B: { bool a=popB(); pushB(!a); pc++; } break;
                case Op::AND_BB: { bool b=popB(), a=popB(); pushB(a&&b); pc++; } break;
                case Op::OR_BB: { bool b=popB(), a=popB(); pushB(a||b); pc++; } break;

                case Op::TEST_HASBUFF: pushB(s.hasBuffId(ins.a)); pc++; break;
                case Op::TEST_NOBUFF: pushB(!s.hasBuffId(ins.a)); pc++; break;
                case Op::TEST_T_HASBUFF: pushB(s.thasBuffId(ins.a)); pc++; break;
                case Op::TEST_T_NOBUFF: pushB(!s.thasBuffId(ins.a)); pc++; break;
                case Op::TEST_SUN_POWER: pushB(s.sun_power); pc++; break;
                case Op::TEST_MOON_POWER: pushB(s.moon_power); pc++; break;
                case Op::TEST_SELF_SKILL: pushB(s.hasSelfSkill(ins.a)); pc++; break;
                case Op::TEST_SELF_NOSKILL: pushB(!s.hasSelfSkill(ins.a)); pc++; break;
                case Op::TEST_SKILL_NOTIN_CD: pushB(s.skillNotInCD(ins.a)); pc++; break;

                case Op::RULE_TEST_END:
                {
                    bool ok=popB();
                    if (!ok) { pc += 2; } else { pc++; }
                } break;

                case Op::SELECT: out = { ins.a, false }; return out;
                case Op::SELECT_FORCED: out = { ins.a, true }; return out;
                case Op::END: return out;
            }
        }
        return out;
    }
};

// ========================== Lexer / Parser ==========================
enum class TokKind { End, Ident, Number, Sym };

struct Tok { TokKind k; string s; };

struct Lexer
{
    string_view z; size_t i=0;
    explicit Lexer(string_view v): z(v) {}

    static bool isIdentStart(char c){ return isalpha((unsigned char)c) || (c & 0x80) || c=='_'; }
    static bool isIdent(char c){ return isalnum((unsigned char)c) || c=='_' || (c & 0x80); }

    Tok next()
    {
        while (i<z.size() && isspace((unsigned char)z[i])) ++i;
        if (i>=z.size()) return { TokKind::End, "" };
        char c=z[i];

        // number（支持整数和小数；小数只用于 life/mana）
        if (isdigit((unsigned char)c) || (c=='.' && i+1<z.size() && isdigit((unsigned char)z[i+1]))) {
            size_t j=i; bool dot=false;
            while (j<z.size() && (isdigit((unsigned char)z[j]) || (!dot && z[j]=='.'))) { if (z[j]=='.') dot=true; ++j; }
            string s(z.substr(i,j-i)); i=j;
            return { TokKind::Number, s };
        }
        if (isIdentStart(c)) {
            size_t j=i; while (j<z.size() && isIdent(z[j])) ++j;
            string s(z.substr(i,j-i)); i=j;
            return { TokKind::Ident, s };
        }
        if (i+1<z.size()) {
            string two(z.substr(i,2));
            if (two==">=" || two=="<=" || two=="~=" || two=="==" || two=="!=") { i+=2; return { TokKind::Sym, two }; }
        }
        ++i; return { TokKind::Sym, string(1,c) };
    }
};

enum class NodeKind
{
    // 常量
    ConstF, ConstI, ConstB,

    // 字段/原子
    Life, Mana,            // float
    Sun, Moon, Rage, Qidian, Energy, NpcLevel, NearbyEnemy, Yaoxing, LastSkill, // int
    HasBuff, BuffTime, THasBuff, TBuffTime,
    SunPower, MoonPower,
    SelfSkill, SelfNoSkill, SkillNotInCD,
    SkillEnergy, SkillEnergyMax,

    // 一元
    Not,

    // 比较
    LT, GT, LE, GE, EQ, NE,

    // 逻辑
    And, Or
};

struct Node
{
    NodeKind         kind;
    float            f = 0.0f;  // float 常量
    int              a = 0;     // 整型常量 / id
    unique_ptr<Node> l, r;
};

static CmpOp nodeToCmp(NodeKind k)
{
    switch (k) {
        case NodeKind::LT: return CmpOp::LT;
        case NodeKind::GT: return CmpOp::GT;
        case NodeKind::LE: return CmpOp::LE;
        case NodeKind::GE: return CmpOp::GE;
        case NodeKind::EQ: return CmpOp::EQ;
        case NodeKind::NE: return CmpOp::NE;
        default: return CmpOp::EQ;
    }
}

struct Parser
{
    Lexer lex; Tok cur;
    explicit Parser(string_view s): lex(s) { cur=lex.next(); }
    void eat(){ cur=lex.next(); }
    bool accept(const string& sym){ if (cur.k==TokKind::Sym && cur.s==sym){ eat(); return true; } return false; }
    [[noreturn]] void err(const string& m){ throw runtime_error("Parse error: "+m); }

    unique_ptr<Node> parse()
    {
        if (cur.k==TokKind::End) { auto n=make_unique<Node>(); n->kind=NodeKind::ConstB; n->a=1; return n; }
        auto n = parseRightAssoc();
        if (cur.k!=TokKind::End) err("unexpected token: "+cur.s);
        return n;
    }

    // 右结合：A&B|C -> A&(B|C)
    unique_ptr<Node> parseRightAssoc()
    {
        auto right = parseUnaryOrPrim();
        if (cur.k==TokKind::Sym && (cur.s=="&" || cur.s=="|")) {
            string op = cur.s; eat();
            auto rest = parseRightAssoc();
            auto n=make_unique<Node>();
            n->kind = (op=="&")?NodeKind::And:NodeKind::Or;
            n->l = std::move(right);
            n->r = std::move(rest);
            return n;
        }
        return right;
    }

    unique_ptr<Node> parseUnaryOrPrim()
    {
        if (accept("!")) {
            auto u=parseUnaryOrPrim();
            auto n=make_unique<Node>(); n->kind=NodeKind::Not; n->l=std::move(u); return n;
        }
        if (accept("(")) {
            auto e=parseRightAssoc();
            if (!accept(")")) err("missing ')'");
            return e;
        }
        // primary 或 primary cmp primary
        auto lhs = parsePrimary();
        if (isCmpStart()) {
            NodeKind ck = parseCmpKind();
            auto rhs = parsePrimary();
            auto n=make_unique<Node>(); n->kind=ck; n->l=std::move(lhs); n->r=std::move(rhs);
            return n;
        }
        return lhs;
    }

    bool isCmpStart()
    {
        if (cur.k!=TokKind::Sym) return false;
        string s=cur.s;
        return (s=="<"||s==">"||s=="="||s=="<="||s==">="||s=="~="||s=="=="||s=="!=");
    }
    NodeKind parseCmpKind()
    {
        if (cur.k!=TokKind::Sym) err("expect comparator");
        string op=cur.s; eat();
        if (op=="<") return NodeKind::LT;
        if (op==">") return NodeKind::GT;
        if (op=="<=") return NodeKind::LE;
        if (op==">=") return NodeKind::GE;
        if (op=="=" || op=="==") return NodeKind::EQ;
        if (op=="~=" || op=="!=") return NodeKind::NE;
        err("unknown comparator "+op);
        return NodeKind::EQ;
    }

    unique_ptr<Node> parsePrimary()
    {
        if (cur.k==TokKind::Number) {
            string t=cur.s; eat();
            if (t.find('.') != string::npos) {
                auto n=make_unique<Node>(); n->kind=NodeKind::ConstF; n->f=stof(t); return n;
            } else {
                auto n=make_unique<Node>(); n->kind=NodeKind::ConstI; n->a=stoi(t); return n;
            }
        }
        if (cur.k==TokKind::Ident) {
            string id=cur.s; eat();

            auto make = [&](NodeKind k){ auto n=make_unique<Node>(); n->kind=k; return n; };

            // 简单字段
            if (id=="life") return make(NodeKind::Life);
            if (id=="mana") return make(NodeKind::Mana);
            if (id=="sun") return make(NodeKind::Sun);
            if (id=="moon") return make(NodeKind::Moon);
            if (id=="rage") return make(NodeKind::Rage);
            if (id=="qidian") return make(NodeKind::Qidian);
            if (id=="energy") return make(NodeKind::Energy);
            if (id=="npclevel") return make(NodeKind::NpcLevel);
            if (id=="nearby_enemy") return make(NodeKind::NearbyEnemy);
            if (id=="yaoxing") return make(NodeKind::Yaoxing);
            if (id=="sun_power") return make(NodeKind::SunPower);
            if (id=="moon_power") return make(NodeKind::MoonPower);
            if (id=="last_skill") return make(NodeKind::LastSkill);

            // 需要冒号的
            if (id=="buff" || id=="nobuff" || id=="bufftime" ||
                id=="tbuff"|| id=="tnobuff"|| id=="tbufftime" ||
                id=="skill_energy" || id=="skill" || id=="noskill" ||
                id=="skill_notin_cd")
            {
                if (!accept(":")) err("expected ':' after "+id);
                if (cur.k!=TokKind::Ident && cur.k!=TokKind::Number) err("expected identifier or number after "+id+":");

                string name = cur.s; eat();

                if (id=="buff") {
                    int bid = nameToBuffId(name);
                    if (bid<0) err("unknown buff "+name);
                    auto n=make_unique<Node>(); n->kind=NodeKind::HasBuff; n->a=bid; return n;
                }
                if (id=="nobuff") {
                    int bid = nameToBuffId(name);
                    if (bid<0) err("unknown buff "+name);
                    auto n=make_unique<Node>(); n->kind=NodeKind::HasBuff; n->a=bid;
                    auto notn=make_unique<Node>(); notn->kind=NodeKind::Not; notn->l=std::move(n); return notn;
                }
                if (id=="bufftime") {
                    int bid=nameToBuffId(name); if (bid<0) err("unknown buff "+name);
                    auto n=make_unique<Node>(); n->kind=NodeKind::BuffTime; n->a=bid; return n;
                }
                if (id=="tbuff") {
                    int bid=nameToBuffId(name); if (bid<0) err("unknown buff "+name);
                    auto n=make_unique<Node>(); n->kind=NodeKind::THasBuff; n->a=bid; return n;
                }
                if (id=="tnobuff") {
                    int bid=nameToBuffId(name); if (bid<0) err("unknown buff "+name);
                    auto n=make_unique<Node>(); n->kind=NodeKind::THasBuff; n->a=bid;
                    auto notn=make_unique<Node>(); notn->kind=NodeKind::Not; notn->l=std::move(n); return notn;
                }
                if (id=="tbufftime") {
                    int bid=nameToBuffId(name); if (bid<0) err("unknown buff "+name);
                    auto n=make_unique<Node>(); n->kind=NodeKind::TBuffTime; n->a=bid; return n;
                }
                if (id=="skill_energy") {
                    int sid=nameToSkillId(name); if (sid<0) err("unknown skill for skill_energy: "+name);
                    auto n=make_unique<Node>(); n->kind=NodeKind::SkillEnergy; n->a=sid; return n;
                }
                if (id=="skill") {
                    int sid=nameToSkillId(name); if (sid<0) err("unknown id/name for skill: "+name);
                    auto n=make_unique<Node>(); n->kind=NodeKind::SelfSkill; n->a=sid; return n;
                }
                if (id=="noskill") {
                    int sid=nameToSkillId(name); if (sid<0) err("unknown id/name for noskill: "+name);
                    auto n=make_unique<Node>(); n->kind=NodeKind::SelfNoSkill; n->a=sid; return n;
                }
                if (id=="skill_notin_cd") {
                    int sid=nameToSkillId(name); if (sid<0) err("unknown id/name for skill_notin_cd: "+name);
                    auto n=make_unique<Node>(); n->kind=NodeKind::SkillNotInCD; n->a=sid; return n;
                }
            }

            err("unknown identifier: "+id);
        }
        err("unexpected token in primary");
        return {};
    }
};

// ========================== Bytecode Compiler ==========================
static void emitCmpFF(vector<Instr> &out, CmpOp c){ Op op=Op::EQ_FF; switch(c){
    case CmpOp::LT: op=Op::LT_FF; break; case CmpOp::GT: op=Op::GT_FF; break;
    case CmpOp::LE: op=Op::LE_FF; break; case CmpOp::GE: op=Op::GE_FF; break;
    case CmpOp::EQ: op=Op::EQ_FF; break; case CmpOp::NE: op=Op::NE_FF; break; }
    out.push_back({op}); }
static void emitCmpII(vector<Instr> &out, CmpOp c){ Op op=Op::EQ_II; switch(c){
    case CmpOp::LT: op=Op::LT_II; break; case CmpOp::GT: op=Op::GT_II; break;
    case CmpOp::LE: op=Op::LE_II; break; case CmpOp::GE: op=Op::GE_II; break;
    case CmpOp::EQ: op=Op::EQ_II; break; case CmpOp::NE: op=Op::NE_II; break; }
    out.push_back({op}); }

static void compileNodeToInstr(const Node *n, vector<Instr> &out)
{
    switch (n->kind) {
        case NodeKind::ConstF: out.push_back({ Op::PUSH_CONST_F, n->f }); break;
        case NodeKind::ConstI: { Instr x{Op::PUSH_CONST_I}; x.a=n->a; out.push_back(x); } break;
        case NodeKind::ConstB: { Instr x{Op::PUSH_CONST_B}; x.a=n->a; out.push_back(x); } break;

        case NodeKind::Life: out.push_back({ Op::LOAD_LIFE }); break;
        case NodeKind::Mana: out.push_back({ Op::LOAD_MANA }); break;

        case NodeKind::Sun: out.push_back({ Op::LOAD_SUN_I }); break;
        case NodeKind::Moon: out.push_back({ Op::LOAD_MOON_I }); break;
        case NodeKind::Rage: out.push_back({ Op::LOAD_RAGE_I }); break;
        case NodeKind::Qidian: out.push_back({ Op::LOAD_QIDIAN_I }); break;
        case NodeKind::Energy: out.push_back({ Op::LOAD_ENERGY_I }); break;
        case NodeKind::NpcLevel: out.push_back({ Op::LOAD_NPCLEVEL_I }); break;
        case NodeKind::NearbyEnemy: out.push_back({ Op::LOAD_NEARBY_ENEMY_I }); break;
        case NodeKind::Yaoxing: out.push_back({ Op::LOAD_YAOXING_I }); break;
        case NodeKind::LastSkill: out.push_back({ Op::LOAD_LAST_SKILL_I }); break;

        case NodeKind::HasBuff: { Instr x{ Op::TEST_HASBUFF }; x.a=n->a; out.push_back(x); } break;
        case NodeKind::THasBuff: { Instr x{ Op::TEST_T_HASBUFF }; x.a=n->a; out.push_back(x); } break;

        case NodeKind::BuffTime: { Instr x{ Op::LOAD_BUFTIME_I }; x.a=n->a; out.push_back(x); } break;
        case NodeKind::TBuffTime: { Instr x{ Op::LOAD_T_BUFTIME_I }; x.a=n->a; out.push_back(x); } break;

        case NodeKind::SunPower: out.push_back({ Op::TEST_SUN_POWER }); break;
        case NodeKind::MoonPower: out.push_back({ Op::TEST_MOON_POWER }); break;

        case NodeKind::SelfSkill: { Instr x{ Op::TEST_SELF_SKILL }; x.a=n->a; out.push_back(x); } break;
        case NodeKind::SelfNoSkill: { Instr x{ Op::TEST_SELF_NOSKILL }; x.a=n->a; out.push_back(x); } break;
        case NodeKind::SkillNotInCD: { Instr x{ Op::TEST_SKILL_NOTIN_CD }; x.a=n->a; out.push_back(x); } break;

        case NodeKind::SkillEnergy: { Instr x{ Op::LOAD_SKILL_ENERGY_I }; x.a=n->a; out.push_back(x); } break;
        case NodeKind::SkillEnergyMax: { Instr x{ Op::LOAD_SKILL_ENERGY_MAX_I }; x.a=n->a; out.push_back(x); } break;

        case NodeKind::Not:
            compileNodeToInstr(n->l.get(), out);
            out.push_back({ Op::NOT_B });
            break;

        case NodeKind::LT:
        case NodeKind::GT:
        case NodeKind::LE:
        case NodeKind::GE:
        case NodeKind::EQ:
        case NodeKind::NE:
        {
            CmpOp c = nodeToCmp(n->kind);
            // 通用：左值入栈，右值入栈，按类型选择比较
            auto isFloaty = [&](const Node* x)->bool{
                return (x->kind==NodeKind::ConstF || x->kind==NodeKind::Life || x->kind==NodeKind::Mana);
            };
            compileNodeToInstr(n->l.get(), out);
            compileNodeToInstr(n->r.get(), out);
            if (isFloaty(n->l.get()) || isFloaty(n->r.get())) emitCmpFF(out, c);
            else emitCmpII(out, c);
        } break;

        case NodeKind::And:
            compileNodeToInstr(n->l.get(), out);
            compileNodeToInstr(n->r.get(), out);
            out.push_back({ Op::AND_BB });
            break;
        case NodeKind::Or:
            compileNodeToInstr(n->l.get(), out);
            compileNodeToInstr(n->r.get(), out);
            out.push_back({ Op::OR_BB });
            break;
    }
}

// ========================== RuleSrc / Macro parsing ==========================
struct RuleSrc
{
    string cond;
    string skill;
    bool   forced  = false;
    int    srcLine = -1;
};

static vector<RuleSrc> parseMacroLines(const string &src)
{
    vector<RuleSrc> out; istringstream iss(src); string line; int lineNo=1;
    auto trim=[](string &s){
        auto l=s.find_first_not_of(" \t\r\n");
        auto r=s.find_last_not_of(" \t\r\n");
        if (l==string::npos){ s.clear(); return; }
        s=s.substr(l, r-l+1);
    };
    while (getline(iss,line)) {
        trim(line);
        if (line.empty()) { lineNo++; continue; }
        if (line.rfind("/cast",0)!=0 && line.rfind("/fcast",0)!=0) { lineNo++; continue; }
        bool forced=line.rfind("/fcast",0)==0;
        string rest=line.substr(forced?6:5); trim(rest);
        string cond, skill;
        if (!rest.empty() && rest[0]=='[') {
            auto rb=rest.find(']');
            if (rb==string::npos) throw runtime_error("Missing ] at line "+to_string(lineNo));
            cond = rest.substr(1, rb-1);
            skill = rest.substr(rb+1);
        } else { cond=""; skill=rest; }
        trim(skill);
        out.push_back({cond, skill, forced, lineNo});
        lineNo++;
    }
    return out;
}

static string readFileAll(const string &path)
{
    ifstream ifs(path);
    if (!ifs) throw runtime_error("Failed to open file: "+path);
    stringstream ss; ss<<ifs.rdbuf(); return ss.str();
}

static vector<vector<RuleSrc>> parseMacroSuites(const string &all)
{
    auto isBlankLine=[](const string &s){
        for (char c: s) { if (!isspace((unsigned char)c)) return false; }
        return true;
    };
    auto isDivider=[](const string &s){
        size_t l=0,r=s.size();
        while (l<r && isspace((unsigned char)s[l])) ++l;
        while (r>l && isspace((unsigned char)s[r-1])) --r;
        if (r-l>=3){
            bool allDash=true, allHash=true;
            for (size_t i=l;i<r;++i){ if (s[i]!='-') allDash=false; if (s[i]!='#') allHash=false; }
            return allDash||allHash;
        }
        return false;
    };
    auto trimAll=[&](string &s){
        size_t pos=0;
        auto blankSV=[](string_view r){ for(char c:r){ if(!isspace((unsigned char)c)) return false; } return true; };
        while (pos<s.size()){
            size_t nl=s.find('\n',pos);
            string_view row=(nl==string::npos)?string_view(s).substr(pos):string_view(s).substr(pos,nl-pos);
            if (!blankSV(row)) break;
            pos=(nl==string::npos)?s.size():nl+1;
        }
        s.erase(0,pos);
        while (!s.empty()){
            size_t p=s.find_last_of('\n');
            string_view tail=(p==string::npos)?string_view(s):string_view(s).substr(p+1);
            bool allsp=true; for(char c:tail){ if(!isspace((unsigned char)c)) { allsp=false; break; } }
            if (allsp){ if (p==string::npos){ s.clear(); break; } s.erase(p); } else break;
        }
    };

    vector<vector<RuleSrc>> suites;
    istringstream iss(all); string line, cur;
    auto flush=[&](){
        trimAll(cur);
        if (cur.empty()) return;
        auto rules = parseMacroLines(cur);
        if (!rules.empty()) suites.push_back(std::move(rules));
        cur.clear();
    };
    while (getline(iss,line)){
        if (isBlankLine(line) || isDivider(line)) flush();
        else { cur += line; cur.push_back('\n'); }
    }
    flush();
    return suites;
}

// ========================== Program build (VM) ==========================
static Program compileRulesToProgram(const vector<RuleSrc> &rules)
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
        for (auto &ins : condCode) prog.code.push_back(ins);
        Instr end{ Op::RULE_TEST_END }; prog.code.push_back(end);
        int skillId = nameToSkillId(r.skill);
        if (skillId < 0) throw runtime_error("Unknown skill: "+r.skill);
        Instr sel{ r.forced ? Op::SELECT_FORCED : Op::SELECT }; sel.a=skillId;
        prog.code.push_back(sel);
    }
    prog.code.push_back({ Op::END });
    return prog;
}

// ========================== Fast Flat（预编译 FCNode IR） ==========================
enum class FCCmp : uint8_t { LT, GT, LE, GE, EQ, NE };
enum class FCOp : uint8_t {
    ConstB,
    Not, And, Or,

    // 原子
    HasBuff, THasBuff, SunPower, MoonPower, SelfSkill, SelfNoSkill, SkillNotInCD,

    // 时间比较（整型 tick）
    BuffTimeCmpI, TBuffTimeCmpI,

    // 资源/字段比较
    FieldCmpConstI, FieldCmpConstF,
    FieldCmpFieldI, FieldCmpFieldF,

    // 技能充能
    SkillEnergyCmp, SkillEnergyMaxCmp,

    // last_skill
    LastSkillCmp
};

enum class FCFieldI : uint8_t { Sun, Moon, Rage, Qidian, Energy, NpcLevel, NearbyEnemy, Yaoxing };
enum class FCFieldF : uint8_t { Life, Mana };

struct FCNode {
    FCOp op;
    int  l = -1, r = -1;

    union {
        struct { uint8_t b; } uConst;

        struct { int id; } uHasBuff; // 也用于 THasBuff / SelfSkill / SelfNoSkill / SkillNotInCD

        struct { int id; int rhs; uint8_t cmp; } uBuffTimeCmpI; // target 版本使用 TBuffTimeCmpI

        struct { uint8_t f; uint8_t cmp; int c; }   uFieldConstI;
        struct { uint8_t f; uint8_t cmp; float c; } uFieldConstF;
        struct { uint8_t f; uint8_t cmp; uint8_t g; } uFieldFieldI;
        struct { uint8_t f; uint8_t cmp; uint8_t g; } uFieldFieldF;

        struct { int id; int rhs; uint8_t cmp; } uSkillEnergyCmp;
        struct { int rhs; uint8_t cmp; } uLastSkillCmp;
    } data;
};

struct FlatCond { vector<FCNode> nodes; int root=-1; };
struct FlatRule { FlatCond cond; int skillId=-1; bool forced=false; };

static FCCmp toFCCmp(NodeKind k)
{
    switch (k) {
        case NodeKind::LT: return FCCmp::LT;
        case NodeKind::GT: return FCCmp::GT;
        case NodeKind::LE: return FCCmp::LE;
        case NodeKind::GE: return FCCmp::GE;
        case NodeKind::EQ: return FCCmp::EQ;
        case NodeKind::NE: return FCCmp::NE;
        default: return FCCmp::EQ;
    }
}
static FCCmp flipFCCmp(FCCmp c)
{
    switch (c) {
        case FCCmp::LT: return FCCmp::GT;
        case FCCmp::GT: return FCCmp::LT;
        case FCCmp::LE: return FCCmp::GE;
        case FCCmp::GE: return FCCmp::LE;
        case FCCmp::EQ: return FCCmp::EQ;
        case FCCmp::NE: return FCCmp::NE;
    }
    return c;
}

static optional<FCFieldI> asIntField(const Node* n)
{
    switch (n->kind) {
        case NodeKind::Sun: return FCFieldI::Sun;
        case NodeKind::Moon: return FCFieldI::Moon;
        case NodeKind::Rage: return FCFieldI::Rage;
        case NodeKind::Qidian: return FCFieldI::Qidian;
        case NodeKind::Energy: return FCFieldI::Energy;
        case NodeKind::NpcLevel: return FCFieldI::NpcLevel;
        case NodeKind::NearbyEnemy: return FCFieldI::NearbyEnemy;
        case NodeKind::Yaoxing: return FCFieldI::Yaoxing;
        default: return nullopt;
    }
}
static optional<FCFieldF> asFloatField(const Node* n)
{
    switch (n->kind) {
        case NodeKind::Life: return FCFieldF::Life;
        case NodeKind::Mana: return FCFieldF::Mana;
        default: return nullopt;
    }
}

static int pushFC(vector<FCNode> &v, const FCNode &n){ v.push_back(n); return (int)v.size()-1; }

static int buildFlatRec(const Node* n, vector<FCNode> &out)
{
    switch (n->kind) {
        case NodeKind::ConstB: { FCNode x{}; x.op=FCOp::ConstB; x.data.uConst.b = n->a?1:0; return pushFC(out,x); }
        case NodeKind::Not: { int l=buildFlatRec(n->l.get(),out); FCNode x{}; x.op=FCOp::Not; x.l=l; return pushFC(out,x); }
        case NodeKind::And: { int l=buildFlatRec(n->l.get(),out), r=buildFlatRec(n->r.get(),out); FCNode x{}; x.op=FCOp::And; x.l=l; x.r=r; return pushFC(out,x); }
        case NodeKind::Or:  { int l=buildFlatRec(n->l.get(),out), r=buildFlatRec(n->r.get(),out); FCNode x{}; x.op=FCOp::Or;  x.l=l; x.r=r; return pushFC(out,x); }

        // 原子
        case NodeKind::HasBuff:    { FCNode x{}; x.op=FCOp::HasBuff;    x.data.uHasBuff.id=n->a; return pushFC(out,x); }
        case NodeKind::THasBuff:   { FCNode x{}; x.op=FCOp::THasBuff;   x.data.uHasBuff.id=n->a; return pushFC(out,x); }
        case NodeKind::SunPower:   { FCNode x{}; x.op=FCOp::SunPower;   return pushFC(out,x); }
        case NodeKind::MoonPower:  { FCNode x{}; x.op=FCOp::MoonPower;  return pushFC(out,x); }
        case NodeKind::SelfSkill:  { FCNode x{}; x.op=FCOp::SelfSkill;  x.data.uHasBuff.id=n->a; return pushFC(out,x); }
        case NodeKind::SelfNoSkill:{ FCNode x{}; x.op=FCOp::SelfNoSkill;x.data.uHasBuff.id=n->a; return pushFC(out,x); }
        case NodeKind::SkillNotInCD:{FCNode x{}; x.op=FCOp::SkillNotInCD;x.data.uHasBuff.id=n->a;return pushFC(out,x); }

        // 这些单独出现作为布尔，统一 false（不直接当 bool）
        case NodeKind::BuffTime:
        case NodeKind::TBuffTime:
        case NodeKind::Life:
        case NodeKind::Mana:
        case NodeKind::Sun:
        case NodeKind::Moon:
        case NodeKind::Rage:
        case NodeKind::Qidian:
        case NodeKind::Energy:
        case NodeKind::NpcLevel:
        case NodeKind::NearbyEnemy:
        case NodeKind::Yaoxing:
        case NodeKind::LastSkill:
        case NodeKind::SkillEnergy:
        case NodeKind::SkillEnergyMax:
        case NodeKind::ConstF:
        case NodeKind::ConstI:
        {
            FCNode x{}; x.op=FCOp::ConstB; x.data.uConst.b=0; return pushFC(out,x);
        }

        // 比较归约
        case NodeKind::LT:
        case NodeKind::GT:
        case NodeKind::LE:
        case NodeKind::GE:
        case NodeKind::EQ:
        case NodeKind::NE:
        {
            FCCmp c = toFCCmp(n->kind);

            // bufftime/tbufftime vs constI
            if (n->l->kind==NodeKind::BuffTime && n->r->kind==NodeKind::ConstI) {
                FCNode x{}; x.op=FCOp::BuffTimeCmpI; x.data.uBuffTimeCmpI.id=n->l->a; x.data.uBuffTimeCmpI.rhs=n->r->a; x.data.uBuffTimeCmpI.cmp=(uint8_t)c; return pushFC(out,x);
            }
            if (n->l->kind==NodeKind::ConstI && n->r->kind==NodeKind::BuffTime) {
                FCNode x{}; x.op=FCOp::BuffTimeCmpI; x.data.uBuffTimeCmpI.id=n->r->a; x.data.uBuffTimeCmpI.rhs=n->l->a; x.data.uBuffTimeCmpI.cmp=(uint8_t)flipFCCmp(c); return pushFC(out,x);
            }
            if (n->l->kind==NodeKind::TBuffTime && n->r->kind==NodeKind::ConstI) {
                FCNode x{}; x.op=FCOp::TBuffTimeCmpI; x.data.uBuffTimeCmpI.id=n->l->a; x.data.uBuffTimeCmpI.rhs=n->r->a; x.data.uBuffTimeCmpI.cmp=(uint8_t)c; return pushFC(out,x);
            }
            if (n->l->kind==NodeKind::ConstI && n->r->kind==NodeKind::TBuffTime) {
                FCNode x{}; x.op=FCOp::TBuffTimeCmpI; x.data.uBuffTimeCmpI.id=n->r->a; x.data.uBuffTimeCmpI.rhs=n->l->a; x.data.uBuffTimeCmpI.cmp=(uint8_t)flipFCCmp(c); return pushFC(out,x);
            }

            // 技能充能 vs constI
            if (n->l->kind==NodeKind::SkillEnergy && n->r->kind==NodeKind::ConstI) {
                FCNode x{}; x.op=FCOp::SkillEnergyCmp; x.data.uSkillEnergyCmp.id=n->l->a; x.data.uSkillEnergyCmp.rhs=n->r->a; x.data.uSkillEnergyCmp.cmp=(uint8_t)c; return pushFC(out,x);
            }
            if (n->l->kind==NodeKind::SkillEnergyMax && n->r->kind==NodeKind::ConstI) {
                FCNode x{}; x.op=FCOp::SkillEnergyMaxCmp; x.data.uSkillEnergyCmp.id=n->l->a; x.data.uSkillEnergyCmp.rhs=n->r->a; x.data.uSkillEnergyCmp.cmp=(uint8_t)c; return pushFC(out,x);
            }

            // last_skill vs constI
            if (n->l->kind==NodeKind::LastSkill && n->r->kind==NodeKind::ConstI) {
                FCNode x{}; x.op=FCOp::LastSkillCmp; x.data.uLastSkillCmp.rhs=n->r->a; x.data.uLastSkillCmp.cmp=(uint8_t)c; return pushFC(out,x);
            }
            if (n->l->kind==NodeKind::ConstI && n->r->kind==NodeKind::LastSkill) {
                FCNode x{}; x.op=FCOp::LastSkillCmp; x.data.uLastSkillCmp.rhs=n->l->a; x.data.uLastSkillCmp.cmp=(uint8_t)flipFCCmp(c); return pushFC(out,x);
            }

            // int 字段 vs constI
            if (auto lf = asIntField(n->l.get()); lf && n->r->kind==NodeKind::ConstI) {
                FCNode x{}; x.op=FCOp::FieldCmpConstI; x.data.uFieldConstI.f=(uint8_t)*lf; x.data.uFieldConstI.cmp=(uint8_t)c; x.data.uFieldConstI.c=n->r->a; return pushFC(out,x);
            }
            // constI vs int 字段 -> flip
            if (n->l->kind==NodeKind::ConstI) {
                if (auto rf = asIntField(n->r.get())) {
                    FCNode x{}; x.op=FCOp::FieldCmpConstI; x.data.uFieldConstI.f=(uint8_t)*rf; x.data.uFieldConstI.cmp=(uint8_t)flipFCCmp(c); x.data.uFieldConstI.c=n->l->a; return pushFC(out,x);
                }
            }
            // int 字段 vs int 字段
            if (auto lf = asIntField(n->l.get())) {
                if (auto rf = asIntField(n->r.get())) {
                    FCNode x{}; x.op=FCOp::FieldCmpFieldI; x.data.uFieldFieldI.f=(uint8_t)*lf; x.data.uFieldFieldI.cmp=(uint8_t)c; x.data.uFieldFieldI.g=(uint8_t)*rf; return pushFC(out,x);
                }
            }

            // float 字段 vs constF
            if (auto lf = asFloatField(n->l.get()); lf && n->r->kind==NodeKind::ConstF) {
                FCNode x{}; x.op=FCOp::FieldCmpConstF; x.data.uFieldConstF.f=(uint8_t)*lf; x.data.uFieldConstF.cmp=(uint8_t)c; x.data.uFieldConstF.c=n->r->f; return pushFC(out,x);
            }
            // constF vs float 字段 -> flip
            if (n->l->kind==NodeKind::ConstF) {
                if (auto rf = asFloatField(n->r.get())) {
                    FCNode x{}; x.op=FCOp::FieldCmpConstF; x.data.uFieldConstF.f=(uint8_t)*rf; x.data.uFieldConstF.cmp=(uint8_t)flipFCCmp(c); x.data.uFieldConstF.c=n->l->f; return pushFC(out,x);
                }
            }
            // float 字段 vs float 字段
            if (auto lf2 = asFloatField(n->l.get())) {
                if (auto rf2 = asFloatField(n->r.get())) {
                    FCNode x{}; x.op=FCOp::FieldCmpFieldF; x.data.uFieldFieldF.f=(uint8_t)*lf2; x.data.uFieldFieldF.cmp=(uint8_t)c; x.data.uFieldFieldF.g=(uint8_t)*rf2; return pushFC(out,x);
                }
            }

            // 兜底（不可识别组合）：恒 false
            FCNode x{}; x.op=FCOp::ConstB; x.data.uConst.b=0; return pushFC(out,x);
        }
    }
    // 不应达此分支
    FCNode x{}; x.op=FCOp::ConstB; x.data.uConst.b=0; return pushFC(out,x);
}

static vector<FlatRule> compileRulesToFlatFast(const vector<RuleSrc> &rules)
{
    vector<FlatRule> out; out.reserve(rules.size());
    for (auto &r : rules) {
        FlatCond c;
        if (r.cond.empty()) {
            FCNode x{}; x.op=FCOp::ConstB; x.data.uConst.b=1; c.root = pushFC(c.nodes, x);
        } else {
            Parser ps(r.cond); auto ast=ps.parse();
            c.root = buildFlatRec(ast.get(), c.nodes);
        }
        int skillId = nameToSkillId(r.skill);
        if (skillId < 0) throw runtime_error("Unknown skill: "+r.skill);
        out.push_back({ std::move(c), skillId, r.forced });
    }
    return out;
}

static inline bool evalFCNode(const FlatCond &c, int idx, const Snapshot &s) noexcept
{
    const FCNode &nd = c.nodes[idx];
    switch (nd.op) {
        case FCOp::ConstB: return nd.data.uConst.b != 0;

        case FCOp::Not: return !evalFCNode(c, nd.l, s);

        case FCOp::And:
        {
            if (!evalFCNode(c, nd.l, s)) return false;
            return evalFCNode(c, nd.r, s);
        }
        case FCOp::Or:
        {
            if (evalFCNode(c, nd.l, s)) return true;
            return evalFCNode(c, nd.r, s);
        }

        // 原子
        case FCOp::HasBuff: return s.hasBuffId(nd.data.uHasBuff.id);
        case FCOp::THasBuff: return s.thasBuffId(nd.data.uHasBuff.id);
        case FCOp::SunPower: return s.sun_power;
        case FCOp::MoonPower: return s.moon_power;
        case FCOp::SelfSkill: return s.hasSelfSkill(nd.data.uHasBuff.id);
        case FCOp::SelfNoSkill: return !s.hasSelfSkill(nd.data.uHasBuff.id);
        case FCOp::SkillNotInCD: return s.skillNotInCD(nd.data.uHasBuff.id);

        // 时间比较
        case FCOp::BuffTimeCmpI:
        {
            int a = s.buffTimeId(nd.data.uBuffTimeCmpI.id);
            int b = nd.data.uBuffTimeCmpI.rhs;
            switch ((FCCmp)nd.data.uBuffTimeCmpI.cmp) {
                case FCCmp::LT: return a < b; case FCCmp::GT: return a > b;
                case FCCmp::LE: return a <= b; case FCCmp::GE: return a >= b;
                case FCCmp::EQ: return a == b; case FCCmp::NE: return a != b;
            }
            return false;
        }
        case FCOp::TBuffTimeCmpI:
        {
            int a = s.tbuffTimeId(nd.data.uBuffTimeCmpI.id);
            int b = nd.data.uBuffTimeCmpI.rhs;
            switch ((FCCmp)nd.data.uBuffTimeCmpI.cmp) {
                case FCCmp::LT: return a < b; case FCCmp::GT: return a > b;
                case FCCmp::LE: return a <= b; case FCCmp::GE: return a >= b;
                case FCCmp::EQ: return a == b; case FCCmp::NE: return a != b;
            }
            return false;
        }

        // 字段比较（整型）
        case FCOp::FieldCmpConstI:
        {
            int lhs = 0;
            switch ((FCFieldI)nd.data.uFieldConstI.f) {
                case FCFieldI::Sun: lhs=s.sun; break; case FCFieldI::Moon: lhs=s.moon; break;
                case FCFieldI::Rage: lhs=s.rage; break; case FCFieldI::Qidian: lhs=s.qidian; break;
                case FCFieldI::Energy: lhs=s.energy; break; case FCFieldI::NpcLevel: lhs=s.npclevel; break;
                case FCFieldI::NearbyEnemy: lhs=s.nearby_enemy; break; case FCFieldI::Yaoxing: lhs=s.yaoxing; break;
            }
            int rhs = nd.data.uFieldConstI.c;
            switch ((FCCmp)nd.data.uFieldConstI.cmp) {
                case FCCmp::LT: return lhs < rhs; case FCCmp::GT: return lhs > rhs;
                case FCCmp::LE: return lhs <= rhs; case FCCmp::GE: return lhs >= rhs;
                case FCCmp::EQ: return lhs == rhs; case FCCmp::NE: return lhs != rhs;
            }
            return false;
        }
        case FCOp::FieldCmpFieldI:
        {
            auto getI=[&](uint8_t tag)->int{
                switch ((FCFieldI)tag) {
                    case FCFieldI::Sun: return s.sun; case FCFieldI::Moon: return s.moon;
                    case FCFieldI::Rage: return s.rage; case FCFieldI::Qidian: return s.qidian;
                    case FCFieldI::Energy: return s.energy; case FCFieldI::NpcLevel: return s.npclevel;
                    case FCFieldI::NearbyEnemy: return s.nearby_enemy; case FCFieldI::Yaoxing: return s.yaoxing;
                }
                return 0;
            };
            int L=getI(nd.data.uFieldFieldI.f), R=getI(nd.data.uFieldFieldI.g);
            switch ((FCCmp)nd.data.uFieldFieldI.cmp) {
                case FCCmp::LT: return L < R; case FCCmp::GT: return L > R;
                case FCCmp::LE: return L <= R; case FCCmp::GE: return L >= R;
                case FCCmp::EQ: return L == R; case FCCmp::NE: return L != R;
            }
            return false;
        }

        // 字段比较（浮点）
        case FCOp::FieldCmpConstF:
        {
            float lhs = (nd.data.uFieldConstF.f==(uint8_t)FCFieldF::Life)? s.life : s.mana;
            float rhs = nd.data.uFieldConstF.c;
            switch ((FCCmp)nd.data.uFieldConstF.cmp) {
                case FCCmp::LT: return lhs < rhs; case FCCmp::GT: return lhs > rhs;
                case FCCmp::LE: return lhs <= rhs; case FCCmp::GE: return lhs >= rhs;
                case FCCmp::EQ: return lhs == rhs; case FCCmp::NE: return lhs != rhs;
            }
            return false;
        }
        case FCOp::FieldCmpFieldF:
        {
            float L = (nd.data.uFieldFieldF.f==(uint8_t)FCFieldF::Life)? s.life : s.mana;
            float R = (nd.data.uFieldFieldF.g==(uint8_t)FCFieldF::Life)? s.life : s.mana;
            switch ((FCCmp)nd.data.uFieldFieldF.cmp) {
                case FCCmp::LT: return L < R; case FCCmp::GT: return L > R;
                case FCCmp::LE: return L <= R; case FCCmp::GE: return L >= R;
                case FCCmp::EQ: return L == R; case FCCmp::NE: return L != R;
            }
            return false;
        }

        // 技能充能
        case FCOp::SkillEnergyCmp:
        {
            int v = s.skillEnergy(nd.data.uSkillEnergyCmp.id);
            int rhs = nd.data.uSkillEnergyCmp.rhs;
            switch ((FCCmp)nd.data.uSkillEnergyCmp.cmp) {
                case FCCmp::LT: return v < rhs; case FCCmp::GT: return v > rhs;
                case FCCmp::LE: return v <= rhs; case FCCmp::GE: return v >= rhs;
                case FCCmp::EQ: return v == rhs; case FCCmp::NE: return v != rhs;
            }
            return false;
        }
        case FCOp::SkillEnergyMaxCmp:
        {
            int v = s.skillEnergyMax(nd.data.uSkillEnergyCmp.id);
            int rhs = nd.data.uSkillEnergyCmp.rhs;
            switch ((FCCmp)nd.data.uSkillEnergyCmp.cmp) {
                case FCCmp::LT: return v < rhs; case FCCmp::GT: return v > rhs;
                case FCCmp::LE: return v <= rhs; case FCCmp::GE: return v >= rhs;
                case FCCmp::EQ: return v == rhs; case FCCmp::NE: return v != rhs;
            }
            return false;
        }

        // last_skill
        case FCOp::LastSkillCmp:
        {
            int lhs = s.last_skill, rhs = nd.data.uLastSkillCmp.rhs;
            switch ((FCCmp)nd.data.uLastSkillCmp.cmp) {
                case FCCmp::LT: return lhs < rhs; case FCCmp::GT: return lhs > rhs;
                case FCCmp::LE: return lhs <= rhs; case FCCmp::GE: return lhs >= rhs;
                case FCCmp::EQ: return lhs == rhs; case FCCmp::NE: return lhs != rhs;
            }
            return false;
        }
    }
    return false;
}

static Decision executeFlatFast(const vector<FlatRule> &rules, const Snapshot &s)
{
    for (auto &r : rules) {
        if (evalFCNode(r.cond, r.cond.root, s)) {
            return { r.skillId, r.forced };
        }
    }
    return {};
}

// ========================== Closures（运行时谓词） ==========================
struct BoolPred
{
    using Fn = bool (*)(const void*, const Snapshot&) noexcept;
    const void* ptr=nullptr; Fn call=nullptr;
};
template <typename T>
static inline BoolPred makePred(const T *p){ return BoolPred{ p, [](const void*vp,const Snapshot&s) noexcept { return (*reinterpret_cast<const T*>(vp))(s); } }; }

struct ConstPred { bool v; bool operator()(const Snapshot&) const noexcept { return v; } };
struct HasBuffPred { int id; bool operator()(const Snapshot&s) const noexcept { return s.hasBuffId(id); } };
struct THasBuffPred { int id; bool operator()(const Snapshot&s) const noexcept { return s.thasBuffId(id); } };
struct NotPred { BoolPred p; bool operator()(const Snapshot&s) const noexcept { return !p.call(p.ptr,s); } };
struct AndPred { BoolPred a,b; bool operator()(const Snapshot&s) const noexcept { if(!a.call(a.ptr,s)) return false; return b.call(b.ptr,s); } };
struct OrPred  { BoolPred a,b; bool operator()(const Snapshot&s) const noexcept { if(a.call(a.ptr,s)) return true; return b.call(b.ptr,s); } };

enum class RCmp:uint8_t { LT,GT,LE,GE,EQ,NE };
static inline RCmp toRCmp(NodeKind k){ switch(k){
    case NodeKind::LT: return RCmp::LT; case NodeKind::GT: return RCmp::GT;
    case NodeKind::LE: return RCmp::LE; case NodeKind::GE: return RCmp::GE;
    case NodeKind::EQ: return RCmp::EQ; case NodeKind::NE: return RCmp::NE;
    default: return RCmp::EQ; } }

struct FloatFieldConstPred
{
    int field; float c; RCmp cmp; // field: 0=life 1=mana
    bool operator()(const Snapshot&s) const noexcept {
        float v = (field==0)? s.life : s.mana;
        switch(cmp){ case RCmp::LT: return v<c; case RCmp::GT: return v>c; case RCmp::LE: return v<=c;
            case RCmp::GE: return v>=c; case RCmp::EQ: return v==c; case RCmp::NE: return v!=c; }
        return false;
    }
};
struct FloatFieldFieldPred
{
    int a,b; RCmp cmp; // a,b: 0=life 1=mana
    bool operator()(const Snapshot&s) const noexcept {
        float L=(a==0)?s.life:s.mana, R=(b==0)?s.life:s.mana;
        switch(cmp){ case RCmp::LT: return L<R; case RCmp::GT: return L>R; case RCmp::LE: return L<=R;
            case RCmp::GE: return L>=R; case RCmp::EQ: return L==R; case RCmp::NE: return L!=R; }
        return false;
    }
};
struct IntConstCmpPred
{
    int (*get)(const Snapshot&) noexcept; int c; RCmp cmp;
    bool operator()(const Snapshot&s) const noexcept {
        int v=get(s);
        switch(cmp){ case RCmp::LT: return v<c; case RCmp::GT: return v>c; case RCmp::LE: return v<=c;
            case RCmp::GE: return v>=c; case RCmp::EQ: return v==c; case RCmp::NE: return v!=c; }
        return false;
    }
};
struct IntFieldFieldPred
{
    int (*ga)(const Snapshot&) noexcept; int (*gb)(const Snapshot&) noexcept; RCmp cmp;
    bool operator()(const Snapshot&s) const noexcept {
        int L=ga(s), R=gb(s);
        switch(cmp){ case RCmp::LT: return L<R; case RCmp::GT: return L>R; case RCmp::LE: return L<=R;
            case RCmp::GE: return L>=R; case RCmp::EQ: return L==R; case RCmp::NE: return L!=R; }
        return false;
    }
};

// 具名 getter（函数指针）
static int get_sun(const Snapshot&s) noexcept { return s.sun; }
static int get_moon(const Snapshot&s) noexcept { return s.moon; }
static int get_rage(const Snapshot&s) noexcept { return s.rage; }
static int get_qidian(const Snapshot&s) noexcept { return s.qidian; }
static int get_energy(const Snapshot&s) noexcept { return s.energy; }
static int get_npclevel(const Snapshot&s) noexcept { return s.npclevel; }
static int get_nearby(const Snapshot&s) noexcept { return s.nearby_enemy; }
static int get_yaoxing(const Snapshot&s) noexcept { return s.yaoxing; }
static int get_last_skill(const Snapshot&s) noexcept { return s.last_skill; }

// 需要携带 id 的比较，使用结构体封装
struct BuffTimeCmpPred
{
    int id; int c; RCmp cmp; bool target=false;
    bool operator()(const Snapshot&s) const noexcept {
        int v = target ? s.tbuffTimeId(id) : s.buffTimeId(id);
        switch(cmp){ case RCmp::LT: return v<c; case RCmp::GT: return v>c; case RCmp::LE: return v<=c;
            case RCmp::GE: return v>=c; case RCmp::EQ: return v==c; case RCmp::NE: return v!=c; }
        return false;
    }
};
struct SkillEnergyCmpPred
{
    int id; int c; RCmp cmp; bool isMax=false;
    bool operator()(const Snapshot&s) const noexcept {
        int v = isMax ? s.skillEnergyMax(id) : s.skillEnergy(id);
        switch(cmp){ case RCmp::LT: return v<c; case RCmp::GT: return v>c; case RCmp::LE: return v<=c;
            case RCmp::GE: return v>=c; case RCmp::EQ: return v==c; case RCmp::NE: return v!=c; }
        return false;
    }
};
struct SunPowerPred { bool operator()(const Snapshot&s) const noexcept { return s.sun_power; } };
struct MoonPowerPred { bool operator()(const Snapshot&s) const noexcept { return s.moon_power; } };
struct SelfSkillPred { int id; bool operator()(const Snapshot&s) const noexcept { return s.hasSelfSkill(id); } };
struct SelfNoSkillPred { int id; bool operator()(const Snapshot&s) const noexcept { return !s.hasSelfSkill(id); } };
struct SkillNotInCDPred { int id; bool operator()(const Snapshot&s) const noexcept { return s.skillNotInCD(id); } };

template <typename T, typename... Args>
static inline T* rcNew(vector<unique_ptr<void, void(*)(void*)>> &arena, Args&&... args){
    T* p=new T{ std::forward<Args>(args)... };
    arena.emplace_back(p, +[](void*vp){ delete reinterpret_cast<T*>(vp); });
    return p;
}

static BoolPred buildClosure(const Node *n, vector<unique_ptr<void, void(*)(void*)>> &arena)
{
    switch (n->kind) {
        case NodeKind::ConstB: return makePred(rcNew<ConstPred>(arena, ConstPred{ n->a!=0 }));
        case NodeKind::HasBuff: return makePred(rcNew<HasBuffPred>(arena, HasBuffPred{ n->a }));
        case NodeKind::THasBuff: return makePred(rcNew<THasBuffPred>(arena, THasBuffPred{ n->a }));
        case NodeKind::SunPower: return makePred(rcNew<SunPowerPred>(arena));
        case NodeKind::MoonPower: return makePred(rcNew<MoonPowerPred>(arena));
        case NodeKind::SelfSkill: return makePred(rcNew<SelfSkillPred>(arena, SelfSkillPred{ n->a }));
        case NodeKind::SelfNoSkill: return makePred(rcNew<SelfNoSkillPred>(arena, SelfNoSkillPred{ n->a }));
        case NodeKind::SkillNotInCD: return makePred(rcNew<SkillNotInCDPred>(arena, SkillNotInCDPred{ n->a }));

        case NodeKind::Not:
        {
            auto p=buildClosure(n->l.get(), arena);
            return makePred(rcNew<NotPred>(arena, NotPred{ p }));
        }

        case NodeKind::And:
        {
            auto A=buildClosure(n->l.get(), arena);
            auto B=buildClosure(n->r.get(), arena);
            return makePred(rcNew<AndPred>(arena, AndPred{ A,B }));
        }
        case NodeKind::Or:
        {
            auto A=buildClosure(n->l.get(), arena);
            auto B=buildClosure(n->r.get(), arena);
            return makePred(rcNew<OrPred>(arena, OrPred{ A,B }));
        }

        case NodeKind::LT:
        case NodeKind::GT:
        case NodeKind::LE:
        case NodeKind::GE:
        case NodeKind::EQ:
        case NodeKind::NE:
        {
            RCmp c = toRCmp(n->kind);

            // float 比较：life/mana/ConstF 任一出现
            auto isFloaty=[&](const Node* x)->bool{
                return (x->kind==NodeKind::ConstF || x->kind==NodeKind::Life || x->kind==NodeKind::Mana);
            };
            if (isFloaty(n->l.get()) || isFloaty(n->r.get())) {
                int lf = (n->l->kind==NodeKind::Life)?0 : (n->l->kind==NodeKind::Mana)?1 : -1;
                int rf = (n->r->kind==NodeKind::Life)?0 : (n->r->kind==NodeKind::Mana)?1 : -1;
                if (lf>=0 && rf>=0) return makePred(rcNew<FloatFieldFieldPred>(arena, FloatFieldFieldPred{ lf, rf, c }));
                if (lf>=0 && n->r->kind==NodeKind::ConstF) return makePred(rcNew<FloatFieldConstPred>(arena, FloatFieldConstPred{ lf, n->r->f, c }));
                if (n->l->kind==NodeKind::ConstF && rf>=0) {
                    RCmp cf = (c==RCmp::LT?RCmp::GT:c==RCmp::GT?RCmp::LT:c==RCmp::LE?RCmp::GE:c==RCmp::GE?RCmp::LE:c);
                    return makePred(rcNew<FloatFieldConstPred>(arena, FloatFieldConstPred{ rf, n->l->f, cf }));
                }
                return makePred(rcNew<ConstPred>(arena, ConstPred{ false }));
            }

            // int getter 常量 / getter getter 等
            auto toGetter=[&](const Node* x)->optional<int(*)(const Snapshot&) noexcept>{
                switch (x->kind) {
                    case NodeKind::Sun: return &get_sun;
                    case NodeKind::Moon: return &get_moon;
                    case NodeKind::Rage: return &get_rage;
                    case NodeKind::Qidian: return &get_qidian;
                    case NodeKind::Energy: return &get_energy;
                    case NodeKind::NpcLevel: return &get_npclevel;
                    case NodeKind::NearbyEnemy: return &get_nearby;
                    case NodeKind::Yaoxing: return &get_yaoxing;
                    case NodeKind::LastSkill: return &get_last_skill;
                    default: return nullopt;
                }
            };

            // BuffTime/SkillEnergy 使用专用谓词
            if (n->l->kind==NodeKind::BuffTime && n->r->kind==NodeKind::ConstI) {
                return makePred(rcNew<BuffTimeCmpPred>(arena, BuffTimeCmpPred{ n->l->a, n->r->a, c, false }));
            }
            if (n->l->kind==NodeKind::TBuffTime && n->r->kind==NodeKind::ConstI) {
                return makePred(rcNew<BuffTimeCmpPred>(arena, BuffTimeCmpPred{ n->l->a, n->r->a, c, true }));
            }
            if (n->l->kind==NodeKind::SkillEnergy && n->r->kind==NodeKind::ConstI) {
                return makePred(rcNew<SkillEnergyCmpPred>(arena, SkillEnergyCmpPred{ n->l->a, n->r->a, c, false }));
            }
            if (n->l->kind==NodeKind::SkillEnergyMax && n->r->kind==NodeKind::ConstI) {
                return makePred(rcNew<SkillEnergyCmpPred>(arena, SkillEnergyCmpPred{ n->l->a, n->r->a, c, true }));
            }

            // 常见：getter vs constI
            if (auto lg=toGetter(n->l.get()); lg && n->r->kind==NodeKind::ConstI) {
                return makePred(rcNew<IntConstCmpPred>(arena, IntConstCmpPred{ *lg, n->r->a, c }));
            }
            // constI vs getter -> flip
            if (n->l->kind==NodeKind::ConstI) {
                if (auto rg=toGetter(n->r.get())) {
                    RCmp cf = (c==RCmp::LT?RCmp::GT:c==RCmp::GT?RCmp::LT:c==RCmp::LE?RCmp::GE:c==RCmp::GE?RCmp::LE:c);
                    return makePred(rcNew<IntConstCmpPred>(arena, IntConstCmpPred{ *rg, n->l->a, cf }));
                }
            }
            // getter vs getter
            if (auto lg=toGetter(n->l.get())) {
                if (auto rg=toGetter(n->r.get())) {
                    return makePred(rcNew<IntFieldFieldPred>(arena, IntFieldFieldPred{ *lg, *rg, c }));
                }
            }

            // last_skill 与常量的比较
            if (n->l->kind==NodeKind::LastSkill && n->r->kind==NodeKind::ConstI) {
                return makePred(rcNew<IntConstCmpPred>(arena, IntConstCmpPred{ &get_last_skill, n->r->a, c }));
            }
            if (n->l->kind==NodeKind::ConstI && n->r->kind==NodeKind::LastSkill) {
                RCmp cf = (c==RCmp::LT?RCmp::GT:c==RCmp::GT?RCmp::LT:c==RCmp::LE?RCmp::GE:c==RCmp::GE?RCmp::LE:c);
                return makePred(rcNew<IntConstCmpPred>(arena, IntConstCmpPred{ &get_last_skill, n->l->a, cf }));
            }

            // 兜底
            return makePred(rcNew<ConstPred>(arena, ConstPred{ false }));
        }

        default:
            return makePred(rcNew<ConstPred>(arena, ConstPred{ false }));
    }
}

struct RCRule
{
    vector<unique_ptr<void, void(*)(void*)>> arena;
    BoolPred pred;
    int      skillId=-1;
    bool     forced=false;
};

static vector<RCRule> compileRulesToClosures(const vector<RuleSrc> &rules)
{
    vector<RCRule> out; out.reserve(rules.size());
    for (auto &r : rules) {
        RCRule rr;
        if (r.cond.empty()) {
            rr.pred = makePred(rcNew<ConstPred>(rr.arena, ConstPred{ true }));
        } else {
            Parser ps(r.cond); auto ast=ps.parse();
            rr.pred = buildClosure(ast.get(), rr.arena);
        }
        rr.skillId = nameToSkillId(r.skill);
        if (rr.skillId < 0) throw runtime_error("Unknown skill: "+r.skill);
        rr.forced = r.forced;
        out.push_back(std::move(rr));
    }
    return out;
}
static Decision executeClosures(const vector<RCRule> &rc, const Snapshot &s) noexcept
{
    for (auto &r : rc) if (r.pred.call(r.pred.ptr, s)) return { r.skillId, r.forced };
    return {};
}

// ========================== Bench & util ==========================
struct BenchResult { double total_ms=0.0; double ns_per_decision=0.0; size_t decisions=0; };
static inline void consumeDecision(const Decision &d) noexcept
{ g_sink += (uint64_t)((uint32_t)(d.skillID < 0 ? 0 : d.skillID) ^ (uint32_t)(d.forced ? 0x9E'37'79'B9u : 0)); }

static BenchResult benchVM(const Program &prog, const vector<Snapshot> &inputs, int rounds)
{
    auto t0=chrono::high_resolution_clock::now(); size_t cnt=0;
    for (int r=0;r<rounds;++r) for (const auto &s: inputs){ auto d=VM::execute(prog,s); consumeDecision(d); cnt++; }
    auto t1=chrono::high_resolution_clock::now(); double ms=chrono::duration<double,milli>(t1-t0).count();
    double ns=(ms*1e6)/cnt; return { ms, ns, cnt };
}
static BenchResult benchFlatFast(const vector<FlatRule> &rules, const vector<Snapshot> &inputs, int rounds)
{
    auto t0=chrono::high_resolution_clock::now(); size_t cnt=0;
    for (int r=0;r<rounds;++r) for (const auto &s: inputs){ auto d=executeFlatFast(rules,s); consumeDecision(d); cnt++; }
    auto t1=chrono::high_resolution_clock::now(); double ms=chrono::duration<double,milli>(t1-t0).count();
    double ns=(ms*1e6)/cnt; return { ms, ns, cnt };
}
static BenchResult benchClosures(const vector<RCRule> &rc, const vector<Snapshot> &inputs, int rounds)
{
    auto t0=chrono::high_resolution_clock::now(); size_t cnt=0;
    for (int r=0;r<rounds;++r) for (const auto &s: inputs){ auto d=executeClosures(rc,s); consumeDecision(d); cnt++; }
    auto t1=chrono::high_resolution_clock::now(); double ms=chrono::duration<double,milli>(t1-t0).count();
    double ns=(ms*1e6)/cnt; return { ms, ns, cnt };
}

static vector<Snapshot> makeRandomInputs(size_t N, uint32_t seed=42)
{
    vector<Snapshot> v; v.reserve(N);
    std::mt19937 rng(seed);
    std::uniform_real_distribution<float> p01(0.0f, 1.0f);
    std::uniform_int_distribution<int>    resD(0, 120);
    std::bernoulli_distribution           hasBuffD(0.4);
    std::uniform_int_distribution<int>    tickD(0, 300);
    std::uniform_int_distribution<int>    lvlD(1, 6);
    std::uniform_int_distribution<int>    enemyD(0, 8);
    std::uniform_int_distribution<int>    yaoxD(-5, 5);

    for (size_t i=0;i<N;++i){
        Snapshot s;
        s.life = p01(rng);
        s.mana = p01(rng);

        s.sun    = resD(rng);
        s.moon   = resD(rng);
        s.rage   = resD(rng);
        s.qidian = resD(rng);
        s.energy = resD(rng);

        s.sun_power  = (s.sun >= 100);
        s.moon_power = (s.moon >= 100);

        s.hasBuff.resize(400, 0);
        s.buffTime.resize(400, -1);
        s.tHasBuff.resize(400, 0);
        s.tBuffTime.resize(400, -1);

        // 示例 Buff
        s.hasBuff[kBuff["神门"]] = hasBuffD(rng) ? 1 : 0;
        s.tHasBuff[kBuff["流血"]] = hasBuffD(rng) ? 1 : 0;
        if (std::bernoulli_distribution(0.5)(rng)) s.buffTime[kBuff["太极"]] = tickD(rng);
        if (std::bernoulli_distribution(0.5)(rng)) s.tBuffTime[kBuff["流血"]] = tickD(rng);

        // 技能充能与 CD
        int sid = kSkill["盾飞"];
        s.skill_energy[sid] = std::uniform_int_distribution<int>(0,3)(rng);
        s.skill_energy_max[sid] = 3;
        s.skill_notin_cd[sid] = std::bernoulli_distribution(0.7)(rng);

        // 自身“技能/奇穴”
        s.self_skill[nameToSkillId("西楚悲歌")] = std::bernoulli_distribution(0.5)(rng);

        // last_skill（整型）
        s.last_skill = std::bernoulli_distribution(0.5)(rng) ? nameToSkillId("钟灵毓秀") : nameToSkillId("赤日轮");

        s.npclevel = lvlD(rng);
        s.nearby_enemy = enemyD(rng);
        s.yaoxing = yaoxD(rng);

        v.push_back(std::move(s));
    }
    return v;
}

// ========================== Consistency then Benchmark ==========================
static bool runConsistencyThenBench(const vector<RuleSrc> &rules, const string &suiteName)
{
    cout<<"=== Suite: "<<suiteName<<" ===\n";
    auto prog = compileRulesToProgram(rules);
    auto rc   = compileRulesToClosures(rules);
    auto flat = compileRulesToFlatFast(rules); // 快速 Flat 预编译

    const size_t inputN=5000; const uint32_t seed=777;
    auto inputs=makeRandomInputs(inputN, seed);

    size_t mismatch=0; Decision dEx{}; bool sampled=false;
    for (size_t i=0;i<inputs.size();++i){
        auto d1 = VM::execute(prog, inputs[i]);
        auto d2 = executeFlatFast(flat, inputs[i]);
        auto d3 = executeClosures(rc, inputs[i]);
        if (!sampled){ dEx=d1; sampled=true; }
        if (!(d1==d2 && d2==d3)) {
            cerr<<"Mismatch at idx="<<i<<" VM("<<d1.skillID<<","<<d1.forced<<")"
                <<" Flat("<<d2.skillID<<","<<d2.forced<<")"
                <<" Closures("<<d3.skillID<<","<<d3.forced<<")\n";
            cerr<<" Snapshot: life="<<inputs[i].life<<" mana="<<inputs[i].mana
                <<" sun="<<inputs[i].sun<<" moon="<<inputs[i].moon
                <<" rage="<<inputs[i].rage<<" qidian="<<inputs[i].qidian<<" energy="<<inputs[i].energy
                <<" npclevel="<<inputs[i].npclevel<<" nearby="<<inputs[i].nearby_enemy
                <<" yaoxing="<<inputs[i].yaoxing<<" last_skill="<<inputs[i].last_skill<<"\n";
            mismatch++; break;
        }
        consumeDecision(d1); consumeDecision(d2); consumeDecision(d3);
    }

    if (mismatch==0){
        cout<<"Consistency OK. Example decision: skill="<<dEx.skillID<<", forced="<<(dEx.forced?"true":"false")<<"\n";
        cout<<"Benchmarking...\n";
        const size_t benchN=20000; const int rounds=40;
        auto inputs2=makeRandomInputs(benchN, 42);
        auto rVM  = benchVM(prog, inputs2, rounds);
        auto rF   = benchFlatFast(flat, inputs2, rounds);
        auto rRC  = benchClosures(rc, inputs2, rounds);
        cout<<fixed<<setprecision(3);
        cout<<"VM:        total "<<rVM.total_ms<<" ms, ns/decision "<<rVM.ns_per_decision<<", decisions "<<rVM.decisions<<"\n";
        cout<<"FlatFast:  total "<<rF.total_ms <<" ms, ns/decision "<<rF.ns_per_decision <<", decisions "<<rF.decisions <<"\n";
        cout<<"Closures:  total "<<rRC.total_ms<<" ms, ns/decision "<<rRC.ns_per_decision<<", decisions "<<rRC.decisions<<"\n";
        cout<<"g_sink="<<(unsigned long long)g_sink<<"\n";
        cout<<"=== Done: "<<suiteName<<" ===\n\n";
        return true;
    } else {
        cerr<<"Consistency FAILED for suite: "<<suiteName<<". Abort benchmark.\n";
        return false;
    }
}

// ========================== Main ==========================
int main(int argc, char **argv)
{
#if defined(_WIN32)
    SetConsoleOutputCP(CP_UTF8);
    std::ios::sync_with_stdio(false);
#endif
    try {
        string macroPath = (argc>=2)? string(argv[1]) : string("macro.txt");
        cout<<"Reading macro from: "<<macroPath<<"\n";
        string allText = readFileAll(macroPath);

        auto suites = parseMacroSuites(allText);
        if (suites.empty()) {
            cerr<<"No suites parsed. Ensure macro.txt has /cast or /fcast lines, suites separated by blank lines.\n";
            return 1;
        }
        cout<<"Suites parsed: "<<suites.size()<<"\n";

        for (size_t i=0;i<suites.size();++i){
            string name="Suite#"+to_string(i+1);
            if (!runConsistencyThenBench(suites[i], name)) return 2;
        }

        cout<<"All suites passed consistency and finished benchmarking.\n";
        cout<<"g_sink="<<(unsigned long long)g_sink<<"\n";
        return 0;
    } catch (const exception &e) {
        cerr<<"Error: "<<e.what()<<"\n";
        return 1;
    }
}