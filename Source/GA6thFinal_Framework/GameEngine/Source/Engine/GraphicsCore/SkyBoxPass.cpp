#include "pch.h"
#include "SkyBoxPass.h"
#include "SkyBox.h"
#include "RenderTarget.h"
#include "RenderScene.h"
#include "ShaderBuilder.h"

SkyBoxPass::SkyBoxPass() {}

SkyBoxPass::~SkyBoxPass() {}

void SkyBoxPass::Initialize(const D3D12_VIEWPORT& viewPort, const D3D12_RECT& sissorRect) 
{
    _skyBox = _ownerScene->GetSkyBox();
    __super::Initialize(viewPort, sissorRect);
    InitShaderAndPSO();
    _skyBox->Initialize();

    File::Path fileName  = L"../../../Resource/Assets/skybox/kloppenheim_05_puresky_4k.hdr";
    File::Path assetPath = UmFileSystem.GetAssetPath();
    File::Path result    = assetPath / fileName;
    _skyBox->SetTexture(result.string());
}

void SkyBoxPass::Begin(ID3D12GraphicsCommandList* commandList) 
{
    commandList->OMSetRenderTargets(1, &_ownerScene->_meshLightingTarget->GetRTVHandle(), FALSE, nullptr);

    commandList->RSSetViewports(1, &_viewPort);
    commandList->RSSetScissorRects(1, &_sissorRect);
}

void SkyBoxPass::End(ID3D12GraphicsCommandList* commandList) 
{
    ComPtr<ID3D12Resource>   rt = _ownerScene->_meshLightingTarget->GetResource();
    CD3DX12_RESOURCE_BARRIER br = CD3DX12_RESOURCE_BARRIER::Transition(rt.Get(), D3D12_RESOURCE_STATE_RENDER_TARGET,
                                                                       D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    commandList->ResourceBarrier(1, &br);
}

void SkyBoxPass::Draw(ID3D12GraphicsCommandList* commandList) 
{
    _skyBox->SetDescriptorHeap(commandList);
    commandList->SetGraphicsRootSignature(_shader->GetRootSignature().Get());
    commandList->SetGraphicsRootConstantBufferView(_shader->GetRootSignatureIndex("cameraData"),
                                                   _ownerScene->_cameraBuffer->GetGPUVirtualAddress());
    commandList->SetPipelineState(_pipelineState.Get());
    _skyBox->Render(commandList, _shader->GetRootSignatureIndex("evnTexture"));
}

void SkyBoxPass::InitShaderAndPSO() 
{
    _shader = std::make_shared<ShaderBuilder>();
    _shader->BeginBuild();
    _shader->SetShader(L"../Shaders/vs_skybox.hlsl", ShaderBuilder::Type::VS);
    _shader->SetShader(L"../Shaders/ps_skybox.hlsl", ShaderBuilder::Type::PS);
    _shader->EndBuild();

    ComPtr<ID3D12Device>               device = UmDevice.GetDevice();
    D3D12_GRAPHICS_PIPELINE_STATE_DESC psodesc;
    HRESULT                            hr = S_OK;
    ZeroMemory(&psodesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
    psodesc.RasterizerState          = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    psodesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
    psodesc.BlendState               = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    psodesc.DepthStencilState        = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
    // depth 안쓸거임
    psodesc.DepthStencilState.DepthEnable = FALSE;
    psodesc.SampleMask                    = UINT_MAX;
    psodesc.PrimitiveTopologyType         = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    psodesc.InputLayout                   = _shader->GetInputLayout();
    psodesc.NumRenderTargets              = 1;
    psodesc.RTVFormats[0]                 = DXGI_FORMAT_R32G32B32A32_FLOAT;
    psodesc.pRootSignature                = _shader->GetRootSignature().Get();
    psodesc.SampleDesc                    = {1, 0};
    psodesc.VS                            = _shader->GetShaderByteCode(ShaderBuilder::Type::VS);
    psodesc.PS                            = _shader->GetShaderByteCode(ShaderBuilder::Type::PS);
    hr = device->CreateGraphicsPipelineState(&psodesc, IID_PPV_ARGS(_pipelineState.GetAddressOf()));
    FAILED_CHECK_BREAK(hr);
}
