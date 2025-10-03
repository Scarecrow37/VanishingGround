#pragma once

class Debugger
{
public:
    inline static bool IsDebugMode() { return _debugMode; }
    inline static void SetDebugMode(bool debug) { _debugMode = debug; }

    /// <summary>
    /// 디버그 모드일 때 ImGui 창을 띄워 func 콜백 함수를 실행합니다.
    /// </summary>
    void operator()(std::function<void(void)> func)
    {
        // 나중에 #ifdef _UMEDITOR 걸어도 됨
        if (IsDebugMode())
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
    inline static bool _debugMode = true; // TODO: 나중에 false가 기본 값이 되도록 변경해야함
    inline static constexpr const char* GUI_LABEL = "Debug Tool##debugger";

};
