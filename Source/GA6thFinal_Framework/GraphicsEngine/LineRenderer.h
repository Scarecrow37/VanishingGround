#pragma once
#include "UnlitRenderer.h"
#include "includes/ILineRenderer.h"

class VertexShader;
class PixelShader;
class GeometryShader;
class Texture;
class LineRenderer final : public GE::ILineRenderer, public UnlitRenderer
{
private:
	struct Line
	{
		Vector4 position;
		Vector2 waveData;
	};
	enum Type { Base, Mask, Gradient, End };

	struct WaveData
	{
		float time;
		float waveAmplitude;
		float waveFrequency;
	};

public:
	// ILineRenderer을(를) 통해 상속됨
	void Release() override;
	void SetSourcePosition(float* pArray) override;
	void SetDestinationPosition(float* pArray) override;	
	void SetSegment(unsigned int segment) override;
	void SetBaseTexture(const wchar_t* filePath) override;
	void SetMaskTexture(const wchar_t* filePath) override;
	void SetGradientTexture(const wchar_t* filePath) override;
	void Update(float deltaTime) override;

	// ILineRenderer을(를) 통해 상속됨
	void SetPostEffectLayer(unsigned int layer) override;

public:
	void Initialize();
	void Render();

private:
	std::shared_ptr<VertexShader>	_vs;
	std::shared_ptr<GeometryShader>	_gs;
	std::shared_ptr<PixelShader>	_ps;
	WaveData						_waveData;
	ID3D11DeviceContext*			_pDeviceContext{ nullptr };
	ID3D11Buffer*					_pVertexBuffer{ nullptr };
	Vector3*						_pSrcPosition;
	Vector3*						_pDstPosition;
	std::shared_ptr<Texture>		_textures[End];
	unsigned int					_numSegments{ 0 };
	unsigned int					_stride{ 0 };
	unsigned int					_offset{ 0 };
};