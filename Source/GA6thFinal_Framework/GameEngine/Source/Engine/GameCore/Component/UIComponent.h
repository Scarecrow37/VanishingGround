#pragma once

class UIRoot;
// POINT
bool  operator==(const POINT& lhs, const POINT& rhs);
bool  operator!=(const POINT& lhs, const POINT& rhs);
POINT operator+(const POINT& lhs, const POINT& rhs);
POINT operator-(const POINT& lhs, const POINT& rhs);
POINT& operator+=(POINT& lhs, const POINT& rhs);

// SIZE
bool operator==(const SIZE& lhs, const SIZE& rhs);
bool operator!=(const SIZE& lhs, const SIZE& rhs);
SIZE operator+(const SIZE& lhs, const SIZE& rhs);
SIZE operator-(const SIZE& lhs, const SIZE& rhs);
SIZE& operator+=(SIZE& lhs, const SIZE& rhs);

struct MinSize
{
    SIZE operator()(const SIZE& lhs, const SIZE& rhs) const;
    SIZE operator()(const SIZE& lhs, const SIZE& rhs, bool useLhsWidth, bool useLhsHeight) const;
};

struct MaxSize
{
    SIZE operator()(const SIZE& lhs, const SIZE& rhs) const;
};

enum class HorizontalAlignment : char
{
    LEFT,
    CENTER,
    RIGHT
};

enum class VerticalAlignment : char
{
    TOP,
    CENTER,
    BOTTOM
};

struct AlignPoint
{
    POINT operator()(HorizontalAlignment horizontal, VerticalAlignment vertical, SIZE size) const;
    LONG  operator()(HorizontalAlignment horizontal, LONG spareWidth) const;
    LONG  operator()(VerticalAlignment vertical, LONG spareHeight) const;
};

enum class FillMode : char
{
    NONE,
    WRAP,
    FILL
};

class UIComponent : public UIBaseComponent
{
    friend class Transform;
    USING_PROPERTY(UIComponent)

private:
    static UIRoot* GetRoot(const GameObject& obj);

public:
    UIComponent();

public:
    REFLECT_PROPERTY(Point, Size, Padding, Margin, HorizontalAlign, VerticalAlign, HorizontalFillMode, VerticalFillMode)

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

    GETTER(PADDING, Padding) { return PADDING(ReflectFields->Padding); }
    SETTER(PADDING, Padding)
    {
        ReflectFields->Padding = static_cast<RECT>(value);
        InvalidateMeasure();
        InvalidateArrange();
    }
    PROPERTY(Padding)

    GETTER(MARGIN, Margin) { return MARGIN(ReflectFields->Margin); }
    SETTER(MARGIN, Margin)
    {
        ReflectFields->Margin = static_cast<RECT>(value);
        InvalidateMeasure();
        InvalidateArrange();
    }
    PROPERTY(Margin)

    GETTER(HorizontalAlignment, HorizontalAlign) { return ReflectFields->HorizontalAlignment; }
    SETTER(HorizontalAlignment, HorizontalAlign)
    {
        ReflectFields->HorizontalAlignment = value;
        InvalidateArrange();
    }
    PROPERTY(HorizontalAlign)

    GETTER(VerticalAlignment, VerticalAlign) { return ReflectFields->VerticalAlignment; }
    SETTER(VerticalAlignment, VerticalAlign)
    {
        ReflectFields->VerticalAlignment = value;
        InvalidateArrange();
    }
    PROPERTY(VerticalAlign)

    GETTER(FillMode, HorizontalFillMode) { return ReflectFields->HorizontalFillMode; }
    SETTER(FillMode, HorizontalFillMode)
    {
        ReflectFields->HorizontalFillMode = value;
        InvalidateMeasure();
    }
    PROPERTY(HorizontalFillMode)

    GETTER(FillMode, VerticalFillMode) { return ReflectFields->VerticalFillMode; }
    SETTER(FillMode, VerticalFillMode)
    {
        ReflectFields->VerticalFillMode = value;
        InvalidateMeasure();
    }
    PROPERTY(VerticalFillMode)


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
        const MARGIN margin           = Margin;
        const POINT  leftTopMargin    = margin.LeftTop();
        const POINT actualPosition   = ReflectFields->ActualPosition;
        const POINT absolutePosition = offset + leftTopMargin + actualPosition;
        return absolutePosition;
    }
    PROPERTY(AbsolutePosition)

    GETTER_ONLY(POINT, AbsoluteChildPosition)
    {
        const POINT absolutePosition      = AbsolutePosition;
        const PADDING padding               = Padding;
        const POINT   leftTopPadding        = padding.LeftTop();
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

    GETTER(bool, IsFocus) { return _isFocus; }
    SETTER(bool, IsFocus) { _isFocus = value; }
    PROPERTY(IsFocus)

    GETTER_ONLY(POINT, AbsoluteCenterPoint)
    {
        const POINT absolute = AbsolutePosition;
        const SIZE  size     = Size;
        return POINT{
        .x = absolute.x + size.cx / 2,
        .y = absolute.y + size.cy / 2};
    }
    PROPERTY(AbsoluteCenterPoint)

    GETTER_ONLY(POINT, CenterPoint)
    {
        const SIZE  size  = Size;
        return POINT{
        .x = size.cx / 2,
        .y = size.cy / 2};
    }
    PROPERTY(CenterPoint)

    GETTER_ONLY(RECT, AbsoluteRect)
    {
        const POINT absolutePosition = AbsolutePosition;
        const SIZE  size             = Size;
        return RECT{
        .left   = absolutePosition.x,
        .top    = absolutePosition.y,
        .right  = absolutePosition.x + size.cx,
        .bottom = absolutePosition.y + size.cy};
    }
    PROPERTY(AbsoluteRect)

    GETTER_ONLY(UIRoot*, Root)
    {
        UIRoot*          uiRoot    = nullptr;
        const Transform& transform = this->transform;
        if (const Transform* rootTransform = transform.Root; nullptr != rootTransform)
        {
            const GameObject& rootGameObject = rootTransform->gameObject;
            uiRoot                           = GetRoot(rootGameObject);
        }
        return uiRoot;
    }
    PROPERTY(Root)

public:
    void Measure(SIZE availableSize);
    void Arrange(POINT finalPosition, SIZE finalSize);
    void Arrange();

    void InvalidateMeasure();
    void InvalidateArrange();

protected:
    void OnAttachParent(GameObject* parentGameObject) override;
    void OnAttachChild(GameObject* childGameObject) override;
    void OnDetachParent(GameObject* previousParentGameObject) override;
    void OnDrawDebugOverride() override;
    void OnDrawDebugSelectedOverride() override;
    void RequestViewOrder() const;
    void RequestCheckNavigationIdFlawless() const;

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
    virtual SIZE ArrangeOverride(SIZE finalSize) = 0;

protected:
    void ImGuiDrawPropertysEvent() override;
    void DeserializedReflectEvent() override;
    void Reset() override;
    void Start() override;

    std::weak_ptr<UIComponent> GetUIWeakPtr() const;

private:
    void ResetPlacement();

protected:
    REFLECT_FIELDS_BEGIN(UIBaseComponent)
    POINT ActualPosition;
    SIZE  ActualSize;
    POINT Offset;
    SIZE  DesiredSize;
    SIZE  AvailableSize;

    RECT Padding;
    RECT Margin;

    HorizontalAlignment HorizontalAlignment;
    VerticalAlignment   VerticalAlignment;
    FillMode            HorizontalFillMode;
    FillMode            VerticalFillMode;

    REFLECT_FIELDS_END(UIComponent)

protected:
    POINT _requestedPoint;
    SIZE  _requestedSize;

private:
    bool _isFocus;
    bool _isMeasureDirty;
    bool _isArrangeDirty;
};

struct ImGuiDebug
{
    void operator()(const char* label) const;
    void operator()(const char* label, long x) const;
    void operator()(const char* label, long x, long y) const;
    void operator()(const char* label, unsigned int x) const;
    void operator()(const char* label, int x) const;
    void operator()(const char* label, size_t x) const;
    void operator()(const char* label, const std::string& str) const;
};

struct DrawDebug
{
    /// <summary>
    /// 지정된 두께와 색상으로 주어진 POINT와 SIZE를 사용하여 사각형을 그립니다.
    /// </summary>
    /// <param name="point">도형을 그릴 기준이 되는 POINT 객체입니다.</param>
    /// <param name="size">도형의 크기를 지정하는 SIZE 객체입니다.</param>
    /// <param name="thickness">도형의 테두리 두께를 나타내는 정수입니다.</param>
    /// <param name="color">도형의 색상을 지정하는 FXMVECTOR 타입의 벡터입니다.</param>
    void operator()(POINT point, SIZE size, int thickness, FXMVECTOR color) const;

    /// <summary>
    /// 두 점 사이에 선을 그립니다.
    /// </summary>
    /// <param name="pointA">선의 시작점입니다.</param>
    /// <param name="pointB">선의 끝점입니다.</param>
    /// <param name="thickness">선의 두께입니다.</param>
    /// <param name="isVertical">선이 수직인지 여부를 나타내는 값입니다.</param>
    /// <param name="color">선의 색상을 나타내는 벡터입니다.</param>
    void operator()(POINT pointA, POINT pointB, int thickness, bool isVertical, FXMVECTOR color) const;

    /// <summary>
    /// 두 점 사이에 화살표를 그리고, 화살촉의 길이와 색상을 지정합니다.
    /// </summary>
    /// <param name="pointA">화살표의 시작점입니다.</param>
    /// <param name="pointB">화살표의 끝점입니다.</param>
    /// <param name="arrowheadLength">화살촉의 길이입니다.</param>
    /// <param name="color">화살표의 색상입니다. FXMVECTOR 타입으로 지정합니다.</param>
    void operator()(POINT pointA, POINT pointB, float arrowheadLength, FXMVECTOR color) const;

    /// <summary>
    /// 두 개의 POINT와 두 개의 RECT를 받아 충돌한 결과로 두 개의 POINT를 반환합니다.
    /// </summary>
    /// <param name="pointA">처리할 첫 번째 POINT입니다.</param>
    /// <param name="pointB">처리할 두 번째 POINT입니다.</param>
    /// <param name="rectA">첫 번째 POINT와 관련된 RECT입니다.</param>
    /// <param name="rectB">두 번째 POINT와 관련된 RECT입니다.</param>
    /// <returns>처리 결과로 반환되는 두 개의 POINT를 담은 std::pair입니다.</returns>
    std::optional<std::pair<POINT, POINT>> operator()(POINT pointA, POINT pointB, RECT rectA, RECT rectB) const;

    /// <summary>
    /// 원점과 반지름, 색상을 사용하여 원 그리기를 수행하는 함수 호출 연산자입니다.
    /// </summary>
    /// <param name="origin">기준이 되는 POINT 구조체입니다.</param>
    /// <param name="radius">원의 반지름을 나타내는 실수 값입니다.</param>
    /// <param name="color">색상을 나타내는 FXMVECTOR 타입의 벡터입니다.</param>
    void operator()(POINT origin, float radius, FXMVECTOR color) const;
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
