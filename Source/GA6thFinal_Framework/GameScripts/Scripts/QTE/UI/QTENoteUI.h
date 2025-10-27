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
        /// <summary>프리팹을 기반으로 노트 오브젝트를 스폰합니다.</summary>
        /// <param name="_prefab">프리팹의 Guid</param>
        /// <param name="parent">부모가 될 Transform</param>
        void SpawnObject(const File::Guid& prefab, Transform* parent);

        void SetPositionX(float posX);

        float GetNoteWidth();

    public:
        bool IsAvailable();
        
        void Reset();

        bool TrySetup(const float noteTime = 0.0f);

        void Update(const float currTime, const float travelTime, const float currSpeed, const float startX, const float endX, const float perfectX, const float offsetX = 0.0f);

        void OnNotePressed(QTE::ResultType resultType);

    private:
        // 노트가 트랙에 나타나기 시작해야할 때
        void OnNoteEnter();
        // 노트가 트랙에서 사라져야할 때
        void OnNoteExit();

        void OnWaitUpdate();
        void OnVisibleUpdate();
        void OnDeadUpdate();

        SpriteAnimationElement* GetSpriteAnimation();

    public:
        float                   Time            = 0.0f;
        UIState                 State           = STATE_AVAILABLE;
        QTE::ResultType         Result          = QTE::QTE_RESULT_NONE;

        OverlayPanel*           Overlay         = nullptr;
        SpriteAnimationElement* StartAnimation  = nullptr;
        SpriteAnimationElement* EndAnimation    = nullptr;

        SpriteAnimationElement* MissEffect      = nullptr;
        SpriteAnimationElement* NormalEffect    = nullptr;
        SpriteAnimationElement* PerfectEffect   = nullptr;

    private:
        static constexpr const char* ANIMATION_START_TAG        = "QTE Note Start";
        static constexpr const char* ANIMATION_END_TAG          = "QTE Note End";
        static constexpr const char* ANIMATION_MISS_TAG         = "QTE Effect Miss";
        static constexpr const char* ANIMATION_NORMAL_TAG       = "QTE Effect Normal";
        static constexpr const char* ANIMATION_PERFECT_TAG      = "QTE Effect Perfect";
    };
}

