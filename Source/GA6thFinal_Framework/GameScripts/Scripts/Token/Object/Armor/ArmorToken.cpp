#include "pchScripts.h"
#include "ArmorToken.h"
#include "TurnSystem/TurnActor/Character/CharacterBase.h"
#include "Stats/Enemy/EnemyStats.h"
#include "Stats/Player/PlayerStats.h"

namespace TokenObject
{
    REGISTER_TOKEN(Armor1)
    REGISTER_TOKEN(Armor2)
    REGISTER_TOKEN(Armor3)

    void Armor::OnTakeDamage(CharacterBase* source, CharacterBase* dest, int& damage, QTE::NoteResult* noteResult) 
    {
        int   tokenID   = GetTokenID();
        int   param     = GetTokenParam(0);
        float factor    = static_cast<float>(param) / 100.0f;
        float newDamage = static_cast<float>(damage) * (1.0f - factor);
        damage          = static_cast<int>(std::ceilf(newDamage));
        if (source)
        {
            TokenInventory& tokenInventory = source->GetTokenInventory();
            tokenInventory.RemoveTokenStackFromID(tokenID);
        }
    }
} // namespace TokenObject