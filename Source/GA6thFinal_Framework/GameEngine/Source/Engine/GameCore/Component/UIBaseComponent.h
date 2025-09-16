#pragma once

using NavigationID = int;

static constexpr NavigationID INVALID_NAVIGATION_ID = -1;

struct NavigationKey
{
    std::string                  Name;
    Input::Controller::Button    ButtonType;
    Input::Controller::StickBias Bias;
};

class UIBaseComponent : public Component
{
    friend class Transform;
    USING_PROPERTY(UIBaseComponent)

protected:
    static bool _isDebug;

public:
    UIBaseComponent();

public:
    REFLECT_PROPERTY()

protected:
    /// <summary>
    /// 이 컴포넌트를 소유한 게임 오브젝트에 다른 자식 게임 오브젝트가 추가되었을 때 호출됩니다.
    /// </summary>
    /// <param name="childGameObject">연결될 자식 GameObject에 대한 포인터입니다.</param>
    virtual void OnAttachChild(GameObject* childGameObject) {};

    /// <summary>
    /// 이 컴포넌트를 소유한 게임 오브젝트가 다른 부모 게임 오브젝트에 추가되었을 때 호출됩니다.
    /// </summary>
    /// <param name="parentGameObject">연결될 부모 GameObject에 대한 포인터입니다.</param>
    virtual void OnAttachParent(GameObject* parentGameObject) {};

    /// <summary>
    /// 이 컴포넌트를 소유한 게임 오브젝트가 다른 부모 게임 오브젝트로부터 분리되었을 때 호출됩니다.
    /// </summary>
    /// <param name="previousParentGameObject">이전에 연결되어 있던 부모 GameObject에 대한 포인터입니다.</param>
    virtual void OnDetachParent(GameObject* previousParentGameObject) {};

    /// <summary>
    /// OnDrawDebug에 의해 호출되는 디버그 오버라이드 함수입니다.
    /// EnableInHierarchy가 true일 때만 호출됩니다.
    /// </summary>
    virtual void OnDrawDebugOverride() {};

    /// <summary>
    /// OnDrawDebugSelected에 의해 호출되는 디버그 선택 오버라이드 함수입니다.
    /// EnableInHierarchy가 true일 때만 호출됩니다.
    /// </summary>
    virtual void OnDrawDebugSelectedOverride() {};

    void ImGuiDrawPropertysEvent() override;

private:
    void OnDrawDebug() override;
    void OnDrawDebugSelected() override;

protected:
    REFLECT_FIELDS_BEGIN(Component)
    REFLECT_FIELDS_END(UIBaseComponent)
};