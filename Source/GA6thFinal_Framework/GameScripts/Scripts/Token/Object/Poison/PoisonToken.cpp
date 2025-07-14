#include "pchScripts.h"
#include "PoisonToken.h"

#include <TurnSystem/TurnActor/Character/CharacterBase.h>
#include <Token/TokenInventory.h>
REGISTER_TOKEN(Poison1Token)
REGISTER_TOKEN(Poison2Token)
REGISTER_TOKEN(Poison3Token)

void Poison1Token::OnTurnStart(CharacterBase* owner)
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
    // owner->TakeDamage(ReflectFields->BleedDamage); 
    RemoveStack();
}

void Poison2Token::OnTurnStart(CharacterBase* owner) 
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
    // owner->TakeDamage(ReflectFields->BleedDamage); 
    RemoveStack();
}

void Poison3Token::OnTurnStart(CharacterBase* owner) 
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
    // owner->TakeDamage(ReflectFields->BleedDamage); 
    RemoveStack();
}
