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
    OnPreFrameBegin();

    InitFrame();

    BeginFrame();

    OnPostFrameBegin();

    if (false == _isFirstTick && true == ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows))
    {
        OnFrameFocused();
    }

    if (true == ImGui::BeginPopupContextItem("##PopupContext"))
    {
        DefaultPopupFrame();
        OnFramePopupOpened();
        ImGui::EndPopup();
    }

    bool canOpenFrame = false == IsClipped() || true == HasEditorToolFlags(EDITORTOOL_FLAGS_ALWAYS_FRAME);

    if (true == canOpenFrame)
    {
        if (true == _isLock)
        {
            ImGui::BeginDisabled();
        }

        OnFrameRender();
        
        if (true == _isLock)
        {
            ImGui::EndDisabled();
        }

    }

    EndFrame();

    OnFrameEnd();

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

bool EditorTool::BeginFrame()
{
    std::string       label     = GetLabel();
    EditorDockWindow* dockSpace = GetOwnerDockWindow();
    if (nullptr != dockSpace)
    {
        auto& windowClass = dockSpace->GetWindowClass();
        ImGui::SetNextWindowClass(&windowClass);
    }
    int windowFlag = _windowFlags | ImGuiWindowFlags_NoCollapse;
    ImGui::Begin(label.c_str(), &_isVisible, windowFlag);

    _isBeginningFrame = true;
    _isClipped = !ImGuiHelper::IsWindowDrawable();

    return _isClipped;
}

void EditorTool::EndFrame()
{
    _isBeginningFrame = false;
    ImGui::End();
}

void EditorTool::InitFrame() 
{
    if (true == _isFirstTick)
    {
        if (true == _size.first)
        {
            ImGui::SetNextWindowSize(_size.second, ImGuiCond_FirstUseEver);
            _size.first = false;
        }

        if (true == _pos.first)
        {
            ImVec2 clientPos = ImGui::GetMainViewport()->Pos;
            ImGui::SetNextWindowPos(clientPos + _pos.second, ImGuiCond_FirstUseEver);
            _pos.first = false;
        }
    }
}

void EditorTool::DefaultPopupFrame()
{
    ImGui::MenuItem("Close", "", &_isVisible);
    ImGui::Separator();
    ImGui::MenuItem("Lock", "", &_isLock);
}
