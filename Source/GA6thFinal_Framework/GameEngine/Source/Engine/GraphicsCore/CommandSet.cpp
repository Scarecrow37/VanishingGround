#include "pch.h"
#include "CommandSet.h"

void CommandSet::Initialize(CommandType type, std::wstring_view resourceName)
{
    UmDevice.CreateCommandList(_commandAllocator, _commandList, type);
    _commandList->Reset(_commandAllocator.Get(), nullptr);

    _commandAllocator->SetName(resourceName.data());
    _commandList->SetName(resourceName.data());
}

void CommandSet::ExecuteCommand(CommandQueueType type)
{
    _commandList->Close();
    UmCommandController.ExecuteCommand(type, _commandList.Get());
}

void CommandSet::Reset()
{
    _commandAllocator->Reset();
    _commandList->Reset(_commandAllocator.Get(), nullptr);
}