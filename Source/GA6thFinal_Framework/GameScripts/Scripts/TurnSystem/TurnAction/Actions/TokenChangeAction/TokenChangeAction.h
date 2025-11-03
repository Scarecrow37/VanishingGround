#pragma once
#include "TurnSystem/TurnAction/TurnActionFactory.h"

class TokenChangeAction : public TurnAction
{
    USING_PROPERTY(TokenChangeAction)
public:
    TokenChangeAction() = default;
    ~TokenChangeAction() override;

    REFLECT_PROPERTY()
protected:
    REFLECT_FIELDS_BEGIN(TurnAction)
    TurnTarget Target  = TurnTarget::PLAYER;
    int        TokenID = 16000;
    REFLECT_FIELDS_END(TokenChangeAction)

    const std::string& GetActionName() override;
    const std::string& GetActionInfo() override;
    void               ImGuiDrawActionEditor() override;

    void OnTokenAddedEnd(CharacterBase& target, int tokenID, int tokenCount) override;

private:
    std::string _actionInfo;
    bool        _validTokenSystem = false;

private:
    void UpdateActionInfo();
    void TryTokenSystemInfo();

};