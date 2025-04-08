#pragma once
#include "Base.h"

class StateManagement : public Base
{
public:
	enum class Type { NoneBlend, AlphaBlend };
public:
	explicit StateManagement() = default;
	virtual ~StateManagement() = default;

public:
	ID3D11BlendState* GetBlendState(const wchar_t* name) { return static_cast<ID3D11BlendState*>(_blendStates[name]); }

public:
	void Initialize();
	void AddBlendState(const wchar_t* name, Type type);

private:
	// Base을(를) 통해 상속됨
	void Free() override;
	
private:
	std::unordered_map<std::wstring, ID3D11DeviceChild*> _blendStates;
	D3D11_BLEND_DESC _noneBlendDesc{};
	D3D11_BLEND_DESC _alphaBlendDesc{};
};