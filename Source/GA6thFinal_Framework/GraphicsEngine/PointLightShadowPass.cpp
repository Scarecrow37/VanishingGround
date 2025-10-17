#include "pch.h"
#include "PointLightShadowPass.h"
#include "FrameResource.h"

PointLightShadowPass::PointLightShadowPass() = default;

PointLightShadowPass::~PointLightShadowPass() = default;

void PointLightShadowPass::Initialize(RenderScene* ownerScene, RenderTechnique* ownerTechnique,
                                      ID3D12GraphicsCommandList* commandList)
{
    RenderPass::Initialize(ownerScene, ownerTechnique, commandList);

    // Atlas 초기화 (8192x8192 아틀라스, 각 페이스 1024x1024)
    _atlas.InitializeAtlas(8192, _shadowFaceSize);

    CreateShadowCubeMapResource();
    CreateShaderAndPSO();

    _instanceDatasBuffer = std::make_unique<StructuredBuffer>();
    _instanceDatasBuffer->Initialize(sizeof(InstanceData), MAX_OBJECTS * MAX_SHADOW_POINT_LIGHT);

    _alignedSize             = (sizeof(PointLightShadowData) + 255) & ~255;
    _pointLightShadowDataCBV = std::make_unique<ConstantBufferView>();
    _pointLightShadowDataCBV->Initialize(_alignedSize * MAX_SHADOW_POINT_LIGHT);
}

void PointLightShadowPass::AddRenderPassDatas(std::string_view sceneName)
{
    Global::renderPassDatas->AddRenderPassImage(sceneName, "PointLightShadowPass", "ShadowAtlas", _shadowAtlasSRV.GPU);
}

void PointLightShadowPass::Update(ID3D12GraphicsCommandList* commandList, const float deltaTime)
{
    // 이전 프레임 데이터 초기화
    for (auto& light : _perLightMeshInfos)
    {
        for (auto& type : light)
        {
            for (auto& meshInfos : type)
            {
                meshInfos.clear();
            }
        }
    }

    UpdateShadowLights();

    _instanceDatas.clear();

    size_t activeLightCount = _activeLightIndices.size();

    // 각 라이트별로 1. 범위 내 메쉬 수집 2. 섀도우 데이터 업데이트 3. 인스턴스 데이터 수집
    
    // 1. 범위 내의 메시 수집
    for (int meshType = 0; meshType < MESH_TYPE_END; ++meshType)
    {
        for (auto& meshInfo : _ownerScene->_activeMeshes[meshType])
        {
            Matrix worldMatrix = XMMatrixTranspose(_ownerScene->_matrices[meshInfo.InstanceData.MatrixID].World);

            BoundingOrientedBox worldOBB;
            meshInfo.Mesh->GetBoundingBox().Transform(worldOBB, worldMatrix);

            for (size_t i = 0; i < activeLightCount; ++i)
            {
                UINT        sceneLightIndex = _activeLightIndices[i];
                UINT        lightDataIndex = MAX_DIRECTIONAL_LIGHT + MAX_POINT_LIGHT + MAX_SPOT_LIGHT + sceneLightIndex;
                const auto& lightData      = _ownerScene->_lightDatas[lightDataIndex];
                Vector3     lightPosition  = lightData.float3_1;
                float       lightRange     = lightData.float_1;

                BoundingSphere lightSphere;
                lightSphere.Center = lightPosition;
                lightSphere.Radius = lightRange;

                if (worldOBB.Intersects(lightSphere))
                {
                    int cullMode = (int)meshInfo.Material.CullMode;
                    _perLightMeshInfos[sceneLightIndex][meshType][cullMode].push_back(&meshInfo);
                }               
            }
        }
    }    

    for (size_t i = 0; i < activeLightCount; ++i)
    {
        UINT        sceneLightIndex = _activeLightIndices[i];
        UINT        atlasIndex      = sceneLightIndex;
        UINT        lightDataIndex  = MAX_DIRECTIONAL_LIGHT + MAX_POINT_LIGHT + MAX_SPOT_LIGHT + sceneLightIndex;
        const auto& lightData       = _ownerScene->_lightDatas[lightDataIndex];

        // 2. 인스턴스 데이터 수집 (Static Mesh)
        for (int cullMode = 0; cullMode < CullMode::END; ++cullMode)
        {
            for (auto& meshInfo : _perLightMeshInfos[sceneLightIndex][STATIC_MESH][cullMode])
            {
                _instanceDatas.emplace_back(meshInfo->InstanceData);
            }
        }
        for (int cullMode = 0; cullMode < CullMode::END; ++cullMode)
        {
            for (auto& meshInfo : _perLightMeshInfos[sceneLightIndex][SKELETAL_MESH][cullMode])
            {
                _instanceDatas.emplace_back(meshInfo->InstanceData);
            }
        }

        // 3. 섀도우 데이터 업데이트
        PointLightShadowData shadowData;
        for (int j = 0; j < 6; ++j)
        {
            shadowData.ViewProjection[j] = _cubeFaceViewProjections[atlasIndex][j];
        }
        shadowData.LightPosition = lightData.float3_1;
        shadowData.FarPlane      = lightData.float_1;

        size_t offset = _alignedSize * sceneLightIndex;
        _pointLightShadowDataCBV->UpdateBufferWithOffset(&shadowData, offset, sizeof(PointLightShadowData));
    }

    _instanceDatasBuffer->CopyStructuredBuffer(commandList, _instanceDatas.data(), (UINT)_instanceDatas.size());
}

void PointLightShadowPass::Begin(ID3D12GraphicsCommandList* commandList)
{
    auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        _atlas.GetResource(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_DEPTH_WRITE);
    commandList->ResourceBarrier(1, &barrier);
}

void PointLightShadowPass::Draw(ID3D12GraphicsCommandList* commandList)
{
    if (_activeLightIndices.empty())
        return;

    UINT  currentBackBufferIndex = Global::device->GetCurrentBackBufferIndex();
    auto  resource               = Global::viewManager->GetShaderResourceHeap()->GetGPUDescriptorHandleForHeapStart();
    auto  instanceData           = _instanceDatasBuffer->GetGPUVirtualAddress();
    auto& frameResource          = _ownerScene->_frameResources[currentBackBufferIndex];

    UINT indicesLength = (UINT)_activeLightIndices.size();

    D3D12_GPU_VIRTUAL_ADDRESS baseCBVAddress = _pointLightShadowDataCBV->GetGPUVirtualAddress();

    UINT globalInstanceOffset = 0;

    for (UINT i = 0; i < indicesLength; ++i)
    {
        UINT sceneLightIndex = _activeLightIndices[i];
        UINT atlasIndex      = sceneLightIndex;

        D3D12_GPU_VIRTUAL_ADDRESS shadowDataCBV = baseCBVAddress + (_alignedSize * sceneLightIndex);

        D3D12_VIEWPORT viewports[6];
        D3D12_RECT     scissorRects[6];

        for (UINT face = 0; face < 6; ++face)
        {
            viewports[face]    = _atlas.GetViewport(atlasIndex, face);
            scissorRects[face] = _atlas.GetScissorRect(atlasIndex, face);
        }

        commandList->RSSetViewports(6, viewports);
        commandList->RSSetScissorRects(6, scissorRects);

        DescriptorHandles dsvHandle = _atlas.GetDSVHandle();
        commandList->OMSetRenderTargets(0, nullptr, FALSE, &dsvHandle.CPU);

        for (UINT face = 0; face < 6; ++face)
        {
            commandList->ClearDepthStencilView(dsvHandle.CPU, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 1, &scissorRects[face]);
        }

        UINT offset = globalInstanceOffset;

        // Static Mesh
        commandList->SetGraphicsRootSignature(_fxStaticMesh.GetRootSignature());
        commandList->SetGraphicsRootDescriptorTable(_fxStaticMesh.GetRootParameterIndex("textures"), resource);
        commandList->SetGraphicsRootConstantBufferView(_fxStaticMesh.GetRootParameterIndex("pointLightShadowData"),
                                                       shadowDataCBV);
        commandList->SetGraphicsRootShaderResourceView(_fxStaticMesh.GetRootParameterIndex("instanceData"),
                                                       instanceData);
        frameResource->SetFrameResource(FrameResourceType::TRANSFORM, _fxStaticMesh.GetRootParameterIndex("matrices"),
                                        commandList);

        commandList->SetPipelineState(_psos[STATIC_MESH][CULL_BACK].Get());
        DrawMeshes(commandList, STATIC_MESH, CULL_BACK, sceneLightIndex, offset);

        offset += (UINT)_perLightMeshInfos[sceneLightIndex][STATIC_MESH][CULL_BACK].size();
        commandList->SetPipelineState(_psos[STATIC_MESH][CULL_FRONT].Get());
        DrawMeshes(commandList, STATIC_MESH, CULL_FRONT, sceneLightIndex, offset);

        offset += (UINT)_perLightMeshInfos[sceneLightIndex][STATIC_MESH][CULL_FRONT].size();
        commandList->SetPipelineState(_psos[STATIC_MESH][TWO_SIDED].Get());
        DrawMeshes(commandList, STATIC_MESH, TWO_SIDED, sceneLightIndex, offset);

        // Skeletal Mesh
        commandList->SetGraphicsRootSignature(_fxSkeletalMesh.GetRootSignature());
        commandList->SetGraphicsRootDescriptorTable(_fxSkeletalMesh.GetRootParameterIndex("textures"), resource);
        commandList->SetGraphicsRootConstantBufferView(_fxSkeletalMesh.GetRootParameterIndex("pointLightShadowData"),
                                                       shadowDataCBV);
        commandList->SetGraphicsRootShaderResourceView(_fxSkeletalMesh.GetRootParameterIndex("instanceData"),
                                                       instanceData);
        frameResource->SetFrameResource(FrameResourceType::TRANSFORM, _fxSkeletalMesh.GetRootParameterIndex("matrices"),
                                        commandList);
        frameResource->SetFrameResource(FrameResourceType::BONE_MATRICES,
                                        _fxSkeletalMesh.GetRootParameterIndex("boneMatrices"), commandList);

        offset += (UINT)_perLightMeshInfos[sceneLightIndex][STATIC_MESH][TWO_SIDED].size();
        commandList->SetPipelineState(_psos[SKELETAL_MESH][CULL_BACK].Get());
        DrawMeshes(commandList, SKELETAL_MESH, CULL_BACK, sceneLightIndex, offset);

        offset += (UINT)_perLightMeshInfos[sceneLightIndex][SKELETAL_MESH][CULL_BACK].size();
        commandList->SetPipelineState(_psos[SKELETAL_MESH][CULL_FRONT].Get());
        DrawMeshes(commandList, SKELETAL_MESH, CULL_FRONT, sceneLightIndex, offset);

        offset += (UINT)_perLightMeshInfos[sceneLightIndex][SKELETAL_MESH][CULL_FRONT].size();
        commandList->SetPipelineState(_psos[SKELETAL_MESH][TWO_SIDED].Get());
        DrawMeshes(commandList, SKELETAL_MESH, TWO_SIDED, sceneLightIndex, offset);
        

        // 다음 라이트의 인스턴스 오프셋 계산
        for (int meshType = 0; meshType < MESH_TYPE_END; ++meshType)
        {
            for (int cullMode = 0; cullMode < CullMode::END; ++cullMode)
            {
                globalInstanceOffset += (UINT)_perLightMeshInfos[sceneLightIndex][meshType][cullMode].size();
            }
        }
    }
}

void PointLightShadowPass::End(ID3D12GraphicsCommandList* commandList)
{
    auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(_atlas.GetResource(), D3D12_RESOURCE_STATE_DEPTH_WRITE,
                                                        D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    commandList->ResourceBarrier(1, &barrier);
}

void PointLightShadowPass::CreateShadowCubeMapResource()
{
    Global::viewManager->AddDescriptorHeap(ViewManager::Type::SHADER_RESOURCE, _shadowAtlasSRV);

    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format                          = DXGI_FORMAT_R32_FLOAT;
    srvDesc.ViewDimension                   = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Shader4ComponentMapping         = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Texture2D.MipLevels             = 1;

    Global::device->GetDevice()->CreateShaderResourceView(_atlas.GetResource(), &srvDesc, _shadowAtlasSRV.CPU);
}

void PointLightShadowPass::CreateShaderAndPSO()
{
    PipelineStateStream pss;
    pss.BlendState                               = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    pss.RasterizerState                          = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    (&pss.RasterizerState)->DepthBias            = 5000;
    (&pss.RasterizerState)->DepthBiasClamp       = 0.01f;
    (&pss.RasterizerState)->SlopeScaledDepthBias = 1.5f;
    pss.PrimitiveTopology                        = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    pss.DSVFormat                                = DXGI_FORMAT_D32_FLOAT;
    

    auto CreatePipelineStateStream = [this, &pss](int meshType) {
        (&pss.RasterizerState)->CullMode = D3D12_CULL_MODE_BACK;
        _psos[meshType][CULL_BACK]       = Global::pipelineStateManager->GetPipelineState(pss);

        (&pss.RasterizerState)->CullMode = D3D12_CULL_MODE_FRONT;
        _psos[meshType][CULL_FRONT]      = Global::pipelineStateManager->GetPipelineState(pss);

        (&pss.RasterizerState)->CullMode = D3D12_CULL_MODE_NONE;
        _psos[meshType][TWO_SIDED]       = Global::pipelineStateManager->GetPipelineState(pss);
    };

    _fxStaticMesh.SetPipelineStateStream(pss);
    CreatePipelineStateStream(STATIC_MESH);

    _fxSkeletalMesh.SetPipelineStateStream(pss);
    CreatePipelineStateStream(SKELETAL_MESH);
}

void PointLightShadowPass::UpdateShadowLights()
{
    _activeLightIndices.clear();

    UINT numShadowPointLights = _ownerScene->_numLight.ShadowPoint;

    for (UINT i = 0; i < numShadowPointLights && i < MAX_SHADOW_POINT_LIGHT; ++i)
    {
        UINT        lightDataIndex = MAX_DIRECTIONAL_LIGHT + MAX_POINT_LIGHT + MAX_SPOT_LIGHT + i;
        const auto& lightData      = _ownerScene->_lightDatas[lightDataIndex];

        Vector3 lightPosition = lightData.float3_1;
        float   lightRange    = lightData.float_1;

        UINT atlasIndex = i;
        if (!_atlas.IsAllocated(atlasIndex))
        {
            _atlas.AllocateLight(atlasIndex);
        }

        UpdateCubeFaceMatrices(atlasIndex, lightPosition, lightRange);

        _activeLightIndices.push_back(atlasIndex);
    }

    // 사용하지 않는 인덱스의 라이트 아틀라스 해제
    for (UINT i = numShadowPointLights; i < MAX_SHADOW_POINT_LIGHT; ++i)
    {
        if (_atlas.IsAllocated(i))
        {
            _atlas.ReleaseLight(i);
        }
    }
}

void PointLightShadowPass::UpdateCubeFaceMatrices(UINT lightIndex, const Vector3& lightPosition, float lightRange)
{
    static const Vector3 targets[6] = {
        Vector3(1.0f, 0.0f, 0.0f),  // +x
        Vector3(-1.0f, 0.0f, 0.0f), // -x
        Vector3(0.0f, 1.0f, 0.0f),  // +y
        Vector3(0.0f, -1.0f, 0.0f), // -y
        Vector3(0.0f, 0.0f, 1.0f),  // +z
        Vector3(0.0f, 0.0f, -1.0f)  // -z
    };

    static const Vector3 ups[6] = {
        Vector3(0.0f, 1.0f, 0.0f),  // +x
        Vector3(0.0f, 1.0f, 0.0f),  // -x
        Vector3(0.0f, 0.0f, -1.0f), // +y
        Vector3(0.0f, 0.0f, 1.0f),  // -y
        Vector3(0.0f, 1.0f, 0.0f),  // +z
        Vector3(0.0f, 1.0f, 0.0f)   // -z
    };

    float fov    = XM_PIDIV2;
    float aspect = 1.0f;
    float nearZ  = 0.01f;
    float farZ   = lightRange;

    XMMATRIX projection = XMMatrixPerspectiveFovLH(fov, aspect, nearZ, farZ);

    for (int face = 0; face < 6; ++face)
    {
        XMVECTOR eyePos = XMLoadFloat3(&lightPosition);
        XMVECTOR target = XMVectorAdd(eyePos, XMLoadFloat3(&targets[face]));
        XMVECTOR up     = XMLoadFloat3(&ups[face]);

        XMMATRIX view     = XMMatrixLookAtLH(eyePos, target, up);
        XMMATRIX viewProj = view * projection;

        XMStoreFloat4x4(&_cubeFaceViewProjections[lightIndex][face], XMMatrixTranspose(viewProj));
    }
}

void PointLightShadowPass::DrawMeshes(ID3D12GraphicsCommandList* commandList, MeshType meshType, CullMode cullMode,
                                      UINT lightIndex, UINT instanceOffset)
{
    UINT      instanceCount = 0;
    BaseMesh* previousMesh  = nullptr;
    BaseMesh* currentMesh   = nullptr;
    UINT parameter[2] = {instanceOffset, 0};

    // lightIndex에 해당하는 메시만 렌더링
    for (auto& meshInfo : _perLightMeshInfos[lightIndex][meshType][cullMode])
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
            parameter[1] = instanceCount;
            switch (meshType)
            {
            case STATIC_MESH:
                commandList->SetGraphicsRoot32BitConstants(
                    _fxStaticMesh.GetRootParameterIndex("bit32_2_shadowMeshData"), 2, &parameter, 0);
                break;
            case SKELETAL_MESH:
                commandList->SetGraphicsRoot32BitConstants(
                    _fxSkeletalMesh.GetRootParameterIndex("bit32_2_shadowMeshData"), 2, &parameter, 0);
                break;
            }

            previousMesh->Render(commandList, instanceCount * 6);
            previousMesh = meshInfo->Mesh;
            parameter[0] += instanceCount;
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
            commandList->SetGraphicsRoot32BitConstants(_fxStaticMesh.GetRootParameterIndex("bit32_2_shadowMeshData"), 2,
                                                       &parameter, 0);
            break;
        case SKELETAL_MESH:
            commandList->SetGraphicsRoot32BitConstants(_fxSkeletalMesh.GetRootParameterIndex("bit32_2_shadowMeshData"),
                                                       2, &parameter, 0);
            break;
        }

        currentMesh->Render(commandList, instanceCount * 6);
    }
}