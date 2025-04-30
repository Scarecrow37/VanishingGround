#include "pch.h"
#include "EditorPopupBoxSystem.h"

EditorPopupBoxSystem::EditorPopupBoxSystem() {}

EditorPopupBoxSystem::~EditorPopupBoxSystem() 
{
    for (auto& popupBox : _popupBoxQueue)
    {
        delete popupBox;
    }
    _popupBoxQueue.clear();
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
    _popupBoxQueue.push_back(_popupBoxTable[name]);
}

bool EditorPopupBoxSystem::IsExistPopupBox(const std::string& name)
{
    bool isExist = _popupBoxTable.find(name) != _popupBoxTable.end();
    return isExist;
}

bool EditorPopupBoxSystem::IsPopupBoxOpened(const std::string& name)
{
    auto itr = _popupBoxTable.find(name);
    if (itr != _popupBoxTable.end())
    {
        return itr->second->IsOpen();
    }
    return false;
}

bool EditorPopupBoxSystem::IsEmpty()
{
    bool isEmpty = _popupBoxQueue.empty();
    return isEmpty;
}

void EditorPopupBoxSystem::OnDrawGui()
{
    if (_popupBoxQueue.empty())
    {
        return;
    }
    static EditorPopupBox* popupBox = nullptr;

    if (nullptr == popupBox)
    {
        popupBox    = _popupBoxQueue.front();
        auto& name  = popupBox->GetName();
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
    auto* popupBox = _popupBoxQueue.front();
    auto& name     = popupBox->GetName();
    _popupBoxTable.erase(name);
    _popupBoxQueue.pop_front();

    delete popupBox;
}
