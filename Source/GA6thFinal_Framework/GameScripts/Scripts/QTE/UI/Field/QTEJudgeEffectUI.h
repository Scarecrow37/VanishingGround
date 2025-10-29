#pragma once
#include "QTE/Result/QTEResult.h"

class OverlayPanel;
class ImageElement;
class SpriteAnimationElement;

namespace QTE
{
    class JudgeEffectUI
    {
    public:
        JudgeEffectUI(const File::Guid& prefab, Transform* parent = nullptr);
        ~JudgeEffectUI();

    private:
        void SpawnObject(const File::Guid& prefab, Transform* parent);

    public:
        void Reset();

        void OnNotePressed(const QTE::NoteResult& resultType, float posX);

        SpriteAnimationElement* GetSpriteAnimation(QTE::ResultType result);

    public:
        QTE::ResultType         Result          = QTE::QTE_RESULT_NONE;
        OverlayPanel*           Overlay         = nullptr;
        SpriteAnimationElement* MissEffect      = nullptr;
        SpriteAnimationElement* NormalEffect    = nullptr;
        SpriteAnimationElement* PerfectEffect   = nullptr;

    private:
        static constexpr const char* ANIMATION_MISS_TAG    = "QTE Effect Miss";
        static constexpr const char* ANIMATION_NORMAL_TAG  = "QTE Effect Normal";
        static constexpr const char* ANIMATION_PERFECT_TAG = "QTE Effect Perfect";
    };
} // namespace QTE
