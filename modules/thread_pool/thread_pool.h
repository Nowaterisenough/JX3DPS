/**
 * @Description : 
 * @Author      : NoWats
 * @Date        : 2023-04-08 02:23:52
 * @Update      : NoWats
 * @LastTime    : 2023-04-08 06:26:58
 * @FilePath    : \LegionTDSim\modules\ThreadPool\ThreadPool.h
 */


#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <vector>
#include <queue>
#include <atomic>
#include <future>
#include <functional>

#if defined _WIN32 || defined __CYGWIN__
#    if defined EXPORT_THREAD_POOL // CMake add_definitions
#        ifdef __GNUC__
#            define THREAD_POOL_API __attribute__((dllexport))
#        else
#            define THREAD_POOL_API __declspec(dllexport)
#        endif // __GNUC__
#    else
#        ifdef __GNUC__
#            define THREAD_POOL_API __attribute__((dllimport))
#        else
#            ifndef IMPORT_STATIC_THREAD_POOL_LIB
#                define THREAD_POOL_API __declspec(dllimport)
#            else
#                define THREAD_POOL_API
#            endif
#        endif // __GNUC__
#    endif     // EXPORT_THREAD_POOL
#    define THREAD_POOL_PRIVATE

#elif defined __GNUC__
#    if __GNUC__ >= 4
#        define THREAD_POOL_API     __attribute__((visibility("default")))
#        define THREAD_POOL_PRIVATE __attribute__((visibility("hidden")))
#    else
#        define THREAD_POOL_API
#        define THREAD_POOL_PRIVATE
#    endif // __GNUC__ >= 4

#elif defined __clang__
#    define THREAD_POOL_API     __attribute__((visibility("default")))
#    define THREAD_POOL_PRIVATE __attribute__((visibility("hidden")))

#else
#    error "Do not know how to export classes for this platform"
#endif // defined(_WIN32) || defined(__CYGWIN__)

template <typename T>
class Task;

template <typename R, typename... Args>
class THREAD_POOL_API Task<R(Args...)>
{
    std::function<R(Args...)> m_fn;

public:
    Task(std::function<R(Args...)> &f) : m_fn(f) {}
    Task(std::function<R(Args...)> &&f) : m_fn(std::move(f)) {}

    R Run(Args &&...args)
    {
        return m_fn(std::forward<Args>(args)...);
    }

    template <typename F>
    auto Then(F &&f) -> Task<typename std::invoke_result_t<F, R>(Args...)>
    {
        using RetType = typename std::invoke_result_t<F, R>;
        auto func     = std::move(m_fn);
        return Task<RetType(Args...)>(
            [func, &f](Args &&...args) { return f(func(std::forward<Args>(args)...)); });
    }

    auto &Func()
    {
        return m_fn;
    }
};

class THREAD_POOL_API ThreadPool
{
    using Task = std::function<void()>;
    std::vector<std::thread> m_pool;               // 线程池
    std::queue<Task>         m_tasks;              // 任务队列
    std::mutex               m_lock;               // 同步
    std::condition_variable  m_taskCV;             // 条件阻塞
    std::atomic<bool>        m_run{true};          // 线程池是否执行
    std::atomic<int>         m_idleThreadCount{0}; // 空闲线程数量

public:
    static ThreadPool *Instance(unsigned short size = std::thread::hardware_concurrency());

    ThreadPool(const ThreadPool &) = delete;
    ThreadPool &operator=(const ThreadPool &) = delete;

    inline ~ThreadPool()
    {
        m_run = false;
        m_taskCV.notify_all(); // 唤醒所有线程执行
        for (auto &thread : m_pool) {
            if (thread.joinable()) {
                thread.join(); // 等待任务结束
            }
        }
    }

    /**
     * @brief  添加一个任务到任务队列
     * @tparam {F} 函数类型
     * @tparam {Args...} 可变参类型
     * @param  {F} && 支持函数; 静态成员函数; 仿函数; 匿名函数; std::bind & std::mem_fn封装的成员函数
     * @param  {Args...} && 可变参
     * @return {auto} 函数返回值, 用std::future接收
     */
    template <typename F, typename... Args>
    auto Enqueue(F &&f, Args &&...args) -> std::future<decltype(f(args...))>
    {
        if (!m_run) { // 线程池生命周期意外终止
            return std::future<decltype(f(args...))>();
        }
        using RetType = decltype(f(args...));
        auto task     = std::make_shared<std::packaged_task<RetType()>>(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...));
        std::future<RetType> future = task->get_future();
        {
            std::lock_guard<std::mutex> lock{m_lock};
            m_tasks.emplace([task]() { (*task)(); });
        }
        m_taskCV.notify_one();
        return future;
    }

    /**
     * @brief 空闲线程数量
     * @param {*}
     * @return {int}
     */
    int GetIdleThreadCount()
    {
        return m_idleThreadCount;
    }

    /**
     * @brief 线程数量
     * @param {*}
     * @return {size_t}
     */
    size_t GetThreadCount()
    {
        return m_pool.size();
    }

private:
    inline ThreadPool(unsigned short size = std::thread::hardware_concurrency())
    {
        AddThread(size);
    }

    /**
     * @brief 添加指定数量的线程
     * @param {unsigned short} size
     * @return {*}
     */
    void AddThread(unsigned short size)
    {
        for (; size > 0; --size) {
            m_pool.emplace_back([this] {
                while (m_run) {
                    Task task; // 获取一个待执行的 task
                    {
                        std::unique_lock<std::mutex> lock{m_lock};
                        m_taskCV.wait(lock, [this] { // wait 直到有 task
                            return !m_run || !m_tasks.empty();
                        });
                        if (!m_run && m_tasks.empty()) {
                            return;
                        }
                        task = std::move(m_tasks.front()); // 按先进先出从队列取一个 task
                        m_tasks.pop();
                    }
                    m_idleThreadCount--;
                    task(); //执行任务
                    m_idleThreadCount++;
                }
            });
            m_idleThreadCount++;
        }
    }
};

#endif // THREAD_POOL_H
