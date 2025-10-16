#pragma once

class Debugger
{
public:
    /// <summary>
    /// 디버그 모드일 때 ImGui 창을 띄워 func 콜백 함수를 실행합니다.
    /// </summary>
    void operator()(std::function<void()> func)
    {
        // 나중에 #ifdef _UMEDITOR 걸어도 됨
        if (UmCore->DebuggerWindow.DebugMode)
        {
            ImGui::Begin(GUI_LABEL, nullptr, ImGuiWindowFlags_AlwaysAutoResize);
            if (func)
            {
                func();
            }
            ImGui::End();
        }
    }

private:
    inline static constexpr const char* GUI_LABEL = "Debug Tool##debugger";

};
