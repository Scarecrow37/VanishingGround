#include "pch.h"
#include "EditorTool.h"

bool EditorTool::SerializeFromData(EditorToolSerializeData* data)
{
    if(nullptr == data)
        return false;

    Super::SerializeFromData(data);

    data->Name            = GetLabel();
    data->IsLock          = IsLock();
    data->ReflectionField = SerializedReflectFields();

    return true;
}

bool EditorTool::DeSerializeFromData(EditorToolSerializeData* data)
{
    if (nullptr == data)
        return false;

    Super::DeSerializeFromData(data);

    SetLock(data->IsLock);
    if (data->ReflectionField != "{}")
    {
        DeserializedReflectFields(data->ReflectionField);
    }
    return false;
}

/*
2025.03.13 -
Begin의 if문 안에 End를 넣으니까 같은 Tab으로 Docking시도 시 Missing End() 예외가 발생하며 터짐.
이유는 ImGui 공식 문서에 있었다...
ImGui 공식 문서:
When using Docking, it is expected that all dockable windows are submitted each frame and properly Begin/End even if you
don't show their contents. 해석: 도킹을 사용할 때는 도킹 가능한 모든 창이 각 프레임에 제출되고, 창이 내용을 표시하지
않더라도 적절하게 시작/종료되는 것이 예상됩니다.

결론: Docking을 사용 시 Begin상관 없이 End를 호출해줘야 한다.

2025.04.02
Popup창도 OpenPopup을 할 필요가 없다. 그냥 BeginPopupContextItem만 호출해줄 것
*/

void EditorTool::OnDrawGui()
{
    ProcessPopupFrame();    // 팝업 확인, 처리 및 콜백

    OnPreFrameBegin();      // [Callback] Begin 이전 콜백
    BeginFrame();           // Begin 호출 및 프레임 처리
    OnPostFrameBegin();     // [Callback] Begin 이후 콜백

    ProcessFocusFrame();    // 포커스 확인, 처리 및 콜백

    ProcessRenderFrame();   // 클리핑 확인, 처리 및 콜백

    EndFrame();             // End 호출 및 프레임 처리

    OnFrameEnd();           // [Callback] End 이후 콜백

    _isFirstTick = false;
}

void EditorTool::OnPreFrameBegin() {}
void EditorTool::OnPostFrameBegin() {}
void EditorTool::OnFrameRender() {}
void EditorTool::OnFrameClipped() {}
void EditorTool::OnFrameEnd() {}
void EditorTool::OnFrameFocusEnter() {}
void EditorTool::OnFrameFocusStay() {}
void EditorTool::OnFrameFocusExit() {}
void EditorTool::OnFramePopupOpened() {}

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
    _isDrawable       = ImGuiHelper::IsWindowDrawable();

    if (true == _isLock)
    {
        ImGui::BeginDisabled();
        _isFrameDisable = true;
    }
}

void EditorTool::EndFrame()
{
    if (true == _isFrameDisable)
    {
        ImGui::EndDisabled();
    }

    _isBeginningFrame = false;
    _isFrameDisable   = false;

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
    if (false == _isFirstTick)
    {
        if (true == ImGui::IsWindowFocused(ImGuiFocusedFlags_DockHierarchy | ImGuiFocusedFlags_RootAndChildWindows))
        {
            if (false == _isFrameFocused)
            {
                OnFrameFocusEnter();
            }
            OnFrameFocusStay();
            _isFrameFocused = true;
        }
        else
        {
            if (true == _isFrameFocused)
            {
                OnFrameFocusExit();
            }
            _isFrameFocused = false;
        }
    }
}

void EditorTool::ProcessRenderFrame()
{
    bool isDrawable     = _isDrawable;
    bool isAlwaysFrame  = HasEditorToolFlags(EDITORTOOL_FLAGS_ALWAYS_FRAME);
    bool canOpenFrame   = true == isDrawable || true == isAlwaysFrame;
    if (true == canOpenFrame)
    {
        OnFrameRender(); // [Callback] Begin 이후 클리핑 통과 시 콜백
        _isFrameDrawed = true;
    }
    // 이전 프레임에 클립되지 않았고 현재 프레임에 클립된 경우 
    else if (false == isDrawable && true == _isFrameDrawed)
    {
        OnFrameClipped(); // [Callback] Begin 이후 클리핑 통과 실패 시 콜백
        _isFrameDrawed = false;
    }
}
