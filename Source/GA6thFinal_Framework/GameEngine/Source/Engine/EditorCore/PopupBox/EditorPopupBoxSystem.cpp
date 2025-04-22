#include "pch.h"
#include "EditorPopupBoxSystem.h"

EditorPopupBoxSystem::EditorPopupBoxSystem() {}

EditorPopupBoxSystem::~EditorPopupBoxSystem() 
{
    for (auto& popupBox : _popupBoxStack)
    {
        delete popupBox;
    }
    _popupBoxStack.clear();
    _popupBoxTable.clear();
}

void EditorPopupBoxSystem::OpenPopupBox(const std::string& name, std::function<void()> content)
{
    auto itr = _popupBoxTable.find(name);
    if (itr != _popupBoxTable.end())
    {
        return;
    }
    _popupBoxTable[name] = new EditorPopupBox(name, content);
    _popupBoxStack.push_back(_popupBoxTable[name]);
}

void EditorPopupBoxSystem::OnDrawGui()
{
    if (_popupBoxStack.empty())
    {
        return;
    }
    static EditorPopupBox* popupBox = nullptr;

    if (nullptr == popupBox)
    {
        popupBox = _popupBoxStack.back();
        auto& name = popupBox->GetName();
        ImGui::OpenPopup(name.c_str());
    }

    if (nullptr != popupBox)
    {
        auto& name   = popupBox->GetName();
        bool  isOpen = popupBox->IsOpen();

        if (false == popupBox->IsNull())
        {
            if (ImGui::BeginPopupModal(name.c_str(), &isOpen, ImGuiWindowFlags_AlwaysAutoResize))
            {
                popupBox->GetContent()();
                ImGui::EndPopup();
            }
            if (false == isOpen)
            {
                Pop();
                popupBox = nullptr;
            }
        }
    }
}

void EditorPopupBoxSystem::Pop()
{
    auto* popupBox = _popupBoxStack.back();
    auto& name     = popupBox->GetName();
    _popupBoxTable.erase(name);
    _popupBoxStack.pop_back();

    delete popupBox;
}
