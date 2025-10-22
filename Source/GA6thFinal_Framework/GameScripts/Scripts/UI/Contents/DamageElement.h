#pragma once

class TextElement;

class DamageElement : public UIComponent
{
    USING_PROPERTY(DamageElement)

public:
    DamageElement();

    void Setup(LONG distance, float angle, float duration, POINT origin, const File::Guid& fontGuid, float fontSize, Color fontColor, const
               std::string& damage, std::span<std::string> revelations);

protected:
    SIZE MeasureOverride(SIZE availableSize) override;
    SIZE ArrangeOverride(SIZE finalSize) override;
    void Update() override;
    POINT GetPoint() const;
    void OnDestroy() override;

private:
    void SetupPoints();
    void SetupChildren(const File::Guid& fontGuid, float fontSize, Color fontColor, const std::string& damage, std::span<std::string>
                       revelations);
    TextElement* CreateTextElement(const File::Guid& fontGuid, const std::string& content, float fontSize, Color fontColor) const;

protected:
    REFLECT_FIELDS_BEGIN(UIComponent)
    REFLECT_FIELDS_END(DamageElement)

    POINT _origin;
    LONG _distance;
    float _angle;
    float _duration;
    float _elapsedTime;
    std::vector<float> _steps;
    std::vector<Vector3> _points;

    TextElement* _damageTextElement;
    std::vector<TextElement*> _revelationTextElements;
};
