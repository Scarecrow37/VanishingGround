#pragma once

class TempNavigationComponent : public UISFXNavigationComponent
{
    USING_PROPERTY(TempNavigationComponent)

public:
    TempNavigationComponent();

public:
    REFLECT_PROPERTY();

protected:
    void FocusIn(FocusCallType callType) override;
    void FocusOut(FocusCallType callType) override;
    void Submit() override;

protected:
    REFLECT_FIELDS_BEGIN(UISFXNavigationComponent)
    REFLECT_FIELDS_END(TempNavigationComponent)
};