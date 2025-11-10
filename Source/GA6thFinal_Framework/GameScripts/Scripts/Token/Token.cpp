#include "pchScripts.h"
#include "Token.h"
#include "TurnSystem/TurnActor/Character/Player/Player.h"
#include "TurnSystem/TurnActor/Character/Enemy/Enemy.h"
#include "TurnSystem/TurnMode/TurnMode.h"
Token::Token() = default;

Token::~Token() = default;

bool Token::CanAdd(CharacterBase* owner) const
{
    return true;
}

bool Token::CanRemove(CharacterBase* owner) const
{
    return true;
}

std::string Token::TokenLog(CharacterBase& dest)
{
    GameObject& gameObject = dest.gameObject;
    TokenInventory& tokenInventory = dest.GetTokenInventory();
    int stackCount = tokenInventory.GetTokenStackFromID(GetTokenID());
    return std::format("{}{} {}{}{}{}", gameObject.ToString(), (const char*)u8"에게서", GetTokenName(),
                       (const char*)u8"의 토큰이 발동했습니다. (", stackCount, (const char*)u8"스택)");
}

void Token::TakeDamage(CharacterBase* dest, int damage) 
{
    if (dest)
    {
        if (TurnMode* turnMode = SingletonComponent<TurnMode>::GetInstance())
        {
            TokenInventory& tokenInvectory = dest->GetTokenInventory();
            const int tokenID = GetTokenID();
            const int tokenCount = tokenInvectory.GetTokenStackFromID(tokenID);
            turnMode->ApplyActions([&](TurnAction& action) {
                if (typeid(Player) == typeid(*dest))
                {
                    action.OnPlayerTokenTakeDamage(tokenID, tokenCount, damage);
                }
                else if (typeid(Enemy) == typeid(*dest))
                {
                    action.OnEnemyTokenTakeDamage(tokenID, tokenCount, damage);
                }
            });
        }
        dest->TakeDamage(damage, false);
    }
}

int Token::GetTokenParam(size_t index) const
{
    if (index < _tokenData.Params.size())
    {
        return _tokenData.Params[index];
    }
    return 0;
}
