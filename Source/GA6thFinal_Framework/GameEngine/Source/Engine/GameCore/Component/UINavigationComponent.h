#pragma once

enum class FocusCallType : unsigned char;

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
    friend class UIRoot;

    USING_PROPERTY(UINavigationComponent)

public:
    using NavigationRoute  = std::tuple<unsigned int, unsigned char, std::string, ::NavigationID>;
    using NavigationRoutes = std::vector<NavigationRoute>;

private:
    static UIRoot*      GetRoot(const GameObject& gameObject);

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
    virtual void FocusIn(FocusCallType callType);

    /// <summary>
    /// 포커스가 해당 객체에서 벗어날 때 호출되는 함수입니다.
    /// </summary>
    virtual void FocusOut(FocusCallType callType);

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

    /// <summary>
    /// 네비게이션 키와 목적지 ID를 사용하여 네비게이션 경로를 추가합니다.
    /// </summary>
    /// <param name="key">경로를 추가할 때 사용할 네비게이션 키입니다.</param>
    /// <param name="toID">경로가 연결될 목적지의 네비게이션 ID입니다.</param>
    void AddNavigationRoute(const NavigationKey& key, NavigationID toID);

    /// <summary>
    /// 지정된 NavigationKey에 해당하는 내비게이션 경로를 제거합니다.
    /// </summary>
    /// <param name="key">제거할 내비게이션 경로를 식별하는 NavigationKey 객체입니다.</param>
    void RemoveNavigationRoute(const NavigationKey& key);

    /// <summary>
    /// 내비게이션 경로를 초기화합니다.
    /// </summary>
    void ClearNavigationRoute();

    /// <summary>
    /// NavigationID를 변경하여 탐색 목적지를 업데이트합니다.
    /// </summary>
    /// <param name="fromId">현재 탐색 목적지의 NavigationID입니다.</param>
    /// <param name="toId">새로운 탐색 목적지의 NavigationID입니다.</param>
    void ChangeNavigationDestinationID(NavigationID fromId, NavigationID toId);

    /// <summary>
    /// UIRoot 객체에서 내비게이션 ID를 획득합니다.
    /// </summary>
    /// <param name="root">내비게이션 ID를 획득할 대상 UIRoot 객체의 포인터입니다.</param>
    void AcquireNavigationID(UIRoot* root);

protected:
    UIComponent* GetSiblingUI() const;

    void ImGuiDrawPropertysEvent() override;
    void OnDrawDebugSelectedOverride() override;

    void OnAttachParent(GameObject* parentGameObject) override;
    void OnDetachParent(GameObject* previousParentGameObject) override;

    void DeserializedReflectEvent() override;

    void Reset() override;

private:
    void ReleaseNavigationID(UIRoot* root);
    void SetID(NavigationID id);

protected:
    REFLECT_FIELDS_BEGIN(UIBaseComponent)
    NavigationID    NavigationID = INVALID_NAVIGATION_ID;
    NavigationRoutes NavigationRoutes;
    REFLECT_FIELDS_END(UINavigationComponent)
};