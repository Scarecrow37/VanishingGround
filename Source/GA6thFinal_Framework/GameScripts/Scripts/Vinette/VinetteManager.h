#pragma once
#include "Utility/FadeHelper.h"
#include "Utility/SingletonHelper.h"

class OverlayPanel;
class SpriteAnimationElement;

class VinetteManager : public Component, public InputReceiver
{
    USING_PROPERTY(VinetteManager)
public:
    REFLECT_PROPERTY(Duration)

    SETTER(float, Duration) { ReflectFields->Duration = value; }
    GETTER(float, Duration) { return ReflectFields->Duration; }
    PROPERTY(Duration)

private:
    void Awake() override;

public:
    void ShowHitVinette();
    void ShowHealVinette();

protected:
    REFLECT_FIELDS_BEGIN(Component)
    float Duration = 0.5f;
    REFLECT_FIELDS_END(VinetteManager)
private:
    SingletonComponent<VinetteManager> _singletonComponent{this};
    OverlayPanel*           _vinetteOverlay     = nullptr;
    SpriteAnimationElement* _hitVinette         = nullptr;
    SpriteAnimationElement* _healVinette        = nullptr;
};
