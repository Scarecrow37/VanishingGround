#pragma once
#include "TurnSystem/TurnAction/Actions/TokenApplyAction/Base/TokenApplyAction.h"

//특정 토큰 부여될때 추가 부여
class TokenGrantorAction : public TokenApplyAction
{
    inline static const std::string NAME = (const char*)u8"특정 캐릭터에게 특정 토큰 부여될때 추가 부여";

public:
    TokenGrantorAction() = default;
    ~TokenGrantorAction() override = default;

protected:
    // TokenApplyAction을(를) 통해 상속됨
    const std::string& GetActionName() override;
    const std::string& GetActionInfo() override;
    void               ImGuiDrawActionEditor() override;

    void OnTokenAddedStart(CharacterBase& target, int& tokenID, int& tokenCount) override;
}; 