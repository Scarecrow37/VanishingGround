#pragma once

class ScriptTestEditor : public EditorTool
{
public:
    ScriptTestEditor();
    virtual ~ScriptTestEditor() override;

    /* ImGui시스템이 초기화된 후 한번 호출 */
    virtual void OnStartGui() override;

protected:
    /* Begin 호출 전에 항상 호출 (Begin 성공 유무 상관 X) */
    virtual void OnPreFrame() override;

    /* Begin 호출 성공 시 호출 */
    virtual void OnFrame() override;

    /* End 호출 후에 항상 호출 (Begin 성공 유무 상관 X) */
    virtual void OnPostFrame() override {};
};
