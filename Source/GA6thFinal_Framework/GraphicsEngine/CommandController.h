#pragma once
#include <mutex>
#include <atomic>

class CommandController
{
public:
    CommandController() = default;
    ~CommandController();

public:
    ID3D12CommandQueue* GetCommandQueue(CommandQueueType type) const { return _commandQueue[type].Get(); }

public:
    void Initialize();
    void AddCommandSet(CommandType command, std::wstring_view resourceName, CommandSet& out);

public:
    void WaitForCommandQueue(CommandQueueType type, UINT64 fenceValue);
    void WaitCommandQueue(CommandQueueType queue, CommandQueueType fence, UINT64 fenceValue);
    void ExecuteCommand(CommandQueueType type, ID3D12CommandList* commandList);
    void ResetCommand(CommandQueueType type);

public:
    UINT64 SignalCommandQueue(CommandQueueType type);
    bool   IsCompleteCommandQueue(CommandQueueType queue, UINT64 fenceValue);

private:
    std::mutex				   _mutex;
    std::vector<CommandSet>    _commandSets;
    ComPtr<ID3D12CommandQueue> _commandQueue[COMMAND_QUEUE_END];
    ComPtr<ID3D12Fence>        _fence[COMMAND_QUEUE_END];
    std::atomic<UINT64>        _fenceValue[COMMAND_QUEUE_END]{0};
    HANDLE                     _fenceEvent[COMMAND_QUEUE_END]{nullptr};
};