#pragma once
#include "../UIComponent/UIComponent.h"

class PanelSlotComponent : public UIComponent
{
    USING_PROPERTY(PanelSlotComponent)

public:
    PanelSlotComponent();

protected:
    REFLECT_FIELDS_BEGIN(UIComponent)
    REFLECT_FIELDS_END(PanelSlotComponent)

    void OnDetachParent(GameObject* previousParentGameObject) override;
};