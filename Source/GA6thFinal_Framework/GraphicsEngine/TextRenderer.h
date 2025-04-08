#pragma once
#include "Component.h"
#include "includes/ITextRenderer.h"

namespace DirectX
{
	inline namespace DX11
	{
		class SpriteBatch;
		class SpriteFont;
	}
}

class TextSystem;
class TextRenderer : public Component, public GE::ITextRenderer
{
public:
	explicit TextRenderer(TextSystem* pTextSystem);
	virtual ~TextRenderer() = default;

public:	
	// ITextRenderer을(를) 통해 상속됨
	void Release() override;
	void SetFont(const wchar_t* font) override;
	void SetFontScale(const float x, const float y) override;
	void SetFontColor(const float r, const float g, const float b, const float a) override;
	void SetLayerDepth(const float depth) override;
	void SetPivot(const float x, const float y) override;
	void SetRotationFromDegree(const float degree) override;
	void SetPosition(const float x, const float y, const float z) override;
	void SetText(const wchar_t* text) override;

public:
	void Render(SpriteBatch* pSpriteBatch);

private:
	std::wstring	_text;
	Vector4			_color;
	Vector3			_position;
	Vector2			_pivot;
	Vector2			_scale{ 1, 1 };
	TextSystem*		_pTextSystem{ nullptr };
	SpriteFont*		_pSpriteFont{ nullptr };
	float			_rotation{ 0.f };
	float			_depth{ 0.f };	
};