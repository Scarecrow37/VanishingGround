#pragma once
#include "Base.h"

class VertexShader;
class VIBuffer;
class Sprite : public Base
{
	enum Type { D2D, D3D, D3D_BillBoard, End };
public:
	explicit Sprite() = default;
	virtual ~Sprite() = default;

public:
	void Initialize();
	void Render(unsigned int type);

private:
	// Base을(를) 통해 상속됨
	void Free() override;

private:
	std::shared_ptr<VertexShader>	_vsSprite[End];
	VIBuffer*						_pVIBuffer{ nullptr };
	ID3D11DeviceContext*			_pDeviceContext{ nullptr };
};