#pragma once
#include "includes/IGraphicsSystem.h"

class RenderSystem;
class GraphicsSystem : public GE::IGraphicsSystem
{
public:
	explicit GraphicsSystem() = default;
	virtual ~GraphicsSystem() = default;

public:
	// IGrahpicsSystem을(를) 통해 상속됨
	void CreateAnimationSystem(GE::IAnimationSystem** ppAnimationSystem) override;
	void CreateCameraSystem(GE::ICameraSystem** ppCameraSystem) override;
	void CreateLightSystem(GE::ILightSystem** ppLightSystem) override;
	void CreateRenderSystem(GE::IRenderSystem** ppRenderSystem) override;
    void GetDevice(ID3D11Device** ppDevice) override;
	void Release() override;
};