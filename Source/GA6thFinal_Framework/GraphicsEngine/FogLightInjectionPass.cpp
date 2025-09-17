#include "pch.h"
#include "FogLightInjectionPass.h"
#include "ShaderBuilder.h"
#include "PBRLitTechnique.h"
#include "VolumetricFogTechnique.h"
#include "ShadowMapPass.h"
#include "d3dUtil.h"

FogLightInjectionPass::~FogLightInjectionPass() = default;

void FogLightInjectionPass::Initialize(RenderScene* ownerScene, RenderTechnique* ownerTechnique, ID3D12GraphicsCommandList* commandList)
{
    __super::Initialize(ownerScene, ownerTechnique, commandList);
    InitShaderAndPSO();
    _volumTech = dynamic_cast<VolumetricFogTechnique*>(ownerTechnique);
}

void FogLightInjectionPass::Begin(ID3D12GraphicsCommandList* commandList)
{
    commandList->SetPipelineState(_pipelineState.Get());
    commandList->SetComputeRootSignature(_shader->GetRootSignature());
}

void FogLightInjectionPass::Draw(ID3D12GraphicsCommandList* commandList)
{
    int                         readIndex    = _volumTech->_readIndex ? 1 : 0;
    int                         writeIndex   = _volumTech->_readIndex ? 0 : 1;
    auto                        pbrlitTech   = _ownerScene->GetRenderTechnique<PBRLitTechnique>();
    auto                        shadowpass   = pbrlitTech->GetRenderPass<ShadowMapPass>();
    D3D12_GPU_VIRTUAL_ADDRESS   cameraData   = _ownerScene->_cameraBuffer->GetGPUVirtualAddress();
    D3D12_GPU_VIRTUAL_ADDRESS   lightData    = _ownerScene->_lightBuffer->GetGPUVirtualAddress();
    D3D12_GPU_VIRTUAL_ADDRESS   cascadeData   = shadowpass->GetCascadeDataCBV();
    D3D12_GPU_VIRTUAL_ADDRESS   fogData      = _volumTech->GetConstantBufferView()->GetGPUVirtualAddress();
    D3D12_GPU_DESCRIPTOR_HANDLE shadowMap    = shadowpass->GetShadowMapSRV();
    auto                        preVoxelTex  = _volumTech->_tempVoxelInjectionTexture3D[readIndex];
    auto                        currVoxelTex = _volumTech->_tempVoxelInjectionTexture3D[writeIndex];

    commandList->SetComputeRootConstantBufferView(_shader->GetRootParameterIndex("cameraData"), cameraData);
    commandList->SetComputeRootConstantBufferView(_shader->GetRootParameterIndex("lightData"), lightData);
    commandList->SetComputeRootConstantBufferView(_shader->GetRootParameterIndex("cascadeData"), cascadeData);
    commandList->SetComputeRootConstantBufferView(_shader->GetRootParameterIndex("fogdata"), fogData);
    commandList->SetComputeRootDescriptorTable(_shader->GetRootParameterIndex("VoxelReadTexture"), preVoxelTex->GetSRVHandle());
    commandList->SetComputeRootDescriptorTable(_shader->GetRootParameterIndex("VoxelWriteTexture"), currVoxelTex->GetUAVHandle());
    commandList->SetComputeRootDescriptorTable(_shader->GetRootParameterIndex("ShadowMap"), shadowpass->GetShadowMapSRV());
    commandList->Dispatch(d3dUtil::Ceil(VOXEL_VOLUME_SIZEX , 8), d3dUtil::Ceil(VOXEL_VOLUME_SIZEY , 8),VOXEL_VOLUME_SIZEZ);
}

void FogLightInjectionPass::AddRenderPassDatas(std::string_view sceneName)
{
    VolumetricFogProperty property;
    property.FogAnisotropy        = 0.001f;
    property.LightShaftAnisotropy = 0.001f;
    property.Density              = 10.f;
    property.Strength             = 3.5f;
    property.BlendWithScene       = 1.f;
    property.BlendWithPrevFrame   = 0.5f;
    property.CustomNear           = 0.01f;
    property.CustomFar            = 1000.f;
    property.FogIntensity         = 1.f;
    property.LightShaftIntensity  = 1.f;
    Global::renderPassDatas->AddRenderPassProperty("VolumetricFogData", property);
}

void FogLightInjectionPass::InitShaderAndPSO()
{
    _shader = std::make_unique<ShaderBuilder>();
    _shader->BeginBuild();
    _shader->SetShader(L"../Shaders/cs_light_injection.hlsl", ShaderBuilder::Type::CS);
    _shader->EndBuild();

    D3D12_COMPUTE_PIPELINE_STATE_DESC psoDesc{};
    psoDesc.pRootSignature = _shader->GetRootSignature();
    psoDesc.CS             = _shader->GetShaderByteCode(ShaderBuilder::Type::CS);
    psoDesc.Flags          = D3D12_PIPELINE_STATE_FLAG_NONE;

    HRESULT hr =
        Global::device->GetDevice()->CreateComputePipelineState(&psoDesc, IID_PPV_ARGS(_pipelineState.GetAddressOf()));
    FAILED_CHECK_MESSAGE(hr, L"DXRComputeSkeletalMeshPass::InitShaderAndPSO CreateComputePipelineState failed");
}
