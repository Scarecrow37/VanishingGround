#pragma once

#include "UI/Base/DrawUIComponent/DrawUIComponent.h"

class TextElement : public DrawUIComponent
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
    REFLECT_PROPERTY(FilePath, Text, Color, FontScale, OutlineColor, OutlineWidth, IsOutlineEnabled)

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

    GETTER(Vector3, OutlineColor) { return Vector3(ReflectFields->FontOutline[0]); }
    SETTER(Vector3, OutlineColor)
    {
        ReflectFields->FontOutline[0] = value.x;
        ReflectFields->FontOutline[1] = value.y;
        ReflectFields->FontOutline[2] = value.z;
        TestUpdateOutline();
    }
    PROPERTY(OutlineColor)

    GETTER(float, OutlineWidth) { return ReflectFields->FontOutline[3]; }
    SETTER(float, OutlineWidth)
    {
        ReflectFields->FontOutline[3] = std::max(0.0f, value);
        TestUpdateOutline();
    }
    PROPERTY(OutlineWidth)

    GETTER(bool, IsOutlineEnabled) { return (ReflectFields->FontFlags & FONT_FLAG_OUTLINE) != 0; }
    SETTER(bool, IsOutlineEnabled)
    {
        if (value)
        {
            ReflectFields->FontFlags |= FONT_FLAG_OUTLINE;
        }
        else
        {
            ReflectFields->FontFlags &= ~FONT_FLAG_OUTLINE;
        }
        TestUpdateOutline();
    }
    PROPERTY(IsOutlineEnabled)

public:
    void SetFont(const File::Guid& Guid);

protected:
    void  Reset() override;
    void  DeserializedReflectEvent() override;
    float GetZOrder() const override;
    void ImGuiDrawPropertysEvent() override;

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

    void TestUpdateOutline();

protected:
    REFLECT_FIELDS_BEGIN(DrawUIComponent)
    std::string          Guid;
    std::string          Text         = "Hello Um!";
    std::array<float, 4> Color        = {0.0f, 0.0f, 0.0f, 1.0f};
    float                FontScale    = 32.0f;
    SIZE                 ContentSize  = SIZE{};
    float                FontWeight   = 0.5f;
    std::array<float, 4> FontOutline  = {0.0f, 0.0f, 0.0f, 1.0f};
    UINT                 FontFlags    = FONT_FLAG_NONE;
    REFLECT_FIELDS_END(TextElement)

private:
    ISDFTextRenderer* _renderer;
    File::Guid     _Guid;
};