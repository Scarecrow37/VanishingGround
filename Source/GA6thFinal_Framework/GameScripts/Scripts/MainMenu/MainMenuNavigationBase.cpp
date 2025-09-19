#include "pchScripts.h"
#include "MainMenuNavigationBase.h"
#include "UI/Elements/Text/TextElement.h"

UMREAL_COMPONENT(MainMenuNavigationBase)

MainMenuNavigationBase::MainMenuNavigationBase() = default;

MainMenuNavigationBase::~MainMenuNavigationBase() = default;

void MainMenuNavigationBase::Awake()
{
    _textElement = GetComponent<TextElement>();
}

void MainMenuNavigationBase::FocusIn()
{
    if (_textElement)
    {
        _textElement->Color = DirectX::SimpleMath::Color(0.0f, 1.0f, 1.0f, 1.0f);
    }
}

void MainMenuNavigationBase::FocusOut()
{
    if (_textElement)
    {
        _textElement->Color = DirectX::SimpleMath::Color(1.0f, 1.0f, 1.0f, 1.0f);
    }
}
