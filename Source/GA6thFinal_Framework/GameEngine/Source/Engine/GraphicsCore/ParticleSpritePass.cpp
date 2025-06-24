#include "pch.h"
#include "ParticleSpritePass.h"
#include "ShaderBuilder.h"
#include "RenderTarget.h"
#include "RenderScene.h"
#include "Model.h"
#include "Texture.h"
#include "Quad.h"
#include "StructuredBuffer.h"
#include "UnorderedAccessView.h"

 ParticleSpritePass::ParticleSpritePass() {}

ParticleSpritePass::~ParticleSpritePass() {}

void ParticleSpritePass::Initialize(const D3D12_VIEWPORT& viewPort, const D3D12_RECT& sissorRect)
{
    __super::Initialize(viewPort, sissorRect);
    _particleQuad = UmResourceManager.LoadResource<Model>("Quad");

    
    InitializeShader();
    InitializePSO();

    //refactoring needed
    _particleRenderCommandList = UmParticleManager.GetRenderCommandList();
    _albedoTextureIDs = std::vector<int>(100,-1);


    UINT buffersize = 100 * sizeof(int);
        // 상수 버퍼는 항상 256바이트 정렬되어야 함
    UINT alignedBufferSize = (buffersize + 255) & ~255;

    D3D12_RESOURCE_DESC bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(buffersize);

    // 2. Upload Heap에 업로드 버퍼 생성 (CPU 접근 가능)
    auto uploadProperty = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);

    FAILED_CHECK_MESSAGE(
        UmDevice.GetDevice()->CreateCommittedResource(&uploadProperty, D3D12_HEAP_FLAG_NONE, &bufferDesc,
                                                      D3D12_RESOURCE_STATE_GENERIC_READ, // Upload Heap 필수 상태
                                                      nullptr, IID_PPV_ARGS(&_textureIdConstantBuffer)),
        L"");

    _textureIDBuffer = std::make_unique<StructuredBuffer>();
    _textureIDBuffer->Initialize(sizeof(int), 100);
}

void ParticleSpritePass::Begin(ID3D12GraphicsCommandList* commandlist)
{
    auto customDepthTarget = UmMultiRenderTargetManager.GetRenderTarget("CustomDepth");
    customDepthTarget->TransitionResource(_particleRenderCommandList, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
                                          D3D12_RESOURCE_STATE_RENDER_TARGET);
    auto br = CD3DX12_RESOURCE_BARRIER::Transition(_ownerScene->_depthStencilBuffer.Get(), D3D12_RESOURCE_STATE_PRESENT,
                                                   D3D12_RESOURCE_STATE_DEPTH_WRITE);
    _particleRenderCommandList->ResourceBarrier(1, &br);

    _particleRenderCommandList->OMSetRenderTargets(1, &customDepthTarget->GetRTVHandle(), FALSE,
                                                   &_ownerScene->_depthStencilHandle);

    _particleRenderCommandList->RSSetViewports(1, &_viewPort);
    _particleRenderCommandList->RSSetScissorRects(1, &_sissorRect);

    ComPtr<ID3D12Resource> resource = UmParticleManager.GetComputeOutputResource();

    CD3DX12_RESOURCE_BARRIER computeOutputBarrior = CD3DX12_RESOURCE_BARRIER::Transition(
        resource.Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);

    _particleRenderCommandList->ResourceBarrier(1, &computeOutputBarrior);


    auto albedoTextures = UmParticleManager.GetActiveAlbedos();
    std::fill(_albedoTextureIDs.begin(), _albedoTextureIDs.end(), -1);
    for (int i = 0; i < albedoTextures.size(); ++i)
    {
        _albedoTextureIDs[i] = albedoTextures[i]->GetID();
    }

    _textureIDBuffer->CopyStructuredBuffer(_particleRenderCommandList, _albedoTextureIDs.data(),
                                           static_cast<UINT>(albedoTextures.size() * sizeof(int)));
}

void ParticleSpritePass::End(ID3D12GraphicsCommandList* commandlist)
{     
    ComPtr<ID3D12Resource>   resource             = UmParticleManager.GetComputeOutputResource();
    CD3DX12_RESOURCE_BARRIER computeOutputBarrior = CD3DX12_RESOURCE_BARRIER::Transition(
        resource.Get(), D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_COMMON);

    _particleRenderCommandList->ResourceBarrier(1, &computeOutputBarrior);

    _accumlateBuffer->TransitionResource(_particleRenderCommandList, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    _revealageBuffer->TransitionResource(_particleRenderCommandList, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
}

void ParticleSpritePass::Draw(ID3D12GraphicsCommandList* commandlist)
{
    auto customDepthTarget = UmMultiRenderTargetManager.GetRenderTarget("CustomDepth");

    ComPtr<ID3D12Device>  device = UmDevice.GetDevice();
    ID3D12DescriptorHeap* hps[]  = {
        UmViewManager.GetShaderResourceHeap(),
    };
     _particleRenderCommandList->SetDescriptorHeaps(_countof(hps), hps);
    _accumlateBuffer->ClearUnorderedAccessView(_particleRenderCommandList);
    _revealageBuffer->ClearUnorderedAccessView(_particleRenderCommandList);

    _particleRenderCommandList->SetGraphicsRootSignature(_spriteParticleShaderBuilder->GetRootSignature());

    _particleRenderCommandList->SetGraphicsRootShaderResourceView(
        _spriteParticleShaderBuilder->GetRootParameterIndex("texID"), _textureIDBuffer->GetGPUVirtualAddress());

    _particleRenderCommandList->SetGraphicsRootDescriptorTable(
        _spriteParticleShaderBuilder->GetRootParameterIndex("gAccumTex"), _accumlateBuffer->GetUAVHandle());
    _particleRenderCommandList->SetGraphicsRootDescriptorTable(
        _spriteParticleShaderBuilder->GetRootParameterIndex("gRevealTex"), _revealageBuffer->GetUAVHandle());

    auto outputResource = UmParticleManager.GetComputeOutputResource();
    _particleRenderCommandList->SetGraphicsRootShaderResourceView(
        _spriteParticleShaderBuilder->GetRootParameterIndex("particleInfo"), outputResource->GetGPUVirtualAddress());

    D3D12_GPU_DESCRIPTOR_HANDLE descHeapPtr =
        UmViewManager.GetShaderResourceHeap()->GetGPUDescriptorHandleForHeapStart();

    _particleRenderCommandList->SetGraphicsRootDescriptorTable(
        _spriteParticleShaderBuilder->GetRootParameterIndex("textures"), descHeapPtr);

    _particleRenderCommandList->SetPipelineState(_psos[0].Get());

    const auto& meshs      = _particleQuad->GetMeshes();
    UINT        totalCount = UmParticleManager.GetTotalCount();
    meshs[0]->Render(_particleRenderCommandList, totalCount);

}

void ParticleSpritePass::InitializeShader() 
{
    _spriteParticleShaderBuilder = std::make_shared<ShaderBuilder>();
    _spriteParticleShaderBuilder->BeginBuild();
    _spriteParticleShaderBuilder->SetShader(L"../Shaders/vs_particle_quad.hlsl", ShaderBuilder::Type::VS);
    _spriteParticleShaderBuilder->SetShader(L"../Shaders/ps_particle_quad.hlsl", ShaderBuilder::Type::PS);
    _spriteParticleShaderBuilder->EndBuild();
}

void ParticleSpritePass::InitializePSO() 
{
    // static two side.
    ComPtr<ID3D12Device>               device = UmDevice.GetDevice();
    D3D12_GRAPHICS_PIPELINE_STATE_DESC psodesc;
    HRESULT                            hr = S_OK;
    ComPtr<ID3D12PipelineState>        statictwosidedpso;
    ZeroMemory(&psodesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
    psodesc.RasterizerState                        = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    psodesc.RasterizerState.CullMode               = D3D12_CULL_MODE_NONE;
    psodesc.BlendState                             = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    //psodesc.BlendState                             = blendDesc;
    psodesc.DepthStencilState                      = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
    psodesc.DepthStencilState.DepthEnable          = FALSE;
    //psodesc.DSVFormat                              = DXGI_FORMAT_D24_UNORM_S8_UINT;

    psodesc.SampleMask                             = UINT_MAX;
    psodesc.PrimitiveTopologyType                  = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    psodesc.InputLayout                            = _spriteParticleShaderBuilder->GetInputLayout();
    psodesc.NumRenderTargets                       = 1;
    psodesc.RTVFormats[0]                          = DXGI_FORMAT_R32_UINT;
    //psodesc.DSVFormat                              = DXGI_FORMAT_D24_UNORM_S8_UINT;
    psodesc.pRootSignature                         = _spriteParticleShaderBuilder->GetRootSignature();
    psodesc.SampleDesc                             = {1, 0};
    psodesc.VS = _spriteParticleShaderBuilder->GetShaderByteCode(ShaderBuilder::Type::VS);
    psodesc.PS = _spriteParticleShaderBuilder->GetShaderByteCode(ShaderBuilder::Type::PS);
    hr         = device->CreateGraphicsPipelineState(&psodesc, IID_PPV_ARGS(statictwosidedpso.GetAddressOf()));
    FAILED_CHECK_MESSAGE(hr, L"");
    _psos.push_back(statictwosidedpso);

}



void ParticleSpritePass::SetAccumulationBuffers(UnorderedAccessView* color, UnorderedAccessView* alpha)
{
    _accumlateBuffer = color;
    _revealageBuffer  = alpha;
}