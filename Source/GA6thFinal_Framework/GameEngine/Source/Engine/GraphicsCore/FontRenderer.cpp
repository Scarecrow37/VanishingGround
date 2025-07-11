#include "pch.h"
#include "FontRenderer.h"
#include "Font.h"

FontRenderer::FontRenderer()
	: _color(1.0f, 1.0f, 1.0f, 1.0f)
{
}

FontRenderer::~FontRenderer()
{}

void FontRenderer::RegisterComponent(std::string_view sceneName)
{
    UmRenderer.RegisterRenderQueue(sceneName, this);
}

void FontRenderer::RegisterComponent()
{
    UmRenderer.RegisterRenderQueue(this);
}

void FontRenderer::SetText(std::wstring_view text)
{
    _text = text;
}

void FontRenderer::LoadFont(std::wstring_view filePath)
{
    _font = UmResourceManager.LoadResource<Font>(filePath);
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
