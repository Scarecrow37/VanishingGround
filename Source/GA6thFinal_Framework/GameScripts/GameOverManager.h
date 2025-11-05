#pragma once
#include "Utility/SingletonHelper.h"

class GameOverManager : public Component, public InputReceiver
{
    USING_PROPERTY(GameOverManager)

public:
    void Start() override;
    void Update() override;

public:
    void ProcessGameOver();

protected:
    REFLECT_FIELDS_BEGIN(Component)
    REFLECT_FIELDS_END(GameOverManager)
private:
    bool                          _isBeginProcess       = false;
    class OverlayPanel*           _vanishedOverlay      = nullptr;
    class SpriteAnimationElement* _vanishedAnimation    = nullptr;
};
