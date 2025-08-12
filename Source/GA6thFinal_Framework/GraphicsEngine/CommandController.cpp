#include "pch.h"
#include "CommandController.h"

CommandController::~CommandController()
{
    for (int i = 0; i < COMMAND_QUEUE_END; ++i)
    {
        if (_fenceEvent[i])
        {
            CloseHandle(_fenceEvent[i]);
            _fenceEvent[i] = nullptr;
        }
    }
}

void CommandController::Initialize()
{
    auto device = Global::device->GetDevice();

    D3D12_COMMAND_QUEUE_DESC desc = {};
    desc.Flags                    = D3D12_COMMAND_QUEUE_FLAG_NONE;
    desc.Priority                 = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
    desc.NodeMask                 = 0;

    HRESULT hr = S_OK;

    desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    hr         = device->CreateCommandQueue(&desc, IID_PPV_ARGS(&_commandQueue[GRAPHICS_QUEUE]));
    FAILED_CHECK_MESSAGE(hr, L"CommandController::Initialize CreateCommandQueue Failed");

    desc.Type = D3D12_COMMAND_LIST_TYPE_COMPUTE;
    hr        = device->CreateCommandQueue(&desc, IID_PPV_ARGS(&_commandQueue[COMPUTE_QUEUE]));
    FAILED_CHECK_MESSAGE(hr, L"CommandController::Initialize CreateCommandQueue Failed");

    desc.Type = D3D12_COMMAND_LIST_TYPE_COPY;
    hr        = device->CreateCommandQueue(&desc, IID_PPV_ARGS(&_commandQueue[COPY_QUEUE]));
    FAILED_CHECK_MESSAGE(hr, L"CommandController::Initialize CreateCommandQueue Failed");

    for (int i = 0; i < COMMAND_QUEUE_END; ++i)
    {
        _fenceValue[i] = 0;
        hr = device->CreateFence(_fenceValue[i], D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&_fence[i]));
        FAILED_CHECK_MESSAGE(hr, L"CommandController::Initialize CreateFence Failed");

        _fenceEvent[i] = CreateEvent(nullptr, FALSE, FALSE, nullptr);
        GRAPHICS_ASSERT(nullptr != _fenceEvent[i], L"CommandController::Initialize CreateEvent Failed");
    }

    _commandQueue[GRAPHICS_QUEUE]->SetName(L"GraphicsCommandQueue");
    _commandQueue[COMPUTE_QUEUE]->SetName(L"ComputeCommandQueue");
    _commandQueue[COPY_QUEUE]->SetName(L"CopyCommandQueue");

    _fence[GRAPHICS_QUEUE]->SetName(L"GraphicsFence");
    _fence[COMPUTE_QUEUE]->SetName(L"ComputeFence");
    _fence[COPY_QUEUE]->SetName(L"CopyFence");
}

void CommandController::AddCommandSet(CommandType command, std::wstring_view resourceName, CommandSet& out)
{
    std::lock_guard<std::mutex> lock(_mutex);
    CommandSet commandSet;
    commandSet.Initialize(command, resourceName);
    _commandSets.emplace_back(std::move(commandSet));
    out = _commandSets.back();
}

void CommandController::WaitForCommandQueue(CommandQueueType type, UINT64 fenceValue)
{
    if (_fence[type]->GetCompletedValue() < fenceValue)
    {
        _fence[type]->SetEventOnCompletion(fenceValue, _fenceEvent[type]);
        ::WaitForSingleObject(_fenceEvent[type], INFINITE);
    }
}

void CommandController::WaitCommandQueue(CommandQueueType queue, CommandQueueType fence, UINT64 fenceValue)
{
    _commandQueue[queue]->Wait(_fence[fence].Get(), fenceValue);
}

void CommandController::ExecuteCommand(CommandQueueType type, ID3D12CommandList* commandList)
{
    _commandQueue[type]->ExecuteCommandLists(1, &commandList);
}

void CommandController::ResetCommand(CommandQueueType type)
{
    for (auto& commandSet : _commandSets)
    {
        commandSet.Reset();
    }
}

UINT64 CommandController::SignalCommandQueue(CommandQueueType type)
{
    std::lock_guard<std::mutex> lock(_mutex);
    _commandQueue[type]->Signal(_fence[type].Get(), ++_fenceValue[type]);
    return _fenceValue[type];
}

bool CommandController::IsCompleteCommandQueue(CommandQueueType queue, UINT64 fenceValue)
{
    if (_fence[queue]->GetCompletedValue() >= fenceValue)
    {
        return true;
    }
    return false;
}