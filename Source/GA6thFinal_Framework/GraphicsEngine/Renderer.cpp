#include "pch.h"
#include "Renderer.h"

// Shader
#include "VertexShader.h"
#include "PixelShader.h"
#include "ComputeShader.h"
#include "GeometryShader.h"

// Geometry
#include "Box.h"
#include "Cylinder.h"
#include "GeoSphere.h"
#include "Grid.h"
#include "Model.h"
#include "Quad.h"
#include "Sphere.h"

// Techniques
#include "BlendTechnique.h"
#include "BloomTechnique.h"
#include "EditorDrawTechnique.h"
#include "FontTechnique.h"
#include "PBRLitTechnique.h"
#include "ParticleRenderTechnique.h"
#include "RayTracingTechnique.h"
#include "SkyBoxRenderTechnique.h"
#include "UITechnique.h"
#include "SSAOTechnique.h"

Renderer::Renderer() {}

Renderer::~Renderer() {}

D3D12_GPU_DESCRIPTOR_HANDLE Renderer::GetRenderSceneImage(std::string_view renderSceneName)
{
    auto iter = _renderScenes.find(renderSceneName.data());

    if (iter == _renderScenes.end())
    {
        std::wstring msg = L"Renderer::GetRenderSceneImage: RenderSceneName '" +
                           std::wstring(renderSceneName.begin(), renderSceneName.end()) + L"' is not registered.";
        GRAPHICS_ASSERT(false, msg.c_str());
    }

    auto& scene = iter->second;
    return scene->GetFinalImage();
}

std::shared_ptr<Camera> Renderer::GetCamera(std::string_view renderSceneName)
{
    auto iter = _renderScenes.find(renderSceneName.data());

    if (iter == _renderScenes.end())
    {
        std::wstring msg = L"Renderer::GetCamera: RenderSceneName '" +
                           std::wstring(renderSceneName.begin(), renderSceneName.end()) + L"' is not registered.";
        GRAPHICS_ASSERT(false, msg.c_str());
    }

    auto& scene = iter->second;
    return scene->GetCamera();
}

RenderScene* Renderer::GetRenderScene(std::string_view renderSceneName)
{
    auto iter = _renderScenes.find(renderSceneName.data());
    if (iter == _renderScenes.end())
    {
        std::wstring msg = L"Renderer::GetRenderScene: RenderSceneName '" +
                           std::wstring(renderSceneName.begin(), renderSceneName.end()) + L"' is not registered.";
        GRAPHICS_ASSERT(false, msg.c_str());
    }

    return iter->second.get();
}

void Renderer::SetCamera(std::string_view renderSceneName, std::shared_ptr<Camera> camera)
{
    auto iter = _renderScenes.find(renderSceneName.data());

    if (iter == _renderScenes.end())
    {
        std::wstring msg = L"Renderer::SetCamera: RenderSceneName '" +
                           std::wstring(renderSceneName.begin(), renderSceneName.end()) + L"' is not registered.";
        GRAPHICS_ASSERT(false, msg.c_str());
    }

    auto& scene = iter->second;
    scene->SetCamera(camera);
}

void Renderer::SetCurrentScene(std::string_view sceneName)
{
    auto iter = _renderScenes.find(sceneName.data());
    if (iter == _renderScenes.end())
    {
        std::wstring msg = L"Renderer::SetCurrentScene: RenderSceneName '" + std::wstring(sceneName.begin(), sceneName.end()) + L"' is not registered.";
        GRAPHICS_ASSERT(false, msg.c_str());
    }

    _currentSceneName = sceneName;
}

void Renderer::AddRenderScene(std::string_view sceneName, RenderTechniqueFlag flag)
{
    auto iter = _renderScenes.find(sceneName.data());
    if (iter != _renderScenes.end())
    {
        std::wstring msg = L"Renderer::AddRenderScene: RenderSceneName '" + std::wstring(sceneName.begin(), sceneName.end()) + L"' is already registered.";
        GRAPHICS_ASSERT(false, msg.c_str());
        return;
    }

    if (RenderTechniqueFlag::NONE == flag)
    {
        GRAPHICS_ASSERT(false, L"Renderer::AddRenderScene: No render techniques specified.");
        return;
    }
    if (sceneName == "Game")
        Global::isRayTracing = flag & RenderTechniqueFlag::RAY_TRACING_TECH ? true : false;
    
    std::unique_ptr<RenderScene> scene = std::make_unique<RenderScene>(sceneName);
    scene->InitializeRenderScene();

    // MeshRenderTarget Pass
    if (RenderTechniqueFlag::SKY_BOX_TECH & flag)
    {
        scene->AddRenderTechnique(std::make_unique<SkyBoxRenderTechnique>());
    }
    if (RenderTechniqueFlag::RAY_TRACING_TECH & flag)
    {
        scene->AddRenderTechnique(std::make_unique<RayTracingTechnique>());
    }
    if (RenderTechniqueFlag::PBR_TECH & flag)
    {
        scene->AddRenderTechnique(std::make_unique<PBRLitTechnique>());
    }
    if (RenderTechniqueFlag::SSAO_TECH & flag)
    {
        scene->AddRenderTechnique(std::make_unique<SSAOTechnique>());
    }
    if (RenderTechniqueFlag::PARTICLE_TECH & flag)
    {
        scene->AddRenderTechnique(std::make_unique<ParticleRenderTechnique>());
        if ("Editor" == sceneName)
            Global::particleManager->AddSceneResource(sceneName, "Game");
        else
            Global::particleManager->AddSceneResource(sceneName);
    }
    if (RenderTechniqueFlag::EDITOR_DRAW_TECH & flag)
    {
        scene->AddRenderTechnique(std::make_unique<EditorDrawTechnique>());
    }

    // FinalRenderTarget Pass

    if (RenderTechniqueFlag::BLOOM_TECH & flag)
    {
        scene->AddRenderTechnique(std::make_unique<BloomTechnique>());
    }
    
    // Blend Pass
    scene->AddRenderTechnique(std::make_unique<BlendTechnique>());

    // UI Pass
    if (RenderTechniqueFlag::UI_TECH & flag)
    {
        scene->AddRenderTechnique(std::make_unique<UITechnique>());
    }
    if (RenderTechniqueFlag::FONT_TECH & flag)
    {
        scene->AddRenderTechnique(std::make_unique<FontTechnique>());
    }

    scene->AddRenderPassDatas();

    _renderScenes.try_emplace(sceneName.data(), std::move(scene));
}

void Renderer::RegisterRenderQueue(std::string_view sceneName, MeshRenderer* component)
{
    auto iter = _renderScenes.find(sceneName.data());

    if (iter == _renderScenes.end())
    {
        GRAPHICS_ASSERT(false, L"Renderer::RegisterRenderQueue : Render Scene Not Registered.");
    }

    auto& scene = iter->second;
    scene->RegisterOnRenderQueue(component);
}

void Renderer::RegisterRenderQueue(std::string_view sceneName, SpriteRenderer* component)
{
    auto iter = _renderScenes.find(sceneName.data());

    if (iter == _renderScenes.end())
    {
        GRAPHICS_ASSERT(false, L"Renderer::RegisterRenderQueue : Render Scene Not Registered.");
    }

    auto& scene = iter->second;
    scene->RegisterOnRenderQueue(component);
}

void Renderer::RegisterRenderQueue(std::string_view sceneName, FontRenderer* component)
{
    auto iter = _renderScenes.find(sceneName.data());

    if (iter == _renderScenes.end())
    {
        GRAPHICS_ASSERT(false, L"Renderer::RegisterRenderQueue : Render Scene Not Registered.");
    }

    auto& scene = iter->second;
    scene->RegisterOnRenderQueue(component);
}

void Renderer::SetSkyBox(std::string_view sceneName, std::wstring_view path)
{
    auto iter = _renderScenes.find(sceneName.data());

    if (iter == _renderScenes.end())
    {
        GRAPHICS_ASSERT(false, L"Renderer::RegisterRenderQueue : Render Scene Not Registered.");
    }

    auto& scene = iter->second;
    scene->SetSkyBox(path);
}

void Renderer::ResetSkyBox(std::string_view sceneName) 
{
    auto iter = _renderScenes.find(sceneName.data());

    if (iter == _renderScenes.end())
    {
        GRAPHICS_ASSERT(false, L"Renderer::RegisterRenderQueue : Render Scene Not Registered.");
    }

    auto& scene = iter->second;
    scene->ResetSkyBox();
}

void Renderer::Initialize()
{
    CreateDefaultResource();

    _shader = std::make_unique<ShaderBuilder>();

    _shader->BeginBuild();
    _shader->SetShader(L"../Shaders/vs_quad.hlsl", ShaderBuilder::Type::VS);
    _shader->SetShader(L"../Shaders/ps_to_backbuffer.hlsl", ShaderBuilder::Type::PS);
    _shader->EndBuild();

    ID3D12Device*                      device = Global::device->GetDevice();
    D3D12_GRAPHICS_PIPELINE_STATE_DESC psodesc{};
    psodesc.RasterizerState               = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    psodesc.BlendState                    = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    psodesc.DepthStencilState             = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
    psodesc.DepthStencilState.DepthEnable = FALSE;
    psodesc.SampleMask                    = UINT_MAX;
    psodesc.PrimitiveTopologyType         = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    psodesc.InputLayout                   = _shader->GetInputLayout();
    psodesc.NumRenderTargets              = 1;
    psodesc.RTVFormats[0]                 = Global::device->GetBackBufferFormat();
    psodesc.pRootSignature                = _shader->GetRootSignature();
    psodesc.SampleDesc                    = {1, 0};
    psodesc.VS                            = _shader->GetShaderByteCode(ShaderBuilder::Type::VS);
    psodesc.PS                            = _shader->GetShaderByteCode(ShaderBuilder::Type::PS);

    HRESULT hr = S_OK;
    hr         = device->CreateGraphicsPipelineState(&psodesc, IID_PPV_ARGS(&_pipelineState));
    FAILED_CHECK_MESSAGE(hr, L"Renderer::Initialize device->CreateGraphicsPipelineState Failed");

    auto quadModel = Global::resourceManager->LoadResource<Model>(L"Quad");
    _frameQuad     = quadModel->GetMeshes().front().get();
}

void Renderer::Update()
{
    Global::device->ClearBackBuffer(D3D12_CLEAR_FLAG_DEPTH, { 0.5f, 0.5f, 0.5f, 1.f });

    for (auto& renderScene : _renderScenes)
    {
        renderScene.second->UpdateRenderScene();
    }
}

void Renderer::Render()
{
    for (auto& renderScene : _renderScenes)
    {
        renderScene.second->Execute();
    }   

    RenderToBackBuffer();
}

void Renderer::Flip()
{    
    Global::device->Execute();
    Global::device->Flip();
    Global::device->ResetCommands();
    Global::device->ResetComputeCommands();
}

void Renderer::RenderToBackBuffer()
{
    if (_currentSceneName.empty())
    {
        return;
    }

    auto iter = _renderScenes.find(_currentSceneName.data());
    if (iter == _renderScenes.end())
    {
        GRAPHICS_ASSERT(false, L"Renderer::DrawCurrentSceneToBackBuffer: Current scene not found.");
        return;
    }

    auto& scene = iter->second;    

    auto commandList = Global::device->GetCommandList();
    auto backBuffer  = Global::device->GetBackBufferHandle();

    commandList->OMSetRenderTargets(1, &backBuffer, false, nullptr);
    commandList->RSSetViewports(1, &Global::device->GetMainViewport());
    commandList->RSSetScissorRects(1, &Global::device->GetMainScissorRect());

    commandList->SetPipelineState(_pipelineState.Get());
    commandList->SetGraphicsRootSignature(_shader->GetRootSignature());

    auto descriptorHeap = Global::viewManager->GetShaderResourceHeap();
    commandList->SetDescriptorHeaps(1, &descriptorHeap);
    commandList->SetGraphicsRootDescriptorTable(_shader->GetRootParameterIndex("screenTexture"), scene->GetFinalImage());

    _frameQuad->Render(commandList);
}

void Renderer::CreateDefaultResource()
{
    CreateDefaultGeometry();
    CreateDefaultTexture();
    CreateDefaultRenderTarget();
    CreateDefaultShader();
}

void Renderer::CreateDefaultGeometry()
{
    std::unique_ptr<Box> box = std::make_unique<Box>();
    box->Initialize(1.f, 1.f, 1.f);

    std::unique_ptr<Sphere> sphere = std::make_unique<Sphere>();
    sphere->Initialize(1.f, 20, 20);

    std::unique_ptr<GeoSphere> geoSphere = std::make_unique<GeoSphere>();
    geoSphere->Initialize(1.f, 5);

    std::unique_ptr<Cylinder> cylinder = std::make_unique<Cylinder>();
    cylinder->Initialize(0.5f, 0.3f, 2.f, 20, 20);

    std::unique_ptr<Grid> grid = std::make_unique<Grid>();
    grid->Initialize(20.f, 30.f, 4, 4);

    std::unique_ptr<Quad> quad = std::make_unique<Quad>();
    quad->Initialize(-1.0f, 1.0f, 2.0f, 2.0f, 0.0f);

    std::unique_ptr<Quad> halfQuad = std::make_unique<Quad>();
    halfQuad->Initialize(-0.5f, 0.5f, 1.0f, 1.0f, 0.0f);

    std::shared_ptr<Model>    geometry;
    std::unique_ptr<BaseMesh> baseMesh;

    auto& resourceManager = Global::resourceManager;

    baseMesh = std::move(box);
    geometry = std::make_shared<Model>();
    geometry->AddMesh(std::move(baseMesh));
    _defaultResource.push_back(geometry);
    resourceManager->AddResource(L"Box", geometry);

    baseMesh = std::move(cylinder);
    geometry = std::make_shared<Model>();
    geometry->AddMesh(std::move(baseMesh));
    _defaultResource.push_back(geometry);
    resourceManager->AddResource(L"Cylinder", geometry);

    baseMesh = std::move(sphere);
    geometry = std::make_shared<Model>();
    geometry->AddMesh(std::move(baseMesh));
    _defaultResource.push_back(geometry);
    resourceManager->AddResource(L"Sphere", geometry);

    baseMesh = std::move(geoSphere);
    geometry = std::make_shared<Model>();
    geometry->AddMesh(std::move(baseMesh));
    _defaultResource.push_back(geometry);
    resourceManager->AddResource(L"GeoSphere", geometry);

    baseMesh = std::move(grid);
    geometry = std::make_shared<Model>();
    geometry->AddMesh(std::move(baseMesh));
    _defaultResource.push_back(geometry);
    resourceManager->AddResource(L"Grid", geometry);

    baseMesh = std::move(quad);
    geometry = std::make_shared<Model>();
    geometry->AddMesh(std::move(baseMesh));
    _defaultResource.push_back(geometry);
    resourceManager->AddResource(L"Quad", geometry);

    baseMesh = std::move(halfQuad);
    geometry = std::make_shared<Model>();
    geometry->AddMesh(std::move(baseMesh));
    _defaultResource.push_back(geometry);
    resourceManager->AddResource(L"HalfQuad", geometry);
}

void Renderer::CreateDefaultTexture()
{
    D3D12_RESOURCE_DESC texDesc = {};
    texDesc.Dimension           = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    texDesc.Width               = 1;
    texDesc.Height              = 1;
    texDesc.DepthOrArraySize    = 1;
    texDesc.MipLevels           = 1;
    texDesc.Format              = DXGI_FORMAT_R8G8B8A8_UNORM;
    texDesc.SampleDesc.Count    = 1;
    texDesc.Layout              = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    texDesc.Flags               = D3D12_RESOURCE_FLAG_NONE;

    ID3D12Device*          device = Global::device->GetDevice();
    ComPtr<ID3D12Resource> texture;

    auto defaultHeapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
    device->CreateCommittedResource(&defaultHeapProp, D3D12_HEAP_FLAG_NONE, &texDesc, D3D12_RESOURCE_STATE_COPY_DEST,
                                    nullptr, IID_PPV_ARGS(&texture));

    UINT64                 uploadBufferSize;
    ComPtr<ID3D12Resource> uploadHeap;

    device->GetCopyableFootprints(&texDesc, 0, 1, 0, nullptr, nullptr, nullptr, &uploadBufferSize);
    auto uploadHeapProp       = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    auto uploadBufferSizeProp = CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize);
    device->CreateCommittedResource(&uploadHeapProp, D3D12_HEAP_FLAG_NONE, &uploadBufferSizeProp,
                                    D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
                                    IID_PPV_ARGS(&uploadHeap));

    D3D12_SUBRESOURCE_DATA textureData   = {};
    static const uint8_t   blackPixel[4] = {0, 0, 0, 255};
    textureData.pData                    = blackPixel;
    textureData.RowPitch                 = 4;
    textureData.SlicePitch               = 4;

    ID3D12GraphicsCommandList* commandList = Global::device->GetCommandList();
    UpdateSubresources(commandList, texture.Get(), uploadHeap.Get(), 0, 0, 1, &textureData);
    CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        texture.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

    commandList->ResourceBarrier(1, &barrier);

    std::shared_ptr<Texture> textureResource = std::make_shared<Texture>();
    textureResource->SetResource(texture.Get());
    textureResource->CreateShaderResourceView();

    Global::resourceManager->AddResource("BlackTexture", textureResource);
    _defaultResource.push_back(textureResource);

    Global::device->UploadResource(uploadHeap);
}

void Renderer::CreateDefaultRenderTarget()
{
    SharedResource<RenderTarget> renderTarget;
   
    auto resolution = Global::device->GetResolution();
    auto desc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R32G32B32A32_FLOAT, resolution.Width, resolution.Height, 1, 1, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);

    while (desc.Width > 1 || desc.Height > 1)
    {
        renderTarget = MakeSharedResource<RenderTarget>();
        renderTarget->Initialize(desc, 0.f);

        std::string name = std::format("Mipmap{}x{}", desc.Width, desc.Height);

        Global::multiRenderTargetManager->AddRenderTarget(name, renderTarget);
        Global::multiRenderTargetManager->AddRenderTargetGroup("Mipmap", name);

        desc.Width >>= 1;
        desc.Height >>= 1;
    }

    renderTarget = MakeSharedResource<RenderTarget>();
    renderTarget->Initialize(desc, 0.f);

    std::string name = std::format("Mipmap{}x{}", desc.Width, desc.Height);

    Global::multiRenderTargetManager->AddRenderTarget(name, renderTarget);
    Global::multiRenderTargetManager->AddRenderTargetGroup("Mipmap", name);

    desc.Width = resolution.Width;
    desc.Height = resolution.Height;
    Global::multiRenderTargetManager->InitializeRenderTargetPool(4, desc);
}

void Renderer::CreateDefaultShader()
{
    // L"../Shaders 폴더를 탐색 후 모든 쉐이더 파일을 미리 컴파일
    std::filesystem::path shaderDir = L"../Shaders";

    for (const auto& entry : std::filesystem::recursive_directory_iterator(shaderDir))
    {
        if (entry.is_regular_file() && entry.path().extension() == L".hlsl")
        {
            std::wstring_view shaderPath = entry.path().c_str();

            if (shaderPath.find(L"vs_") != std::wstring_view::npos)
            {
                _defaultResource.push_back(Global::resourceManager->LoadResource<VertexShader>(shaderPath));
            }
            else if (shaderPath.find(L"ps_") != std::wstring_view::npos)
            {
                _defaultResource.push_back(Global::resourceManager->LoadResource<PixelShader>(shaderPath));
            }
            else if (shaderPath.find(L"cs_") != std::wstring_view::npos)
            {
                _defaultResource.push_back(Global::resourceManager->LoadResource<ComputeShader>(shaderPath));
            }
            else if (shaderPath.find(L"gs_") != std::wstring_view::npos)
            {
                _defaultResource.push_back(Global::resourceManager->LoadResource<GeometryShader>(shaderPath));
            }

            Global::shaderPathMappings[entry.path().filename()] = shaderPath;
        }
    }
}