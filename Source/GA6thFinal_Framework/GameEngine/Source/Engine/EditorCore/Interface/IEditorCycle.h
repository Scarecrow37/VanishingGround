#pragma once
class IEditorCycle
{
public:
    /* 항상 계층에 상관없이 매 틱마다 호출 */
    virtual void OnTickGui() = 0;

    /* ImGui시스템이 초기화 된 후 호출 */
    virtual void OnStartGui() = 0;

    /* Gui가 그려져야 할 때 호출 */
    virtual void OnDrawGui() = 0;

    /* ImGui시스템이 닫히기 직전에 호출 */
    virtual void OnEndGui() = 0;
};
