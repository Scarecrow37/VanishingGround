#pragma once
#include "../UIComponent/UIComponent.h"

class PanelSlotComponent : public UIComponent
{
    friend class Transform;
    USING_PROPERTY(PanelSlotComponent)

public:
    PanelSlotComponent();

protected:
    REFLECT_FIELDS_BEGIN(UIComponent)
    REFLECT_FIELDS_END(PanelSlotComponent)
};