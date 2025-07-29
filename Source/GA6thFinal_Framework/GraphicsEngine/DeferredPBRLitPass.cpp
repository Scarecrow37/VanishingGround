#include "pch.h"
#include "DeferredPBRLitPass.h"
#include "SkyBox.h"
#include "ShadowMapPass.h"

DeferredPBRLitPass::~DeferredPBRLitPass() {}

void DeferredPBRLitPass::Initialize(RenderScene* ownerScene, RenderTechnique* ownerTechnique, ID3D12GraphicsCommandList* commandList)
{
    __super::Initialize(ownerScene, ownerTechnique, commandList);

    InitShaderAndPSO();
}

void DeferredPBRLitPass::Begin(ID3D12GraphicsCommandList* commandList)
{
    _meshRenderTarget->TransitionResource(commandList, D3D12_RESOURCE_STATE_RENDER_TARGET);

    commandList->OMSetRenderTargets(1, &_meshRenderTarget->GetRTVHandle(), FALSE, nullptr);
    commandList->RSSetViewports(1, &_meshRenderTarget->GetViewPort());
    commandList->RSSetScissorRects(1, &_meshRenderTarget->GetScissorRect());
}

void DeferredPBRLitPass::Draw(ID3D12GraphicsCommandList* commandList)
{
    commandList->SetPipelineState(_pipelineState.Get());
    commandList->SetGraphicsRootSignature(_shader->GetRootSignature());

    //"BaseColor", "Normal", "ORM", "Emissive", "WorldPosition", "Depth", "CustomDepth"
    const auto& renderTargetGroup = Global::multiRenderTargetManager->GetRenderTargetGroup("GBuffer");

    auto shadowMapPass = GetRenderPass<ShadowMapPass>();
    if (nullptr == shadowMapPass)
        return;

    commandList->SetGraphicsRoot32BitConstants(_shader->GetRootParameterIndex("bit32_3_numLight"), 3, &_ownerScene->_numLight, 0);
    commandList->SetGraphicsRootConstantBufferView(_shader->GetRootParameterIndex("cameraData"), _ownerScene->_cameraBuffer->GetGPUVirtualAddress());
    commandList->SetGraphicsRootConstantBufferView(_shader->GetRootParameterIndex("lightData"), _ownerScene->_lightBuffer->GetGPUVirtualAddress());
    commandList->SetGraphicsRootConstantBufferView(_shader->GetRootParameterIndex("cascadeData"), shadowMapPass->GetCascadeDataCBV());
    commandList->SetGraphicsRootDescriptorTable(_shader->GetRootParameterIndex("shadowMap"), shadowMapPass->GetShadowMapSRV());
    commandList->SetGraphicsRootDescriptorTable(_shader->GetRootParameterIndex("irradianceMap"), _ownerScene->_skyBox->GetIrradianceMapSRV());
    commandList->SetGraphicsRootDescriptorTable(_shader->GetRootParameterIndex("prefilteredMap"), _ownerScene->_skyBox->GetPrefilteredMapSRV());
    commandList->SetGraphicsRootDescriptorTable(_shader->GetRootParameterIndex("brdfLUT"), _ownerScene->_skyBox->GetBrdfLUTSRV());
    commandList->SetGraphicsRootDescriptorTable(_shader->GetRootParameterIndex("baseColorMap"), renderTargetGroup[GBuffer::BASECOLOR]->GetSRVHandle());
    commandList->SetGraphicsRootDescriptorTable(_shader->GetRootParameterIndex("normalMap"), renderTargetGroup[GBuffer::NORMAL]->GetSRVHandle());
    commandList->SetGraphicsRootDescriptorTable(_shader->GetRootParameterIndex("ormMap"), renderTargetGroup[GBuffer::ORM]->GetSRVHandle());
    commandList->SetGraphicsRootDescriptorTable(_shader->GetRootParameterIndex("worldPositionMap"), renderTargetGroup[GBuffer::WORLDPOSITION]->GetSRVHandle());
    commandList->SetGraphicsRootDescriptorTable(_shader->GetRootParameterIndex("depthMap"), renderTargetGroup[GBuffer::DEPTH]->GetSRVHandle());

    _ownerScene->_frameQuad->Render(commandList);
}

void DeferredPBRLitPass::End(ID3D12GraphicsCommandList* commandList)
{
    _meshRenderTarget->TransitionResource(commandList, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
}

void DeferredPBRLitPass::InitShaderAndPSO()
{
    _shader = std::make_unique<ShaderBuilder>();
    _shader->BeginBuild();
    _shader->SetShader(L"../Shaders/vs_quad.hlsl", ShaderBuilder::Type::VS);
    _shader->SetShader(L"../Shaders/ps_pbr_lighting.hlsl", ShaderBuilder::Type::PS);
    _shader->EndBuild();

    ID3D12Device*                      device = Global::device->GetDevice();
    D3D12_GRAPHICS_PIPELINE_STATE_DESC psodesc{};
    HRESULT                            hr = S_OK;

    psodesc.RasterizerState               = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    psodesc.RasterizerState.CullMode      = D3D12_CULL_MODE_NONE;
    psodesc.BlendState                    = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    psodesc.DepthStencilState             = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
    psodesc.DepthStencilState.DepthEnable = FALSE;
    psodesc.SampleMask                    = UINT_MAX;
    psodesc.PrimitiveTopologyType         = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    psodesc.InputLayout                   = _shader->GetInputLayout();
    psodesc.NumRenderTargets              = 1;
    psodesc.RTVFormats[0]                 = DXGI_FORMAT_R32G32B32A32_FLOAT;
    psodesc.pRootSignature                = _shader->GetRootSignature();
    psodesc.SampleDesc                    = {1, 0};
    psodesc.VS                            = _shader->GetShaderByteCode(ShaderBuilder::Type::VS);
    psodesc.PS                            = _shader->GetShaderByteCode(ShaderBuilder::Type::PS);

    hr = device->CreateGraphicsPipelineState(&psodesc, IID_PPV_ARGS(&_pipelineState));
    FAILED_CHECK_MESSAGE(hr, L"DeferredPBRLitPass::InitShaderAndPSO device->CreateGraphicsPipelineState Failed");
}