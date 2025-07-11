#pragma once
#include "GraphicsBase.h"

class Font;
class FontRenderer : public GraphicsBase
{
    friend class TextDrawPass;

public:
    FontRenderer();
    virtual ~FontRenderer();

public:
    void RegisterComponent(std::string_view sceneName) override;
    void RegisterComponent() override;

public:
    const Vector4& GetColor() const { return _color; }
    const Vector3& GetPosition() const { return _position; }
    const Vector2& GetScale() const { return _scale; }
    const Vector2& GetOrigin() const { return _origin; }
    const float&   GetRotation() const { return _rotation; }

public:
    void SetColor(const Vector4& color) { _color = color; }
    void SetPosition(const Vector3& position) { _position = position; }
    void SetScale(const Vector2& scale) { _scale = scale; }
    void SetOrigin(const Vector2& origin) { _origin = origin; }
    void SetRotation(float rotation) { _rotation = rotation; }
    void SetText(std::wstring_view text);

public:
    void LoadFont(std::wstring_view filePath);

private:
    void Draw(SpriteBatch* spriteBatch);

private:
    std::wstring          _text;
    std::shared_ptr<Font> _font;
    Vector4               _color;
    Vector3               _position;
    Vector2               _scale;
    Vector2               _origin;
    float                 _rotation;
};