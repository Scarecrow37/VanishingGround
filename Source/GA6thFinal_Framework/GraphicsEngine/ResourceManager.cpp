#include "pch.h"
#include "ResourceManager.h"

constexpr unsigned int MAX_THREAD = 4;

ResourceManager::ResourceManager()
{
    _threads.reserve(MAX_THREAD);
    _mutexes.resize(MAX_THREAD);

    for (auto& mutex : _mutexes)
    {
        mutex = std::make_unique<std::mutex>();
    }
}

ResourceManager::~ResourceManager()
{
    _threadEvent = ThreadEvent::DESTROY;
    _cv.notify_all();

    for (auto& thread : _threads)
    {
        if (thread.joinable())
            thread.join();
    }
}

void ResourceManager::RequestResource(const std::function<void()>& callback)
{
    _taskQueue.push(callback);
}

void ResourceManager::Update()
{
    if (_taskQueue.empty())
        return;
    
    _threadEvent = ThreadEvent::PROCESS;
    _cv.notify_all();

    if (_threads.empty())
    {
        for (unsigned int i = 0; i < MAX_THREAD; ++i)
        {
            std::thread thread(&ResourceManager::WorkerThread, this, i);
            thread.detach();
            _threads.emplace_back(std::move(thread));
        }
    }
}

void ResourceManager::WorkerThread(const unsigned int index)
{
    bool isLoop = true;
    while (isLoop)
    {
        std::unique_lock<std::mutex> lock(*_mutexes[index]);
        _cv.wait(lock, [this] { return ThreadEvent::NONE != _threadEvent; });

        switch (_threadEvent)
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
            _threadEvent = ThreadEvent::NONE;
            break;
        }
        case ThreadEvent::DESTROY:
            isLoop = false;
            break;
        }
    }
}