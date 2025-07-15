#pragma once
#include "../PlacementUIComponent/PlacementUIComponent.h"

class PanelSlotComponent : public PlacementUIComponent
{
    USING_PROPERTY(PanelSlotComponent)

public:
    PanelSlotComponent();

    void SetPlacement(POINT point, SIZE size);

    virtual void PassPlacement() const {}

protected:
    REFLECT_FIELDS_BEGIN(PlacementUIComponent)
    REFLECT_FIELDS_END(PanelSlotComponent)

    void OnDetachParent(GameObject* previousParentGameObject) override;
};