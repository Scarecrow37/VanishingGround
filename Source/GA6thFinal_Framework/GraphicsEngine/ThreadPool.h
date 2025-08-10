#pragma once

class ThreadPool
{
    enum class ThreadEvent { NONE, PROCESS, DONE, DESTROY };

public:
    ThreadPool();
    ~ThreadPool();

public:
    void Initialize(unsigned int threadCount);
    void AddTask(const std::function<void()> task);
    void Done();

private:
    void WorkerThread(const unsigned int index);

private:
    std::condition_variable                              _cvDone;
    std::mutex                                           _mutexDone;
    std::condition_variable                              _cv;
    std::vector<std::unique_ptr<std::mutex>>             _mutexes;
    std::vector<std::thread>                             _threads;
    std::vector<ThreadEvent>                             _threadEvents;
    Concurrency::concurrent_queue<std::function<void()>> _taskQueue;
    std::atomic<unsigned int>                            _remainingTasks;
};