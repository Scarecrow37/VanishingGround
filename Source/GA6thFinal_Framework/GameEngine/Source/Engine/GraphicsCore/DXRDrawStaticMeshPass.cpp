#include "pch.h"
#include "DXRDrawStaticMeshPass.h"
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

DXRDrawStaticMeshPass::~DXRDrawStaticMeshPass() {}

void DXRDrawStaticMeshPass::Initialize(RenderScene* ownerScene)
{
    __super::Initialize(ownerScene);
    CreateStateObject();
    CreateShaderResource();
}

void DXRDrawStaticMeshPass::Begin(ID3D12GraphicsCommandList* commandList)
{
    CreateShaderTable();
    UINT currentBackBufferIndex = _ownerScene->_currentFrameIndex;
    _ownerScene->_accelerationStructureManager->RemoveUnUsedStaticMeshes(_ownerScene->_staticMesh);
    UpdateStaticMeshVIBufferID(commandList);
}

void DXRDrawStaticMeshPass::Draw(ID3D12GraphicsCommandList* commandList)
{
    for (auto* renderer : _ownerScene->_staticMesh)
    {
        _ownerScene->_accelerationStructureManager->SubmitInstance(renderer);
    }

    _ownerScene->_accelerationStructureManager->EndFrame();
}

void DXRDrawStaticMeshPass::End(ID3D12GraphicsCommandList* commandList)
{
    UINT  currentBackBufferIndex = UmDevice.GetCurrentBackBufferIndex();
    auto  resource               = UmViewManager.GetShaderResourceHeap()->GetGPUDescriptorHandleForHeapStart();
    auto  cameraData             = _ownerScene->_cameraBuffer->GetGPUVirtualAddress();
    auto& frameResource          = _ownerScene->_frameResources[currentBackBufferIndex];
    WriteCommand();
}
//
// void DXRDrawStaticMeshPass::CreateStateObject()
//{
//    std::array<D3D12_STATE_SUBOBJECT, 12> subobjects{};
//    uint32_t                              index = 0;
//
//    DxilLibrary dxilLibrary = RTPipeline::CreateDxilLibrary();
//    subobjects[index++]     = dxilLibrary.stateSubObject; // 0
//
//    HitProgram hitProgram(nullptr, RTPipeline::ClosestHitShader, RTPipeline::HitGroup);
//    subobjects[index++] = hitProgram.subObject; // 1
//
//    LocalRootSignature rgsRootSignature(RTPipeline::CreateRayGenRootDesc().desc);
//    subobjects[index] = rgsRootSignature.subObject; // 2
//
//    const uint32_t    rgsRootIndex = index++;
//    ExportAssociation rgsRootAssociation(&RTPipeline::RayGenShader, 1, &(subobjects[rgsRootIndex]));
//    subobjects[index++] = rgsRootAssociation.subObject; // 3
//
//    LocalRootSignature hitRootSignature(RTPipeline::CreateHitRootDesc().desc);
//    subobjects[index] = hitRootSignature.subObject; // 4
//
//    const uint32_t    hitRootIndex = index++;
//    ExportAssociation hitRootAssociation(&RTPipeline::ClosestHitShader, 1, &(subobjects[hitRootIndex]));
//    subobjects[index++] = hitRootAssociation.subObject; // 5
//
//    LocalRootSignature missRootSignature(RTPipeline::CreateMissRootDesc().desc);
//    subobjects[index] = missRootSignature.subObject; // 6
//
//    const uint32_t    missRootIndex = index++;
//    ExportAssociation missRootAssociation(&RTPipeline::MissShader, 1, &(subobjects[missRootIndex]));
//    subobjects[index++] = missRootAssociation.subObject; // 7
//
//    // payload size float4 + uint + float3
//    ShaderConfig shaderConfig(sizeof(float) * 2, sizeof(float) * (4 + 1));
//    subobjects[index] = shaderConfig.subObject; // 8
//
//    const uint32_t shaderConfigIndex = index++;
//    const WCHAR*   shaderExports[]   = {RTPipeline::MissShader, RTPipeline::ClosestHitShader,
//    RTPipeline::RayGenShader}; ExportAssociation configAssociation(shaderExports, _countof(shaderExports),
//    &(subobjects[shaderConfigIndex])); subobjects[index++] = configAssociation.subObject; // 9
//
//    PipelineConfig config(7 + 1);
//    subobjects[index++] = config.subObject; // 10
//
//    GlobalRootSignature root(RTPipeline::CreateGlobalRootDesc().desc);
//    _globalRootsignature = root.rootSignature;
//    subobjects[index++]  = root.subObject; // 11z
//
//    D3D12_STATE_OBJECT_DESC desc;
//    desc.NumSubobjects = index;
//    desc.pSubobjects   = subobjects.data();
//    desc.Type          = D3D12_STATE_OBJECT_TYPE_RAYTRACING_PIPELINE;
//
//    ComPtr<ID3D12Device5> device5 = UmDevice.GetDevice5();
//    HRESULT               hr      = device5->CreateStateObject(&desc, IID_PPV_ARGS(_pso.GetAddressOf()));
//    FAILED_CHECK_MESSAGE(hr, L"DXRDrawStaticMeshPass::CreateStateObject() failed Create RT Pipeline stateObject ");
//}

void DXRDrawStaticMeshPass::CreateStateObject()
{
    std::array<D3D12_STATE_SUBOBJECT, 15> subobjects{};
    uint32_t                              index = 0;
    // Subobject 0: DXIL Library (모든 셰이더 코드를 포함)
    DxilLibrary dxilLibrary = RTPipeline::CreateDxilLibrary();
    subobjects[index++]     = dxilLibrary.stateSubObject; // index = 1

    // Subobject 1: Primary Hit Group (ClosestHit)
    HitProgram hitProgram(nullptr, RTPipeline::ClosestHitShader, RTPipeline::HitGroup);
    subobjects[index++] = hitProgram.subObject; // index = 2

    // Subobject 2: Shadow Hit Group (AnyHit)
    HitProgram shadowHitProgram(nullptr, nullptr, RTPipeline::ShadowHitGroup);
    shadowHitProgram.desc.AnyHitShaderImport = RTPipeline::ShadowAnyHitShader;
    subobjects[index++] =
        shadowHitProgram
            .subObject; // index = 3

    // Subobject 3, 4: RayGen 셰이더를 위한 Local Root Signature (LRS) 와 그 연결(Association)
    LocalRootSignature rgsRootSignature(RTPipeline::CreateRayGenRootDesc().desc);
    subobjects[index]              = rgsRootSignature.subObject;
    const uint32_t    rgsRootIndex = index++; // rgsRootIndex = 3, index = 4
    ExportAssociation rgsRootAssociation(&RTPipeline::RayGenShader, 1, &(subobjects[rgsRootIndex]));
    subobjects[index++] = rgsRootAssociation.subObject; // index = 5

    // Subobject 5, 6: Primary Hit Group을 위한 LRS 와 그 연결
    LocalRootSignature hitRootSignature(RTPipeline::CreateHitRootDesc().desc);
    subobjects[index]              = hitRootSignature.subObject;
    const uint32_t    hitRootIndex = index++; // hitRootIndex = 5, index = 6
    ExportAssociation hitRootAssociation(&RTPipeline::ClosestHitShader, 1, &(subobjects[hitRootIndex]));
    subobjects[index++] = hitRootAssociation.subObject; // index = 7

    // Subobject 7, 8: Shadow Hit Group을 위한 LRS 와 그 연결
    // (파라미터가 없으므로 비어있는 LRS 사용)
    D3D12_ROOT_SIGNATURE_DESC emptyDesc = {0, nullptr, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_LOCAL_ROOT_SIGNATURE};
    LocalRootSignature        shadowHitRootSignature(emptyDesc);
    subobjects[index]                    = shadowHitRootSignature.subObject;
    const uint32_t    shadowHitRootIndex = index++; // shadowHitRootIndex = 7, index = 8
    ExportAssociation shadowHitRootAssociation(&RTPipeline::ShadowAnyHitShader, 1, &(subobjects[shadowHitRootIndex]));
    subobjects[index++] =
        shadowHitRootAssociation.subObject; // index = 9

    // Subobject 9, 10: Miss 셰이더들(Primary, Shadow)을 위한 LRS 와 그 연결
    LocalRootSignature missRootSignature(RTPipeline::CreateMissRootDesc().desc);
    subobjects[index]               = missRootSignature.subObject;
    const uint32_t    missRootIndex = index++; // missRootIndex = 9, index = 10
    const WCHAR*      missShaders[] = {RTPipeline::MissShader, RTPipeline::ShadowMissShader};
    ExportAssociation missRootAssociation(missShaders, _countof(missShaders), &(subobjects[missRootIndex]));
    subobjects[index++] = missRootAssociation.subObject; // index = 11

    // Subobject 11, 12: 셰이더 Payload 설정과 그 연결
    ShaderConfig shaderConfig(sizeof(float) * 2, sizeof(float) * (4 + 1)); // MaxAttributeSize, MaxPayloadSize
    subobjects[index]                   = shaderConfig.subObject;
    const uint32_t    shaderConfigIndex = index++; // shaderConfigIndex = 11, index = 12
    const WCHAR*      shaderExports[] = {RTPipeline::RayGenShader, RTPipeline::MissShader, RTPipeline::ClosestHitShader,
                                         RTPipeline::ShadowMissShader, RTPipeline::ShadowAnyHitShader};
    ExportAssociation configAssociation(shaderExports, _countof(shaderExports), &(subobjects[shaderConfigIndex]));
    subobjects[index++] = configAssociation.subObject; // index = 13

    // Subobject 13: 파이프라인 전체 설정
    PipelineConfig config(1);                          // 최대 재귀 깊이 설정 (HLSL과 일치시켜야 함)
    subobjects[index++] = config.subObject;            // index = 14

    // Subobject 14: Global Root Signature
    GlobalRootSignature root(RTPipeline::CreateGlobalRootDesc().desc);
    _globalRootsignature = root.rootSignature;
    subobjects[index++]  = root.subObject; // index = 15
                                           // 최종 State Object 생성
    D3D12_STATE_OBJECT_DESC desc;
    desc.NumSubobjects            = index; // 최종 index 값은 15
    desc.pSubobjects              = subobjects.data();
    desc.Type                     = D3D12_STATE_OBJECT_TYPE_RAYTRACING_PIPELINE;
    ComPtr<ID3D12Device5> device5 = UmDevice.GetDevice5();
    HRESULT               hr      = device5->CreateStateObject(&desc, IID_PPV_ARGS(_pso.GetAddressOf()));
    FAILED_CHECK_MESSAGE(hr, L"DXRDrawStaticMeshPass::CreateStateObject() failed Create RT Pipeline stateObject ");
}

void DXRDrawStaticMeshPass::CreateShaderTable()
{
    // 0) 공통 상수 정의
    constexpr UINT bytesId   = D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES;         // 32
    constexpr UINT bytesArgs = sizeof(D3D12_GPU_DESCRIPTOR_HANDLE);           //  8
    constexpr UINT aligeRec  = D3D12_RAYTRACING_SHADER_RECORD_BYTE_ALIGNMENT; // 32

    const UINT raygenSize = d3dUtil::AlignTo(bytesId + 2 * bytesArgs, 32);
    const UINT missSize   = d3dUtil::AlignTo(bytesId + bytesArgs, 32);
    const UINT hitSize    = d3dUtil::AlignTo(bytesId + 6 * bytesArgs, 32);
    _shaderTableEntrySize =
        d3dUtil::AlignTo(std::max({raygenSize, missSize, hitSize}), D3D12_RAYTRACING_SHADER_TABLE_BYTE_ALIGNMENT);

    const UINT recordCount = 5;
    const UINT tableSize   = _shaderTableEntrySize * recordCount;
    // 1) 업로드 버퍼 생성
    if (!_init)
    {
        UmDevice.CreateUploadBuffer(tableSize, D3D12_RESOURCE_FLAG_NONE, D3D12_RESOURCE_STATE_GENERIC_READ,
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
    const void* ID_SHADOW_HIT  = props->GetShaderIdentifier(RTPipeline::ShadowHitGroup);
    // 3) Shader Table 레코드 작성
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
        // SRV t4 evnTexture
        *reinterpret_cast<UINT64*>(p + bytesId + (1 * bytesArgs)) = _ownerScene->_skyBox->GetCubeMapSRV().ptr;
        // SRV t5 Vertices
        *reinterpret_cast<UINT64*>(p + bytesId + (2 * bytesArgs)) = UmViewManager.GetVertexBufferSrvPtr();
        // SRV t2005 Indices
        *reinterpret_cast<UINT64*>(p + bytesId + (3 * bytesArgs)) = UmViewManager.GetIndexBufferSrvPtr();
        // SRV t4005~ textures
        *reinterpret_cast<UINT64*>(p + bytesId + (4 * bytesArgs)) =
            UmViewManager.GetShaderResourceHeap()->GetGPUDescriptorHandleForHeapStart().ptr;
    }
    /* ── 4‑5) Hit Group ─────────────────────────────────────────────── */
    {
        p += _shaderTableEntrySize;
        memcpy(p, ID_SHADOW_HIT, bytesId); // Shadow Hit (파라미터 없음)
    }
    _shaderTable->Unmap(0, nullptr);
}

void DXRDrawStaticMeshPass::CreateShaderResource()
{
    ID3D12GraphicsCommandList* cmdlist = UmDevice.GetCommandList();
    _outputResourceUAV                 = MakeSharedResource<UnorderedAccessView>();
    DXGI_MODE_DESC mode                = UmDevice.GetMode();
    mode.Format                        = DXGI_FORMAT_R32G32B32A32_FLOAT;
    _outputResourceUAV->Initialize(mode);
    UmDXResourceManager.AddResource(_outputResourceUAV);
    _outputResourceUAV->TransitionResource(cmdlist, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
}

void DXRDrawStaticMeshPass::UpdateStaticMeshVIBufferID(ID3D12GraphicsCommandList* commandList)
{
    _vertexBufferIDs.clear();
    _indexBufferIDs.clear();
    for (auto& component : _ownerScene->_staticMesh)
    {
        const auto& model  = component->GetModel();
        const auto& meshes = model->GetMeshes();

        UINT size = static_cast<UINT>(meshes.size());
        for (UINT i = 0; i < size; ++i)
        {
            const auto& vibuffer       = meshes[i]->GetVIBuffer();
            UINT        vertexBufferID = vibuffer->_vertexBufferID;
            UINT        indexbufferID  = vibuffer->_indexBufferID;

            _vertexBufferIDs.push_back(vertexBufferID);
            _indexBufferIDs.push_back(indexbufferID);
        }
    }
    UINT currentFrameIndex = _ownerScene->_currentFrameIndex;
    _ownerScene->_frameResources[currentFrameIndex]->CopyStructuredBuffer(
        commandList, FrameResourceType::VERTEX_BUFFER_ID, _vertexBufferIDs.data(),
        static_cast<UINT>(_vertexBufferIDs.size()));

    _ownerScene->_frameResources[currentFrameIndex]->CopyStructuredBuffer(
        commandList, FrameResourceType::INDEX_BUFFER_ID, _indexBufferIDs.data(),
        static_cast<UINT>(_indexBufferIDs.size()));
}

void DXRDrawStaticMeshPass::WriteCommand()
{
    ComPtr<ID3D12GraphicsCommandList4> cmdList4 = UmDevice.GetCommandList4();
    _outputResourceUAV->TransitionResource(cmdList4.Get(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

    D3D12_DISPATCH_RAYS_DESC rayTraceDesc{};
    DXGI_MODE_DESC           mode = _outputResourceUAV->GetMode();
    rayTraceDesc.Width            = mode.Width;
    rayTraceDesc.Height           = mode.Height;
    rayTraceDesc.Depth            = 1;

    // raygen 1개
    rayTraceDesc.RayGenerationShaderRecord.StartAddress =
        _shaderTable->GetGPUVirtualAddress() + 0 * _shaderTableEntrySize;
    rayTraceDesc.RayGenerationShaderRecord.SizeInBytes = _shaderTableEntrySize;

    // miss 1개
    const size_t missOffset                    = 1 * _shaderTableEntrySize;
    rayTraceDesc.MissShaderTable.StartAddress  = _shaderTable->GetGPUVirtualAddress() + missOffset;
    rayTraceDesc.MissShaderTable.SizeInBytes   = _shaderTableEntrySize;
    rayTraceDesc.MissShaderTable.StrideInBytes = _shaderTableEntrySize * 1;

    // hit 1개
    const size_t hitOffset                   = 2 * _shaderTableEntrySize;
    rayTraceDesc.HitGroupTable.StartAddress  = _shaderTable->GetGPUVirtualAddress() + hitOffset;
    rayTraceDesc.HitGroupTable.SizeInBytes   = _shaderTableEntrySize;
    rayTraceDesc.HitGroupTable.StrideInBytes = _shaderTableEntrySize * 1;

    auto  cameraData    = _ownerScene->_RaycameraBuffer->GetGPUVirtualAddress();
    auto  lightData     = _ownerScene->_lightBuffer->GetGPUVirtualAddress();
    auto& frameResource = _ownerScene->_frameResources[_ownerScene->_currentFrameIndex];

    // bind
    cmdList4->SetComputeRootSignature(_globalRootsignature.Get());

    cmdList4->SetComputeRootConstantBufferView(0, cameraData);
    cmdList4->SetComputeRootConstantBufferView(1, lightData);
    cmdList4->SetComputeRoot32BitConstants(2, 3, &_ownerScene->_numLight, 0);
    frameResource->SetRayTracingFrameResource(FrameResourceType::VERTEX_BUFFER_ID, 3, cmdList4.Get());
    frameResource->SetRayTracingFrameResource(FrameResourceType::INDEX_BUFFER_ID, 4, cmdList4.Get());
    frameResource->SetRayTracingFrameResource(FrameResourceType::MATERIAL, 5, cmdList4.Get());
    frameResource->SetRayTracingFrameResource(FrameResourceType::STATIC_MESH_INSTANCE_ID, 6, cmdList4.Get());

    cmdList4->SetPipelineState1(_pso.Get());
    cmdList4->DispatchRays(&rayTraceDesc);
    _meshRenderTarget->TransitionResource(cmdList4.Get(), D3D12_RESOURCE_STATE_COPY_DEST);
    _outputResourceUAV->TransitionResource(cmdList4.Get(), D3D12_RESOURCE_STATE_COPY_SOURCE);
    cmdList4->CopyResource(_meshRenderTarget->GetResource(), _outputResourceUAV->GetResource());
}
