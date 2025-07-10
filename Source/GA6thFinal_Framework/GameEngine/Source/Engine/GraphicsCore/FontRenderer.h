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
    void SetColor(const Vector4& color) { _color = color; }
    void SetPosition(const Vector3& position) { _position = position; }
    void SetScale(const Vector2& scale) { _scale = scale; }
    void SetRotation(float rotation) { _rotation = rotation; }
    void SetOrigin(const Vector2& origin) { _origin = origin; }
    void SetMaxTextLength(const UINT size);
    void SetText(std::wstring_view text);

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
    UINT                  _maxTextLength;
};