#include "pchScripts.h"
#include "UIRoot.h"

UIRoot::UIRoot() : _isShowEditor(false)
{
    if (const ImGuiViewport* viewport = ImGui::GetMainViewport(); viewport != nullptr)
    {
        const ImVec2 size = viewport->Size;
        _size.cx = static_cast<LONG>(size.x);
        _size.cy    = static_cast<LONG>(size.y);
    }
}