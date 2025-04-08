#include "pch.h"
#include "SkyBoxRenderer.h"
#include "StaticMesh.h"
#include "Texture.h"
#include "VertexShader.h"
#include "PixelShader.h"
#include "Mesh.h"
#include "VIBuffer.h"

void SkyBoxRenderer::Query(void** ppOut)
{
	(*ppOut) = static_cast<GE::ISkyBoxRenderer*>(this);
}

void SkyBoxRenderer::SetSkyBoxTexture(const wchar_t* filePath)
{
	_skyBox = g_pResourceMgr->LoadResource<Texture>(filePath);
}

void SkyBoxRenderer::SetBRDFTexture(const wchar_t* filePath)
{
	_textures[BRDF] = g_pResourceMgr->LoadResource<Texture>(filePath);
}

void SkyBoxRenderer::SetDiffuseTexture(const wchar_t* filePath)
{
	_textures[Diffsue] = g_pResourceMgr->LoadResource<Texture>(filePath);
}

void SkyBoxRenderer::SetSpecularTexture(const wchar_t* filePath)
{
	_textures[Specular] = g_pResourceMgr->LoadResource<Texture>(filePath);
}

void SkyBoxRenderer::SetIBLTextures(ID3D11DeviceContext* pDeviceContext)
{
	for (int i = 0; i < End;i++)
	{
		pDeviceContext->PSGetShaderResources(11 + i, 1, _textures[i]->Get());
	}
}

void SkyBoxRenderer::SetParameter(ID3D11DeviceContext* pDeviceContext, unsigned int startSlot)
{
	pDeviceContext->PSSetShaderResources(startSlot, 1, _textures[Diffsue]->Get());
	pDeviceContext->PSSetShaderResources(startSlot + 1, 1, _textures[Specular]->Get());
	pDeviceContext->PSSetShaderResources(startSlot + 2, 1, _textures[BRDF]->Get());
}

void SkyBoxRenderer::Initialize(const wchar_t* filePath)
{
	_model = g_pResourceMgr->LoadResource<StaticMesh>(filePath);

	_vertexShader = g_pResourceMgr->LoadResource<VertexShader>(L"Assets/Shaders/SkyBoxVS.cso");
	_pixelShader = g_pResourceMgr->LoadResource<PixelShader>(L"Assets/Shaders/SkyBoxPS.cso");
	_type = MeshType::SkyBox;

	_model->BindVertexShader(_vertexShader);
}

void SkyBoxRenderer::Render(ID3D11DeviceContext* pDeviceContext)
{
	pDeviceContext->PSSetShaderResources(0, 1, _skyBox->Get());
	_vertexShader->SetVertexShader();
	_pixelShader->SetPixelShader();

	for (auto& mesh : _model->_meshs)
	{
		mesh->_pVIBuffer->SetParameters(pDeviceContext);
		mesh->_pVIBuffer->DrawIndexed(pDeviceContext);
	}
}

void SkyBoxRenderer::Release()
{
	MeshRenderer::Release();
}
