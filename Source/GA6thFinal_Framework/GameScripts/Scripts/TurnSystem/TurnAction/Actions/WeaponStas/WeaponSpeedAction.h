#pragma once
#include <TurnSystem/TurnAction/TurnActionFactory.h>

//사용한 무기 스피드 영구 증가
class WeaponSpeedAction : public TurnAction
{
    USING_PROPERTY(WeaponSpeedAction)
public:
    WeaponSpeedAction();
    ~WeaponSpeedAction() override = default;

    enum class TriggerType
    {
        QTE_END,
        ENEMY_DEAD_BY_WEAPON,
    };

public:
    GETTER(TriggerType, Trigger) { return ReflectFields->Trigger; }
    SETTER(TriggerType, Trigger)
    {
        ReflectFields->Trigger = value;
        UpdateActionInfo();
    }
    PROPERTY(Trigger)

    GETTER(int, Speed) { return ReflectFields->Speed; }
    SETTER(int, Speed)
    {
        ReflectFields->Speed = value;
        UpdateActionInfo();
    }
    PROPERTY(Speed)

    REFLECT_PROPERTY(Trigger, Speed)
protected:
    REFLECT_FIELDS_BEGIN(TurnAction)
    TriggerType Trigger = TriggerType::QTE_END;
    int         Speed   = 0;
    REFLECT_FIELDS_END(WeaponSpeedAction)

    const std::string& GetActionName() override;
    const std::string& GetActionInfo() override;
    void               ImGuiDrawActionEditor() override;

    void DeserializedReflectEvent() override;
    void OnPlayerQTEResult(Player& player, const QTE::OverallResult& result) override;
    void OnEnemyDeadByWeapon(Enemy& enemy, WeaponElement& weapon) override;

private:
    void        UpdateActionInfo();
    std::string _actionInfo;
};