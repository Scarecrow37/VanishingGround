#pragma once
#include "../UIComponent/UIComponent.h"

class PlacementUIComponent : public UIComponent
{
    USING_PROPERTY(PlacementUIComponent)

protected:
    static constexpr float VIEW_ORDER_IMAGE_RATIO = 0.1f;
    static constexpr float VIEW_ORDER_TEXT_RATIO  = 0.0001f;
    static constexpr float VIEW_ORDER_TEXT_OFFSET = VIEW_ORDER_TEXT_RATIO / VIEW_ORDER_IMAGE_RATIO;

private:
    static void RequestViewOrder(const Transform& transform);

public:
    PlacementUIComponent();

public:
    virtual SIZE GetContentSize() const;
    virtual void  SetViewOrder(int viewOrder);

    POINT GetPoint() const;
    SIZE  GetSize() const;
    POINT GetScopePoint() const;
    SIZE  GetScopeSize() const;
    POINT GetAbsolutePoint() const;

    void SetScopePlacement(POINT scopePoint, SIZE scopeSize); // 내림

protected:
    void ImGuiDrawPropertysEvent() override;
    void OnAttachChild(GameObject* childGameObject) override;
    void OnDetachParent(GameObject* previousParentGameObject) override;

    virtual void OnChildPlacementChange(PlacementUIComponent* changedComponent){} // 올라옴
    virtual void  OnPlacementChange() {}
    virtual float GetZOrder() const;
    virtual void  SpreadPlacementToParent(); // 올림

    void ResetPlacement();

protected:
    REFLECT_FIELDS_BEGIN(UIComponent)
    POINT Point = {0, 0};
    SIZE  Size  = {200, 100};
    POINT ScopePoint;
    SIZE  ScopeSize;
    int   ViewOrder = 0;
    REFLECT_FIELDS_END(PlacementUIComponent)

};
