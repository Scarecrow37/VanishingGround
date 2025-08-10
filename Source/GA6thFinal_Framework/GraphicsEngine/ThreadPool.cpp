#include "pch.h"
#include "ThreadPool.h"

ThreadPool::ThreadPool() = default;

ThreadPool::~ThreadPool()
{    
    for (auto& event : _threadEvents)
        event = ThreadEvent::DESTROY;
        
    _cv.notify_all();

    for (auto& thread : _threads)
    {
        if (thread.joinable())
            thread.join();
    }
}

void ThreadPool::Initialize(unsigned int threadCount)
{
    _threads.resize(threadCount);
    _mutexes.resize(threadCount);
    _threadEvents.resize(threadCount);
    _remainingTasks = 0;

    for (unsigned int i = 0; i < threadCount; i++)
    {
        _mutexes[i] = std::make_unique<std::mutex>();
        _threads[i] = std::thread(&ThreadPool::WorkerThread, this, i);
    }
}

void ThreadPool::AddTask(const std::function<void()> task)
{    
    _taskQueue.push(task);
    _remainingTasks++;
}

void ThreadPool::Done()
{
    if (0 == _remainingTasks)
        return;

    for (auto& event : _threadEvents)
    {
        event = ThreadEvent::PROCESS;
    }

    _cv.notify_all();

    std::unique_lock<std::mutex> lock(_mutexDone);
    _cvDone.wait(lock, [this] { return 0 == _remainingTasks; });
}

void ThreadPool::WorkerThread(const unsigned int index)
{
    bool isLoop = true;
    while (isLoop)
    {
        std::unique_lock<std::mutex> lock(*_mutexes[index]);
        _cv.wait(lock, [this, index] { return ThreadEvent::NONE != _threadEvents[index]; });

        switch (_threadEvents[index])
        {
        case ThreadEvent::PROCESS:
        {
            std::function<void()> task;
            while (!_taskQueue.empty())
            {
                if (_taskQueue.try_pop(task))
                {
                    task();
                    _remainingTasks--;
                }
            }

            _threadEvents[index] = ThreadEvent::NONE;

            if (0 == _remainingTasks)
            {
                std::unique_lock<std::mutex> doneLock(_mutexDone);
                _cvDone.notify_one();
            }
            break;
        }
        case ThreadEvent::DESTROY:
            isLoop = false;
            break;
        }
    }
}