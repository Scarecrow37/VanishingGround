#pragma once
#include "VIBuffer.h"

class BaseMesh
{
public:
	BaseMesh();
	virtual ~BaseMesh();

public:
	void Initialize(const VIBuffer::Descriptor& descriptor);
	void Render(ID3D12GraphicsCommandList* commandList);

private:
	std::unique_ptr<VIBuffer> _viBuffer;
};