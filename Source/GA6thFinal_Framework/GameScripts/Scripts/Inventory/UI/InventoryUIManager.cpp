#include "pchScripts.h"
#include "InventoryUIManager.h"
#include "ItemDropSystem/UI/ItemInfoUIManager.h"
#include "UI/Elements/Image/ImageElement.h"
#include "Inventory/UI/HorizontalPageUIManager.h"
#include "WeaponSystem/WeaponSystem.h"
#include "RevelationSystem/RevelationSystem.h"
#include "AccessorySystem/AccessorySystem.h"
#include "Inventory/UINavi/InventoryItemFocusNavi.h"

UMREAL_COMPONENT(InventoryUIManager)

InventoryUIManager::InventoryUIManager() 
    :
    _singletonComponent(this)
{
    _itemInfoUIManager = nullptr;
}

InventoryUIManager::~InventoryUIManager() = default;

void InventoryUIManager::ImGuiDrawPropertysEvent() 
{
    if (UmCore->IsPlay())
    {
        if (ImGui::Button("Open Inventory"))
        {
            OpenInventory();
        }    
        if (ImGui::Button("Close Inventory"))
        {
            CloseInventory();
        }    
    }
}

void InventoryUIManager::Added() 
{
    Base::Added();
    if (_singletonComponent.TrySingleTon())
    {
        BindInputAction(ControllerButton::B, Action::PRESSED, this, &InventoryUIManager::OnButtonB);
    }
}

void InventoryUIManager::Update() 
{
    if (_closeFlag)
    {
        CloseInventory();
    }
}

void InventoryUIManager::FindUIElements()
{
    //아이템 정보 UI 관리자
    if (Transform* itemInfoPanel = transform->FindWithTag("Item Info"))
    {
        _itemInfoUIManager = itemInfoPanel->gameObject->GetComponent<ItemInfoUIManager>();
    }

    _weaponsUI.clear();
    _consumableUI.clear();
    _revelationUI.Manager = nullptr;
    _revelationUI.Icons.clear();
    _accessoryUI.Manager = nullptr;
    _accessoryUI.Icons.clear();
    if (Transform* itemsPanel = transform->FindWithTag("Items Panel"))
    {
        for (size_t i = 0; i < itemsPanel->ChildCount; i++)
        {
            Transform* curr = itemsPanel->GetChild((int)i);
            if (curr)
            {
                GameObject& object = curr->gameObject;
                if (object.CompareTag("Weapon"))
                {
                    _weaponsUI   = FindIcons(curr);
                    _weaponsNavi = FindFocus(curr);
                }
                else if (object.CompareTag("Revelation"))
                {
                    if (_revelationUI.Manager = object.GetComponent<HorizontalPageUIManager>())
                    {
                        if (RevelationSystem* system = SingletonComponent<RevelationSystem>::GetInstance())
                        {
                            size_t horizontalCount = GetHorizontalPageCount(system->GetPlayerElementList().size());
                            _revelationUI.Manager->UpdateHorizontalUI(horizontalCount);
                        }  
                    }
                    _revelationUI.Icons   = FindIcons(curr);
                    _revelationUI.Navis   = FindFocus(curr);                          
                }
                else if (object.CompareTag("Accessory"))
                {
                    if (_accessoryUI.Manager = object.GetComponent<HorizontalPageUIManager>())
                    {
                        if (AccessorySystem* system = SingletonComponent<AccessorySystem>::GetInstance())
                        {
                            size_t horizontalCount = GetHorizontalPageCount(system->GetPlayerAccessoryItems().size());
                            _accessoryUI.Manager->UpdateHorizontalUI(horizontalCount);
                        }
                    }
                    _accessoryUI.Icons   = FindIcons(curr);
                    _accessoryUI.Navis   = FindFocus(curr);
                }
                else if (object.CompareTag("Consumable"))
                {
                    _consumableUI   = FindIcons(curr);
                    _consumableNavi = FindFocus(curr);
                }
            }
        }
    }
}

std::vector<ImageElement*> InventoryUIManager::FindIcons(Transform* tr)
{
    std::vector<ImageElement*> ui;
    Transform::ForeachBFS(*tr, [&ui](Transform* curr) 
    {
        GameObject& obj = curr->gameObject;
        if (obj.CompareTag("Icon"))
        {
            ImageElement* icon = obj.GetComponent<ImageElement>();
            if (icon)
            {
                ui.push_back(icon);
            }
        }
    });
    return ui;
}

std::vector<InventoryItemFocusNavi*> InventoryUIManager::FindFocus(Transform* tr)
{
    std::vector<InventoryItemFocusNavi*> navis;
    Transform::ForeachBFS(*tr, [&navis](Transform* curr) 
    {
        GameObject& obj = curr->gameObject;
        if (obj.CompareTag("Focus"))
        {         
            if (InventoryItemFocusNavi* navi = obj.GetComponent<InventoryItemFocusNavi>())
            {
                navis.push_back(navi);
            }
            if (ImageElement* image = obj.GetComponent<ImageElement>())
            {
                image->Enable = false;
            }
        }
    });
    return navis;
}

void InventoryUIManager::OnButtonB(const Input::Controller&) 
{
    if (true == gameObject->ActiveSelf)
    {
        _closeFlag = true;
    }
}

size_t InventoryUIManager::GetHorizontalPageCount(size_t artifactCount)
{
    size_t pageCount = 1;
    if (artifactCount == 0)
    {
        return 1;
    }
    pageCount = (artifactCount - 1) / 5 + 1;
    return pageCount;
}

void InventoryUIManager::OpenInventory(UINavigationComponent* lastFocus) 
{
    if (false == EnableInHierarchy)
    {
        gameObject->ActiveSelf = true;
        if (lastFocus)
        {
            _lastFocus = lastFocus->GetWeakPtr();
        }
        FindUIElements();
        UpdateWeaponUI();
        UpdateRevelationUI();
        UpdateAccessoryUI();
        UpdateConsumable();
        if (0 < _weaponsNavi.size())
        {
            _weaponsNavi[0]->Focus();
        }
    }  
}

void InventoryUIManager::CloseInventory() 
{
    gameObject->ActiveSelf = false;
    if (auto lastFocus = _lastFocus.lock())
    {
        UINavigationComponent* navi = static_cast<UINavigationComponent*>(lastFocus.get());
        navi->Focus();
    }
    _closeFlag = false;
}

void InventoryUIManager::UpdateWeaponUI()
{
    //무기 갱신
    if (WeaponSystem* weaponSystem = SingletonComponent<WeaponSystem>::GetInstance())
    {
        const auto& equipWeapons = weaponSystem->GetEquipWeapons();
        for (size_t i = 0; i < _weaponsUI.size(); i++)
        {
            const WeaponElement& weapon = equipWeapons[i];

            DropItemInfo      info  = weapon.GetItemInfo();
            int               id    = DropItemInfo::GetArtifactIconID(info);
            const File::Guid& guid  = UmFileSystem.GetGuidFromAssetID(id);
            ImageElement*     image = _weaponsUI[i];
            if (image)
            {
                image->SetImage(guid);
            }         

            if (i < _weaponsNavi.size())
            {
                _weaponsNavi[i]->SetItemInfo(info);
                _weaponsNavi[i]->Enable = true;
            }
        }
    }
}

void InventoryUIManager::UpdateRevelationUI(size_t startIndex)
{
    //계시 갱신
    if (RevelationSystem* system = SingletonComponent<RevelationSystem>::GetInstance())
    {
        const auto&   elements = system->GetPlayerElementList();
        size_t revelationCount = elements.size();
        if (_revelationUI.Manager)
        {
            size_t iconsCount = _revelationUI.Icons.size();
            if (0 < iconsCount)
            {
                for (size_t i = 0; i < iconsCount; i++)
                {
                    ImageElement* image = _revelationUI.Icons[i];
                    if (image)
                    {
                        const size_t elementIndex = i + startIndex;
                        if (elementIndex < elements.size())
                        {
                            const auto& revelation = elements[elementIndex];
                            if (revelation)
                            {
                                DropItemInfo      info = revelation->GetItemInfo();
                                int               id   = DropItemInfo::GetArtifactIconID(info);
                                const File::Guid& guid = UmFileSystem.GetGuidFromAssetID(id);
                                image->Enable          = true;
                                image->SetImage(guid);
                             
                                if (i < _revelationUI.Navis.size())
                                {
                                    _revelationUI.Navis[i]->SetItemInfo(info);
                                    _revelationUI.Navis[i]->Enable = true;
                                }
                            }
                        }
                        else
                        {
                            image->Enable = false;
                            if (i < _revelationUI.Navis.size())
                            {
                                _revelationUI.Navis[i]->Enable = false;
                            }
                        }
                    }
                }
            }
            else
            {
                for (size_t i = 0; i < _revelationUI.Icons.size(); i++)
                {
                    ImageElement* icon = _revelationUI.Icons[i];
                    if (icon)
                    {
                        icon->Enable = false;
                        if (i < _revelationUI.Navis.size())
                        {
                            _revelationUI.Navis[i]->Enable = false;
                        }
                    }
                }
            }
        }
    }
}

void InventoryUIManager::UpdateAccessoryUI(size_t startIndex)
{
    //장신구 갱신
    if (AccessorySystem* system = SingletonComponent<AccessorySystem>::GetInstance())
    {
        const auto& items          = system->GetPlayerAccessoryItems();
        size_t      accessoryCount = items.size();
        if (_accessoryUI.Manager)
        {
            size_t iconsCount = _accessoryUI.Icons.size();
            if (0 < iconsCount)
            {
                for (size_t i = 0; i < iconsCount; i++)
                {
                    ImageElement* image = _accessoryUI.Icons[i];
                    if (image)
                    {
                        const size_t itemIndex = i + startIndex;
                        if (itemIndex < items.size())
                        {
                            const auto&  accessory = items[itemIndex];
                            DropItemInfo info      = accessory.GetItemInfo();
                            int          id        = DropItemInfo::GetArtifactIconID(info);
                            File::Guid   guid      = UmFileSystem.GetGuidFromAssetID(id);
                            image->Enable          = true;
                            image->SetImage(guid);

                            if (i < _accessoryUI.Navis.size())
                            {
                                _accessoryUI.Navis[i]->SetItemInfo(info);
                                _accessoryUI.Navis[i]->Enable = true;
                            }
                        }
                        else
                        {
                            image->Enable = false;
                            if (i < _accessoryUI.Navis.size())
                            {
                                _accessoryUI.Navis[i]->Enable = false;
                            }
                        }
                    }
                }
            }
            else
            {
                for (size_t i = 0; i < _accessoryUI.Icons.size(); ++i)
                {
                    ImageElement* image = _accessoryUI.Icons[i];
                    if (image)
                    {
                        image->Enable = false;
                    }
                    if (i < _accessoryUI.Navis.size())
                    {
                        _accessoryUI.Navis[i]->Enable = false;
                    }
                }
            }
        }
    }
}

void InventoryUIManager::UpdateConsumable() 
{
    //TODO: 소모품 시스템 추가 이후 갱신 일단 비활성화

    for (size_t i = 0; i < _consumableUI.size(); i++)
    {
        if (ImageElement* icon = _consumableUI[i])
        {
            icon->Enable = false;
        }
    }

    for (size_t i = 0; i < _consumableNavi.size(); i++)
    {
        if (InventoryItemFocusNavi* navi = _consumableNavi[i])
        {
            navi->Enable = false;
        }
    }
}

void InventoryUIManager::UpdateScroll(HorizontalPageUIManager* manager)
{
    auto ScrollCheck = [this](HorizontalPageUI& page) 
    {
        if (nullptr != page.Manager && false == page.Navis.empty())
        {
            HorizontalPageUIManager::DIR dir = page.Manager->GetLastDIR();
            switch (dir)
            {
            case HorizontalPageUIManager::DIR::LEFT:
                if (InventoryItemFocusNavi* navi = page.Navis.back())
                {
                    navi->Focus();
                }
                break;
            case HorizontalPageUIManager::DIR::RIGHT:
                if (InventoryItemFocusNavi* navi = page.Navis.front())
                {
                    navi->Focus();
                } 
                break;
            case HorizontalPageUIManager::DIR::UNKNOWN:
            default:
                break;
            }             
        }
    };

    if (manager && manager == _revelationUI.Manager)
    {
        size_t focus = manager->CurrentFocus;
        UpdateRevelationUI(focus * _revelationUI.Icons.size());
        ScrollCheck(_revelationUI);
    }
    else if (manager && manager == _accessoryUI.Manager)
    {
        size_t focus = manager->CurrentFocus;
        UpdateAccessoryUI(focus * _accessoryUI.Icons.size());
        ScrollCheck(_accessoryUI);   
    }
}

