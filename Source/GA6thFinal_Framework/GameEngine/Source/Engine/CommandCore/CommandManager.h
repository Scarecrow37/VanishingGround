#pragma once
#include "UmCommand.h"

class UmCommand;

class ECommandManager
{
    using CommandQueue = std::deque<std::shared_ptr<UmCommand>>;

public:
    template <typename T, typename... Args>
    void Do(Args... args)
    {
        static_assert(std::is_base_of<UmCommand, T>::value, "T is not based ICommand");
        std::shared_ptr<UmCommand> ptr = std::make_shared<T>(args...);
        ptr->Execute();
        _undoStack.push_back(ptr);
        _redoStack.clear();
    }

    void Undo();
    void Undo(UINT cnt);
    bool Undo(CommandQueue::const_iterator itr);

    void Redo();
    void Redo(UINT cnt);
    bool Redo(CommandQueue::const_iterator itr);

    void Clear();

public:
    inline const auto& GetCommandFromUndoStack(int index) const { return _undoStack[index]; }
    inline const auto& GetCommandFromRedoStack(int index) const { return _redoStack[index]; }

    inline int GetUndoStackSize() const { return static_cast<int>(_undoStack.size()); }
    inline int GetRedoStackSize() const { return static_cast<int>(_redoStack.size()); }

    inline const auto UndoStackBegin() const { return _undoStack.begin(); }
    inline const auto UndoStackEnd() const { return _undoStack.end(); }
    inline const auto RedoStackBegin() const { return _redoStack.begin(); }
    inline const auto RedoStackEnd() const { return _redoStack.end(); }

private:
    void ClampCommandStack();
    
private:
    CommandQueue _undoStack;
    CommandQueue _redoStack;

    size_t _maxCommandSize = 100;
};
