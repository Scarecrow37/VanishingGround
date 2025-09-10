#pragma once

class UIRoot : public UIBaseComponent
{
    USING_PROPERTY(UIRoot)

    static Input::Controller* _controller;

public:
    UIRoot();

public:
    void SortViewOrder() const;

    NavigationID AcquireNavigationID();
    void         ReleaseNavigationID(NavigationID id);

    static std::optional<NavigationKey> GetPressedButton();

protected:
    void ImGuiDrawPropertysEvent() override;

    void Reset() override;

protected:
    REFLECT_FIELDS_BEGIN(UIBaseComponent)
    NavigationID LastID = 0;
    std::set<NavigationID> SpareID;
    REFLECT_FIELDS_END(UIRoot)
};