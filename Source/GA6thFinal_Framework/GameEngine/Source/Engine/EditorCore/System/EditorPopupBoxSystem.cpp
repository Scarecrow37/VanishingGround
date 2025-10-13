#include "pch.h"
#include "EditorPopupBoxSystem.h"

EditorPopupBoxSystem::EditorPopupBoxSystem() {}

EditorPopupBoxSystem::~EditorPopupBoxSystem() 
{
    Clear();
}

void EditorPopupBoxSystem::OnTickGui() {}

void EditorPopupBoxSystem::OnStartGui() {}

void EditorPopupBoxSystem::OnDrawGui() 
{
    if (_popupBoxQueue.empty())
    {
        return;
    }

    // 현재 처리중인 팝업 박스가 없으면 큐의 앞에 있는 팝업 박스를 현재 팝업 박스로 설정
    if (nullptr == _currentPopupBox)
    {
        if (_currentPopupBox = &_popupBoxQueue.front())
        {
            const std::string& popupName = _currentPopupBox->GetName();
            ImGui::OpenPopup(popupName.c_str());
        }
    }

    if (_currentPopupBox)
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

void EditorPopupBoxSystem::Clear()
{
    _popupBoxQueue.clear();
    _popupBoxTable.clear();
    _currentPopupBox = nullptr;
}

EditorPopupBox* EditorPopupBoxSystem::OpenPopupBox(const std::string& name, std::function<void()> content)
{
    auto itr = _popupBoxTable.find(name);
    if (itr != _popupBoxTable.end())
    {
        return itr->second;
    }
    else
    {
        EditorPopupBox popup = EditorPopupBox(name, content);
        _popupBoxQueue.push_back(std::move(popup));
        _popupBoxTable[name] = &_popupBoxQueue.back();
    }
    return _popupBoxTable[name];
}

bool EditorPopupBoxSystem::IsExistPopupBox(const std::string& name) const
{
    return _popupBoxTable.contains(name);
}

bool EditorPopupBoxSystem::IsPopupBoxOpened(const std::string& name) const
{
    auto itr = _popupBoxTable.find(name);
    if (itr != _popupBoxTable.end())
    {
        if (nullptr != itr->second)
        {
            return itr->second->IsOpen();
        }
    }
    return false;
}

bool EditorPopupBoxSystem::IsEmpty() const
{
    return _popupBoxQueue.empty();
}

void EditorPopupBoxSystem::PopFront()
{
    if (IsEmpty())
    {
        return;
    }

    auto& popup = _popupBoxQueue.front();
    auto& name  = popup.GetName();
    _popupBoxTable.erase(name);
    _popupBoxQueue.pop_front();
}