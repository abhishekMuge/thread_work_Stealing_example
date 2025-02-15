//============================================================================
// Name        : Thread_work_Stealing.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <random>
#include <atomic>

struct Task {
    int priority;
    std::function<void()> func;
    bool operator<(const Task &other) const { return priority < other.priority; }
};

class WorkStealingThreadPool {
public:
    WorkStealingThreadPool(int num_threads) : stop(false) {
        for (int i = 0; i < num_threads; ++i) {
            threads.emplace_back(&WorkStealingThreadPool::worker, this, i);
        }
    }

    ~WorkStealingThreadPool() {
        stop = true;
        cv.notify_all();
        for (auto &t : threads) {
            if (t.joinable()) t.join();
        }
    }

    void submit(int priority, std::function<void()> task, int thread_id) {
        {
            std::lock_guard<std::mutex> lock(queue_mutexes[thread_id]);
            task_queues[thread_id].push({priority, task});
        }
        cv.notify_all();
    }

private:
    std::vector<std::thread> threads;
    std::vector<std::priority_queue<Task>> task_queues{std::thread::hardware_concurrency()};
    std::vector<std::mutex> queue_mutexes{std::thread::hardware_concurrency()};
    std::condition_variable cv;
    std::atomic<bool> stop;

    void worker(size_t thread_id) {
        std::mt19937 rng(thread_id); //Random number generator
        while (!stop) {
            Task task;
            bool found_task = false;
            {
                std::lock_guard<std::mutex> lock(queue_mutexes[thread_id]);
                if (!task_queues[thread_id].empty()) {
                    task = task_queues[thread_id].top();
                    task_queues[thread_id].pop();
                    found_task = true;
                }
            }

            if (!found_task) {
                for (size_t i = 0; i < threads.size(); ++i) {
                    if (i == thread_id) continue;
                    std::lock_guard<std::mutex> lock(queue_mutexes[i]);
                    if (!task_queues[i].empty()) {
                        task = task_queues[i].top();
                        task_queues[i].pop();
                        found_task = true;
                        break;
                    }
                }
            }

            if (found_task) {
                task.func();
            } else {
                std::unique_lock<std::mutex> lock(queue_mutexes[thread_id]);
                cv.wait_for(lock, std::chrono::milliseconds(50));
            }
        }
    }
};

int main() {
    int num_threads = 4;
    WorkStealingThreadPool pool(num_threads);

    for (int i = 0; i < 10; ++i) {
        pool.submit(i % 5, [i, num_threads]() { std::cout << "Task " << i << " executed. by Thread "<< i % num_threads  << std::endl; }, i % num_threads);
    }

    std::this_thread::sleep_for(std::chrono::seconds(2));
    return 0;
}

