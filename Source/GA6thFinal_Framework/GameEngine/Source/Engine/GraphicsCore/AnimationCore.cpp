#include "pch.h"
#include "AnimationCore.h"
#include "Animator.h"

AnimationCore::AnimationCore()
{
}

AnimationCore::~AnimationCore()
{
    for (auto& event : _threadEvents)
        event = ThreadEvent::DESTROY;

    for (auto& cv : _cvs)
        cv->notify_one();

    for (auto& thread : _threads)
        thread.join();
}

void AnimationCore::RegisterAnimator(std::shared_ptr<Animator> animator)
{
    _components.push_back(animator.get());
}

void AnimationCore::Initialize(const unsigned int maxThread)
{
    _maxThread = maxThread;
    _components.resize(maxThread);
    _cvs.reserve(maxThread);
    _mutexes.reserve(maxThread);
    _threadEvents.resize(maxThread, ThreadEvent::NONE);

    for (unsigned int i = 0; i < maxThread; ++i)
    {
        _threads[i] = std::thread(&AnimationCore::WorkerThread, this, i);
        _cvs.push_back(std::make_unique<std::condition_variable>());
        _mutexes.push_back(std::make_unique<std::mutex>());
    }
}

void AnimationCore::Update(const float deltaTime)
{
    unsigned int size = (unsigned int)_components.size();

    // Animator가 64개 미만이면 스레드를 사용하지 않음
    if (size < 64)
    {
        for (auto& component : _components)
            component->Update(deltaTime);
    }
    else
    {
        _offset         = size / _maxThread;
        _deltaTime      = deltaTime;
        _remainingTasks = _maxThread;

        for (unsigned int i = 0; i < _maxThread; i++)
        {
            _threadEvents[i] = ThreadEvent::PROCESS;
            _cvs[i]->notify_one();
        }

        std::unique_lock<std::mutex> lock(_mutexDone);
        _cvDone.wait(lock, [this]() { return 0 == _remainingTasks; });
    }
}

void AnimationCore::WorkerThread(unsigned int index)
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
            unsigned int start = index * _offset;
            unsigned int end   = start + _offset;

            if (index + 1 == _maxThread)
            {
                end += _components.size() % _maxThread;
            }

            for (unsigned int i = start; i < end; i++)
                _components[i]->Update(_deltaTime);

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