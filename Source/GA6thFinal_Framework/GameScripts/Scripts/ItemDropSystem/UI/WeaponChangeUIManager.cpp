#include "pchScripts.h"
#include "WeaponChangeUIManager.h"
#include "UI/Elements/Image/ImageElement.h"
#include "UI/Elements/Text/TextElement.h"
#include "UI/Panels/Description/DescriptionPanel.h"
#include "ItemDropSystem/UINavi/PlayerWeaponChangeNavi.h"
#include "WeaponSystem/WeaponTable/WeaponTableComponent.h"
#include "WeaponSystem/WeaponSystem.h"

UMREAL_COMPONENT(WeaponChangeUIManager)

WeaponChangeUIManager::WeaponChangeUIManager() = default;
WeaponChangeUIManager::~WeaponChangeUIManager() = default;

void WeaponChangeUIManager::ShowWeaponChangeUI(const std::string& changeWeaponName) 
{
    if (WeaponTableComponent* table = SingletonComponent<WeaponTableComponent>::GetInstance())
    {
        if (const WeaponElement* element = table->GetWeaponToName(changeWeaponName))
        {
            ShowWeaponChangeUI(*element);
        }
        else
        {
            std::string message = changeWeaponName;
            message += (const char*)u8"존재하지 않는 무기 이름입니다.";
            UmLogger.Log(LogLevel::LEVEL_DEBUG, message);
        }
    }
}

void WeaponChangeUIManager::ShowWeaponChangeUI(const WeaponElement& changeWeapon)
{
    gameObject->SetActive(true);
    if (0 < _playerWeapons.size())
    {
        if (_playerWeapons[0].WeaponNavi)
        {
            _playerWeapons[0].WeaponNavi->Focus();
        }
    }
   
    if (WeaponSystem* weaponSystem = SingletonComponent<WeaponSystem>::GetInstance())
    {
        const auto& playerWeapons = weaponSystem->GetEquipWeapons();
        for (size_t i = 0; i < playerWeapons.size(); ++i)
        {
            const auto& element = playerWeapons[i];
            if (i < _playerWeapons.size() && _playerWeapons[i].IconImage)
            {
                DropItemInfo info = element.GetItemInfo();
                File::Guid   guid = UmFileSystem.GetGuidFromAssetID(DropItemInfo::GetArtifactIconID(info));
                _playerWeapons[i].IconImage->SetImage(guid);
            }
        }
    }
}

void WeaponChangeUIManager::SetPlayerWeaponStatsUI(const WeaponElement& focusWeapon) 
{
    const WeaponStats& stats = focusWeapon.Stats;
    const std::string& name  = stats.WeaponName;
    DropItemInfo       info  = focusWeapon.GetItemInfo();
    if (_playerWeaponStats.Name)
    {
        _playerWeaponStats.Name->Text = name;
    }
    if (_playerWeaponStats.Icon)
    {
        File::Guid guid = UmFileSystem.GetGuidFromAssetID(DropItemInfo::GetArtifactIconID(info));
        _playerWeaponStats.Icon->SetImage(guid);
    }
    if (_playerWeaponStats.Damage)
    {
        _playerWeaponStats.Damage->Text = std::to_string(stats.HitDamage);
    }
    if (_playerWeaponStats.Critical)
    {
        _playerWeaponStats.Critical->Text = std::to_string(stats.CriticalDamage);
    }
    if (_playerWeaponStats.AttackCount)
    {
        _playerWeaponStats.AttackCount->Text = std::to_string(stats.AttackCount);
    }
    if (_playerWeaponStats.Speed)
    {
        _playerWeaponStats.Speed->Text = std::to_string(stats.Speed);
    }
    if (_playerWeaponStats.Description)
    {
        _playerWeaponStats.Description->Description = DropItemInfo::GetArtifactDescription(info);
    }
}

void WeaponChangeUIManager::ImGuiDrawPropertysEvent() 
{
    if (ImGui::TreeNode("Test Weapon Change UI"))
    {
        if (WeaponTableComponent* weaponTable = SingletonComponent<WeaponTableComponent>::GetInstance())
        {
            for (auto& element : weaponTable->GetWeaponTableElements())
            {
                WeaponStats& stats = element->Stats;
                const std::string& name = stats.WeaponName;
                if (ImGui::Selectable(name.c_str()))
                {
                    ShowWeaponChangeUI(*element);
                }
            }
        }
        ImGui::TreePop();
    }
}

void WeaponChangeUIManager::Reset() 
{
    Base::Reset();
    if (true == UmCore->IsPlay())
    {
        gameObject->ActiveSelf = true;
    }
}

void WeaponChangeUIManager::Awake()
{
    Base::Awake();
    if (_singletonComponent.TrySingleTon())
    {
        gameObject->AddTag(TAG);
        FindUIElements();
    }
}

void WeaponChangeUIManager::Start() 
{
    Base::Start();
    gameObject->SetActive(false);
    for (auto& info : _playerWeapons)
    {
        if (info.FocusImage)
        {
            info.FocusImage->Enable = false;
        }
    }
}

void WeaponChangeUIManager::FindUIElements() 
{
    _playerWeapons.clear();
    if (Transform* playerWeanpons = transform->FindWithTag("Player Weapons"))
    {
        size_t focusIndex = 0;
        size_t iconIndex  = 0;
        Transform::ForeachDFS(*playerWeanpons, [&](Transform* curr) 
        {
            GameObject& object = curr->gameObject;
            if (object.CompareTag("Focus"))
            {
                if (_playerWeapons.size() <= focusIndex)
                {
                    _playerWeapons.emplace_back();
                }
                PlayerWeaponInfo& info = _playerWeapons[focusIndex];
                info.FocusImage        = object.GetComponent<ImageElement>();
                if (info.WeaponNavi = object.GetComponent<PlayerWeaponChangeNavi>())
                {
                    info.WeaponNavi->_weaponIndex = focusIndex;
                }
                ++focusIndex;
            }
            if (object.CompareTag("Icon"))
            {
                if (_playerWeapons.size() <= iconIndex)
                {
                    _playerWeapons.emplace_back();
                }
                PlayerWeaponInfo& info = _playerWeapons[iconIndex];
                info.IconImage         = object.GetComponent<ImageElement>();
                ++iconIndex;
            }
        });
    }
    if (Transform* playerWeaponStats = transform->FindWithTag("Player Weapon Stats"))
    {
        Transform::ForeachDFS(*playerWeaponStats, [this](Transform* curr) 
        {
            GameObject& object = curr->gameObject;
            if (object.CompareTag("Name"))
            {
                _playerWeaponStats.Name = object.GetComponent<TextElement>();
            }
            else if (object.CompareTag("Icon"))
            {
                _playerWeaponStats.Icon = object.GetComponent<ImageElement>();
            }
            else if(object.CompareTag("Damage"))
            {
                _playerWeaponStats.Damage = object.GetComponent<TextElement>();
            }
            else if(object.CompareTag("Critical"))
            {
                _playerWeaponStats.Critical = object.GetComponent<TextElement>();
            }
            else if(object.CompareTag("Count"))
            {
                _playerWeaponStats.AttackCount = object.GetComponent<TextElement>();
            }
            else if(object.CompareTag("Speed"))
            {
                _playerWeaponStats.Speed = object.GetComponent<TextElement>();
            }
            else if (object.CompareTag("Description"))
            {
                _playerWeaponStats.Description = object.GetComponent<DescriptionPanel>();
            }
        });
    }

}
