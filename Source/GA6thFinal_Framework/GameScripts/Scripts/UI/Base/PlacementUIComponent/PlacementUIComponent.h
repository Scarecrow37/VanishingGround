#pragma once
#include "../UIComponent/UIComponent.h"

class PlacementUIComponent : public UIComponent
{
    USING_PROPERTY(PlacementUIComponent)
public:
    PlacementUIComponent();

public:
    virtual SIZE GetContentSize() const;

    POINT GetPoint() const;
    SIZE  GetSize() const;
    POINT GetScopePoint() const;
    SIZE  GetScopeSize() const;
    POINT GetAbsolutePoint() const;

    void SetScopePlacement(POINT scopePoint, SIZE scopeSize); // 내림

protected:
    void ImGuiDrawPropertysEvent() override;
    void OnDetachParent(GameObject* previousParentGameObject) override;

    virtual void OnChildPlacementChange(PlacementUIComponent* changedComponent){} // 올라옴
    virtual void  OnPlacementChange() {}
    virtual void  SpreadPlacementToParent(); // 올림

    void ResetPlacement();

protected:
    REFLECT_FIELDS_BEGIN(UIComponent)
    POINT Point = {0, 0};
    SIZE  Size  = {200, 100};
    POINT ScopePoint;
    SIZE  ScopeSize;
    REFLECT_FIELDS_END(PlacementUIComponent)

};
