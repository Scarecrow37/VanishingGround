#include "pch.h"
#include "DXRDrawStaticMeshPass.h"
#include "RenderTarget.h"
#include "RenderScene.h"
#include "RTPipeline.h"
#include "HitProgram.h"
#include "LocalRootSignature.h"
#include "ExportAssociation.h"
#include "ShaderConfig.h"
#include "PipelineConfig.h"
#include "GlobalRootSignature.h"
#include "MeshRenderer.h"
#include "Model.h"

DXRDrawStaticMeshPass::~DXRDrawStaticMeshPass() {}

void DXRDrawStaticMeshPass::Initialize() 
{
    __super::Initialize();
    CreateStateObject();
}

void DXRDrawStaticMeshPass::Begin(ID3D12GraphicsCommandList* commandList) 
{
    UmAccelerationStructureManager.RemoveUnUsedStaticMeshes(_ownerScene->_staticMesh);
    
    // statice mesh의 descriptor heap에서의 texture id 상수버퍼 값 갱신을 위한 분류
    for (auto& componenet : _ownerScene->_staticMesh)
    {
        const auto& model  = componenet->GetModel();
        const auto& meshes = model->GetMeshes();
        const auto& textures = model->GetTextures();
        UINT        size   = static_cast<UINT>(meshes.size());
        
        for (UINT i = 0; i < size; ++i)
        {
            MaterialID MaterialID{};
            
            for (UINT j = 0; j < 4; ++j)
            {
                MaterialID.ID[j] = textures[i][j]->GetID();
            }
            _staticMeshMaterialID.push_back(MaterialID);
        }
    }
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
    std::array<D3D12_STATE_SUBOBJECT, 11> subobjects{};
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

    D3D12_ROOT_SIGNATURE_DESC emptyDesc{};
    emptyDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_LOCAL_ROOT_SIGNATURE;
    LocalRootSignature missRootSignature(emptyDesc);
    subobjects[index] = missRootSignature.subObject; // 6

    // payload size float4+uint
    ShaderConfig shaderConfig(sizeof(float) * 2, sizeof(float) * (4 + 1));
    subobjects[index] = shaderConfig.subObject; // 7

    const uint32_t    shaderConfigIndex = index++;
    const WCHAR*      shaderExports[] = {RTPipeline::MissShader, RTPipeline::ClosestHitShader, RTPipeline::RayGenShader};
    ExportAssociation configAssociation(shaderExports, _countof(shaderExports), &(subobjects[shaderConfigIndex]));
    subobjects[index++] = configAssociation.subObject; // 8

    PipelineConfig config(7 + 1);
    subobjects[index++] = config.subObject; // 9

    GlobalRootSignature root({});
    _emptyRootsignature = root.rootSignature;
    subobjects[index++] = root.subObject; // 10

    D3D12_STATE_OBJECT_DESC desc;
    desc.NumSubobjects = index;
    desc.pSubobjects   = subobjects.data();
    desc.Type          = D3D12_STATE_OBJECT_TYPE_RAYTRACING_PIPELINE;

    ComPtr<ID3D12Device5> device5 = UmDevice.GetDevice5();
    HRESULT               hr      = device5->CreateStateObject(&desc, IID_PPV_ARGS(_pipelineState.GetAddressOf()));
    FAILED_CHECK_MESSAGE(hr, L"DXRDrawStaticMeshPass::CreateHitRootDesc() failed Create RT Pipeline stateObject ");
}

D3D12_ROOT_SIGNATURE_DESC DXRDrawStaticMeshPass::CreateRayGenRootDest()
{
    static CD3DX12_DESCRIPTOR_RANGE ranges[5];
    ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 0);     // Output(u0)
    ranges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 5, 0);     // t0~t4
    ranges[2].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 4, 5);     // t5~t8 (Texture2D[])
    ranges[3].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 5, 0); // b0~b4
    ranges[4].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, 2, 0); // s0~s1

    static CD3DX12_ROOT_PARAMETER rootParams[5];
    rootParams[0].InitAsDescriptorTable(1, &ranges[0]);
    rootParams[1].InitAsDescriptorTable(1, &ranges[1]);
    rootParams[2].InitAsDescriptorTable(1, &ranges[2]);
    rootParams[3].InitAsDescriptorTable(1, &ranges[3]);
    rootParams[4].InitAsDescriptorTable(1, &ranges[4]);

    D3D12_ROOT_SIGNATURE_DESC desc = {};
    desc.NumParameters             = _countof(rootParams);
    desc.pParameters               = rootParams;
    desc.NumStaticSamplers         = 0;
    desc.pStaticSamplers           = nullptr;
    desc.Flags                     = D3D12_ROOT_SIGNATURE_FLAG_LOCAL_ROOT_SIGNATURE; // DXR에서는 Local로

    return desc;
}

// ClosestHit 루트 시그니처는 RayGen과 동일하게 사용 가능하므로
// 별도로 분리할 필요 없고 동일 함수 호출 권장.
D3D12_ROOT_SIGNATURE_DESC DXRDrawStaticMeshPass::CreateHitRootDesc()
{
    return CreateRayGenRootDest();
}