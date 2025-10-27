#include "pchScripts.h"
#include "TokenHelper.h"
#include "TurnSystem/TurnActor/Character/CharacterBase.h"

bool TokenHelper::HasTokenFromID(CharacterBase* owner, TokenID tokenID)
{
    if (owner)
    {
        return owner->GetTokenInventory().HasTokenFromID(tokenID);
    }
    return false;
}

bool TokenHelper::HasTokenFromTag(CharacterBase* owner, const std::string& tag)
{
    if (owner)
    {
        return owner->GetTokenInventory().HasTokenFromTag(tag);
    }
    return false;
}
