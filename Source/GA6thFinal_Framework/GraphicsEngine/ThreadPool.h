#pragma once

class ThreadPool
{
    enum class ThreadEvent { NONE, PROCESS, DONE, DESTROY };

public:
    enum ThreadType { ASYNK, PARALLEL, END };

public:
    ThreadPool();
    ~ThreadPool();

public:
    bool IsParallelTaskDone() const;

public:
    void AddTask(ThreadType type, const std::function<void(ID3D12GraphicsCommandList*)>& task);

public:
    void Initialize(unsigned int threadCount);
    void Update();

private:
    void ParallelWorkerThread(CommandSet& commandSet, const unsigned int index);
    void AsyncWorkerThread(CommandSet& commandSet, const unsigned int index);

private:
    Concurrency::concurrent_queue<std::function<void(ID3D12GraphicsCommandList*)>> _taskQueue[END];
    std::condition_variable                                                        _cv[END];
    std::vector<std::thread>                                                       _threads[END];
    std::vector<CommandSet>                                                        _commandSets[END];
    std::vector<ThreadEvent>                                                       _threadEvents[END];
    std::vector<std::unique_ptr<std::mutex>>                                       _mutexes[END];

    std::mutex                _parallelMutexDone;
    std::atomic<unsigned int> _parallelRemainingTasks;
    std::condition_variable   _parallelCvDone;
    bool                      _isDone             = false;
    UINT64                    _parallelFenceValue = 0;
};