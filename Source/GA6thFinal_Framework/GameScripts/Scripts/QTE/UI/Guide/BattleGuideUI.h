#pragma once

class HoldingProgressImageElement;
class Horizontal;
class OverlayPanel;
class ImageElement;
class TextElement;
class FadeUIComponent;

struct BattleGuideUI
{
    enum { A, RB, RT };

    OverlayPanel*                Overlay    = nullptr;
    FadeUIComponent*             Fader      = nullptr;
    HoldingProgressImageElement* Progress   = nullptr;
    std::array<ImageElement*, 3> ImageList  = {nullptr, nullptr, nullptr};
    std::array<TextElement*, 3>  TextList   = {nullptr, nullptr, nullptr};

    // 해당 오브젝트에 자신의 멤버가 있으면 매칭합니다.
    void MatchUIFromObject(GameObject& object);

    // 상위 오브젝트의 활성화 여부를 조작합니다.
    void Active(bool active);

    // UI 알파를 적용합니다.
    void Alpha(float alpha);

    void FadeIn();

    void FadeOut();
};
