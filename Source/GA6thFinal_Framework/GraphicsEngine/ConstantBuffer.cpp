#include "pch.h"
#include "ConstantBuffer.h"

ConstantBuffer::ConstantBuffer()
{	
	_pDeviceContext = g_pGraphicDevice->GetDeviceContext();
}

void ConstantBuffer::AddConstantBuffer(const std::filesystem::path& name, const unsigned int byteWidth)
{
	ID3D11Device* pDevice = g_pGraphicDevice->GetDevice();
	const wchar_t* tag = name.c_str();

	if (_datas.find(tag) == _datas.end())
	{
		D3D11_BUFFER_DESC constantBuffer
		{
			.ByteWidth = byteWidth,
			.Usage = D3D11_USAGE_DYNAMIC,
			.BindFlags = D3D11_BIND_CONSTANT_BUFFER,
			.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE
		};

		ID3D11Buffer* pConstantBuffer = nullptr;
		FAILED_CHECK_BREAK(pDevice->CreateBuffer(&constantBuffer, nullptr, &pConstantBuffer));

		_datas[tag] = { pConstantBuffer, byteWidth };
	}
}

void ConstantBuffer::SetConstantBuffer(const wchar_t* name, ShaderType type, unsigned int start, unsigned int numBuffers)
{
	switch (type)
	{
	case ShaderType::Vertex:
		_pDeviceContext->VSSetConstantBuffers(start, numBuffers, &_datas[name].pBuffer);
		break;
	case ShaderType::Pixel:
		_pDeviceContext->PSSetConstantBuffers(start, numBuffers, &_datas[name].pBuffer);
		break;
	case ShaderType::Geometry:
		_pDeviceContext->GSSetConstantBuffers(start, numBuffers, &_datas[name].pBuffer);
		break;
	case ShaderType::Compute:
		_pDeviceContext->CSSetConstantBuffers(start, numBuffers, &_datas[name].pBuffer);
		break;
	}
}

void ConstantBuffer::UpdateConstantBuffer(const wchar_t* name, const void* pData)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource{};
	Data& data = _datas[name];

	HRESULT hr = _pDeviceContext->Map(data.pBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	memcpy(mappedResource.pData, pData, data.size);
	_pDeviceContext->Unmap(data.pBuffer, 0);
}

void ConstantBuffer::Free()
{
	for (auto& pair : _datas)
		SafeRelease(pair.second.pBuffer);
}