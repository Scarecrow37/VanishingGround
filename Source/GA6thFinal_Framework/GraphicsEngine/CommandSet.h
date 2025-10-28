#pragma once

class CommandSet
{
    friend class CommandController;

public:
    CommandSet()  = default;
    ~CommandSet() = default;

public:
    ID3D12GraphicsCommandList* operator->() const { return _commandList.Get(); }
    operator ID3D12GraphicsCommandList*() const { return _commandList.Get(); }
    operator ID3D12CommandAllocator*() const { return _commandAllocator.Get(); }

public:
    void Initialize(CommandType type, std::wstring_view resourceName);
    void ExecuteCommand(CommandQueueType type = CommandQueueType::GRAPHICS_QUEUE);
    void Reset();

public:
    ComPtr<ID3D12CommandAllocator>    _commandAllocator;
    ComPtr<ID3D12GraphicsCommandList> _commandList;
    std::wstring                      _resourceName;
};