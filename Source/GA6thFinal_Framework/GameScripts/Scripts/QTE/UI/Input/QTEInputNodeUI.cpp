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

                if (childObject.CompareTag(BUTTON_IMAGE_TAG))
                {
                    ButtonImage = childObject.GetComponent<ImageElement>();
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
        if (ButtonImage)
        {
            ButtonImage->Alpha = alpha;
        }
    }
    void InputNodeUI::Reset()
    {
        Active(false);
        Alpha(1.0f);
    }
    void InputNodeUI::SetParent(Transform* parent) 
    {
        if (Overlay)
        {
            Transform& transform = Overlay->gameObject->transform;
            transform.SetParent(parent);
        }
    }
    void InputNodeUI::SetImage(const File::Guid& guid)
    {
        if (ButtonImage)
        {
            ButtonImage->SetImage(guid);
        }
    }
}

