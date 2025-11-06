#pragma once
#include "Utility/SingletonHelper.h"
#include "Utility/FadeHelper.h"

class OverlayPanel;
class ImageElement;
class SpriteAnimationElement;

class GameOverManager : public Component, public InputReceiver
{
    USING_PROPERTY(GameOverManager)

private:
    void Awake() override;
    void Start() override;
    void Update() override;

    void ImGuiDrawPropertysEvent() override;

public:
    void ProcessGameOver();

private:
    void TransitionTitleScene();

    void PressedAButton(const Input::Controller& controller);

protected:
    REFLECT_FIELDS_BEGIN(Component)
    REFLECT_FIELDS_END(GameOverManager)
private:
    SingletonComponent<GameOverManager> _singletoneComponent{this};

    bool                        _isBeginProcess       = false;
    OverlayPanel*               _vanishedOverlay      = nullptr;
    ImageElement*               _vanishedBackground   = nullptr;
    SpriteAnimationElement*     _vanishedAnimation    = nullptr;

    Fader _backgroundFader;
};
