#include "pch.h"
#include "FontRenderer.h"
#include "Font.h"

FontRenderer::FontRenderer()
	: _color(1.0f, 1.0f, 1.0f, 1.0f)
    , _scale(1.f, 1.f)
{
}

FontRenderer::~FontRenderer()
{}

void FontRenderer::SetText(std::wstring_view text)
{
    _text = text;
}

XMVECTOR FontRenderer::GetStringSize() const
{
    if (_font)
    {
        if (const auto font = _font->GetFont())
        {
            return font->MeasureString(_text.c_str());
        }
    }

    return XMVectorZero();
}
void FontRenderer::SetFont(std::shared_ptr<Font> font)
{
    _font = std::move(font);
}

void FontRenderer::Draw(SpriteBatch* spriteBatch)
{
    if (_font)
    {
        auto font = _font->GetFont();
        if (font)
        {
            font->DrawString(spriteBatch, _text.c_str(), _position, _color, XMConvertToRadians(_rotation), _origin, _scale, SpriteEffects_None, _position.z);
        }
    }
}