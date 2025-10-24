#include "pchScripts.h"
#include "StageVigneeteUIComponent.h"
#include "UI/Elements/Image/ImageElement.h"
#include "ItemDropSystem/ItemDropSystem.h"

UMREAL_COMPONENT(StageVigneeteUIComponent)

StageVigneeteUIComponent::StageVigneeteUIComponent() = default;
StageVigneeteUIComponent::~StageVigneeteUIComponent() = default;

void StageVigneeteUIComponent::SetActiveVigneeteImages(bool value) 
{
    for (auto& image : _vigneeteImages)
    {
        image->Enable = value;
    }
}

void StageVigneeteUIComponent::Awake() 
{
    FindImages();
    SetActiveVigneeteImages(false);
}

void StageVigneeteUIComponent::Start() 
{
    UpdateVigneete();
}

void StageVigneeteUIComponent::FindImages() 
{
    _vigneeteImages.clear();
    for (int i = 0; i < transform->ChildCount; i++)
    {
        if (Transform* child = transform->GetChild(i))
        {
            if (ImageElement* image = child->gameObject->GetComponent<ImageElement>())
            {
                _vigneeteImages.push_back(image);
            }
        }
    }
}

void StageVigneeteUIComponent::UpdateVigneete() 
{
    if (ItemDropSystem* system = SingletonComponent<ItemDropSystem>::GetInstance())
    {
        SetActiveVigneeteImages(false);
        int index = system->StageClearCount;
        if (index < _vigneeteImages.size())
        {
            _vigneeteImages[index]->Enable = true;
        }
    }
}
