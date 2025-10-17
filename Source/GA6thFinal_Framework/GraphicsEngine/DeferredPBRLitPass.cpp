#include "pch.h"
#include "DeferredPBRLitPass.h"
#include "SkyBox.h"
#include "ShadowMapPass.h"
#include "PointLightShadowPass.h"
#include "SSAOWritePass.h"

void DeferredPBRLitPass::Initialize(RenderScene* ownerScene, RenderTechnique* ownerTechnique, ID3D12GraphicsCommandList* commandList)
{
    RenderPass::Initialize(ownerScene, ownerTechnique, commandList);

    InitShaderAndPSO();
}

void DeferredPBRLitPass::Begin(ID3D12GraphicsCommandList* commandList)
{
    _meshRenderTarget->TransitionResource(commandList, D3D12_RESOURCE_STATE_RENDER_TARGET);

    commandList->OMSetRenderTargets(1, &_meshRenderTarget->GetRTVHandle(), FALSE, nullptr);
    commandList->RSSetViewports(1, &_meshRenderTarget->GetViewport());
    commandList->RSSetScissorRects(1, &_meshRenderTarget->GetScissorRect());

    // ssao 비활성화시 이전 프레임에 사용되던걸 clear 해야함. ssaomap을 아예 set하지 않는게 가장 좋지만 지금 구조상
    // set에서 제외시키는 방법은 
}

void DeferredPBRLitPass::Draw(ID3D12GraphicsCommandList* commandList)
{
    commandList->SetPipelineState(_pipelineState.Get());
    commandList->SetGraphicsRootSignature(_fx.GetRootSignature());

    //"BaseColor", "Normal", "ORM", "Emissive", "Depth", "CustomDepth"
    const auto& renderTargetGroup = Global::multiRenderTargetManager->GetRenderTargetGroup("G-Buffer");

    auto shadowMapPass = _ownerTechnique->GetRenderPass<ShadowMapPass>();
    auto pointLightShadowPass = _ownerTechnique->GetRenderPass<PointLightShadowPass>();
    auto ssaoPass      = _ownerTechnique->GetRenderPass<SSAOWritePass>();

    if (nullptr == shadowMapPass || nullptr == ssaoPass || nullptr == pointLightShadowPass)
        return;

    D3D12_GPU_DESCRIPTOR_HANDLE brdf;
    D3D12_GPU_DESCRIPTOR_HANDLE irradiance;
    D3D12_GPU_DESCRIPTOR_HANDLE prefiltered;

    if (_ownerScene->_skyBox->HasIBLTexture())
    {
        brdf        = _ownerScene->_skyBox->GetBrdfLUTSRV();
        irradiance  = _ownerScene->_skyBox->GetIrradianceMapSRV();
        prefiltered = _ownerScene->_skyBox->GetPrefilteredMapSRV();
    }
    else
    {
        auto defaultTexture = Global::resourceManager->LoadResource<Texture>("BlackTexture")->GetGPUHandle();
        brdf                = defaultTexture;
        irradiance          = defaultTexture;
        prefiltered         = defaultTexture;
    }
    bool useSSAO    = ssaoPass->IsEnable(); 
    int useSSAOInt = useSSAO ? 1 : 0;
    commandList->SetGraphicsRoot32BitConstants(_fx.GetRootParameterIndex("bit32_1_isssao"), 1, &useSSAOInt, 0); 
    commandList->SetGraphicsRoot32BitConstants(_fx.GetRootParameterIndex("bit32_4_numLight"), 4, &_ownerScene->_numLight, 0);
    commandList->SetGraphicsRootConstantBufferView(_fx.GetRootParameterIndex("cameraData"), _ownerScene->_cameraBuffer->GetGPUVirtualAddress());
    commandList->SetGraphicsRootConstantBufferView(_fx.GetRootParameterIndex("lightData"), _ownerScene->_lightBuffer->GetGPUVirtualAddress());
    commandList->SetGraphicsRootConstantBufferView(_fx.GetRootParameterIndex("cascadeData"), shadowMapPass->GetCascadeDataCBV());
    commandList->SetGraphicsRootDescriptorTable(_fx.GetRootParameterIndex("shadowMap"), shadowMapPass->GetShadowMapSRV());
    commandList->SetGraphicsRootDescriptorTable(_fx.GetRootParameterIndex("irradianceMap"), _ownerScene->_skyBox->GetIrradianceMapSRV());
    commandList->SetGraphicsRootDescriptorTable(_fx.GetRootParameterIndex("prefilteredMap"), _ownerScene->_skyBox->GetPrefilteredMapSRV());
    commandList->SetGraphicsRootDescriptorTable(_fx.GetRootParameterIndex("brdfLUT"), _ownerScene->_skyBox->GetBrdfLUTSRV());
    commandList->SetGraphicsRootDescriptorTable(_fx.GetRootParameterIndex("baseColorMap"), renderTargetGroup[GBuffer::BASECOLOR]->GetSRVHandle());
    commandList->SetGraphicsRootDescriptorTable(_fx.GetRootParameterIndex("normalMap"), renderTargetGroup[GBuffer::NORMAL]->GetSRVHandle());
    commandList->SetGraphicsRootDescriptorTable(_fx.GetRootParameterIndex("ormMap"), renderTargetGroup[GBuffer::ORM]->GetSRVHandle());
    commandList->SetGraphicsRootDescriptorTable(_fx.GetRootParameterIndex("emissiveMap"), renderTargetGroup[GBuffer::EMISSIVE]->GetSRVHandle());
    commandList->SetGraphicsRootDescriptorTable(_fx.GetRootParameterIndex("depthMap"), renderTargetGroup[GBuffer::DEPTH]->GetSRVHandle());
    if (useSSAO)
        commandList->SetGraphicsRootDescriptorTable(_fx.GetRootParameterIndex("SSAOMap"), ssaoPass->GetAOTexture());
    if (pointLightShadowPass)
    {
        commandList->SetGraphicsRootDescriptorTable(_fx.GetRootParameterIndex("pointLightShadowMap"),
                                                    pointLightShadowPass->GetShadowAtlasSRV());
    }
    _ownerScene->_frameQuad->Render(commandList);
}

void DeferredPBRLitPass::End(ID3D12GraphicsCommandList* commandList)
{
    _meshRenderTarget->TransitionResource(commandList, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
}

void DeferredPBRLitPass::InitShaderAndPSO()
{
    PipelineStateStream pss;
    pss.BlendState                        = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    pss.RasterizerState                   = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    pss.DepthStencilState                 = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
    (&pss.DepthStencilState)->DepthEnable = FALSE;
    pss.PrimitiveTopology                 = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    pss.RTVFormats                        = {{DXGI_FORMAT_R32G32B32A32_FLOAT}, 1};

    _fx.SetPipelineStateStream(pss);
    _pipelineState = Global::pipelineStateManager->GetPipelineState(pss);
}