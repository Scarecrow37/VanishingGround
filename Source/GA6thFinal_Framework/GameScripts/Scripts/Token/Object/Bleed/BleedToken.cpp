#include "pchScripts.h"
#include "BleedToken.h"
#include "TurnSystem/TurnActor/Character/CharacterBase.h"

#include<Token/TokenSystem.h>
REGISTER_TOKEN(Bleed1Token)
REGISTER_TOKEN(Bleed2Token)
REGISTER_TOKEN(Bleed3Token)

void Bleed1Token::OnTurnStart(CharacterBase* owner)
{
    std::string msg = std::format("{}{} {}{} {}{}",
        owner->gameObject->ToString(),
        "에게서",
        (const char*)NAME,
        (const char*)u8"의 토큰이 발동했습니다. (",
        GetStackCount(),
        (const char*)u8"스택)"
    );
    UmLogger.Log(LogLevel::LEVEL_WARNING, msg);
    // TODO: 출혈 데미지 적용
    // owner->TakeDamage(1); 
    auto& system = owner->GetTokenSystem();
    system.RemoveTokenStackFromID(ID, 1);
}

void Bleed2Token::OnTurnStart(CharacterBase* owner)
{
   std::string msg = std::format("{}{} {}{} {}{}",
        owner->gameObject->ToString(),
        "에게서",
        (const char*)NAME,
        (const char*)u8"의 토큰이 발동했습니다. (",
        GetStackCount(),
        (const char*)u8"스택)"
    );
    UmLogger.Log(LogLevel::LEVEL_WARNING, msg);
    // TODO: 출혈 데미지 적용
    // owner->TakeDamage(1);
    auto& system = owner->GetTokenSystem();
    system.RemoveTokenStackFromID(ID, 1);
}

void Bleed3Token::OnTurnStart(CharacterBase* owner)
{
    std::string msg = std::format("{}{} {}{} {}{}",
        owner->gameObject->ToString(),
        "에게서",
        (const char*)NAME,
        (const char*)u8"의 토큰이 발동했습니다. (",
        GetStackCount(),
        (const char*)u8"스택)"
    );
    UmLogger.Log(LogLevel::LEVEL_WARNING, msg);
    // TODO: 출혈 데미지 적용
    // owner->TakeDamage(1);
    auto& system = owner->GetTokenSystem();
    system.RemoveTokenStackFromID(ID, 1);
}
