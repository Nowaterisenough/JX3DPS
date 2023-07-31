/**
 * Project: 
 * File: ThreadPool.cpp
 * Description: 
 * Created Date: 2023-07-19 05:52:32
 * Author: 难为水
 * -----
 * Last Modified: 2023-07-19 05:58:37
 * Modified By: 难为水
 * -----
 * CHANGELOG:
 * Date      	By     	Comments
 * ----------	-------	----------------------------------------------------------
 */

#include "ThreadPool.h"

ThreadPool *ThreadPool::Instance(unsigned short size)
{
    static ThreadPool instance(size);
    return &instance;
}