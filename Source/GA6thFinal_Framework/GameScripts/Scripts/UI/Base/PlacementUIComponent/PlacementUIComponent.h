#pragma once
#include "../UIComponent/UIComponent.h"

class PlacementUIComponent : public UIComponent
{
    USING_PROPERTY(PlacementUIComponent)

protected:
    static constexpr float VIEW_ORDER_IMAGE_RATIO = 0.1f;
    static constexpr float VIEW_ORDER_TEXT_RATIO = 0.0001f;
    static constexpr float VIEW_ORDER_TEXT_OFFSET = VIEW_ORDER_TEXT_RATIO / VIEW_ORDER_IMAGE_RATIO;

public:
    PlacementUIComponent();

public:
    REFLECT_PROPERTY(ViewOrder)
    GETTER(int, ViewOrder)
    {
        return ReflectFields->ViewOrder;
    }
    SETTER(int, ViewOrder)
    {
        SetViewOrder(value);
    }
    PROPERTY(ViewOrder)

public:
    POINT GetPoint() const;
    SIZE  GetSize() const;
    POINT GetScopePoint() const;
    SIZE  GetScopeSize() const;
    POINT GetAbsolutePoint() const;

    void  SetScopePlacement(POINT point, SIZE size);
    int   SortViewOrder(int startOrder);

protected:
    virtual void OnPlacementChange() {};
    void ResetPlacement();
    void ImGuiDrawPropertysEvent() override;
    void OnAttachChild(GameObject* childGameObject) override;
    void OnDetachParent(GameObject* previousParentGameObject) override;
    virtual float GetZOrder() const;
    virtual void OnSetViewOrder() {};

private:
    void SetViewOrder(int viewOrder);
    static void RequestViewOrder(const Transform& transform);

protected:
    REFLECT_FIELDS_BEGIN(UIComponent)
    POINT Point = {0, 0};
    SIZE  Size  = {200, 100};
    POINT ScopePoint;
    SIZE  ScopeSize;
    int   ViewOrder = 0;
    REFLECT_FIELDS_END(PlacementUIComponent)

};
