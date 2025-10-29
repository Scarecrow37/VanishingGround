#include "pch.h"
#include "CommandSet.h"

void CommandSet::Initialize(CommandType type, std::wstring_view resourceName)
{
    Global::device->CreateCommandList(_commandAllocator, _commandList, type);
    _commandList->Reset(_commandAllocator.Get(), nullptr);

    _resourceName = resourceName;
    _commandList->SetName(_resourceName.c_str());
    _commandAllocator->SetName(_resourceName.c_str());
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