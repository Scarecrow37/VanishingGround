#include "pch.h"
#include "AccumulationModule.h"

AccumulationModule::AccumulationModule() = default;

AccumulationModule::~AccumulationModule() = default;

void AccumulationModule::Initialize()
{
    PipelineStateStream pss{};
    pss.BlendState                        = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    pss.RasterizerState                   = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    pss.DepthStencilState                 = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
    (&pss.DepthStencilState)->DepthEnable = FALSE;
    pss.PrimitiveTopology                 = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    
    _fx.SetPipelineStateStream(pss);
    _pipelineState = Global::pipelineStateManager->GetPipelineState(pss);

    auto model = Global::resourceManager->LoadResource<Model>("Quad");
    _quadMesh  = model->GetMeshes().front().get();
}

void AccumulationModule::Execute(ID3D12GraphicsCommandList* commandList, D3D12_GPU_DESCRIPTOR_HANDLE input, UnorderedAccessView* output)
{
    const auto& resolution = output->GetResolution();

    D3D12_VIEWPORT viewport{.Width = static_cast<float>(resolution.cx), .Height = static_cast<float>(resolution.cy)};
    D3D12_RECT     scissorRect{.right = resolution.cx, .bottom = resolution.cy};

    commandList->OMSetRenderTargets(0, nullptr, FALSE, nullptr);
    commandList->RSSetViewports(1, &viewport);
    commandList->RSSetScissorRects(1, &scissorRect);

    commandList->SetPipelineState(_pipelineState.Get());
    commandList->SetGraphicsRootSignature(_fx.GetRootSignature());

    commandList->SetGraphicsRootDescriptorTable(_fx.GetRootParameterIndex("sourceTexture"), input);
    commandList->SetGraphicsRootDescriptorTable(_fx.GetRootParameterIndex("accumulation"), output->GetUAVHandle());
    
    _quadMesh->Render(commandList);
}