#pragma once

class UIComponent : public Component
{
    friend class Transform;
    USING_PROPERTY(UIComponent)

public:
    UIComponent();

protected:
    /// <summary>
    /// 이 컴포넌트를 소유한 게임 오브젝트에 다른 자식 게임 오브젝트가 추가되었을 때 호출됩니다.
    /// </summary>
    /// <param name="childGameObject">연결될 자식 GameObject에 대한 포인터입니다.</param>
    virtual void OnAttachChild(GameObject* childGameObject) {};

    /// <summary>
    /// 이 컴포넌트를 소유한 게임 오브젝트가 다른 부모 게임 오브젝트로부터 분리되었을 때 호출됩니다.
    /// </summary>
    /// <param name="previousParentGameObject">이전에 연결되어 있던 부모 GameObject에 대한 포인터입니다.</param>
    virtual void OnDetachParent(GameObject* previousParentGameObject) {};

    virtual void DrawDebug() {};
    virtual void DrawDebugSelected() {};

private:
    void OnDrawDebug() override;
    void OnDrawDebugSelected() override;

protected:
    REFLECT_FIELDS_BEGIN(Component)
    REFLECT_FIELDS_END(UIComponent)

};

template <typename T>
struct FindChildComponents
{
    std::vector<T*> operator()(Transform& parentTransform)
    {
        std::vector<T*> components;
        for (int i = 0; i < parentTransform.GetChildCount(); ++i)
        {
            const Transform* child           = parentTransform.GetChild(i);
            GameObject&      gameObject      = child->gameObject;
            std::vector<T*>  childComponents = gameObject.GetComponents<T>();
            std::move(childComponents.begin(), childComponents.end(), std::back_inserter(components));
        }
        return components;
    }
};

inline bool operator==(const POINT& lhs, const POINT& rhs)
{
    return lhs.x == rhs.x && lhs.y == rhs.y;
}

inline bool operator!=(const POINT& lhs, const POINT& rhs)
{
    return !(lhs == rhs);
}

inline POINT operator+(const POINT& lhs, const POINT& rhs)
{
    return POINT{lhs.x + rhs.x, lhs.y + rhs.y};
}

inline bool operator==(const SIZE& lhs, const SIZE& rhs)
{
    return lhs.cx == rhs.cx && lhs.cy == rhs.cy;
}

inline bool operator!=(const SIZE& lhs, const SIZE& rhs)
{
    return !(lhs == rhs);
}

inline SIZE operator+(const SIZE& lhs, const SIZE& rhs)
{
    return SIZE{lhs.cx + rhs.cx, lhs.cy + rhs.cy};
}