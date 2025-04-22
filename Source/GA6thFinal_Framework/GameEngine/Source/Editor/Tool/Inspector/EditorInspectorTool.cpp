#include "pch.h"
#include "EditorInspectorTool.h"

EditorInspectorTool::EditorInspectorTool()
{
    SetLabel("Inspector");
    SetDockLayout(DockLayout::RIGHT);
}

EditorInspectorTool::~EditorInspectorTool()
{
}

void  EditorInspectorTool::OnStartGui()
{
}

void  EditorInspectorTool::OnPreFrame()
{
}

void  EditorInspectorTool::OnFrame()
{
    if (false == _focusedObject.expired())
    {
        auto spFocusedObject = _focusedObject.lock();
        spFocusedObject->OnInspectorStay();
    }

    ImGui::Button("EventA");
    if (ImGui::BeginDragDropSource())
    {
        int data = 1;
        ImGui::SetDragDropPayload("EventA", &data, sizeof(int));
        ImGui::Text("EventA");
        ImGui::EndDragDropSource();
    }

    ImGui::Button("EventB");
    if (ImGui::BeginDragDropSource())
    {
        int data = 2;
        ImGui::SetDragDropPayload("EventB", &data, sizeof(int));
        ImGui::Text("EventB");
        ImGui::EndDragDropSource();
    }

    int data;
    ImGui::Button("Target");
    if (ImGuiHelper::DragDrop::RecieveItemDragDropEvent<int>("EventA", &data))
    {
        UmEngineLogger.Log(LogLevel::LEVEL_DEBUG, "EventA received");
    }
    if (ImGuiHelper::DragDrop::RecieveItemDragDropEvent<int>("EventB", &data))
    {
        UmEngineLogger.Log(LogLevel::LEVEL_DEBUG, "EventB received");
    }
    {
        const char* eventID = DragDropAsset::KEY;
        DragDropAsset::Data data;
        if (ImGuiHelper::DragDrop::RecieveItemDragDropEvent<DragDropAsset::Data>(eventID, &data))
        {
            UmEngineLogger.Log(LogLevel::LEVEL_DEBUG, data.context.lock()->GetPath().string().c_str());
        }
    }
    
}

void  EditorInspectorTool::OnPostFrame()
{
}

void EditorInspectorTool::OnFocus()
{
}