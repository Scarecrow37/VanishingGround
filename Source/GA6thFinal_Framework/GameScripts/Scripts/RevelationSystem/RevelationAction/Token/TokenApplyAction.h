#pragma once
#include "../Base/RevelationActionBase.h"
#include <DamageSystem/Interface/IDamageAction.h>

class TokenApplyAction : public RevelationActionBase, public IDamageAction
{
    USING_PROPERTY(TokenApplyAction)
public:
    TokenApplyAction();
    ~TokenApplyAction() override = default;
    REFLECT_PROPERTY()

protected:
    REFLECT_FIELDS_BEGIN(RevelationActionBase)
    
    REFLECT_FIELDS_END(TokenApplyAction)


public:
    // RevelationActionBase을(를) 통해 상속됨
    std::string_view GetActionInfo() override;
    void             ImGuiDrawActionEditor() override;

    // IDamageAction을(를) 통해 상속됨
    void Execute(CharacterBase* attacker, CharacterBase* target) override;

};