#pragma once

class TextElement;

class DamageElement : public UIComponent
{
    USING_PROPERTY(DamageElement)

    static constexpr LONG  GAP                              = 8L;
    static constexpr LONG  REVELATION_SIZE_RATIO_LARGE      = 3L;
    static constexpr LONG  REVELATION_SIZE_RATIO_SMALL      = 2L;
    static constexpr float REVELATION_FONT_SIZE_RATIO_LARGE = static_cast<float>(REVELATION_SIZE_RATIO_LARGE);
    static constexpr float REVELATION_FONT_SIZE_RATIO_SMALL = static_cast<float>(REVELATION_SIZE_RATIO_SMALL);

    static constexpr Color COLOR_TRANSPARENT = Color(0.0f, 0.0f, 0.0f, 0.0f);

public:
    struct SetupData
    {
        LONG                         Distance;
        float                        Angle;
        float                        Duration;
        POINT                        Origin;
        File::Guid                   FontGuid;
        float                        BeginFontSize;
        float                        EndFontSize;
        Color                        BeginColor;
        Color                        EndColor;
        Color                        BeginOutlineColor;
        Color                        EndOutlineColor;
        std::string                  Damage;
        std::span<const std::string> Revelations;
        float                        TurningPoint;
        Mathf::EasingFunctionType    EasingFunctionType;
    };

public:
    DamageElement();

    void Setup(const SetupData& data);

protected:
    SIZE  MeasureOverride(SIZE availableSize) override;
    SIZE  ArrangeOverride(SIZE finalSize) override;
    void  Update() override;
    POINT GetPoint(float t) const;

private:
    float GetT() const;
    void  UpdatePoint(float t);
    void  UpdateFont(float t) const;

    void SetupPoints();
    void SetupChildren(const File::Guid& fontGuid, const std::string& damage, std::span<const std::string> revelations);
    std::weak_ptr<TextElement> CreateTextElement(const File::Guid& fontGuid, const std::string& content, float fontSize,
                                                 Color fontColor, Color outlineColor) const;

protected:
    REFLECT_FIELDS_BEGIN(UIComponent)
    REFLECT_FIELDS_END(DamageElement)

    POINT                     _origin;
    LONG                      _distance;
    float                     _angle;
    float                     _duration;
    float                     _elapsedTime;
    float                     _beginFontSize;
    float                     _endFontSize;
    float                     _beginRevelationFontSize;
    float                     _endRevelationFontSize;
    LONG                      _sizeRatio;
    Color                     _beginColor;
    Color                     _endColor;
    Color                     _beginOutlineColor;
    Color                     _endOutlineColor;
    std::vector<float>        _steps;
    std::vector<Vector3>      _points;
    float                     _turningPoint;
    Mathf::EasingFunctionType _easingFunctionType;

    std::weak_ptr<TextElement>              _damageTextElement;
    std::vector<std::weak_ptr<TextElement>> _revelationTextElements;

    std::weak_ptr<TextElement>              _damageOutlineElement;
    std::vector<std::weak_ptr<TextElement>> _revelationOutlineElements;
};
