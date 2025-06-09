#include "pch.h"

HierarchyFindTool::HierarchyFindTool() 
{
    SetLabel("Hierarchy Finder");
    SetDockLayout(ImGuiDir_Left);
}

HierarchyFindTool::~HierarchyFindTool() 
{

}

void HierarchyFindTool::OnTickGui() 
{

}

void HierarchyFindTool::OnStartGui() 
{
    _ownerDockWindow = GetOwnerDockWindow();
}

void HierarchyFindTool::OnEndGui() {}

void HierarchyFindTool::OnPreFrameBegin() {}

void HierarchyFindTool::OnPostFrameBegin() {}

void HierarchyFindTool::OnFrameRender() 
{
    DrawFinder();
    DrawFindList();
}

void HierarchyFindTool::OnFrameClipped() {}

void HierarchyFindTool::OnFrameEnd() {}

void HierarchyFindTool::OnFrameFocusEnter() {}

void HierarchyFindTool::OnFrameFocusStay() {}

void HierarchyFindTool::OnFrameFocusExit() {}

void HierarchyFindTool::OnFramePopupOpened() {}

void HierarchyFindTool::DrawFinder() 
{
    static FindType findType;
   
    if (ImGui::Button("Find with name"))
    {
        findType = FindType::NAME;
        ImGui::OpenPopup("Find popup");
    }
    ImGui::SameLine();
    if (ImGui::Button("Find with tag"))
    {
        findType = FindType::TAG;
        ImGui::OpenPopup("Find popup");
    }

    if (ImGui::BeginPopup("Find popup"))
    {
        static std::string findBuffer;
        ImGui::InputText("##FindNameInput", &findBuffer);
        if (ImGui::IsKeyReleased(ImGuiKey_Enter) || ImGui::Button("Find"))
        {
            switch (findType)
            {
            case HierarchyFindTool::FindType::NAME:
                _findList = GameObject::FindGameObjects(findBuffer);
                break;
            case HierarchyFindTool::FindType::TAG:
                _findList = GameObject::FindGameObjectsWithTag(findBuffer);
                break;
            default:
                break;
            }
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::IsKeyReleased(ImGuiKey_Escape) || ImGui::Button("Cancel"))
        {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

    ImGui::Separator();
}

void HierarchyFindTool::DrawFindList() 
{
    bool expired = false;
    for (auto& item : _findList)
    {
        if (false == item.expired())
        {
            auto object = item.lock();
            ImGui::PushID(object.get());
            {
                if (object->IsValid())
                {
                    if (ImGuiHelper::TreeStyleTextButton(object->ToString().data()))
                    {
                        EditorHierarchyTool::HierarchyFocusObjWeak = object;
                    }
                }
            }
            ImGui::PopID();
        }
        else
        {
            expired = true;
        }
    }

    if (expired)
    {
        std::erase_if(_findList, [](std::weak_ptr<GameObject>& obj) 
        { 
            return obj.expired();
        });
    }
    ImGui::Separator();
}
