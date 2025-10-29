#pragma once
#include "QTENoteUI.h"
#include "QTEJudgeEffectUI.h"

class OverlayPanel;
class ImageElement;
class SpriteAnimationElement;

namespace QTE
{
    struct FieldUI
    {
        enum UIState
        {
            STATE_NONE,
            STATE_ENTER,
            STATE_STAY,
        };

        OverlayPanel*           Overlay         = nullptr;
        ImageElement*           Line            = nullptr;
        SpriteAnimationElement* Flow            = nullptr;
        SpriteAnimationElement* JudgeNote       = nullptr;
        SpriteAnimationElement* StartAnimation  = nullptr;

        std::vector<QTE::NoteUI>        NotePool;
        std::vector<QTE::JudgeEffectUI> EffectPool;

        void Initialize(File::Guid noteGuid, File::Guid effectGuid, size_t poolSize);

        // 해당 오브젝트에 자신의 멤버가 있으면 매칭합니다.
        void MatchUIFromObject(GameObject& object);

        // 상위 오브젝트의 활성화 여부를 조작합니다.
        void Active(bool active);

        // UI 알파를 적용합니다.
        void Alpha(float alpha);

        void Reset();

        void OnQTEEnter();

        void Update();

        void OnButtonPressed();

    private:
        static constexpr const char* OVERLAY_TAG = "QTE Field Panel";
        static constexpr const char* LINE_TAG = "QTE Note Line";
        static constexpr const char* FLOW_TAG = "QTE Flow";
        static constexpr const char* JUDGE_TAG = "QTE Judge Note";
        static constexpr const char* START_ANIMATION_TAG = "QTE Start Animation";
    };
} // namespace QTE