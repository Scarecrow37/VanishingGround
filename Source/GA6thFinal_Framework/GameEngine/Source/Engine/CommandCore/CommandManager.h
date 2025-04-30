#pragma once
#include "UmCommand.h"

class UmCommand;

class CommandManager : public IAppModule
{
public:
// IAppModule을(를) 통해 상속됨
    void PreInitialize() override;
    void ModuleInitialize() override;
    void PreUnInitialize() override;
    void ModuleUnInitialize() override;

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

    void Redo();

private:
    void ClampCommandStack();
    
private:
    std::deque<std::shared_ptr<UmCommand>> _undoStack;
    std::deque<std::shared_ptr<UmCommand>> _redoStack;

    size_t _maxCommandSize = 100;
};
