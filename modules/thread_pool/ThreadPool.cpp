#include "ThreadPool.hpp"

ThreadPool *ThreadPool::GetInstance(unsigned short size)
{
    static ThreadPool instance(size);
    return &instance;
}