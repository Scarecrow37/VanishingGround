#pragma once

class SlotComponent : public UIBaseComponent
{
    USING_PROPERTY(SlotComponent)

public:
    SlotComponent();

public:
    GETTER_ONLY(UIComponent*, UI)
    {
        const GameObject& object      = gameObject;
        UIComponent*      uiComponent = object.GetComponentDynamic<UIComponent>();
        return uiComponent;
    }
    PROPERTY(UI)

protected:
    void OnDetachParent(GameObject* previousParentGameObject) override;

protected:
    REFLECT_FIELDS_BEGIN(UIBaseComponent)
    REFLECT_FIELDS_END(SlotComponent)

};