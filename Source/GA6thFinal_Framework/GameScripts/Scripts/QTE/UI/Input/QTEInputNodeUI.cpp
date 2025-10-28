#include "pchScripts.h"
#include "QTEInputNodeUI.h"
#include "UI/Panels/Overlay/OverlayPanel.h"
#include "UI/Elements/Image/ImageElement.h"

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
                for (int i = 0; i < BUTTON_COUNT; ++i)
                {
                    if (childObject.CompareTag(BUTTON_IMAGE_TAG[i]))
                    {
                        ButtonImage[i] = childObject.GetComponent<ImageElement>();
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
        for (int i = 0; i < BUTTON_COUNT; ++i)
        {
            if (ButtonImage[i])
            {
                ButtonImage[i]->Alpha = alpha;
            }
        }
    }
    void InputNodeUI::Reset()
    {
        for (int i = 0; i < BUTTON_COUNT; ++i)
        {
            if (ButtonImage[i])
            {
                ButtonImage[i]->gameObject->ActiveSelf = false;
            }
        }
        Active(false);
        Alpha(1.0f);
    }
    void InputNodeUI::Show(Input::Controller::Button button)
    {
        Active(true);
        switch (button)
        {
        case Input::Controller::Button::X: {
            if (ButtonImage[X])
            {
                ButtonImage[X]->gameObject->ActiveSelf = true;
            }
            break;
        }
        case Input::Controller::Button::Y: {
            if (ButtonImage[Y])
            {
                ButtonImage[Y]->gameObject->ActiveSelf = true;
            }
            break;
        }
        case Input::Controller::Button::B: {
            if (ButtonImage[B])
            {
                ButtonImage[B]->gameObject->ActiveSelf = true;
            }
            break;
        }
        default:
            break;
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

