#pragma once

namespace UI
{
    class Element
    {
    public:
        Element()                          = default;
        Element(const Element&)            = default;
        Element(Element&&)                 = default;
        Element& operator=(const Element&) = default;
        Element& operator=(Element&&)      = default;
        virtual ~Element()                 = default;

    public:
        void                    Measure(Transform::Size availableSize);
        virtual Transform::Size MeasureOverride(Transform::Size availableSize) = 0;

        void                    Arrange(Transform::Rect finalRect);
        virtual Transform::Size ArrangeOverride(Transform::Size finalSize) = 0;

        void InvalidateMeasure();
        void InvalidateArrange();

    private:
        bool _isMeasureDirty;
        bool _isArrangeDirty;
    };
} // namespace UI