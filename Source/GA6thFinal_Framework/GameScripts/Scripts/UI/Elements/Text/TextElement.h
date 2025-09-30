#pragma once

#include "UI/Base/DrawUIComponent/DrawUIComponent.h"

class TextElement : public DrawUIComponent
{
    USING_PROPERTY(TextElement)

public:
    TextElement();
    TextElement(const TextElement&)            = delete;
    TextElement& operator=(const TextElement&) = delete;
    TextElement(TextElement&&)                 = delete;
    TextElement& operator=(TextElement&&)      = delete;
    ~TextElement() override;

public:
    REFLECT_PROPERTY(FilePath, Text, Color, FontScale)

    GETTER_ONLY(std::string, FilePath) { return _guidRef.ToPath().string(); }
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
        InvalidateMeasure();
    }
    PROPERTY(FontScale)

    GETTER_ONLY(SIZE, ContentSize) { return ReflectFields->ContentSize; }
    PROPERTY(ContentSize)

public:
    void SetFont(const File::GuidRef& guidRef);

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
    void UpdateContentSize();

protected:
    REFLECT_FIELDS_BEGIN(DrawUIComponent)
    std::string          Guid;
    std::string          Text         = "Hello Um!";
    std::array<float, 4> Color        = {0.0f, 0.0f, 0.0f, 1.0f};
    float                FontScale    = 1.0f;
    SIZE                 ContentSize  = SIZE{};
    REFLECT_FIELDS_END(TextElement)

private:
    std::unique_ptr<TextRenderer> _renderer;
    File::GuidRef                 _guidRef;

};