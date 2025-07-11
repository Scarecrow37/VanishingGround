#pragma once
#include "../Base/RevelationActionBase.h"

class CriticalDamageAction : public RevelationActionBase
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
    REFLECT_FIELDS_BEGIN(RevelationActionBase)
    float AdditionalDamage = 0.1f; //치명타 피해 증가량
    REFLECT_FIELDS_END(CriticalDamageAction)

    std::string_view GetActionInfo() override;
    void ImGuiDrawActionEditor() override;

    void DeserializedReflectEvent() override;

private:
    void UpdateActionInfo();
    std::string _actionInfo; 

};