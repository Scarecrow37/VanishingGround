#pragma once
#include "Utility/SingletonHelper.h"

class GameOverManager : public Component, public InputReceiver
{
    USING_PROPERTY(GameOverManager)

public:
    void Awake() override;
    void Start() override;
    void Update() override;

public:
    void ProcessGameOver();

private:
    void TransitionTitleScene();

protected:
    REFLECT_FIELDS_BEGIN(Component)
    REFLECT_FIELDS_END(GameOverManager)
private:
    SingletonComponent<GameOverManager> _singletoneComponent{this};

    bool                          _isBeginProcess       = false;
    class OverlayPanel*           _vanishedOverlay      = nullptr;
    class SpriteAnimationElement* _vanishedAnimation    = nullptr;
};
