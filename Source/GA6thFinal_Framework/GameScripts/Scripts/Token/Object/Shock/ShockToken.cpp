#include "pchScripts.h"
#include "ShockToken.h"

#include <Token/TokenInventory.h>
#include <Token/Object/Stun/StunToken.h>
#include <TurnSystem/TurnActor/Character/CharacterBase.h>
namespace TokenObject
{
    REGISTER_TOKEN(Shock)

    void Shock::OnTurnStart(CharacterBase* owner)
    {
        //auto& tokenInventory = owner->GetTokenInventory();
        //int   stackCount     = tokenInventory.GetTokenStackFromID(ID);
        //float resistanceRate = 1.0f;
        //if (owner)
        //{
        //    if (true == tokenInventory.HasToken(ShockResistance1::ID))
        //    {
        //        resistanceRate += ShockResistance1::Data::ResistanceRate;
        //    }
        //    if (true == tokenInventory.HasToken(ShockResistance2::ID))
        //    {
        //        resistanceRate += ShockResistance2::Data::ResistanceRate;
        //    }
        //    if (true == tokenInventory.HasToken(ShockResistance3::ID))
        //    {
        //        resistanceRate += ShockResistance3::Data::ResistanceRate;
        //    }
        //    int triggerCount = (int)((float)ReflectFields->TransitionTriggerCount * resistanceRate);
        //
        //    if (stackCount >= triggerCount)
        //    {
        //        // 충격 토큰이 요구 조건만큼 쌓였으므로 기절 토큰을 부여.
        //        tokenInventory.AddTokenStackFromID(Stun::ID);
        //        // 충격 토큰은 제거합니다.
        //        tokenInventory.RemoveTokenFromID(ID);
        //    }
        //    else
        //    { // 충격 토큰이 요구 조건만큼 쌓이지 않았으므로 충격 토큰을 1개 제거.
        //        tokenInventory.RemoveTokenStackFromID(ID, 1);
        //    }
        //}
    }
} // namespace TokenObject