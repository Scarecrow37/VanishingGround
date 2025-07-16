#pragma once
#include <TurnSystem/TurnAction/TurnActionFactory.h>

// 적용 조건
enum class CriticalDamageCondition
{
    // 항상
    ALWAYS,
    // 대상 출혈시
    TARGET_BLEED
};

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
    CriticalDamageCondition Condition = CriticalDamageCondition::ALWAYS; // 조건
    REFLECT_FIELDS_END(CriticalDamageAction)

    std::string_view GetActionName() override;
    std::string_view GetActionInfo() override;
    void ImGuiDrawActionEditor() override;
    void DeserializedReflectEvent() override;

private:
    void UpdateActionInfo();
    std::string _actionInfo; 

    /*조건 여부를 검사합니다.*/
    bool Evaluate(CriticalDamageCondition condition, CharacterBase* attacker, CharacterBase* target);

};