#pragma once
#include "Base.h"

class VertexShader;
class VIBuffer;
class Quad : public Base
{
public:
	explicit Quad() = default;
	virtual ~Quad() = default;

public:
	void Initialize();
	void Render();

private:
	// Base��(��) ���� ��ӵ�
	void Free() override;

private:
	std::shared_ptr<VertexShader> _vsQuad;
	VIBuffer* _pVIBuffer{ nullptr };
	ID3D11DeviceContext* _pDeviceContext{ nullptr };
};

