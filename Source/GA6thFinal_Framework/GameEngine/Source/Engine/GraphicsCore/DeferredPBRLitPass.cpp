#include "pch.h"
#include "DeferredPBRLitPass.h"
#include "ShaderBuilder.h"
#include "RenderTarget.h"
#include "RenderScene.h"
#include "Quad.h"
DeferredPBRLitPass::~DeferredPBRLitPass() {}

void DeferredPBRLitPass::Initialize(const D3D12_VIEWPORT& viewPort, const D3D12_RECT& sissorRect)
{
    __super::Initialize(viewPort, sissorRect);

    D3D12_DESCRIPTOR_HEAP_DESC desc{.Type           = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
                                    .NumDescriptors = 7,
                                    .Flags          = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE,
                                    .NodeMask       = 0};

    UmDevice.GetDevice().Get()->CreateDescriptorHeap(&desc, IID_PPV_ARGS(_srvDescriptorHeap.GetAddressOf()));
    InitShaderAndPSO();
}

void DeferredPBRLitPass::Begin(ID3D12GraphicsCommandList* commandList)
{
    ComPtr<ID3D12Resource> rt = _ownerScene->_meshLightingTarget->GetResource();
    CD3DX12_RESOURCE_BARRIER br = CD3DX12_RESOURCE_BARRIER::Transition(
        rt.Get(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET);
    commandList->ResourceBarrier(1, &br);
  
    commandList->OMSetRenderTargets(1, &_ownerScene->_meshLightingTarget->GetRTVHandle(), FALSE, nullptr);

    commandList->RSSetViewports(1, &_viewPort);
    commandList->RSSetScissorRects(1, &_sissorRect);
}

void DeferredPBRLitPass::End(ID3D12GraphicsCommandList* commandList) 
{
    ComPtr<ID3D12Resource>   rt = _ownerScene->_meshLightingTarget->GetResource();
    CD3DX12_RESOURCE_BARRIER br = CD3DX12_RESOURCE_BARRIER::Transition(rt.Get(), D3D12_RESOURCE_STATE_RENDER_TARGET,
                                                                       D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    commandList->ResourceBarrier(1, &br);
}

void DeferredPBRLitPass::Draw(ID3D12GraphicsCommandList* commandList)
{
    // 사용할 gbuffer 복사 descriptor 복사
    ComPtr<ID3D12Device> device = UmDevice.GetDevice();
    auto                 dest   = _srvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
    UINT                 descriptorSize = UmDevice.GetCBVSRVUAVDescriptorSize();

    for (UINT i = 0; i <= RenderScene::GBuffer::CUSTOMDEPTH; ++i)
    {
        D3D12_CPU_DESCRIPTOR_HANDLE destHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(dest, i, descriptorSize);
        device->CopyDescriptorsSimple(1, destHandle, _ownerScene->_gBuffer[i]->GetSRVHandle(),
                                      D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    }

    ID3D12DescriptorHeap* hps[] = {
        _srvDescriptorHeap.Get(),
    };
    // 디스크립터-힙 설정.
    commandList->SetDescriptorHeaps(_countof(hps), hps);
    commandList->SetGraphicsRootSignature(_shader->GetRootSignature().Get());
    commandList->SetGraphicsRootConstantBufferView(_shader->GetRootSignatureIndex("cameraData"),
                                                   _ownerScene->_cameraBuffer->GetGPUVirtualAddress());
    D3D12_GPU_DESCRIPTOR_HANDLE gbuffer = _srvDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
    commandList->SetGraphicsRootDescriptorTable(_shader->GetRootSignatureIndex("gBuffers"), gbuffer);
    // pso 세팅
    commandList->SetPipelineState(_pipelineState.Get());
    //quad draw하기
    _ownerScene->_frameQuad->Render(commandList);
}

void DeferredPBRLitPass::InitShaderAndPSO()
{
    _shader = std::make_shared<ShaderBuilder>();
    _shader->BeginBuild();
    _shader->SetShader(L"../Shaders/vs_quad.hlsl", ShaderBuilder::Type::VS);
    _shader->SetShader(L"../Shaders/ps_pbr_lighting.hlsl", ShaderBuilder::Type::PS);
    _shader->EndBuild();

    ComPtr<ID3D12Device>               device = UmDevice.GetDevice();
    D3D12_GRAPHICS_PIPELINE_STATE_DESC psodesc;
    HRESULT                            hr = S_OK;
    ZeroMemory(&psodesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
    psodesc.RasterizerState          = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    psodesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
    psodesc.BlendState               = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    psodesc.DepthStencilState        = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
    //depth 안쓸거임
    psodesc.DepthStencilState.DepthEnable = FALSE;
    psodesc.SampleMask               = UINT_MAX;
    psodesc.PrimitiveTopologyType    = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    psodesc.InputLayout              = _shader->GetInputLayout();
    psodesc.NumRenderTargets         = 1;
    psodesc.RTVFormats[0]            = DXGI_FORMAT_R32G32B32A32_FLOAT;
    psodesc.pRootSignature           = _shader->GetRootSignature().Get();
    psodesc.SampleDesc               = {1, 0};
    psodesc.VS                       = _shader->GetShaderByteCode(ShaderBuilder::Type::VS);
    psodesc.PS                       = _shader->GetShaderByteCode(ShaderBuilder::Type::PS);
    hr = device->CreateGraphicsPipelineState(&psodesc, IID_PPV_ARGS(_pipelineState.GetAddressOf()));
    FAILED_CHECK_BREAK(hr);
}
