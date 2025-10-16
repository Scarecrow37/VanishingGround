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
    _instanceDatasBuffer->Initialize(sizeof(InstanceData), MAX_OBJECTS);

    _pointLightShadowDataCBV = std::make_unique<ConstantBufferView>();
    _pointLightShadowDataCBV->Initialize(sizeof(PointLightShadowData) * MAX_SHADOW_POINT_LIGHT);
}

void PointLightShadowPass::AddRenderPassDatas(std::string_view sceneName)
{
    Global::renderPassDatas->AddRenderPassImage(sceneName, "PointLightShadowPass", "ShadowAtlas",
                                                _shadowAtlasSRV.GPU);
}

void PointLightShadowPass::Update(ID3D12GraphicsCommandList* commandList, const float deltaTime)
{
    for (auto& type : _meshInfos)
    {
        for (auto& meshInfos : type)
        {
            meshInfos.clear();
        }
    }

    UpdateShadowLights();

    // 메시를 컬 모드별로 분류
    for (int i = 0; i < MESH_TYPE_END; i++)
    {
        for (auto& meshInfo : _ownerScene->_activeMeshes[i])
        {
            int cullMode = (int)meshInfo.Material.CullMode;
            _meshInfos[i][cullMode].push_back(&meshInfo);
        }
    }

    // 인스턴스 데이터 수집
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
    for (UINT i = 0; i < indicesLength; ++i)
    {
        UINT        lightIndex     = _activeLightIndices[i];
        UINT        lightDataIndex = MAX_DIRECTIONAL_LIGHT + MAX_POINT_LIGHT + MAX_SPOT_LIGHT + lightIndex;
        const auto& lightData      = _ownerScene->_lightDatas[lightDataIndex];

        PointLightShadowData shadowData = {};
        for (int j = 0; j < 6; ++j)
        {
            shadowData.ViewProjection[j] = _cubeFaceViewProjections[lightIndex][j];
        }

        shadowData.LightPosition = lightData.float3_1;
        shadowData.FarPlane      = lightData.float_1;
        _pointLightShadowDataCBV->UpdateBuffer(&shadowData);
        auto shadowDatrCBV = _pointLightShadowDataCBV->GetGPUVirtualAddress();

        // 6면 캡쳐
        for (UINT faceIndex = 0; faceIndex < 6; ++faceIndex)
        {
            DescriptorHandles dsvHandle = _atlas.GetDSVHandle(lightIndex, faceIndex);
            commandList->ClearDepthStencilView(dsvHandle.CPU, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
            commandList->OMSetRenderTargets(0, nullptr, FALSE, &dsvHandle.CPU);
            D3D12_VIEWPORT viewport    = _atlas.GetViewport(lightIndex, faceIndex);
            D3D12_RECT     scissorRect = _atlas.GetScissorRect(lightIndex, faceIndex);
            commandList->RSSetViewports(1, &viewport);
            commandList->RSSetScissorRects(1, &scissorRect);

            UINT offset = 0;

            // Static Mesh
            commandList->SetGraphicsRootSignature(_fxStaticMesh.GetRootSignature());
            commandList->SetGraphicsRootDescriptorTable(_fxStaticMesh.GetRootParameterIndex("textures"), resource);
            commandList->SetGraphicsRootConstantBufferView(_fxStaticMesh.GetRootParameterIndex("pointLightShadowData"),
                                                           shadowDatrCBV);
            commandList->SetGraphicsRootShaderResourceView(_fxStaticMesh.GetRootParameterIndex("instanceData"),
                                                           instanceData);
            frameResource->SetFrameResource(FrameResourceType::TRANSFORM,
                                            _fxStaticMesh.GetRootParameterIndex("matrices"), commandList);

            commandList->SetPipelineState(_psos[STATIC_MESH][CULL_BACK].Get());
            DrawMeshes(commandList, STATIC_MESH, CULL_BACK, lightIndex, faceIndex, offset);

            offset += (UINT)_meshInfos[STATIC_MESH][CULL_BACK].size();
            commandList->SetPipelineState(_psos[STATIC_MESH][CULL_FRONT].Get());
            DrawMeshes(commandList, STATIC_MESH, CULL_FRONT, lightIndex, faceIndex, offset);

            offset += (UINT)_meshInfos[STATIC_MESH][CULL_FRONT].size();
            commandList->SetPipelineState(_psos[STATIC_MESH][TWO_SIDED].Get());
            DrawMeshes(commandList, STATIC_MESH, TWO_SIDED, lightIndex, faceIndex, offset);

            // Skeletal Mesh
            commandList->SetGraphicsRootSignature(_fxSkeletalMesh.GetRootSignature());
            commandList->SetGraphicsRootDescriptorTable(_fxSkeletalMesh.GetRootParameterIndex("textures"), resource);
            commandList->SetGraphicsRootConstantBufferView(_fxSkeletalMesh.GetRootParameterIndex("pointLightShadowData"), shadowDatrCBV);
            commandList->SetGraphicsRootShaderResourceView(_fxSkeletalMesh.GetRootParameterIndex("instanceData"), instanceData);
            frameResource->SetFrameResource(FrameResourceType::TRANSFORM, _fxSkeletalMesh.GetRootParameterIndex("matrices"), commandList);
            frameResource->SetFrameResource(FrameResourceType::BONE_MATRICES, _fxSkeletalMesh.GetRootParameterIndex("boneMatrices"), commandList);
            
            offset += (UINT)_meshInfos[STATIC_MESH][TWO_SIDED].size();
            commandList->SetPipelineState(_psos[SKELETAL_MESH][CULL_BACK].Get());
            DrawMeshes(commandList, SKELETAL_MESH, CULL_BACK, lightIndex, faceIndex, offset);

            offset += (UINT)_meshInfos[SKELETAL_MESH][CULL_BACK].size();
            commandList->SetPipelineState(_psos[SKELETAL_MESH][CULL_FRONT].Get());
            DrawMeshes(commandList, SKELETAL_MESH, CULL_FRONT, lightIndex, faceIndex, offset);

            offset += (UINT)_meshInfos[SKELETAL_MESH][CULL_FRONT].size();
            commandList->SetPipelineState(_psos[SKELETAL_MESH][TWO_SIDED].Get());
            DrawMeshes(commandList, SKELETAL_MESH, TWO_SIDED, lightIndex, faceIndex, offset);
        }
    }
}

void PointLightShadowPass::End(ID3D12GraphicsCommandList* commandList)
{
    // Atlas 리소스를 Pixel Shader Resource 상태로 전환
    auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(_atlas.GetResource(), D3D12_RESOURCE_STATE_DEPTH_WRITE,
                                                        D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    commandList->ResourceBarrier(1, &barrier);
}

void PointLightShadowPass::CreateShadowCubeMapResource()
{
    // SRV 생성
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
    float nearZ  = 0.1f;
    float farZ   = lightRange;

    XMMATRIX projection = XMMatrixPerspectiveFovLH(fov, aspect, nearZ, farZ);

    for (int face = 0; face < 6; ++face)
    {
        XMVECTOR eyePos = XMLoadFloat3(&lightPosition);
        XMVECTOR target = XMVectorAdd(eyePos, XMLoadFloat3(&targets[face]));
        XMVECTOR up     = XMLoadFloat3(&ups[face]);

        XMMATRIX view     = XMMatrixLookAtLH(eyePos, target, up);
        XMMATRIX viewProj = view * projection;

        XMStoreFloat4x4(&_cubeFaceViewProjections[lightIndex][face],XMMatrixTranspose(viewProj));
    }
}

void PointLightShadowPass::DrawMeshes(ID3D12GraphicsCommandList* commandList, MeshType meshType, CullMode cullMode,
                                      UINT lightIndex, UINT faceIndex, UINT instanceOffset)
{
    UINT      parameter[2]  = {faceIndex, instanceOffset};
    UINT      instanceCount = 0;
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
                commandList->SetGraphicsRoot32BitConstants(
                    _fxStaticMesh.GetRootParameterIndex("bit32_2_shadowMeshData"), 2, &parameter, 0);
                break;
            case SKELETAL_MESH:
                commandList->SetGraphicsRoot32BitConstants(
                    _fxSkeletalMesh.GetRootParameterIndex("bit32_2_shadowMeshData"), 2, &parameter, 0);
                break;
            }

            previousMesh->Render(commandList, instanceCount);
            previousMesh = meshInfo->Mesh;
            parameter[1] += instanceCount;
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

        currentMesh->Render(commandList, instanceCount);
    }
}
