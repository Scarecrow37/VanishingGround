#include "pch.h"
#include "GaussianBlurModule.h"

GaussianBlurModule::GaussianBlurModule() = default;

GaussianBlurModule::~GaussianBlurModule() = default;

void GaussianBlurModule::Initialize()
{
    _shaders[AXIS_X] = std::make_unique<ShaderBuilder>();
    _shaders[AXIS_X]->BeginBuild();
    _shaders[AXIS_X]->SetShader(L"../Shaders/vs_quad.hlsl", ShaderBuilder::Type::VS);
    _shaders[AXIS_X]->SetShader(L"../Shaders/ps_gaussianblur_x.hlsl", ShaderBuilder::Type::PS);
    _shaders[AXIS_X]->EndBuild();

    _shaders[AXIS_Y] = std::make_unique<ShaderBuilder>();
    _shaders[AXIS_Y]->BeginBuild();
    _shaders[AXIS_Y]->SetShader(L"../Shaders/vs_quad.hlsl", ShaderBuilder::Type::VS);
    _shaders[AXIS_Y]->SetShader(L"../Shaders/ps_gaussianblur_y.hlsl", ShaderBuilder::Type::PS);
    _shaders[AXIS_Y]->EndBuild();

    HRESULT       hr     = S_OK;
    ID3D12Device* device = Global::device->GetDevice();

    D3D12_GRAPHICS_PIPELINE_STATE_DESC psodesc = {};
    psodesc.RasterizerState                    = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    psodesc.BlendState                         = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    psodesc.DepthStencilState                  = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
    psodesc.DepthStencilState.DepthEnable      = FALSE;
    psodesc.SampleMask                         = UINT_MAX;
    psodesc.PrimitiveTopologyType              = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    psodesc.NumRenderTargets                   = 1;
    psodesc.RTVFormats[0]                      = DXGI_FORMAT_R32G32B32A32_FLOAT;
    psodesc.SampleDesc                         = {1, 0};

    psodesc.InputLayout    = _shaders[AXIS_X]->GetInputLayout();
    psodesc.pRootSignature = _shaders[AXIS_X]->GetRootSignature();
    psodesc.VS             = _shaders[AXIS_X]->GetShaderByteCode(ShaderBuilder::Type::VS);
    psodesc.PS             = _shaders[AXIS_X]->GetShaderByteCode(ShaderBuilder::Type::PS);

    hr = device->CreateGraphicsPipelineState(&psodesc, IID_PPV_ARGS(&_pipelineStates[AXIS_X]));
    FAILED_CHECK_MESSAGE(hr, L"GaussianBlurModule::Initialize device->CreateGraphicsPipelineState Failed");

    psodesc.InputLayout    = _shaders[AXIS_Y]->GetInputLayout();
    psodesc.pRootSignature = _shaders[AXIS_Y]->GetRootSignature();
    psodesc.VS             = _shaders[AXIS_Y]->GetShaderByteCode(ShaderBuilder::Type::VS);
    psodesc.PS             = _shaders[AXIS_Y]->GetShaderByteCode(ShaderBuilder::Type::PS);

    hr = device->CreateGraphicsPipelineState(&psodesc, IID_PPV_ARGS(&_pipelineStates[AXIS_Y]));
    FAILED_CHECK_MESSAGE(hr, L"GaussianBlurModule::Initialize device->CreateGraphicsPipelineState Failed");

    auto model = Global::resourceManager->LoadResource<Model>("Quad");
    _quadMesh  = model->GetMeshes().front().get();
}

void GaussianBlurModule::Execute(ID3D12GraphicsCommandList* commandList, D3D12_GPU_DESCRIPTOR_HANDLE input, RenderTarget* output, GaussianBlurType type)
{
    commandList->OMSetRenderTargets(1, &output->GetRTVHandle(), FALSE, nullptr);
    commandList->RSSetViewports(1, &output->GetViewport());
    commandList->RSSetScissorRects(1, &output->GetScissorRect());

    commandList->SetPipelineState(_pipelineStates[type].Get());
    commandList->SetGraphicsRootSignature(_shaders[type]->GetRootSignature());

    const auto& resolution = output->GetResolution();
    PostProcessData postProcessData{.TexelSize = {1.0f / resolution.Width, 1.0f / resolution.Height}};

    commandList->SetGraphicsRoot32BitConstants(_shaders[type]->GetRootParameterIndex("bit32_5_postProcessData"), 5, &postProcessData, 0);
    commandList->SetGraphicsRootDescriptorTable(_shaders[type]->GetRootParameterIndex("sourceTexture"), input);
    
    _quadMesh->Render(commandList);
}