#include "pch.h"
#include "CommandSet.h"

void CommandSet::Initialize(CommandType type, std::wstring_view resourceName)
{
    Global::device->CreateCommandList(_commandAllocator, _commandList, type);
    _commandList->Reset(_commandAllocator.Get(), nullptr);

    _commandAllocator->SetName(resourceName.data());
    _commandList->SetName(resourceName.data());
}

void CommandSet::ExecuteCommand(CommandQueueType type)
{
    _commandList->Close();
    Global::commandController->ExecuteCommand(type, _commandList.Get());
}

void CommandSet::Reset()
{
    _commandAllocator->Reset();
    _commandList->Reset(_commandAllocator.Get(), nullptr);
}