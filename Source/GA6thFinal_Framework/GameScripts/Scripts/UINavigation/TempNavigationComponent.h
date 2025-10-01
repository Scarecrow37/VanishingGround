#pragma once

class TempNavigationComponent : public UINavigationComponent
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
    REFLECT_FIELDS_BEGIN(UINavigationComponent)
    REFLECT_FIELDS_END(TempNavigationComponent)
};