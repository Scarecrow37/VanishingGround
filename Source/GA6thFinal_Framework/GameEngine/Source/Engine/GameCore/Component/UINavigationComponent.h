#pragma once

struct NavigationInfo
{
    unsigned int  Button;
    unsigned char Bias;
    int           ToID;
};

struct NavigationInfoHash
{
    std::size_t operator()(const NavigationInfo& info) const noexcept
    {
        const std::size_t h1 = std::hash<unsigned int>{}(info.Button);
        const std::size_t h2 = std::hash<unsigned char>{}(info.Bias);
        return h1 ^ (h2 << 1);
    }
};

class UINavigationComponent : public UIBaseComponent
{
    friend class UIManager;
    USING_PROPERTY(UINavigationComponent)

    using NavigationRoutes = std::vector<std::tuple<unsigned int, unsigned char, ::NavigationID>>;

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

    GETTER_ONLY(NavigationID, ID) { return ReflectFields->NavigationID; }
    PROPERTY(ID)

public:
    virtual void FocusIn();
    virtual void FocusOut();
    virtual void Submit() {}

    void SetInitialFocus();
    void ResetInitialFocus();
    NavigationID GetNavigatedId(const NavigationKey& key);

protected:
    UIComponent* GetSiblingUI() const;

    void ImGuiDrawPropertysEvent() override;
    void OnDrawDebugSelectedOverride() override;

    void OnAttachParent(GameObject* childGameObject) override;
    void OnDetachParent(GameObject* previousParentGameObject) override;

    void DeserializedReflectEvent() override;

    void Reset() override;

private:
    static UIRoot* GetRoot(const GameObject& gameObject);
    void           AcquireNavigationID(UIRoot* root);
    void           ReleaseNavigationID(UIRoot* root);

    void ClearNavigationRoute();
    void AddNavigationRoute(const NavigationKey& key, NavigationID toID);

protected:
    REFLECT_FIELDS_BEGIN(UIBaseComponent)
    NavigationID    NavigationID = INVALID_NAVIGATION_ID;
    NavigationRoutes NavigationRoutes;
    bool            IsInitialFocus = false;
    REFLECT_FIELDS_END(UINavigationComponent)
};