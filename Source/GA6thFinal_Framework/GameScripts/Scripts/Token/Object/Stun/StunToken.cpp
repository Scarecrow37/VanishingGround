#include "pchScripts.h"
#include "StunToken.h"

#include <TurnSystem/TurnActor/Character/CharacterBase.h>
#include <Token/TokenInventory.h>
namespace TokenObject
{
    REGISTER_TOKEN(Stun)
    void Stun::OnTurnStart(CharacterBase* owner)
    {
        // TODO: Actor 턴을 넘긴다? 이걸 여기서? 아니면 Actor코드에서?
    }
    void StunResistance::OnTurnStart(CharacterBase* owner) 
    {

    }
} // namespace TokenObject