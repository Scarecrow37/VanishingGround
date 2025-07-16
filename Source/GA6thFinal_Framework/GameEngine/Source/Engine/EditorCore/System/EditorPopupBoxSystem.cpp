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

void EditorPopupBoxSystem::OnTickGui() {}

void EditorPopupBoxSystem::OnStartGui() {}

void EditorPopupBoxSystem::OnDrawGui() 
{
    if (_popupBoxQueue.empty())
    {
        return;
    }

    if (nullptr == _currentPopupBox)
    {
        _currentPopupBox = _popupBoxQueue.front();
        auto& popupName  = _currentPopupBox->GetName();
        ImGui::OpenPopup(popupName.c_str());
    }

    if (nullptr != _currentPopupBox)
    {
        auto& popupName = _currentPopupBox->GetName();
        int   flags     = _currentPopupBox->GetFlags();
        bool  isOpen    = _currentPopupBox->IsOpen();

        if (false == _currentPopupBox->IsNull())
        {
            //ImGuiWindowFlags_AlwaysAutoResize 플래그가 없으면 사이즈 조절
            if (false == (flags & ImGuiWindowFlags_AlwaysAutoResize))
            {
                const ImVec2& size = _currentPopupBox->GetSize();
                ImGui::SetNextWindowSize(size, ImGuiCond_FirstUseEver);
            }
            if (ImGui::BeginPopupModal(popupName.c_str(), &isOpen, flags))
            {
                _currentPopupBox->GetContent()();
                ImGui::EndPopup();
            }
            if (false == isOpen)
            {
                PopFront();
                _currentPopupBox = nullptr;
            }
        }
    }
}

void EditorPopupBoxSystem::OnEndGui() 
{
}

EditorPopupBox* EditorPopupBoxSystem::OpenPopupBox(const std::string& name, std::function<void()> content)
{
    EditorPopupBox* instance = nullptr;
    auto itr = _popupBoxTable.find(name);
    if (itr != _popupBoxTable.end())
    {
        instance = itr->second;
    }
    else
    {
         instance = new EditorPopupBox(name, content);
        _popupBoxTable[name] = instance;
        _popupBoxQueue.push_back(_popupBoxTable[name]);
    }
    return instance;
}

bool EditorPopupBoxSystem::IsExistPopupBox(const std::string& name) const
{
    bool isExist = _popupBoxTable.find(name) != _popupBoxTable.end();
    return isExist;
}

bool EditorPopupBoxSystem::IsPopupBoxOpened(const std::string& name) const
{
    auto itr = _popupBoxTable.find(name);
    if (itr != _popupBoxTable.end())
    {
        return itr->second->IsOpen();
    }
    return false;
}

bool EditorPopupBoxSystem::IsEmpty() const
{
    bool isEmpty = _popupBoxQueue.empty();
    return isEmpty;
}

void EditorPopupBoxSystem::PopFront()
{
    auto* popupBox = _popupBoxQueue.front();
    auto& name     = popupBox->GetName();
    _popupBoxTable.erase(name);
    _popupBoxQueue.pop_front();

    delete popupBox;
}