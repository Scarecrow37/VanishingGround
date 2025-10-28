#include "QTE/System/QTESystem.h"
#include "QTE/Track/QTETrack.h"
#include "QTEJudgeEffectUI.h"
#include "UI/Elements/Image/ImageElement.h"
#include "UI/Elements/SpriteAnimation/SpriteAnimationElement.h"
#include "UI/Panels/Overlay/OverlayPanel.h"
#include "pchScripts.h"

namespace QTE
{
    JudgeEffectUI::JudgeEffectUI(const File::Guid& prefab, Transform* parent)
    {
        SpawnObject(prefab, parent);
    }
    JudgeEffectUI::~JudgeEffectUI() = default;

    void JudgeEffectUI::SpawnObject(const File::Guid& prefab, Transform* parent)
    {
        auto sharedObject = UmGameObjectFactory.DeserializeToGuid(prefab);
        if (GameObject* clone = sharedObject.get())
        {
            Overlay              = clone->GetComponent<OverlayPanel>();
            Transform& transform = clone->transform;
            if (parent)
            {
                transform.SetParent(parent);
            }
            Transform::ForeachBFS(transform, [this](Transform* child) {
                GameObject& childObject = child->gameObject;
                if (childObject.CompareTag(ANIMATION_MISS_TAG))
                {
                    MissEffect = childObject.GetComponent<SpriteAnimationElement>();
                }
                else if (childObject.CompareTag(ANIMATION_NORMAL_TAG))
                {
                    NormalEffect = childObject.GetComponent<SpriteAnimationElement>();
                }
                else if (childObject.CompareTag(ANIMATION_PERFECT_TAG))
                {
                    PerfectEffect = childObject.GetComponent<SpriteAnimationElement>();
                }
            });
        }
    }

    void JudgeEffectUI::Reset()
    {
        if (MissEffect)
        {
            MissEffect->Setup();
            MissEffect->gameObject->ActiveSelf = false;
        }
        if (NormalEffect)
        {
            NormalEffect->Setup();
            NormalEffect->gameObject->ActiveSelf = false;
        }
        if (PerfectEffect)
        {
            PerfectEffect->Setup();
            PerfectEffect->gameObject->ActiveSelf = false;
        }
        if (Overlay)
        {
            Overlay->Point = POINT(-LONG_MAX, 0); // 화면 밖으로 이동
            Overlay->gameObject->ActiveSelf = false;
        }
    }
    void JudgeEffectUI::OnNotePressed(const QTE::NoteResult& resultType, float posX)
    {
        if (resultType.IsPressedButton())
        {
            if (SpriteAnimationElement* effect = GetSpriteAnimation(resultType.Result))
            {
                effect->gameObject->ActiveSelf = true;
                effect->StartAnimation();
            }
            if (Overlay)
            {
                const POINT oldPoint            = Overlay->Point;
                const LONG  posXLong            = static_cast<LONG>(posX);
                Overlay->Point                  = POINT{posXLong, oldPoint.y};
                Overlay->gameObject->ActiveSelf = true;
            }
        }
    }
    SpriteAnimationElement* JudgeEffectUI::GetSpriteAnimation(QTE::ResultType result)
    {
        switch (result)
        {
        case QTE::QTE_RESULT_PERFECT:
            return PerfectEffect;
            break;
        case QTE::QTE_RESULT_NORMAL:
            return NormalEffect;
            break;
        case QTE::QTE_RESULT_MISS:
            return MissEffect;
            break;
        default:
            break;
        }
        return nullptr;
    }
} // namespace QTE