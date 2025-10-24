#pragma once
#include "UmFramework.h"
#include "Utility/SingletonHelper.h"

//플레이어 시스템용 싱글톤 제어 컴포넌트
class PlayerSystem : public Component
{
    USING_PROPERTY(PlayerSystem)

public:
    PlayerSystem();
    ~PlayerSystem() override;
    
    /// <summary>
    /// 플레이어의 스테이터를 새 게임 상태로 만듭니다.
    /// </summary>
    void SetStatsGameStart();

    /// <summary>
    /// 플레이어의 스테이터를 전투 시작 상태로 만듭니다.
    /// </summary>
    void SetStatsCombatStart();

    /// <summary>
    /// 플레이어 체력 UI 갱신합니다.
    /// </summary>
    void NotifyPlayerHP();

    /// <summary>
    /// 플레이어의 실제 Stats를 반환합니다. nullptr일 수 있습니다.
    /// </summary>
    /// <returns>PlayerStatsComponent*</returns>
    PlayerStatsComponent* GetPlayerStats() { return _playerStatsComponent; }

public:
    REFLECT_PROPERTY
    (
        ReflectFields->RevivePlayer
    )

protected:
    REFLECT_FIELDS_BEGIN(Component)
    bool RevivePlayer = false;
    REFLECT_FIELDS_END(PlayerSystem)

    void Reset() override;
    void Awake() override;
    void Start() override;
    void OnDestroy() override;

    void ImGuiDrawPropertysEvent() override;

private:
    SingletonObject<PlayerSystem> _singletonObject{this};
    SingletonComponent<PlayerSystem> _singletonComponent{this};

    class WeaponSystem*           _weaponSystem         = nullptr;
    class WeaponTableComponent*   _weaponTableComponent = nullptr;
    class RevelationSystem*       _revelationSystem     = nullptr;
    class AccessorySystem*        _accessorySystem      = nullptr;
    class ConsumableSystem*       _consumableSystem     = nullptr;
    class ItemDropSystem*         _itemDropSystem       = nullptr;
    class PlayerStatsComponent*   _playerStatsComponent = nullptr;

private:
    void CheckSystem();

};
