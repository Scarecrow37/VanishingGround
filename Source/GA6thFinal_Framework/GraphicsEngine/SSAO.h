#pragma once
#include "AO.h"

class Texture;
class SSAO : public AO
{
	enum Blur { X, Y, End };
	enum AO { Sample, Blend };

public:
	explicit SSAO() = default;
	virtual ~SSAO() = default;

public:
	// AO을(를) 통해 상속됨
	ID3D11ShaderResourceView* GetTexture() override;

public:
	// Filter을(를) 통해 상속됨
	void Initialize() override;
	void Render() override;

private:
	// AO을(를) 통해 상속됨
	void Free() override;

private:
	std::vector<Vector3> _kernel;
	std::vector<Vector3> _noise;
	std::shared_ptr<PixelShader> _psSSAO[End];
	std::shared_ptr<PixelShader> _psGaussianBlur[End];
	std::shared_ptr<PixelShader> _psBlend;
	std::shared_ptr<Texture> _texture;
};