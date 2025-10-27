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
                else if (childObject.CompareTag(ANIMATION_MISS_TAG))
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

    void NoteUI::SetPositionX(float posX) 
    {
        if (Overlay)
        {
            const POINT oldPoint = Overlay->Point;
            const LONG  posXLong = static_cast<LONG>(posX);
            Overlay->Point = POINT{posXLong, oldPoint.y};
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

    void NoteUI::Update(const float currTime, const float travelTime, const float currSpeed, const float startX,
                        const float endX, const float perfectX, const float offsetX)
    {
        if (State == STATE_AVAILABLE)
        {
            return;
        }
        const float deltaTime = Time - currTime;
        const float noteWidth = GetNoteWidth();
        // 노트 위치 가중치를 구한다. 0 이하면 나타나기 전, 1 이상이면 퍼펙트 지점을 넘었다는 것.
        const float posXFactor = Math::CalculateNotePosXFactor(deltaTime, currSpeed, travelTime);
        // 주의: end 지점을 PerfectX로 한다.
        const float posXValue = Math::CalculateNotePosX(posXFactor, startX, perfectX);
        switch (State)
        {
        case STATE_WAIT: {
            OnWaitUpdate();
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
            {   // 최종 값은 EndX값을 넘지 않는 X값에 오프셋을 더한 값.
                const SIZE  size      = Overlay->Size;
                const float finalXPos = std::min(posXValue, endX) + offsetX - static_cast<float>(size.cx / 2);
                SetPositionX(finalXPos); // 위치 설정
            }
            OnVisibleUpdate();
            // X값이 EndX값을 넘었거나, 결과가 생긴 노트는 Dead처리
            if (posXValue >= endX ||
                Result != QTE::QTE_RESULT_NONE)
            {
                State = STATE_DEAD;
                OnNoteExit();
            }
            break;
        }
        case STATE_DEAD: {
            OnDeadUpdate();
            break;
        }
        default:
            break;
        }
    }
    
    void NoteUI::OnNotePressed(QTE::ResultType resultType) 
    {
        Result = resultType;
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
        // 어차피 결과가 없으면 effect는 nullptr이므로 재생되지 않음.
        if (SpriteAnimationElement* effectAnimation = GetSpriteAnimation())
        {
            effectAnimation->gameObject->ActiveSelf = true;
            effectAnimation->StartAnimation();
        }
    }
    void NoteUI::OnWaitUpdate() 
    {
    }
    void NoteUI::OnVisibleUpdate() 
    {
    }
    void NoteUI::OnDeadUpdate() 
    {
        if (EndAnimation)
        {
            // 애니메이션 끝났는지 확인
            if (false == EndAnimation->IsPlaying)
            {
                EndAnimation->gameObject->ActiveSelf = false;
            }
        }
        if (SpriteAnimationElement* effectAnimation = GetSpriteAnimation())
        {
            // 애니메이션 끝났는지 확인
            if (false == effectAnimation->IsPlaying)
            {
                effectAnimation->gameObject->ActiveSelf = false;
            }
        }
    }

    SpriteAnimationElement* NoteUI::GetSpriteAnimation()
    {
        switch (Result)
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
}