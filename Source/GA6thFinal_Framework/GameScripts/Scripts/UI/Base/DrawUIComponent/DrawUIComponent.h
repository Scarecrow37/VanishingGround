#pragma once
#include "UI/Base/PlacementUIComponent/PlacementUIComponent.h"

class DrawUIComponent : public PlacementUIComponent
{
    USING_PROPERTY(DrawUIComponent)

protected:
    static constexpr float VIEW_ORDER_IMAGE_RATIO = 0.1f;
    static constexpr float VIEW_ORDER_TEXT_RATIO  = 0.0001f;
    static constexpr float VIEW_ORDER_TEXT_OFFSET = VIEW_ORDER_TEXT_RATIO / VIEW_ORDER_IMAGE_RATIO;

private:
    static void RequestViewOrder(const Transform& transform);

public:
    DrawUIComponent();

public:
    virtual void SetViewOrder(int viewOrder);

protected:
    void ImGuiDrawPropertysEvent() override;
    void OnAttachChild(GameObject* childGameObject) override;

    virtual float GetZOrder() const;

protected:
    REFLECT_FIELDS_BEGIN(PlacementUIComponent)
    int ViewOrder = 0;
    REFLECT_FIELDS_END(DrawUIComponent)
};