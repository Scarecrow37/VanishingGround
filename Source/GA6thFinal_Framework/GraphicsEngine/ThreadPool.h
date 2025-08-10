#pragma once

class ThreadPool
{
    enum class ThreadEvent { NONE, PROCESS, DESTROY };
    enum CVEvent { THREAD, DONE };

public:
    ThreadPool();
    ~ThreadPool();

private:
    void WorkerThread(unsigned int index);

private:
    std::condition_variable                                  _cvDone;
    std::mutex                                               _mutexDone;    
    std::vector<std::unique_ptr<std::condition_variable>>    _cvs;
    std::vector<std::unique_ptr<std::mutex>>                 _mutexes;
    std::vector<std::thread>                                 _threads;
    std::vector<ThreadEvent>                                 _threadEvents;
    Concurrency::concurrent_queue<std::function<void()>>     _taskQueue;
    unsigned int                                             _remainingTasks{0};
    unsigned int                                             _offset{0};
    float                                                    _deltaTime{0.f};
};