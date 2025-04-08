#include "pch.h"
#include "Sampler.h"

void Sampler::Initialize()
{
	auto CreateSamplerDesc = [](D3D11_FILTER filter, D3D11_TEXTURE_ADDRESS_MODE mode)
		{
			D3D11_SAMPLER_DESC samplerDesc
			{
				.Filter = filter,
				.AddressU = mode,
				.AddressV = mode,
				.AddressW = mode,
				.ComparisonFunc = D3D11_COMPARISON_NEVER,
				.MinLOD = 0,
				.MaxLOD = D3D11_FLOAT32_MAX
			};

			return samplerDesc;
		};

	ID3D11Device* pDevice = g_pGraphicDevice->GetDevice();
	ID3D11SamplerState* pSampler = nullptr;

	D3D11_SAMPLER_DESC samplerDesc{};
	samplerDesc = CreateSamplerDesc(D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_WRAP);
	pDevice->CreateSamplerState(&samplerDesc, &pSampler);
	_samplers[L"samLinear_wrap"] = pSampler;

	samplerDesc = CreateSamplerDesc(D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_CLAMP);
	pDevice->CreateSamplerState(&samplerDesc, &pSampler);
	_samplers[L"samLinear_clamp"] = pSampler;

	D3D11_SAMPLER_DESC samplerDesc2
	{
		.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR,
		.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP,
		.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP,
		.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP,
		.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL,
	};

	pDevice->CreateSamplerState(&samplerDesc2, &pSampler);
	_samplers[L"samComparison"] = pSampler;
}

ID3D11SamplerState* Sampler::GetSampler(const wchar_t* name) const
{
	auto iter = _samplers.find(name);

	if (iter != _samplers.end())
		return iter->second;

	return nullptr;
}

void Sampler::Free()
{
	for (auto& pair : _samplers)
		SafeRelease(pair.second);
}