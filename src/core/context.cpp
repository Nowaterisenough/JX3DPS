/**
 * @file context.cpp
 * @brief SimContext的线程局部存储定义
 */

#include "context.h"

namespace JX3DPS {

// 定义线程局部上下文
thread_local SimContext tls_sim;

} // namespace JX3DPS
