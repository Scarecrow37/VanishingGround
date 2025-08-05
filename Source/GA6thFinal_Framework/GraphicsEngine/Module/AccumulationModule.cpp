#include "pch.h"
#include "AccumulationModule.h"

AccumulationModule::AccumulationModule() = default;

AccumulationModule::~AccumulationModule() = default;

void AccumulationModule::Initialize()
{
    _shader = std::make_unique<ShaderBuilder>();
    _shader->BeginBuild();
    _shader->SetShader(L"../Shaders/vs_quad.hlsl", ShaderBuilder::Type::VS);
    _shader->SetShader(L"../Shaders/ps_accumulation.hlsl", ShaderBuilder::Type::PS);
    _shader->EndBuild();

    HRESULT       hr     = S_OK;
    ID3D12Device* device = Global::device->GetDevice();

    D3D12_GRAPHICS_PIPELINE_STATE_DESC psodesc = {};
    psodesc.RasterizerState                    = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    psodesc.BlendState                         = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    psodesc.DepthStencilState                  = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
    psodesc.DepthStencilState.DepthEnable      = FALSE;
    psodesc.SampleMask                         = UINT_MAX;
    psodesc.PrimitiveTopologyType              = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    psodesc.NumRenderTargets                   = 0;
    psodesc.SampleDesc                         = {1, 0};
    psodesc.InputLayout                        = _shader->GetInputLayout();
    psodesc.pRootSignature                     = _shader->GetRootSignature();
    psodesc.VS                                 = _shader->GetShaderByteCode(ShaderBuilder::Type::VS);
    psodesc.PS                                 = _shader->GetShaderByteCode(ShaderBuilder::Type::PS);


    hr = device->CreateGraphicsPipelineState(&psodesc, IID_PPV_ARGS(&_pipelineState));
    FAILED_CHECK_MESSAGE(hr, L"AccumulationModule::Initialize device->CreateGraphicsPipelineState Failed");

    auto model = Global::resourceManager->LoadResource<Model>("Quad");
    _quadMesh  = model->GetMeshes().front().get();
}

void AccumulationModule::Execute(ID3D12GraphicsCommandList* commandList, D3D12_GPU_DESCRIPTOR_HANDLE input, UnorderedAccessView* output)
{
    const auto& resolution = output->GetResolution();

    D3D12_VIEWPORT viewport{.Width  = static_cast<float>(resolution.Width),
                            .Height = static_cast<float>(resolution.Height)};

    D3D12_RECT scissorRect{.right  = static_cast<LONG>(resolution.Width),
                           .bottom = static_cast<LONG>(resolution.Height)};

    commandList->OMSetRenderTargets(0, nullptr, FALSE, nullptr);
    commandList->RSSetViewports(1, &viewport);
    commandList->RSSetScissorRects(1, &scissorRect);

    commandList->SetPipelineState(_pipelineState.Get());
    commandList->SetGraphicsRootSignature(_shader->GetRootSignature());

    commandList->SetGraphicsRootDescriptorTable(_shader->GetRootParameterIndex("sourceTexture"), input);
    commandList->SetGraphicsRootDescriptorTable(_shader->GetRootParameterIndex("accumulation"), output->GetUAVHandle());
    
    _quadMesh->Render(commandList);
}