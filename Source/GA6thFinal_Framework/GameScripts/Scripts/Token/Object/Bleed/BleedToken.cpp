#include "pchScripts.h"
#include "BleedToken.h"

#include <TurnSystem/TurnActor/Character/CharacterBase.h>
#include <Token/TokenInventory.h>
namespace TokenObject
{
    REGISTER_TOKEN(Bleed1)
    REGISTER_TOKEN(Bleed2)
    REGISTER_TOKEN(Bleed3)

    void Bleed1::OnTurnStart(CharacterBase* owner)
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

    void Bleed2::OnTurnStart(CharacterBase* owner)
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

    void Bleed3::OnTurnStart(CharacterBase* owner)
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