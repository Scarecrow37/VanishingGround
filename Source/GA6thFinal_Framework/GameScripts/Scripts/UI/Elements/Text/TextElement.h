#pragma once

#include "UI/Base/DrawUIComponent/DrawUIComponent.h"
#include "UI/Base/IOpacity/IOpacity.h"

class TextElement : public DrawUIComponent, public IOpacity
{
    enum FontFlags : uint32_t
    {
        FONT_FLAG_NONE    = 0,
        FONT_FLAG_OUTLINE = 1 << 0,
    };
    USING_PROPERTY(TextElement)

public:
    TextElement();
    TextElement(const TextElement&)            = delete;
    TextElement& operator=(const TextElement&) = delete;
    TextElement(TextElement&&)                 = delete;
    TextElement& operator=(TextElement&&)      = delete;
    ~TextElement() override;

public:
    REFLECT_PROPERTY(FilePath, Text, Color, FontScale, OutlineColor, OutlineWidth, Outline)

    GETTER_ONLY(std::string, FilePath) { return _Guid.ToPath().string(); }
    PROPERTY(FilePath)

    GETTER(std::string, Text) { return ReflectFields->Text; }
    SETTER(std::string, Text)
    {
        ReflectFields->Text = value;
        UpdateText();
        UpdateContentSize();
        InvalidateMeasure();
    }
    PROPERTY(Text)

    GETTER(DirectX::SimpleMath::Color, Color) { return DirectX::SimpleMath::Color(&ReflectFields->Color[0]); }
    SETTER(DirectX::SimpleMath::Color, Color)
    {
        std::memcpy(&ReflectFields->Color[0], &value.x, sizeof(ReflectFields->Color));
        UpdateColor();
    }
    PROPERTY(Color)

    GETTER(float, Alpha) { return ReflectFields->Color[3]; }
    SETTER(float, Alpha) { SetOpacity(value); }
    PROPERTY(Alpha)

    GETTER(float, FontScale) { return ReflectFields->FontScale; }
    SETTER(float, FontScale)
    {
        ReflectFields->FontScale = std::max(0.0f, value);
        UpdateScale();
        UpdateContentSize();
        InvalidateMeasure();
    }
    PROPERTY(FontScale)

    GETTER(float, FontWeight) { return ReflectFields->FontWeight; }
    SETTER(float, FontWeight)
    {
        ReflectFields->FontWeight = std::clamp(value, 0.0f, 1.0f);
        UpdateWeight();
        UpdateContentSize();
        InvalidateMeasure();
    }
    PROPERTY(FontWeight)

    GETTER_ONLY(SIZE, ContentSize) { return ReflectFields->ContentSize; }
    PROPERTY(ContentSize)

    GETTER(bool, Outline) { return (ReflectFields->FontFlags & FONT_FLAG_OUTLINE) != 0; }
    SETTER(bool, Outline)
    {
        UINT fontFlags = ReflectFields->FontFlags;
        value ? (fontFlags |= FONT_FLAG_OUTLINE) : (fontFlags &= ~FONT_FLAG_OUTLINE);
        ReflectFields->FontFlags = fontFlags;
        UpdateOutline();
    }
    PROPERTY(Outline)

    GETTER(DirectX::SimpleMath::Color, OutlineColor) { return DirectX::SimpleMath::Color(&ReflectFields->FontOutlineColor[0]); }
    SETTER(DirectX::SimpleMath::Color, OutlineColor)
    {
        std::memcpy(&ReflectFields->FontOutlineColor[0], &value.x, sizeof(ReflectFields->FontOutlineColor));
        UpdateOutline();
    }
    PROPERTY(OutlineColor)

    GETTER(float, OutlineWidth) { return ReflectFields->FontOutlineWidth; }
    SETTER(float, OutlineWidth)
    {
        ReflectFields->FontOutlineWidth = std::clamp(value, 0.0f, 4.0f);
        UpdateOutline();
    }
    PROPERTY(OutlineWidth)



public:
    void SetFont(const File::Guid& guid);

    void SetOpacity(float opacity) override;

protected:
    void  Reset() override;
    void  DeserializedReflectEvent() override;
    float GetZOrder() const override;
    void  ImGuiDrawPropertysEvent() override;

    SIZE MeasureOverride(SIZE availableSize) override;
    SIZE ArrangeOverride(SIZE finalSize) override;

private:
    void SetViewOrder(int viewOrder) override;

    void RequestResource();
    void LoadFont() const;
    void UpdateProperties();
    void UpdateText() const;
    void UpdateColor() const;
    void UpdatePosition() const;
    void UpdateScale() const;
    void UpdateWeight() const;
    void UpdateContentSize();
    void UpdateOutline();

protected:
    REFLECT_FIELDS_BEGIN(DrawUIComponent)
    std::string          Guid;
    std::string          Text             = "Hello Um!";
    std::array<float, 4> Color            = {0.0f, 0.0f, 0.0f, 1.0f};
    float                FontScale        = 32.0f;
    SIZE                 ContentSize      = SIZE{};
    float                FontWeight       = 0.5f;
    std::array<float, 4> FontOutlineColor = {0.0f, 0.0f, 0.0f, 1.0f};
    float                FontOutlineWidth = 0.0f;
    UINT                 FontFlags        = FONT_FLAG_NONE;
    REFLECT_FIELDS_END(TextElement)

private:
    ISDFTextRenderer* _renderer;
    File::Guid        _Guid;
};