#include "pchScripts.h"
#include "BlackArmorToken.h"
#include "Token/TokenInventory.h"
#include "TurnSystem/TurnActor/Character/CharacterBase.h"
#include "Token/Object/Stun/StunToken.h"

namespace TokenObject
{
    REGISTER_TOKEN(BlackArmor)

    void BlackArmor::OnHit(CharacterBase* owner)
    {
        if (owner)
        {
            auto& tokenInventory = owner->GetTokenInventory();
            int   tokenID        = GetTokenID();
            tokenInventory.RemoveTokenStackFromID(tokenID);
            // 토큰이 0이 되면 기절 저항 제거하고 기절 부여
            if (false == tokenInventory.HasTokenFromID(tokenID))
            {
                int param = GetTokenParam(0);
                tokenInventory.RemoveTokenFromID(TokenObject::StunResistance::ID);
                tokenInventory.AddTokenStackFromID(TokenObject::Stun::ID, param);
            }
            UmLogger.Log(LogLevel::LEVEL_TRACE, TokenLog(*owner));
        }
    }
} // namespace TokenObject