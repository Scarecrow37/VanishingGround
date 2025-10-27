#include "pch.h"
#include "ShadowMapPass.h"
#include "FrameResource.h"

ShadowMapPass::ShadowMapPass() = default;

ShadowMapPass::~ShadowMapPass() = default;

void ShadowMapPass::Initialize(RenderScene* ownerScene, RenderTechnique* ownerTechnique, ID3D12GraphicsCommandList* commandList)
{
    RenderPass::Initialize(ownerScene, ownerTechnique, commandList);

    CreateShadowMapResource();
    CreateShaderAndPSO();

    _instanceDatasBuffer = std::make_unique<StructuredBuffer>();
    _instanceDatasBuffer->Initialize(sizeof(InstanceData), MAX_OBJECTS);
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

        Global::renderPassDatas->AddRenderPassProperty("ShadowMapPass", ShadowPassProperty({0.01f, 100.f, 0.75f}));
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

    for (auto& type : _meshInfos)
    {
        for (auto& meshInfos : type)
        {
            meshInfos.clear();
        }
    }

    UpdateCascades(lightDirection);

    for (int i = 0; i < MESH_TYPE_END; i++)
    {
        for (auto& meshInfo : _ownerScene->_activeMeshes[i])
        {
            int cullMode = (int)meshInfo.Material.CullMode;
            _meshInfos[i][cullMode].push_back(&meshInfo);
        }
    }

    _instanceDatas.clear();        
    for (int i = 0; i < CullMode::END; i++)
    {
        for (auto& meshInfo : _meshInfos[STATIC_MESH][i])
        {
            _instanceDatas.emplace_back(meshInfo->InstanceData);
        }
    }
    
    for (int i = 0; i < CullMode::END; i++)
    {
        for (auto& meshInfo : _meshInfos[SKELETAL_MESH][i])
        {
            _instanceDatas.emplace_back(meshInfo->InstanceData);
        }
    }

    _instanceDatasBuffer->CopyStructuredBuffer(commandList, _instanceDatas.data(), (UINT)_instanceDatas.size());
}

void ShadowMapPass::Begin(ID3D12GraphicsCommandList* commandList)
{
    auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(_shadowMap.Get(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_DEPTH_WRITE);
    commandList->ResourceBarrier(1, &barrier);
    
    commandList->ClearDepthStencilView(_shadowMapDSV, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

    commandList->OMSetRenderTargets(0, nullptr, FALSE, &_shadowMapDSV);
    commandList->RSSetViewports(1, &_viewport);
    commandList->RSSetScissorRects(1, &_scissorRect);
}

void ShadowMapPass::Draw(ID3D12GraphicsCommandList* commandList)
{
    UINT  currentBackBufferIndex = Global::device->GetCurrentBackBufferIndex();
    auto  resource               = Global::viewManager->GetShaderResourceHeap()->GetGPUDescriptorHandleForHeapStart();
    auto  cascadeData            = _cascadeDataCBV->GetGPUVirtualAddress();
    auto  instanceData           = _instanceDatasBuffer->GetGPUVirtualAddress();
    auto& frameResource          = _ownerScene->_frameResources[currentBackBufferIndex];
    
    UINT offset = 0;
    
    // Static
    commandList->SetGraphicsRootSignature(_fxStaticMesh.GetRootSignature());
    commandList->SetGraphicsRootDescriptorTable(_fxStaticMesh.GetRootParameterIndex("textures"), resource);
    commandList->SetGraphicsRootConstantBufferView(_fxStaticMesh.GetRootParameterIndex("cascadeData"), cascadeData);
    commandList->SetGraphicsRootShaderResourceView(_fxStaticMesh.GetRootParameterIndex("instanceData"), instanceData);
    frameResource->SetFrameResource(FrameResourceType::TRANSFORM, _fxStaticMesh.GetRootParameterIndex("matrices"), commandList);

    commandList->SetPipelineState(_psos[STATIC_MESH][CULL_BACK].Get());
    DrawMeshes(commandList, STATIC_MESH, CULL_BACK, offset);

    offset += (UINT)_meshInfos[STATIC_MESH][CULL_BACK].size();
    commandList->SetPipelineState(_psos[STATIC_MESH][CULL_FRONT].Get());
    DrawMeshes(commandList, STATIC_MESH, CULL_FRONT, offset);

    offset += (UINT)_meshInfos[STATIC_MESH][CULL_FRONT].size();
    commandList->SetPipelineState(_psos[STATIC_MESH][TWO_SIDED].Get());
    DrawMeshes(commandList, STATIC_MESH, TWO_SIDED, offset);

    // Skeletal
    commandList->SetGraphicsRootSignature(_fxSkeletalMesh.GetRootSignature());
    commandList->SetGraphicsRootDescriptorTable(_fxSkeletalMesh.GetRootParameterIndex("textures"), resource);
    commandList->SetGraphicsRootConstantBufferView(_fxSkeletalMesh.GetRootParameterIndex("cascadeData"), cascadeData);
    commandList->SetGraphicsRootShaderResourceView(_fxSkeletalMesh.GetRootParameterIndex("instanceData"), instanceData);
    frameResource->SetFrameResource(FrameResourceType::TRANSFORM, _fxSkeletalMesh.GetRootParameterIndex("matrices"), commandList);
    frameResource->SetFrameResource(FrameResourceType::BONE_MATRICES, _fxSkeletalMesh.GetRootParameterIndex("boneMatrices"), commandList);

    offset += (UINT)_meshInfos[STATIC_MESH][TWO_SIDED].size();
    commandList->SetPipelineState(_psos[SKELETAL_MESH][CULL_BACK].Get());
    DrawMeshes(commandList, SKELETAL_MESH, CULL_BACK, offset);

    offset += (UINT)_meshInfos[SKELETAL_MESH][CULL_BACK].size();
    commandList->SetPipelineState(_psos[SKELETAL_MESH][CULL_FRONT].Get());
    DrawMeshes(commandList, SKELETAL_MESH, CULL_FRONT, offset);

    offset += (UINT)_meshInfos[SKELETAL_MESH][CULL_FRONT].size();
    commandList->SetPipelineState(_psos[SKELETAL_MESH][TWO_SIDED].Get());
    DrawMeshes(commandList, SKELETAL_MESH, TWO_SIDED, offset);
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
    
    Global::viewManager->AddDescriptorHeap(ViewManager::Type::DEPTH_STENCIL, _shadowMapDSV);    
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

    // Create DSV for Shadow Map
    D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
    dsvDesc.Format                         = DXGI_FORMAT_D32_FLOAT;
    dsvDesc.ViewDimension                  = D3D12_DSV_DIMENSION_TEXTURE2DARRAY;
    dsvDesc.Texture2DArray.FirstArraySlice = 0;
    dsvDesc.Texture2DArray.ArraySize       = MAX_CASCADES;
    device->CreateDepthStencilView(_shadowMap.Get(), &dsvDesc, _shadowMapDSV);

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
    PipelineStateStream pss{};
    pss.BlendState                               = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    pss.DepthStencilState                        = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
    pss.RasterizerState                          = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    (&pss.RasterizerState)->DepthBias            = 5000;
    (&pss.RasterizerState)->DepthBiasClamp       = 0.01f;
    (&pss.RasterizerState)->SlopeScaledDepthBias = 1.5f;
    pss.PrimitiveTopology                        = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    pss.DSVFormat                                = DXGI_FORMAT_D32_FLOAT;

    auto CreatePipelineStateStream = [this, &pss](int meshType)
    {
        (&pss.RasterizerState)->CullMode      = D3D12_CULL_MODE_BACK;
        _psos[meshType][CULL_BACK] = Global::pipelineStateManager->GetPipelineState(pss);

        (&pss.RasterizerState)->CullMode       = D3D12_CULL_MODE_FRONT;
        _psos[meshType][CULL_FRONT] = Global::pipelineStateManager->GetPipelineState(pss);

        (&pss.RasterizerState)->CullMode      = D3D12_CULL_MODE_NONE;
        _psos[meshType][TWO_SIDED] = Global::pipelineStateManager->GetPipelineState(pss);
    };

    _fxStaticMesh.SetPipelineStateStream(pss);
    CreatePipelineStateStream(STATIC_MESH);

    _fxSkeletalMesh.SetPipelineStateStream(pss);
    CreatePipelineStateStream(SKELETAL_MESH);
}

void ShadowMapPass::UpdateCascades(const Vector3& lightDirection)
{
    if (XMVector3Equal(lightDirection, Vector3::Zero))
        return;    

    XMVECTOR lightDir = lightDirection;

    auto& camera = _ownerScene->_camera;    

    const auto& shadowMapProps = std::any_cast<const ShadowPassProperty&>(Global::renderPassDatas->GetRenderPassProperty("ShadowMapPass"));

    // 1. 캐스케이드 분할 거리 계산
    float nearZ  = std::max(0.01f, shadowMapProps.NearPlane);
    float farZ   = std::max(nearZ + 1, shadowMapProps.FarPlane);
    float lambda = shadowMapProps.SplitFactor;

    XMVECTOR L   = XMVector3Normalize(lightDir);
    XMVECTOR upY = XMVectorSet(0, 1, 0, 0);
    XMVECTOR upZ = XMVectorSet(0, 0, 1, 0);
    float    d   = fabsf(XMVectorGetX(XMVector3Dot(L, upY)));
    
    XMVECTOR U = (d > 0.95f) ? upZ : upY;
    XMVECTOR R = XMVector3Normalize(XMVector3Cross(U, L));
    U          = XMVector3Cross(L, R);

    for (int i = 0; i < MAX_CASCADES; i++)
    {
        float ratio         = (float)(i + 1) / MAX_CASCADES;
        float split_log     = nearZ * std::powf(farZ / nearZ, ratio);
        float split_uniform = nearZ + (farZ - nearZ) * ratio;

        _cascadeData.CascadeSplits[i] = lambda * split_log + (1.0f - lambda) * split_uniform;        
    }

    for (int i = 0; i < MAX_CASCADES; i++)
    {
        float prevSplit = i == 0 ? nearZ : _cascadeData.CascadeSplits[i - 1];
        float splitFar  = _cascadeData.CascadeSplits[i];

        // 1. 해당 캐스케이드의 프러스텀 조각을 정의
        BoundingFrustum frustumInProjSpace(camera->GetProjectionMatrix());
        frustumInProjSpace.Near = prevSplit;
        frustumInProjSpace.Far  = splitFar;

        // 2. 월드 공간으로 변환
        BoundingFrustum frustumInWorldSpace;
        frustumInProjSpace.Transform(frustumInWorldSpace, camera->GetWorldMatrix());

        // 3. 월드 공간 프러스텀의 8개 코너를 구함
        XMFLOAT3 corners[8];
        frustumInWorldSpace.GetCorners(corners);

        // 4. 8개 코너를 포함하는 바운딩 스피어의 중심과 반지름을 계산
        XMVECTOR frustumCenter = XMVectorZero();
        for (const auto& corner : corners)
        {
            frustumCenter = XMVectorAdd(frustumCenter, XMLoadFloat3(&corner));
        }
        frustumCenter = XMVectorScale(frustumCenter, 1.0f / 8.0f);

        float frustumRadius = 0.0f;
        for (const auto& corner : corners)
        {
            float distance = XMVectorGetX(XMVector3Length(XMVectorSubtract(XMLoadFloat3(&corner), frustumCenter)));
            frustumRadius = std::max(frustumRadius, distance);
        }

        // 5. 라이트 뷰 행렬 생성
        // 스피어 중심을 바라보되, 반지름만큼 뒤로 물러나서 모든 것을 볼 수 있도록 eye 위치 설정
        XMVECTOR eyePosition = XMVectorSubtract(frustumCenter, XMVectorScale(L, frustumRadius));
        XMMATRIX lightView = XMMatrixLookAtLH(eyePosition, frustumCenter, U);

        // 6. 안정적인 라이트 직교 투영 행렬 생성
        // 스피어의 지름(radius * 2)을 투영의 너비와 높이로 사용
        float viewWidth = frustumRadius * 2.0f;
        float viewHeight = frustumRadius * 2.0f;
        // Z 범위는 스피어를 충분히 포함하도록 설정 (0 부터 지름까지)
        XMMATRIX lightProj = XMMatrixOrthographicLH(viewWidth, viewHeight, 0.0f, frustumRadius * 2.0f);

        // 7. 최종 행렬 계산 및 저장
        _cascadeData.ShadowVP[i] = XMMatrixTranspose(lightView * lightProj);
    }    

    // 계산된 모든 캐스케이드 데이터를 상수 버퍼에 한 번에 업데이트
    _cascadeDataCBV->UpdateBuffer(&_cascadeData);
}

void ShadowMapPass::DrawMeshes(ID3D12GraphicsCommandList* commandList, MeshType meshType, CullMode cullMode, UINT offset)
{
    const auto& parallaxMappingProperty = std::any_cast<const ParallaxMappingProperty&>(Global::renderPassDatas->GetRenderPassProperty("G-BufferPass"));

    switch (meshType)
    {
    case STATIC_MESH:
        commandList->SetGraphicsRoot32BitConstants(_fxStaticMesh.GetRootParameterIndex("bit32_1_mipBias"), 1, &parallaxMappingProperty.MipBias, 0);
        break;

    case SKELETAL_MESH:
        commandList->SetGraphicsRoot32BitConstants(_fxSkeletalMesh.GetRootParameterIndex("bit32_1_mipBias"), 1, &parallaxMappingProperty.MipBias, 0);
        break;
    }
    
    UINT instanceCount = 0;
    BaseMesh* previousMesh  = nullptr;
    BaseMesh* currentMesh   = nullptr;
    for (auto& meshInfo : _meshInfos[meshType][cullMode])
    {
        if (nullptr == previousMesh)
        {
            currentMesh   = meshInfo->Mesh;
            previousMesh  = meshInfo->Mesh;
            instanceCount = 1;
            continue;
        }

        if (meshInfo->Mesh != previousMesh)
        {
            switch (meshType)
            {
            case STATIC_MESH:
                commandList->SetGraphicsRoot32BitConstants(_fxStaticMesh.GetRootParameterIndex("bit32_1_instanceOffset"), 1, &offset, 0);
                break;

            case SKELETAL_MESH:
                commandList->SetGraphicsRoot32BitConstants(_fxSkeletalMesh.GetRootParameterIndex("bit32_1_instanceOffset"), 1, &offset, 0);
                break;
            }

            previousMesh->Render(commandList, instanceCount * MAX_CASCADES);
            previousMesh = meshInfo->Mesh;
            offset += instanceCount;
            instanceCount = 1;
        }
        else
        {
            instanceCount++;
        }

        currentMesh = meshInfo->Mesh;
    }

    if (nullptr != currentMesh)
    {
        switch (meshType)
        {
        case STATIC_MESH:
            commandList->SetGraphicsRoot32BitConstants(_fxStaticMesh.GetRootParameterIndex("bit32_1_instanceOffset"), 1, &offset, 0);
            break;
        case SKELETAL_MESH:
            commandList->SetGraphicsRoot32BitConstants(_fxSkeletalMesh.GetRootParameterIndex("bit32_1_instanceOffset"), 1, &offset, 0);
            break;
        }

        currentMesh->Render(commandList, instanceCount * MAX_CASCADES);
    }
}