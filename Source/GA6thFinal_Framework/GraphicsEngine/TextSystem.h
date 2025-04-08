#pragma once
#include "Base.h"
#include "includes/ITextSystem.h"

namespace DirectX
{
	inline namespace DX11
	{
		class SpriteBatch;
		class SpriteFont;
	}
}

class TextRenderer;
class TextSystem : public Base, public GE::ITextSystem
{
public:
	explicit TextSystem() = default;
	virtual ~TextSystem() = default;

public:
	SpriteFont* GetSpriteFont(const wchar_t* filePath);

public:
	// ITextSystem을(를) 통해 상속됨
	void RegisterFont(const wchar_t* filePath);
	void CreateTextRenderer(GE::ITextRenderer** ppComponent) override;
	void RegisterRenderQueue(GE::ITextRenderer* pComponent) override;
	void UnRegisterRenderQueue(GE::ITextRenderer* pComponent) override;

public:
	void Initialize();
	void Render();

private:
	void Free() override;

private:
	std::unordered_map<std::wstring, SpriteFont*>	_fonts;
	std::vector<TextRenderer*>						_components;
	SpriteBatch*									_pSpriteBatch{ nullptr };
};