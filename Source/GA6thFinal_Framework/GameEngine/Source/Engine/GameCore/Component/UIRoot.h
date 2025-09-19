#pragma once
#include "Engine/GameCore/InputReciver/InputReceiver.h"

class UINavigationComponent;

class UIRoot : public UIBaseComponent
{
    USING_PROPERTY(UIRoot)

public:
    static std::optional<NavigationKey> GetPressedButton();

private:
    static Input::Controller* _controller;


public:
    UIRoot();

public:
    void Update() override;

    void SortViewOrder() const;
    void SetInitialFocus(const UINavigationComponent* uiComponent);
    void ChangeFocusComponent(UINavigationComponent* nextFocusComponent);

    NavigationID           AcquireNavigationID(NavigationID tempID);
    NavigationID           ReleaseNavigationID(NavigationID id);
    UINavigationComponent* FindNavigationComponent(NavigationID id);

protected:
    void ImGuiDrawPropertysEvent() override;
    void Reset() override;
    void Awake() override;

private:
    void UpdateNavigation();
    void UpdateNavigationMap();
    void ChangeNavigationID(NavigationID from, NavigationID to);

    NavigationID           GetSpareID();
    UINavigationComponent* FindNavigationComponentInTransform(NavigationID id) const;

protected:
    REFLECT_FIELDS_BEGIN(UIBaseComponent)
    NavigationID LastID = INVALID_NAVIGATION_ID + 1;
    std::unordered_set<NavigationID> SpareID;
    NavigationID           InitialFocusID = INVALID_NAVIGATION_ID;
    REFLECT_FIELDS_END(UIRoot)

    UINavigationComponent* _currentFocusNavigation;
    std::unordered_map<NavigationID, UINavigationComponent*> _navigationMap;
};