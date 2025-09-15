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

    auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(_staticShadowMap.Get(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_DEPTH_WRITE);
    commandList->ResourceBarrier(1, &barrier);
}

void ShadowMapPass::AddRenderPassDatas(std::string_view sceneName)
{
    auto device = Global::device->GetDevice();   
    
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
    for (int i = 0; i < MAX_CASCADES; i++)
    {
        Global::viewManager->AddDescriptorHeap(ViewManager::Type::SHADER_RESOURCE, _debugHandles[i]);

        srvDesc.Format                         = DXGI_FORMAT_R32_FLOAT;
        srvDesc.ViewDimension                  = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
        srvDesc.Shader4ComponentMapping        = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        srvDesc.Texture2DArray.FirstArraySlice = i;
        srvDesc.Texture2DArray.MipLevels       = 1;
        srvDesc.Texture2DArray.ArraySize       = 1;

        device->CreateShaderResourceView(_shadowMap.Get(), &srvDesc, _debugHandles[i].CPU);
        Global::renderPassDatas->AddRenderPassImage(sceneName, "ShadowMapPass", "ShadowMap", _debugHandles[i].GPU);

        Global::renderPassDatas->AddRenderPassProperty(sceneName, "ShadowMapPass", ShadowPassProperty({0.1f, 200.f, 0.75f}));
    }
}

void ShadowMapPass::Update(ID3D12GraphicsCommandList* commandList, const float deltaTime)
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

    for (auto& data : _renderDatas)
    {
        data.clear();
    }

    MeshType meshType = END;
    const auto& shadowMapProps = std::any_cast<ShadowPassProperty>(_ownerScene->GetRenderPassProperty("ShadowMapPass"));
    if (_isDirtyFlag || !XMVector3Equal(lightDirection, _prevLightDirection) || shadowMapProps != _previousShadowPassProperty)
    {
        UpdateCascades(lightDirection);

        for (auto& meshInfo : _ownerScene->_activeMeshes[STATIC_MESH])
        {
            // cull_back, cull_front, cull_none
            meshType = MeshType(STATIC_MESH * 3 + (int)meshInfo.Material.CullMode);
            _renderDatas[meshType].emplace_back(meshInfo.Mesh, meshInfo.InstanceID, meshInfo.CustomDepth);
        }

        _isDirtyFlag = true;
    }

    for (auto& meshInfo : _ownerScene->_activeMeshes[SKELETAL_MESH])
    {
        // cull_back, cull_front, cull_none
        meshType = MeshType(SKELETAL_MESH * 3 + (int)meshInfo.Material.CullMode);
        _renderDatas[meshType].emplace_back(meshInfo.Mesh, meshInfo.InstanceID, meshInfo.CustomDepth);
    }
}

void ShadowMapPass::Begin(ID3D12GraphicsCommandList* commandList)
{
    commandList->RSSetViewports(1, &_viewport);
    commandList->RSSetScissorRects(1, &_scissorRect);
}

void ShadowMapPass::Draw(ID3D12GraphicsCommandList* commandList)
{
    UINT  currentBackBufferIndex = Global::device->GetCurrentBackBufferIndex();
    auto  resource               = Global::viewManager->GetShaderResourceHeap()->GetGPUDescriptorHandleForHeapStart();
    auto  cascadeData            = _cascadeDataCBV->GetGPUVirtualAddress();
    auto& frameResource          = _ownerScene->_frameResources[currentBackBufferIndex];

    if (_isDirtyFlag)
    {        
        for (int i = 0; i < MAX_CASCADES; i++)
        {
            commandList->OMSetRenderTargets(0, nullptr, FALSE, &_staticShadowMapDSVs[i]);
            commandList->ClearDepthStencilView(_staticShadowMapDSVs[i], D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

            // Static
            commandList->SetGraphicsRootSignature(_fxStaticShadow.GetRootSignature());
            commandList->SetGraphicsRootDescriptorTable(_fxStaticShadow.GetRootParameterIndex("textures"), resource);
            commandList->SetGraphicsRootConstantBufferView(_fxStaticShadow.GetRootParameterIndex("cascadeData"), cascadeData);
            frameResource->SetFrameResource(FrameResourceType::TRANSFORM, _fxStaticShadow.GetRootParameterIndex("matrices"), commandList);
            frameResource->SetFrameResource(FrameResourceType::MATERIAL, _fxStaticShadow.GetRootParameterIndex("material"), commandList);

            commandList->SetPipelineState(_psos[STATIC_CULL_BACK].Get());
            DrawMeshes(commandList, STATIC_MESH, STATIC_CULL_BACK, i);

            commandList->SetPipelineState(_psos[STATIC_CULL_FRONT].Get());
            DrawMeshes(commandList, STATIC_MESH, STATIC_CULL_FRONT, i);

            commandList->SetPipelineState(_psos[STATIC_TWO_SIDED].Get());
            DrawMeshes(commandList, STATIC_MESH, STATIC_TWO_SIDED, i);
        }

        _isDirtyFlag = false;
    }

    // Copy previous cascade data
    CopyPreviousCascadeData(commandList);

    for (int i = 0; i < MAX_CASCADES; i++)
    {
        commandList->OMSetRenderTargets(0, nullptr, FALSE, &_shadowMapDSVs[i]);
        //commandList->ClearDepthStencilView(_shadowMapDSVs[i], D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

        // Skeletal
        commandList->SetGraphicsRootSignature(_fxSkeletalShadow.GetRootSignature());
        commandList->SetGraphicsRootDescriptorTable(_fxSkeletalShadow.GetRootParameterIndex("textures"), resource);
        commandList->SetGraphicsRootConstantBufferView(_fxSkeletalShadow.GetRootParameterIndex("cascadeData"), cascadeData);
        frameResource->SetFrameResource(FrameResourceType::TRANSFORM, _fxSkeletalShadow.GetRootParameterIndex("matrices"), commandList);
        frameResource->SetFrameResource(FrameResourceType::BONE_MATRICES, _fxSkeletalShadow.GetRootParameterIndex("boneMatrices"), commandList);
        frameResource->SetFrameResource(FrameResourceType::MATERIAL, _fxSkeletalShadow.GetRootParameterIndex("material"), commandList);

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

    if (_ownerScene->IsDirtyFlag())
    {
        _isDirtyFlag = true;
    }
}

void ShadowMapPass::CreateShadowMapResource()
{
    _cascadeDataCBV = std::make_unique<ConstantBufferView>();
    _cascadeDataCBV->Initialize(sizeof(CascadeData));

    for (int i = 0; i < MAX_CASCADES; i++)
    {
        Global::viewManager->AddDescriptorHeap(ViewManager::Type::DEPTH_STENCIL, _shadowMapDSVs[i]);
        Global::viewManager->AddDescriptorHeap(ViewManager::Type::DEPTH_STENCIL, _staticShadowMapDSVs[i]);
    }

    Global::viewManager->AddDescriptorHeap(ViewManager::Type::SHADER_RESOURCE, _shadowMapSRV);

    auto device = Global::device->GetDevice();

    D3D12_CLEAR_VALUE optClear{.Format = DXGI_FORMAT_D32_FLOAT, .DepthStencil = {.Depth = 1.f, .Stencil = 0}};

    // Create Shadow Map Resource
    auto desc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R32_TYPELESS, _shadowMapSize, _shadowMapSize, MAX_CASCADES, 1, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);
    auto heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
    HRESULT hr             = device->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &desc,
                                                             D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, &optClear,
                                                             IID_PPV_ARGS(&_shadowMap));
    FAILED_CHECK_MESSAGE(hr, L"ShadowMapPass::CreateShadowMapResource device->CreateCommittedResource Failed");

    hr = device->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &desc,
                                         D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, &optClear,
                                         IID_PPV_ARGS(&_staticShadowMap));
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
        device->CreateDepthStencilView(_staticShadowMap.Get(), &dsvDesc, _staticShadowMapDSVs[i]);
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
    _psos.resize(MeshType::END);

    PipelineStateStream pss{};
    pss.BlendState                               = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    pss.DepthStencilState                        = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
    pss.RasterizerState                          = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    (&pss.RasterizerState)->DepthBias            = 100;
    (&pss.RasterizerState)->DepthBiasClamp       = 0.0f;
    (&pss.RasterizerState)->SlopeScaledDepthBias = 1.5f;
    pss.PrimitiveTopology                        = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    pss.DSVFormat                                = DXGI_FORMAT_D32_FLOAT;

    _fxStaticShadow.SetPipelineStateStream(pss);

    (&pss.RasterizerState)->CullMode = D3D12_CULL_MODE_BACK;
    _psos[STATIC_CULL_BACK]          = Global::pipelineStateManager->GetPipelineState(pss);

    (&pss.RasterizerState)->CullMode = D3D12_CULL_MODE_FRONT;
    _psos[STATIC_CULL_FRONT]         = Global::pipelineStateManager->GetPipelineState(pss);

    (&pss.RasterizerState)->CullMode = D3D12_CULL_MODE_NONE;
    _psos[STATIC_TWO_SIDED]          = Global::pipelineStateManager->GetPipelineState(pss);

    _fxSkeletalShadow.SetPipelineStateStream(pss);

    (&pss.RasterizerState)->CullMode = D3D12_CULL_MODE_BACK;
    _psos[SKELETAL_CULL_BACK]        = Global::pipelineStateManager->GetPipelineState(pss);

    (&pss.RasterizerState)->CullMode = D3D12_CULL_MODE_FRONT;
    _psos[SKELETAL_CULL_FRONT]       = Global::pipelineStateManager->GetPipelineState(pss);

    (&pss.RasterizerState)->CullMode = D3D12_CULL_MODE_NONE;
    _psos[SKELETAL_TWO_SIDED]        = Global::pipelineStateManager->GetPipelineState(pss);
}

void ShadowMapPass::UpdateCascades(const Vector3& lightDirection)
{
    if (XMVector3Equal(lightDirection, Vector3::Zero))
        return;    

    XMVECTOR lightDir = lightDirection;

    auto& camera = _ownerScene->_camera;

    const auto& shadowMapProps = std::any_cast<ShadowPassProperty>(_ownerScene->GetRenderPassProperty("ShadowMapPass"));

    // 1. 캐스케이드 분할 거리 계산
    float nearZ  = std::max(0.01f, shadowMapProps.NearPlane);
    float farZ   = std::max(nearZ + 1, shadowMapProps.FarPlane);
    float lambda = shadowMapProps.SplitFactor;

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
        BoundingFrustum worldFrustum = camera->GetFrustum();
        worldFrustum.Near            = c == 0 ? nearZ : _cascadeData.CascadeSplits[c - 1];
        worldFrustum.Far             = _cascadeData.CascadeSplits[c];
        
        // 2. 절두체 코너들을 월드 좌표로 변환 → AABB 구함
        XMFLOAT3 corners[8];
        worldFrustum.GetCorners(corners);
        XMVECTOR frustumCenter = XMVectorZero();

        for (auto& corner : corners)
        {
            frustumCenter += XMLoadFloat3(&corner);
        }
        frustumCenter /= 8.0f;

        float radius = 0.f;
        for (auto& corner : corners)
        {
            radius = std::max(radius, XMVectorGetX(XMVector3Length(XMLoadFloat3(&corner) - frustumCenter)));
        }
        // 텍셀 스냅을 위해 울림
        radius = std::ceil(radius * 16.0f) / 16.0f;

        // 3. 라이트 뷰
        XMVECTOR eye    = frustumCenter - lightDir * (radius + shadowMapProps.Offset1);
        XMVECTOR target = frustumCenter;
        XMVECTOR up     = XMVectorSet(0, 1, 0, 0);

        if (std::fabs(XMVectorGetX(XMVector3Dot(up, lightDir))) > 0.99f)
            up = XMVectorSet(1, 0, 0, 0);

        XMMATRIX lightView = XMMatrixLookAtLH(eye, target, up);

        // 4. 라이트 공간 Orthographic Projection
        XMMATRIX lightProj = XMMatrixOrthographicLH(radius * 2, radius * 2, 0.0f, radius * 2 + shadowMapProps.Offset2);
        XMStoreFloat4x4(&_cascadeData.ShadowVP[c], XMMatrixTranspose(lightView * lightProj));
    }

    // 계산된 모든 캐스케이드 데이터를 상수 버퍼에 한 번에 업데이트
    _cascadeDataCBV->UpdateBuffer(&_cascadeData);

    _prevLightDirection         = lightDirection;
    _previousShadowPassProperty = shadowMapProps;
}

void ShadowMapPass::DrawMeshes(ID3D12GraphicsCommandList* commandList, int shaderType, MeshType meshType, int cascadeIndex)
{
    UINT parameter[4]{0, MAX_BONE_MATRIX, 0, 0};

    const auto& parallaxMappingProperty = std::any_cast<const GbufferProperty&>(_ownerScene->GetRenderPassProperty("G-BufferPass"));

    switch (shaderType)
    {
    case STATIC_MESH:
        commandList->SetGraphicsRoot32BitConstants(_fxStaticShadow.GetRootParameterIndex("bit32_1_mipBias"), 1, &parallaxMappingProperty.MipBias, 0);
        break;

    case SKELETAL_MESH:
        commandList->SetGraphicsRoot32BitConstants(_fxSkeletalShadow.GetRootParameterIndex("bit32_1_mipBias"), 1, &parallaxMappingProperty.MipBias, 0);
        break;
    }

    for (auto& [mesh, instanceID, customDepth] : _renderDatas[meshType])
    {
        parameter[0] = instanceID;
        parameter[2] = customDepth;
        parameter[3] = cascadeIndex;

        switch (shaderType)
        {
        case STATIC_MESH:
            commandList->SetGraphicsRoot32BitConstants(_fxStaticShadow.GetRootParameterIndex("bit32_4_objectData2"), 4, parameter, 0);
            break;

        case SKELETAL_MESH:
            commandList->SetGraphicsRoot32BitConstants(_fxSkeletalShadow.GetRootParameterIndex("bit32_4_objectData2"), 4, parameter, 0);
            break;
        }

        mesh->Render(commandList);
    }
}

void ShadowMapPass::CopyPreviousCascadeData(ID3D12GraphicsCommandList* commandList)
{
    D3D12_RESOURCE_BARRIER barriers[2];
    barriers[0] = CD3DX12_RESOURCE_BARRIER::Transition(_staticShadowMap.Get(), D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_COPY_SOURCE);
    barriers[1] = CD3DX12_RESOURCE_BARRIER::Transition(_shadowMap.Get(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_COPY_DEST);

    commandList->ResourceBarrier(2, barriers);
    commandList->CopyResource(_shadowMap.Get(), _staticShadowMap.Get());

    barriers[0] = CD3DX12_RESOURCE_BARRIER::Transition(_staticShadowMap.Get(), D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_DEPTH_WRITE);
    barriers[1] = CD3DX12_RESOURCE_BARRIER::Transition(_shadowMap.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_DEPTH_WRITE);

    commandList->ResourceBarrier(2, barriers);
}