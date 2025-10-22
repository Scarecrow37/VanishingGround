#include "pchScripts.h"
#include "Token.h"
#include "TurnSystem/TurnActor/Character/CharacterBase.h"

REFLECT_FUNCTION(Token)

Token::Token() = default;

Token::~Token() = default;

int Token::GetMaxStackCount() const
{
    return ReflectFields->MaxStackCount;
}

TokenTag Token::GetTokenTag() const
{
    return ReflectFields->Tag;
}

bool Token::CanAdd(CharacterBase* owner) const
{
    return true;
}

bool Token::CanRemove(CharacterBase* owner) const
{
    return true;
}

int Token::GetTokenOrder() const
{
    return ReflectFields->Order;
}

void Token::SetMaxStackCount(UINT16 maxStack)
{
    ReflectFields->MaxStackCount = maxStack;
}

void Token::SetTokenOrder(int order)
{
    if (ReflectFields->Order != order)
    {
        ReflectFields->Order = order;
    }
    if (_dirtyOrderCallback)
    {
        _dirtyOrderCallback(GetTokenID());
    }
}

void Token::SetDirtyOrderCallback(std::function<void(int)> callback) 
{
    _dirtyOrderCallback = callback;
}

std::string Token::TokenLog(CharacterBase& dest)
{
    GameObject& gameObject = dest.gameObject;
    TokenInventory& tokenInventory = dest.GetTokenInventory();
    int stackCount = tokenInventory.GetTokenStackFromID(GetTokenID());
    return std::format("{}{} {}{}{}{}", gameObject.ToString(), (const char*)u8"에게서", GetTokenName(),
                       (const char*)u8"의 토큰이 발동했습니다. (", stackCount, (const char*)u8"스택)");
}
