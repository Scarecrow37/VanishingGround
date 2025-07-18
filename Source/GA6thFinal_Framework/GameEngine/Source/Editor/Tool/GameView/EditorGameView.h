#pragma once

class EditorGameView : public EditorTool
{
public:
    EditorGameView();
    virtual ~EditorGameView() override;

protected:
    virtual void OnTickGui() override {}
    virtual void OnStartGui() override {}
    virtual void OnEndGui() override {}

    /// <summary>
    /// ImGui::Begin 호출 직전에 호출됩니다.
    /// </summary>
    virtual void OnPreFrameBegin() override {}

    /// <summary>
    /// ImGui::Begin 호출 직후에 호출됩니다.
    /// </summary>
    virtual void OnPostFrameBegin() override {}

    /// <summary>
    /// Gui의 프레임이 클리핑이 될 때 1회 호출됩니다.
    /// </summary>
    virtual void OnFrameClipped() override {}

    /// <summary>
    /// ImGui::End 호출 직후에 호출됩니다.
    /// </summary>
    virtual void OnFrameEnd() override {}

    /// <summary>
    /// 프레임이 포커싱 상태에 진입되었을 때 1회 호출됩니다.
    /// </summary>
    virtual void OnFrameFocusEnter() override;

    /// <summary>
    /// 프레임이 포커싱 상태에 있는 동안 매 프레임마다 호출됩니다.
    /// </summary>
    virtual void OnFrameFocusStay() override {}
    /// <summary>
    /// 프레임이 포커싱 상태에서 벗어났을 때 1회 호출됩니다.
    /// </summary>
    virtual void OnFrameFocusExit() override {}

    /// <summary>
    /// ImGui::Begin 호출 후 클리핑 테스트를 통과할 시 호출됩니다.
    /// </summary>
    virtual void OnFrameRender() override;
     
    /// <summary>
    /// 프레임 Tab의 팝업이 열렸을 때 호출됩니다.
    /// </summary>
    virtual void OnFramePopupOpened() override {}

};