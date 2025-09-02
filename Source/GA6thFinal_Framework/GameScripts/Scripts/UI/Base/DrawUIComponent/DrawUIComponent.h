#pragma once

class UIRoot;

class DrawUIComponent : public UIComponent
{
    USING_PROPERTY(DrawUIComponent)

protected:
    static constexpr float VIEW_ORDER_IMAGE_RATIO = 0.1f;
    static constexpr float VIEW_ORDER_TEXT_RATIO  = 0.0001f;
    static constexpr float VIEW_ORDER_TEXT_OFFSET = VIEW_ORDER_TEXT_RATIO / VIEW_ORDER_IMAGE_RATIO;

public:
    DrawUIComponent();

public:
    GETTER_ONLY(UIRoot*, Root)
    {
        UIRoot*          uiRoot    = nullptr;
        const Transform& transform = this->transform;
        if (const Transform* rootTransform = transform.Root; nullptr != rootTransform)
        {
            const GameObject& rootGameObject = rootTransform->gameObject;
            uiRoot                           = GetRoot(rootGameObject);
        }
        return uiRoot;
    }
    PROPERTY(Root)

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
    void           RequestViewOrder() const;
    static UIRoot* GetRoot(const GameObject& rootGameObject);

protected:
    REFLECT_FIELDS_BEGIN(UIComponent)
    int ViewOrder = 0;
    REFLECT_FIELDS_END(DrawUIComponent)
};