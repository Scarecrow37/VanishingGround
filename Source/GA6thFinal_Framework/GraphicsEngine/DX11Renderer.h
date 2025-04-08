#pragma once
#include "IRenderer.h"
#include "Light.h"

class Texture;
class VertexShader;
class PixelShader;
class Mesh;
class SkyBoxRenderer;
class AO;

class DX11Renderer : public IRenderer
{
	struct DrawData
	{
		unsigned int modelID;
		unsigned int layerMask;
		Mesh* mesh;
	};
	enum TextureType { Diffuse, Normal, RMA, Emissive, ShadowPosition, LayerMask, End };
	enum ZWrite { On, Off, ZWrite_End };

public:
	explicit DX11Renderer() = default;
	virtual ~DX11Renderer() = default;

public:
	void Initialize() override;
	void Render() override;

private:
	void SetViewport(float width, float height);

	void ShadowPass();
	void GBufferPass(std::list<DrawData>& noneAlphaMeshes);
	void LigthingPass(SkyBoxRenderer* pSkyBox, std::list<DrawData>* alphaMeshes, std::list<std::pair<unsigned int, SkyBoxRenderer*>>& skyBoxes);
	void SkyBoxPass(std::list<std::pair<unsigned int, SkyBoxRenderer*>>& skyBoxes);
	void DeferredLighting();
	void ForwardLigthing(std::list<DrawData>& alphaMeshes);
	void AmbientOcclusion();
	void RenderMesh(std::list<DrawData>& renderData, std::shared_ptr<PixelShader>& pixelShader);

private:
	void InitState();
	void InitOptional();
	void InitMRT();
	void InitShaders();
	void InitDepthStencil();
	void InitStructuredBuffer();
	void InitFilters();
	void InitTextures();

private:
	// IRenderer을(를) 통해 상속됨
	void Free() override;

private:
	std::vector<Matrix>					_matrices;
	std::vector<Matrix>					_boneMatrices;
	std::vector<LightData>				_directionalLights;
	std::vector<LightData>				_pointLights;
	std::vector<Vector3>				_kernel;
	D3D11_VIEWPORT						_viewport{};

	// Filters	
	AO*									_pSSAO{ nullptr };

	// Shaders
	std::shared_ptr<VertexShader>		_vsShadow[MeshType::End];
	std::shared_ptr<PixelShader>		_psGBuffer;
	std::shared_ptr<PixelShader>		_psDeferredLighting;
	std::shared_ptr<PixelShader>		_psForwardLighting;	

	// Textures
	std::shared_ptr<Texture>			_aoNoiseTexture{ nullptr };

	// Device
	ID3D11DeviceContext*				_pDeviceContext{ nullptr };

	// View
	ID3D11DepthStencilView*				_pDefaultDSV{ nullptr };
	ID3D11ShaderResourceView*			_pDefaultSRV{ nullptr };
	ID3D11DepthStencilView*				_pShadowDSV{ nullptr };
	ID3D11ShaderResourceView*			_pShadowSRV{ nullptr };

	// State
	
	ID3D11RasterizerState*				_pRSSkyBoxState{ nullptr };
	ID3D11DepthStencilState*			_pAlphaDepthState{ nullptr };
};