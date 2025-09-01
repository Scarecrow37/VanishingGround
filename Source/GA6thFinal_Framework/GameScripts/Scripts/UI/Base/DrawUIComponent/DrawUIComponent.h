#pragma once
#include "UI/Base/PlacementUIComponent/PlacementUIComponent.h"

class DrawUIComponent : public PlacementUIComponent
{
    USING_PROPERTY(DrawUIComponent)

protected:
    static constexpr float VIEW_ORDER_IMAGE_RATIO = 0.1f;
    static constexpr float VIEW_ORDER_TEXT_RATIO  = 0.0001f;
    static constexpr float VIEW_ORDER_TEXT_OFFSET = VIEW_ORDER_TEXT_RATIO / VIEW_ORDER_IMAGE_RATIO;

public:
    DrawUIComponent();

public:
    /// <summary>
    /// View Order를 설정합니다.
    /// </summary>
    /// <param name="viewOrder">설정할 View Order 값</param>
    virtual void SetViewOrder(int viewOrder);

protected:
    /// <summary>
    /// 이 컴포넌트의 Z-Order를 반환합니다.
    /// </summary>
    /// <returns>계산된 Z-Order</returns>
    virtual float GetZOrder() const;

    void ImGuiDrawPropertysEvent() override;
    void OnAttachChild(GameObject* childGameObject) override;

private:
    void RequestViewOrder() const;

protected:
    REFLECT_FIELDS_BEGIN(PlacementUIComponent)
    int ViewOrder = 0;
    REFLECT_FIELDS_END(DrawUIComponent)
};