#include "pch.h"
#include "SSAOWritePass.h"
SSAOWritePass::SSAOWritePass() {}

SSAOWritePass::~SSAOWritePass() {}

void SSAOWritePass::Initialize(RenderScene* ownerScene, RenderTechnique* ownerTechnique,
                               ID3D12GraphicsCommandList* commandList)
{
    __super::Initialize(ownerScene, ownerTechnique, commandList);
    auto mode = Global::device->GetMode();
    auto desc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R8_UNORM, mode.Width, mode.Height, 1, 1, 1, 0,
                                             D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);
    
    _renderTarget = MakeSharedResource<RenderTarget>();
    _renderTarget->Initialize(desc, 1.f);
    _renderTarget->TransitionResource(commandList, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
}

void SSAOWritePass::AddRenderPassDatas(std::string_view sceneName) 
{
    auto device = Global::device->GetDevice();
    auto desc   = _meshRenderTarget->GetResource()->GetDesc();
    
    Global::renderPassDatas->AddRenderPassProperty(sceneName, "SSAOWritePass",
                                                   SSAOPassProperty({0.0005f, 3.f, 2.f, 2.f}));
    Global::renderPassDatas->AddRenderPassImage(sceneName, "SSAOWritePass", "SSAOTexture", _renderTarget->GetSRVHandle());
}

void SSAOWritePass::Begin(ID3D12GraphicsCommandList* commandList) 
{
    _renderTarget->TransitionResource(commandList, D3D12_RESOURCE_STATE_RENDER_TARGET);
    commandList->OMSetRenderTargets(1, &_renderTarget->GetRTVHandle(), FALSE,
                                    &_ownerScene->_depthStencilView->GetDSVHandle());
    commandList->RSSetViewports(1, &_renderTarget->GetViewport());
    commandList->RSSetScissorRects(1, &_renderTarget->GetScissorRect());
}

void SSAOWritePass::Draw(ID3D12GraphicsCommandList* commandList)
{
    const auto& ssaoProperty =
        std::any_cast<const SSAOPassProperty&>(_ownerScene->GetRenderPassProperty("SSAOWritePass"));
    const auto& renderTargetGroup = Global::multiRenderTargetManager->GetRenderTargetGroup("GBuffer");
    commandList->SetGraphicsRootSignature(_fxSSAOWrite.GetRootSignature());
    commandList->SetPipelineState(_pipelineState.Get());
    commandList->SetGraphicsRoot32BitConstants(_fxSSAOWrite.GetRootParameterIndex("ssaoParams"), 4, &ssaoProperty, 0);
    if (Global::isRayTracing)
    {
        commandList->SetGraphicsRootDescriptorTable(_fxSSAOWrite.GetRootParameterIndex("normalMap"),
                                                    renderTargetGroup[DXRGBuffer::DXRNORMAL]->GetSRVHandle());

        commandList->SetGraphicsRootDescriptorTable(_fxSSAOWrite.GetRootParameterIndex("depthMap"),
                                                    renderTargetGroup[DXRGBuffer::DXRDEPTH]->GetSRVHandle());
    }
    else
    {
        commandList->SetGraphicsRootDescriptorTable(_fxSSAOWrite.GetRootParameterIndex("normalMap"),
                                                    renderTargetGroup[GBuffer::ORM]->GetSRVHandle());

        commandList->SetGraphicsRootDescriptorTable(_fxSSAOWrite.GetRootParameterIndex("depthMap"),
                                                    renderTargetGroup[GBuffer::DEPTH]->GetSRVHandle());
    }
    _ownerScene->_frameQuad->Render(commandList);
}

void SSAOWritePass::End(ID3D12GraphicsCommandList* commandList)
{
    _meshRenderTarget->TransitionResource(commandList, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
}

void SSAOWritePass::InitShaderAndPSO()
{
    PipelineStateStream pss;
    pss.BlendDesc = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    pss.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    pss.DepthStencilState            = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
    pss.PrimitiveTopology            = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    pss.RTVFormats                   = {{DXGI_FORMAT_R8_UNORM}, 1};
    pss.DSVFormat                    = _ownerScene->_depthStencilView->GetFormat();
    (&pss.RasterizerState)->CullMode = D3D12_CULL_MODE_BACK;
    _pipelineState                   = Global::pipelineStateManager->GetPipelineState(pss);
}