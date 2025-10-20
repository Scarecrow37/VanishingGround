#pragma once

class TextElement;

class DamageElement : public UIComponent
{
    USING_PROPERTY(DamageElement)

    static constexpr LONG GAP = 8L;
    static constexpr float REVELATION_FONT_SIZE_RATIO = 3.0f;

public:
    DamageElement();

    void Setup(LONG distance, float angle, float duration, POINT origin, const File::Guid& fontGuid,
               float beginFontSize, float endFontSize, Color beginColor, Color endColor, const std::string& damage,
               std::span<std::string> revelations);

protected:
    SIZE MeasureOverride(SIZE availableSize) override;
    SIZE ArrangeOverride(SIZE finalSize) override;
    void Update() override;
    POINT GetPoint(float t) const;
    void OnDestroy() override;

private:
    float GetT() const;
    void  UpdatePoint(float t);
    void  UpdateFont(float t) const;

    void SetupPoints();
    void SetupChildren(const File::Guid& fontGuid, const std::string& damage, std::span<std::string>
                       revelations);
    std::weak_ptr<TextElement> CreateTextElement(const File::Guid& fontGuid, const std::string& content, float fontSize, Color fontColor) const;

protected:
    REFLECT_FIELDS_BEGIN(UIComponent)
    REFLECT_FIELDS_END(DamageElement)

    POINT                _origin;
    LONG                 _distance;
    float                _angle;
    float                _duration;
    float                _elapsedTime;
    float                _beginFontSize;
    float                _endFontSize;
    float                _beginRevelationFontSize;
    float                _endRevelationFontSize;
    Color                _beginColor;
    Color                _endColor;
    std::vector<float>   _steps;
    std::vector<Vector3> _points;

    std::weak_ptr<TextElement> _damageTextElement;
    std::vector<std::weak_ptr<TextElement>> _revelationTextElements;
};
