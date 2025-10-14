#include "pchScripts.h"
#include "ArtifactButtonNavi.h"
#include "UI/Elements/Image/ImageElement.h"
#include "ItemDropSystem/UI/ArtifactUIManager.h"

UMREAL_COMPONENT(ArtifactButtonNavi)

ArtifactButtonNavi::ArtifactButtonNavi()
{
    _itemInfo.Category = ArtifactDropType::ERASE_REVELATION;
    _itemInfo.ID       = 0;
    _itemInfo.Name     = (const char*)u8"계시 지우기";
}
ArtifactButtonNavi::~ArtifactButtonNavi() = default;

void ArtifactButtonNavi::SettingItem(const DropItemInfo& item) 
{
    _itemInfo = item;
}

void ArtifactButtonNavi::Awake() 
{
    Base::Awake();
    ImageElement* focusImage = GetComponent<ImageElement>();
    if (focusImage)
    {
        auto component = focusImage->GetWeakPtr().lock();
        if (component)
        {
            _focusImage = std::static_pointer_cast<ImageElement>(component);
        }
    }
}

void ArtifactButtonNavi::FocusIn(FocusCallType type)
{
    Base::FocusIn(type);
    if (auto focus = _focusImage.lock())
    {
        focus->Enable = true;
    }
}

void ArtifactButtonNavi::Submit()
{
    Base::Submit();
    if (Enable)
    {
        if (ArtifactUIManager* manager = SingletonComponent<ArtifactUIManager>::GetInstance())
        {
            Enable = false;
            for (size_t i = 0; i < ARTIFACT_DROP_COUNT; i++)
            {
                if (manager->FocusNavi(i))
                {
                    break;
                }
            }
        }
    }
}

void ArtifactButtonNavi::FocusOut(FocusCallType type) 
{
    Base::FocusOut(type);
    if (auto focus = _focusImage.lock())
    {
        focus->Enable = false;
    }
}
