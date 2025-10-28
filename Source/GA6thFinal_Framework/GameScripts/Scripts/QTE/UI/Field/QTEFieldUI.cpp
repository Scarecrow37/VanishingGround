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
        Overlay = object.CompareTag(OVERLAY_TAG) 
            ? object.GetComponent<OverlayPanel>() 
            : Overlay;

        Line = object.CompareTag(LINE_TAG) 
            ? object.GetComponent<ImageElement>() 
            : Line;

        Flow = object.CompareTag(FLOW_TAG) 
            ? object.GetComponent<SpriteAnimationElement>() 
            : Flow;

        JudgeNote = object.CompareTag(JUDGE_TAG) 
            ? object.GetComponent<SpriteAnimationElement>() 
            : JudgeNote;

        StartAnimation = object.CompareTag(START_ANIMATION_TAG) 
            ? object.GetComponent<SpriteAnimationElement>() 
            : StartAnimation;
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
        if (Flow)
        {
            Flow->gameObject->ActiveSelf = true;
            Flow->StartAnimation();
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