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

void InventoryUIManager::Awake()
{
    Base::Awake();
    if (_singletonComponent.TrySingleTon())
    {
        BindInputAction(ControllerButton::B, Action::PRESSED, this, &InventoryUIManager::OnButtonB);
    }
    _closeFlag = true;
}

void InventoryUIManager::Update() 
{
    if (_closeFlag)
    {
        CloseInventory();
        _closeFlag = false;
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
                    _revelationUI.Manager = object.GetComponent<HorizontalPageUIManager>();
                    _revelationUI.Icons   = FindIcons(curr);
                    _revelationUI.Navis   = FindFocus(curr);
                }
                else if (object.CompareTag("Accessory"))
                {
                    _accessoryUI.Manager = object.GetComponent<HorizontalPageUIManager>();
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
            InventoryItemFocusNavi* navi = obj.GetComponent<InventoryItemFocusNavi>();
            if (navi)
            {
                navis.push_back(navi);
            }
        }
    });
    return navis;
}

void InventoryUIManager::OnButtonB(const Input::Controller&) 
{
    if (true == gameObject->ActiveSelf)
    {
        CloseInventory();
        _closeFlag = true;
    }
}

void InventoryUIManager::OpenInventory(UINavigationComponent* lastFocus) 
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
    if (0 < _weaponsNavi.size())
    {
        _weaponsNavi[0]->Focus();
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
        }
    }
}

void InventoryUIManager::UpdateRevelationUI() 
{
    //계시 갱신
    if (RevelationSystem* system = SingletonComponent<RevelationSystem>::GetInstance())
    {
        const auto&   elements = system->GetPlayerElementList();
        size_t revelationCount = elements.size();
        if (_revelationUI.Manager)
        {
            size_t iconsCount      = _revelationUI.Icons.size();
            size_t horizontalCount = revelationCount / iconsCount + 1;
            _revelationUI.Manager->UpdateHorizontalUI(horizontalCount);
            if (0 < iconsCount)
            {
                for (size_t i = 0; i < iconsCount; i++)
                {
                    ImageElement* image = _revelationUI.Icons[i];
                    if (image)
                    {
                        if (i < elements.size())
                        {
                            const auto& revelation = elements[i];
                            if (revelation)
                            {
                                DropItemInfo info = revelation->GetItemInfo();
                                int          id   = DropItemInfo::GetArtifactIconID(info);
                                File::Guid   guid = UmFileSystem.GetGuidFromAssetID(id);
                                image->Enable     = true;
                                image->SetImage(guid);
                            }
                        }
                        else
                        {
                            image->Enable = false;
                        }
                    }
                }
            }
            else
            {
                for (auto& icon : _revelationUI.Icons)
                {
                    if (icon)
                    {
                        icon->Enable = false;
                    }
                }
            }
        }
    }
}

void InventoryUIManager::UpdateAccessoryUI() 
{
    //장신구 갱신
    if (AccessorySystem* system = SingletonComponent<AccessorySystem>::GetInstance())
    {
        const auto& items          = system->GetPlayerAccessoryItems();
        size_t      accessoryCount = items.size();
        if (_accessoryUI.Manager)
        {
            size_t iconsCount      = _accessoryUI.Icons.size();
            size_t horizontalCount = accessoryCount / iconsCount + 1;
            _accessoryUI.Manager->UpdateHorizontalUI(horizontalCount);
            if (0 < iconsCount)
            {
                for (size_t i = 0; i < iconsCount; i++)
                {
                    ImageElement* image = _accessoryUI.Icons[i];
                    if (image)
                    {
                        if (i < items.size())
                        {
                            const auto&  accessory = items[i];
                            DropItemInfo info      = accessory.GetItemInfo();
                            int          id        = DropItemInfo::GetArtifactIconID(info);
                            File::Guid   guid      = UmFileSystem.GetGuidFromAssetID(id);
                            image->Enable          = true;
                            image->SetImage(guid);
                        }
                        else
                        {
                            image->Enable = false;
                        }
                    }
                }
            }
            else
            {
                for (auto& image : _accessoryUI.Icons)
                {
                    if (image)
                    {
                        image->Enable = false;
                    }
                }
            }
        }
    }
}

