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

public:
    REFLECT_PROPERTY()

protected:
    REFLECT_FIELDS_BEGIN(Component)
    REFLECT_FIELDS_END(PlayerSystem)

    void Reset() override;
    void Awake() override;

    void ImGuiDrawPropertysEvent() override;

private:
    SingletonObject<PlayerSystem> _singletonObject{this};
    class WeaponSystem*           _weaponSystem         = nullptr;
    class WeaponTableComponent*   _weaponTableComponent = nullptr;
    class RevelationSystem*       _revelationSystem     = nullptr;
    class ItemDropSystem*         _itemDropSystem       = nullptr;
    class PlayerStatsComponent*   _playerStatsComponent = nullptr;

    private:
    void CheckSystem();

};
