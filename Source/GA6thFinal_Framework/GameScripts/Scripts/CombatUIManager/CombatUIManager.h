#pragma once
#include <Utility/SingletonHelper.h>

#include <CombatUIManager/CharacterHUD/CharactorHUDGroup.h>
#include <CombatUIManager/TurnQueue/TurnQueueGroup.h>
#include <CombatUIManager/Consumable/ConsumableGroup.h>
#include <CombatUIManager/Revelations/RevelationsGroup.h>
#include <CombatUIManager/Weaapon/WeaponGroup.h>
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
    /// UI데이터를 갱신합니다.
    /// </summary>
    void Refresh();

    void RefreshPosition();

    /// <summary>
    /// UI의 활성화 상태를 설정합니다.
    /// </summary>
    /// <param name="active">UI를 활성화할지 여부</param>
    void SetActiveUI(bool active);

protected:
    void Reset() override;
    void Awake() override;
    void Update() override;
    void FixedUpdate() override;
    void ImGuiDrawPropertysEvent() override;
    void SerializedReflectEvent() override;
    void DeserializedReflectEvent() override;

private:
    SingletonComponent<CombatUIManager> _singletonComponent{this};

    CombatUI::CharacterHUDGroup _charactorHUDGroup;
    CombatUI::TurnQueueGroup    _turnQueueGroup;
    CombatUI::ConsumableGroup   _consumableGroup;
    CombatUI::RevelationsGroup  _revelationsGroup;
    CombatUI::WeaponGroup       _weaponGroup;
    CombatUI::AccessoriesGroup  _accessoriesGroup;

protected:
    REFLECT_FIELDS_BEGIN(Component)
    REFLECT_FIELDS_END(CombatUIManager)

private:
    void PreferencesKeyDown(const Input::Controller&);

};

