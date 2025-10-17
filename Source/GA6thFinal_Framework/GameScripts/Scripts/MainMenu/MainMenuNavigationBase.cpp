#include "pchScripts.h"
#include "MainMenuNavigationBase.h"
#include "UI/Elements/Text/TextElement.h"

UMREAL_COMPONENT(MainMenuNavigationBase)

MainMenuNavigationBase::MainMenuNavigationBase() = default;

MainMenuNavigationBase::~MainMenuNavigationBase() = default;

void MainMenuNavigationBase::Awake()
{
    _textElement = GetComponent<TextElement>();
    if (_textElement)
    {
        _color = _textElement->Color;
    }
}

void MainMenuNavigationBase::FocusIn(const FocusCallType callType)
{
    Base::FocusIn(callType);
    if (_textElement)
    {
        Color color         = _color;
        color.w             = 1.f;
        _textElement->Color = color;
        _textElement->FontWeight = 1.0f;
    }
}

void MainMenuNavigationBase::FocusOut(FocusCallType callType)
{
    if (_textElement)
    {
        _textElement->Color = _color;
        _textElement->FontWeight = 0.5f;
    }
}
