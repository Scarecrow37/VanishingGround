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
        
    };

public:
    TokenElement();

    void Setup(const SetupData& data);

protected:
    void Update() override;
    POINT GetPoint(float t) const;

private:
    float GetT() const;
    void  UpdatePoint(float t);
    void  UpdateOpacity(float t) const;

protected:
    REFLECT_FIELDS_BEGIN(DescriptionPanel)
    REFLECT_FIELDS_END(TokenElement)

private:
    POINT _beginPoint;
    POINT _endPoint;
    POINT _beginOpacity;
    POINT _endOpacity;

    std::weak_ptr<ImageElement> _iconImageElement;
    std::weak_ptr<TextElement>  _nameTextElement;
};