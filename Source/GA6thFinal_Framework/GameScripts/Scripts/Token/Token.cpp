#include "pchScripts.h"
#include "Token.h"

Token::Token() = default;

Token::~Token() = default;

UINT16 Token::GetStackCount() const
{
    return _stackCount;
}

UINT16 Token::GetMaxStackCount() const
{
    return ReflectFields->MaxStackCount;
}
 
void Token::ClearStack() 
{
    _stackCount = 0;
}

void Token::SetStack(UINT16 count)
{
    _stackCount = std::clamp(count, (UINT16)0, ReflectFields->MaxStackCount);
}

void Token::AddStack(UINT16 count)
{
    _stackCount += count;
    _stackCount = std::min(_stackCount, ReflectFields->MaxStackCount);
}

void Token::RemoveStack(UINT16 count)
{
    _stackCount = _stackCount <= count ? (UINT16)0 : _stackCount - count;
}

void Token::SetMaxStackCount(UINT16 maxStack)
{
    ReflectFields->MaxStackCount = maxStack;
}
