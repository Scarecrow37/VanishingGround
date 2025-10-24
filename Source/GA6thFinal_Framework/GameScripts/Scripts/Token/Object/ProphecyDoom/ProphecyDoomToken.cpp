#include "pchScripts.h"
#include "ProphecyDoomToken.h"
#include "Token/TokenInventory.h"
#include "TurnSystem/TurnActor/Character/CharacterBase.h"

namespace TokenObject
{
    REGISTER_TOKEN(ProphecyDoom)
    // 데미지 증가 효과는 몬스터 액션에서 구현
    void ProphecyDoom::OnTurnEnd(CharacterBase* owner) 
    {
        if (owner)
        {
            auto& tokenInventory = owner->GetTokenInventory();
            int   tokenID        = GetTokenID();
            tokenInventory.RemoveTokenStackFromID(tokenID);
        }
    }
} // namespace TokenObject