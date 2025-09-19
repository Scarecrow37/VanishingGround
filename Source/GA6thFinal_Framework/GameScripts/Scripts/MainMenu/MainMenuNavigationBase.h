#pragma once

class TextElement;
class MainMenuNavigationBase : public UINavigationComponent
{
    USING_PROPERTY(MainMenuNavigationBase)

public:
    MainMenuNavigationBase();
    ~MainMenuNavigationBase() override;

public:
    void Awake() override;

public:
    void FocusIn() override;
    void FocusOut() override;

public:
    REFLECT_PROPERTY()

protected:
    REFLECT_FIELDS_BEGIN(UINavigationComponent)
    REFLECT_FIELDS_END(MainMenuNavigationBase)

private:
    TextElement* _textElement;
};