#include "pchScripts.h"
#include "CombatUIManager.h"
#include "Preferences/PreferencesManager.h"
#include "Inventory/UI/InventoryUIManager.h"

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

    CharacterHUDGroup.RefreshEnemiesPosition();
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
    if (CharacterHUDGroup.IsValid())
    {
        CharacterHUDGroup.RefreshUIPosition();
    }
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
