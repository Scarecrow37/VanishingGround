#include "pch.h"
#include "EditorDockWindow.h"

void EditorDockWindow::OnTickGui() 
{
}

void EditorDockWindow::OnStartGui() 
{
    _dockSplitMainID = ImHashStr(GetLabel().c_str());

    _imGuiWindowClass.ClassId = _dockSplitMainID;
    _imGuiWindowClass.DockingAllowUnclassed = false;
    _imGuiWindowClass.DockingAlwaysTabBar   = true;
}

void EditorDockWindow::OnDrawGui() 
{
    UpdateFlag(); 
    PushDockStyle(); 

    ImGui::Begin(GetLabel().c_str(), nullptr, _imGuiWindowFlags);

    SubmitDockSpace(); 
    PopDockStyle();    

    ImGui::End();
}

void EditorDockWindow::OnEndGui() 
{
}

void EditorDockWindow::InitDockLayout()
{
    if (NULL == ImGui::DockBuilderGetNode(_dockSplitMainID))
    {
        ImGui::DockBuilderRemoveNode(_dockSplitMainID);
        ImGui::DockBuilderAddNode(_dockSplitMainID, _userImGuiDockFlags); // 새로 추가

        ImGuiID dock_main_id = _dockSplitMainID; // This variable will track the document node, however we are not using

        for (auto& [id, direction, ratio] : _dockSplitLayoutID)
        {
            id = ImGui::DockBuilderSplitNode(dock_main_id, direction, ratio, NULL, &dock_main_id);
        }

        ImGui::DockBuilderFinish(_dockSplitMainID);
    }
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
        InitDockLayout();
        ImGui::DockSpace(_dockSplitMainID, ImVec2(0.0f, 0.0f), _imGuiDockFlags);
    }
    style.WindowMinSize.x = minWinSizeX;
}

void EditorDockWindow::UpdateFlag() 
{
    _imGuiWindowFlags = _userImGuiWindowFlags;
    if (_flags & DOCKWINDOW_FLAGS_FULLSCREEN)
    {
        _imGuiWindowFlags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
                            ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus |
                            ImGuiWindowFlags_NoNavFocus;
    }
    if (_imGuiDockFlags & ImGuiDockNodeFlags_PassthruCentralNode)
        _imGuiWindowFlags |= ImGuiWindowFlags_NoBackground;
}

void EditorDockWindow::PushDockStyle() 
{
    if (_flags & DOCKWINDOW_FLAGS_FULLSCREEN)
    {
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    }
    if (_flags & DOCKWINDOW_FLAGS_PADDING)
    {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    }
}

void EditorDockWindow::PopDockStyle() 
{
    if (_flags & DOCKWINDOW_FLAGS_FULLSCREEN)
    {
        ImGui::PopStyleVar(2);
    }
    if (_flags & DOCKWINDOW_FLAGS_PADDING)
    {
        ImGui::PopStyleVar();
    }
}

void EditorDockWindow::SetOptionFlags(EditorDockWindowFlags flags) 
{
    _flags = flags;
}

void EditorDockWindow::AddDockLayoutNode(ImGuiDir direction, float ratio)
{
    if (ratio < 0.0f || ratio > 1.0f)
    {
        /* 도킹 레이아웃 분할 비율은 [0 ~ 1] 사이의 값 입니다. */
        ASSERT(false, L"Dock ratio must be between 0.0f and 1.0f");
    }
    else
    {
        _dockSplitLayoutID.push_back({0, direction, ratio});
    }
}
