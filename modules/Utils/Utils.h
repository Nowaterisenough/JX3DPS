/**
 * @Description :
 * @Author      : NoWats
 * @Date        : 2023-04-08 02:23:52
 * @Update      : NoWats
 * @LastTime    : 2023-04-08 06:27:45
 * @FilePath    : \LegionTDSim\modules\Utils\Utils.h
 */

#include <random>
#include <string>

template <typename T>
std::string Dec2HexStr(T w, size_t hexLen = sizeof(T) << 1)
{
    static const char *digits = "0123456789ABCDEF";
    std::string        rc(hexLen, '0');
    for (size_t i = 0, j = (hexLen - 1) * 4; i < hexLen; ++i, j -= 4) {
        rc[i] = digits[(w >> j) & 0x0f];
    }
    return rc;
}

// 均匀分布
template <typename T>
T RandomUniform(T a, T b)
{
    static std::mt19937 e{ std::random_device{}() };
    if constexpr (std::is_integral<T>::value) {
        static std::uniform_int_distribution<T> u;
        return u(e, decltype(u)::param_type(a, b));
    } else {
        static std::uniform_real_distribution<T> u;
        return u(e, decltype(u)::param_type(a, b));
    }
}

// 正态分布
template <typename T>
T RandomNormal(T a, T b)
{
    static std::mt19937 e{ std::random_device{}() };

    static std::normal_distribution<double> u;
    double result = u(e, decltype(u)::param_type((a + b) / 2.0, (b - a) / 6.0));
    if constexpr (std::is_integral<T>::value) {
        return static_cast<T>(result);
    } else {
        return result;
    }
}
