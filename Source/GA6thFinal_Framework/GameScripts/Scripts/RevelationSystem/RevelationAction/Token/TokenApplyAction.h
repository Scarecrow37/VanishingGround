#pragma once
#include <TurnSystem/TurnAction/TurnActionFactory.h>

class TokenApplyAction : public TurnAction
{
    USING_PROPERTY(TokenApplyAction)
public:
    TokenApplyAction();
    ~TokenApplyAction() override = default;
    REFLECT_PROPERTY()

protected:
    REFLECT_FIELDS_BEGIN(TurnAction)
    REFLECT_FIELDS_END(TokenApplyAction)

public:
    const std::string& GetActionInfo() override;
    void               ImGuiDrawActionEditor() override;
    const std::string& GetActionName() override;
};