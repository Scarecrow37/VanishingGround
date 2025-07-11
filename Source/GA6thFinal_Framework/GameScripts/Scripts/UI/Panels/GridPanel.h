#pragma once
#include "UI/Base/AreaUIComponent/AreaUIComponent.h"
#include "UI/Base/PanelSlotComponent/PanelSlotComponent.h"

class GridPanel : public AreaUIComponent
{
    USING_PROPERTY(GridPanel)
public:
    GridPanel();

    REFLECT_PROPERTY(Columns, Rows)

    GETTER(unsigned int, Columns) { return ReflectFields->Columns; }
    SETTER(unsigned int, Columns) { ReflectFields->Columns = std::clamp(value, 1u, 64u); }
    PROPERTY(Columns)

    GETTER(unsigned int, Rows) { return ReflectFields->Rows; }
    SETTER(unsigned int, Rows) { ReflectFields->Rows = std::clamp(value, 1u, 64u); }
    PROPERTY(Rows)

protected:
    REFLECT_FIELDS_BEGIN(AreaUIComponent)
    unsigned int Columns = 1;
    unsigned int Rows = 1;
    REFLECT_FIELDS_END(GridPanel)

    void OnAttachChild(GameObject* childGameObject) override;

    void OnDrawDebug() override;

    void OnDrawDebugSelected() override;

private:
    void DrawLine(FXMVECTOR color) const;

};

class GridPanelSlot : public PanelSlotComponent
{
    USING_PROPERTY(GridPanelSlot)
public:
    GridPanelSlot()           = default;
    ~GridPanelSlot() override = default;

    REFLECT_PROPERTY()

protected:
    REFLECT_FIELDS_BEGIN(PanelSlotComponent)
    REFLECT_FIELDS_END(GridPanelSlot)
};