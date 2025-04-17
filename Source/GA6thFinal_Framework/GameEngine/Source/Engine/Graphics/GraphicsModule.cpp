#include "pch.h"
#include "GraphicsModule.h"

GraphicsModule::GraphicsModule()
	: _graphicsSystem(nullptr)
	, _renderSystem(nullptr)
	, _cameraSystem(nullptr)
	, _animationSystem(nullptr)
	, _lightSystem(nullptr)
	, _textSystem(nullptr)
	, _postprocessSystem(nullptr)
	, _spriteSystem(nullptr)
	, _unlitSystem(nullptr)
{
}

void GraphicsModule::PreInitialize()
{
    const wchar_t* path = nullptr;

#ifdef _DEBUG
    //SetDllDirectory(L"../dll/Debug");
    path = L"GraphicsEngine_d.dll";
#else
    //SetDllDirectory(L"../dll/Release");
    path = L"GraphicsEngine.dll";
#endif

    _dll = LoadLibrary(path);

    if (!_dll)
    {
        int a = 0;
        __debugbreak();
    }

    const CreateInstanceFunction pCreateFunc = reinterpret_cast<CreateInstanceFunction>(GetProcAddress(_dll, "DllCreateInstance"));
    pCreateFunc(&_graphicsSystem);

    // CameraSystem
    _graphicsSystem->CreateCameraSystem(&_cameraSystem);

    // RenderSystem
    _graphicsSystem->CreateRenderSystem(&_renderSystem);

    const GE::RENDERER_DESC desc
    {
        .hWnd = Global::engineCore->App.GetHwnd(),
        .shaderRootPath = L"../Shaders/",
        .width = 1920,
        .height = 1080,
        .type = GE::RENDERER_DESC::Type::DX11,
        .maxLayer = 1,
        .isFullScreen = false
    };
    _renderSystem->Initialize(&desc);

    _renderSystem->GetTextSystem(&_textSystem);
    _renderSystem->GetPostProcessSystem(&_postprocessSystem);
    _renderSystem->GetSpriteSystem(&_spriteSystem);
    _renderSystem->GetUnlitSystem(&_unlitSystem);

    _graphicsSystem->CreateAnimationSystem(&_animationSystem);
    _animationSystem->Initialize(1);

    _graphicsSystem->CreateLightSystem(&_lightSystem);
    
    ID3D11Device* device = nullptr;
    _graphicsSystem->GetDevice(&device);

    if (!device)
    {
        int a = 0;
        __debugbreak();
    }

    ImGuiDX11Module::SetDevice(device);
}

void GraphicsModule::ModuleInitialize()
{
}

void GraphicsModule::ModuleUnInitialize()
{
}

void GraphicsModule::PreUnInitialize()
{
    _lightSystem->Release();
    _animationSystem->Release();
    _renderSystem->Release();
    _cameraSystem->Release();
    _graphicsSystem->Release();

    FreeLibrary(_dll);
}


void GraphicsModule::PreUpdate(const float deltaTime) const
{
	_animationSystem->Update(deltaTime);
}

void GraphicsModule::PostUpdate(float deltaTime) const
{
	_cameraSystem->Update();
}

void GraphicsModule::Render() const
{
	_renderSystem->Render();
}

//void GraphicsModule::CreateCamera(Graphics::Camera** camera)
//{
//	if (camera == nullptr) Utility::ThrowIfFailed()(E_INVALIDARG);
//	else
//	{
//		Camera* temp = new Camera();
//		if (temp == nullptr) Utility::ThrowIfFailed()(E_OUTOFMEMORY);
//		GE::ICamera* geCamera = nullptr;
//		_cameraSystem->CreateCamera(&geCamera);
//		temp->Setup(geCamera);
//		*camera = temp;
//	}
//}
//
//void GraphicsModule::RegisterCamera(const std::wstring_view name, Graphics::Camera* camera)
//{
//	const Camera* geCamera = reinterpret_cast<Camera*>(camera);
//	_cameraSystem->RegisterCamera(name.data(), geCamera->Get());
//}
//
//void GraphicsModule::UnregisterCamera(const std::wstring_view name)
//{
//	_cameraSystem->UnRegisterCamera(name.data());
//}
//
//void GraphicsModule::SetActiveCamera(const std::wstring_view name)
//{
//	_cameraSystem->SetCurrentCamera(name.data());
//}