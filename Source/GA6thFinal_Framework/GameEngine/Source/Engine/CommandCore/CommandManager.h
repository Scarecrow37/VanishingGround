#pragma once
#include "UmCommand.h"

class UmCommand;

class ECommandManager
{
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

    void Redo();
    void Redo(UINT cnt);

    void Clear();

public:
    inline const auto UndoStackBegin() const { return _undoStack.begin(); }
    inline const auto UndoStackEnd() const { return _undoStack.end(); }
    inline const auto RedoStackBegin() const { return _redoStack.begin(); }
    inline const auto RedoStackEnd() const { return _redoStack.end(); }

private:
    void ClampCommandStack();
    
private:
    std::deque<std::shared_ptr<UmCommand>> _undoStack;
    std::deque<std::shared_ptr<UmCommand>> _redoStack;

    size_t _maxCommandSize = 100;
};
