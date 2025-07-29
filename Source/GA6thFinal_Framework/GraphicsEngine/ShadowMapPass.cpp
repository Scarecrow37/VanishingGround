#include "pch.h"
#include "ShadowMapPass.h"
#include "FrameResource.h"

ShadowMapPass::ShadowMapPass() = default;

ShadowMapPass::~ShadowMapPass() = default;

void ShadowMapPass::Initialize(RenderScene* ownerScene, RenderTechnique* ownerTechnique, ID3D12GraphicsCommandList* commandList)
{
    __super::Initialize(ownerScene, ownerTechnique, commandList);

    CreateShadowMapResource();
    CreateShaderAndPSO();
}

void ShadowMapPass::Update(ID3D12GraphicsCommandList* commandList)
{
    int   mainLight    = 0;
    float maxIntensity = 0.0f;

    for (int i = 0; i < MAX_DIRECTIONAL_LIGHT; i++)
    {
        if (maxIntensity < _ownerScene->_lightDatas[i].Intensity)
        {
            maxIntensity = _ownerScene->_lightDatas[i].Intensity;
            mainLight    = i;
        }
    }

    Camera* mainCamera     = _ownerScene->_camera.get();
    Vector3 lightDirection = (_ownerScene->_lightDatas[mainLight].float3_1);
    lightDirection.Normalize();

    UpdateCascades(lightDirection);

    for (auto& data : _renderDatas)
    {
        data.clear();
    }

    MeshType meshType = END;
    for (int i = 0; i < MESH_TYPE_END; i++)
    {
        for (auto& [material, mesh, customDepth, instanceID] : _ownerScene->_activeMeshes[i])
        {
            /*const auto& cameraFrustum = _ownerScene->_camera->GetWorldFrustum();

            BoundingOrientedBox boundingOrientedBox;
            const auto&         meshBoundingBox = mesh->GetBoundingBox();
            meshBoundingBox.Transform(boundingOrientedBox, XMMatrixTranspose(_ownerScene->_worldMatrices[instanceID]));

            if (!cameraFrustum.Intersects(boundingOrientedBox))
            {
                continue;
            }*/

            // cull_back, cull_front, cull_none
            meshType = MeshType(i * 3 + (int)material.CullMode);
            _renderDatas[meshType].emplace_back(mesh, instanceID, customDepth);
        }
    }
}

void ShadowMapPass::Begin(ID3D12GraphicsCommandList* commandList)
{
    commandList->RSSetViewports(1, &_viewport);
    commandList->RSSetScissorRects(1, &_scissorRect);

    auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(_shadowMap.Get(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_DEPTH_WRITE);
    commandList->ResourceBarrier(1, &barrier);
}

void ShadowMapPass::Draw(ID3D12GraphicsCommandList* commandList)
{
    UINT  currentBackBufferIndex = Global::device->GetCurrentBackBufferIndex();
    auto  resource               = Global::viewManager->GetShaderResourceHeap()->GetGPUDescriptorHandleForHeapStart();
    auto  cascadeData            = _cascadeDataCBV->GetGPUVirtualAddress();
    auto& frameResource          = _ownerScene->_frameResources[currentBackBufferIndex];

    for (int i = 0; i < MAX_CASCADES; i++)
    {
        commandList->OMSetRenderTargets(0, nullptr, FALSE, &_shadowMapDSVs[i]);
        commandList->ClearDepthStencilView(_shadowMapDSVs[i], D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

        // Static
        commandList->SetGraphicsRootSignature(_shaders[STATIC_MESH]->GetRootSignature());
        commandList->SetGraphicsRootConstantBufferView(_shaders[STATIC_MESH]->GetRootParameterIndex("cascadeData"), cascadeData);
        frameResource->SetFrameResource(FrameResourceType::TRANSFORM, _shaders[STATIC_MESH]->GetRootParameterIndex("worldMatrices"), commandList);

        commandList->SetPipelineState(_psos[STATIC_CULL_BACK].Get());
        DrawMeshes(commandList, STATIC_MESH, STATIC_CULL_BACK, i);

        commandList->SetPipelineState(_psos[STATIC_CULL_FRONT].Get());
        DrawMeshes(commandList, STATIC_MESH, STATIC_CULL_FRONT, i);

        commandList->SetPipelineState(_psos[STATIC_TWO_SIDED].Get());
        DrawMeshes(commandList, STATIC_MESH, STATIC_TWO_SIDED, i);

        // Skeletal
        commandList->SetGraphicsRootSignature(_shaders[SKELETAL_MESH]->GetRootSignature());
        commandList->SetGraphicsRootConstantBufferView(_shaders[SKELETAL_MESH]->GetRootParameterIndex("cascadeData"), cascadeData);
        frameResource->SetFrameResource(FrameResourceType::TRANSFORM, _shaders[SKELETAL_MESH]->GetRootParameterIndex("worldMatrices"), commandList);
        frameResource->SetFrameResource(FrameResourceType::BONE_MATRICES, _shaders[SKELETAL_MESH]->GetRootParameterIndex("boneMatrices"), commandList);

        commandList->SetPipelineState(_psos[SKELETAL_CULL_BACK].Get());
        DrawMeshes(commandList, SKELETAL_MESH, SKELETAL_CULL_BACK, i);

        commandList->SetPipelineState(_psos[SKELETAL_CULL_FRONT].Get());
        DrawMeshes(commandList, SKELETAL_MESH, SKELETAL_CULL_FRONT, i);

        commandList->SetPipelineState(_psos[SKELETAL_TWO_SIDED].Get());
        DrawMeshes(commandList, SKELETAL_MESH, SKELETAL_TWO_SIDED, i);
    }
}

void ShadowMapPass::End(ID3D12GraphicsCommandList* commandList)
{
    auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(_shadowMap.Get(), D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    commandList->ResourceBarrier(1, &barrier);
}

void ShadowMapPass::CreateShadowMapResource()
{
    _cascadeDataCBV = std::make_unique<ConstantBufferView>();
    _cascadeDataCBV->Initialize(sizeof(CascadeData));

    for (auto& shadowMapDSV : _shadowMapDSVs)
    {
        Global::viewManager->AddDescriptorHeap(ViewManager::Type::DEPTH_STENCIL, shadowMapDSV);
    }

    Global::viewManager->AddDescriptorHeap(ViewManager::Type::SHADER_RESOURCE, _shadowMapSRV);

    auto device = Global::device->GetDevice();

    D3D12_CLEAR_VALUE optClear{.Format = DXGI_FORMAT_D32_FLOAT, .DepthStencil = {.Depth = 1.f, .Stencil = 0}};

    // Create Shadow Map Resource
    auto desc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R32_TYPELESS, _shadowMapSize, _shadowMapSize, MAX_CASCADES, 1, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);
    auto heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
    HRESULT hr = device->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, &optClear, IID_PPV_ARGS(&_shadowMap));
    FAILED_CHECK_MESSAGE(hr, L"ShadowMapPass::CreateShadowMapResource device->CreateCommittedResource Failed");

    // Create DSV for Shadow Map
    D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
    dsvDesc.Format        = DXGI_FORMAT_D32_FLOAT;
    dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DARRAY;

    for (int i = 0; i < MAX_CASCADES; i++)
    {
        dsvDesc.Texture2DArray.FirstArraySlice = i;
        dsvDesc.Texture2DArray.ArraySize       = 1;
        device->CreateDepthStencilView(_shadowMap.Get(), &dsvDesc, _shadowMapDSVs[i]);
    }

    // Create SRV for Shadow Map
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
    srvDesc.Format                   = DXGI_FORMAT_R32_FLOAT;
    srvDesc.ViewDimension            = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
    srvDesc.Shader4ComponentMapping  = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Texture2DArray.MipLevels = 1;
    srvDesc.Texture2DArray.ArraySize = MAX_CASCADES;
    device->CreateShaderResourceView(_shadowMap.Get(), &srvDesc, _shadowMapSRV.CPU);

    _shadowMap->SetName(L"ShadowMapResource");

    _viewport.TopLeftX = 0.0f;
    _viewport.TopLeftY = 0.0f;
    _viewport.Width    = static_cast<float>(_shadowMapSize);
    _viewport.Height   = static_cast<float>(_shadowMapSize);
    _viewport.MinDepth = 0.0f;
    _viewport.MaxDepth = 1.0f;

    _scissorRect.left  = 0;
    _scissorRect.top   = 0;
    _scissorRect.right = _shadowMapSize;
    _scissorRect.bottom = _shadowMapSize;
}

void ShadowMapPass::CreateShaderAndPSO()
{
    _shaders.resize(MESH_TYPE_END);
    _psos.resize(MeshType::END);

    std::unique_ptr<ShaderBuilder> staticMeshShaderBuilder = std::make_unique<ShaderBuilder>();
    staticMeshShaderBuilder->BeginBuild();
    staticMeshShaderBuilder->SetShader(L"../Shaders/vs_static_shadow_fr.hlsl", ShaderBuilder::Type::VS);
    staticMeshShaderBuilder->EndBuild();
    _shaders[STATIC_MESH] = std::move(staticMeshShaderBuilder);

    std::unique_ptr<ShaderBuilder> skeletalMeshShaderBuilder = std::make_unique<ShaderBuilder>();
    skeletalMeshShaderBuilder->BeginBuild();
    skeletalMeshShaderBuilder->SetShader(L"../Shaders/vs_skeletal_shadow_fr.hlsl", ShaderBuilder::Type::VS);
    skeletalMeshShaderBuilder->EndBuild();
    _shaders[SKELETAL_MESH] = std::move(skeletalMeshShaderBuilder);

    ID3D12Device*                      device = Global::device->GetDevice();
    D3D12_GRAPHICS_PIPELINE_STATE_DESC psodesc{};
    HRESULT                            hr = S_OK;
    ComPtr<ID3D12PipelineState>        pipelineState;

    psodesc.RasterizerState                      = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    psodesc.RasterizerState.DepthBias            = 10000;
    psodesc.RasterizerState.DepthBiasClamp       = 0.0f;
    psodesc.RasterizerState.SlopeScaledDepthBias = 1.5f;
    psodesc.BlendState                           = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    psodesc.DepthStencilState                    = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
    psodesc.SampleMask                           = UINT_MAX;
    psodesc.PrimitiveTopologyType                = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    psodesc.NumRenderTargets                     = 0;
    psodesc.DSVFormat                            = DXGI_FORMAT_D32_FLOAT;
    psodesc.SampleDesc                           = {1, 0};
    psodesc.InputLayout                          = _shaders[STATIC_MESH]->GetInputLayout();
    psodesc.pRootSignature                       = _shaders[STATIC_MESH]->GetRootSignature();
    psodesc.VS                                   = _shaders[STATIC_MESH]->GetShaderByteCode(ShaderBuilder::Type::VS);

    // static one side back.
    psodesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
    hr                               = device->CreateGraphicsPipelineState(&psodesc, IID_PPV_ARGS(&pipelineState));
    FAILED_CHECK_MESSAGE(hr, L"GBufferPass::InitShaderAndPSO device->CreateGraphicsPipelineState Failed");
    _psos[STATIC_CULL_BACK] = pipelineState;

    // static one side front.
    psodesc.RasterizerState.CullMode = D3D12_CULL_MODE_FRONT;
    hr                               = device->CreateGraphicsPipelineState(&psodesc, IID_PPV_ARGS(&pipelineState));
    FAILED_CHECK_MESSAGE(hr, L"GBufferPass::InitShaderAndPSO device->CreateGraphicsPipelineState Failed");
    _psos[STATIC_CULL_FRONT] = pipelineState;

    // static two side.
    psodesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
    hr                               = device->CreateGraphicsPipelineState(&psodesc, IID_PPV_ARGS(&pipelineState));
    FAILED_CHECK_MESSAGE(hr, L"GBufferPass::InitShaderAndPSO device->CreateGraphicsPipelineState Failed");
    _psos[STATIC_TWO_SIDED] = pipelineState;

    // Skeletal Mesh PSO
    psodesc.InputLayout    = _shaders[SKELETAL_MESH]->GetInputLayout();
    psodesc.pRootSignature = _shaders[SKELETAL_MESH]->GetRootSignature();
    psodesc.VS             = _shaders[SKELETAL_MESH]->GetShaderByteCode(ShaderBuilder::Type::VS);

    // skeletal one side back.
    psodesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
    hr                               = device->CreateGraphicsPipelineState(&psodesc, IID_PPV_ARGS(&pipelineState));
    FAILED_CHECK_MESSAGE(hr, L"GBufferPass::InitShaderAndPSO device->CreateGraphicsPipelineState Failed");
    _psos[SKELETAL_CULL_BACK] = pipelineState;

    // skeletal one side front.
    psodesc.RasterizerState.CullMode = D3D12_CULL_MODE_FRONT;
    hr                               = device->CreateGraphicsPipelineState(&psodesc, IID_PPV_ARGS(&pipelineState));
    FAILED_CHECK_MESSAGE(hr, L"GBufferPass::InitShaderAndPSO device->CreateGraphicsPipelineState Failed");
    _psos[SKELETAL_CULL_FRONT] = pipelineState;

    // skeletal two side.
    psodesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
    hr                               = device->CreateGraphicsPipelineState(&psodesc, IID_PPV_ARGS(&pipelineState));
    FAILED_CHECK_MESSAGE(hr, L"GBufferPass::InitShaderAndPSO device->CreateGraphicsPipelineState Failed");
    _psos[SKELETAL_TWO_SIDED] = pipelineState;
}

void ShadowMapPass::UpdateCascades(const Vector3& lightDirection)
{
    if (XMVector3Equal(lightDirection, Vector3::Zero))
        return;

    XMVECTOR lightDir = lightDirection;

    auto& camera = _ownerScene->_camera;

    // 1. 캐스케이드 분할 거리 계산
    float nearZ  = camera->GetNearZ();
    float farZ   = camera->GetFarZ();
    float lambda = 0.75f;

    for (int i = 0; i < MAX_CASCADES; i++)
    {
        float ratio         = (float)(i + 1) / MAX_CASCADES;
        float split_log     = nearZ * std::powf(farZ / nearZ, ratio);
        float split_uniform = nearZ + (farZ - nearZ) * ratio;

        _cascadeData.CascadeSplits[i] = lambda * split_log + (1.0f - lambda) * split_uniform;
    }

    for (uint32_t c = 0; c < MAX_CASCADES; ++c)
    {
        // 1. 해당 스플릿 z 범위로 카메라 뷰 절두체 8 코너 산출
        BoundingFrustum splitFrust = camera->GetSplitFrustum(c == 0 ? nearZ : _cascadeData.CascadeSplits[c - 1], _cascadeData.CascadeSplits[c]);

        // 2. 절두체 코너들을 월드 좌표로 변환 → AABB 구함
        XMFLOAT3 corners[8];
        splitFrust.GetCorners(corners);
        XMVECTOR frustumCenter = XMVectorZero();

        for (int i = 0; i < 8; ++i)
        {
            frustumCenter += XMLoadFloat3(&corners[i]);
        }
        frustumCenter /= 8.0f;

        float radius = 0.f;
        for (int i = 0; i < 8; ++i)
        {
            radius = std::max(radius, XMVectorGetX(XMVector3Length(XMLoadFloat3(&corners[i]) - frustumCenter)));
        }
        // 텍셀 스냅을 위해 울림
        radius = std::ceil(radius * 16.0f) / 16.0f;

        // 3. 라이트 뷰
        XMVECTOR eye    = frustumCenter - lightDir * (radius + 100.0f);
        XMVECTOR target = frustumCenter;
        XMVECTOR up     = XMVectorSet(0, 1, 0, 0);

        if (std::fabs(XMVectorGetX(XMVector3Dot(up, lightDir))) > 0.99f)
            up = XMVectorSet(1, 0, 0, 0);

        XMMATRIX lightView = XMMatrixLookAtLH(eye, target, up);

        // 4. 라이트 공간 Orthographic Projection
        XMMATRIX lightProj = XMMatrixOrthographicLH(radius * 2, radius * 2, 0.0f, radius * 2 + 200.0f);

        XMStoreFloat4x4(&_cascadeData.ShadowVP[c], XMMatrixTranspose(lightView * lightProj));
    }

    // 계산된 모든 캐스케이드 데이터를 상수 버퍼에 한 번에 업데이트
    _cascadeDataCBV->UpdateBuffer(&_cascadeData);
}

void ShadowMapPass::DrawMeshes(ID3D12GraphicsCommandList* commandList, int shaderType, MeshType meshType, int cascadeIndex)
{
    UINT parameter[4]{0, MAX_BONE_MATRIX, 0, 0};
    for (auto& [mesh, instanceID, customDepth] : _renderDatas[meshType])
    {
        parameter[0] = instanceID;
        parameter[2] = customDepth;
        parameter[3] = cascadeIndex;

        commandList->SetGraphicsRoot32BitConstants(_shaders[shaderType]->GetRootParameterIndex("bit32_4_objectData2"), 4, parameter, 0);
        mesh->Render(commandList);
    }
}