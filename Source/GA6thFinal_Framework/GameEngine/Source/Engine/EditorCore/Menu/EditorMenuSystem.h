#pragma once
#include "EditorMenuBar.h"

class EditorMenuSystem
    : public IEditorCycle
{
public:
    EditorMenuSystem();
    ~EditorMenuSystem();

public:
    // IEditorCycle을(를) 통해 상속됨
    void OnTickGui() override;
    void OnStartGui() override;
    void OnDrawGui() override;
    void OnEndGui() override;

public:
    template <typename T> T* RegisterGui();
    template <typename T> T* GetGui();

private:

};

template <typename T>
inline T* EditorMenuSystem::RegisterGui()
{
    return nullptr;
}

template <typename T>
inline T* EditorMenuSystem::GetGui()
{
    return nullptr;
}
