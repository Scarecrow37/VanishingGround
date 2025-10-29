#pragma once
#include <Utility/SingletonHelper.h>

#include <CombatUIManager/CharacterHUD/CharacterHUDGroup.h>
#include <CombatUIManager/TurnQueue/TurnQueueGroup.h>
#include <CombatUIManager/Consumable/ConsumableGroup.h>
#include <CombatUIManager/Revelations/RevelationsGroup.h>
#include <CombatUIManager/Weapon/WeaponGroup.h>
#include <CombatUIManager/Accessories/AccessoriesGroup.h>

class OverlayPanel;
class ImageElement;

class CombatUIManager : public Component, public InputReceiver
{
    USING_PROPERTY(CombatUIManager)

public:
    CombatUIManager();
    ~CombatUIManager() override;

public:
    /// <summary>
    /// CombatUI의 그룹이 모두 유효한지 여부를 반환합니다.
    /// </summary>
    bool IsValid() const { return _validGroupCount == _uiGroups.size(); }

    /// <summary>
    /// UI데이터를 갱신합니다.
    /// </summary>
    void Refresh();

    /// <summary>
    /// UI의 활성화 상태를 설정합니다.
    /// </summary>
    /// <param name="active">UI를 활성화할지 여부</param>
    void SetActiveUI(bool active);

    void FadeIn(float duration);
    void FadeOut(float duration);

protected:
    void Reset() override;
    void Awake() override;
    void Start() override;
    void Update() override;
    void FixedUpdate() override;
    void ImGuiDrawPropertysEvent() override;
    void SerializedReflectEvent() override;
    void DeserializedReflectEvent() override;

private:
    void PreferencesKeyDown(const Input::Controller&);
    void InventoryKeyDown(const Input::Controller&);
    UINavigationComponent* GetLastFocusNaviFromObjectName(const std::string& uiRootObjectName);

public:
    CombatUI::CharacterHUDGroup CharacterHUDGroup;
    CombatUI::TurnQueueGroup    TurnQueueGroup;
    CombatUI::ConsumableGroup   ConsumableGroup;
    CombatUI::RevelationsGroup  RevelationsGroup;
    CombatUI::WeaponGroup       WeaponGroup;
    CombatUI::AccessoriesGroup  AccessoriesGroup;

    size_t _validGroupCount = 0;

private:
    SingletonComponent<CombatUIManager> _singletonComponent{this};

    // Group 추가 시 여기에도 추가 필요함
    const std::vector<UIGroup*> _uiGroups = {
        &CharacterHUDGroup,
        &TurnQueueGroup,
        &ConsumableGroup,
        &RevelationsGroup,
        &WeaponGroup,
        &AccessoriesGroup
    };

protected:
    REFLECT_FIELDS_BEGIN(Component)
    REFLECT_FIELDS_END(CombatUIManager)

};

