#include "pchScripts.h"
#include "BleedToken.h"

#include<Token/TokenSystem.h>
REGISTER_TOKEN(BleedToken)

void BleedToken::OnRoundStart(CharacterBase* owner) {}

void BleedToken::OnRoundEnd(CharacterBase* owner) {}

void BleedToken::OnTurnStart(CharacterBase* owner) 
{
    UmLogger.Log(LogLevel::LEVEL_WARNING, (const char*)u8"출혈 발생!");
    // TODO: 출혈 데미지 적용
    // owner->TakeDamage(1); 
    RemoveStack();
}

void BleedToken::OnTurnEnd(CharacterBase* owner) {}

void BleedToken::OnHit(CharacterBase* owner) {}

void BleedToken::OnCombatStart(CharacterBase* source) {}

void BleedToken::OnDead(CharacterBase* owner) {}

void BleedToken::OnKill(CharacterBase* source, CharacterBase* destination) {}

void BleedToken::OnTokenAdded(CharacterBase* owner, int tokenID) {}

void BleedToken::OnTokenRemoved(CharacterBase* owner, int tokenID) {}
