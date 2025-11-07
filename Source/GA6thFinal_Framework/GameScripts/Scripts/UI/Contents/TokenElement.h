#pragma once
#include "UI/Panels/Horizontal/HorizontalPanel.h"

class TextElement;
class ImageElement;

class TokenElement : public HorizontalPanel
{
    USING_PROPERTY(TokenElement)

    static constexpr float MIN_DURATION = 0.1f;

public:
    struct SetupData
    {
        POINT       BeginPoint;
        POINT       EndPoint;
        float       BeginOpacity;
        float       EndOpacity;
        float       Duration;
        std::string TokenName;
        Color       NameColor;
        File::Guid  IconGuid;
    };

public:
    TokenElement();

    void Setup(const SetupData& data);

protected:
    void Update() override;
    POINT GetPoint(float t) const;
    float GetOpacity(float t) const;

private:
    float GetT() const;
    void  UpdatePoint(float t);
    void  UpdateOpacity(float t) const;
    void  FindComponents();
    void  ResetState();

protected:
    REFLECT_FIELDS_BEGIN(HorizontalPanel)
    REFLECT_FIELDS_END(TokenElement)

private:
    POINT _beginPoint;
    POINT _endPoint;
    float _beginOpacity;
    float _endOpacity;
    float _duration;

    float _elapsedTime;

    std::weak_ptr<ImageElement> _iconImageElement;
    std::weak_ptr<TextElement>  _nameTextElement;
};