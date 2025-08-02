#pragma once
#include <array>
#include <atomic>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

class JobSystem {

private:
    using Task = std::function<void()>;

    void workerLoop() {
        while (true) {
            Task task;
            
            {
                std::unique_lock<std::mutex> lk(mu);
                cv.wait(lk, [this] { return stop || !q.empty(); });
                if (stop && q.empty()) return;
                task = std::move(q.front());
                q.pop();
            }

            task();

            if (--pending == 0) {
                std::lock_guard<std::mutex> lk(doneMu);
                doneCv.notify_one();
            }
        }
    }

    std::vector<std::thread> threads;
    std::queue<Task> q;
    std::condition_variable cv;
    std::mutex mu;
    std::atomic_uint pending{0};

    std::condition_variable doneCv;
    std::mutex doneMu;
    bool stop;


public:
    explicit JobSystem(std::size_t workers = std::max(1u, std::thread::hardware_concurrency() - 1u)) : stop(false) {
        for (std::size_t i = 0; i < workers; ++i) {
            threads.emplace_back([this] { workerLoop(); });
        }
    }
    
    ~JobSystem() {
        {
            std::lock_guard<std::mutex> lk(mu);
            stop = true;
        }

        cv.notify_all();
        for (auto &t : threads)
            t.join();
    }

    void push(Task &&task) {
        {
            std::lock_guard<std::mutex> lk(mu);
            q.emplace(std::move(task));
            ++pending;
        }

        cv.notify_one();
    }

    void wait() {
        std::unique_lock<std::mutex> lk(doneMu);
        doneCv.wait(lk, [this] { return pending.load() == 0; });
    }
};
