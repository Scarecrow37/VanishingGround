#include "pchScripts.h"
#include "Token.h"

Token::Token() = default;

Token::~Token() = default;

int Token::GetMaxStackCount() const
{
    return ReflectFields->MaxStackCount;
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
