#pragma once
#include "UI/Base/EditablePlacementUIComponent/EditablePlacementUIComponent.h"

class RatioWrapper : public EditablePlacementUIComponent
{
    USING_PROPERTY(RatioWrapper)

public:
    REFLECT_PROPERTY(Ratio)

    GETTER(float, Ratio) { return ReflectFields->Ratio; }
    SETTER(float, Ratio)
    {
        ReflectFields->Ratio = std::max(value, 0.1f);
        OnPlacementChange();
    }
    PROPERTY(Ratio)

public:
    float GetRatio() const;

protected:
    void OnPlacementChange() override;
    void OnAttachChild(GameObject* childGameObject) override;

private:
    void AssignChild(PlacementUIComponent& component) const;

protected:
    REFLECT_FIELDS_BEGIN(EditablePlacementUIComponent)
    float Ratio = 1.0f;
    REFLECT_FIELDS_END(RatioWrapper)
};