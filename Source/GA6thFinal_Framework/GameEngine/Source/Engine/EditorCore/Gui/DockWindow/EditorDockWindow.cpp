#include "pch.h"
#include "EditorDockWindow.h"

EditorDockWindow::EditorDockWindow() 
: _needBuildDockLayout(true)
, _isBuildingDockLayout(false)
, _dockWindowOptionFlags(0)
, _dockSplitMainID(0)
, _imGuiDockFlags(0)
{}

EditorDockWindow::~EditorDockWindow() 
{
    for (auto gui : _editorGuiList)
    {
        if (gui)
        {
            delete gui;
        }
    }
    _editorGuiList.clear();
    _editorGuiClassTable.clear();
    _editorToolTable.clear();
    _editorMenuTable.clear();
    _dockWindowTable.clear();
    _dockSplitLayoutID.clear();
    _dockSplitIDTable.clear();
}

void EditorDockWindow::OnTickGui() 
{
    for (auto& editor : _editorGuiList)
    {
        if (nullptr != editor)
        {
            editor->OnTickGui();
        }
    }
}

void EditorDockWindow::OnStartGui() 
{
    AddEditorToolFlags(EditorTool::EDITORTOOL_FLAGS_IS_DOCKWINDOW);
    for (auto& editor : _editorGuiList)
    {
        if (nullptr != editor)
        {
            editor->OnStartGui();
        }
    }
}

void EditorDockWindow::OnEndGui() 
{
    for (auto& editor : _editorGuiList)
    {
        if (nullptr != editor)
        {
            editor->OnEndGui();
        }
    }
}

void EditorDockWindow::OnPreFrameBegin() 
{
    UpdateFlag();
    PushDockStyle();
}

void EditorDockWindow::OnPostFrameBegin()
{
    bool isBeginDockBuild = BeginBuildDockLayout();

    SubmitDockSpace();
    PopDockStyle();

    // [Gui] Begin - End
    for (auto& editor : _editorGuiList)
    {
        if (editor && editor->IsVisible())
        {
            editor->OnDrawGui();
        }
    }

    if (true == isBeginDockBuild)
    {
        EndBuildDockLayout();
    }
}

void EditorDockWindow::OnFrameRender() 
{
}

void EditorDockWindow::OnFrameEnd()
{
}

void EditorDockWindow::OnFrameFocusStay() 
{
}

void EditorDockWindow::ProcessFocusFrame()
{
    _isFrameFocused = false;
    for (auto& [key, editor] : _editorToolTable)
    {
        if (nullptr != editor)
        {
            bool isFocused = editor->IsFocusFrame();
            if (true == isFocused)
            {
                _isFrameFocused = true;
            }
        }
    }
}

bool EditorDockWindow::RegisterChildDockWindow(EditorDockWindow* childDockWindow)
{
    if (nullptr == childDockWindow)
    {
        return false;
    }

    _editorGuiList.push_back(childDockWindow);
    childDockWindow->SetOwnerDockWindow(this);
    _dockWindowTable[childDockWindow->GetLabel()] = childDockWindow;
    return true;
}

void EditorDockWindow::UpdateFlag()
{
    _imGuiDockFlags = _userImGuiDockFlags;
    if (false == HasEditorToolFlags(EditorTool::EDITORTOOL_FLAGS_ALWAYS_FRAME))
    {
        AddEditorToolFlags(EditorTool::EDITORTOOL_FLAGS_ALWAYS_FRAME);
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
        auto& windowClass = GetWindowClass();
        ImGui::DockSpace(_dockSplitMainID, ImVec2(0.0f, 0.0f), _imGuiDockFlags, &windowClass);
    }
    style.WindowMinSize.x = minWinSizeX;
}

void EditorDockWindow::RequestBuildDockLayout() 
{
    _needBuildDockLayout = true;
}

bool EditorDockWindow::IsBuildingDockLayout() const
{
    return _isBuildingDockLayout;
}

bool EditorDockWindow::BeginBuildDockLayout()
{
    if (true == _needBuildDockLayout /* && nullptr == dockNode*/)
    {
        _needBuildDockLayout = false;
        _isBuildingDockLayout = true;
        _dockSplitMainID     = GetWindowClass().ClassId;
        // Main으로부터 파생된 ID 전부 제거
        ImGui::DockBuilderRemoveNode(_dockSplitMainID);
        // 메인 ID 생성
        ImGui::DockBuilderAddNode(_dockSplitMainID, _imGuiDockFlags); 
        ImGuiID dock_main_id = _dockSplitMainID;

        // 메인 ID로부터 파생되는 분할 Layout ID 생성
        for (auto& [direction, ratio] : _dockSplitLayoutID)
        {
            ImGuiID id;
            id = ImGui::DockBuilderSplitNode(dock_main_id, direction, ratio, NULL, &dock_main_id);
            _dockSplitIDTable[direction] = id;
        }
        _dockSplitIDTable[ImGuiDir_None] = dock_main_id;
        return true;
    }
    return false;
}

void EditorDockWindow::EndBuildDockLayout() 
{
    _isBuildingDockLayout = false;
    ImGui::DockBuilderFinish(_dockSplitMainID);
}

void EditorDockWindow::PushDockStyle() 
{
    if (_dockWindowOptionFlags & DOCKWINDOW_FLAGS_FULLSCREEN)
    {
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        _pushedStyleCount += 2;
    }
    if (_dockWindowOptionFlags & DOCKWINDOW_FLAGS_PADDING)
    {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        _pushedStyleCount += 1;
    }
}

void EditorDockWindow::PopDockStyle() 
{
    ImGui::PopStyleVar(_pushedStyleCount);
    _pushedStyleCount = 0;
}

ImGuiID EditorDockWindow::GetDockSplitID(int split) const
{
    auto itr = _dockSplitIDTable.find(split);
    if (itr != _dockSplitIDTable.end())
        return itr->second;
    return _dockSplitMainID;
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
        assert(false && L"Dock ratio must be between 0.0f and 1.0f");
        ImClamp(ratio, 0.0f, 1.0f);
    }
    _dockSplitLayoutID.push_back({direction, ratio});
    _dockSplitIDTable[direction] = 0;
}

bool EditorDockWindow::SetGuiDockLayout(EditorTool* tool)
{
    ImGuiID splitID;
    auto    itr = _dockSplitIDTable.find(tool->GetDockLayout());
    if (itr == _dockSplitIDTable.end())
    {
        splitID = _dockSplitIDTable[ImGuiDir_None];
    }
    else
    {
        splitID = _dockSplitIDTable[tool->GetDockLayout()];
    }
    const std::string& label = tool->GetLabel();
    ImGui::DockBuilderDockWindow(label.c_str(), splitID);
    return true;
}
