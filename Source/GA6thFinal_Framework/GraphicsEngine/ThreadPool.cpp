#include "pch.h"
#include "ThreadPool.h"

ThreadPool::ThreadPool() = default;

ThreadPool::~ThreadPool()
{    
    for (auto& events : _threadEvents)
    {
        for (auto& event : events)
        {
            event = ThreadEvent::DESTROY;
        }
    }

    _cv[ASYNK].notify_all();
    _cv[PARALLEL].notify_all();

    for (auto& threads : _threads)
    {
        for (auto& thread : threads)
        {
            if (thread.joinable())
                thread.join();
        }
    }
}

bool ThreadPool::IsParallelTaskDone() const
{
    return _taskQueue[PARALLEL].empty() && (0 == _parallelRemainingTasks);
}

void ThreadPool::AddTask(ThreadType type, const std::function<void(ID3D12GraphicsCommandList*)>& task)
{
    if (ThreadType::PARALLEL == type)
    {
        _parallelRemainingTasks++;
    }

    _taskQueue[type].push(task);
}

void ThreadPool::Initialize(unsigned int threadCount)
{
    for (auto& events : _threadEvents)
    {
        events.resize(threadCount, ThreadEvent::NONE);
    }

    for (auto& mutexes : _mutexes)
    {
        mutexes.resize(threadCount);
    }

    for (auto& mutexes : _mutexes)
    {
        for (auto& mutex : mutexes)
        {
            mutex = std::make_unique<std::mutex>();
        }
    }

    for (auto& commandSets : _commandSets)
    {
        commandSets.resize(threadCount);
    }

    for (auto& threads : _threads)
    {
        threads.resize(threadCount);
    }
        
    for (unsigned int i = 0; i < threadCount; i++)
    {
        _commandSets[ASYNK][i].Initialize(CommandType::DIRECT, L"ThreadPoolAsynkCommandSet" + std::to_wstring(i));
        _commandSets[ASYNK][i]->Close();

        _commandSets[PARALLEL][i].Initialize(CommandType::DIRECT, L"ThreadPoolParallelCommandSet" + std::to_wstring(i));
        _commandSets[PARALLEL][i]->Close();

        _threads[ASYNK][i] = std::thread(&ThreadPool::AsyncWorkerThread, this, std::ref(_commandSets[ASYNK][i]), i);
        _threads[PARALLEL][i] = std::thread(&ThreadPool::ParallelWorkerThread, this, std::ref(_commandSets[PARALLEL][i]), i);
    }

    _parallelRemainingTasks = 0;
}

void ThreadPool::Update()
{
    if (_isDone)
    {
        if (Global::commandController->IsCompleteCommandQueue(CommandQueueType::GRAPHICS_QUEUE, _parallelFenceValue))
        {
            for (auto& commandSet : _commandSets[PARALLEL])
            {
                commandSet.Reset();
            }

            _isDone = false;
        }
    }

    if (!_taskQueue[ASYNK].empty())
    {
        for (auto& event : _threadEvents[ASYNK])
        {
            event = ThreadEvent::PROCESS;
        }

        _cv[ASYNK].notify_all();
    }

    if (0 == _parallelRemainingTasks)
        return;

    for (auto& event : _threadEvents[PARALLEL])
    {
        event = ThreadEvent::PROCESS;
    }

    _isDone = false;
    _cv[PARALLEL].notify_all();

    std::unique_lock<std::mutex> lock(_parallelMutexDone);
    _parallelCvDone.wait(lock, [this] { return 0 == _parallelRemainingTasks; });

    _parallelFenceValue = Global::commandController->SignalCommandQueue(CommandQueueType::GRAPHICS_QUEUE);
}

void ThreadPool::ParallelWorkerThread(CommandSet& commandSet, const unsigned int index)
{
    bool isLoop = true;
    while (isLoop)
    {
        std::unique_lock<std::mutex> lock(*_mutexes[PARALLEL][index]);
        _cv[PARALLEL].wait(lock, [this, index] { return ThreadEvent::NONE != _threadEvents[PARALLEL][index]; });

        switch (_threadEvents[PARALLEL][index])
        {
        case ThreadEvent::PROCESS:
        {
            std::function<void(ID3D12GraphicsCommandList*)> task;
            commandSet->Reset(commandSet, nullptr);
            while (!_taskQueue[PARALLEL].empty())
            {
                if (_taskQueue[PARALLEL].try_pop(task))
                {
                    task(commandSet);
                    _parallelRemainingTasks--;
                }
            }
            commandSet.ExecuteCommand();

            _threadEvents[PARALLEL][index] = ThreadEvent::NONE;

            if (0 == _parallelRemainingTasks)
            {
                std::unique_lock<std::mutex> doneLock(_parallelMutexDone);
                _isDone = true;
                _parallelCvDone.notify_one();
            }
            break;
        }
        case ThreadEvent::DESTROY:
            isLoop = false;
            break;
        }
    }
}

void ThreadPool::AsyncWorkerThread(CommandSet& commandSet, const unsigned int index)
{
    bool isLoop = true;
    while (isLoop)
    {
        std::unique_lock<std::mutex> lock(*_mutexes[ASYNK][index]);
        _cv[ASYNK].wait(lock, [this, index] { return ThreadEvent::NONE != _threadEvents[ASYNK][index]; });

        switch (_threadEvents[ASYNK][index])
        {
        case ThreadEvent::PROCESS:
        {
            std::function<void(ID3D12GraphicsCommandList*)> task;
            while (!_taskQueue[ASYNK].empty())
            {
                if (_taskQueue[ASYNK].try_pop(task))
                {
                    task(commandSet);
                }
            }

            _threadEvents[ASYNK][index] = ThreadEvent::NONE;
            break;
        }
        case ThreadEvent::DESTROY:
            isLoop = false;
            break;
        }
    }
}