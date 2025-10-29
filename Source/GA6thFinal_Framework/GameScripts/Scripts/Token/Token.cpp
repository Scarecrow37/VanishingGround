#include "pchScripts.h"
#include "Token.h"
#include "TurnSystem/TurnActor/Character/CharacterBase.h"

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

int Token::GetTokenParam(size_t index) const
{
    if (index < _tokenData.Params.size())
    {
        return _tokenData.Params[index];
    }
    return 0;
}
