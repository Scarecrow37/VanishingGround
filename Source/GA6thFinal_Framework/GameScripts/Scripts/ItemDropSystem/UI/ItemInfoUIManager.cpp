#include "pchScripts.h"
#include "ItemInfoUIManager.h"
#include "UI/Elements/Image/ImageElement.h"
#include "UI/Elements/Text/TextElement.h"
#include "UI/Panels/Description/DescriptionPanel.h"
#include "ExcelDataSystem/ExcelDataSystem.h"
#include "WeaponSystem/WeaponTable/WeaponTableComponent.h"
#include "WeaponSystem/WeaponElement/WeaponElement.h"

UMREAL_COMPONENT(ItemInfoUIManager)

ItemInfoUIManager::ItemInfoUIManager() = default;
ItemInfoUIManager::~ItemInfoUIManager() = default;

void ItemInfoUIManager::SetItemInfoUI(const DropItemInfo& info) 
{
    SetItemName(info.Name);
    SetItemIcon(info);
    SetItemDescription(info);
    SetWeaponStats(info);
}

void ItemInfoUIManager::SetItemName(const std::string& name) 
{
    // 이름 UI 갱신
    if (_uiComponents.ItemName)
    {
        _uiComponents.ItemName->Text = name;
    }
}

void ItemInfoUIManager::SetItemIcon(const DropItemInfo& info) 
{
    int iconAssetID = DropItemInfo::GetArtifactIconID(info);
    File::Guid iconGuid = UmFileSystem.GetGuidFromAssetID(iconAssetID);
    SetItemIcon(iconGuid);
    if (iconGuid.IsNull())
    {
        std::string message = "Asset ID : ";     
        message += std::to_string(iconAssetID);   
        message += " is not Import";
        UmLogger.Log(LogLevel::LEVEL_WARNING, message);
    }
}

void ItemInfoUIManager::SetItemIcon(const File::Guid& guid)
{
    if (_uiComponents.ItemIcon)
    {
        if (false == guid.IsNull())
        {
            _uiComponents.ItemIcon->Enable = true;
            _uiComponents.ItemIcon->SetImage(guid);
        }
        else
        {
            _uiComponents.ItemIcon->Enable = false;
        }       
    }
}

void ItemInfoUIManager::SetItemDescription(const DropItemInfo& info) 
{
    std::string description = DropItemInfo::GetArtifactDescription(info);
    SetItemDescription(description);

    //TODO: Flavor 텍스트에 대한 표시 해야함.
    SetFlavorDescription("");
    
    //TODO: 키워드에 대한 설명 표시해야함.
    SetKeywordDescription("");
}

void ItemInfoUIManager::SetItemDescription(const std::string& description) 
{
    if (_uiComponents.ItemDescription)
    {
        _uiComponents.ItemDescription->Description = description;
    }
}

void ItemInfoUIManager::SetKeywordDescription(const std::string& description) 
{
    if (_uiComponents.ItemKeyword)
    {
        _uiComponents.ItemKeyword->Description = description;
    }
}

void ItemInfoUIManager::SetFlavorDescription(const std::string& description) 
{
    if (_uiComponents.ItemFlavor)
    {
        _uiComponents.ItemFlavor->Description = description;
    }
}

void ItemInfoUIManager::SetWeaponStats(const DropItemInfo& info) 
{
    if (WeaponTableComponent* table = SingletonComponent<WeaponTableComponent>::GetInstance())
    {
        const std::string& weaponName = info.Name;
        if (const WeaponElement* element = table->GetWeaponToName(weaponName))
        {
            SetWeaponStats(element->Stats);
        }
        else
        {
            ClearWeaponStats();
        }
    }
    else
    {
        ClearWeaponStats();
    }
}

void ItemInfoUIManager::SetWeaponStats(const WeaponStats& stats) 
{
    if (_uiComponents.Damage)
    {
        _uiComponents.Damage->Text = std::to_string(stats.HitDamage);
    }
    if (_uiComponents.Critical)
    {
        _uiComponents.Critical->Text = std::to_string(stats.CriticalDamage);
    }
    if (_uiComponents.AttackCount)
    {
        _uiComponents.AttackCount->Text = std::to_string(stats.AttackCount);
    }
    if (_uiComponents.Speed)
    {
        _uiComponents.Speed->Text = std::to_string(stats.Speed);
    }
}

void ItemInfoUIManager::ClearWeaponStats() 
{
    if (_uiComponents.Damage)
    {
        _uiComponents.Damage->Text = "";
    }
    if (_uiComponents.Critical)
    {
        _uiComponents.Critical->Text = "";
    }
    if (_uiComponents.AttackCount)
    {
        _uiComponents.AttackCount->Text = "";
    }
    if (_uiComponents.Speed)
    {
        _uiComponents.Speed->Text = "";
    }
}

void ItemInfoUIManager::Awake() 
{
    Base::Awake();
    gameObject->AddTag(TAG);
    FindComponents();
    
}

void ItemInfoUIManager::FindComponents() 
{
    Transform::ForeachBFS(transform, [this](Transform* curr) 
    { 
        GameObject& object = curr->gameObject;
        if (nullptr == _uiComponents.ItemName && object.CompareTag("Name"))
        {
            _uiComponents.ItemName = object.GetComponent<TextElement>();
        }
        else if (nullptr == _uiComponents.ItemIcon && object.CompareTag("Icon"))
        {
            _uiComponents.ItemIcon = object.GetComponent<ImageElement>();
        }
        else if (nullptr == _uiComponents.ItemDescription && object.CompareTag("Description"))
        {
            _uiComponents.ItemDescription = object.GetComponent<DescriptionPanel>();
        }
        else if (nullptr == _uiComponents.ItemKeyword && object.CompareTag("Keyword Description"))
        {
            _uiComponents.ItemKeyword = object.GetComponent<DescriptionPanel>();
        }
        else if (nullptr == _uiComponents.ItemFlavor && object.CompareTag("Flavor Description"))
        {
            _uiComponents.ItemFlavor = object.GetComponent<DescriptionPanel>();
        }
        else if (nullptr == _uiComponents.Damage && object.CompareTag("Damage"))
        {
            _uiComponents.Damage = object.GetComponent<TextElement>();
        }
        else if (nullptr == _uiComponents.Critical && object.CompareTag("Critical"))
        {
            _uiComponents.Critical = object.GetComponent<TextElement>();
        }
        else if (nullptr == _uiComponents.AttackCount && object.CompareTag("Count"))
        {
            _uiComponents.AttackCount = object.GetComponent<TextElement>();
        }
        else if (nullptr == _uiComponents.Speed && object.CompareTag("Speed"))
        {
            _uiComponents.Speed = object.GetComponent<TextElement>();
        }
    });
}
