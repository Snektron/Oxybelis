#include "utility/ThreadPool.h"
#include <algorithm>
#include <functional>

ThreadPool::ThreadPool(std::size_t num_threads) {
    std::generate_n(std::back_inserter(this->threads), num_threads, [&]{
        return std::thread(std::bind(&ThreadPool::worker, this), std::ref(this->context));
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

void ThreadPool::worker() {
    while (true) {
        std::unique_lock<std::mutex> lock(this->context.mutex);

        this->context.task_scheduled.wait(lock, [&] {
            return this->context.shutting_down || this->context.pending_tasks.size() > 0;
        });

        if (this->context.shutting_down)
            return;

        std::unique_ptr<BaseTask> task = std::move(this->context.pending_tasks.front());
        this->context.pending_tasks.pop_front();
        ++this->context.processing_tasks;
        lock.unlock();

        task->invoke();

        --this->context.processing_tasks;
        this->context.task_finished.notify_all();
    }
}
