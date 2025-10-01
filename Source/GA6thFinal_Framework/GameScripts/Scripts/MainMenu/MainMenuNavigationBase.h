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
    void FocusIn(FocusCallType callType) override;
    void FocusOut(FocusCallType callType) override;

public:
    REFLECT_PROPERTY()

protected:
    REFLECT_FIELDS_BEGIN(UISFXNavigationComponent)
    REFLECT_FIELDS_END(MainMenuNavigationBase)

private:
    TextElement* _textElement;
    Color        _color;
};