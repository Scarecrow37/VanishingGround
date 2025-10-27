#include "pch.h"
#include "DXRDrawPass.h"
#include "CommandSet.h"
#include "ExportAssociation.h"
#include "FrameResource.h"
#include "GlobalRootSignature.h"
#include "HitProgram.h"
#include "LocalRootSignature.h"
#include "MeshRenderer.h"
#include "Model.h"
#include "PipelineConfig.h"
#include "RTPipeline.h"
#include "RenderScene.h"
#include "RenderTarget.h"
#include "ShaderConfig.h"
#include "SkyBox.h"
#include "d3dUtil.h"
#include "DXRSkeletalMesh.h"

DXRDrawPass::~DXRDrawPass() {}

void DXRDrawPass::Initialize(RenderScene* ownerScene, RenderTechnique* ownerTechnique,
                             ID3D12GraphicsCommandList* commandList)
{
    RenderPass::Initialize(ownerScene, ownerTechnique, commandList);
    CreateStateObject();
    CreateShaderResource();

    _instanceDatasBuffer = std::make_unique<StructuredBuffer>();
    _instanceDatasBuffer->Initialize(sizeof(InstanceData), MAX_OBJECTS);
}

void DXRDrawPass::Begin(ID3D12GraphicsCommandList* commandList)
{
    UpdateFrameResource(commandList);
}

void DXRDrawPass::Draw(ID3D12GraphicsCommandList* commandList)
{
    for (auto& mesh : _ownerScene->_activeMeshes[STATIC_MESH])
    {
        _ownerScene->_accelerationStructureManager->SubmitStaticInstance(mesh);
    }
    for (auto& mesh : _ownerScene->_activeMeshes[SKELETAL_MESH])
    {
        _ownerScene->_accelerationStructureManager->SubmitSkeletalInstance(mesh);
    }
    auto                               commandlist = Global::device->GetCommandList();
    ComPtr<ID3D12GraphicsCommandList4> commandList4;

    HRESULT hr = commandlist->QueryInterface(IID_PPV_ARGS(commandList4.GetAddressOf()));
    FAILED_CHECK_MESSAGE(hr, L"DXRDrawPass::Draw() failed to get ID3D12GraphicsCommandList4 interface");

    _ownerScene->_accelerationStructureManager->EndFrame(commandList4.Get());
    commandlist->Close();
    Global::commandController->ExecuteCommand(CommandQueueType::GRAPHICS_QUEUE, commandlist);

    Global::device->ResetGraphicsCommnad();
    CreateShaderTable();
}

void DXRDrawPass::End(ID3D12GraphicsCommandList* commandList)
{
    WriteCommand(commandList);
}

void DXRDrawPass::CreateStateObject()
{
    std::array<D3D12_STATE_SUBOBJECT, 12> subobjects{};
    uint32_t                              index = 0;

    DxilLibrary dxilLibrary = RTPipeline::CreateDxilLibrary();
    subobjects[index++]     = dxilLibrary.stateSubObject; // 0

    HitProgram hitProgram(nullptr, RTPipeline::ClosestHitShader, RTPipeline::HitGroup);
    subobjects[index++] = hitProgram.subObject; // 1

    LocalRootSignature rgsRootSignature(RTPipeline::CreateRayGenRootDesc().desc);
    subobjects[index] = rgsRootSignature.subObject; // 2

    const uint32_t    rgsRootIndex = index++;
    ExportAssociation rgsRootAssociation(&RTPipeline::RayGenShader, 1, &(subobjects[rgsRootIndex]));
    subobjects[index++] = rgsRootAssociation.subObject; // 3

    LocalRootSignature hitRootSignature(RTPipeline::CreateHitRootDesc().desc);
    subobjects[index] = hitRootSignature.subObject; // 4

    const uint32_t    hitRootIndex = index++;
    ExportAssociation hitRootAssociation(&RTPipeline::ClosestHitShader, 1, &(subobjects[hitRootIndex]));
    subobjects[index++] = hitRootAssociation.subObject; // 5

    LocalRootSignature missRootSignature(RTPipeline::CreateMissRootDesc().desc);
    subobjects[index] = missRootSignature.subObject; // 6

    const uint32_t missRootIndex     = index++;
    const WCHAR*   missRootShaders[] = {RTPipeline::MissShader, RTPipeline::ShadowMissShader};

    ExportAssociation missRootAssociation(missRootShaders, _countof(missRootShaders), &(subobjects[missRootIndex]));
    subobjects[index++] = missRootAssociation.subObject; // 7

    // payload size float4 + uint + float3
    ShaderConfig shaderConfig(sizeof(float) * 2, sizeof(float) * (4 + 1));
    subobjects[index] = shaderConfig.subObject; // 8

    const uint32_t    shaderConfigIndex = index++;
    const WCHAR*      shaderExports[] = {RTPipeline::MissShader, RTPipeline::ClosestHitShader, RTPipeline::RayGenShader,
                                         RTPipeline::ShadowMissShader};
    ExportAssociation configAssociation(shaderExports, _countof(shaderExports), &(subobjects[shaderConfigIndex]));
    subobjects[index++] = configAssociation.subObject; // 9

    PipelineConfig config(7 + 1);
    subobjects[index++] = config.subObject; // 10

    GlobalRootSignature root(RTPipeline::CreateGlobalRootDesc().desc);
    _globalRootsignature = root.rootSignature;
    subobjects[index++]  = root.subObject; // 11

    D3D12_STATE_OBJECT_DESC desc;
    desc.NumSubobjects = index;
    desc.pSubobjects   = subobjects.data();
    desc.Type          = D3D12_STATE_OBJECT_TYPE_RAYTRACING_PIPELINE;

    ComPtr<ID3D12Device5> device5 = Global::device->GetDevice5();
    HRESULT               hr      = device5->CreateStateObject(&desc, IID_PPV_ARGS(_pso.GetAddressOf()));
    FAILED_CHECK_MESSAGE(hr, L"DXRDrawPass::CreateStateObject() failed Create RT Pipeline stateObject ");
}

void DXRDrawPass::CreateShaderTable()
{
    // 0) 공통 상수 정의
    constexpr UINT bytesId   = D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES;         // 32
    constexpr UINT bytesArgs = sizeof(D3D12_GPU_DESCRIPTOR_HANDLE);           //  8
    constexpr UINT aligeRec  = D3D12_RAYTRACING_SHADER_RECORD_BYTE_ALIGNMENT; // 32

    const UINT raygenSize     = d3dUtil::AlignTo(bytesId + 2 * bytesArgs, 32);
    const UINT missSize       = d3dUtil::AlignTo(bytesId + bytesArgs, 32);
    const UINT hitSize        = d3dUtil::AlignTo(bytesId + 7 * bytesArgs, 32);
    const UINT shadowMissSize = d3dUtil::AlignTo(bytesId, 32);
    _shaderTableEntrySize     = d3dUtil::AlignTo(std::max({raygenSize, missSize, hitSize, shadowMissSize}),
                                                 D3D12_RAYTRACING_SHADER_TABLE_BYTE_ALIGNMENT);

    const UINT recordCount = 4;
    const UINT tableSize   = _shaderTableEntrySize * recordCount;
    // 1) 업로드 버퍼 생성
    if (!_init)
    {
        Global::device->CreateUploadBuffer(tableSize, D3D12_RESOURCE_FLAG_NONE, D3D12_RESOURCE_STATE_GENERIC_READ,
                                           _shaderTable);
        _init = true;
    }
    // 2) Shader Identifier 가져오기
    ComPtr<ID3D12StateObjectProperties> props;
    _pso->QueryInterface(IID_PPV_ARGS(props.GetAddressOf()));

    const void* ID_RGS         = props->GetShaderIdentifier(RTPipeline::RayGenShader);
    const void* ID_MISS        = props->GetShaderIdentifier(RTPipeline::MissShader);
    const void* ID_HIT         = props->GetShaderIdentifier(RTPipeline::HitGroup);
    const void* ID_SHADOW_MISS = props->GetShaderIdentifier(RTPipeline::ShadowMissShader);
    // const void* ID_SHADOW_HIT  = props->GetShaderIdentifier(RTPipeline::ShadowHitGroup);
    //  3) Shader Table 레코드 작성
    uint8_t* p = nullptr;
    _shaderTable->Map(0, nullptr, reinterpret_cast<void**>(&p));
    /* ── 4‑1) Ray Generation ────────────────────────────────────────── */
    {
        memcpy(p, ID_RGS, bytesId);
        // RootParam#1 : SRV t0 테이블 핸들
        *reinterpret_cast<UINT64*>(p + bytesId) = _ownerScene->_accelerationStructureManager->GetTopLevelSRV().GPU.ptr;
        // RootParam#2 : UAV u0 테이블 핸들
        *reinterpret_cast<UINT64*>(p + bytesId + bytesArgs) = _outputResourceUAV->GetUAVHandle().ptr;
    }
    /* ── 4‑2) Miss ────────────────────────────────────────── */
    p += _shaderTableEntrySize;
    {
        memcpy(p, ID_MISS, bytesId);
        // RootParam#0 : envTexture(t4) 테이블 핸들
        *reinterpret_cast<UINT64*>(p + bytesId) = _ownerScene->_skyBox->GetCubeMapSRV().ptr;
    }
    /* ── 4‑3) ShadowMiss ────────────────────────────────────────── */
    {
        p += _shaderTableEntrySize;
        memcpy(p, ID_SHADOW_MISS, bytesId); // Shadow Miss (파라미터 없음)
    }
    /* ── 4‑4) Hit Group ─────────────────────────────────────────────── */
    p += _shaderTableEntrySize;
    {
        memcpy(p, ID_HIT, bytesId);
        // SRV t0 rtScene
        *reinterpret_cast<UINT64*>(p + bytesId) = _ownerScene->_accelerationStructureManager->GetTopLevelSRV().GPU.ptr;
        // SRV t5 irradiance
        *reinterpret_cast<UINT64*>(p + bytesId + (1 * bytesArgs)) = _ownerScene->_skyBox->GetIrradianceMapSRV().ptr;
        // SRV t6 prefiltered
        *reinterpret_cast<UINT64*>(p + bytesId + (2 * bytesArgs)) = _ownerScene->_skyBox->GetPrefilteredMapSRV().ptr;
        // SRV t7 brdfLUT
        *reinterpret_cast<UINT64*>(p + bytesId + (3 * bytesArgs)) = _ownerScene->_skyBox->GetBrdfLUTSRV().ptr;
        // SRV t8 VerticesS
        *reinterpret_cast<UINT64*>(p + bytesId + (4 * bytesArgs)) = Global::viewManager->GetVertexBufferSrvPtr();
        // SRV t2008 Indices
        *reinterpret_cast<UINT64*>(p + bytesId + (5 * bytesArgs)) = Global::viewManager->GetIndexBufferSrvPtr();
        // SRV t4008~ textures
        *reinterpret_cast<UINT64*>(p + bytesId + (6 * bytesArgs)) =
            Global::viewManager->GetShaderResourceHeap()->GetGPUDescriptorHandleForHeapStart().ptr;
    }
    ///* ── 4‑5) Hit Group ─────────────────────────────────────────────── */
    //{
    //    p += _shaderTableEntrySize;
    //    memcpy(p, ID_SHADOW_HIT, bytesId); // Shadow Hit (파라미터 없음)
    //}
    _shaderTable->Unmap(0, nullptr);
}

void DXRDrawPass::CreateShaderResource()
{
    ID3D12GraphicsCommandList* cmdlist = Global::device->GetCommandList();
    _outputResourceUAV                 = MakeSharedResource<UnorderedAccessView>();
    DXGI_MODE_DESC mode                = Global::device->GetMode();

    auto desc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R32G32B32A32_FLOAT, mode.Width, mode.Height, 1, 1, 1, 0,
                                             D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);

    _outputResourceUAV->InitializeAsTexture(desc, UnorderedAccessView::UAVSliceType::PER_MIP, false);
    Global::dxResourceManager->AddResource(_outputResourceUAV);
}

void DXRDrawPass::UpdateFrameResource(ID3D12GraphicsCommandList* commandList)
{
    UINT currentBackbufferIndex = _ownerScene->_currentFrameIndex;

    _instanceDatas.clear();
    for (auto& meshInfo : _ownerScene->_activeMeshes[STATIC_MESH])
    {
        _instanceDatas.emplace_back(meshInfo.InstanceData);
    }

    for (auto& meshInfo : _ownerScene->_activeMeshes[SKELETAL_MESH])
    {
        _instanceDatas.emplace_back(meshInfo.InstanceData);
    }

    _instanceDatasBuffer->CopyStructuredBuffer(commandList, _instanceDatas.data(), (UINT)_instanceDatas.size());


    //// Update Instance ID
    //_meshInstanceIDs.clear();
    //UINT staticMeshCount = static_cast<UINT>(_ownerScene->_staticMeshInstanceIDs.size());
    //for (UINT i = 0; i < staticMeshCount; ++i)
    //{
    //    _meshInstanceIDs.push_back(_ownerScene->_staticMeshInstanceIDs[i]);
    //}
    //UINT skeletalMeshCount = static_cast<UINT>(_ownerScene->_skeletalMeshInstanceIDs.size());
    //for (UINT i = 0; i < skeletalMeshCount; ++i)
    //{
    //    _meshInstanceIDs.push_back(_ownerScene->_skeletalMeshInstanceIDs[i]);
    //}
    //UINT currentFrameIndex = _ownerScene->_currentFrameIndex;
    //_ownerScene->_frameResources[currentFrameIndex]->CopyStructuredBuffer(
    //    commandList, FrameResourceType::MESH_INSTANCE_ID, _meshInstanceIDs.data(),
    //    static_cast<UINT>(_meshInstanceIDs.size()));

    // Update VIBUffer ID
    _vertexBufferIDs.clear();
    _indexBufferIDs.clear();
    for (auto& meshInfo : _ownerScene->_activeMeshes[STATIC_MESH])
    {
        const auto& vibuffer       = meshInfo.Mesh->GetVIBuffer();
        UINT        vertexBufferID = vibuffer->_vertexBufferID;
        UINT        indexbufferID  = vibuffer->_indexBufferID;

        _vertexBufferIDs.push_back(vertexBufferID);
        _indexBufferIDs.push_back(indexbufferID);
    }
    for (auto& meshInfo : _ownerScene->_activeMeshes[SKELETAL_MESH])
    {
        const auto& instance = meshInfo.SkinnedInstance;
        const auto& vibuffer       = meshInfo.Mesh->GetVIBuffer();
        UINT        vertexBufferID = instance->GetID();
        UINT        indexbufferID  = vibuffer->_indexBufferID;

        _vertexBufferIDs.push_back(vertexBufferID);
        _indexBufferIDs.push_back(indexbufferID);
    }
    _ownerScene->_frameResources[currentBackbufferIndex]->CopyStructuredBuffer(
        commandList, FrameResourceType::VERTEX_BUFFER_ID, _vertexBufferIDs.data(),
        static_cast<UINT>(_vertexBufferIDs.size()));

    _ownerScene->_frameResources[currentBackbufferIndex]->CopyStructuredBuffer(
        commandList, FrameResourceType::INDEX_BUFFER_ID, _indexBufferIDs.data(),
        static_cast<UINT>(_indexBufferIDs.size()));
}

void DXRDrawPass::WriteCommand(ID3D12GraphicsCommandList* cmdList)
{
    ComPtr<ID3D12GraphicsCommandList4> cmdList4;
    HRESULT                            hr = cmdList->QueryInterface(IID_PPV_ARGS(cmdList4.GetAddressOf()));
    FAILED_CHECK_MESSAGE(hr, L"DXRDrawPass::WriteCommand() failed to QueryInterface for ID3D12GraphicsCommandList4");
    _outputResourceUAV->TransitionResource(cmdList4.Get(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

    D3D12_DISPATCH_RAYS_DESC rayTraceDesc{};
    SIZE                     resolution = _outputResourceUAV->GetResolution();
    rayTraceDesc.Width                  = resolution.cx;
    rayTraceDesc.Height                 = resolution.cy;
    rayTraceDesc.Depth                  = 1; 

    // raygen 1개
    rayTraceDesc.RayGenerationShaderRecord.StartAddress =
        _shaderTable->GetGPUVirtualAddress() + 0 * _shaderTableEntrySize;
    rayTraceDesc.RayGenerationShaderRecord.SizeInBytes = _shaderTableEntrySize;

    // miss 2개
    const size_t missOffset                    = 1 * _shaderTableEntrySize;
    rayTraceDesc.MissShaderTable.StartAddress  = _shaderTable->GetGPUVirtualAddress() + missOffset;
    rayTraceDesc.MissShaderTable.SizeInBytes   = _shaderTableEntrySize * 2;
    rayTraceDesc.MissShaderTable.StrideInBytes = _shaderTableEntrySize * 1;

    // hit 1개
    const size_t hitOffset                   = 3 * _shaderTableEntrySize;
    rayTraceDesc.HitGroupTable.StartAddress  = _shaderTable->GetGPUVirtualAddress() + hitOffset;
    rayTraceDesc.HitGroupTable.SizeInBytes   = _shaderTableEntrySize;
    rayTraceDesc.HitGroupTable.StrideInBytes = _shaderTableEntrySize * 1;

    auto  cameraData    = _ownerScene->_RaycameraBuffer->GetGPUVirtualAddress();
    auto  lightData     = _ownerScene->_lightBuffer->GetGPUVirtualAddress();
    auto  instanceData  = _instanceDatasBuffer->GetGPUVirtualAddress();
    auto& frameResource = _ownerScene->_frameResources[_ownerScene->_currentFrameIndex];

    // bind
    cmdList4->SetComputeRootSignature(_globalRootsignature.Get());

    cmdList4->SetComputeRootConstantBufferView(0, cameraData);
    cmdList4->SetComputeRootConstantBufferView(1, lightData);
    cmdList4->SetComputeRoot32BitConstants(2, 3, &_ownerScene->_numLight, 0);
    frameResource->SetComputeFrameResource(FrameResourceType::VERTEX_BUFFER_ID, 3, cmdList4.Get());
    frameResource->SetComputeFrameResource(FrameResourceType::INDEX_BUFFER_ID, 4, cmdList4.Get());
    cmdList4->SetComputeRootShaderResourceView(5, instanceData);
    //frameResource->SetComputeFrameResource(FrameResourceType::MESH_INSTANCE_ID, 6, cmdList4.Get());

    cmdList4->SetPipelineState1(_pso.Get());
    cmdList4->DispatchRays(&rayTraceDesc);
    _meshRenderTarget->TransitionResource(cmdList4.Get(), D3D12_RESOURCE_STATE_COPY_DEST);
    _outputResourceUAV->TransitionResource(cmdList4.Get(), D3D12_RESOURCE_STATE_COPY_SOURCE);
    cmdList4->CopyResource(_meshRenderTarget->GetResource(), _outputResourceUAV->GetResource());
}
