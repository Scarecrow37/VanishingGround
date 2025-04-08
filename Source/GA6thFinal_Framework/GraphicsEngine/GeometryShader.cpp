#include "pch.h"
#include "GeometryShader.h"

GeometryShader::~GeometryShader()
{
	Free();
}

void GeometryShader::SetGeometryShader()
{
	_pDeviceContext->GSSetShader(_pGeometryShader, nullptr, 0);

	for (auto& slot : _inputBindSlot)
	{
		g_pConstantBuffer->SetConstantBuffer(slot.name.c_str(), ShaderType::Geometry, slot.start, slot.count);
	}
}

HRESULT GeometryShader::LoadResource(const std::filesystem::path& filePath)
{
	ID3D11Device* pDevice = g_pGraphicDevice->GetDevice();

#ifdef _DEBUG
	std::vector<uint8_t> geometryShaderData;
	CompileShader(geometryShaderData, filePath, "main", "gs_5_0");
#else
	std::vector<uint8_t> geometryShaderData = ReadData(filePath.c_str());
	if (geometryShaderData.empty())
	{
		CompileShader(geometryShaderData, filePath, "main", "gs_5_0");
	}
#endif

	ID3D11ShaderReflection* pReflector = nullptr;
	D3DReflect(geometryShaderData.data(), geometryShaderData.size(), IID_ID3D11ShaderReflection, (void**)&pReflector);

	D3D11_SHADER_DESC shaderDesc;
	pReflector->GetDesc(&shaderDesc);

	for (unsigned int i = 0; i < shaderDesc.BoundResources; i++)
	{
		D3D11_SHADER_INPUT_BIND_DESC inputBindDesc{};
		pReflector->GetResourceBindingDesc(i, &inputBindDesc);

		if (D3D_SIT_CBUFFER == inputBindDesc.Type)
		{
			ID3D11ShaderReflectionConstantBuffer* pConstantBuffer = pReflector->GetConstantBufferByName(inputBindDesc.Name);
			D3D11_SHADER_BUFFER_DESC bufferDesc{};
			pConstantBuffer->GetDesc(&bufferDesc);
			g_pConstantBuffer->AddConstantBuffer(std::filesystem::path(bufferDesc.Name).c_str(), bufferDesc.Size);
			_inputBindSlot.emplace_back(std::filesystem::path(inputBindDesc.Name).c_str(), inputBindDesc.BindPoint, inputBindDesc.BindCount);
		}
	}

	FAILED_CHECK_BREAK(pDevice->CreateGeometryShader(
					   geometryShaderData.data(),
					   geometryShaderData.size(),
					   nullptr,
					   &_pGeometryShader));

	SafeRelease(pReflector);

	return S_OK;
}

void GeometryShader::Free()
{
	SafeRelease(_pGeometryShader);
}
