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

    using NavigationRoutes = std::vector<std::tuple<unsigned int, unsigned char, std::string, ::NavigationID>>;

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
    /// <summary>
    /// 포커스를 설정합니다.
    /// </summary>
    void Focus();

    /// <summary>
    /// 포커스가 들어올 때 호출되는 함수입니다.
    /// </summary>
    virtual void FocusIn();

    /// <summary>
    /// 포커스가 해당 객체에서 벗어날 때 호출되는 함수입니다.
    /// </summary>
    virtual void FocusOut();

    /// <summary>
    /// Navigation Route로 자신을 설정하고, 해당 Route를 실행하게 되면 호출되는 함수입니다.
    /// </summary>
    virtual void Submit() {}

    /// <summary>
    /// 초기 포커스로 설정합니다.
    /// </summary>
    void SetInitialFocus() const;

    /// <summary>
    /// NavigationKey를 받아 해당하는 NavigationID를 반환합니다.
    /// </summary>
    /// <param name="key">탐색에 사용되는 NavigationKey 객체입니다.</param>
    /// <returns>입력된 NavigationKey에 대응하는 NavigationID입니다.</returns>
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
    REFLECT_FIELDS_END(UINavigationComponent)
};