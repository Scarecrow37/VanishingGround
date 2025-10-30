#include "pchScripts.h"
#include "WeaponChangeUIManager.h"
#include "UI/Elements/Image/ImageElement.h"
#include "UI/Elements/Text/TextElement.h"
#include "UI/Panels/Description/DescriptionPanel.h"
#include "ItemDropSystem/UINavi/PlayerWeaponChangeNavi.h"
#include "WeaponSystem/WeaponTable/WeaponTableComponent.h"
#include "WeaponSystem/WeaponSystem.h"
#include "ItemDropSystem/UI/ItemDropUIRootManager.h"
#include "ItemDropSystem/UI/ArtifactUIManager.h"
#include "ItemDropSystem/UINavi/ArtifactButtonNavi.h"

UMREAL_COMPONENT(WeaponChangeUIManager)

WeaponChangeUIManager::WeaponChangeUIManager()
{
    _state            = UIState::IDLE;
    _changeWeaponSlot = -1;
    _changeWeaponIcon = nullptr;
}
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
    _changeWeaponElement     = changeWeapon;
    WeaponStats&       stats = _changeWeaponElement.Stats;
    const std::string& name  = stats.WeaponName;
    DropItemInfo       info  = _changeWeaponElement.GetItemInfo();
    const File::Guid&  guid  = UmFileSystem.GetGuidFromAssetID(DropItemInfo::GetArtifactIconID(info));
    if (_changeWeaponIcon)
    {
        _changeWeaponIcon->SetImage(guid);
    }
    if (_changeWeaponStats.Name)
    {
        _changeWeaponStats.Name->Text = name;
    }
    if (_changeWeaponStats.Icon)
    {
        _changeWeaponStats.Icon->SetImage(guid);
    }
    if (_changeWeaponStats.Damage.Text)
    {
        _changeWeaponStats.Damage.Text->Text = std::to_string(stats.HitDamage);
    }
    if (_changeWeaponStats.Critical.Text)
    {
        _changeWeaponStats.Critical.Text->Text = std::to_string(stats.CriticalDamage);
    }
    if (_changeWeaponStats.AttackCount.Text)
    {
        _changeWeaponStats.AttackCount.Text->Text = std::to_string(stats.AttackCount);
    }
    if (_changeWeaponStats.Speed.Text)
    {
        _changeWeaponStats.Speed.Text->Text = std::to_string(stats.Speed);
    }
    if (_changeWeaponStats.Description)
    {
        _changeWeaponStats.Description->Description = DropItemInfo::GetArtifactDescription(info);
    }
    if (_changeWeaponStats.Keyword)
    {
        //TODO 키워드 설명 파싱 필요
        _changeWeaponStats.Keyword->Description = "";
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

    if (0 < _playerWeapons.size())
    {
        if (_playerWeapons[0].WeaponNavi)
        {
            _playerWeapons[0].WeaponNavi->Focus();
        }
    } 
}

void WeaponChangeUIManager::ShowChangeWarningUI(int slot) 
{
    if (WeaponSystem* weaponSystem = SingletonComponent<WeaponSystem>::GetInstance())
    {
        const auto& weapons = weaponSystem->GetEquipWeapons();
        if (slot < weapons.size())
        {
            const WeaponElement& weapon = weapons[slot];
            if (nullptr != _warningUI.WarningUIObject)
            {
                _warningUI.WarningUIObject->SetActive(true);
                _changeWeaponSlot = slot;

                if (_warningUI.Before)
                {
                    DropItemInfo info = weapon.GetItemInfo();
                    File::Guid   icon = UmFileSystem.GetGuidFromAssetID(DropItemInfo::GetArtifactIconID(info));
                    _warningUI.Before->SetImage(icon);
                }

                if (_warningUI.After)
                {
                    DropItemInfo info = _changeWeaponElement.GetItemInfo();
                    File::Guid   icon = UmFileSystem.GetGuidFromAssetID(DropItemInfo::GetArtifactIconID(info));
                    _warningUI.After->SetImage(icon);
                }
                PushInputLayer();
            }
        }
        _state = UIState::IDLE;
    }
}

bool WeaponChangeUIManager::HasWarningUI() const
{
    if (_warningUI.WarningUIObject)
    {
        return _warningUI.WarningUIObject->ActiveSelf;
    }
    return false;
}

void WeaponChangeUIManager::SetPlayerWeaponStatsUI(const WeaponElement& focusWeapon) 
{
    const WeaponStats& playerStats = focusWeapon.Stats;
    const std::string& playerName  = playerStats.WeaponName;
    DropItemInfo       playerInfo  = focusWeapon.GetItemInfo();
    WeaponStats&       changeStats = _changeWeaponElement.Stats;
    if (_playerWeaponStats.Name)
    {
        _playerWeaponStats.Name->Text = playerName;
    }
    if (_playerWeaponStats.Icon)
    {
        File::Guid guid = UmFileSystem.GetGuidFromAssetID(DropItemInfo::GetArtifactIconID(playerInfo));
        _playerWeaponStats.Icon->SetImage(guid);
    }
    if (_playerWeaponStats.Damage)
    {
        _playerWeaponStats.Damage->Text = std::to_string(playerStats.HitDamage);
    }
    if (_playerWeaponStats.Critical)
    {
        _playerWeaponStats.Critical->Text = std::to_string(playerStats.CriticalDamage);
    }
    if (_playerWeaponStats.AttackCount)
    {
        _playerWeaponStats.AttackCount->Text = std::to_string(playerStats.AttackCount);
    }
    if (_playerWeaponStats.Speed)
    {
        _playerWeaponStats.Speed->Text = std::to_string(playerStats.Speed);
    }
    if (_playerWeaponStats.Description)
    {
        _playerWeaponStats.Description->Description = DropItemInfo::GetArtifactDescription(playerInfo);
    }
    if (_playerWeaponStats.Keyword)
    {
        //TODO: 키워드 파싱 필요
        _playerWeaponStats.Keyword->Description = "";
    }

    auto UpdateArrowUI = [](int player, int change, auto& upArrow, auto& downArrow) 
    { 
        bool upActive   = false;
        bool downActive = false;
        if (player < change)
        {
            upActive = true;
        }
        else if (player > change)
        {
            downActive = true;
        }

        if (upArrow)
        {
            upArrow->Enable = upActive;
        }
        if (downArrow)
        {
            downArrow->Enable = downActive; 
        }
    };

    UpdateArrowUI(playerStats.HitDamage, changeStats.HitDamage, 
        _changeWeaponStats.Damage.UpArrow, _changeWeaponStats.Damage.DownArrow);
    UpdateArrowUI(playerStats.CriticalDamage, changeStats.CriticalDamage, 
        _changeWeaponStats.Critical.UpArrow, _changeWeaponStats.Critical.DownArrow);
    UpdateArrowUI(playerStats.AttackCount, changeStats.AttackCount, 
        _changeWeaponStats.AttackCount.UpArrow, _changeWeaponStats.AttackCount.DownArrow);
    UpdateArrowUI(playerStats.Speed, changeStats.Speed, 
        _changeWeaponStats.Speed.UpArrow, _changeWeaponStats.Speed.DownArrow);
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
        BindInputAction(ControllerButton::B, Action::PRESSED, this, &WeaponChangeUIManager::OnPressedActionB);
        BindInputAction(ControllerButton::A, Action::PRESSED, this, &WeaponChangeUIManager::OnPressedActionA);


        gameObject->AddTag(TAG);
        FindUIElements();
    }
}

void WeaponChangeUIManager::Start() 
{
    Base::Start();
    HideUI();
}

void WeaponChangeUIManager::Update() 
{
    if (_state != UIState::IDLE)
    {
        switch (_state)
        {
        case UIState::CANCEL:
            if (_warningUI.WarningUIObject && _warningUI.WarningUIObject->ActiveInHierarchy)
            {
                _warningUI.WarningUIObject->SetActive(false);
                PopInputLayer();
                _changeWeaponSlot = -1;
            }
            else
            {
                if (ItemDropUIRootManager* rootManager = SingletonComponent<ItemDropUIRootManager>::GetInstance())
                {
                    rootManager->AutoFocus(false);
                    HideUI();
                }
            }
            break;
        case UIState::APPROVE:
            if (_warningUI.WarningUIObject && _warningUI.WarningUIObject->ActiveInHierarchy)
            {
                if (WeaponSystem* system = SingletonComponent<WeaponSystem>::GetInstance())
                {
                    system->EquipWeapon(_changeWeaponSlot, _changeWeaponElement);
                    _warningUI.WarningUIObject->SetActive(false);
                    PopInputLayer();
                    _changeWeaponSlot = -1;
                    if (ArtifactUIManager* artifactManager = SingletonComponent<ArtifactUIManager>::GetInstance())
                    {                     
                        artifactManager->ObtainFocusNavi(ArtifactButtonNavi::GetLastFocusIndex());               
                        HideUI();
                    }
                }
            }
            break;
        default:
            break;
        }
        _state = UIState::IDLE;
    }
}

void WeaponChangeUIManager::HideUI() 
{
    gameObject->SetActive(false);
    for (auto& info : _playerWeapons)
    {
        if (info.FocusImage)
        {
            info.FocusImage->Enable = false;
        }
    }
    
    if (_warningUI.WarningUIObject && true == _warningUI.WarningUIObject->ActiveSelf)
    {
        _warningUI.WarningUIObject->SetActive(false);
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
            if (object.CompareTag("Change Weapon Icon"))
            {
                _changeWeaponIcon = object.GetComponent<ImageElement>();
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
            else if (object.CompareTag("Keyword Description"))
            {
                _playerWeaponStats.Keyword = object.GetComponent<DescriptionPanel>();
            }
        });
    }

    if (Transform* changeWeaponStats = transform->FindWithTag("Change Weapon Stats"))
    {
        Transform::ForeachDFS(*changeWeaponStats, [this](Transform* curr) 
        {
            GameObject& object = curr->gameObject;
            if (object.CompareTag("Name"))
            {
                _changeWeaponStats.Name = object.GetComponent<TextElement>();
            }
            else if (object.CompareTag("Icon"))
            {
                _changeWeaponStats.Icon = object.GetComponent<ImageElement>();
            }
            else if (object.CompareTag("Damage"))
            {            
                if (object.CompareTag("Up Arrow"))
                {
                    _changeWeaponStats.Damage.UpArrow = object.GetComponent<ImageElement>();
                }
                else if (object.CompareTag("Down Arrow"))
                {
                    _changeWeaponStats.Damage.DownArrow = object.GetComponent<ImageElement>();
                }
                else
                {
                    _changeWeaponStats.Damage.Text = object.GetComponent<TextElement>();
                }
            }
            else if (object.CompareTag("Critical"))
            {
                if (object.CompareTag("Up Arrow"))
                {
                    _changeWeaponStats.Critical.UpArrow = object.GetComponent<ImageElement>();
                }
                else if (object.CompareTag("Down Arrow"))
                {
                    _changeWeaponStats.Critical.DownArrow = object.GetComponent<ImageElement>();
                }
                else
                {
                    _changeWeaponStats.Critical.Text = object.GetComponent<TextElement>();
                }
            }
            else if (object.CompareTag("Count"))
            {
                if (object.CompareTag("Up Arrow"))
                {
                    _changeWeaponStats.AttackCount.UpArrow = object.GetComponent<ImageElement>();
                }
                else if (object.CompareTag("Down Arrow"))
                {
                    _changeWeaponStats.AttackCount.DownArrow = object.GetComponent<ImageElement>();
                }
                else
                {
                    _changeWeaponStats.AttackCount.Text = object.GetComponent<TextElement>();
                }
            }
            else if (object.CompareTag("Speed"))
            {
                if (object.CompareTag("Up Arrow"))
                {
                    _changeWeaponStats.Speed.UpArrow = object.GetComponent<ImageElement>();
                }
                else if (object.CompareTag("Down Arrow"))
                {
                    _changeWeaponStats.Speed.DownArrow = object.GetComponent<ImageElement>();
                }
                else
                {
                    _changeWeaponStats.Speed.Text = object.GetComponent<TextElement>();
                }
            }
            else if (object.CompareTag("Description"))
            {
                _changeWeaponStats.Description = object.GetComponent<DescriptionPanel>();
            }
            else if (object.CompareTag("Keyword Description"))
            {
                _changeWeaponStats.Keyword = object.GetComponent<DescriptionPanel>();
            }
        });
    }

    if (Transform* warningUI = transform->FindWithTag("Warning Panel"))
    {
        _warningUI.WarningUIObject = &warningUI->gameObject;
        Transform::ForeachDFS(*warningUI, [this](Transform* curr)        
        { 
            GameObject& object = curr->gameObject;
            if (object.CompareTag("Before"))
            {
                _warningUI.Before = object.GetComponent<ImageElement>();
            }
            else if (object.CompareTag("After"))
            {
                _warningUI.After = object.GetComponent<ImageElement>();
            }
        });
    }
}

void WeaponChangeUIManager::OnPressedActionB(const Input::Controller&) 
{
    if (EnableInHierarchy)
    {      
        _state = UIState::CANCEL;
    }
}

void WeaponChangeUIManager::OnPressedActionA(const Input::Controller&) 
{
    if (EnableInHierarchy)
    {
        _state = UIState::APPROVE;
    }
}
