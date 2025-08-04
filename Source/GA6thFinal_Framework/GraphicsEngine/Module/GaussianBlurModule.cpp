#include "pch.h"
#include "GaussianBlurModule.h"

GaussianBlurModule::GaussianBlurModule() = default;

GaussianBlurModule::~GaussianBlurModule() = default;

void GaussianBlurModule::Initialize()
{
    InitShaderAndPipelineState(RTV);
    InitShaderAndPipelineState(UAV);

    auto model = Global::resourceManager->LoadResource<Model>("Quad");
    _quadMesh  = model->GetMeshes().front().get();
}

void GaussianBlurModule::Execute(ID3D12GraphicsCommandList* commandList, D3D12_GPU_DESCRIPTOR_HANDLE input, RenderTarget* output, GaussianBlurType type)
{
    const auto& resolution = output->GetResolution();
    PostProcessData postProcessData{.TexelSize = {1.0f / resolution.Width, 1.0f / resolution.Height}};

    commandList->OMSetRenderTargets(1, &output->GetRTVHandle(), FALSE, nullptr);
    commandList->RSSetViewports(1, &output->GetViewport());
    commandList->RSSetScissorRects(1, &output->GetScissorRect());

    commandList->SetPipelineState(_pipelineStates[RTV][type].Get());
    commandList->SetGraphicsRootSignature(_shaders[RTV][type]->GetRootSignature());

    commandList->SetGraphicsRoot32BitConstants(_shaders[RTV][type]->GetRootParameterIndex("bit32_5_postProcessData"), 5, &postProcessData, 0);
    commandList->SetGraphicsRootDescriptorTable(_shaders[RTV][type]->GetRootParameterIndex("sourceTexture"), input);
    
    _quadMesh->Render(commandList);
}

void GaussianBlurModule::Execute(ID3D12GraphicsCommandList* commandList, UnorderedAccessView* uav, GaussianBlurType type)
{
    const auto& resolution = uav->GetResolution();

    D3D12_VIEWPORT  viewport{0.0f, 0.0f, static_cast<float>(resolution.Width), static_cast<float>(resolution.Height), 0.0f, 1.0f};
    D3D12_RECT      scissorRect{0, 0, static_cast<LONG>(resolution.Width), static_cast<LONG>(resolution.Height)};
    PostProcessData postProcessData{.TexelSize = {1.0f / resolution.Width, 1.0f / resolution.Height}};

    commandList->OMSetRenderTargets(0, nullptr, FALSE, nullptr);
    commandList->RSSetViewports(1, &viewport);
    commandList->RSSetScissorRects(1, &scissorRect);

    commandList->SetPipelineState(_pipelineStates[UAV][type].Get());
    commandList->SetGraphicsRootSignature(_shaders[UAV][type]->GetRootSignature());

    commandList->SetGraphicsRoot32BitConstants(_shaders[UAV][type]->GetRootParameterIndex("bit32_5_postProcessData"), 5, &postProcessData, 0);
    commandList->SetGraphicsRootDescriptorTable(_shaders[UAV][type]->GetRootParameterIndex("sourceTexture"), uav->GetUAVHandle());   
    
    _quadMesh->Render(commandList);
}

void GaussianBlurModule::InitShaderAndPipelineState(ViewType type)
{
    const wchar_t* vs   = L"../Shaders/vs_quad.hlsl";
    const wchar_t* ps_x = nullptr;
    const wchar_t* ps_y = nullptr;

    if (type == RTV)
    {
        ps_x   = L"../Shaders/ps_gaussianblur_x_rtv.hlsl";
        ps_y   = L"../Shaders/ps_gaussianblur_y_rtv.hlsl";
    }
    else if (type == UAV)
    {
        ps_x   = L"../Shaders/ps_gaussianblur_x_uav.hlsl";
        ps_y   = L"../Shaders/ps_gaussianblur_y_uav.hlsl";
    }

    ID3D12Device* device = Global::device->GetDevice();
    HRESULT       hr     = S_OK;

    _shaders[type][AXIS_X] = std::make_unique<ShaderBuilder>();
    _shaders[type][AXIS_X]->BeginBuild();
    _shaders[type][AXIS_X]->SetShader(vs, ShaderBuilder::Type::VS);
    _shaders[type][AXIS_X]->SetShader(ps_x, ShaderBuilder::Type::PS);
    _shaders[type][AXIS_X]->EndBuild();

    _shaders[type][AXIS_Y] = std::make_unique<ShaderBuilder>();
    _shaders[type][AXIS_Y]->BeginBuild();
    _shaders[type][AXIS_Y]->SetShader(vs, ShaderBuilder::Type::VS);
    _shaders[type][AXIS_Y]->SetShader(ps_y, ShaderBuilder::Type::PS);
    _shaders[type][AXIS_Y]->EndBuild();

    D3D12_GRAPHICS_PIPELINE_STATE_DESC psodesc = {};

    if (type == RTV)
    {
        psodesc.NumRenderTargets = 1;
        psodesc.RTVFormats[0]    = DXGI_FORMAT_R32G32B32A32_FLOAT;
    }
    else
    {
        psodesc.NumRenderTargets = 0;
        psodesc.RTVFormats[0]    = DXGI_FORMAT_UNKNOWN;
    }

    psodesc.RasterizerState                    = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    psodesc.BlendState                         = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    psodesc.DepthStencilState                  = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
    psodesc.DepthStencilState.DepthEnable      = FALSE;
    psodesc.SampleMask                         = UINT_MAX;
    psodesc.PrimitiveTopologyType              = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    psodesc.SampleDesc                         = {1, 0};

    psodesc.InputLayout    = _shaders[type][AXIS_X]->GetInputLayout();
    psodesc.pRootSignature = _shaders[type][AXIS_X]->GetRootSignature();
    psodesc.VS             = _shaders[type][AXIS_X]->GetShaderByteCode(ShaderBuilder::Type::VS);
    psodesc.PS             = _shaders[type][AXIS_X]->GetShaderByteCode(ShaderBuilder::Type::PS);    

    hr = device->CreateGraphicsPipelineState(&psodesc, IID_PPV_ARGS(&_pipelineStates[type][AXIS_X]));
    FAILED_CHECK_MESSAGE(hr, L"GaussianBlurModule::Initialize device->CreateGraphicsPipelineState Failed");

    psodesc.InputLayout    = _shaders[type][AXIS_Y]->GetInputLayout();
    psodesc.pRootSignature = _shaders[type][AXIS_Y]->GetRootSignature();
    psodesc.VS             = _shaders[type][AXIS_Y]->GetShaderByteCode(ShaderBuilder::Type::VS);
    psodesc.PS             = _shaders[type][AXIS_Y]->GetShaderByteCode(ShaderBuilder::Type::PS);

    hr = device->CreateGraphicsPipelineState(&psodesc, IID_PPV_ARGS(&_pipelineStates[type][AXIS_Y]));
    FAILED_CHECK_MESSAGE(hr, L"GaussianBlurModule::Initialize device->CreateGraphicsPipelineState Failed");
}
