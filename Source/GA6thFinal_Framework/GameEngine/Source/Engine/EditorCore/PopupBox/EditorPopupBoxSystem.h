#pragma once
#include "Engine/EditorCore/PopupBox/EditorPopupBox.h"

class EditorPopupBox;

class EditorPopupBoxSystem
{
public:
    EditorPopupBoxSystem();
    ~EditorPopupBoxSystem();

public:
    void OnDrawGui();

    void OpenPopupBox(const std::string& name, std::function<void()> content);

    bool IsExistPopupBox(const std::string& name);

    bool IsPopupBoxOpened(const std::string& name);

    bool IsEmpty();

private:
    void Pop();

private:
    std::unordered_map<std::string, EditorPopupBox*> _popupBoxTable;
    std::deque<EditorPopupBox*>                      _popupBoxQueue;
};