#include "pchScripts.h"
#include "ShockToken.h"

#include <Token/TokenSystem.h>
#include <Token/Object/Stun/StunToken.h>
#include <TurnSystem/TurnActor/Character/CharacterBase.h>
REGISTER_TOKEN(ShockToken)
REGISTER_TOKEN(ShockResistance1Token)
REGISTER_TOKEN(ShockResistance2Token)
REGISTER_TOKEN(ShockResistance3Token)

void ShockToken::OnTurnStart(CharacterBase* owner) 
{
    int   stackCount = (int)GetStackCount();
    float resistanceRate = 1.0f;
    if (owner)
    {
        auto& system = owner->GetTokenSystem();
        if (true == system.HasToken(ShockResistance1Token::ID))
        {
            resistanceRate += ShockResistance1Token::Data::ResistanceRate;
        }
        if (true == system.HasToken(ShockResistance2Token::ID))
        {
            resistanceRate += ShockResistance2Token::Data::ResistanceRate;
        }
        if (true == system.HasToken(ShockResistance3Token::ID))
        {
            resistanceRate += ShockResistance3Token::Data::ResistanceRate;
        }
        int triggerCount = (int)((float)ReflectFields->TransitionTriggerCount * resistanceRate);

        if (stackCount >= triggerCount)
        {
            // 충격 토큰이 요구 조건만큼 쌓였으므로 기절 토큰을 부여.
            system.AddTokenStackFromID(StunToken::ID);
            // 충격 토큰은 제거합니다.
            system.RemoveTokenFromID(GetTokenID());
        }
        else
        {   // 충격 토큰이 요구 조건만큼 쌓이지 않았으므로 충격 토큰을 1개 제거.
            RemoveStack();
        }
    }
    
}

void ShockResistance1Token::OnTurnStart(CharacterBase* owner) 
{
    RemoveStack();
}

void ShockResistance2Token::OnTurnStart(CharacterBase* owner) 
{
    RemoveStack();
}

void ShockResistance3Token::OnTurnStart(CharacterBase* owner) 
{
    RemoveStack();
}
