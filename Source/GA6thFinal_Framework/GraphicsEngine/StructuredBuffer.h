#pragma once
#include "Base.h"

class StructuredBuffer : public Base
{
	struct Data
	{
		ID3D11Buffer* pBuffer;
		ID3D11ShaderResourceView* pSRV;
		unsigned int size;
	};
public:
	explicit StructuredBuffer();
	virtual ~StructuredBuffer() = default;

public:
	void AddStructuredBuffer(const std::filesystem::path& name, const unsigned int byteWidth, const unsigned int numElements);
	void SetStructuredBuffer(const wchar_t* name, ShaderType type, unsigned int start = 0, unsigned int numBuffers = 1);
	void UpdateStructuredBuffer(const wchar_t* name, const void* pData);

private:
	// Base을(를) 통해 상속됨
	void Free() override;
	
private:
	std::unordered_map<std::wstring, Data> _datas;
	ID3D11DeviceContext* _pDeviceContext = nullptr;
};