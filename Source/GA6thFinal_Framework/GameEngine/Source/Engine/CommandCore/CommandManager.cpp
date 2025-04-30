#include "pch.h"
#include "CommandManager.h"

void CommandManager::PreInitialize() 
{
    _undoStack.resize(_maxCommandSize);
    _redoStack.resize(_maxCommandSize);
}

void CommandManager::ModuleInitialize() 
{

}

void CommandManager::PreUnInitialize() 
{
}

void CommandManager::ModuleUnInitialize() 
{
    _undoStack.clear();
    _redoStack.clear();
}

void CommandManager::Undo()
{
    if (true == _undoStack.empty())
        return;

    auto cmd = _undoStack.back();
    cmd->Undo();

    _undoStack.pop_back();
    _redoStack.push_back(cmd);

    ClampCommandStack();
}

void CommandManager::Undo(UINT cnt) 
{
    for (UINT i = 0; i < cnt; ++i)
    {
        if (true == _undoStack.empty())
        {
            break;
        }
        else
        {
            Undo();
        }
    }
}

void CommandManager::Redo() 
{
    if (true == _redoStack.empty())
        return;

    auto cmd = _redoStack.back();
    cmd->Execute();

    _redoStack.pop_back();
    _undoStack.push_back(cmd);

    ClampCommandStack();
}

void CommandManager::Redo(UINT cnt) 
{
    for (UINT i = 0; i < cnt; ++i)
    {
        if (true == _redoStack.empty())
        {
            break;
        }
        else
        {
            Redo();
        }
    }
}

void CommandManager::ClampCommandStack() 
{
    if (_undoStack.size() > _maxCommandSize)
    {
        _undoStack.pop_front();
    }
    if (_redoStack.size() > _maxCommandSize)
    {
        _redoStack.pop_front();
    }
}
