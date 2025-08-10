#include "pch.h"
#include "ResourceManager.h"

constexpr unsigned int MAX_THREAD = 3;

ResourceManager::ResourceManager()
{
    _threads.reserve(MAX_THREAD);
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
            std::thread thread(&ResourceManager::WorkerThread, this);
            _threads.emplace_back(std::move(thread));
            thread.detach();
        }
    }
}

void ResourceManager::WorkerThread()
{
    bool isLoop = true;
    while (isLoop)
    {
        std::unique_lock<std::mutex> lock(_mutex);
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