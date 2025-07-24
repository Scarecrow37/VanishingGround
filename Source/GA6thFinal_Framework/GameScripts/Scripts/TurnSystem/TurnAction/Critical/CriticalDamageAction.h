#pragma once
#include <TurnSystem/TurnAction/TurnActionFactory.h>

class CharacterBase;
class CriticalDamageAction : public TurnAction
{
    USING_PROPERTY(CriticalDamageAction)
public:
    CriticalDamageAction();
    ~CriticalDamageAction() override;

    REFLECT_PROPERTY(AdditionalDamage)

    GETTER(float, AdditionalDamage) { return ReflectFields->AdditionalDamage; }
    SETTER(float, AdditionalDamage) 
    { 
        ReflectFields->AdditionalDamage = value;
        UpdateActionInfo();
    } 
    // 치명타 피해 증가량
    PROPERTY(AdditionalDamage)

protected:
    REFLECT_FIELDS_BEGIN(TurnAction)
    float AdditionalDamage = 0.1f; //치명타 피해 증가량
    REFLECT_FIELDS_END(CriticalDamageAction)

    const std::string& GetActionName() override;
    const std::string& GetActionInfo() override;
    void ImGuiDrawActionEditor() override;
    void DeserializedReflectEvent() override;

    void OnPlayerBattleStart (Player& attacker, PlayerStats& attackerStats, WeaponStats& weaponStats, Enemy& target,
                             EnemyStats& targetStats) override;

    void OnEnemyBattleStart(Enemy& attacker, EnemyStats& attackerStats, Player& target,
                           PlayerStats& targetStats) override;

private:
    void UpdateActionInfo();
    std::string _actionInfo; 

};