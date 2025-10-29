#pragma once
#include <TurnSystem/TurnAction/TurnActionFactory.h>

class RecoveryAction : public TurnAction
{
    USING_PROPERTY(RecoveryAction)
public: 
    RecoveryAction();
    ~RecoveryAction() override = default;

    enum class TriggerType
    {
        WEAPON_KILL_ENEMY,  // 무기 공격으로 적 처치 시
        ON_ATTACK,          // 공격할 때마다
    };

    enum class Unit
    {
        FLAT,
        PERCENT,
    };

    REFLECT_PROPERTY(Trigger, Target, RecoveryHP, RecoveryUnit)

    SETTER(TriggerType, Trigger)
    {
        ReflectFields->Trigger = value;
        UpdateActionInfo();
    }
    GETTER(TriggerType, Trigger) { return ReflectFields->Trigger; }
    PROPERTY(Trigger)

    SETTER(TurnTarget, Target)
    {
        ReflectFields->Target = value;
        UpdateActionInfo();
    }
    GETTER(TurnTarget, Target) { return ReflectFields->Target; }
    PROPERTY(Target)

    SETTER(int, RecoveryHP)
    {   
        ReflectFields->RecoveryHP = value;
        UpdateActionInfo();
    }
    GETTER(int, RecoveryHP) { return ReflectFields->RecoveryHP; }
    PROPERTY(RecoveryHP)

    SETTER(Unit, RecoveryUnit)
    {
        ReflectFields->RecoveryUnit = value;
        UpdateActionInfo();
    }
    GETTER(Unit, RecoveryUnit) { return ReflectFields->RecoveryUnit; }
    PROPERTY(RecoveryUnit)

protected:
    REFLECT_FIELDS_BEGIN(TurnAction)
    TriggerType Trigger      = TriggerType::WEAPON_KILL_ENEMY;
    TurnTarget  Target       = TurnTarget::SELF;
    int         RecoveryHP   = 0;
    Unit        RecoveryUnit = Unit::FLAT;
    REFLECT_FIELDS_END(RecoveryAction)

private:
    const std::string& GetActionName() override;
    const std::string& GetActionInfo() override;
    void               ImGuiDrawActionEditor() override;
    void               DeserializedReflectEvent() override;
    void               UpdateActionInfo();

    void ProcessHeal();
    void ProcessHeal(CharacterBase* target);

    void               OnPlayerTakeDamageStart(Player& target, int& damage) override;
    void               OnEnemyDeadByWeapon(Enemy& enemy, WeaponElement& weapon) override;
    std::string        _actionInfo;
};
