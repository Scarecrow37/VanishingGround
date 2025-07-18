#pragma once
#include "UmCommand.h"

class ECommandManager
{
    using CommandQueue = std::deque<std::shared_ptr<UmCommand>>;

public:
    template <typename T, typename... Args>
    void Do(Args... args)
    {
        static_assert(std::is_base_of<UmCommand, T>::value, "T is not based ICommand");
        std::shared_ptr<UmCommand> ptr = std::make_shared<T>(args...);
        bool result = ptr->Execute();
        if (true == result && false == Global::IsPlay())
        {
            _undoStack.push_back(ptr);
            _redoStack.clear();
        }
    }

    void Undo();
    void Undo(size_t cnt);
    bool Undo(CommandQueue::const_iterator itr);

    void Redo();
    void Redo(size_t cnt);
    bool Redo(CommandQueue::const_iterator itr);

    void Clear();

    void SetMaxCommandSize(size_t size);

    inline const std::shared_ptr<UmCommand>& GetCommandFromUndoStack(size_t index) const { return _undoStack[index]; }
    inline const std::shared_ptr<UmCommand>& GetCommandFromRedoStack(size_t index) const { return _redoStack[index]; }

    inline size_t GetUndoStackSize() const { return _undoStack.size(); }
    inline size_t GetRedoStackSize() const { return _redoStack.size(); }

    inline const auto UndoStackBegin() const { return _undoStack.begin(); }
    inline const auto UndoStackEnd() const { return _undoStack.end(); }
    inline const auto RedoStackBegin() const { return _redoStack.begin(); }
    inline const auto RedoStackEnd() const { return _redoStack.end(); }

    inline bool IsEmpty() const { return IsUndoEmpty() && IsRedoEmpty(); }
    inline bool IsUndoEmpty() const { return _undoStack.empty(); }
    inline bool IsRedoEmpty() const { return _redoStack.empty(); }

private:
    void ClampCommandStack();
    
private:
    CommandQueue _undoStack;
    CommandQueue _redoStack;

    size_t _maxCommandSize = 100;
};
