#pragma once
#include "Engine/GraphicsCore/FontRenderer.h"
#include "UI/Base/EditablePlacementUIComponent/EditablePlacementUIComponent.h"

class TextElement : public EditablePlacementUIComponent
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
    REFLECT_PROPERTY(FilePath, Text, Color, FontSize, IsFitContent)

    GETTER_ONLY(std::string, FilePath) { return _guidRef.ToPath().string(); }
    PROPERTY(FilePath)

    GETTER(std::string, Text) { return ReflectFields->Text; }
    SETTER(std::string, Text)
    {
        ReflectFields->Text = value;
        UpdateText();
        if (ReflectFields->IsFitContent)
            FitContent();
    }
    PROPERTY(Text)

    GETTER(DirectX::SimpleMath::Color, Color) { return DirectX::SimpleMath::Color(&ReflectFields->Color[0]); }
    SETTER(DirectX::SimpleMath::Color, Color)
    {
        std::memcpy(&ReflectFields->Color[0], &value.x, sizeof(ReflectFields->Color));
        UpdateColor();
    }
    PROPERTY(Color)

    GETTER(float, FontSize) { return ReflectFields->FontSize; }
    SETTER(float, FontSize)
    {
        ReflectFields->FontSize = std::max(0.0f, value);
        UpdateScale();
    }
    PROPERTY(FontSize)

    GETTER(bool, IsFitContent) { return ReflectFields->IsFitContent; }
    SETTER(bool, IsFitContent)
    {
        ReflectFields->IsFitContent = value;
        if (ReflectFields->IsFitContent)
            FitContent();
    }
    PROPERTY(IsFitContent)

protected:
    void Reset() override;
    void DeserializedReflectEvent() override;
    void OnPlacementChange() override;

private:
    void LoadFont() const;
    void PassProperty() const;
    void UpdateAll() const;

    void UpdateText() const;
    void UpdateColor() const;
    void UpdatePosition() const;
    void UpdateScale() const;

    void FitContent();

protected:
    REFLECT_FIELDS_BEGIN(EditablePlacementUIComponent)
    std::string          Guid;
    std::string          Text     = "Hello Um!";
    std::array<float, 4> Color    = {0.0f, 0.0f, 0.0f, 1.0f};
    float                FontSize = 1.0f;
    bool                 IsFitContent = false;
    REFLECT_FIELDS_END(TextElement)

private:
    std::unique_ptr<FontRenderer> _renderer;
    File::GuidRef                 _guidRef;
};