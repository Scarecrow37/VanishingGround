#pragma once
#include "UI/Base/EditablePlacementUIComponent/EditablePlacementUIComponent.h"

class PaddingWrapper : public EditablePlacementUIComponent
{
    USING_PROPERTY(PaddingWrapper)

public:
    REFLECT_PROPERTY(Left, Right, Top, Bottom)

    GETTER(int, Left) { return ReflectFields->PadLeft; }
    SETTER(int, Left)
    {
        ReflectFields->PadLeft = value;
        OnPlacementChange();
    }
    PROPERTY(Left)

    GETTER(int, Right) { return ReflectFields->PadRight; }
    SETTER(int, Right)
    {
        ReflectFields->PadRight = value;
        OnPlacementChange();
    }
    PROPERTY(Right)

    GETTER(int, Top) { return ReflectFields->PadTop; }
    SETTER(int, Top)
    {
        ReflectFields->PadTop = value;
        OnPlacementChange();
    }
    PROPERTY(Top)

    GETTER(int, Bottom) { return ReflectFields->PadBottom; }
    SETTER(int, Bottom)
    {
        ReflectFields->PadBottom = value;
        OnPlacementChange();
    }
    PROPERTY(Bottom)

public:
    POINT GetPaddedPoint() const;
    SIZE  GetPaddedSize() const;

protected:
    void OnPlacementChange() override;
    void OnAttachChild(GameObject* childGameObject) override;

private:
    void AssignChild(PlacementUIComponent& component) const;

protected:
    REFLECT_FIELDS_BEGIN(EditablePlacementUIComponent)
    int PadLeft = 0; 
    int PadRight = 0; 
    int PadTop   = 0; 
    int PadBottom = 0;
    REFLECT_FIELDS_END(PaddingWrapper)

};