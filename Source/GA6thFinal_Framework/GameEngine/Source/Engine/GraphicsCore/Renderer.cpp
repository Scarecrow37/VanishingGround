#include "pch.h"
#include "Renderer.h"
#include "Shader.h"
#include "FrameResource.h"
#include "Model.h"
#include "UmScripts.h"

#define SeongU01
#ifdef SeongU01
#include "Box.h"
#include "Sphere.h"
#include "GeoSphere.h"
#include "Cylinder.h"
#include "Grid.h"
#include "Quad.h"
#include "RenderScene.h"
#include "PBRLitTechnique.h"
#endif

Renderer::Renderer() : _currnetState(0)
{
}

Renderer::~Renderer()
{
}

void Renderer::RegisterRenderQueue(MeshRenderer* component)
{
    auto iter = std::find_if(_components.begin(), _components.end(), [component](const auto& ptr) { return ptr == component; });

    if (iter != _components.end())
    {
        ASSERT(false, L"Renderer::RegisterRenderQueue : Already registered component.");
        return;
    }

    _components.push_back(component);
    // test code
    for (auto& meshRenerComps : _components)
    {
        _renderScenes["Test"]->RegisterOnRenderQueue(meshRenerComps);
    }
}

void Renderer::Initialize()
{
    /*if (IS_EDITOR)
    {
    }
    else
    {
    }*/
    //std::shared_ptr<RenderScene> testRenderScene = std::make_shared<RenderScene>();
    //testRenderScene->InitializeRenderScene(1);
    //std::shared_ptr<PBRTechnique> pbrTech = std::make_shared<PBRTechnique>();
    //testRenderScene->AddRenderTechnique("PBR", pbrTech);

    //_renderScenes["TEST PBR"] = testRenderScene;
    std::shared_ptr<RenderScene> testRenderScene = std::make_shared<RenderScene>();
    testRenderScene->InitializeRenderScene();
    std::shared_ptr<PBRLitTechnique> pbrTech = std::make_shared<PBRLitTechnique>();
    testRenderScene->AddRenderTechnique("PBRLIT", pbrTech);
    _renderScenes["Test"] = testRenderScene;

}

void Renderer::Update()
{
    // 비활성된 컴포넌트 제거
    auto first = std::remove_if(_components.begin(), _components.end(), [](const auto& ptr) { return (!ptr->Enable || !ptr->gameObject->ActiveInHierarchy); });
    _components.erase(first, _components.end());

    UmMainCamera.Update();

	UmDevice.ResetCommands();
	//UpdateFrameResource();
	UmDevice.ClearBackBuffer(D3D12_CLEAR_FLAG_DEPTH, { 0.5f, 0.5f, 0.5f, 1.f });
    for (auto& renderScene : _renderScenes)
    {
        renderScene.second->UpdateRenderScene();
    } 
}

void Renderer::Render()
{
	ComPtr<ID3D12GraphicsCommandList> commandList = UmDevice.GetCommandList();

    for (auto& renderScene : _renderScenes)
    {
        renderScene.second->Execute(commandList.Get());
    }

}

void Renderer::Flip()
{
    UmDevice.Flip();
}

