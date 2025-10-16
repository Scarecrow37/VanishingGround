#include "pchScripts.h"
#include "ArtifactButtonNavi.h"
#include "UI/Elements/Image/ImageElement.h"
#include "ItemDropSystem/UI/ArtifactUIManager.h"
#include "ItemDropSystem/UI/ItemInfoUIManager.h"
#include "ItemDropSystem/UI/ItemDropUIRootManager.h"
#include "ItemDropSystem/UI/WeaponChangeUIManager.h"
#include "WeaponSystem/WeaponTable/WeaponTableComponent.h"
#include "RevelationSystem/RevelationSystem.h"
#include "AccessorySystem/AccessorySystem.h"

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

std::shared_ptr<ImageElement> ArtifactButtonNavi::FindFocusImage()
{
    ImageElement* focusImage = GetComponent<ImageElement>();
    if (focusImage)
    {
        auto component = focusImage->GetWeakPtr().lock();
        if (component)
        {
            std::shared_ptr<ImageElement> focusImage = std::static_pointer_cast<ImageElement>(component);
            _focusImage = focusImage;
            return focusImage;
        }
    }
    return nullptr;
}

void ArtifactButtonNavi::Awake() 
{
    Base::Awake();
}

void ArtifactButtonNavi::FocusIn(FocusCallType type)
{ 
    if (true == EnableInHierarchy)
    {
        Base::FocusIn(type);
        auto focus = _focusImage.lock();
        if (nullptr == focus)
        {
            focus = FindFocusImage();
        }
        if (focus)
        {        
            focus->Enable = true;
    
            // UI 설정
            if (ItemInfoUIManager* infoManager = SingletonComponent<ItemInfoUIManager>::GetInstance())
            {
                infoManager->SetItemInfoUI(_itemInfo);
            }
        }
        LastFocusIndex = _buttonIndex; // 마지막 포커스된 버튼 인덱스
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
            const std::string& itemName = _itemInfo.Name;
            switch (_itemInfo.Category)
            {
            case ArtifactDropType::SWORD:
            case ArtifactDropType::DAGGER:
            case ArtifactDropType::WARHAMMER:
                if (WeaponChangeUIManager* changeManager = SingletonComponent<WeaponChangeUIManager>::GetInstance())
                {
                    changeManager->ShowWeaponChangeUI(_itemInfo.Name);
                }
                break;
            case ArtifactDropType::ACCESSORY:
                if (AccessorySystem* accessoryManager = SingletonComponent<AccessorySystem>::GetInstance())
                {
                    std::unique_ptr<AccessoryElement> element = accessoryManager->TryMakeAccessoryToName(itemName);
                    if (element)
                    {
                        accessoryManager->EquipAccessory(*element);
                    }
                    manager->ObtainFocusNavi(_buttonIndex);
                }
                break;
            case ArtifactDropType::REVELATION:
                if (RevelationSystem* revelationManager = SingletonComponent<RevelationSystem>::GetInstance())
                {
                    RevelationElement* element = revelationManager->FindElement(itemName);
                    if (element)
                    {
                        revelationManager->PushBackRevelation(*element);
                    }
                    manager->ObtainFocusNavi(_buttonIndex);
                }
                break;
            default:
                manager->ObtainFocusNavi(_buttonIndex);
                break;
            }
        }
    }
}

void ArtifactButtonNavi::FocusOut(FocusCallType type) 
{
    Base::FocusOut(type);
    auto focus = _focusImage.lock();
    if (nullptr == focus)
    {
        focus = FindFocusImage();
    }
    if (focus)
    {
        focus->Enable = false;
    }
}
