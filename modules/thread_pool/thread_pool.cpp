#include "thread_pool.h"

ThreadPool *ThreadPool::GetInstance(unsigned short size)
{
    static ThreadPool instance(size);
    return &instance;
}