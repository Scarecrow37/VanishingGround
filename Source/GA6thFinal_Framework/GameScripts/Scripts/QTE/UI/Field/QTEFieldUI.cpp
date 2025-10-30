#include "pchScripts.h"
#include "QTEFieldUI.h"
#include "UI/Panels/Overlay/OverlayPanel.h"
#include "UI/Elements/SpriteAnimation/SpriteAnimationElement.h"

namespace QTE
{
    void FieldUI::Initialize(File::Guid noteGuid, File::Guid effectGuid, size_t poolSize)
    {
        NotePool.clear();
        EffectPool.clear();
        if (Overlay)
        {
            Transform& parent = Overlay->transform;
            for (int i = 0; i < poolSize; ++i)
            {
                NotePool.emplace_back(noteGuid, &parent);
                EffectPool.emplace_back(effectGuid, &parent);
            }
        }
    }
    void FieldUI::MatchUIFromObject(GameObject& object) 
    {
        if (object.CompareTag(OVERLAY_TAG))
        {
            Overlay = object.GetComponent<OverlayPanel>();
        }
        if (object.CompareTag(LINE_TAG))
        {
            Line = object.GetComponent<ImageElement>();
        }
        if (object.CompareTag(FLOW_TAG))
        {
            Flow = object.GetComponent<SpriteAnimationElement>();
        }
        if (object.CompareTag(JUDGE_TAG))
        {
            JudgeNote = object.GetComponent<SpriteAnimationElement>();
        }
        if (object.CompareTag(START_ANIMATION_TAG))
        {
            StartAnimation = object.GetComponent<SpriteAnimationElement>();
        }
    }
    void FieldUI::Active(bool active)
    {
        if (Overlay)
        {
            Overlay->gameObject->ActiveSelf = active;
        }
    }
    void FieldUI::Alpha(float alpha)
    {
        if (Line)
        {
            Line->Alpha = alpha;
        }
        if (Flow)
        {
            Flow->Alpha = alpha;
        }
        if (JudgeNote)
        {
            JudgeNote->Alpha = alpha;
        }
        if (StartAnimation)
        {
            StartAnimation->Alpha = alpha;
        }
    }
    void FieldUI::Reset()
    {
        if (StartAnimation)
        {
            StartAnimation->Setup();
            StartAnimation->gameObject->ActiveSelf = false;
        }
        if (JudgeNote)
        {
            JudgeNote->Setup();
            JudgeNote->gameObject->ActiveSelf = false;
        }
        if (Flow)
        {
            Flow->Setup();
            Flow->gameObject->ActiveSelf = false;
        }
        if (Line)
        {
            Line->gameObject->ActiveSelf = false;
        }
        for (auto& noteUI : NotePool)
        {
            noteUI.Reset();
        }
        for (auto& effectUI : EffectPool)
        {
            effectUI.Reset();
        }
    }
    void FieldUI::OnQTEEnter()
    {
        if (StartAnimation)
        {
            StartAnimation->gameObject->ActiveSelf = true;
            StartAnimation->StartAnimation();
        }
    }
    void FieldUI::Update()
    {
        if (StartAnimation)
        {
            if (false == StartAnimation->IsPlaying)
            {
                StartAnimation->gameObject->ActiveSelf = false;
                if (Line)
                {
                    Line->gameObject->ActiveSelf = true;
                }
                if (JudgeNote)
                {
                    JudgeNote->gameObject->ActiveSelf = true;
                }
                if (Flow)
                {
                    Flow->gameObject->ActiveSelf = true;
                    Flow->StartAnimation();
                }
            }
        }
    }
    void FieldUI::OnButtonPressed() 
    {
        if (JudgeNote)
        {
            JudgeNote->Setup();
            JudgeNote->StartAnimation();
        }
    }

} // namespace QTE