#include "pch.h"
#include "TextRenderer.h"
#include "TextSystem.h"
#include <directxtk/SpriteFont.h>

TextRenderer::TextRenderer(TextSystem* pTextSystem)
	: _pTextSystem(pTextSystem)
{
}

void TextRenderer::Release()
{
	delete this;
}

void TextRenderer::SetFont(const wchar_t* name)
{
	_pSpriteFont = _pTextSystem->GetSpriteFont(name);
}

void TextRenderer::SetFontScale(const float x, const float y)
{
	_scale = { x, y };
}

void TextRenderer::SetFontColor(const float r, const float g, const float b, const float a)
{
	_color = { r, g, b, a };
}

void TextRenderer::SetLayerDepth(const float depth)
{
	_depth = depth;
}

void TextRenderer::SetPivot(const float x, const float y)
{
	_pivot = { x, y };
}

void TextRenderer::SetRotationFromDegree(const float degree)
{
	_rotation = XMConvertToRadians(degree);
}

void TextRenderer::SetPosition(const float x, const float y, const float z)
{
	_position = { x, y, z };
}

void TextRenderer::SetText(const wchar_t* text)
{
	_text = text;
}

void TextRenderer::Render(SpriteBatch* pSpriteBatch)
{
	_pSpriteFont->DrawString(pSpriteBatch, _text.c_str(), _position, _color, _rotation, _pivot, _scale, SpriteEffects_None, _depth);
}