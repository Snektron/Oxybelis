#ifndef _OXYBELIS_UTILITY_THREADPOOL_H
#define _OXYBELIS_UTILITY_THREADPOOL_H

#include <vector>
#include <thread>
#include <cstddef>
#include <condition_variable>
#include <mutex>
#include <deque>
#include <atomic>
#include <memory>

class BaseTask {
public:
    virtual void invoke() = 0;
    virtual ~BaseTask() = default;
};

template <typename F>
class Task: public BaseTask {
    F f;

public:
    Task(F&& f);
    virtual void invoke() override;
};

class ThreadPool {
    struct Context {
        std::mutex mutex;
        std::condition_variable task_scheduled;
        std::condition_variable task_finished;
        std::deque<std::unique_ptr<BaseTask>> pending_tasks;
        std::atomic_size_t processing_tasks;
        bool shutting_down;

        Context():
            processing_tasks(0),
            shutting_down(false) {
        }
    };

    Context context;
    std::vector<std::thread> threads;

public:
    ThreadPool(std::size_t num_threads);
    ~ThreadPool();

    template <typename F>
    void schedule(F&& task);

    void wait_for_all();

    friend void worker(Context&);
};

template <typename F>
Task<F>::Task(F&& f):
    f(std::move(f)) {
}

template <typename F>
void Task<F>::invoke() {
    this->f();
}

template <typename F>
void ThreadPool::schedule(F&& f) {
    {
        std::unique_ptr<Task<F>> task(new Task<F>(std::move(f)));
        std::lock_guard<std::mutex> lock(this->context.mutex);
        this->context.pending_tasks.push_back(std::move(task));
    }
    this->context.task_scheduled.notify_one();
}

#endif
