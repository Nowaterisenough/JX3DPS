/**
 * Project: 
 * File: Utils.hpp
 * Description: 
 * Created Date: 2023-07-22 13:08:19
 * Author: 难为水
 * -----
 * Last Modified: 2023-07-26 17:49:24
 * Modified By: 难为水
 * -----
 * CHANGELOG:
 * Date      	By     	Comments
 * ----------	-------	----------------------------------------------------------
 */

#ifndef __JX3DPS_UTILS_HPP__
#define __JX3DPS_UTILS_HPP__

#include <random>

namespace JX3DPS {

// 均匀分布
template <typename T>
T RandomUniform(T a, T b)
{
    static std::mt19937 e{ std::random_device{}() };
    if constexpr (std::is_integral<T>::value) {
        static std::uniform_int_distribution<T> u;
        return u(e, typename decltype(u)::param_type(a, b));
    } else {
        static std::uniform_real_distribution<T> u;
        return u(e, typename decltype(u)::param_type(a, b));
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

}  // namespace JX3DPS

#endif  // __JX3DPS_UTILS_HPP__
