#include "pch.h"
#include "UI2DPass.h"

UI2DPass::UI2DPass(const std::vector<UINT>& instanceIDs)
    : _instanceIDs(instanceIDs)
{
}

UI2DPass::~UI2DPass() {}

void UI2DPass::Initialize(RenderScene* ownerScene)
{
    __super::Initialize(ownerScene);

    const auto& mode = UmDevice.GetMode();

    _2DCamera = std::make_unique<Camera>();
    _2DCamera->SetupOrthographic((float)mode.Width, (float)mode.Height, 0.1f, 100.f);

    _cameraBuffer = std::make_unique<ConstantBufferView>();
    _cameraBuffer->Initialize(sizeof(CameraData));

    CameraData cameraData{.View              = XMMatrixTranspose(_2DCamera->GetViewMatrix()),
                          .Projection        = XMMatrixTranspose(_2DCamera->GetProjectionMatrix()),
                          .ViewInverse       = XMMatrixTranspose(_2DCamera->GetWorldMatrix()),
                          .ProejctionInverse = XMMatrixTranspose(_2DCamera->GetProjectionInverseMatrix()),
                          .Position          = Vector4(_2DCamera->GetPosition())};

    _cameraBuffer->UpdateBuffer(&cameraData);

    _shader = std::make_unique<ShaderBuilder>();
    _shader->BeginBuild();
    _shader->SetShader(L"../Shaders/vs_ui_fr.hlsl", ShaderBuilder::Type::VS);
    _shader->SetShader(L"../Shaders/ps_ui.hlsl", ShaderBuilder::Type::PS);
    _shader->EndBuild(ShaderBuilder::BindType::DIRECT);

    ID3D12Device*                      device  = UmDevice.GetDevice();
    D3D12_GRAPHICS_PIPELINE_STATE_DESC psodesc = {};
    psodesc.RasterizerState                    = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    psodesc.BlendState                         = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    psodesc.DepthStencilState                  = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
    psodesc.DepthStencilState.DepthEnable      = FALSE;
    psodesc.SampleMask                         = UINT_MAX;
    psodesc.PrimitiveTopologyType              = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    psodesc.InputLayout                        = _shader->GetInputLayout();
    psodesc.RTVFormats[0]                      = DXGI_FORMAT_R32G32B32A32_FLOAT;
    psodesc.NumRenderTargets                   = 1;
    psodesc.pRootSignature                     = _shader->GetRootSignature();
    psodesc.SampleDesc                         = {1, 0};
    psodesc.VS                                 = _shader->GetShaderByteCode(ShaderBuilder::Type::VS);
    psodesc.PS                                 = _shader->GetShaderByteCode(ShaderBuilder::Type::PS);

    HRESULT hr = S_OK;
    hr         = device->CreateGraphicsPipelineState(&psodesc, IID_PPV_ARGS(&_pipelineState));
    FAILED_CHECK_MESSAGE(hr, L"UI2DPass::Initialize device->CreateGraphicsPipelineState Failed");
}

void UI2DPass::Begin(ID3D12GraphicsCommandList* commandList)
{    
    _finalRenderTarget->TransitionResource(commandList, D3D12_RESOURCE_STATE_RENDER_TARGET);
    _finalRenderTarget->ClearRenderTarget(commandList);

    commandList->OMSetRenderTargets(1, &_finalRenderTarget->GetRTVHandle(), FALSE, nullptr);
    commandList->RSSetViewports(1, &_finalRenderTarget->GetViewPort());
    commandList->RSSetScissorRects(1, &_finalRenderTarget->GetScissorRect());
}

void UI2DPass::Draw(ID3D12GraphicsCommandList* commandList)
{
    commandList->SetPipelineState(_pipelineState.Get());
    commandList->SetGraphicsRootSignature(_shader->GetRootSignature());

    commandList->SetGraphicsRootDescriptorTable(_shader->GetRootParameterIndex("screenTexture"), _meshRenderTarget->GetSRVHandle());

    _ownerScene->_frameQuad->Render(commandList, _instanceIDs.size());
}

void UI2DPass::End(ID3D12GraphicsCommandList* commandList)
{    
    _finalRenderTarget->TransitionResource(commandList, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
}