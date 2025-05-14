#include "EditorCommandTool.h"
#include "pch.h"

EditorCommandTool::EditorCommandTool()
{
    SetLabel("Command");
    SetDockLayout(ImGuiDir_Right);
}

EditorCommandTool::~EditorCommandTool() {}

void EditorCommandTool::OnTickGui()
{
    auto& manager = UmCommandManager;

    bool ctrl      = ImGui::IsKeyDown(ImGuiKey::ImGuiKey_LeftCtrl);
    bool z         = ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_Z, false);
    bool y         = ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_Y, false);
    bool isKeyUndo = ctrl && z;
    bool isKeyRedo = ctrl && y;

    if (true == isKeyUndo)
    {
        manager.Undo();
    }
    if (true == isKeyRedo)
    {
        manager.Redo();
    }
}

void EditorCommandTool::OnStartGui() {}

void EditorCommandTool::OnEndGui() {}

void EditorCommandTool::OnPreFrameBegin() {}

void EditorCommandTool::OnPostFrameBegin() {}

void EditorCommandTool::OnFrameRender()
{
    auto& manager = UmCommandManager;
    if (ImGui::Button("Clear"))
    {
        manager.Clear();
    }
    ImGui::Separator();

    int         index = 0;
    std::string icon  = EditorIcon::UnicodeToUTF8(0xf044);

    ImGuiChildFlags flags = ImGuiChildFlags_Border;
    ImGui::BeginChild("FolderHierarchyFrame", ImVec2(0, 0), flags);
    {
        auto undoBegin = manager.UndoStackBegin();
        auto undoEnd   = manager.UndoStackEnd();
        for (auto itr = undoBegin; itr != undoEnd; ++itr, ++index)
        {
            auto& command = *itr;
            if (nullptr != command)
            {
                auto name = icon + " " + command->GetName();

                ImVec4 defaultColor = GetSelectableColor(index, _tableDefaultColor);
                ImVec4 hoveredColor = GetSelectableColor(index, _tableHoveredColor);
                ImGui::PushStyleColor(ImGuiCol_Header, defaultColor);
                ImGui::PushStyleColor(ImGuiCol_HeaderHovered, hoveredColor);
                ImGui::Selectable(name.c_str(), true);
                ImGui::PopStyleColor(2);
            }
        }
        auto redoBegin = manager.RedoStackBegin();
        auto redoEnd   = manager.RedoStackEnd();

        for (auto itr = redoBegin; itr != redoEnd; ++itr, ++index)
        {
            auto& command = *itr;
            if (nullptr != command)
            {
                auto name = icon + " " + command->GetName();

                ImVec4 defaultColor = GetSelectableColor(index, _tableDefaultColor);
                defaultColor.w      = 0.2f;
                ImVec4 hoveredColor = GetSelectableColor(index, _tableHoveredColor);
                hoveredColor.w      = 0.2f;
                ImGui::PushStyleColor(ImGuiCol_Header, defaultColor);
                ImGui::PushStyleColor(ImGuiCol_HeaderHovered, hoveredColor);
                ImGui::Selectable(name.c_str(), true);
                ImGui::PopStyleColor(2);
            }
        }
    }
    ImGui::EndChild();
}

void EditorCommandTool::OnFrameClipped() {}

void EditorCommandTool::OnFrameEnd() {}

void EditorCommandTool::OnFrameFocusEnter() {}

void EditorCommandTool::OnFrameFocusStay() {}

void EditorCommandTool::OnFrameFocusExit() {}

void EditorCommandTool::OnFramePopupOpened() {}

ImVec4 EditorCommandTool::GetSelectableColor(int index, ImVec4 color)
{
    // 빼거나 더할 값
    ImVec4 blend = ImVec4(0.1f, 0.1f, 0.1f, 0.0f);
    // 짝수 인덱스는 어둡게, 홀수 인덱스는 밝게
    ImVec4 out = index % 2 == 0 ? color + blend : color - blend;
    return out;
}
