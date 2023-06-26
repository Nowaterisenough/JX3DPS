/**
 * @Description : 
 * @Author      : NoWats
 * @Date        : 2023-04-08 02:23:52
 * @Update      : NoWats
 * @LastTime    : 2023-04-08 06:27:05
 * @FilePath    : \LegionTDSim\modules\ThreadPool\ThreadPool.cpp
 */


#include "ThreadPool.h"

ThreadPool *ThreadPool::Instance(unsigned short size)
{
    static ThreadPool instance(size);
    return &instance;
}