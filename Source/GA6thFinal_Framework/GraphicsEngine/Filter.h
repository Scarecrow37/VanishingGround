#pragma once

class PixelShader;
class Filter abstract
{
public:
	explicit Filter();
	virtual ~Filter() = default;

public:
	virtual void Initialize() = 0;
	virtual void Render() = 0;

protected:
	ID3D11RenderTargetView*		_pBlendRTVs[2]{ nullptr, };
	ID3D11ShaderResourceView*	_pBlendSRVs[2]{ nullptr, };
	ID3D11DeviceContext*		_pDeviceContext{ nullptr };
	ID3D11RenderTargetView*		_pPostRTV{ nullptr };
	ID3D11ShaderResourceView*	_pPostSRV{ nullptr };
};