#include "pchScripts.h"
#include "Token.h"

UINT16 Token::GetStackCount() const
{
    return _stackCount;
}

UINT16 Token::GetMaxStackCount() const
{
    return _maxStackCount;
}
 
void Token::ClearStack() 
{
    _stackCount = 0;
}

void Token::SetStack(UINT16 count)
{
    _stackCount = std::clamp(count, (UINT16)0, _maxStackCount);
}

void Token::AddStack(UINT16 count)
{
    _stackCount += count;
    _stackCount = std::min(_stackCount, _maxStackCount);
}

void Token::RemoveStack(UINT16 count)
{
    _stackCount = _stackCount <= count ? (UINT16)0 : _stackCount - count;
}

void Token::SetMaxStackCount(UINT16 maxStack)
{
    _maxStackCount = maxStack;
}