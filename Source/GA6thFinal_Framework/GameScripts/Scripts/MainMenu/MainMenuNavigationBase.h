#pragma once

class TextElement;
class MainMenuNavigationBase : public UISFXNavigationComponent
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
    REFLECT_FIELDS_BEGIN(UISFXNavigationComponent)
    REFLECT_FIELDS_END(MainMenuNavigationBase)

private:
    TextElement* _textElement;
};