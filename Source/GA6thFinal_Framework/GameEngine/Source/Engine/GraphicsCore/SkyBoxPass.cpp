#include "pch.h"
#include "SkyBoxPass.h"
#include "SkyBox.h"
#include "RenderTarget.h"
#include "RenderScene.h"

SkyBoxPass::SkyBoxPass() {}

SkyBoxPass::~SkyBoxPass() {}

void SkyBoxPass::Initialize() 
{
    __super::Initialize();

    _skyBox = _ownerScene->GetSkyBox();
    InitShaderAndPSO();
    _skyBox->Initialize();
}

void SkyBoxPass::Begin(ID3D12GraphicsCommandList* commandList) 
{
    commandList->OMSetRenderTargets(1, &_meshRenderTarget->GetRTVHandle(), FALSE, nullptr);
    commandList->RSSetViewports(1, &_meshRenderTarget->GetViewPort());
    commandList->RSSetScissorRects(1, &_meshRenderTarget->GetScissorRect());
}

void SkyBoxPass::Draw(ID3D12GraphicsCommandList* commandList)
{
    bool isActive = _skyBox->HasTexture();
    if (isActive)
    {
        commandList->SetGraphicsRootSignature(_shader->GetRootSignature());
        commandList->SetGraphicsRootConstantBufferView(_shader->GetRootParameterIndex("cameraData"), _ownerScene->_cameraBuffer->GetGPUVirtualAddress());
        commandList->SetPipelineState(_pipelineState.Get());
        _skyBox->Render(commandList, _shader->GetRootParameterIndex("evnTexture"));
    }
}

void SkyBoxPass::End(ID3D12GraphicsCommandList* commandList) 
{
}

void SkyBoxPass::InitShaderAndPSO() 
{
    _shader = std::make_unique<ShaderBuilder>();
    _shader->BeginBuild();
    _shader->SetShader(L"../Shaders/vs_skybox.hlsl", ShaderBuilder::Type::VS);
    _shader->SetShader(L"../Shaders/ps_skybox.hlsl", ShaderBuilder::Type::PS);
    _shader->EndBuild();

    ID3D12Device*                      device = UmDevice.GetDevice();
    HRESULT                            hr = S_OK;

    D3D12_GRAPHICS_PIPELINE_STATE_DESC psodesc{};
    psodesc.RasterizerState               = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
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
    FAILED_CHECK_MESSAGE(hr, L"SkyBoxPass::InitShaderAndPSO device->CreateGraphicsPipelineState Failed");
}
