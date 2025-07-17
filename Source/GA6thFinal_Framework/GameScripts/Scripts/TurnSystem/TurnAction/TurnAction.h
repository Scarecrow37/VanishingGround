#pragma once
#include "UmFrameWork.h"
#include <Interface/ITriggerType.h>

class CharacterBase;
class Player;
struct PlayerStats;
class Enemy;
struct EnemyStats;
struct WeaponStats;

//턴 라이프 사이클 사용을 위한 Base 클래스입니다.
class TurnAction abstract : public ReflectSerializer
{
    USING_PROPERTY(TurnAction)
    friend class TurnMode;
public:
    TurnAction() = default;
    virtual ~TurnAction()
    { 
        SetDestroy();
    }

    /// <summary>
    /// 이 액션을 라이프 사이클에서 제외합니다.
    /// </summary>
    void SetDestroy()
    {
        if (_isDestroy)
        {
            *_isDestroy = true;
        }
    }

    /// <summary>
    /// 이 액션의 life cycle이 활성화 되어있는지 확인합니다.
    /// </summary>
    bool IsValidAction() { return _isDestroy != nullptr; }

public:
    /*Action의 이름을 반환해야합니다.*/
    virtual const std::string& GetActionName() = 0;

    /*Action의 효과를 정의하는 내용을 반환해야합니다.*/
    virtual const std::string& GetActionInfo() = 0;

    /*편집을 위한 ImGui 함수를 구현해야합니다.*/
    virtual void ImGuiDrawActionEditor() = 0;

    /// <summary>전투가 시작될 때 호출됩니다.</summary>
    virtual void OnCombatStart() {}

    /// <summary>라운드가 시작될 때 호출됩니다.</summary>
    virtual void OnRoundStart() {}

    /// <summary>라운드가 끝날 때 호출됩니다.</summary>
    virtual void OnRoundEnd() {}

    /// <summary>인자로 넘어온 캐릭터의 턴이 시작되면 호출됩니다..</summary>
    /// <param name="destination">턴이 시작된 대상</param>
    virtual void OnTurnStart(CharacterBase* destination) {}

    /// <summary>인자로 넘어온 캐릭터의 턴이 종료되면 호출됩니다.</summary>
    virtual void OnTurnEnd(CharacterBase* destination) {}

    /// <summary>
    /// 플레이어가 배틀 데미지 계산 전에 호출됩니다.
    /// </summary>
    /// <param name="attacker :">플레이어 컴포넌트</param>
    /// <param name="attackerStats :">실제 데미지 계산에 사용될 스텟</param>
    /// <param name="weaponStats :">실제 데미지 계산에 사용될 스텟</param>
    /// <param name="target :">공격 당하는 적 컴포넌트</param>
    /// <param name="targetStats :">실제 데미지 계산에 사용될 스텟</param>
    virtual void OnPlayerBattleStart(Player& attacker, PlayerStats& attackerStats, WeaponStats& weaponStats, Enemy& target, EnemyStats& targetStats) {}

    /// <summary>
    /// 적이 배틀데미지 계산 직전에 호출됩니다.
    /// </summary>
    /// <param name="attacker :">적 컴포넌트</param>
    /// <param name="attackerStats :">실제 계산에 사용되는 적 스텟</param>
    /// <param name="target :">플레이어 컴포넌트</param>
    /// <param name="targetStats :">실제 계산에 사용되는 플레이어 스텟</param>
    virtual void OnEnemyBattleStart(Enemy& attacker, EnemyStats& attackerStats, Player& target, PlayerStats& targetStats) {}

public:
    REFLECT_PROPERTY(Name)

    GETTER_ONLY(const std::string&, Name) { return GetActionName(); }
    // 계시 이름
    PROPERTY(Name)

protected:
    REFLECT_FIELDS_BEGIN(ReflectSerializer)
    REFLECT_FIELDS_END(TurnAction)

private:
    bool* _isDestroy = nullptr;

};