#include "pch.h"
#include "ThreadPool.h"

constexpr unsigned int MAX_THREAD = 3;

ThreadPool::ThreadPool() = default;

ThreadPool::~ThreadPool() = default;

void ThreadPool::WorkerThread(unsigned int index)
{
    bool isLoop = true;
    while (isLoop)
    {
        std::unique_lock<std::mutex> lock(*_mutexes[index]);
        _cvs[index]->wait(lock, [this, index] { return ThreadEvent::NONE != _threadEvents[index]; });

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
                }
            }

            {
                std::scoped_lock nestLock(_mutexDone);
                _remainingTasks--;

                if (0 == _remainingTasks)
                    _cvDone.notify_one();
            }

            _threadEvents[index] = ThreadEvent::NONE;
            break;
        }
        case ThreadEvent::DESTROY:
            isLoop = false;
            break;
        }
    }
}