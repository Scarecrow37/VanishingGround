#include "pch.h"
#include "CommandManager.h"

void ECommandManager::Undo()
{
    if (true == _undoStack.empty())
        return;

    auto& cmd = _undoStack.back();
    cmd->Undo();

    _redoStack.push_back(cmd);
    _undoStack.pop_back();

    ClampCommandStack();
}

void ECommandManager::Undo(UINT cnt)
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

void ECommandManager::Redo()
{
    if (true == _redoStack.empty())
        return;

    auto& cmd = _redoStack.back();
    cmd->Execute();

    _undoStack.push_back(cmd);
    _redoStack.pop_back();

    ClampCommandStack();
}

void ECommandManager::Redo(UINT cnt)
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

void ECommandManager::Clear() 
{
    _undoStack.clear();
    _redoStack.clear();
}

void ECommandManager::ClampCommandStack()
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
