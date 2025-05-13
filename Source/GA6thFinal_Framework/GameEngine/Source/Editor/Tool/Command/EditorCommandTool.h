#pragma once

class EditorCommandTool : public EditorTool
{
public:
    EditorCommandTool();
    virtual ~EditorCommandTool();

private:
    virtual void OnTickGui() override;
    virtual void OnStartGui() override;
    virtual void OnEndGui() override;

private:
    /* Begin 호출 전에 호출 */
    virtual void OnPreFrameBegin();

    /* Begin 호출 시 호출 */
    virtual void OnPostFrameBegin();

    /* End 호출 후에 호출 */
    virtual void OnFrameEnd();

    /* 프레임이 포커싱 될 때 호출 (Begin 후에 호출) */
    virtual void OnFrameFocused();

    /* PopUp창 호출 성공 시 호출 (Begin 후에 호출) */
    virtual void OnFramePopupOpened();

private:
    ImVec4 GetSelectableColor(int index, ImVec4 color);

private:
    ImVec4 _tableDefaultColor = ImVec4(0.4f, 0.4f, 0.4f, 0.8f);
    ImVec4 _tableHoveredColor = ImVec4(0.6f, 0.6f, 0.6f, 0.8f);
    ;
};
