#include "pch.h"
#include "FontRenderer.h"
#include "Font.h"

FontRenderer::FontRenderer()
	: _maxTextLength(0)
	, _color(1.0f, 1.0f, 1.0f, 1.0f)
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

void FontRenderer::SetMaxTextLength(const UINT size)
{
	_text.resize(size);
}

void FontRenderer::SetText(std::wstring_view text)
{
	if (text.size() > _maxTextLength)
	{
		GRAPHICS_ASSERT(false, L"Font text size is too long.");
		return;
	}

	lstrcpy(_text.data(), text.data());
}

void FontRenderer::Draw(SpriteBatch* spriteBatch)
{
    auto font = _font->GetFont();
    font->DrawString(spriteBatch, _text.c_str(), _position, _color, XMConvertToRadians(_rotation), _origin, _scale, SpriteEffects_None, _position.z);
}
