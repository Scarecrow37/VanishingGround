#include "pchScripts.h"
#include "ArtifactButtonNavi.h"
#include "UI/Elements/Image/ImageElement.h"
#include "ItemDropSystem/UI/ArtifactUIManager.h"
#include "ItemDropSystem/UI/ItemInfoUIManager.h"
#include "ItemDropSystem/UI/ItemDropUIRootManager.h"

UMREAL_COMPONENT(ArtifactButtonNavi)

ArtifactButtonNavi::ArtifactButtonNavi()
{
    _itemInfo.Category = ArtifactDropType::ERASE_REVELATION;
    _itemInfo.ID       = 0;
    _itemInfo.Name     = (const char*)u8"계시 지우기";
    _buttonIndex       = -1;
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
            component->Enable = false; //focus는 전부 꺼둬야함
        }
    }
}

void ArtifactButtonNavi::FocusIn(FocusCallType type)
{ 
    LastFocusIndex = _buttonIndex; // 마지막 포커스된 버튼 인덱스
    if (true == EnableInHierarchy)
    {
        Base::FocusIn(type);
        if (auto focus = _focusImage.lock())
        {
            focus->Enable = true;
    
            // UI 설정
            if (ItemInfoUIManager* infoManager = SingletonComponent<ItemInfoUIManager>::GetInstance())
            {
                infoManager->SetItemInfoUI(_itemInfo);
            }
        }
    }
    else
    {
        if (ItemDropUIRootManager* rootManager = SingletonComponent<ItemDropUIRootManager>::GetInstance())
        {
            rootManager->AutoFocus();
        }
    }  
}

void ArtifactButtonNavi::Submit()
{
    Base::Submit();
    if (Enable)
    {
        if (ArtifactUIManager* manager = SingletonComponent<ArtifactUIManager>::GetInstance())
        {         
            manager->DisableFocusNavi(_buttonIndex);
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
