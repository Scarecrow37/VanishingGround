#include "pch.h"

HierarchyFindTool::HierarchyFindTool() 
    : 
    _tagFilter(STR_NULL)
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
    _editorHierarchyTool = _ownerDockWindow->GetGui<EditorHierarchyTool>();
}

void HierarchyFindTool::OnEndGui() {}

void HierarchyFindTool::OnPreFrameBegin() {}

void HierarchyFindTool::OnPostFrameBegin() {}

void HierarchyFindTool::OnFrameRender()
{
    DrawFinder();
    DrawFindList();
}

void HierarchyFindTool::OnFrameClipped()
{

}

void HierarchyFindTool::OnFrameEnd() {}

void HierarchyFindTool::OnFrameFocusEnter()
{
    FindWithTagFilter();
}

void HierarchyFindTool::OnFrameFocusStay() {}

void HierarchyFindTool::OnFrameFocusExit()
{
    FindWithTagFilter();
}

void HierarchyFindTool::OnFramePopupOpened() {}

void HierarchyFindTool::DrawFinder()
{
    if (ImGui::Button("Tag filter"))
    {
        ImGui::OpenPopup("Find popup");
    }
    ImGui::SameLine();
    if (ImGui::Button("Reset"))
    {
        _tagFilter = STR_NULL;
    }
    _nameFilter.Draw("Search");

    if (ImGui::BeginPopup("Find popup"))
    {
        ImGui::InputText("##FindNameInput", &_tagFilter);
        if (ImGui::IsKeyReleased(ImGuiKey_Enter) || ImGui::Button("Find"))
        {
            FindWithTagFilter();
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
    if (_tagFilter != STR_NULL)
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
                        std::string_view name = object->ToString();
                        if (_nameFilter.PassFilter(name.data()))
                        {
                            if (ImGui::Selectable(name.data()))
                            {
                                EditorHierarchyTool::SetFocusObject(object);
                                EditorInspectorTool::SetFocusObject(object);
                            }
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
            std::erase_if(_findList, [](std::weak_ptr<GameObject>& obj) { return obj.expired(); });
        }
    }
    else
    {
        for (auto& object : ESceneManager::Engine::GetRuntimeObjects())
        {
            if (object && object->IsValid())
            {
                std::string_view name = object->ToString();
                if (false == name.empty())
                {
                    if (_nameFilter.PassFilter(name.data()))
                    {
                        if (ImGui::Selectable(name.data()))
                        {
                            EditorHierarchyTool::SetFocusObject(object);
                            EditorInspectorTool::SetFocusObject(object);
                            _editorHierarchyTool->SetFocusFrame();
                            _editorHierarchyTool->OpenFocusObjectTree();
                        }
                    }
                }
            }
        }
    }
    ImGui::Separator();
}

void HierarchyFindTool::FindWithTagFilter() 
{
    if (_tagFilter != STR_NULL)
    {
        _findList = GameObject::FindGameObjectsWithTag(_tagFilter);
    }
}
