#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <atomic>
#include <functional>
#include <future>
#include <queue>
#include <vector>

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
    std::function<R(Args...)> _fn;

public:
    Task(std::function<R(Args...)> &f) : _fn(f) { }

    Task(std::function<R(Args...)> &&f) : _fn(std::move(f)) { }

    R Run(Args &&...args) { return _fn(std::forward<Args>(args)...); }

    template <typename F>
    auto Then(F &&f) -> Task<typename std::invoke_result_t<F, R>(Args...)>
    {
        using RetType = typename std::invoke_result_t<F, R>;
        auto func     = std::move(_fn);
        return Task<RetType(Args...)>([func, &f](Args &&...args) {
            return f(func(std::forward<Args>(args)...));
        });
    }

    auto &Func() { return _fn; }
};

class THREAD_POOL_API ThreadPool
{
    using Task = std::function<void()>;
    std::vector<std::thread> _pool;                   // 线程池
    std::queue<Task>         _tasks;                  // 任务队列
    std::mutex               _lock;                   // 同步
    std::condition_variable  _task_cv;                // 条件阻塞
    std::atomic<bool>        _run{ true };            // 线程池是否执行
    std::atomic<int>         _idle_thread_count{ 0 }; // 空闲线程数量

public:
    static ThreadPool *GetInstance(unsigned short size = std::thread::hardware_concurrency());

    ThreadPool(const ThreadPool &)            = delete;
    ThreadPool &operator=(const ThreadPool &) = delete;

    ~ThreadPool()
    {
        _run = false;
        _task_cv.notify_all(); // 唤醒所有线程执行
        for (auto &thread : _pool) {
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
        if (!_run) { // 线程池生命周期意外终止
            return std::future<decltype(f(args...))>();
        }
        using RetType = decltype(f(args...));
        auto task     = std::make_shared<std::packaged_task<RetType()>>(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...));
        std::future<RetType> future = task->get_future();
        {
            std::lock_guard<std::mutex> lock{ _lock };
            _tasks.emplace([task]() { (*task)(); });
        }
        _task_cv.notify_one();
        return future;
    }

    /**
     * @brief 空闲线程数量
     * @param {*}
     * @return {int}
     */
    int GetIdleThreadCount() { return _idle_thread_count; }

    /**
     * @brief 线程数量
     * @param {*}
     * @return {size_t}
     */
    size_t GetThreadCount() { return _pool.size(); }

private:
    ThreadPool(unsigned short size = std::thread::hardware_concurrency())
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
            _pool.emplace_back([this] {
                while (_run) {
                    Task task; // 获取一个待执行的 task
                    {
                        std::unique_lock<std::mutex> lock{ _lock };
                        _task_cv.wait(lock, [this] { // wait 直到有 task
                            return !_run || !_tasks.empty();
                        });
                        if (!_run && _tasks.empty()) {
                            return;
                        }
                        task = std::move(_tasks.front()); // 按先进先出从队列取一个 task
                        _tasks.pop();
                    }
                    _idle_thread_count--;
                    task(); // 执行任务
                    _idle_thread_count++;
                }
            });
            _idle_thread_count++;
        }
    }
};

#endif // THREAD_POOL_H
