#pragma once
#include "Base.h"

class VIBuffer : public Base
{
public:
	struct VIBufferInfo
    {
        void* vertices = nullptr;
		void* indices = nullptr;
        unsigned int vertexByteWidth = 0;
		unsigned int vertexBufferStride = 0;
		unsigned int vertexBufferOffset = 0;
		unsigned int indexByteWidth = 0;
		unsigned int indexCount = 0;
    };
public:
	explicit VIBuffer() = default;
	virtual ~VIBuffer() = default;

public:
	void SetParameters(ID3D11DeviceContext* pDeviceContext);
	void DrawIndexed(ID3D11DeviceContext* pDeviceContext);

public:
	HRESULT Initialize(const VIBufferInfo& info);

private:
	// DXBase을(를) 통해 상속됨
	void Free() override;

private:
	ID3D11Buffer*	_pVertexBuffer{ nullptr };
	ID3D11Buffer*	_pIndexBuffer{ nullptr };
	unsigned int	_vertexBufferOffset{ 0 };
	unsigned int	_vertexBufferStride{ 0 };
	unsigned int	_indexCount{ 0 };
};