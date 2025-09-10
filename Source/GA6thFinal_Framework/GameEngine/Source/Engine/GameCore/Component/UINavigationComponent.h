#pragma once

struct F
{
    unsigned int a;
    unsigned char b;
};

class UINavigationComponent : public UIBaseComponent
{
    friend class UIManager;
    USING_PROPERTY(UINavigationComponent)

    static NavigationID _toID;

public:
    UINavigationComponent();

public:
    REFLECT_PROPERTY()

    GETTER_ONLY(UIComponent*, SiblingUI) { return GetSiblingUI(); }
    PROPERTY(SiblingUI)

    GETTER_ONLY(UIRoot*, Root)
    {
        const GameObject& obj = gameObject;
        return GetRoot(obj);
    }
    PROPERTY(Root)

protected:
    virtual void OnFocusIn();
    virtual void OnFocusOut();
    virtual void OnSubmit() {}

    UIComponent* GetSiblingUI() const;

    void ImGuiDrawPropertysEvent() override;
    void OnDrawDebugSelectedOverride() override;

    void OnAttachParent(GameObject* childGameObject) override;
    void OnDetachParent(GameObject* previousParentGameObject) override;

    void DeserializedReflectEvent() override;

private:
    static UIRoot* GetRoot(const GameObject& gameObject);
    void           AcquireNavigationID(UIRoot* root);
    void           ReleaseNavigationID(UIRoot* root);

    void ClearNavigationRoute();
    void AddNavigationRoute(const NavigationKey& key, NavigationID toID);
    void GetNavigatedId(const NavigationKey& key);

protected:
    REFLECT_FIELDS_BEGIN(UIBaseComponent)
    NavigationID               NavigationID = INVALID_NAVIGATION_ID;
    std::vector<F>             NavigationButtons;
    std::vector<unsigned char> NavigationStickBiases;
    F                          f;
    //std::vector<Input::Controller::StickBias> NavigationStickBiases;
    //std::vector<::NavigationID>               NavigationTo;
    REFLECT_FIELDS_END(UINavigationComponent)
};