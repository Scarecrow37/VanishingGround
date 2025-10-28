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
                const SIZE  size      = Overlay->Size;
                const POINT oldPoint  = Overlay->Point;
                const float half      = static_cast<float>(-size.cx / 2);
                const float finalXPos = std::min(posXValue, endX) + half;
                const LONG  posXLong  = static_cast<LONG>(finalXPos) - size.cx / 2;
                Overlay->Point        = POINT{posXLong, oldPoint.y};
                {
                    const float hideX  = perfectX + 300.0f;
                    const float dist   = hideX - perfectX;
                    const float delta  = hideX - finalXPos;
                    const float factor = std::clamp(delta / dist, 0.0f, 1.0f);
                    Alpha(std::clamp(factor, 0.0f, 1.0f));
                }
              
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