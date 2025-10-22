#pragma once
#include "TurnSystem/TurnAction/TurnActionFactory.h"
#include "TurnSystem/TurnAction/TurnActionEnums.h"

class ApplyDamage : public TurnAction
{
    USING_PROPERTY(ApplyDamage)
public:
    ApplyDamage();
    ~ApplyDamage() override;

    enum class TriggerType
    {
        QTE_END,           // QTE 종료시
        WEAPON_KILL_ENEMY, // 적 처치시
    };

    inline static constexpr const char8_t* GetTriggerToolTip(TriggerType type) 
    {
        switch (type)
        {
        case ApplyDamage::TriggerType::QTE_END:
            return u8"QTE 종료시";
        case ApplyDamage::TriggerType::WEAPON_KILL_ENEMY:
            return u8"무기 공격으로적 처치시";
        default:
            return u8"NULL";
        }
    };

    GETTER(TurnTarget, Target) { return ReflectFields->Target; }
    SETTER(TurnTarget, Target) { ReflectFields->Target = value; }
    // type : TurnTarget
    // 적용 대상
    PROPERTY(Target)

    GETTER(int, Damage) { return ReflectFields->Damage; }
    SETTER(int, Damage) { ReflectFields->Damage = value; }
    // type : int
    // 데미지
    PROPERTY(Damage)
    
    REFLECT_PROPERTY()
protected:
    REFLECT_FIELDS_BEGIN(TurnAction)
    TriggerType Trigger = TriggerType::QTE_END;
    TurnTarget  Target  = TurnTarget::ENEMY;
    int         Damage  = 0;
    REFLECT_FIELDS_END(ApplyDamage)

    const std::string& GetActionName() override;
    const std::string& GetActionInfo() override;
    void ImGuiDrawActionEditor() override;
    void DeserializedReflectEvent() override;

    void OnPlayerQTEResult(Player& player, const QTE::OverallResult& result) override;
    void OnEnemyDeadByWeapon(Enemy& enemy, WeaponElement& weapon) override;

private:
    std::string _actionInfo;
    void UpdateInfoText();

};