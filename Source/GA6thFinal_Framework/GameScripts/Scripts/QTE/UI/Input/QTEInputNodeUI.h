#pragma once
#include "QTE/Result/QTEResult.h"

class GameObject;
class OverlayPanel;
class ImageElement;
class SpriteAnimationElement;

namespace QTE
{
    class InputNodeUI
    {
        enum Button {X, Y, B, BUTTON_COUNT};
        enum Judge {MISS, GOOD, PERFECT, JUDGE_COUNT};
        using Element = std::array<SpriteAnimationElement*, JUDGE_COUNT>;
    public:
        InputNodeUI(const File::Guid& prefab, Transform* parent);
        ~InputNodeUI();

    public:
        // 상위 오브젝트의 활성화 여부를 조작합니다.
        void Active(bool active);

        // UI 알파를 적용합니다.
        void Alpha(float alpha);

        // UI 상태를 초기화합니다.
        void Reset();

        void Show(Input::Controller::Button button, QTE::ResultType result);

        void SetParent(Transform* parent);

    private:
        void SpawnObject(const File::Guid& prefab, Transform* parent);

    public:
        Button  InputButton = X;
        Judge   InputJudge  = MISS;

        OverlayPanel* Overlay = nullptr;
        std::array<Element, BUTTON_COUNT> ButtonImage = {nullptr, nullptr, nullptr};

    private:
        static constexpr std::array<const char*, BUTTON_COUNT> BUTTON_IMAGE_TAG = {
            "QTE Button X Image", "QTE Button Y Image", "QTE Button B Image"
        };
        static constexpr std::array<const char*, JUDGE_COUNT> BUTTON_JUDGE_TAG = {
            "Miss", "Good", "Perfect"};
    };
}
