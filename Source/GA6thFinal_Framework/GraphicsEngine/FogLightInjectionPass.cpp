#include "pch.h"
#include "FogLightInjectionPass.h"
#include "ShaderBuilder.h"
#include "LightingTechnique.h"
#include "VolumetricFogTechnique.h"
#include "ShadowMapPass.h"
#include "d3dUtil.h"

FogLightInjectionPass::~FogLightInjectionPass() = default;

void FogLightInjectionPass::Initialize(RenderScene* ownerScene, RenderTechnique* ownerTechnique, ID3D12GraphicsCommandList* commandList)
{
    RenderPass::Initialize(ownerScene, ownerTechnique, commandList);
    InitShaderAndPSO();
    _volumTech = dynamic_cast<VolumetricFogTechnique*>(ownerTechnique);
}

void FogLightInjectionPass::Begin(ID3D12GraphicsCommandList* commandList)
{
    commandList->SetPipelineState(_pipelineState.Get());
    commandList->SetComputeRootSignature(_fx.GetRootSignature());
}

void FogLightInjectionPass::Draw(ID3D12GraphicsCommandList* commandList)
{
    int                         readIndex    = _volumTech->_readIndex ? 1 : 0;
    int                         writeIndex   = _volumTech->_readIndex ? 0 : 1;
    auto                        pbrlitTech    = _ownerScene->GetRenderTechnique<LightingTechnique>();
    auto                        shadowpass   = pbrlitTech->GetRenderPass<ShadowMapPass>();
    D3D12_GPU_VIRTUAL_ADDRESS   cameraData   = _ownerScene->_cameraBuffer->GetGPUVirtualAddress();
    D3D12_GPU_VIRTUAL_ADDRESS   lightData    = _ownerScene->_lightBuffer->GetGPUVirtualAddress();
    D3D12_GPU_VIRTUAL_ADDRESS   cascadeData   = shadowpass->GetCascadeDataCBV();
    D3D12_GPU_VIRTUAL_ADDRESS   fogData      = _volumTech->GetConstantBufferView()->GetGPUVirtualAddress();
    D3D12_GPU_DESCRIPTOR_HANDLE shadowMap    = shadowpass->GetShadowMapSRV();
    auto                        preVoxelTex  = _volumTech->_tempVoxelInjectionTexture3D[readIndex];
    auto                        currVoxelTex = _volumTech->_tempVoxelInjectionTexture3D[writeIndex];

    commandList->SetComputeRootConstantBufferView(_fx.GetRootParameterIndex("cameraData"), cameraData);
    commandList->SetComputeRootConstantBufferView(_fx.GetRootParameterIndex("lightData"), lightData);
    commandList->SetComputeRootConstantBufferView(_fx.GetRootParameterIndex("cascadeData"), cascadeData);
    commandList->SetComputeRootConstantBufferView(_fx.GetRootParameterIndex("fogdata"), fogData);
    commandList->SetComputeRootDescriptorTable(_fx.GetRootParameterIndex("VoxelReadTexture"), preVoxelTex->GetSRVHandle());
    commandList->SetComputeRootDescriptorTable(_fx.GetRootParameterIndex("VoxelWriteTexture"), currVoxelTex->GetUAVHandle());
    commandList->SetComputeRootDescriptorTable(_fx.GetRootParameterIndex("ShadowMap"), shadowpass->GetShadowMapSRV());
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
    property.FogColor[0]          = 1.f;
    property.FogColor[1]          = 1.f;
    property.FogColor[2]          = 1.f;
    property.FogColor[3]          = 1.f;
    Global::renderPassDatas->AddRenderPassProperty("VolumetricFogData", property);
}

void FogLightInjectionPass::InitShaderAndPSO()
{
    ComputePipelineStateStream pss;
    _fx.SetPipelineStateStream(pss);
    _pipelineState = Global::pipelineStateManager->GetPipelineState(pss);
}
