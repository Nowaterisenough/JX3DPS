/**
 * @Description : 公共方法
 * @Author      : NoWats
 * @Date        : 2022-01-28 23:13:33
 * @Update      : NoWats
 * @LastTime    : 2022-02-01 18:21:33
 * @FilePath    : \JX3DPS\modules\Common\Common.h
 */

#include <random>

namespace JX3DPS {

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
