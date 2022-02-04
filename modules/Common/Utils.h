/**
 * @Description : 公共方法
 * @Author      : NoWats
 * @Date        : 2022-01-28 23:13:33
 * @Update      : NoWats
 * @LastTime    : 2022-02-01 19:42:57
 * @FilePath    : \JX3DPS\modules\Common\Common.h
 */

#include <random>

namespace JX3DPS {

/* 取最大最小整数 */
#define GET_MAX_INT(x, y) ((1 & (~(((x) - (y)) >> 31))) * (x) - (~(((y) - (x)) >> 31)) * (y))
#define GET_MIN_INT(x, y) ((1 & (~(((y) - (x)) >> 31))) * (x) - (~(((x) - (y)) >> 31)) * (y))

/* skill调息结合充能判定 - 1st为正整数时, 返回0, 否则返回2nd */
#define IF_1ST_POS_TO_0_ELSE_2ND(x, y) ((((x)-1) >> 31) & (y))

/* 1st为0时, 返回0, 否则返回2nd */
#define IF_1ST_0_TO_0_ELSE_2ND(x, y) (~(((x)-1) >> 31) & (y))

/* 1st为0时, 返回2nd, 否则返回3rd */
#define IF_1ST_0_TO_2ND_ELSE_3RD(a, b, c) \
    ((1 & (~(-((a)-1) >> 31))) * (c) - (~(((a)-1) >> 31)) * (b))

#if _MSVC_LANG >= 201703L || __cplusplus >= 201703L
template <typename T>
T Random(T a, T b)
{
    static std::default_random_engine e{std::random_device{}()};
    if constexpr (std::is_integral<T>::value) {
        static std::uniform_int_distribution<T> u;
        return u(e, decltype(u)::param_type(a, b));
    } else {
        static std::uniform_real_distribution<T> u;
        return u(e, decltype(u)::param_type(a, b));
    }
}

#elif _MSVC_LANG >= 201402L || __cplusplus >= 201402L
template <typename T>
std::enable_if_t<std::is_integral<T>::value, T> Random(T a, T b)
{
    static std::default_random_engine       e{std::random_device{}()};
    static std::uniform_int_distribution<T> u;
    return u(e, decltype(u)::param_type(a, b));
}

template <typename T>
std::enable_if_t<std::is_floating_point<T>::value, T> Random(T a, T b)
{
    static std::default_random_engine        e{std::random_device{}()};
    static std::uniform_real_distribution<T> u;
    return u(e, decltype(u)::param_type(a, b));
}

#else // _MSVC_LANG >= 201103L || __cplusplus >= 201103L
template <typename T>
typename std::enable_if<std::is_integral<T>::value, T>::type Random(T a, T b)
{
    static std::default_random_engine       e{std::random_device{}()};
    static std::uniform_int_distribution<T> u;
    return u(e, decltype(u)::param_type(a, b));
}

template <typename T>
typename std::enable_if<std::is_floating_point<T>::value, T>::type Random(T a, T b)
{
    static std::default_random_engine        e{std::random_device{}()};
    static std::uniform_real_distribution<T> u;
    return u(e, decltype(u)::param_type(a, b));
}

#endif // C++ 标准

} // namespace JX3DPS
