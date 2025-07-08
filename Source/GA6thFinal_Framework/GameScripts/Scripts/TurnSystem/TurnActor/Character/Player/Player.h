#pragma once
#include "TurnSystem/TurnActor/Character/CharacterBase.h"
#include <Stats/Weapon/WeaponStats.h>

class FiniteStateMachine;
class Player : public CharacterBase
{
    USING_PROPERTY(Player)
public:
    inline static constexpr const char* TAG                = "Player";
    inline static constexpr size_t      EQUIP_WEAPONS_SIZE = 4;

public:
    REFLECT_PROPERTY(
        Shield, 
        CurrentWeaponSlot
    )

    GETTER_ONLY(int, Shield) { return GetShield(); }
    PROPERTY(Shield)

    GETTER_ONLY(int, CurrentWeaponSlot) { return _currentWeaponSlot; }
    //현재 사용중인 무기의 슬롯 번호 입니다.
    PROPERTY(CurrentWeaponSlot)

public:
    Player();
    virtual ~Player();

protected:
    REFLECT_FIELDS_BEGIN(CharacterBase)
    std::array<std::string, EQUIP_WEAPONS_SIZE> EquipWeaponsData;
    REFLECT_FIELDS_END(Player)

private:
    class PlayerStatsComponent* _playerStats = nullptr;
    PlayerStatsComponent* GetPlayerStats();
    int GetShield();

private:
    FiniteStateMachine* _finiteStateMachine = nullptr;
    void BuildPlayerFSM();
    struct PlayerStates
    {
        class PlayerWaitTurnState* PlayerWaitTurnState = nullptr;
        class PlayerPlayTurnState* PlayerPlayTurnState = nullptr;
        class PlayerDeadState*     PlayerDeadState     = nullptr;
    } 
    _fsmStates;
public:
    /*플레이어의 턴을 시작합니다.*/
    virtual void PlayTurn() override;
    /*플레이어의 턴을 종료합니다.*/
    virtual void EndTurn() override;
    /*플레이어를 사망 상태로 만듭니다.*/
    virtual void Dead() override;

    FiniteStateMachine& GetFSM() { return *_finiteStateMachine; }
    const PlayerStates& GetFSMStates() { return _fsmStates; }

public:
    // CharacterBase을(를) 통해 상속됨
    virtual int GetSpeed() override;
    virtual int GetRandomSpeed() override;
    CharacterStats* GetCharacterStats() override;
    virtual void    OnRoundStart() override;

public:
    //Weapon

    /// <summary>
    /// 무기를 slot에 장착합니다.
    /// 기존에 장착되어있던 무기는 반환되며 제거됩니다.
    /// 잘못된 slot을 접근시 WeaponStats는 0 damege 무기를 반환합니다.
    /// </summary>
    /// <param name="slot :">장착할 슬롯</param>
    WeaponStats EquipWeapon(int slot, const WeaponStats& weaponStats);

    /// <summary>
    /// 현재 사용할 무기를 선택합니다.
    /// 속도, 데미지, 타격 횟수 등이 해당 슬롯에 무기로 설정됩니다.
    /// </summary>
    void SetCurrentWeaponSlot(int slot);

    /// <summary>
    /// 현재 사용중인 무기의 Stats을 반환합니다.
    /// </summary>
    /// <returns></returns>
    const WeaponStats& GetCurrentWeaponStats() { return _equipWeapons[_currentWeaponSlot]; }

    /// <summary>
    /// 특정 슬롯에 무기 라운드 속도를 반환합니다. (RandomSpeed + Speed)
    /// </summary>
    /// <param name="slot :">반환할 무기 슬롯</param>
    /// <returns>해당 슬롯 무기의 속도</returns>
    int GetRoundSpeedToSlot(int slot);

private:
    /*현재 사용중인 무기 슬롯*/
    int _currentWeaponSlot = 0;

    /*장착된 무기들*/
    std::array<WeaponStats, EQUIP_WEAPONS_SIZE> _equipWeapons;

    /*이번 라운드의 Weapon Speed의 순서 (속도 기준 내림차순)*/

    /*에디터용 무기 Imgui 함수*/
    void ImguiEquipWeapons();

protected:
    /// <summary>
    /// <para> 이 함수는 항상 Start 함수 전에 호출되며 프리팹이 인스턴스화 된 직후에 호출됩니다.                </para>
    /// <para> 게임 오브젝트의 Active가 false 상태인 경우 Awake 함수는 true가 될때까지 호출되지 않습니다.      </para>
    /// </summary>
    virtual void Awake();
    
    /// <summary>
    /// Update 는 프레임당 한 번 호출됩니다.
    /// </summary>
    virtual void Update();

    /// <summary>
    /// <para> 직렬화 직전 자동으로 호출되는 이벤트 함수입니다. </para>
    /// <para> 직접 override 해서 사용합니다.                 </para>
    /// </summary>
    virtual void SerializedReflectEvent() override;

    /// <summary>
    /// <para> 역직렬화 이후 자동으로 호출되는 이벤트 함수 입니다.  </para>
    /// <para> 직접 override 해서 사용합니다.                     </para>
    /// </summary>
    virtual void DeserializedReflectEvent() override;

    /// <summary>
    /// <para>  ImGuiDrawPropertys() 호출 이후 콜되는 이벤트 함수입니다. </para>
    /// </summary>
    virtual void ImGuiDrawPropertysEvent();
};
