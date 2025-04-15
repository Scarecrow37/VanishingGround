#include "pch.h"
#include "EditorDockSpace.h"

EditorDockSpace::EditorDockSpace()
    : _dockSpaceMainID()
    , _dockLayoutID()
    , _dockNodeFlags(ImGuiDockNodeFlags_NoCloseButton | ImGuiDockNodeFlags_NoWindowMenuButton)
    , _dockWindowFlag(ImGuiWindowFlags_None)
{
    SetLabel("DockSpace");
}

EditorDockSpace::~EditorDockSpace()
{
    _editorToolTable.clear();
    _editorToolList.clear();
}

void EditorDockSpace::OnTickGui()
{
}

void EditorDockSpace::OnStartGui()
{
    for (auto& [key, tool] : _editorToolTable)
    {
        if (nullptr != tool)
        {
            tool->OnStartGui();
        }
    }
    std::sort(_editorToolList.begin(), _editorToolList.end(),
        [](EditorBase* a, EditorBase* b) {
            return a->GetCallOrder() > b->GetCallOrder();
        });
}

void EditorDockSpace::OnDrawGui()
{
    UpdateWindowFlag();     // DockWindow Flag
    PushDockStyle();        // Dock Style Push
    
    ImGui::Begin(GetLabel().c_str(), nullptr, _dockWindowFlag);

    SubmitDockSpace();      // Submit Dock
    PopDockStyle();         // Dock Style Pop

    for (auto& tool : _editorToolList)
    {
        if (nullptr != tool)
        {
            ImGui::PushID(reinterpret_cast<uintptr_t>(tool));
            tool->OnDrawGui();
            ImGui::PopID();
        }
    }

    ImGui::End();
}

void EditorDockSpace::OnEndGui()
{
    for (auto& [key, tool] : _editorToolTable)
    {
        if (nullptr != tool)
        {
            tool->OnEndGui();
        }
    }
}

void EditorDockSpace::InitDockLayout()
{
    if (NULL == ImGui::DockBuilderGetNode(_dockSpaceMainID))
    {
        ImGui::DockBuilderRemoveNode(_dockSpaceMainID);
        ImGui::DockBuilderAddNode(_dockSpaceMainID, _dockNodeFlags); // 새로 추가

        ImGuiID dock_main_id = _dockSpaceMainID; // This variable will track the document node, however we are not using it here as we aren't docking anything into it.
        // 오른쪽 20%
        _dockLayoutID[(INT)DockLayout::RIGHT] = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Right, 0.25f, NULL, &dock_main_id);
        // 아래쪽 25%
        _dockLayoutID[(INT)DockLayout::DOWN] = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Down, 0.40f, NULL, &dock_main_id);
        // 왼쪽 20%
        _dockLayoutID[(INT)DockLayout::LEFT] = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Left, 0.30f, NULL, &dock_main_id);
        // 위쪽 25%
        _dockLayoutID[(INT)DockLayout::UP] = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Up, 0.50f, NULL, &dock_main_id);
        // 나머지 가운데
        _dockLayoutID[(INT)DockLayout::CENTER] = dock_main_id;

        for (auto& [key, tool] : _editorToolTable)
        {
            if (tool && tool->GetDockLayout() != DockLayout::NONE)
            {
                ImGui::DockBuilderDockWindow(
                    tool->GetLabel().c_str(),
                    GetDockLayoutID(tool->GetDockLayout())
                );
            }
        }
        ImGui::DockBuilderFinish(_dockSpaceMainID);
    }
}

void EditorDockSpace::SubmitDockSpace()
{
    //////////////////////////////////////////
    // Sumit the DockSpace
    //////////////////////////////////////////
    ImGuiIO& io = ImGui::GetIO();
    ImGuiStyle& style = ImGui::GetStyle();
    float minWinSizeX = style.WindowMinSize.x;
    style.WindowMinSize.x = 370.0f;
    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
    {
        _dockSpaceMainID = ImGui::GetID(GetLabel().c_str());
        InitDockLayout();
        ImGui::DockSpace(_dockSpaceMainID, ImVec2(0.0f, 0.0f), _dockNodeFlags);
    }
    style.WindowMinSize.x = minWinSizeX;
}

void EditorDockSpace::UpdateWindowFlag()
{
    // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
    // because it would be confusing to have two docking targets within each others.
    _dockWindowFlag = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    if (_isFullSpace)
    {
        _dockWindowFlag |=
            ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoBringToFrontOnFocus |
            ImGuiWindowFlags_NoNavFocus;
    }
    // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
    // and handle the pass-thru hole, so we ask Begin() to not render a background.
    if (_dockNodeFlags & ImGuiDockNodeFlags_PassthruCentralNode)
        _dockWindowFlag |= ImGuiWindowFlags_NoBackground;
}

void EditorDockSpace::PushDockStyle()
{
    if (_isFullSpace)
    {
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    }
    if (!_isPadding)
    {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    }
}

void EditorDockSpace::PopDockStyle()
{
    if (_isFullSpace)
        ImGui::PopStyleVar(2);

    if (!_isPadding)
        ImGui::PopStyleVar();
}
