#include "pch.h"
#include "StructuredBuffer.h"

StructuredBuffer::StructuredBuffer()
{
	_pDeviceContext = g_pGraphicDevice->GetDeviceContext();
}

void StructuredBuffer::AddStructuredBuffer(const std::filesystem::path& name, const unsigned int byteWidth, const unsigned int numElements)
{
	ID3D11Device* pDevice = g_pGraphicDevice->GetDevice();
	const wchar_t* tag = name.c_str();

	if (_datas.find(tag) == _datas.end())
	{
		D3D11_BUFFER_DESC bufferDesc
		{
			.ByteWidth = byteWidth,
			.Usage = D3D11_USAGE_DEFAULT,
			.BindFlags = D3D11_BIND_SHADER_RESOURCE,
			.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED,
			.StructureByteStride = sizeof(XMMATRIX)
		};

		ID3D11Buffer* pStructuredBuffer = nullptr;
		FAILED_CHECK_BREAK(pDevice->CreateBuffer(&bufferDesc, nullptr, &pStructuredBuffer));

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc
		{
			.Format = DXGI_FORMAT_UNKNOWN, 
			.ViewDimension = D3D11_SRV_DIMENSION_BUFFER,
			.Buffer
			{
				.FirstElement = 0,
				.NumElements = numElements
			}
		};

		ID3D11ShaderResourceView* pSRV = nullptr;
		pDevice->CreateShaderResourceView(pStructuredBuffer, &srvDesc, &pSRV);

		_datas[tag] = { pStructuredBuffer, pSRV, byteWidth };
	}
}

void StructuredBuffer::SetStructuredBuffer(const wchar_t* name, ShaderType type, unsigned int start, unsigned int numBuffers)
{
	switch (type)
	{
	case ShaderType::Vertex:
		_pDeviceContext->VSSetShaderResources(start, numBuffers, &_datas[name].pSRV);
		break;
	case ShaderType::Pixel:
		_pDeviceContext->PSSetShaderResources(start, numBuffers, &_datas[name].pSRV);
		break;
	}
}

void StructuredBuffer::UpdateStructuredBuffer(const wchar_t* name, const void* pData)
{
	_pDeviceContext->UpdateSubresource(_datas[name].pBuffer, 0, nullptr, pData, 0, 0);
}

void StructuredBuffer::Free()
{
	for (auto& pair : _datas)
	{
		SafeRelease(pair.second.pBuffer);
		SafeRelease(pair.second.pSRV);
	}
}