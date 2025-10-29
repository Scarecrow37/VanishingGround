#pragma once

class ImageElement;
class SpriteAnimationElement;

namespace QTE
{
    struct BackgroundUI
    {
        ImageElement* Background = nullptr;

        //std::array<ImageElement*, 3> Guide = {nullptr, nullptr, nullptr};

        // 해당 오브젝트에 자신의 멤버가 있으면 매칭합니다.
        void MatchUIFromObject(GameObject& object);

        // 상위 오브젝트의 활성화 여부를 조작합니다.
        void Active(bool active);

        // UI 알파를 적용합니다.
        void Alpha(float alpha);

        void Reset();

    private:
        static constexpr const char* BACKGROUND_TAG = "QTE Background";
    };
}