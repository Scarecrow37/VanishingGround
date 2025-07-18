#pragma once
#include "Resource.h"

class Font : public Resource
{
public:
	Font();
	virtual ~Font();

public:
    SpriteFont* GetFont() const { return _font.get(); }

public:
	// Resource을(를) 통해 상속됨
    void LoadResource(const std::filesystem::path& filePath) override;	

private:
    DescriptorHandles           _handle;
    std::unique_ptr<SpriteFont> _font;
};