#include "pch.h"
#include "FogCompositePass.h"
#include "VolumetricFogTechnique.h"

FogCompositePass::~FogCompositePass() {}

void FogCompositePass::Initialize(RenderScene* ownerScene, RenderTechnique* ownerTechnique,
                                  ID3D12GraphicsCommandList* commandList)
{
    __super::Initialize(ownerScene, ownerTechnique, commandList);
    auto resolution = Global::device->GetResolution();
    auto desc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R32G32B32A32_FLOAT, resolution.cx, resolution.cy, 1, 1,
                                             1, 0, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);
    InitShaderAndPSO();
    _volumTech = dynamic_cast<VolumetricFogTechnique*>(ownerTechnique);
}

void FogCompositePass::Update(ID3D12GraphicsCommandList* commandList, const float deltaTime) {}

void FogCompositePass::Begin(ID3D12GraphicsCommandList* commandList) {}

void FogCompositePass::Draw(ID3D12GraphicsCommandList* commandList) 
{
    auto renderTarget = Global::multiRenderTargetManager->GetAvailableRenderTarget();
    renderTarget->TransitionResource(commandList, D3D12_RESOURCE_STATE_RENDER_TARGET);
    renderTarget->ClearRenderTarget(commandList, 0);
    commandList->OMSetRenderTargets(1, &renderTarget->GetRTVHandle(), FALSE, nullptr);
    commandList->RSSetViewports(1, &renderTarget->GetViewport());
    commandList->RSSetScissorRects(1, &renderTarget->GetScissorRect());
    commandList->SetPipelineState(_pipelineState.Get());
    commandList->SetGraphicsRootSignature(_fx.GetRootSignature());

    const auto& renderTargetGroup = Global::multiRenderTargetManager->GetRenderTargetGroup("GBuffer");
    auto        compositeData     = _volumTech->GetVolumetricFogBufferView()->GetGPUVirtualAddress();

    commandList->SetGraphicsRootSignature(_fx.GetRootSignature());
    commandList->SetPipelineState(_pipelineState.Get());
    commandList->SetGraphicsRootDescriptorTable(_fx.GetRootParameterIndex("screenMap"), _meshRenderTarget->GetSRVHandle());
    commandList->SetGraphicsRootDescriptorTable(_fx.GetRootParameterIndex("fogGridTexture"), _volumTech->_finalVoxelAccumulationTexture3D->GetSRVHandle());
    commandList->SetGraphicsRootDescriptorTable(_fx.GetRootParameterIndex("depthMap"), renderTargetGroup[GBuffer::DEPTH]->GetSRVHandle());
    commandList->SetGraphicsRootConstantBufferView(_fx.GetRootParameterIndex("VolumetricFogCompositeData"), compositeData);

    _ownerScene->_frameQuad->Render(commandList);
    renderTarget->TransitionResource(commandList, D3D12_RESOURCE_STATE_COPY_SOURCE);
    _meshRenderTarget->TransitionResource(commandList, D3D12_RESOURCE_STATE_COPY_DEST);
    commandList->CopyResource(_meshRenderTarget->GetResource(), renderTarget->GetResource());
    _meshRenderTarget->TransitionResource(commandList, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

    Global::multiRenderTargetManager->ReturnRenderTarget(renderTarget);
}

void FogCompositePass::End(ID3D12GraphicsCommandList* commandList) {}

void FogCompositePass::AddRenderPassDatas(std::string_view sceneName) {}

void FogCompositePass::InitShaderAndPSO()
{
    PipelineStateStream pss;
    pss.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    pss.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    pss.DepthStencilState            = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
    (&pss.DepthStencilState)->DepthEnable = FALSE;
    pss.PrimitiveTopology                 = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    pss.RTVFormats                        = {{DXGI_FORMAT_R32G32B32A32_FLOAT}, 1};
    
    _fx.SetPipelineStateStream(pss);
    _pipelineState = Global::pipelineStateManager->GetPipelineState(pss);
}
