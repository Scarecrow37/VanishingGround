#include "pchScripts.h"
#include "Bleed1Token.h"
#include "TurnSystem/TurnActor/Character/CharacterBase.h"
#include<Token/TokenSystem.h>
REGISTER_TOKEN(Bleed1Token)

void Bleed1Token::OnRoundStart(CharacterBase* owner) {}

void Bleed1Token::OnRoundEnd(CharacterBase* owner) {}

void Bleed1Token::OnTurnStart(CharacterBase* owner) 
{
    UmLogger.Log(LogLevel::LEVEL_WARNING, (const char*)u8"출혈 발생!");
    // TODO: 출혈 데미지 적용
    // owner->TakeDamage(1); 
    auto& system = owner->GetTokenSystem();
    system.RemoveTokenStackFromID(ID, 1);
}

void Bleed1Token::OnTurnEnd(CharacterBase* owner) {}

void Bleed1Token::OnHit(CharacterBase* owner) {}

void Bleed1Token::OnCombatStart(CharacterBase* source) {}

void Bleed1Token::OnDead(CharacterBase* owner) {}

void Bleed1Token::OnKill(CharacterBase* source, CharacterBase* destination) {}

void Bleed1Token::OnTokenAdded(CharacterBase* owner, int tokenID) {}

void Bleed1Token::OnTokenRemoved(CharacterBase* owner, int tokenID) {}
