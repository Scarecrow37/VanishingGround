#pragma once
#include "../PlacementUIComponent/PlacementUIComponent.h"

class PanelSlotComponent : public PlacementUIComponent
{
    USING_PROPERTY(PanelSlotComponent)

public:
    PanelSlotComponent();

    void         PassScopedPlacement(POINT point, SIZE size) const;

protected:
    REFLECT_FIELDS_BEGIN(PlacementUIComponent)
    REFLECT_FIELDS_END(PanelSlotComponent)

    void OnDetachParent(GameObject* previousParentGameObject) override;
};