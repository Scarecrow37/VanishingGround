#pragma once
#include "QTE/Result/QTEResult.h"

class OverlayPanel;
class ImageElement;
class SpriteAnimationElement;

namespace QTE 
{
    class NoteUI
    {
    public:
        NoteUI(const File::Guid& prefab, Transform* parent = nullptr);
        ~NoteUI();

        enum UIState
        {
            STATE_AVAILABLE,

            STATE_WAIT,
            STATE_VISIBLE,
            STATE_DEAD
        };

    private:
        void SpawnObject(const File::Guid& prefab, Transform* parent);

        float GetNoteWidth();

        /// <summary>노트 사이즈가 고려되지 않은 x값을 사이즈까지 계산해 x포지션을 위치시킵니다. 최종 x좌표 위치를 반환합니다.</summary>
        float SetNotePositionX(const float positionX);

        /// <summary>노트의 알파 처리를 계산합니다.</summary>
        void ProcessAlpha(const float positionX, const float fadeInStartX, const float fadeInEndX,
                          const float fadeOutStartX, const float fadeOutEndX);

    public:
        void Alpha(float alpha);

        bool IsAvailable();
        
        void Reset();

        bool TrySetup(const float noteTime = 0.0f);

        void Update(const float currTime, const float travelTime, const float currSpeed, 
                    const float startX, const float endX, const float perfectX,
                    float fadeInStartX = FLT_MIN, float fadeInEndX= FLT_MAX,
                    float fadeOutStartX = FLT_MIN, float fadeOutEndX= FLT_MAX,
                    const float offsetX = 0.0f);

        void OnNotePressed(const QTE::NoteResult& resultType);

    private:
        void OnNoteEnter();
        void OnNoteExit();
        void OnDeadUpdate();

    public:
        float                   Time            = 0.0f;
        UIState                 State           = STATE_AVAILABLE;
        QTE::ResultType         Result          = QTE::QTE_RESULT_NONE;

        OverlayPanel*           Overlay         = nullptr;
        SpriteAnimationElement* StartAnimation  = nullptr;
        SpriteAnimationElement* EndAnimation    = nullptr;

    private:
        static constexpr const char* ANIMATION_START_TAG        = "QTE Note Start";
        static constexpr const char* ANIMATION_END_TAG          = "QTE Note End";
    };
}

