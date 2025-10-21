#include "pchScripts.h"
#include "Token.h"

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
