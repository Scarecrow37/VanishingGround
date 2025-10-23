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
        WEAPON_KILL_ENEMY,
    };

    // 대상
    enum class ActionTarget
    {
        SELF,
        PLAYER,
        ENEMY,
        ALL_ENEMIES,
        ALL
    };

    REFLECT_PROPERTY(Trigger, Target, RecoveryHP)

    SETTER(TriggerType, Trigger)
    {
        ReflectFields->Trigger = value;
        UpdateActionInfo();
    }
    GETTER(TriggerType, Trigger) { return ReflectFields->Trigger; }
    PROPERTY(Trigger)

    SETTER(ActionTarget, Target)
    {
        ReflectFields->Target = value;
        UpdateActionInfo();
    }
    GETTER(ActionTarget, Target) { return ReflectFields->Target; }
    PROPERTY(Target)

    SETTER(int, RecoveryHP)
    {   
        ReflectFields->RecoveryHP = value;
        UpdateActionInfo();
    }
    GETTER(int, RecoveryHP) { return ReflectFields->RecoveryHP; }
    PROPERTY(RecoveryHP)

protected:
    REFLECT_FIELDS_BEGIN(TurnAction)
    TriggerType Trigger = TriggerType::WEAPON_KILL_ENEMY;
    ActionTarget Target = ActionTarget::SELF;
    int RecoveryHP = 0;
    REFLECT_FIELDS_END(RecoveryAction)

private:
    const std::string& GetActionName() override;
    const std::string& GetActionInfo() override;
    void               ImGuiDrawActionEditor() override;
    void               DeserializedReflectEvent() override;
    void               UpdateActionInfo();

    void               OnEnemyDeadByWeapon(Enemy& enemy, WeaponElement& weapon) override;
    std::string        _actionInfo;
};
