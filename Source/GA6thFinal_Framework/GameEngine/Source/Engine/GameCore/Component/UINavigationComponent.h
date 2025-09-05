#pragma once

class UINavigationComponent : public UIBaseComponent
{
    friend class UIManager;
    USING_PROPERTY(UINavigationComponent)

public:
    UINavigationComponent();

public:
    REFLECT_PROPERTY()

protected:
    virtual void OnFocusIn() {}
    virtual void OnFocusOut() {}
    virtual void OnSubmit() {}

private:
    void OnDrawDebugSelectedOverride() override;

protected:
    REFLECT_FIELDS_BEGIN(UIBaseComponent)
    REFLECT_FIELDS_END(UINavigationComponent)
};