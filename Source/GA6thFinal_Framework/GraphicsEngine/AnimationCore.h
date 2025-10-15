#pragma once

class Animator;
class AnimationCore
{
    enum class ThreadEvent
    {
        NONE,
        PROCESS,
        DESTROY
    };
    enum CVEvent
    {
        THREAD,
        DONE
    };

public:
    AnimationCore();
    ~AnimationCore();

public:
    void RegisterAnimator(Animator* animator);
    void ClearAnimationQueue();

public:
    void Initialize(const unsigned int maxThread);
    void Update(const float deltaTime);

private:
    void WorkerThread(unsigned int index);

private:
    std::condition_variable                                  _cvDone;
    std::mutex                                               _mutexDone;
    std::vector<Animator*>                                   _components;
    std::vector<std::unique_ptr<std::condition_variable>>    _cvs;
    std::vector<std::unique_ptr<std::mutex>>                 _mutexes;
    std::vector<std::thread>                                 _threads;
    std::vector<ThreadEvent>                                 _threadEvents;
    unsigned int                                             _remainingTasks{0};
    unsigned int                                             _maxThread{0};
    float                                                    _deltaTime{0.f};
    unsigned int                                             _offset{0};
};