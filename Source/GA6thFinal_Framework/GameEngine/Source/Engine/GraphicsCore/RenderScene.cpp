#include "pch.h"
#include "RenderScene.h"
#include "FrameResource.h"
#include "Model.h"
#include "Quad.h"
#include "RenderPass.h"
#include "RenderTarget.h"
#include "RenderTechnique.h"
#include "Shader.h"
#include "UmScripts.h"

RenderScene::RenderScene() : _frameQuad{std::make_unique<Quad>()}, _frameShader{std::make_unique<Shader>()} {}

void RenderScene::UpdateRenderScene()
{
    // 비활성된 컴포넌트 제거
    auto first = std::remove_if(_renderQueue.begin(), _renderQueue.end(),
                                [](const auto& ptr) { return (!ptr->Enable || !ptr->gameObject->ActiveInHierarchy); });
    _renderQueue.erase(first, _renderQueue.end());


    _currentFrameIndex   = UmDevice.GetCurrentBackBufferIndex();
    Vector4    cameraPos = Vector4(UmMainCamera.GetWorldMatrix().Translation());
    CameraData cameraData{.View       = XMMatrixTranspose(UmMainCamera.GetViewMatrix()),
                          .Projection = XMMatrixTranspose(UmMainCamera.GetProjectionMatrix()),
                          .Position   = cameraPos};

    UmDevice.UpdateBuffer(_cameraBuffer, &cameraData, sizeof(CameraData));

    std::unordered_map<size_t, UINT>         materialPair;
    std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> handles;
    std::vector<XMMATRIX>                    worldMatrixes;
    std::vector<MaterialData>                materialDatas;
    UINT                                     materialID = 0;

    for (auto& component : _renderQueue)
    {
        auto& model     = component->GetModel();
        if (!model.get())
            continue;
        auto& meshes    = model->GetMeshes();
        auto& materials = model->GetMaterials();
        
        XMMATRIX world = XMMatrixTranspose(component->gameObject->transform->GetWorldMatrix());
        UINT     size  = (UINT)meshes.size();

        for (UINT i = 0; i < size; i++)
        {
            worldMatrixes.emplace_back(world);
            MaterialData materialData{};

            for (UINT j = 0; j < 4; j++)
            {
                if (nullptr == materials[i][j])
                    continue;

                auto iter = materialPair.find(materials[i][j]->GetHandle().ptr);
                if (iter == materialPair.end())
                {
                    materialPair.emplace(materials[i][j]->GetHandle().ptr, materialID);
                    materialData.ID[j] = materialID++;
                    handles.push_back(materials[i][j]->GetHandle());
                }
                else
                {
                    materialData.ID[j] = iter->second;
                }
            }

            materialDatas.push_back(materialData);
        }
    }

    UINT size = static_cast<UINT>(worldMatrixes.size());
    _frameResources[_currentFrameIndex]->CopyStructuredBuffer(UmDevice.GetCommandList().Get(), worldMatrixes.data(),
                                                              size * sizeof(ObjectData),
                                                              FrameResource::Type::TRANSFORM);
    _frameResources[_currentFrameIndex]->CopyStructuredBuffer(UmDevice.GetCommandList().Get(), materialDatas.data(),
                                                              size * sizeof(MaterialData),
                                                              FrameResource::Type::MATERIAL);
    _frameResources[_currentFrameIndex]->CopyDescriptors(handles);
}

void RenderScene::RegisterOnRenderQueue(MeshRenderer* renderable)
{
    auto iter = std::find_if(_renderQueue.begin(), _renderQueue.end(),
                             [renderable](const auto& ptr) { return ptr == renderable; });

    if (iter != _renderQueue.end())
    {
        ASSERT(false, L"RenderScene::RegisterRenderQueue : Already registered component.");
        return;
    }
    _renderQueue.push_back(renderable);
}

void RenderScene::Execute(ID3D12GraphicsCommandList* commandList)
{
    for (auto& [name, tech] : _techniques)
    {
        tech->Execute(commandList);
    }
}

D3D12_CPU_DESCRIPTOR_HANDLE RenderScene::GetFinalImage()
{
    return _gBufferSrvHandles[BASECOLOR];
}

void RenderScene::AddRenderTechnique(const std::string& name, std::shared_ptr<RenderTechnique> technique)
{
    technique->SetOwnerScene(this);
    technique->Initialize();
    _techniques[name] = technique;
}

// 250424
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void RenderScene::InitializeRenderScene()
{
    CreateRenderTarget();
    CreateDepthStencil();
    CreateFrameQuadAndFrameShader();
    CreateFramePSO();
    CreateSrvDescriptorHeap();
    CreateFrameResource();
}

void RenderScene::CreateRenderTarget()
{
    // gbuffer 생성
    _gBuffer.resize(_gBufferCount);
    _gBufferSrvHandles.resize(_gBufferCount);
    for (UINT i = 0; i <= GBuffer::EMISSIVE; ++i)
    {
        _gBuffer[i] = std::make_shared<RenderTarget>();
        _gBuffer[i]->Initialize(DXGI_FORMAT_R32G32B32A32_FLOAT);
        _gBufferSrvHandles[i] = _gBuffer[i]->CreateShaderResourceView();
    }
  
    _gBuffer[GBuffer::DEPTH] = std::make_shared<RenderTarget>();
    _gBuffer[GBuffer::DEPTH]->Initialize(DXGI_FORMAT_R32_FLOAT);
    _gBufferSrvHandles[GBuffer::DEPTH] = _gBuffer[GBuffer::DEPTH]->CreateShaderResourceView();
    
    _gBuffer[GBuffer::COSTOMDEPTH] = std::make_shared<RenderTarget>();
    _gBuffer[GBuffer::COSTOMDEPTH]->Initialize(DXGI_FORMAT_R32_UINT);
    _gBufferSrvHandles[GBuffer::COSTOMDEPTH] = _gBuffer[GBuffer::COSTOMDEPTH]->CreateShaderResourceView();
    


    // 후처리용으로 돌려쓸 renderTarget 생성해주기
    _renderTargets.resize(_renderTargetPoolCount);
    _renderTargetSrvHandles.resize(_renderTargetPoolCount);
    for (UINT i = 0; i < _renderTargetPoolCount; ++i)
    {
        _renderTargets[i] = std::make_shared<RenderTarget>();
        _renderTargets[i]->Initialize(DXGI_FORMAT_R32G32B32A32_FLOAT);
        _renderTargetSrvHandles[i] = _renderTargets[i]->CreateShaderResourceView();
    }

    // 메쉬 음영처리가 된 타겟 하나 생성 -> 이 타겟을 가져와서 후처리를 진행해야함.
    _meshLightingTarget = std::make_shared<RenderTarget>();
    _meshLightingTarget->Initialize(DXGI_FORMAT_R32G32B32A32_FLOAT);
    _meshLightingSrv = _meshLightingTarget->CreateShaderResourceView();
}

void RenderScene::CreateDepthStencil() 
{
    HRESULT hr = S_OK;
    hr         = UmViewManager.AddDescriptorHeap(ViewManager::Type::DEPTH_STENCIL, _depthStencilHandle);
    FAILED_CHECK_BREAK(hr);

    D3D12_RESOURCE_DESC depthDesc{.Dimension        = D3D12_RESOURCE_DIMENSION_TEXTURE2D,
                                  .Alignment        = 0,
                                  .Width            = UmDevice.GetMode().Width,
                                  .Height           = UmDevice.GetMode().Height,
                                  .DepthOrArraySize = 1,
                                  .MipLevels        = 1,
                                  .Format           = DXGI_FORMAT_R24G8_TYPELESS,
                                  .SampleDesc{.Count   = UmDevice.GetMSAAState() ? (UINT)4 : (UINT)1,
                                              .Quality = UmDevice.GetMSAAState() ? (UmDevice.GetMSAAQuality() - 1) : 0},
                                  .Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN,
                                  .Flags  = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL};

    D3D12_CLEAR_VALUE   optClear{.Format = UmDevice.GetDepthStencilFormat(), .DepthStencil{.Depth = 1.f, .Stencil = 0}};
    CD3DX12_HEAP_PROPERTIES property(D3D12_HEAP_TYPE_DEFAULT);
    hr = UmDevice.GetDevice()->CreateCommittedResource(&property, D3D12_HEAP_FLAG_NONE, &depthDesc,
                                                       D3D12_RESOURCE_STATE_PRESENT, &optClear,
                                                       IID_PPV_ARGS(_depthStencilBuffer.GetAddressOf()));

    D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{.Format        = UmDevice.GetDepthStencilFormat(),
                                          .ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D,
                                          .Flags         = D3D12_DSV_FLAG_NONE};

    UmDevice.GetDevice()->CreateDepthStencilView(_depthStencilBuffer.Get(), &dsvDesc, _depthStencilHandle);
}

void RenderScene::CreateFrameQuadAndFrameShader()
{
    // 화면 크기만한 quad만들기. NDC 좌표계로
    _frameQuad->Initialize(-1.f, 1.f, 2.f, 2.f, 0.f);
    _frameShader->BeginBuild();
    _frameShader->LoadShader(L"../Shaders/vs_quad.hlsl", Shader::Type::VS);
    _frameShader->LoadShader(L"../Shaders/ps_quad_frame.hlsl", Shader::Type::PS);
    _frameShader->EndBuild();

}

void RenderScene::CreateFramePSO()
{
    HRESULT                            hr = S_OK;
    D3D12_GRAPHICS_PIPELINE_STATE_DESC psodesc;
    ZeroMemory(&psodesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
    psodesc.RasterizerState       = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    psodesc.BlendState            = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    psodesc.DepthStencilState     = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
    psodesc.SampleMask            = UINT_MAX;
    psodesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    psodesc.InputLayout           = _frameShader->GetInputLayout();
    psodesc.NumRenderTargets      = 1;
    psodesc.RTVFormats[0]         = UmDevice.GetMode().Format;
    psodesc.DSVFormat             = DXGI_FORMAT_D24_UNORM_S8_UINT;
    psodesc.pRootSignature        = _frameShader->GetRootSignature().Get();
    psodesc.SampleDesc            = {1, 0};
    psodesc.VS                    = _frameShader->GetShaderByteCode(Shader::Type::VS);
    psodesc.PS                    = _frameShader->GetShaderByteCode(Shader::Type::PS);
    ComPtr<ID3D12Device> device   = UmDevice.GetDevice();

    hr = device->CreateGraphicsPipelineState(&psodesc, IID_PPV_ARGS(_framePSO.GetAddressOf()));
    FAILED_CHECK_BREAK(hr);
}

void RenderScene::CreateSrvDescriptorHeap() 
{
    D3D12_DESCRIPTOR_HEAP_DESC desc = {};
    desc.NumDescriptors             = 3;
    desc.Type                       = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    desc.Flags                      = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    HRESULT hr                      = S_OK;
    hr                              = UmDevice.GetDevice()->
                                      CreateDescriptorHeap(
                                          &desc, 
                                           IID_PPV_ARGS(_srvDescriptorHeap.GetAddressOf())
                                      );
    FAILED_CHECK_BREAK(hr);
}

void RenderScene::CreateFrameResource()
{
    _frameResources.resize(SWAPCHAIN_BUFFER_COUNT);
    for (UINT i = 0; i < SWAPCHAIN_BUFFER_COUNT; ++i)
    {
        _frameResources[i] = std::make_shared<FrameResource>();
        // 임시 텍스쳐 갯수가 달라질 수 있는거 아닌가요?
        _frameResources[i]->Initialize(100, 6);
    }
    // 임시 : 메인 카메라를 통해 Camera ConstantBuffer 만들기.
    CameraData cameraData{.View       = UmMainCamera.GetViewMatrix(),
                          .Projection = UmMainCamera.GetProjectionMatrix(),
                          .Position   = {0.f, 0.f, -5.f, 1.f}};

    UmDevice.CreateConstantBuffer(&cameraData, sizeof(CameraData), _cameraBuffer);
}
