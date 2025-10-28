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

        void SetPositionX(float posX);

        float GetNoteWidth();

    public:
        void Alpha(float alpha);

        bool IsAvailable();
        
        void Reset();

        bool TrySetup(const float noteTime = 0.0f);

        void Update(const float currTime, const float travelTime, const float currSpeed, const float startX, const float endX, const float perfectX, const float offsetX = 0.0f);

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

