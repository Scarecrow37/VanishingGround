#include "pch.h"
#include "TextSystem.h"
#include "TextRenderer.h"

#include <directxtk/SpriteFont.h>
#include <directxtk/SpriteBatch.h>

SpriteFont* TextSystem::GetSpriteFont(const wchar_t* filePath)
{
	auto iter = _fonts.find(filePath);

	if (iter == _fonts.end())
	{
		RegisterFont(filePath);
	}

	return _fonts[filePath];
}

void TextSystem::RegisterFont(const wchar_t* filePath)
{
	SpriteFont* pFont = new SpriteFont(g_pGraphicDevice->GetDevice(), filePath);

	if (nullptr == pFont)
	{
		ASSERT(false, L"The font could not be found by that path.");
		return;
	}

	_fonts[filePath] = pFont;
}

void TextSystem::CreateTextRenderer(GE::ITextRenderer** ppComponent)
{
	(*ppComponent) = new TextRenderer(this);
}

void TextSystem::RegisterRenderQueue(GE::ITextRenderer* pComponent)
{
	auto compare = [pComponent](const auto* component)
		{ return static_cast<TextRenderer*>(pComponent)->GetID() == component->GetID();};

	if (std::ranges::any_of(_components, compare))
	{
		ASSERT(false, L"Already registered TextRenderer.");
		return;
	}

	_components.push_back(static_cast<TextRenderer*>(pComponent));
}

void TextSystem::UnRegisterRenderQueue(GE::ITextRenderer* pComponent)
{
	std::erase_if(_components, [pComponent](const auto* component)
		{ return static_cast<TextRenderer*>(pComponent)->GetID() == component->GetID(); });
}

void TextSystem::Initialize()
{
	_pSpriteBatch = new SpriteBatch(g_pGraphicDevice->GetDeviceContext());
}

void TextSystem::Render()
{
	_pSpriteBatch->Begin(SpriteSortMode_FrontToBack);

	for (auto& component : _components)
	{
		component->Render(_pSpriteBatch);
	}

	_pSpriteBatch->End();
}

void TextSystem::Free()
{
	for (auto& font : _fonts)
		delete font.second;

	delete _pSpriteBatch;
}