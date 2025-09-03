#include "pch.h"
#include "FogLightAccmulatePass.h"
#include "VolumetricFogTechnique.h"
#include "d3dUtil.h"
#include "ShaderBuilder.h"

FogLightAccmulatePass::~FogLightAccmulatePass() {}

void FogLightAccmulatePass::Initialize(RenderScene* ownerScene, RenderTechnique* ownerTechnique,
                                       ID3D12GraphicsCommandList* commandList)
{
    __super::Initialize(ownerScene, ownerTechnique, commandList);
    InitShaderAndPSO();
    _volumTech = dynamic_cast<VolumetricFogTechnique*>(ownerTechnique);
}

void FogLightAccmulatePass::Update(ID3D12GraphicsCommandList* commandList) {}

void FogLightAccmulatePass::Begin(ID3D12GraphicsCommandList* commandList) 
{
    commandList->SetPipelineState(_pso.Get());
    commandList->SetComputeRootSignature(_shader->GetRootSignature());
}

void FogLightAccmulatePass::Draw(ID3D12GraphicsCommandList* commandList) 
{
    int                       readIndex    = _volumTech->_readIndex ? 0 : 1;
    auto injectionTex = _volumTech->_tempVoxelInjectionTexture3D[readIndex];
    auto finalaccumulateTex = _volumTech->_finalVoxelAccumulationTexture3D;
    D3D12_GPU_VIRTUAL_ADDRESS fogData      = _volumTech->GetConstantBufferView()->GetGPUVirtualAddress();
    commandList->SetComputeRootConstantBufferView(_shader->GetRootParameterIndex("fogdata"), fogData);
    commandList->SetComputeRootDescriptorTable(_shader->GetRootParameterIndex("VoxelReadTexture"),
                                               injectionTex->GetSRVHandle());
    commandList->SetComputeRootDescriptorTable(_shader->GetRootParameterIndex("VoxelWriteTexture"),
                                               finalaccumulateTex->GetUAVHandle());
    commandList->Dispatch(d3dUtil::Ceil(VOXEL_VOLUME_SIZEX , 8), d3dUtil::Ceil(VOXEL_VOLUME_SIZEY ,8), 1);
}

void FogLightAccmulatePass::End(ID3D12GraphicsCommandList* commandList) {}

void FogLightAccmulatePass::AddRenderPassDatas(std::string_view sceneName) {}

void FogLightAccmulatePass::InitShaderAndPSO()
{
    _shader = std::make_unique<ShaderBuilder>();
    _shader->BeginBuild();
    _shader->SetShader(L"../Shaders/cs_light_accumulation.hlsl", ShaderBuilder::Type::CS);
    _shader->EndBuild();

    D3D12_COMPUTE_PIPELINE_STATE_DESC psoDesc{};
    psoDesc.pRootSignature = _shader->GetRootSignature();
    psoDesc.CS             = _shader->GetShaderByteCode(ShaderBuilder::Type::CS);
    psoDesc.Flags          = D3D12_PIPELINE_STATE_FLAG_NONE;

    HRESULT hr = Global::device->GetDevice()->CreateComputePipelineState(&psoDesc, IID_PPV_ARGS(_pso.GetAddressOf()));
    FAILED_CHECK_MESSAGE(hr, L"FogLightAccmulatePass::InitShader CreateComputePipelineState failed");
}