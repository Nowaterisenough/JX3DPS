/**
 * @Description :
 * @Author      : NoWats
 * @Date        : 2023-04-08 02:23:52
 * @Update      : NoWats
 * @LastTime    : 2023-05-28 03:48:17
 * @FilePath    : \JX3DPS\tests\TestThreadPool.cpp
 */

#include <iostream>

#include <gtest/gtest.h>

#include <ThreadPool/ThreadPool.h>

int TestFunction1(unsigned int time)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(time));
    std::cout << "thread id: " << std::this_thread::get_id() << " " << time << std::endl;
    return time;
}

struct TestFunctors
{
    int operator()(unsigned int time)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(time));
        std::cout << "thread id: " << std::this_thread::get_id() << " " << time << std::endl;
        return time + 1;
    }
};

class TestClass
{
public:
    static int TestFunction2(unsigned int time = 0)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(time));
        std::cout << "thread id: " << std::this_thread::get_id() << " " << time << std::endl;
        return time;
    }

    std::string TestFunction3(unsigned int time, std::string str, char c)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(time));
        std::cout << "thread id: " << std::this_thread::get_id() << " " << time << " " << str << " " << c << std::endl;
        return str;
    }
};

TEST(ThreadPool, Function)
{
    auto testRet = ThreadPool::Instance(10)->Enqueue(TestFunction1, 3000);
    EXPECT_EQ(testRet.get(), 3000);
}

TEST(ThreadPool, Functor)
{
    auto testRet = ThreadPool::Instance(10)->Enqueue(TestFunctors(), 1500);
    EXPECT_EQ(testRet.get(), 1501);
}

TEST(ThreadPool, MemberFunction)
{
    TestClass testClass;
    auto      testRet =
        ThreadPool::Instance(10)->Enqueue(std::bind(&TestClass::TestFunction3, &testClass, 2000, "MemberFunction", 'F'));
    EXPECT_EQ(testRet.get(), "MemberFunction");
}

TEST(ThreadPool, StaticMemberFunction)
{
    auto testRet = ThreadPool::Instance(10)->Enqueue(TestClass::TestFunction2, 1000);
    EXPECT_EQ(testRet.get(), 1000);
}

TEST(ThreadPool, Lambda)
{
    auto testRet = ThreadPool::Instance(10)->Enqueue([] {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        std::cout << "thread id: " << std::this_thread::get_id() << "Lambda" << std::endl;
        return 999;
    });
    EXPECT_EQ(testRet.get(), 999);
}

TEST(ThreadPool, MethodChaining)
{
    Task<std::string(std::string)> task([](std::string i) { return i; });

    std::string u = " test";
    auto        f = task.Then([&u](std::string i) {
                     std::this_thread::sleep_for(std::chrono::milliseconds(500));
                     return i.append(u);
                 })
                 .Then([](std::string i) {
                     std::this_thread::sleep_for(std::chrono::milliseconds(500));
                     return i.append(" was");
                 })
                 .Then([](std::string i) {
                     std::this_thread::sleep_for(std::chrono::milliseconds(500));
                     return i.append(" successful.");
                 })
                 .Func();

    auto testRet = ThreadPool::Instance(10)->Enqueue(f, "MethodChaining");
    EXPECT_EQ(testRet.get(), "MethodChaining test was successful.");
}

TEST(ThreadPool, UnitTest)
{
    ThreadPool::Instance(10)->Enqueue(TestFunction1, 3000);
    auto      testRet1 = ThreadPool::Instance()->Enqueue(TestFunctors(), 1500);
    TestClass testClass;
    auto      testRet2 = ThreadPool::Instance()->Enqueue(TestClass::TestFunction2, 1000);
    auto      testRet3 =
        ThreadPool::Instance()->Enqueue(std::bind(&TestClass::TestFunction3, &testClass, 2000, "MemberFunction", 'F'));
    std::this_thread::sleep_for(std::chrono::milliseconds(800));
    int idleThreadCount1 = ThreadPool::Instance()->GetIdleThreadCount();
    EXPECT_EQ(idleThreadCount1, 6);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    int idleThreadCount2 = ThreadPool::Instance()->GetIdleThreadCount();
    EXPECT_EQ(idleThreadCount2, 7);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    int idleThreadCount3 = ThreadPool::Instance()->GetIdleThreadCount();
    EXPECT_EQ(idleThreadCount3, 8);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    int idleThreadCount4 = ThreadPool::Instance()->GetIdleThreadCount();
    EXPECT_EQ(idleThreadCount4, 9);
    EXPECT_EQ(testRet1.get(), 1501);
    EXPECT_EQ(testRet2.get(), 1000);
    EXPECT_EQ(testRet3.get(), "MemberFunction");
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}