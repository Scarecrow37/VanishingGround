#include "pchScripts.h"
#include "StunToken.h"

#include <TurnSystem/TurnActor/Character/CharacterBase.h>
#include <Token/TokenInventory.h>
namespace TokenObject
{
    REGISTER_TOKEN(Stun)
    REGISTER_TOKEN(StunResistance)

    void Stun::OnTurnStart(CharacterBase* owner)
    {
        // TODO: Actor 턴을 넘긴다. Actor의 턴 넘기기 플래그를 켠다?
    }
    void StunResistance::OnTokenRemoved(CharacterBase* owner, int tokenID) 
    {
        // 기절의 조건은 기절 저항 토큰이 제거되어 0이 되었을 때.
        if (owner)
        {
            // 토큰이 제거 될 경우, 자신 토큰이 없는지 확인한 후 없다면 기절 토큰 부여
            bool isSelf = (ID == tokenID);
            if (isSelf)
            {
                auto& tokenInventory = owner->GetTokenInventory();
                bool hasSelf = tokenInventory.HasTokenFromID(TokenObject::StunResistance::ID);
                if (false == hasSelf)
                {
                    tokenInventory.AddTokenStackFromID(TokenObject::Stun::ID);
                }
            }
        }
    }
} // namespace TokenObject