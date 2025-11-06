#include "pchScripts.h"
#include "CombatUIManager.h"
#include "Preferences/PreferencesManager.h"
#include "Inventory/UI/InventoryUIManager.h"
#include "ItemDropSystem/UI/ItemDropUIRootManager.h"
#include "AccessorySystem/AccessorySystem.h"
#include "KeyCallbackUINavi/KeyCallbackUINavi.h"

UMREAL_COMPONENT(CombatUIManager)

CombatUIManager::CombatUIManager() = default;

CombatUIManager::~CombatUIManager() = default;

void CombatUIManager::Refresh() 
{
    _validGroupCount = 0;

    for (UIGroup* group : _uiGroups)
    {
        if (group)
        {
            if (group->FindUI())
            {
                ++_validGroupCount;
            }
        }
    }
}

void CombatUIManager::SetActiveUI(bool active) 
{
    for (UIGroup* group : _uiGroups)
    {
        if (group && group->IsValid())
        {
            group->ActiveUI(active);
        }
    }
}

void CombatUIManager::Reset() 
{
    _singletonComponent.SetSingleTon();
}

void CombatUIManager::Awake() 
{
    BindInputAction(ControllerButton::BACK,  Action::PRESSED, this, &CombatUIManager::PreferencesKeyDown);  // 옵션 창 키 바인딩
    BindInputAction(ControllerButton::START, Action::PRESSED, this, &CombatUIManager::InventoryKeyDown);    // 인벤 창 키 바인딩
    BindInputAction(ControllerButton::RIGHT_SHOULDER, Action::PRESSED, this, &CombatUIManager::ChangeFocusKeyDown); // 포커스 변경
    _singletonComponent.TrySingleTon();  
}

void CombatUIManager::Start() 
{
    if (_singletonComponent.IsSingleTon())
    {
        SetActiveUI(false);
    }
}

void CombatUIManager::Update() 
{
    CharacterHUDGroup.RefreshUIPosition();
    CharacterHUDGroup.RefreshCharactersUIPosition();
}

void CombatUIManager::FixedUpdate() 
{
}

void CombatUIManager::ImGuiDrawPropertysEvent() 
{
    if (false == IsValid())
    {
        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), (const char*)u8"모든 전투 UI가 유효하지 않습니다.");
        ImGui::Indent();
        for (const auto& ui : _uiGroups)
        {
            if (false == ui->IsValid())
            {
                const char* name = typeid(*ui).name(); // 클래스 이름 가져오기
                ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), (const char*)u8"%s UI가 유효하지 않습니다.", name + 6);
            }
        }
        ImGui::Unindent();
    }
    else
    {
        ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), (const char*)u8"모든 전투 UI가 유효합니다.");
    }

    if (ImGui::Button("Find All UI"))
    {
        Refresh();
    }
}

void CombatUIManager::SerializedReflectEvent() 
{
}

void CombatUIManager::DeserializedReflectEvent() 
{
}

bool CombatUIManager::CheckCombatUIInput()
{
    if (InventoryUIManager* manager = SingletonComponent<InventoryUIManager>::GetInstance())
    {
        if (manager->gameObject->ActiveInHierarchy == true)
        {
            return false;
        }
    }
    if (PreferencesManager* manager = SingletonComponent<PreferencesManager>::GetInstance())
    {
        if (manager->IsOpen())
        {
            return false;
        }
    }
    if (ItemDropUIRootManager* itemDropUIRootManager = SingletonComponent<ItemDropUIRootManager>::GetInstance())
    {
        if (itemDropUIRootManager->gameObject->ActiveSelf)
        {
            return false;
        }
    }

    return true;
}

void CombatUIManager::PreferencesKeyDown(const Input::Controller&) 
{
    bool isOpen = true;
    if (InventoryUIManager* manager = SingletonComponent<InventoryUIManager>::GetInstance())
    {
        isOpen = manager->gameObject->ActiveInHierarchy == false;
    }

    if (isOpen)
    {
        if (PreferencesManager* manager = SingletonComponent<PreferencesManager>::GetInstance())
        {
            UINavigationComponent* lastFocus = GetLastFocusNaviFromObjectName("UI Root");
            manager->OnPreferencesWindow(lastFocus);
        }
    }
}

void CombatUIManager::InventoryKeyDown(const Input::Controller&)
{
    bool isOpen = true;
    if (PreferencesManager* manager = SingletonComponent<PreferencesManager>::GetInstance())
    {
        isOpen = manager->IsOpen() == false;
    }

    if (isOpen)
    {
        if (InventoryUIManager* manager = SingletonComponent<InventoryUIManager>::GetInstance())
        {
            UINavigationComponent* lastFocus = GetLastFocusNaviFromObjectName("UI Root");
            manager->OpenInventory(lastFocus);
        }
    }
}

void CombatUIManager::ChangeFocusKeyDown(const Input::Controller&)
{
    if (CheckCombatUIInput())
    {
        if (_isFocusAccessories == false)
        {
            if (AccessorySystem* system = SingletonComponent<AccessorySystem>::GetInstance())
            {
                size_t itemCount = system->GetPlayerAccessoryItems().size();
                if (0 < itemCount)
                {
                    if (auto naviPanel = GameObject::FindWithTag("Accessories Horizontal Navis Panel").lock())
                    {
                        if (Transform* child = naviPanel->transform->GetChild(0))
                        {
                            if (KeyCallbackUINavi* navi = child->gameObject->GetComponent<KeyCallbackUINavi>())
                            {
                                navi->Focus();
                                _isFocusAccessories = true;
                            }
                        }
                    }
                }
            }
        }   
        else
        {
            if (CombatUIManager* combatUIManager = SingletonComponent<CombatUIManager>::GetInstance())
            {
                if (auto focusNavi = GameObject::FindComponentWithTag<KeyCallbackUINavi>("Weapon Panel UI Navi").lock())
                {
                    focusNavi->Focus();
                    _isFocusAccessories = false;
                }
            }
        }
    }
}

UINavigationComponent* CombatUIManager::GetLastFocusNaviFromObjectName(const std::string& uiRootObjectName)
{
    UINavigationComponent* value = nullptr;
    if (auto uiRootObject = GameObject::Find(uiRootObjectName).lock())
    {
        if (UIRoot* root = uiRootObject->GetComponent<UIRoot>())
        {
            value = root->GetFocusedNavigationComponent();
        }
    }
    return value;
}


void CombatUIManager::FadeIn(float duration)
{
    for (auto& group : _uiGroups)
    {
        group->FadeIn(duration);
    }
}

void CombatUIManager::FadeOut(float duration)
{
    for (auto& group : _uiGroups)
    {
        group->FadeOut(duration);
    }
}