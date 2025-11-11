#pragma once
#include "ViewModels/Weapon/WeaponViewModel.h"

class ImageElement;
class RevelationsXYBView : public Component
{
    USING_PROPERTY(RevelationsXYBView)

public:
    RevelationsXYBView();
    ~RevelationsXYBView() override;

public:
    REFLECT_PROPERTY(RevelationIndex)

    GETTER(int, RevelationIndex) { return ReflectFields->RevelationIndex; }
    SETTER(int, RevelationIndex) { ReflectFields->RevelationIndex = std::clamp(value, 0, 3); }
    PROPERTY(RevelationIndex)

protected:
    REFLECT_FIELDS_BEGIN(Component)
    int RevelationIndex = 0;
    REFLECT_FIELDS_END(RevelationsXYBView)

    void Awake() override;
    void Start() override;

private:
    void FindElements();
    WeaponViewModel::Handle _watchHandle;

    struct UiImages
    {
        ImageElement* X = nullptr;
        ImageElement* Y = nullptr;
        ImageElement* B = nullptr;

        void SetEnable(bool enable);
        void SetLeftEnable(bool enable);
        void SetMiddleEnable(bool enable);
        void SetRightEnable(bool enable);
    };
    UiImages _disable;
    UiImages _enable;
    void UpdateUI();
};

