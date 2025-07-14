#include "pchScripts.h"
#include "StunToken.h"

#include <TurnSystem/TurnActor/Character/CharacterBase.h>
#include <Token/TokenInventory.h>
REGISTER_TOKEN(StunToken)

void StunToken::OnTurnStart(CharacterBase* owner) 
{
    // TODO: Actor 턴을 넘긴다? 이걸 여기서? 아니면 Actor코드에서?
    RemoveStack();
}
