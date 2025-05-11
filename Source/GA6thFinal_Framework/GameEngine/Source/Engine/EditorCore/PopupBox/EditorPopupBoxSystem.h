#pragma once
#include "EditorPopupBox.h"

class EditorPopupBox;

class EditorPopupBoxSystem : public IEditorCycle
{
public:
    EditorPopupBoxSystem();
    ~EditorPopupBoxSystem();

public:
    // IEditorCycle을(를) 통해 상속됨
    void OnTickGui() override;
    void OnStartGui() override;
    void OnDrawGui() override;
    void OnEndGui() override;

public:
    void OpenPopupBox(const std::string& name, std::function<void()> content);

    bool IsExistPopupBox(const std::string& name);

    bool IsPopupBoxOpened(const std::string& name);

    bool IsEmpty();

private:
    void PopFront();

private:
    EditorPopupBox* _currentPopupBox = nullptr; // 현재 팝업박스

    std::unordered_map<std::string, EditorPopupBox*> _popupBoxTable;
    std::deque<EditorPopupBox*>                      _popupBoxQueue;

    
};