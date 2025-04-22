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
    _currentFrameIndex = UmDevice.GetCurrentBackBufferIndex();

    CameraData cameraData{
        .View       = XMMatrixTranspose(UmMainCamera.GetViewMatrix()),
        .Projection = XMMatrixTranspose(UmMainCamera.GetProjectionMatrix()),
    };

    UmDevice.UpdateBuffer(_cameraBuffer, &cameraData, sizeof(CameraData));

    std::unordered_map<size_t, UINT>         materialPair;
    std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> handles;
    std::vector<XMMATRIX>                    worldMatrixes;
    std::vector<MaterialData>                materialDatas;
    UINT                                     materialID = 0;

    for (auto& component : _renderQueue)
    {
        auto& model     = component->GetModel();
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
                                         size * sizeof(ObjectData), FrameResource::Type::TRANSFORM);
    _frameResources[_currentFrameIndex]->CopyStructuredBuffer(UmDevice.GetCommandList().Get(), materialDatas.data(),
                                         size * sizeof(MaterialData), FrameResource::Type::MATERIAL);
    _frameResources[_currentFrameIndex]->CopyDescriptors(handles);
}

void RenderScene::InitializeRenderScene(UINT renderTargetCount)
{
    // Create Render Target pool and Render Target View
    _renderTargetPool.resize(renderTargetCount);
    _renderTargetHandles.resize(renderTargetCount);
    _renderTargetSRVHandles.resize(renderTargetCount);
    for (UINT i = 0; i < renderTargetCount; ++i)
    {
        _renderTargetPool[i] = std::make_shared<RenderTarget>();
    }
    // 화면 크기만한 quad만들기. NDC 좌표계로
    _frameQuad->Initialize(-1.f, 1.f, 2.f, 2.f, 0.f);
    _frameShader->BeginBuild();
    _frameShader->LoadShader(L"../Shaders/vs_quad.hlsl", Shader::Type::VS);
    _frameShader->LoadShader(L"../Shaders/ps_quad_frame.hlsl", Shader::Type::PS);
    _frameShader->EndBuild();
    CreatePso();
    CreateDescriptorHeap();

    for (UINT i = 0; i < renderTargetCount; ++i)
    {
        _renderTargetPool[i]->Initialize();
    }
    for (UINT i = 0; i < renderTargetCount; ++i)
    {
        _renderTargetHandles[i] = _renderTargetPool[i]->GetHandle();
    }
    // Create Depth Stecil Buffer and Depth Stencil View
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
                                  .SampleDesc{.Count   = UmDevice.GetMsaaState() ? (UINT)4 : (UINT)1,
                                              .Quality = UmDevice.GetMsaaState() ? (UmDevice.GetMsaaQuality() - 1) : 0},
                                  .Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN,
                                  .Flags  = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL};

    D3D12_CLEAR_VALUE optClear{.Format = UmDevice.GetDepthStencilFormat(), .DepthStencil{.Depth = 1.f, .Stencil = 0}};
    CD3DX12_HEAP_PROPERTIES property(D3D12_HEAP_TYPE_DEFAULT);
    hr = UmDevice.GetDevice()->CreateCommittedResource(&property, D3D12_HEAP_FLAG_NONE, &depthDesc,
                                                       D3D12_RESOURCE_STATE_PRESENT, &optClear,
                                                       IID_PPV_ARGS(_depthStencilBuffer.GetAddressOf()));

    D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{.Format        = UmDevice.GetDepthStencilFormat(),
                                          .ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D,
                                          .Flags         = D3D12_DSV_FLAG_NONE};

    UmDevice.GetDevice()->CreateDepthStencilView(_depthStencilBuffer.Get(), &dsvDesc, _depthStencilHandle);

    // Srv 생성하기(RenderTarget에 대한)
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format                          = DXGI_FORMAT_R32G32B32A32_FLOAT;
    srvDesc.ViewDimension                   = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MostDetailedMip       = 0;
    srvDesc.Texture2D.MipLevels             = 1;
    srvDesc.Shader4ComponentMapping         = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

    for (UINT i = 0; i < renderTargetCount; ++i)
    {
        _renderTargetSRVHandles[i] = _renderTargetPool[i]->CreateShaderResourceView();
    }

    // Frame Resource만들기
    _frameResources.resize(SWAPCHAIN_BUFFER_COUNT);
    for (UINT i = 0; i < SWAPCHAIN_BUFFER_COUNT; ++i)
    {
        _frameResources[i] = std::make_shared<FrameResource>();
        // 임시
        _frameResources[i]->Initialize(100, 6);
    }

    // 임시 : 메인 카메라를 통해 Camera ConstantBuffer 만들기.
    CameraData cameraData{
        .View       = UmMainCamera.GetViewMatrix(),
        .Projection = UmMainCamera.GetProjectionMatrix(),
    };

    UmDevice.CreateConstantBuffer(&cameraData, sizeof(CameraData), _cameraBuffer);
}
void RenderScene::RegisterOnRenderQueue(MeshRenderer* renderable)
{
    _renderQueue.push_back(renderable);
}

void RenderScene::AddRenderTechnique(const std::string& name, std::shared_ptr<RenderTechnique> technique)
{
    technique->SetOwnerScene(this);
    technique->Initalize();
    _techniques[name] = technique;
}

void RenderScene::Excute(ComPtr<ID3D12GraphicsCommandList> commandList)
{

    for (auto& [name, tech] : _techniques)
    {
        tech->Execute(commandList);
    }


    RenderOnBackBuffer(commandList);
}

void RenderScene::RenderOnBackBuffer(ComPtr<ID3D12GraphicsCommandList> commandList)
{
    UmDevice.SetBackBuffer();
    commandList->SetPipelineState(_framePSO.Get());
    commandList->SetGraphicsRootSignature(_frameShader->GetRootSignature().Get());
    auto dest = _srvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();

    D3D12_CPU_DESCRIPTOR_HANDLE src = _renderTargetSRVHandles[0];

    UmDevice.GetDevice()->CopyDescriptorsSimple(2, dest, src, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

    ID3D12DescriptorHeap* dh[] = {_srvDescriptorHeap.Get()};
    commandList->SetDescriptorHeaps(_countof(dh), dh);
    auto srv = _srvDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
    commandList->SetGraphicsRootDescriptorTable(_frameShader->GetRootSignatureIndex("screen"), srv);
    _frameQuad->Render(commandList.Get());
}

void RenderScene::CreatePso()
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

void RenderScene::CreateDescriptorHeap()
{
    D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};

    srvHeapDesc.NumDescriptors = 2;
    srvHeapDesc.Type           = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    srvHeapDesc.Flags          = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    HRESULT hr                 = S_OK;
    hr = UmDevice.GetDevice()->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(_srvDescriptorHeap.GetAddressOf()));
    FAILED_CHECK_BREAK(hr);
}
