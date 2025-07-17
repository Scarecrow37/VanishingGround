#include "pchScripts.h"
#include "BleedToken.h"

#include <TurnSystem/TurnActor/Character/CharacterBase.h>
#include <Token/TokenInventory.h>
namespace TokenObject
{
    REGISTER_TOKEN(Bleed)

    void Bleed::OnRoundStart(CharacterBase* owner)
    {
        if (owner)
        {
            auto& tokenInventory = owner->GetTokenInventory();
            int   stackCount     = tokenInventory.GetTokenStackFromID(ID);

            GameObject& gameObject = owner->gameObject;
            std::string msg =
                std::format("{}{} {}{}{}{}", gameObject.ToString(), (const char*)u8"에게서", GetTokenName(),
                            (const char*)u8"의 토큰이 발동했습니다. (", stackCount, (const char*)u8"스택)");
            UmLogger.Log(LogLevel::LEVEL_WARNING, msg);
            // TODO: 데미지 적용
            int damage = ReflectFields->TickDamage * stackCount;
            owner->TakeDamage(damage);

            tokenInventory.RemoveTokenStackFromID(ID);
        }
    }
} // namespace TokenObject