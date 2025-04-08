#pragma once
#include "Base.h"

class Texture;
class PixelShader;
class Material : public Base
{
public:
	explicit Material();
	virtual ~Material() = default;

public:
	bool IsAlpha() const { return _isAlpha; }
	void SetParameters(std::shared_ptr<PixelShader>& pixelShader);

public:
	void BindTextureMap(TextureType type, const std::shared_ptr<Texture>& texture);

private:
	// Base을(를) 통해 상속됨
	void Free() override;

private:
	std::vector<std::shared_ptr<Texture>> _textureMaps;
	bool _isAlpha{ false };
};