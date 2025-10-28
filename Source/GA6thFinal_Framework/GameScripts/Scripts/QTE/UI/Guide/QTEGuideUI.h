#pragma once

class OverlayPanel;
class ImageElement;
class TextElement;

namespace QTE
{
    struct GuideUI
    {
        enum { X, Y, B, BUTTON_COUNT, };

        OverlayPanel* Overlay = nullptr;
        std::array<ImageElement*, BUTTON_COUNT> GuideButton = {nullptr, nullptr, nullptr};
        std::array<TextElement*, BUTTON_COUNT>  GuideText   = {nullptr, nullptr, nullptr};

        // 해당 오브젝트에 자신의 멤버가 있으면 매칭합니다.
        void MatchUIFromObject(GameObject& object);

        // 상위 오브젝트의 활성화 여부를 조작합니다.
        void Active(bool active);

        // UI 알파를 적용합니다.
        void Alpha(float alpha);

    private:
        static constexpr const char* OVERLAY_TAG = "QTE Guide Panel";
        static constexpr const char* GUIDE_BUTTON_TAG[3] = {
            "QTE Guide Button X",
            "QTE Guide Button Y",
            "QTE Guide Button B",
        };
        static constexpr const char* GUIDE_TEXT_TAG[3] = {
            "QTE Guide Text X",
            "QTE Guide Text Y",
            "QTE Guide Text B",
        };
    };
} // namespace QTE
