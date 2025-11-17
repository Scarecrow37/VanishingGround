#include "pchScripts.h"
#include "QTENoteUI.h"
#include "QTE/System/QTESystem.h"
#include "QTE/Track/QTETrack.h"
#include "UI/Panels/Overlay/OverlayPanel.h"
#include "UI/Elements/Image/ImageElement.h"
#include "UI/Elements/SpriteAnimation/SpriteAnimationElement.h"

namespace QTE
{
    NoteUI::NoteUI(const File::Guid& prefab, Transform* parent)
    {
        SpawnObject(prefab, parent);
    }
    NoteUI::~NoteUI() = default;

    void NoteUI::SpawnObject(const File::Guid& prefab, Transform* parent) 
    {
        auto sharedObject = UmGameObjectFactory.DeserializeToGuid(prefab);
        if (GameObject* clone = sharedObject.get())
        {
            Overlay = clone->GetComponent<OverlayPanel>();
            Transform& transform = clone->transform;
            if (parent)
            {
                transform.SetParent(parent);
            }
            Transform::ForeachBFS(transform, [this](Transform* child) {
                GameObject& childObject = child->gameObject;

                if (childObject.CompareTag(ANIMATION_START_TAG))
                {
                    StartAnimation = childObject.GetComponent<SpriteAnimationElement>();
                }
                else if (childObject.CompareTag(ANIMATION_END_TAG))
                {
                    EndAnimation = childObject.GetComponent<SpriteAnimationElement>();
                }
            });
        }
    }

    float NoteUI::GetNoteWidth()
    {
        if (Overlay)
        {
            const SIZE  noteSize  = Overlay->Size;
            const float noteWidth = static_cast<float>(noteSize.cx);
            return noteWidth;
        }
        return 0.0f;
    }

    float NoteUI::SetNotePositionX(const float positionX)
    {
        const POINT oldPoint  = Overlay->Point;
        const float halfWidth = GetNoteWidth() * 0.5f;
        const float finalPosX = positionX - halfWidth;
        Overlay->Point        = POINT{static_cast<LONG>(finalPosX), oldPoint.y};
        return finalPosX;
    }

    void NoteUI::ProcessAlpha(const float positionX, const float fadeInStartX, const float fadeInEndX,
                              const float fadeOutStartX, const float fadeOutEndX) 
    {
        float alpha = 0.0f;
        // FadeIn
        if (fadeInEndX > 0.0f && positionX >= fadeInStartX && positionX <= fadeInEndX)
        {
            const float dist   = fadeInEndX - fadeInStartX;
            const float delta  = fadeInEndX - positionX;
            alpha              = std::clamp(delta / dist, 0.0f, 1.0f);
        }
        else if (positionX > fadeInEndX && positionX < fadeOutStartX)
        {
            alpha = 1.0f;
        }
        // FadeOut
        else if (fadeOutStartX > fadeInEndX && positionX >= fadeOutStartX)
        {
            const float dist   = fadeOutEndX - fadeOutStartX;
            const float delta  = fadeOutEndX - positionX;
            alpha              = std::clamp(delta / dist, 0.0f, 1.0f);
        }
        Alpha(alpha);
    }

    void NoteUI::Reset()
    {
        Time   = 0.0f;
        Result = QTE::QTE_RESULT_NONE;
        State  = STATE_AVAILABLE;

        if (Overlay)
        {
            Overlay->Point = POINT(-LONG_MAX, 0); // 화면 밖으로 이동
            Overlay->gameObject->ActiveSelf = false;
        }
        if (StartAnimation)
        {
            StartAnimation->Setup();
            StartAnimation->gameObject->ActiveSelf = false;
        }
        if (EndAnimation)
        {
            EndAnimation->Setup();
            EndAnimation->gameObject->ActiveSelf = false;
        }
    }

    bool NoteUI::IsAvailable()
    {
        return State == STATE_AVAILABLE;
    }

    bool NoteUI::TrySetup(const float noteTime)
    {
        if (STATE_AVAILABLE == State)
        {
            State = STATE_WAIT;
            Time  = noteTime; 
            return true;
        }
        return false;
    }
    void NoteUI::Alpha(float alpha)
    {
        if (StartAnimation)
        {
            StartAnimation->Alpha = alpha;
        }
        if (EndAnimation)
        {
            EndAnimation->Alpha = alpha;
        }
    }
    void NoteUI::Update(const float currTime, const float travelTime, const float currSpeed, 
                        const float startX, const float endX, const float perfectX,
                        float fadeInStartX /*= FLT_MIN*/ , float fadeInEndX /*= FLT_MAX*/,
                        float fadeOutStartX /*= FLT_MIN*/ , float fadeOutEndX /*= FLT_MAX*/,
                        const float offsetX /*= 0.0f*/)
    {
        if (State == STATE_AVAILABLE)
        {
            return;
        }
        const float deltaTime = Time - currTime;
        
        // 노트 위치 가중치를 구한다. 0 이하면 나타나기 전, 1 이상이면 퍼펙트 지점을 넘었다는 것.
        const float posXFactor = Math::CalculateNotePosXFactor(deltaTime, currSpeed, travelTime);
        // 주의: end 지점을 PerfectX로 한다.
        const float posXValue = perfectX * posXFactor;

        switch (State)
        {
        case STATE_WAIT: {
            if (posXFactor >= 0.0f)
            {
                State = STATE_VISIBLE;
                Overlay->gameObject->ActiveSelf = true;
                OnNoteEnter();
            }
            break;
        }
        case STATE_VISIBLE: {
            if (Overlay)
            {
                const float finalPosX = SetNotePositionX(posXValue + offsetX);
                // fade start 지점이 기본 값(FLT_MIN)이거나 이상한 값이면 0.0f로 대체
                if (fadeInStartX < startX || fadeInStartX > endX)
                {
                    fadeInStartX = startX;
                }
                // fade end 지점이 기본 값(FLT_MAX)이거나 이상한 값이면 0.0f로 대체
                if (fadeInEndX > endX || fadeInEndX < startX)
                {
                    fadeInEndX = fadeInStartX;
                }
                // fade start 지점이 기본 값(FLT_MIN)이거나 이상한 값이면 perfectX 값으로 대체
                if (fadeOutStartX < startX || fadeOutStartX > endX)
                {
                    fadeOutStartX = perfectX;
                }
                // fade end 지점이 기본 값(FLT_MAX)이거나 이상한 값이면 endX 값으로 대체
                if (fadeOutEndX > endX || fadeOutEndX < startX)
                {
                    fadeOutEndX = endX;
                }
                ProcessAlpha(finalPosX, fadeInStartX, fadeInEndX, fadeOutStartX, fadeOutEndX);
            }
            // 결과가 생긴 노트는 Dead처리
            if (Result != QTE::QTE_RESULT_NONE)
            {
                State = STATE_DEAD;
                OnNoteExit();
            }
            
            break;
        }
        case STATE_DEAD: {
            if (EndAnimation)
            {
                // 애니메이션 끝났는지 확인
                if (false == EndAnimation->IsPlaying)
                {
                    EndAnimation->gameObject->ActiveSelf = false;
                }
            }
            break;
        }
        default:
            break;
        }
    }
    
    void NoteUI::OnNotePressed(const QTE::NoteResult& resultType) 
    {
        if (resultType.IsPressedButton())
        {
            Result = resultType.Result;
        }
    }
    
    void NoteUI::OnNoteEnter() 
    {
        if (StartAnimation)
        {
            StartAnimation->gameObject->ActiveSelf = true;
            StartAnimation->StartAnimation();
        }
    }
    void NoteUI::OnNoteExit() 
    {
        if (EndAnimation)
        {
            EndAnimation->gameObject->ActiveSelf = true;
            EndAnimation->StartAnimation();
            if (StartAnimation)
            {
                StartAnimation->gameObject->ActiveSelf = false;
            }
        }
    }
}