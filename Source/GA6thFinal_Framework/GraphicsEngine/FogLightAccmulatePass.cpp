#include "pch.h"
#include "FogLightAccmulatePass.h"
#include "VolumetricFogTechnique.h"
#include "d3dUtil.h"

FogLightAccmulatePass::~FogLightAccmulatePass() = default;

void FogLightAccmulatePass::Initialize(RenderScene* ownerScene, RenderTechnique* ownerTechnique, ID3D12GraphicsCommandList* commandList)
{
    RenderPass::Initialize(ownerScene, ownerTechnique, commandList);
    InitShaderAndPSO();
    _volumTech = dynamic_cast<VolumetricFogTechnique*>(ownerTechnique);
}

void FogLightAccmulatePass::Begin(ID3D12GraphicsCommandList* commandList) 
{
    commandList->SetPipelineState(_pipelineState.Get());
    commandList->SetComputeRootSignature(_fx.GetRootSignature());
}

void FogLightAccmulatePass::Draw(ID3D12GraphicsCommandList* commandList) 
{
    int                       readIndex          = _volumTech->_readIndex ? 0 : 1;
    auto                      injectionTex       = _volumTech->_tempVoxelInjectionTexture3D[readIndex];
    auto                      finalaccumulateTex = _volumTech->_finalVoxelAccumulationTexture3D;
    D3D12_GPU_VIRTUAL_ADDRESS fogData            = _volumTech->GetConstantBufferView()->GetGPUVirtualAddress();

    commandList->SetComputeRootConstantBufferView(_fx.GetRootParameterIndex("fogdata"), fogData);
    commandList->SetComputeRootDescriptorTable(_fx.GetRootParameterIndex("VoxelReadTexture"), injectionTex->GetSRVHandle());
    commandList->SetComputeRootDescriptorTable(_fx.GetRootParameterIndex("VoxelWriteTexture"), finalaccumulateTex->GetUAVHandle());
    commandList->Dispatch(d3dUtil::Ceil(VOXEL_VOLUME_SIZEX , 8), d3dUtil::Ceil(VOXEL_VOLUME_SIZEY ,8), 1);
}

void FogLightAccmulatePass::InitShaderAndPSO()
{
    ComputePipelineStateStream pss;
    _fx.SetPipelineStateStream(pss);
    _pipelineState = Global::pipelineStateManager->GetPipelineState(pss);
}