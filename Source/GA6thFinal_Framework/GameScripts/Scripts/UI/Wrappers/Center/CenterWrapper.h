#pragma once
#include "UI/Base/EditablePlacementUIComponent/EditablePlacementUIComponent.h"

class CenterWrapper : public EditablePlacementUIComponent
{
    USING_PROPERTY(CenterWrapper)

    using CenterType = unsigned char;

    static constexpr CenterType CENTER_HORIZONTAL = 0b01;
    static constexpr CenterType CENTER_VERTICAL   = 0b10;

public:
    REFLECT_PROPERTY(Horizontal, Vertical)

    GETTER(bool, Horizontal) { return IsCenterHorizontal(); }
    SETTER(bool, Horizontal)
    {
        if (value)
            ReflectFields->CenterType |= CENTER_HORIZONTAL;
        else
            ReflectFields->CenterType &= ~CENTER_HORIZONTAL;
        OnPlacementChange();
    }
    PROPERTY(Horizontal)

    GETTER(bool, Vertical) { return IsCenterVertical(); }
    SETTER(bool, Vertical)
    {
        if (value)
            ReflectFields->CenterType |= CENTER_VERTICAL;
        else
            ReflectFields->CenterType &= ~CENTER_VERTICAL;
        OnPlacementChange();
    }
    PROPERTY(Vertical)

public:
    POINT GetCenterPoint(SIZE childSize) const;
    bool  IsCenterHorizontal() const;
    bool  IsCenterVertical() const;

protected:
    void OnPlacementChange() override;
    void OnAttachChild(GameObject* childGameObject) override;

private:
    void AssignChild(PlacementUIComponent& component) const;

protected:
    REFLECT_FIELDS_BEGIN(EditablePlacementUIComponent)
    CenterType CenterType = CENTER_HORIZONTAL | CENTER_VERTICAL;
    REFLECT_FIELDS_END(CenterWrapper)
};