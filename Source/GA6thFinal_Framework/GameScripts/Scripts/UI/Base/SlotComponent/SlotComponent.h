#pragma once

class SlotComponent : public UIBaseComponent
{
    USING_PROPERTY(SlotComponent)

public:
    SlotComponent();

protected:
    void OnDetachParent(GameObject* previousParentGameObject) override;

protected:
    REFLECT_FIELDS_BEGIN(UIBaseComponent)
    REFLECT_FIELDS_END(SlotComponent)

};