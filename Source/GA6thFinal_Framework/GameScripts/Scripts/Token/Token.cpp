#include "pchScripts.h"
#include "Token.h"

Token::Token(int tokenID) : _tokenID(tokenID), _stackCount(0) {}
Token::~Token() {}

UINT8 Token::GetStackCount() const
{
    return _stackCount;
}

UINT8 Token::GetMaxStackCount() const
{
    return _maxStackCount;
}
UINT8 Token::GetTokenID() const
{
    return _tokenID;
}

void Token::ClearStack() 
{
    _stackCount = 0;
}

void Token::SetStack(UINT8 count)
{
    _stackCount = std::clamp(count, (UINT8)0, _maxStackCount);
}

void Token::AddStack(UINT8 count)
{
    _stackCount += count;
    _stackCount = std::min(_stackCount, _maxStackCount);
}

void Token::RemoveStack(UINT8 count)
{
    _stackCount = _stackCount <= count ? (UINT8)0 : _stackCount - count;
}

void Token::SetMaxStackCount(UINT8 maxStack)
{
    _maxStackCount = maxStack;
}
