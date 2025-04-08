#pragma once
#include "Resource.h"

class Sampler : public Base
{
public:
	explicit Sampler() = default;
	virtual ~Sampler() = default;

public:
	void Initialize();

public:
	ID3D11SamplerState* GetSampler(const wchar_t* name) const;

private:
	void Free() override;

private:
	std::unordered_map<std::wstring, ID3D11SamplerState*>  _samplers;
};