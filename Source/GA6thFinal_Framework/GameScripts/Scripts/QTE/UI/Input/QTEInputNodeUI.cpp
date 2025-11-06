#include "pchScripts.h"
#include "QTEInputNodeUI.h"
#include "UI/Panels/Overlay/OverlayPanel.h"
#include "UI/Elements/Image/ImageElement.h"
#include "UI/Elements/SpriteAnimation/SpriteAnimationElement.h"

namespace QTE
{
    InputNodeUI::InputNodeUI(const File::Guid& prefab, Transform* parent)
    {
        SpawnObject(prefab, parent);
    }
    InputNodeUI::~InputNodeUI() = default;
    
    void InputNodeUI::SpawnObject(const File::Guid& prefab, Transform* parent) 
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
                for (int button = 0; button < BUTTON_COUNT; ++button)
                {
                    if (childObject.CompareTag(BUTTON_IMAGE_TAG[button]))
                    {
                        for (int judge = 0; judge < JUDGE_COUNT; ++judge)
                        {
                            if (Transform* judgeChild = child->GetChild(judge))
                            {
                                if (judgeChild->gameObject->CompareTag(BUTTON_JUDGE_TAG[judge]))
                                {
                                    ButtonImage[button][judge] =
                                        judgeChild->gameObject->GetComponent<SpriteAnimationElement>();
                                }
                            }
                        }
                    }
                }
            });
        }
    }

    void InputNodeUI::Active(bool active)
    {
        if (Overlay)
        {
            Overlay->gameObject->ActiveSelf = active;
        }
    }
    void InputNodeUI::Alpha(float alpha)
    {
        if (ButtonImage[InputButton][InputJudge])
        {
            ButtonImage[InputButton][InputJudge]->Alpha = std::clamp(alpha, 0.0f, 1.0f);
        }
    }
    void InputNodeUI::Reset()
    {
        InputButton = X;
        InputJudge  = MISS;
        for (int i = 0; i < BUTTON_COUNT; ++i)
        {
            for (int j = 0; j < JUDGE_COUNT; ++j)
            {
                if (ButtonImage[i][j])
                {
                    ButtonImage[i][j]->gameObject->ActiveSelf = false;
                }
            }
        }
        Active(false);
        Alpha(1.0f);
    }
    void InputNodeUI::Show(Input::Controller::Button button, QTE::ResultType result)
    {
        Active(true);

        switch (button)
        {
        case Input::Controller::Button::X: 
            InputButton = X;
            break;
        case Input::Controller::Button::Y:
            InputButton = Y;
            break;
        case Input::Controller::Button::B:
            InputButton = B;
            break;
        default:
            break;
        }

        switch (result)
        {
        case QTE::QTE_RESULT_MISS:
            InputJudge = MISS;
            break;
        case QTE::QTE_RESULT_NORMAL:
            InputJudge = GOOD;
            break;
        case QTE::QTE_RESULT_PERFECT:
            InputJudge = PERFECT;
            break;
        default:
            break;
        }

        if (ButtonImage[InputButton][InputJudge])
        {
            ButtonImage[InputButton][InputJudge]->Alpha                  = 1.0f;
            ButtonImage[InputButton][InputJudge]->gameObject->ActiveSelf = true;
            ButtonImage[InputButton][InputJudge]->Setup();
            ButtonImage[InputButton][InputJudge]->StartAnimation();
        }
    }
    void InputNodeUI::SetParent(Transform* parent) 
    {
        if (Overlay)
        {
            Transform& transform = Overlay->gameObject->transform;
            transform.SetParent(parent);
        }
    }
}

