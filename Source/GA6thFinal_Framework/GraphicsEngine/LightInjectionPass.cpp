#include "pch.h"
#include "LightInjectionPass.h"
#include "ShaderBuilder.h"
#include "RenderScene.h"
#include "PBRLitTechnique.h"
#include "VolumetricFogTechnique.h"
#include "ShadowMapPass.h"

LightInjectionPass::~LightInjectionPass() {}

void LightInjectionPass::Initialize(RenderScene* ownerScene, RenderTechnique* ownerTechnique,
                                    ID3D12GraphicsCommandList* commandList)
{
    __super::Initialize(ownerScene, ownerTechnique, commandList);
    InitShaderAndPSO();
    _volumTech = dynamic_cast<VolumetricFogTechnique*>(ownerTechnique);
    
    // 이러면 패스별로 동일한 텍스쳐를 사용하고 있기에 나중에 공용 텍스쳐로 빼야할것같음 일다는 되는거 확인 먼저
    _noiseTexture                      = std::make_unique<Texture>();
    std::filesystem::path noiseTexPath = L"../Shaders/texture/blueNoise.dds";
    _noiseTexture->LoadResource(noiseTexPath);
    
    _volumetricFogBuffer = std::make_unique<ConstantBufferView>();
    UINT alignedSize     = (sizeof(VolumetricFogTechnique) + 255) & ~255;
    _volumetricFogBuffer->Initialize(alignedSize);
    
}

void LightInjectionPass::Update(ID3D12GraphicsCommandList* commandList) 
{
    //VolumetricFogData fogData;
    // VolumetricFogData
}

void LightInjectionPass::Begin(ID3D12GraphicsCommandList* commandList) 
{
    commandList->SetPipelineState(_pso.Get());
    commandList->SetComputeRootSignature(_shader->GetRootSignature());
}

void LightInjectionPass::Draw(ID3D12GraphicsCommandList* commandList) 
{
    auto pbrlitTech = _ownerTechnique->GetRenderTechnique<PBRLitTechnique>();

    auto shadowpass = pbrlitTech->GetRenderPass<ShadowMapPass>();
    
    D3D12_GPU_VIRTUAL_ADDRESS cameraData = _ownerScene->_cameraBuffer->GetGPUVirtualAddress();
    D3D12_GPU_VIRTUAL_ADDRESS lightData   = _ownerScene->_lightBuffer->GetGPUVirtualAddress();
    D3D12_GPU_VIRTUAL_ADDRESS cacadeData  = shadowpass->GetCascadeDataCBV();

    D3D12_GPU_DESCRIPTOR_HANDLE shadowMap  = shadowpass->GetShadowMapSRV();
    auto                        preVoxelTex = _volumTech->GetPrevVoxelTexture();
    auto                        currVoxelTex = _volumTech->GetCurrVoxelTexture();
    commandList->SetComputeRootConstantBufferView(_shader->GetRootParameterIndex("cameraData"), cacadeData);
    commandList->SetComputeRootConstantBufferView(_shader->GetRootParameterIndex("lightdata"), lightData);
    commandList->SetComputeRootConstantBufferView(_shader->GetRootParameterIndex("cascadeData"),cacadeData);
    commandList->SetComputeRootDescriptorTable(_shader->GetRootParameterIndex("VoxelReadTexture"),
                                               _volumTech->GetPrevVoxelTexture()->GetSRVHandle());
    commandList->SetComputeRootDescriptorTable(_shader->GetRootParameterIndex("VoxelWriteTexture"),
                                               _volumTech->GetCurrVoxelTexture()->GetUAVHandle());
    commandList->SetComputeRootDescriptorTable(_shader->GetRootParameterIndex("ShadowMap"),
                                               shadowpass->GetShadowMapSRV());
    
}

void LightInjectionPass::End(ID3D12GraphicsCommandList* commandList) {}

void LightInjectionPass::AddRenderPassDatas(std::string_view sceneName)
{
    VolumetricFogProperty property;
    property.Anisotropy = 0.3;
    property.Density    = 3.f;
    property.Strength   = 15.f;
    property.BlendWithScene = 1.f;
    property.BlendWithPrevFrame = 0.95f;
    property.CustomNear         = 0.5f;
    property.CustomFar = 1000.f;
    Global::renderPassDatas->AddRenderPassProperty(sceneName, "VolumetricFogData", property);
}

void LightInjectionPass::InitShaderAndPSO()
{
    _shader = std::make_unique<ShaderBuilder>();
    _shader->BeginBuild();
    _shader->SetShader(L"../Shaders/cs_light_injection.hlsl", ShaderBuilder::Type::CS);
    _shader->EndBuild();

    D3D12_COMPUTE_PIPELINE_STATE_DESC psoDesc{};
    psoDesc.pRootSignature = _shader->GetRootSignature();
    psoDesc.CS             = _shader->GetShaderByteCode(ShaderBuilder::Type::CS);
    psoDesc.Flags          = D3D12_PIPELINE_STATE_FLAG_NONE;

    HRESULT hr = Global::device->GetDevice()->CreateComputePipelineState(&psoDesc, IID_PPV_ARGS(_pso.GetAddressOf()));
    FAILED_CHECK_MESSAGE(hr, L"DXRComputeSkeletalMeshPass::InitShaderAndPSO CreateComputePipelineState failed");
}
