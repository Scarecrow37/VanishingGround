#pragma once
#include "../UIComponent/UIComponent.h"

class PlacementUIComponent : public UIComponent
{
    USING_PROPERTY(PlacementUIComponent)

public:
    PlacementUIComponent();

    void SetScopePlacement(POINT point, SIZE size);

protected:
    REFLECT_FIELDS_BEGIN(UIComponent)
    POINT Point = {0, 0};
    SIZE  Size  = {200, 100};
    POINT ScopePoint;
    SIZE  ScopeSize;
    REFLECT_FIELDS_END(PlacementUIComponent)

    void ResetPlacement();

    void OnDrawDebug() override;

    void OnDrawDebugSelected() override;

    void OnDetachParent(GameObject* previousParentGameObject) override;
};