#pragma once

class Font;
class FontManager
{
public:
	FontManager();
	~FontManager();

public:
	HRESULT Initialize(const UINT numFonts);
	std::shared_ptr<Font> LoadFont(std::wstring_view filePath);

public:
	void Render();

private:
	std::unordered_map<std::wstring, std::shared_ptr<Font>> _fonts;
	std::shared_ptr<SpriteBatch> _spriteBatch;
	ComPtr<ID3D12DescriptorHeap> _fontHeap;
	UINT _numFonts;
	UINT _maxFonts;
	UINT _offset;
};