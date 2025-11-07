#pragma once
#include "UI/Panels/Description/DescriptionPanel.h"

class TextElement;
class ImageElement;

class TokenElement : public DescriptionPanel
{
    USING_PROPERTY(TokenElement)

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
    bool IsPlaying() const;

protected:
    void Update() override;
    POINT GetPoint(float t) const;

private:
    float GetT() const;
    void  UpdatePoint(float t);
    void  UpdateOpacity(float t) const;
    void  FindComponents();

protected:
    REFLECT_FIELDS_BEGIN(DescriptionPanel)
    REFLECT_FIELDS_END(TokenElement)

private:
    POINT _beginPoint;
    POINT _endPoint;
    float _beginOpacity;
    float _endOpacity;
    float _duration;

    float _elapsedTime;
    bool  _isPlaying;

    std::weak_ptr<ImageElement> _iconImageElement;
    std::weak_ptr<TextElement>  _nameTextElement;
};