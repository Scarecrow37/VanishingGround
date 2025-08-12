#pragma once

// POINT
bool  operator==(const POINT& lhs, const POINT& rhs);
bool  operator!=(const POINT& lhs, const POINT& rhs);
POINT operator+(const POINT& lhs, const POINT& rhs);
POINT operator-(const POINT& lhs, const POINT& rhs);

// SIZE
bool operator==(const SIZE& lhs, const SIZE& rhs);
bool operator!=(const SIZE& lhs, const SIZE& rhs);
SIZE operator+(const SIZE& lhs, const SIZE& rhs);
SIZE operator-(const SIZE& lhs, const SIZE& rhs);

struct MinSize
{
    SIZE operator()(const SIZE& lhs, const SIZE& rhs) const;
};

class UIComponent : public Component
{
    friend class Transform;
    USING_PROPERTY(UIComponent)

protected:
    static bool _isDebug;

public:
    UIComponent();

public:
    REFLECT_PROPERTY(Point, Size, Padding, Margin)

    GETTER(POINT, Point) { return ReflectFields->ActualPosition; }
    SETTER(POINT, Point)
    {
        _requestedPoint = value;
        InvalidateArrange();
    }
    PROPERTY(Point)

    GETTER(SIZE, Size) { return ReflectFields->ActualSize; }
    SETTER(SIZE, Size)
    {
        _requestedSize = value;
        InvalidateMeasure();
    }
    PROPERTY(Size)

    GETTER(PADDING, Padding) { return _padding; }
    SETTER(PADDING, Padding)
    {
        _padding = value;
        ReflectFields->PaddingLeft = value.left;
        ReflectFields->PaddingTop  = value.top;
        ReflectFields->PaddingRight = value.right;
        ReflectFields->PaddingBottom = value.bottom;
        InvalidateMeasure();
    }
    PROPERTY(Padding)

    GETTER(MARGIN, Margin) { return _margin; }
    SETTER(MARGIN, Margin)
    {
        _margin = value;
        ReflectFields->MarginLeft = value.left;
        ReflectFields->MarginTop  = value.top;
        ReflectFields->MarginRight = value.right;
        ReflectFields->MarginBottom = value.bottom;
        InvalidateMeasure();
    }
    PROPERTY(Margin)


    GETTER_ONLY(UIComponent*, Parent)
    {
        UIComponent*     parent    = nullptr;
        const Transform&      transform = this->transform;
        if (const Transform* parentTransform = transform.Parent; nullptr != parentTransform)
        {
            const GameObject& parentObject = parentTransform->gameObject;
            parent                         = parentObject.GetComponentDynamic<UIComponent>();
        }
        return parent;
    }
    PROPERTY(Parent)

    GETTER_ONLY(std::vector<UIComponent*>, Children)
    {
        std::vector<UIComponent*> children;
        Transform&          transform = this->transform;
        for (int i = 0; i < transform.GetChildCount(); ++i)
        {
            const Transform* child           = transform.GetChild(i);
            GameObject&      gameObject      = child->gameObject;
            std::vector<UIComponent*> childComponents = gameObject.GetComponents<UIComponent>();
            std::ranges::move(childComponents, std::back_inserter(children));
        }
        return children;
    }
    PROPERTY(Children)

    GETTER_ONLY(POINT, AbsolutePosition)
    {
        const POINT offset           = ReflectFields->Offset;
        const POINT leftTopMargin    = _margin.LeftTop();
        const POINT actualPosition   = ReflectFields->ActualPosition;
        const POINT absolutePosition = offset + leftTopMargin + actualPosition;
        return absolutePosition;
    }
    PROPERTY(AbsolutePosition)

    GETTER_ONLY(POINT, AbsoluteChildPosition)
    {
        const POINT absolutePosition      = AbsolutePosition;
        const POINT leftTopPadding        = _padding.LeftTop();
        const POINT absoluteChildPosition = absolutePosition + leftTopPadding;
        return absoluteChildPosition;
    }
    PROPERTY(AbsoluteChildPosition)

    GETTER_ONLY(bool, IsMeasureDirty)
    {
        return _isMeasureDirty;
    }
    PROPERTY(IsMeasureDirty)

    GETTER_ONLY(bool, IsArrangeDirty)
    {
        return _isArrangeDirty;
    }
    PROPERTY(IsArrangeDirty)

    GETTER_ONLY(SIZE, DesiredSize) { return ReflectFields->DesiredSize; }
    PROPERTY(DesiredSize)

    GETTER_ONLY(POINT, Offset) { return ReflectFields->Offset; }
    PROPERTY(Offset)

    GETTER_ONLY(POINT, ActualPosition) { return ReflectFields->ActualPosition; }
    PROPERTY(ActualPosition)

    GETTER_ONLY(SIZE, ActualSize) { return ReflectFields->ActualSize; }
    PROPERTY(ActualSize)

public:
    void Measure(SIZE availableSize);
    void Arrange(POINT finalPosition, SIZE finalSize);
    void Arrange();

protected:
    /// <summary>
    /// 이 컴포넌트를 소유한 게임 오브젝트에 다른 자식 게임 오브젝트가 추가되었을 때 호출됩니다.
    /// </summary>
    /// <param name="childGameObject">연결될 자식 GameObject에 대한 포인터입니다.</param>
    virtual void OnAttachChild(GameObject* childGameObject);

    /// <summary>
    /// 이 컴포넌트를 소유한 게임 오브젝트가 다른 부모 게임 오브젝트로부터 분리되었을 때 호출됩니다.
    /// </summary>
    /// <param name="previousParentGameObject">이전에 연결되어 있던 부모 GameObject에 대한 포인터입니다.</param>
    virtual void OnDetachParent(GameObject* previousParentGameObject);

    /// <summary>
    /// OnDrawDebug에 의해 호출되는 디버그 오버라이드 함수입니다.
    /// EnableInHierarchy가 true일 때만 호출됩니다.
    /// </summary>
    virtual void OnDrawDebugOverride();

    /// <summary>
    /// OnDrawDebugSelected에 의해 호출되는 디버그 선택 오버라이드 함수입니다.
    /// EnableInHierarchy가 true일 때만 호출됩니다.
    /// </summary>
    virtual void OnDrawDebugSelectedOverride();


    /// <summary>
    /// UI 컴포넌트의 측정 로직을 구현하는 함수입니다.
    /// </summary>
    /// <param name="availableSize">측정에 사용할 가용 크기입니다.</param>
    /// <returns>측정된 요소의 크기(SIZE)를 반환합니다.</returns>
    virtual SIZE MeasureOverride(SIZE availableSize) = 0;

    /// <summary>
    /// 지정된 최종 크기에 맞게 요소를 배치하고 실제 배치된 크기를 반환합니다.
    /// </summary>
    /// <param name="finalSize">배치에 사용할 최종 크기입니다.</param>
    /// <returns>실제로 배치된 요소의 크기(SIZE)를 반환합니다.</returns>
    virtual SIZE ArrangeOverride(SIZE finalSize)     = 0;

protected:
    void ImGuiDrawPropertysEvent() override;
    void DeserializedReflectEvent() override;

    void InvalidateMeasure();
    void InvalidateArrange();


private:
    void ResetPlacement();

    void OnDrawDebug() override;
    void OnDrawDebugSelected() override;

protected:
    REFLECT_FIELDS_BEGIN(Component)
    POINT ActualPosition;
    SIZE  ActualSize;
    POINT Offset;
    SIZE  DesiredSize;
    SIZE  AvailableSize;

    LONG  PaddingLeft;
    LONG  PaddingTop;
    LONG  PaddingRight;
    LONG  PaddingBottom;

    LONG  MarginLeft;
    LONG  MarginTop;
    LONG  MarginRight;
    LONG  MarginBottom;
    REFLECT_FIELDS_END(UIComponent)

protected:
    POINT _requestedPoint;
    SIZE  _requestedSize;

    PADDING _padding;
    MARGIN  _margin;

private:
    bool _isMeasureDirty;
    bool _isArrangeDirty;
};

struct DrawDebug
{
    void operator()(const char* label) const;
    void operator()(const char* label, long x) const;
    void operator()(const char* label, long x, long y) const;
};

struct DrawQuad
{
    void operator()(POINT point, SIZE size, const int thickness, FXMVECTOR color) const;
};

// TODO 지울지 고민
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
