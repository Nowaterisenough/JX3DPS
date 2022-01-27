/* 测试clang-format格式化效果
 */

class Test
{
    // 对齐这个public修饰符 AccessModifierOffset: -2
public:
};

// 括号断行后参数对齐方式 AlignAfterOpenBracket
void ttttttt(int aaaaaaa,
             int bbbbbbbbb,
             int ccccccccc,
             int ddddddddd,
             int eeeeeeeeeeeeeee,
             int ffffffffffffffffffffffffffffffff)
{
}
// TODO: 不能实现下面的对齐方式
// 1. 下一行长度大于上一行
// 2. 换行缩进为4格
//
void ttttttt(int aaaaaaa,
             int bbbbbbbbb,
             int ccccccccc,
             int ddddddddd,
             int eeeeeeeeeeeeeee,
             int ffffffffffffffffffffffffffffffff)
{
}

// 赋值时等号对齐 AlignConsecutiveAssignments
int aaaaaaa = 222222222;
int b       = 7;
int ccccc   = 99999;

// 变量名左对齐，应该和上面的冲突 AlignConsecutiveDeclarations
int aaaa        = 12;
float b         = 23;
std::string ccc = 23;

// 宏对齐 AlignConsecutiveMacros
#define SHORT_NAME       42
#define LONGER_NAME      0x007f
#define EVEN_LONGER_NAME (2)
#define foo(x)           (x * x)
#define bar(y, z)        (y + z)

// 断行符的对齐 AlignEscapedNewlines
#define A     \
    int aaaa; \
    int b;    \
    int dddddddddd

// 运算变量对齐 AlignOperands
int sum = aaaaaaaaaaa + bbbbbbbbbb + ccccccccccccccccccc + ddddddddddddddd +
          eeeeeeeeeee + fff;

// 是否对齐行尾注释 AlignTrailingComments
int x;             // test xxxxxxxx
int yyyyyyyyyyyyy; // test yyyyyyyyy
int zzzz;          // test zzzzzzz

// 调用函数时，参数的断行方式 AllowAllArgumentsOnNextLine
looooooooooooooooooooooooooooooooooooooooooooooong_call(aaaaaaaaaa,
                                                        bbbbbbbb,
                                                        cccccccc);

// 构造函数初始化列表断行方式 AllowAllConstructorInitializersOnNextLine
class LongInitializers
{
public:
    LongInitializers()
        : aaaaaaaaaaaaaaaaa(1), bbbbbbbbbbbbbbbbbb(2), ccccccccccccc(3)
    {
    }
};

// 函数声明时参数的断行方式 AllowAllParametersOfDeclarationOnNextLine
// 上面的AlignAfterOpenBracket优先级更高，会影响这个
int ddddddddddddddddddddddddddddddddddddddddd(int aaaaaa,
                                              int bbbbbbb,
                                              int cccccccccccc);

// 是否把简短的代码合并成一行 AllowShortBlocksOnASingleLine
// 这个没生效，而且和文档也对不上了。文档有好几个值，配置只接受 true flase
if (a > b) a++;
while (true) {
    a++;
    b++;
};

// switch的case是否合并成一行 AllowShortCaseLabelsOnASingleLine
switch (type) {
case 1: a++; break;
}

// 简短的函数能不能放一行 AllowShortFunctionsOnASingleLine
class TestFuncOneLine
{
    void test() { a++; }
};

void not_one_line()
{
    a++;
}

// if 语句能不能放一行 AllowShortIfStatementsOnASingleLine
if (a > b) {
    a++;
    if (0 == b) b++;
} else {
    b++;
    if (0 == a) {
        a = 0;
        b = 0;
    }
}

// lambda表达式能不能放一行 AllowShortLambdasOnASingleLine
void lambda_one_line()
{
    auto lambda = [](int a, int b) {
        return a > b;
    };

    // TODO: 这里人换行
    sort(
        a.begin(), a.end(), ()[] { return x < y; });
}

// for等循环能不能放一行 AllowShortLoopsOnASingleLine
// 这个测试没生效
while (true) {
    a++;
};

do {
    a++;
} while (0);
do {
    a++
} while (0);

// TODO:这个宏总被展开
#define TEST(a) \
    do {        \
        a++     \
    } while (0)

// 函数声明 返回类型之后要不要断行 AlwaysBreakAfterDefinitionReturnType
int test() {} // 不换行
int test() {} // 换行

// 函数实现 返回类型之后要不要断行 AlwaysBreakAfterReturnType
int test()
{
    a++;
}

// 字符串换行时，等号后面要不要换行 AlwaysBreakBeforeMultilineStrings
aaaa = "bbbb" // 换行
       "cccc";

aaaa = "bbbb" // 不换行
       "cccc";

// 模板声明是否换行 AlwaysBreakTemplateDeclarations
template <typename T> T foo() {}
template <typename T>
T foo(int aaaaaaaaaaaaaaaaaaaaa, int bbbbbbbbbbbbbbbbbbbbb)
{
}

// 调用函数时，参数是否独占一行 BinPackArguments
func(a, b, c);

funcccccccccccccccccccccccccc(aaaaaaaaaaaaaaaaaaaaaaaaaaaa,
                              bbbbbbbbbbbbbbbbbb,
                              ccccccccccccccccc,
                              cccccccccccccccccccc);

// 声明或者实现函数时，参数是否独占一行 BinPackParameters
int funcccccccccccccccccccccccccc(aaaaaaaaaaaaaaaaaaaaaaaaaaaa,
                                  bbbbbbbbbbbbbbbbbb,
                                  ccccccccccccccccc,
                                  cccccccccccccccccccc);

// 控制大括号的断行方式 BraceWrapping
// BreakBeforeBraces的值为Custom才有效
class foo
{
};

// 运算符(=、+、、*等)换行方式 BreakBeforeBinaryOperators
LooooooooooongType loooooooooooooooooooooongVariable =
    someLooooooooooooooooongFunction();

bool value = aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa +
                     aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa ==
                 aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa &&
             aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa >
                 ccccccccccccccccccccccccccccccccccccccccc;

// 双目运算符的断行方式 BreakBeforeTernaryOperators
int a = a > b ? a : b;
veryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryLongDescription
    ? firstValue
    : SecondValueVeryVeryVeryVeryLong;

// 初始化列表换行方式 BreakConstructorInitializers
// 继承的换行方式 BreakInheritanceList
class Tttttttttttttttttttttttttttttttttttttttttttttttttttttest : Aaaaaa, Bbbbbbbbb
{
public:
    Tttttttttttttttttttttttttttttttttttttttttttttttttttttest()
        : Aaaaaa(), Bbbbbbbbb()
    {
    }
};

// 字符串是否允许换行
std::string str = "looooooooooooooooooooooooooooooooooooooooooooooooooooooooooo"
                  "ooooooooooooong str";

// 单行字符数 ColumnLimit

// 控制注释中哪些内容不允许换行的正则 CommentPragmas

// 是否合并命名空间到一行 CompactNamespaces
namespace Foo
{
namespace Bar
{ // 合并
}
} // namespace Foo
// 不合并
namespace Foo
{
namespace Bar
{
}
} // namespace Foo

// 初始化列表是否全放到一行 ConstructorInitializerAllOnOneLineOrOnePerLine
SomeClass::Constructor()
    : aaaaaaaa(aaaaaaaa), aaaaaaaa(aaaaaaaa), aaaaaaaa(aaaaaaaaaaaaaaaaaaaaaaaaa)
{
    return 0;
}

// 初始化列表换行时，缩进的宽度 ConstructorInitializerIndentWidth
// 发生换行时，缩进的宽度 ContinuationIndentWidth

// 大括号数组是否有空格 Cpp11BracedListStyle
vector<int> x{1, 2, 3, 4}; // cpp11风格，没有
vector<int> x{1, 2, 3, 4}; // 有

// 引用和指针的对齐方式 DerivePointerAlignment
// 不知道这个是用来干啥的

// 完全不格式化(为啥会有这个选项，不格式化不调用不就行了么) DisableFormat

// 自动根据当前文件其他地方的格式来格式化函数参数 ExperimentalAutoDetectBinPacking
// 比如说其他地方的参数是每个参数占一行，那它决定按每个参数占一行来格式化
// 这个是实验性的

// 是否自动添加命名空间结束注释 FixNamespaceComments
namespace a
{
foo();
} // namespace a

// 一些第三方的foreach循环宏定义，比如QT的 ForEachMacros

// include的合并方式 IncludeBlocks
// 一般按名字来排，按空行分组(因为有些顺序是特定的)，注意下面有个 SortIncludes 选项
#include "b.h"

#include <lib/main.h>
#include "a.h"
#include <cstd>
#include <yy>

// include 优先级 IncludeCategories
// 当上面的IncludeBlocks设置为Regroup，会把include全部排序，排序规则就按这个来

// include规则 ，和上面的差不多 IncludeIsMainRegex

// switch的case缩进 IndentCaseLabels
switch (a) {
case 1: break; // 不缩进
case 2: break; // 缩进
}

// togo标签是否缩进，文档里有，程序不认这个选项了 IndentGotoLabels

// 多层宏定义镶嵌时，缩进方式 IndentPPDirectives
#if FOO
    #if BAR
        #include <foo>
    #endif
#endif

// 缩进宽度 IndentWidth

// 当返回类型和函数名断行时，函数名是否缩进 IndentWrappedFunctionNames
// 真有这么长的类型和函数名吗？
LoooooooooooooooooooooooooooooooooooooooongReturnType
LoooooooooooooooooooooooooooooooongFunctionDeclaration();

// 代码块开始时，要不要空一行 KeepEmptyLinesAtTheStartOfBlocks
// 这个没生效，可能是需要大括号在行尾的Java风格才有效
if (true) {

    test(); // 上面空一行
}

// 指定格式化提哪个语言 Language

// 匹配一对开始和结束的宏 MacroBlockBegin MacroBlockEnd
NS_MAP_BEGIN
foo();
NS_MAP_END

// 允许连续空多少行 MaxEmptyLinesToKeep
if (true) {
    a++;

    b++;
}

// 命名空间里的代码是否缩进 NamespaceIndentation
namespace out
{
int i; // 不缩进
namespace in
{
int i; // 缩进
}
} // namespace out

// 表示命名空间的宏(我用不着，暂时不测试) NamespaceMacros

// 这几个Penalty暂时不知道是啥，下面的链接有讨论
// https://stackoverflow.com/questions/26635370/in-clang-format-what-do-the-penalties-do
// 大概意思是当刚好超过行宽，但又不超几个字符时，如果断行
// 我试了下PenaltyBreakBeforeFirstCallParameter = 19， PenaltyExcessCharacter = 10
// 时，下面那个函数调用就不换行。但这个数值是根据算法来的，算法把各种因素给定一些值，得
// 出不同换行时的penalty值，其大小没有标准可参考

// PenaltyBreakAssignment
auto loooooooooooooooooooooooooooooooooooooooooooooooooooooooooosong_var =
    "abc";
// PenaltyBreakBeforeFirstCallParameter
Namespaces::Are::Pervasive::SomeReallyVerySuperDuperLooooooooongFunctionName(args);
// PenaltyBreakComment
// PenaltyBreakFirstLessLess
// PenaltyBreakString
// PenaltyBreakTemplateDeclaration
// PenaltyExcessCharacter
// PenaltyReturnTypeOnItsOwnLine

// 指针的*号放哪 PointerAlignment
int *a; // left
int *a; // right

// 匹配字符串中代码的正则，如果匹配到，字符串的代码将会被格式化 RawStringFormats

// 是否格式化注释(这里测试没生效) ReflowComments
/* second veryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryLongComment with plenty of information */

// 是否对include 排序 SortIncludes
// 见上面的IncludeBlocks测试

// using xxx是否需要排序 SortUsingDeclarations
using std::cout;
using std::cin;

// 哪些操作后面需要加空格
// SpaceAfterCStyleCast: (int) i
// SpaceAfterLogicalNot: ! someExpression()
// SpaceAfterTemplateKeyword: template <int> void foo();
// SpaceBeforeAssignmentOperators: int a = 5
// SpaceBeforeCpp11BracedList: vector<int> { 1, 2, 3 }
// SpaceBeforeCtorInitializerColon: Foo::Foo() : a(a) {}
// SpaceBeforeInheritanceColon: class Foo : Bar {}
// SpaceBeforeParens: if (true) {}
// SpaceBeforeRangeBasedForLoopColon: for (auto v : values) {}
// SpaceInEmptyParentheses: f( ) // 括号中间有一个空格
// SpacesBeforeTrailingComments: // aaa //后面有一个空格再接注释
// SpacesInAngles: static_cast< int >(arg) <>中间是否有空格
// SpacesInContainerLiterals: f({a : 1, b : 2, c : 3}); 这个C++没用到
// SpacesInCStyleCastParentheses: x = ( int32 )y 类型转换时，括号要不要加空格
// SpacesInParentheses: t f( Deleted & ) & = delete; 括号要不要加空格，
// SpacesInSquareBrackets: int a[ 5 ]; 数组的中括号要不要加空格

// 使用哪个标准 Standard

// tab宽度 TabWidth

// 是否用tab缩进 UseTab

// TODO: 不能强制if换行时加大括号
if (true)
    a + = bbbbbbbbbbbbbbbbbbbbb + ccccccccccccccccccccccc +
          ddddddddddddddddddddddd;