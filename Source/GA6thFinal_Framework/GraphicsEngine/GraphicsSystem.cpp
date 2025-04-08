#include "pch.h"
#include "GraphicsSystem.h"
#include "AnimationSystem.h"
#include "RenderSystem.h"
#include "CameraSystem.h"
#include "LightSystem.h"

// Common
ResourceManager*	g_pResourceMgr;
CameraSystem*		g_pCameraSystem;
LightSystem*		g_pLightSystem;
std::wstring		g_shaderRootPath;

void GraphicsSystem::CreateAnimationSystem(GE::IAnimationSystem** ppAnimationSystem)
{
	(*ppAnimationSystem) = new AnimationSystem;
}

void GraphicsSystem::CreateCameraSystem(GE::ICameraSystem** ppCameraSystem)
{
	g_pCameraSystem = new CameraSystem;
	(*ppCameraSystem) = g_pCameraSystem;
}

void GraphicsSystem::CreateLightSystem(GE::ILightSystem** ppLightSystem)
{
	g_pLightSystem = new LightSystem;
	(*ppLightSystem) = g_pLightSystem;
}

void GraphicsSystem::CreateRenderSystem(GE::IRenderSystem** ppRenderSystem)
{
	(*ppRenderSystem) = new RenderSystem;
}

void GraphicsSystem::GetDevice(ID3D11Device** ppDevice)
{
    (*ppDevice) = g_pGraphicDevice->GetDevice();
}

void GraphicsSystem::Release()
{
	delete this;
}
