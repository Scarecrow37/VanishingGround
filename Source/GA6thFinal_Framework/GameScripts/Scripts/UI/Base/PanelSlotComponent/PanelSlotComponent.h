#pragma once
#include "../PlacementUIComponent/PlacementUIComponent.h"

class PanelSlotComponent : public PlacementUIComponent
{
    USING_PROPERTY(PanelSlotComponent)

public:
    PanelSlotComponent();

public:
    void PassScopedPlacementToSibling(POINT point, SIZE size) const;

protected:
    void OnDetachParent(GameObject* previousParentGameObject) override;

protected:
    REFLECT_FIELDS_BEGIN(PlacementUIComponent)
    REFLECT_FIELDS_END(PanelSlotComponent)

};