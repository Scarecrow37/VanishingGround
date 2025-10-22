#include "pchScripts.h"
#include "PoisonToken.h"

#include <TurnSystem/TurnActor/Character/CharacterBase.h>
#include <Token/TokenInventory.h>
namespace TokenObject
{
    REGISTER_TOKEN(Poison1)
    REGISTER_TOKEN(Poison2)
    REGISTER_TOKEN(Poison3)

    REFLECT_FUNCTION(Poison1)
    REFLECT_FUNCTION(Poison2)
    REFLECT_FUNCTION(Poison3)

    void Poison1::OnEachTurnStart(CharacterBase* owner, CharacterBase* destiantion)
    {
        auto& tokenInventory = owner->GetTokenInventory();
        int   stackCount     = tokenInventory.GetTokenStackFromID(ID);

        GameObject& gameObject = owner->gameObject;
        std::string msg = std::format("{}{} {}{}{}{}", gameObject.ToString(), (const char*)u8"에게서", GetTokenName(),
                                      (const char*)u8"의 토큰이 발동했습니다. (", stackCount, (const char*)u8"스택)");
        UmLogger.Log(LogLevel::LEVEL_WARNING, msg);
        // TODO: 데미지 적용
        tokenInventory.RemoveTokenStackFromID(ID);
    }

    void TokenObject::Poison2::OnEachTurnStart(CharacterBase* owner, CharacterBase* destination)
    {
        auto& tokenInventory = owner->GetTokenInventory();
        int   stackCount     = tokenInventory.GetTokenStackFromID(ID);

        GameObject& gameObject = owner->gameObject;
        std::string msg = std::format("{}{} {}{}{}{}", gameObject.ToString(), (const char*)u8"에게서", GetTokenName(),
                                      (const char*)u8"의 토큰이 발동했습니다. (", stackCount, (const char*)u8"스택)");
        UmLogger.Log(LogLevel::LEVEL_WARNING, msg);
        // TODO: 데미지 적용
        tokenInventory.RemoveTokenStackFromID(ID);
    }

    void TokenObject::Poison3::OnEachTurnStart(CharacterBase* owner, CharacterBase* destination)
    {
        auto& tokenInventory = owner->GetTokenInventory();
        int   stackCount     = tokenInventory.GetTokenStackFromID(ID);

        GameObject& gameObject = owner->gameObject;
        std::string msg = std::format("{}{} {}{}{}{}", gameObject.ToString(), (const char*)u8"에게서", GetTokenName(),
                                      (const char*)u8"의 토큰이 발동했습니다. (", stackCount, (const char*)u8"스택)");
        UmLogger.Log(LogLevel::LEVEL_WARNING, msg);
        // TODO: 데미지 적용
        tokenInventory.RemoveTokenStackFromID(ID);
    }
} // namespace TokenObject