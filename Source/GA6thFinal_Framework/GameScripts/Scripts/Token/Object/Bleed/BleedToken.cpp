#include "pchScripts.h"
#include "BleedToken.h"

#include <TurnSystem/TurnActor/Character/CharacterBase.h>
#include <Token/TokenManager.h>
REGISTER_TOKEN(Bleed1Token)
REGISTER_TOKEN(Bleed2Token)
REGISTER_TOKEN(Bleed3Token)

void Bleed1Token::OnTurnStart(CharacterBase* owner)
{
    GameObject& gameObject = owner->gameObject;
    std::string msg = std::format("{}{} {}{}{}{}",
        gameObject.ToString(),
        (const char*)u8"에게서",
        GetTokenName(),
        (const char*)u8"의 토큰이 발동했습니다. (",
        GetStackCount(),
        (const char*)u8"스택)"
    );
    UmLogger.Log(LogLevel::LEVEL_WARNING, msg);
    // TODO: 출혈 데미지 적용
    // owner->TakeDamage(ReflectFields->TickDamage); 
    RemoveStack();
}

void Bleed2Token::OnTurnStart(CharacterBase* owner)
{
    GameObject& gameObject = owner->gameObject;
    std::string msg = std::format("{}{} {}{}{}{}",
        gameObject.ToString(),
        (const char*)u8"에게서",
        GetTokenName(),
        (const char*)u8"의 토큰이 발동했습니다. (",
        GetStackCount(),
        (const char*)u8"스택)"
    );
    UmLogger.Log(LogLevel::LEVEL_WARNING, msg);
    // TODO: 출혈 데미지 적용
    // owner->TakeDamage(ReflectFields->TickDamage);
    RemoveStack();
}

void Bleed3Token::OnTurnStart(CharacterBase* owner)
{
    GameObject& gameObject = owner->gameObject;
    std::string msg = std::format("{}{} {}{}{}{}",
        gameObject.ToString(),
        (const char*)u8"에게서",
        GetTokenName(),
        (const char*)u8"의 토큰이 발동했습니다. (",
        GetStackCount(),
        (const char*)u8"스택)"
    );
    UmLogger.Log(LogLevel::LEVEL_WARNING, msg);
    // TODO: 출혈 데미지 적용
    // owner->TakeDamage(ReflectFields->TickDamage);
    RemoveStack();
}
