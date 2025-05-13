#include "pch.h"
#include "EditorTool.h"

/*
2025.03.13 -
Begin의 if문 안에 End를 넣으니까 같은 Tab으로 Docking시도 시 Missing End() 예외가 발생하며 터짐.
이유는 ImGui 공식 문서에 있었다...
ImGui 공식 문서:
When using Docking, it is expected that all dockable windows are submitted each frame and properly Begin/End even if you don't show their contents.
해석:
도킹을 사용할 때는 도킹 가능한 모든 창이 각 프레임에 제출되고, 창이 내용을 표시하지 않더라도 적절하게 시작/종료되는 것이 예상됩니다.

결론: Docking을 사용 시 Begin상관 없이 End를 호출해줘야 한다.

2025.04.02
Popup창도 OpenPopup을 할 필요가 없다. 그냥 BeginPopupContextItem만 호출해줄 것
*/
void EditorTool::OnDrawGui()
{
    ImGui::PushID(this);

    ProcessPopupFrame();    // 팝업 확인, 처리 및 콜백

    OnPreFrameBegin();      // [Callback] Begin 이전 콜백

    BeginFrame();           // Begin 호출 및 프레임 처리

    OnPostFrameBegin();     // [Callback] Begin 이후 콜백

    ProcessFocusFrame();    // 포커스 확인, 처리 및 콜백

    bool canOpenFrame = false == IsClipped() || true == HasEditorToolFlags(EDITORTOOL_FLAGS_ALWAYS_FRAME);
    if (true == canOpenFrame)
    {
        OnFrameRender();    // [Callback] Begin 이후 클리핑 통과 시 콜백
    }

    EndFrame();             // End 호출 및 프레임 처리

    OnFrameEnd();           // [Callback] End 이후 콜백

    ImGui::PopID();

    _isFirstTick = false;
}

void EditorTool::OnPreFrameBegin()
{
}

void EditorTool::OnPostFrameBegin()
{
}

void EditorTool::OnFrameRender() 
{
}

void EditorTool::OnFrameEnd()
{
}

void EditorTool::OnFrameFocused()
{
}

void EditorTool::OnFramePopupOpened()
{
}

void EditorTool::PushStyle() 
{
    if (true == HasEditorToolFlags(EDITORTOOL_FLAGS_NO_PADDING))
    {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ++_imguiSytleStackCount;
    }
}

void EditorTool::PopStyle() 
{
    ImGui::PopStyleVar(_imguiSytleStackCount);
    _imguiSytleStackCount = 0;
}

void EditorTool::BeginFrame()
{
    InitFrame();

    auto label      = GetLabel().c_str();
    auto owner      = GetOwnerDockWindow();
    int  windowFlag = _windowFlags | ImGuiWindowFlags_NoCollapse;

    // 속한 도킹스페이스가 있으면 윈도우 클래스 지정
    if (nullptr != owner)
    {
        auto& windowClass = owner->GetWindowClass();
        ImGui::SetNextWindowClass(&windowClass);
    }
   
    ImGui::Begin(label, &_isVisible, windowFlag);

    _imguiWindow      = ImGui::GetCurrentWindow();
    _isBeginningFrame = true;
    _isClipped        = !ImGuiHelper::IsWindowDrawable();

    if (true == _isLock)
    {
        ImGui::BeginDisabled();
        _isBeginningDisable = true;
    }
}

void EditorTool::EndFrame()
{
    if (true == _isBeginningDisable)
    {
        ImGui::EndDisabled();
    }

    _isBeginningFrame = false;
    _isBeginningDisable = false;

    ImGui::End();
}

void EditorTool::InitFrame() 
{
    if (true == _isFirstTick)
    {
        // Init Size
        if (true == _size.first)
        {
            ImGui::SetNextWindowSize(_size.second, ImGuiCond_FirstUseEver);
            _size.first = false;
        }
        // Init Position
        if (true == _pos.first)
        {
            ImVec2 clientPos = ImGui::GetMainViewport()->Pos;
            ImGui::SetNextWindowPos(clientPos + _pos.second, ImGuiCond_FirstUseEver);
            _pos.first = false;
        }
        // Init DockLayout
        if (nullptr != _ownerDockWindow)
        {
            _ownerDockWindow->SetDockBuildWindow(_label, _dockLayout.second);
            _dockLayout.first = false;
        }
    }
}

void EditorTool::ProcessPopupFrame() 
{
    if (nullptr != _imguiWindow)
    {
        if (true == _imguiWindow->SkipItems)
            return;

        ImGuiDockNode* node   = _imguiWindow->DockNode;

        if (nullptr == node)
            return;

        ImGuiTabBar* tabbar = node ? node->TabBar : nullptr;

        if (tabbar)
        {
            ImVec2 min = tabbar->BarRect.Min, max = tabbar->BarRect.Max;

            bool isTabBarHovered = ImGui::IsMouseHoveringRect(min, max, false);
            bool isMouseReleased = ImGui::IsMouseReleased(ImGuiMouseButton_Right);

            if (true == isTabBarHovered && true == isMouseReleased)
            {
                ImGui::OpenPopup("##TabBarContextMenu");
            }
            if (ImGui::BeginPopup("##TabBarContextMenu"))
            {
                ImGui::MenuItem("Close", "", &_isVisible);
                ImGui::Separator();
                ImGui::MenuItem("Lock", "", &_isLock);
                OnFramePopupOpened();
                ImGui::EndPopup();
            }
        }
    }
}

void EditorTool::ProcessFocusFrame()
{
    if (false == _isFirstTick && true == ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows))
    {
        OnFrameFocused();
    }
}
