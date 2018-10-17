#include "utility/ThreadPool.h"
#include <algorithm>
#include <functional>

void worker(ThreadPool::Context& ctx) {
    while (true) {
        std::unique_lock<std::mutex> lock(ctx.mutex);

        ctx.task_scheduled.wait(lock, [&] {
            return ctx.shutting_down || ctx.pending_tasks.size() > 0;
        });

        if (ctx.shutting_down)
            return;

        std::unique_ptr<BaseTask> task = std::move(ctx.pending_tasks.front());
        ctx.pending_tasks.pop_front();
        ++ctx.processing_tasks;
        lock.unlock();

        task->invoke();

        --ctx.processing_tasks;
        ctx.task_finished.notify_all();
    }
}

ThreadPool::ThreadPool(std::size_t num_threads) {
    std::generate_n(std::back_inserter(this->threads), num_threads, [&]{
        return std::thread(worker, std::ref(this->context));
    });
}

ThreadPool::~ThreadPool() {
    {
        std::lock_guard<std::mutex> lock(this->context.mutex);
        this->context.shutting_down = true;
    }
    this->context.task_scheduled.notify_all();

    for (std::thread& thread : this->threads)
        thread.join();
}

void ThreadPool::wait_for_all() {
    std::unique_lock<std::mutex> lock(this->context.mutex);
    this->context.task_finished.wait(lock, [&] {
        return this->context.pending_tasks.size() == 0 && this->context.processing_tasks.load() == 0;
    });
}
