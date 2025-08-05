#include "pch.h"
#include "GaussianBlurModule.h"

GaussianBlurModule::GaussianBlurModule() = default;

GaussianBlurModule::~GaussianBlurModule() = default;

void GaussianBlurModule::Initialize()
{
    InitShaderAndPipelineState();

    auto model = Global::resourceManager->LoadResource<Model>("Quad");
    _quadMesh  = model->GetMeshes().front().get();
}

void GaussianBlurModule::Execute(ID3D12GraphicsCommandList* commandList, D3D12_GPU_DESCRIPTOR_HANDLE input, RenderTarget* output, DXGI_FORMAT rtvFormat, BlurType type)
{
    const auto& resolution = output->GetResolution();
    PostProcessData postProcessData{.TexelSize = {1.0f / resolution.Width, 1.0f / resolution.Height}};

    commandList->OMSetRenderTargets(1, &output->GetRTVHandle(), FALSE, nullptr);
    commandList->RSSetViewports(1, &output->GetViewport());
    commandList->RSSetScissorRects(1, &output->GetScissorRect());

    _pipelineStates[type].RTVFormats          = {{rtvFormat}, 1};
    ComPtr<ID3D12PipelineState> pipelineState = Global::pipelineStateManager->GetPipelineState(_pipelineStates[type]);

    commandList->SetPipelineState(pipelineState.Get());

    switch (type)
    {
    case AXIS_X:
        commandList->SetGraphicsRootSignature(_fxBlurX.GetRootSignature());
        commandList->SetGraphicsRoot32BitConstants(_fxBlurX.GetRootParameterIndex("bit32_5_postProcessData"), 5, &postProcessData, 0);
        commandList->SetGraphicsRootDescriptorTable(_fxBlurX.GetRootParameterIndex("sourceTexture"), input);
        break;
    case AXIS_Y:
        commandList->SetGraphicsRootSignature(_fxBlurY.GetRootSignature());
        commandList->SetGraphicsRoot32BitConstants(_fxBlurY.GetRootParameterIndex("bit32_5_postProcessData"), 5, &postProcessData, 0);
        commandList->SetGraphicsRootDescriptorTable(_fxBlurY.GetRootParameterIndex("sourceTexture"), input);
        break;
    }
        
    _quadMesh->Render(commandList);
}

void GaussianBlurModule::InitShaderAndPipelineState()
{    
    _pipelineStates[AXIS_X].BlendState                        = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    _pipelineStates[AXIS_X].RasterizerState                   = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    _pipelineStates[AXIS_X].DepthStencilState                 = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
    (&_pipelineStates[AXIS_X].DepthStencilState)->DepthEnable = FALSE;
    _pipelineStates[AXIS_X].PrimitiveTopology                 = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

    _pipelineStates[AXIS_Y] = _pipelineStates[AXIS_X];

    _fxBlurX.SetPipelineStateStream(_pipelineStates[AXIS_X]);
    _fxBlurY.SetPipelineStateStream(_pipelineStates[AXIS_Y]);
}