#pragma once
#include "../../GraphicsEngine/includes/GraphicsEngine.h"

class GraphicsModule : public IAppModule
{
	using CreateInstanceFunction = HRESULT(__stdcall*)(void* ppv);
public:
    GraphicsModule();

    // IAppModule을(를) 통해 상속됨
    void PreInitialize() override;
    void ModuleInitialize() override;
    void PreUnInitialize() override;
    void ModuleUnInitialize() override;

	void PreUpdate(float deltaTime) const;
	void PostUpdate(float deltaTime) const;
	void Render() const;

	GE::IRenderSystem* GetRenderSystem() const { return _renderSystem; }
	GE::ICameraSystem* GetCameraSystem() const  { return _cameraSystem; }
	GE::IAnimationSystem* GetAnimationSystem() const { return _animationSystem; }
	GE::ILightSystem* GetLightSystem() const  { return _lightSystem; }
	GE::ITextSystem* GetTextSystem() const  { return _textSystem; }
	GE::IPostProcessSystem* GetPostProcessSystem() const { return _postprocessSystem; }
	GE::ISpriteSystem* GetSpriteSystem() const { return _spriteSystem; }
	GE::IUnlitSystem* GetUnlitSystem() const { return _unlitSystem; }

private:
	HMODULE _dll;

	GE::IGraphicsSystem* _graphicsSystem;

	// Sub systems
	GE::IRenderSystem* _renderSystem;
	GE::ICameraSystem* _cameraSystem;
	GE::IAnimationSystem* _animationSystem;
	GE::ILightSystem* _lightSystem;
	GE::ITextSystem* _textSystem;
	GE::IPostProcessSystem* _postprocessSystem;
	GE::ISpriteSystem* _spriteSystem;
	GE::IUnlitSystem* _unlitSystem;    
};