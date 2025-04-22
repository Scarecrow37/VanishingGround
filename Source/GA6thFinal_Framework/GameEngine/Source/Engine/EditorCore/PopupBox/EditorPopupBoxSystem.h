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

private:
    void Pop();

private:
    std::unordered_map<std::string, EditorPopupBox*> _popupBoxTable;
    std::vector<EditorPopupBox*>                     _popupBoxStack;
};