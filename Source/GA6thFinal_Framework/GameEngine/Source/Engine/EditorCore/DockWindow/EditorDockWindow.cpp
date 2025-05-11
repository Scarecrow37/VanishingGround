#include "pch.h"
#include "EditorDockWindow.h"

EditorDockWindow::EditorDockWindow() 
{
}

EditorDockWindow::~EditorDockWindow() 
{
    for (auto& editor : _editorToolList)
    {
        if (nullptr != editor)
        {
            delete editor;
            editor = nullptr;
        }
    }
    _editorToolList.clear();
    _editorToolTable.clear();
}


void EditorDockWindow::OnTickGui() 
{
    for (auto& editor : _editorToolList)
    {
        if (nullptr != editor)
        {
            editor->OnTickGui();
        }
    }
}

void EditorDockWindow::OnStartGui() 
{
    for (auto& editor : _editorToolList)
    {
        if (nullptr != editor)
        {
            editor->OnStartGui();
        }
    }
}

void EditorDockWindow::OnEndGui() 
{
    for (auto& editor : _editorToolList)
    {
        if (nullptr != editor)
        {
            editor->OnEndGui();
        }
    }
}

void EditorDockWindow::OnPreFrame() 
{
    UpdateFlag();
    PushDockStyle();
}

void EditorDockWindow::OnFrame()
{
    SubmitDockSpace();
    PopDockStyle();
}

void EditorDockWindow::OnPostFrame()
{
    for (auto& editor : _editorToolList)
    {
        if (nullptr != editor)
        {
            editor->OnDrawGui();
        }
    }
}

void EditorDockWindow::UpdateFlag()
{
}

void EditorDockWindow::SubmitDockSpace() 
{
    //////////////////////////////////////////
    // Sumit the DockSpace
    //////////////////////////////////////////
    ImGuiIO&    io          = ImGui::GetIO();
    ImGuiStyle& style       = ImGui::GetStyle();
    float       minWinSizeX = style.WindowMinSize.x;
    style.WindowMinSize.x   = 370.0f;
    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
    {
        auto& windowClass = GetWindowClass();
        _dockSplitMainID = windowClass.ClassId;
        InitDockLayout();
        ImGui::DockSpace(windowClass.ClassId, ImVec2(0.0f, 0.0f), _imGuiDockFlags, &windowClass);
    }
    style.WindowMinSize.x = minWinSizeX;
}

void EditorDockWindow::InitDockLayout()
{
    bool useDockBuild = (_optionFlags & DOCKWINDOW_FLAGS_USE_DOCKBUILD) != 0;
    ImGuiDockNode* dockNode = ImGui::DockBuilderGetNode(_dockSplitMainID);

    if (true == useDockBuild && NULL == dockNode)
    {
        ImGui::DockBuilderRemoveNode(_dockSplitMainID);
        ImGui::DockBuilderAddNode(_dockSplitMainID, _imGuiDockFlags); // 새로 추가

        ImGuiID dock_main_id = _dockSplitMainID;

        for (auto& [direction, ratio] : _dockSplitLayoutID)
        {
            ImGuiID id;
            id = ImGui::DockBuilderSplitNode(dock_main_id, direction, ratio, NULL, &dock_main_id);
            _dockSplitIDTable[direction] = id;
        }
        _dockSplitIDTable[ImGuiDir_None] = dock_main_id;
        for (auto& [key, tool] : _editorToolTable)
        {
            if (nullptr != tool)
            {
                const char* label     = tool->GetLabel().c_str();
                ImGuiDir    direction = tool->GetDockLayout();
                ImGuiID     splitID   = _dockSplitIDTable[direction];
                ImGui::DockBuilderDockWindow(label, splitID);
            }
        }
        ImGui::DockBuilderFinish(_dockSplitMainID);
    }
}

void EditorDockWindow::PushDockStyle() 
{
    if (_optionFlags & DOCKWINDOW_FLAGS_FULLSCREEN)
    {
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    }
    if (_optionFlags & DOCKWINDOW_FLAGS_PADDING)
    {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    }
}

void EditorDockWindow::PopDockStyle() 
{
    if (_optionFlags & DOCKWINDOW_FLAGS_FULLSCREEN)
    {
        ImGui::PopStyleVar(2);
    }
    if (_optionFlags & DOCKWINDOW_FLAGS_PADDING)
    {
        ImGui::PopStyleVar();
    }
}

void EditorDockWindow::CreateDockLayoutNode(ImGuiDir direction, float ratio)
{
    auto itr = _dockSplitIDTable.find(direction);
    if (_dockSplitIDTable.end() != itr)
    {
        /* 도킹 레이아웃 분할 노드는 방향당 한 번만 생성 가능합니다. */
        return;
    }
    if (ratio < 0.0f || ratio > 1.0f)
    {
        /* 도킹 레이아웃 분할 비율은 [0 ~ 1] 사이의 값 입니다. */
        ASSERT(false, L"Dock ratio must be between 0.0f and 1.0f");
        ImClamp(ratio, 0.0f, 1.0f);
    }
    _dockSplitLayoutID.push_back({direction, ratio});
    _dockSplitIDTable[direction] = 0;

    _optionFlags |= DOCKWINDOW_FLAGS_USE_DOCKBUILD;
}
