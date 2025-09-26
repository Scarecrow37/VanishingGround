#pragma once

class ISDFTextRenderer;
class TestSDFTextRenderer : public Component
{
    USING_PROPERTY(TestSDFTextRenderer)

public:
    TestSDFTextRenderer();
    ~TestSDFTextRenderer() override;

public:
    REFLECT_PROPERTY(FilePath, Text, Color, FontScale)

    GETTER_ONLY(std::string, FilePath) { return _guidRef.ToPath().string(); }
    PROPERTY(FilePath)

    GETTER(std::string, Text) { return ReflectFields->Text; }
    SETTER(std::string, Text)
    {
        ReflectFields->Text = value;
        UpdateText();
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
    }
    PROPERTY(FontScale)

public:
    void SetFont(const File::GuidRef& guidRef);

protected:
    void Reset() override;

private:
    void UpdateText() const;
    void UpdateColor() const;
    void UpdateScale() const;

protected:
    REFLECT_FIELDS_BEGIN(Component)
    std::string          Guid;
    std::string          Text      = "Hello Um!";
    std::array<float, 4> Color     = {0.0f, 0.0f, 0.0f, 1.0f};
    float                FontScale = 1.0f;
    REFLECT_FIELDS_END(TestSDFTextRenderer)

private:
    ISDFTextRenderer* _renderer;
    File::GuidRef     _guidRef;
};

