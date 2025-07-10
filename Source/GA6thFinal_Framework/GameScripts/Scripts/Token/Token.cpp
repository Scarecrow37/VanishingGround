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

int Token::GetTokenOrder() const
{
    return ReflectFields->Order;
}
 
void Token::ClearStack() 
{
    _stackCount = 0;
}

void Token::SetStack(UINT16 count)
{
    UINT16 resultCount = std::clamp(count, (UINT16)0, ReflectFields->MaxStackCount);
    if (resultCount != _stackCount)
    {
        _stackCount = resultCount;
        if (_dirtyCountCallback)
        {
            _dirtyCountCallback(GetTokenID()); // 스택이 변경되었을 때 콜백 호출
        }
    }
}

void Token::AddStack(UINT16 count)
{
    if (0 < count)
    {
        UINT16 resultCount = std::min((UINT16)(_stackCount + count), ReflectFields->MaxStackCount);
        if (resultCount != _stackCount)
        {
            _stackCount = resultCount;
            if (_dirtyCountCallback)
            {
                _dirtyCountCallback(GetTokenID()); // 스택이 변경되었을 때 콜백 호출
            }
        }
    }
}

void Token::RemoveStack(UINT16 count)
{
    if (0 < count)
    {
        UINT16 resultCount = _stackCount <= count ? (UINT16)0 : (UINT16)(_stackCount - count);
        if (resultCount != _stackCount)
        {
            _stackCount = resultCount;
            if (_dirtyCountCallback)
            {
                _dirtyCountCallback(GetTokenID()); // 스택이 변경되었을 때 콜백 호출
            }
        }
    }
}

void Token::SetMaxStackCount(UINT16 maxStack)
{
    ReflectFields->MaxStackCount = maxStack;
}

void Token::SetDirtyCountCallback(std::function<void(int)> callback)
{
    _dirtyCountCallback = callback;
}

void Token::SetDirtyOrderCallback(std::function<void(int)> callback) 
{
    _dirtyOrderCallback = callback;
}

void Token::SetTokenOrder(int order)
{
    if (ReflectFields->Order != order)
    {
        ReflectFields->Order = order;
        if (_dirtyOrderCallback)
        {
            _dirtyOrderCallback(GetTokenID()); // 순서가 변경되었을 때 콜백 호출
        }
    }
}
