#pragma once
#include <TurnSystem/TurnAction/TurnActionFactory.h>

class CharacterBase;
class AdditionalDamageAction : public TurnAction
{
    USING_PROPERTY(AdditionalDamageAction)
public:
    AdditionalDamageAction();
    ~AdditionalDamageAction() override;

    REFLECT_PROPERTY(AdditionalDamage, AdditionalDamageMultiplier, 
        AdditionalCriticalDamage, AdditionalCriticalDamageMultiplier)

    GETTER(int, AdditionalDamage) { return ReflectFields->AdditionalDamage; }
    SETTER(int, AdditionalDamage) 
    { 
        ReflectFields->AdditionalDamage = value;
        UpdateActionInfo();
    } 
    //int : 피해 증가량
    PROPERTY(AdditionalDamage)

    GETTER(float, AdditionalDamageMultiplier) { return ReflectFields->AdditionalDamageMultiplier; }
    SETTER(float, AdditionalDamageMultiplier) 
    { 
        ReflectFields->AdditionalDamageMultiplier = value;
        UpdateActionInfo();
    } 
    //float : 피해 배율 증가량
    PROPERTY(AdditionalDamageMultiplier)

    GETTER(int, AdditionalCriticalDamage) { return ReflectFields->AdditionalCriticalDamage; }
    SETTER(int, AdditionalCriticalDamage) 
    { 
        ReflectFields->AdditionalCriticalDamage = value;
        UpdateActionInfo();
    } 
    //int : 치명타 피해 증가량
    PROPERTY(AdditionalCriticalDamage)

    GETTER(float, AdditionalCriticalDamageMultiplier) { return ReflectFields->AdditionalCriticalDamageMultiplier; }
    SETTER(float, AdditionalCriticalDamageMultiplier) 
    { 
        ReflectFields->AdditionalCriticalDamageMultiplier = value;
        UpdateActionInfo();
    } 
    //float : 치명타 피해 배율 증가량
    PROPERTY(AdditionalCriticalDamageMultiplier)

protected:
    REFLECT_FIELDS_BEGIN(TurnAction)
    int AdditionalDamage = 0; //피해 증가량
    float AdditionalDamageMultiplier = 1.f; //피해 배율 증가량
    int AdditionalCriticalDamage = 0; //피해 증가량
    float AdditionalCriticalDamageMultiplier = 1.f; //피해 배율 증가량
    REFLECT_FIELDS_END(AdditionalDamageAction)

    const std::string& GetActionName() override;
    const std::string& GetActionInfo() override;
    void ImGuiDrawActionEditor() override;
    void DeserializedReflectEvent() override;

    void OnPlayerBattleCalculateDamageModifier(Player& attacker, PlayerStats& attackerStats, WeaponStats& weaponStats, Enemy& target,
                             EnemyStats& targetStats) override;

    void OnConvertWeaponViewModel(WeaponStats& stats) override;

private:
    void UpdateActionInfo();
    std::string _actionInfo; 

};