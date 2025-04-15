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
    ImGuiIO& io = ImGui::GetIO();

    OnTickGui();

    if (GetVisible())
    {
        OnPreFrame();

        std::string label = GetLabel();
        ImGui::Begin(label.c_str(), &_isVisible, _windowFlags | ImGuiWindowFlags_NoCollapse);

        if (true == Global::editorManager->IsDebugMode())
        {
            DefaultDebugFrame();
        }

        if (true == ImGui::IsWindowFocused())
        {
            OnFocus();
        }

        if (true == ImGui::BeginPopupContextItem("PopupContext"))
        {
            DefaultPopupFrame();
            OnPopup();
            ImGui::EndPopup();
        }

        if (true == _isLock)
        {
            ImGui::BeginDisabled();
        }

        OnFrame();

        if (true == _isLock)
        {
            ImGui::EndDisabled();
        }

        ImGui::End();

        OnPostFrame();
    }
}

void EditorTool::OnPreFrame()
{
}

void EditorTool::OnFrame()
{
}

void EditorTool::OnPostFrame()
{
}

void EditorTool::OnFocus()
{
}

void EditorTool::OnPopup()
{
}

void EditorTool::DefaultPopupFrame()
{
    ImGui::MenuItem("Close", "", &_isVisible);
    ImGui::Separator();
    ImGui::MenuItem("Lock", "", &_isLock);
    //ImGui::Separator();
}

void EditorTool::DefaultDebugFrame()
{
    static char tooltip[256];

    snprintf(tooltip, sizeof(tooltip), 
        "GuiID: 0x%08X\nDockID: %d\nFlag: %d\nOrder: %d",
        ImGui::GetID(""),
        ImGui::GetWindowDockID(), 
        _windowFlags, 
        _callOrder
    );

    if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal))
    {
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(tooltip);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}
