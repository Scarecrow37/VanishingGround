#pragma once
#include <TurnSystem/TurnAction/TurnActionFactory.h>

class WeaponDamageAction : public TurnAction
{
    USING_PROPERTY(WeaponDamageAction)
public:
    WeaponDamageAction();
    ~WeaponDamageAction() override = default;

    enum class TriggerType
    {
        QTE_END,
    };

public:
    GETTER(TriggerType, Trigger) { return ReflectFields->Trigger; }
    SETTER(TriggerType, Trigger)
    {
        ReflectFields->Trigger = value;
        UpdateActionInfo();
    }
    PROPERTY(Trigger)

    GETTER(int, Damage) { return ReflectFields->Damage; }
    SETTER(int, Damage)
    {
        ReflectFields->Damage = value;
        UpdateActionInfo();
    }
    PROPERTY(Damage)

    GETTER(float, DamageMultiplier) { return ReflectFields->DamageMultiplier; }
    SETTER(float, DamageMultiplier)
    {
        ReflectFields->DamageMultiplier = value;
        UpdateActionInfo();
    }
    PROPERTY(DamageMultiplier)

    GETTER(int, CriticalDamage) { return ReflectFields->CriticalDamage; }
    SETTER(int, CriticalDamage)
    {
        ReflectFields->CriticalDamage = value;
        UpdateActionInfo();
    }
    PROPERTY(CriticalDamage)

    GETTER(float, CriticalDamageMultiplier) { return ReflectFields->CriticalDamageMultiplier; }
    SETTER(float, CriticalDamageMultiplier)
    {
        ReflectFields->CriticalDamageMultiplier = value;
        UpdateActionInfo();
    }
    PROPERTY(CriticalDamageMultiplier)

    REFLECT_PROPERTY(Trigger, Damage, DamageMultiplier, CriticalDamage, CriticalDamageMultiplier)
protected:
    REFLECT_FIELDS_BEGIN(TurnAction)
    TriggerType Trigger = TriggerType::QTE_END;
    int         Damage                   = 0;
    float       DamageMultiplier         = 1.f;
    int         CriticalDamage           = 0;
    float       CriticalDamageMultiplier = 1.f;
    REFLECT_FIELDS_END(WeaponDamageAction)

    const std::string& GetActionName() override;
    const std::string& GetActionInfo() override;
    void               ImGuiDrawActionEditor() override;

    void DeserializedReflectEvent() override;
    void OnPlayerQTEResult(Player& player, const QTE::OverallResult& result) override; 

private:
    void UpdateActionInfo();
    std::string _actionInfo;
    
};