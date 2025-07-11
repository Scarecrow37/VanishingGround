#include "pch.h"
#include "DXRDrawStaticMeshPass.h"
#include "ExportAssociation.h"
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
#include "d3dUtil.h"
#include "SkyBox.h"
#include "FrameResource.h"

DXRDrawStaticMeshPass::~DXRDrawStaticMeshPass() {}

void DXRDrawStaticMeshPass::Initialize(RenderScene* ownerScene)
{
    __super::Initialize(ownerScene);
    CreateStateObject();
    CreateShaderResource();
    CreateShaderTable();
}

void DXRDrawStaticMeshPass::Begin(ID3D12GraphicsCommandList* commandList)
{
    UINT currentBackBufferIndex = _ownerScene->_currentFrameIndex;
    UmAccelerationStructureManager.RemoveUnUsedStaticMeshes(_ownerScene->_staticMesh);
    UpdateStaticMeshVIBufferID(commandList);
}

void DXRDrawStaticMeshPass::Draw(ID3D12GraphicsCommandList* commandList)
{
    for (auto* renderer : _ownerScene->_staticMesh)
    {
        UmAccelerationStructureManager.SubmitInstance(renderer);
    }

    UmAccelerationStructureManager.EndFrame();
}

void DXRDrawStaticMeshPass::End(ID3D12GraphicsCommandList* commandList)
{
    UINT  currentBackBufferIndex = UmDevice.GetCurrentBackBufferIndex();
    auto  resource               = UmViewManager.GetShaderResourceHeap()->GetGPUDescriptorHandleForHeapStart();
    auto  cameraData             = _ownerScene->_cameraBuffer->GetGPUVirtualAddress();
    auto& frameResource          = _ownerScene->_frameResources[currentBackBufferIndex];
    _meshRenderTarget->TransitionResource(commandList, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
}

void DXRDrawStaticMeshPass::CreateStateObject()
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

    const uint32_t    missRootIndex = index++;
    ExportAssociation missRootAssociation(&RTPipeline::MissShader, 1, &(subobjects[missRootIndex]));
    subobjects[index++] = missRootAssociation.subObject; // 7

    // payload size float4+uint
    ShaderConfig shaderConfig(sizeof(float) * 2, sizeof(float) * (4 + 1));
    subobjects[index] = shaderConfig.subObject; // 8

    const uint32_t shaderConfigIndex = index++;
    const WCHAR*   shaderExports[]   = {RTPipeline::MissShader, RTPipeline::ClosestHitShader, RTPipeline::RayGenShader};
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

    ComPtr<ID3D12Device5> device5 = UmDevice.GetDevice5();
    HRESULT               hr      = device5->CreateStateObject(&desc, IID_PPV_ARGS(_pso.GetAddressOf()));
    FAILED_CHECK_MESSAGE(hr, L"DXRDrawStaticMeshPass::CreateHitRootDesc() failed Create RT Pipeline stateObject ");
}

void DXRDrawStaticMeshPass::CreateShaderTable()
{
    // 0) 공통 상수 정의
    constexpr UINT bytesId  = D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES;         // 32
    constexpr UINT bytesArgs = sizeof(D3D12_GPU_DESCRIPTOR_HANDLE);           //  8
    constexpr UINT aligeRec = D3D12_RAYTRACING_SHADER_RECORD_BYTE_ALIGNMENT; // 32

    const UINT raygenSize = d3dUtil::AlignTo(bytesId + 3 * bytesArgs, aligeRec);
    const UINT missSize = d3dUtil::AlignTo(bytesId +  bytesArgs, aligeRec);
    const UINT hitSize    = bytesId + 12 * bytesArgs;
    _shaderTableEntrySize = std::max({raygenSize, missSize, hitSize});
    const UINT recordCount = 3;
    const UINT tableSize   = _shaderTableEntrySize * recordCount;
    // 1) 업로드 버퍼 생성
    UmDevice.CreateUploadBuffer(tableSize, D3D12_RESOURCE_FLAG_NONE, D3D12_RESOURCE_STATE_GENERIC_READ, _shaderTable);
    // 2) Shader Identifier 가져오기
    ComPtr<ID3D12StateObjectProperties> props;
    _pso->QueryInterface(IID_PPV_ARGS(props.GetAddressOf()));

    const void* ID_RGS  = props->GetShaderIdentifier(RTPipeline::RayGenShader);
    const void* ID_MISS = props->GetShaderIdentifier(RTPipeline::MissShader);
    const void* ID_HIT  = props->GetShaderIdentifier(RTPipeline::HitGroup);
    // 3) Shader Table 레코드 작성
    uint8_t* p = nullptr;
    _shaderTable->Map(0, nullptr, reinterpret_cast<void**>(&p));
    /* ── 4‑1) Ray Generation ────────────────────────────────────────── */
    {
        memcpy(p, ID_RGS, bytesId);
        // RootParam#1 : SRV t0 테이블 핸들
        *reinterpret_cast<UINT64*>(p + bytesId) = UmAccelerationStructureManager.GetTopLevelSRV().GPU.ptr;
        // RootParam#1 : SRV t0 테이블 핸들
        *reinterpret_cast<UINT64*>(p + bytesId + bytesArgs) = _outputResourceUAV->GetUAVHandle().ptr;
    }
    /* ── 4‑2) Miss Generation ────────────────────────────────────────── */
    p += _shaderTableEntrySize;
    {
        memcpy(p, ID_MISS, bytesId);
        // RootParam#0 : envTexture(t4) 테이블 핸들
        *reinterpret_cast<UINT64*>(p + bytesId) = _ownerScene->_skyBox->GetCubeMapSRV().ptr;
    }
    /* ── 4‑3) Hit Group ─────────────────────────────────────────────── */
    p += _shaderTableEntrySize;
    {
        memcpy(p, ID_HIT, bytesId);
        uint8_t* a = p + bytesId;
        // SRV t0 rtScene
        *reinterpret_cast<UINT64*>(a) = UmAccelerationStructureManager.GetTopLevelSRV().GPU.ptr;
        a += bytesArgs; 
        // SRV t4 evnTexture
        *reinterpret_cast<UINT64*>(a) = _ownerScene->_skyBox->GetCubeMapSRV().ptr;
        a += bytesArgs; 
        // SRV t5 Vertices
        *reinterpret_cast<UINT64*>(a) = UmViewManager.GetVertexBufferSrvPtr();
        a += bytesArgs; 
        // SRV t2005 Indices
        *reinterpret_cast<UINT64*>(a) = UmViewManager.GetIndexBufferSrvPtr();
        a += bytesArgs;
        // SRV t4005~ textures
        *reinterpret_cast<UINT64*>(a) = UmViewManager.GetShaderResourceHeap()->GetGPUDescriptorHandleForHeapStart().ptr;
    }

    _shaderTable->Unmap(0, nullptr);
}

void DXRDrawStaticMeshPass::CreateShaderResource()
{
    _outputResourceUAV = MakeSharedResource<UnorderedAccessView>();
    DXGI_MODE_DESC mode = UmDevice.GetMode();
    mode.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    _outputResourceUAV->Initialize(mode);
    UmDXResourceManager.AddResource(_outputResourceUAV);
}

void DXRDrawStaticMeshPass::UpdateStaticMeshVIBufferID(ID3D12GraphicsCommandList* commandList)
{
    _vertexBufferIDs.clear();
    _indexBufferIDs.clear();
    for (auto& component : _ownerScene->_staticMesh)
    {
        const auto& model = component->GetModel();
        const auto& meshes = model->GetMeshes();
        
        UINT size = static_cast<UINT>(meshes.size());
        for (UINT i = 0; i < size; ++i)
        {
            const auto& vibuffer = meshes[i]->GetVIBuffer();
            UINT        vertexBufferID = vibuffer->_vertexBufferID;
            UINT        indexbufferID = vibuffer->_indexBufferID;

            _vertexBufferIDs.push_back(vertexBufferID);
            _indexBufferIDs.push_back(indexbufferID);
        }
    }
    UINT currentFrameIndex = _ownerScene->_currentFrameIndex;
    _ownerScene->_frameResources[currentFrameIndex]->CopyStructuredBuffer(
        commandList, 
        FrameResourceType::VERTEX_BUFFER_ID, 
        _vertexBufferIDs.data(),
        static_cast<UINT>(_vertexBufferIDs.size()));

    _ownerScene->_frameResources[currentFrameIndex]->CopyStructuredBuffer(
        commandList, 
        FrameResourceType::INDEX_BUFFER_ID, 
        _indexBufferIDs.data(),
        static_cast<UINT>(_indexBufferIDs.size()));
}

void DXRDrawStaticMeshPass::WriteCommand()
{
    ComPtr<ID3D12GraphicsCommandList4> cmdList4 = UmDevice.GetCommandList4();
    
    D3D12_DISPATCH_RAYS_DESC rayTraceDesc{};
    DXGI_MODE_DESC           mode = _outputResourceUAV->GetMode();
    rayTraceDesc.Width            = mode.Width;
    rayTraceDesc.Height           = mode.Height;
    rayTraceDesc.Depth            = 1;

    // raygen 1개
    rayTraceDesc.RayGenerationShaderRecord.StartAddress =
        _shaderTable->GetGPUVirtualAddress() + 0 * _shaderTableEntrySize;
    rayTraceDesc.RayGenerationShaderRecord.SizeInBytes = _shaderTableEntrySize;
    rayTraceDesc.RayGenerationShaderRecord.StartAddress = _shaderTableEntrySize * 1;

    // miss 1개
    const size_t missOffset = 1 * _shaderTableEntrySize;
    rayTraceDesc.MissShaderTable.StartAddress = _shaderTable->GetGPUVirtualAddress() + missOffset;
    rayTraceDesc.MissShaderTable.SizeInBytes   = _shaderTableEntrySize;
    rayTraceDesc.MissShaderTable.StrideInBytes = _shaderTableEntrySize * 1;

    // hit 1개
    const size_t hitOffset = 2 * _shaderTableEntrySize;
    rayTraceDesc.HitGroupTable.StartAddress = _shaderTable->GetGPUVirtualAddress() + hitOffset;
    rayTraceDesc.HitGroupTable.SizeInBytes  = _shaderTableEntrySize;
    rayTraceDesc.HitGroupTable.StrideInBytes = _shaderTableEntrySize * 1;

    auto cameraData = _ownerScene->_cameraBuffer->GetGPUVirtualAddress();
    auto lightData  = _ownerScene->_lightBuffer->GetGPUVirtualAddress();

    //bind
    cmdList4->SetComputeRootSignature(_globalRootsignature.Get());
    cmdList4->SetComputeRootConstantBufferView(0, cameraData);
    cmdList4->SetComputeRootConstantBufferView(1, lightData);
    cmdList4->SetComputeRoot32BitConstants(2, 3, &_ownerScene->_numLight, 0);
    
}

