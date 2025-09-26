#include "pch.h"
#include "GenerateSSGIPass.h"
#include "ShaderBuilder.h"
#include "SSGITechnique.h"

GenerateSSGIPass::~GenerateSSGIPass() {}

void GenerateSSGIPass::Initialize(RenderScene* ownerScene, RenderTechnique* ownerTechnique,
                                  ID3D12GraphicsCommandList* commandList)
{
    RenderPass::Initialize(ownerScene, ownerTechnique, commandList);
    InitShaderAnsPSO();
    _ssgiTech = dynamic_cast<SSGITechnique*>(ownerTechnique);
}


void GenerateSSGIPass::Begin(ID3D12GraphicsCommandList* commandList) 
{
    commandList->SetPipelineState(_pipelineState.Get());
    commandList->SetComputeRootSignature(_shader->GetRootSignature());
}

void GenerateSSGIPass::Draw(ID3D12GraphicsCommandList* commandList)
{
    D3D12_GPU_VIRTUAL_ADDRESS giData       = _ssgiTech->GetConstantBufferView()->GetGPUVirtualAddress();
    D3D12_GPU_VIRTUAL_ADDRESS cameraData   = _ownerScene->_cameraBuffer->GetGPUVirtualAddress();
    int                       currentIndex = _ssgiTech->_currIndex;
    currentIndex                           = 0;
    auto                      currGITex    = _ssgiTech->_GIHalf2D[currentIndex];
    const auto&               gbuffers     = Global::multiRenderTargetManager->GetRenderTargetGroup("G-Buffer");
    SIZE                      res          = Global::device->GetResolution();
    SIZE                      halfRes      = SIZE((res.cx / 2.f), (res.cy / 2.f));
    commandList->SetComputeRootConstantBufferView(_shader->GetRootParameterIndex("ssgiData"), giData);
    commandList->SetComputeRootConstantBufferView(_shader->GetRootParameterIndex("cameraData"), cameraData);
    commandList->SetComputeRootDescriptorTable(_shader->GetRootParameterIndex("screenDepth"),
                                               gbuffers[GBuffer::DEPTH]->GetSRVHandle());
    commandList->SetComputeRootDescriptorTable(_shader->GetRootParameterIndex("screenNormal"),
                                               gbuffers[GBuffer::NORMAL]->GetSRVHandle());
    commandList->SetComputeRootDescriptorTable(_shader->GetRootParameterIndex("screenAlbedo"),
                                               gbuffers[GBuffer::BASECOLOR]->GetSRVHandle());
    commandList->SetComputeRootDescriptorTable(_shader->GetRootParameterIndex("screenColor"),
                                               _meshRenderTarget->GetSRVHandle());
    commandList->SetComputeRootDescriptorTable(_shader->GetRootParameterIndex("SSGI_Result"),
                                               currGITex->GetUAVHandle());
    commandList->Dispatch((halfRes.cx + 15) / 16, (halfRes.cy + 15) / 16, 1);
}

void GenerateSSGIPass::AddRenderPassDatas(std::string_view sceneName)
{
    Global::renderPassDatas->AddRenderPassImage(sceneName, "GenerateSSGIPass", "SSGITexture",
                                                _ssgiTech->_GIHalf2D[0]->GetSRVHandle());
}

void GenerateSSGIPass::InitShaderAnsPSO() 
{
    _shader = std::make_unique<ShaderBuilder>();
    _shader->BeginBuild();
    _shader->SetShader(L"../Shaders/cs_generate_ssgi.hlsl", ShaderBuilder::Type::CS);
    _shader->EndBuild();
    
    D3D12_COMPUTE_PIPELINE_STATE_DESC psoDesc{};
    psoDesc.pRootSignature = _shader->GetRootSignature();
    psoDesc.CS             = _shader->GetShaderByteCode(ShaderBuilder::Type::CS);
    psoDesc.Flags          = D3D12_PIPELINE_STATE_FLAG_NONE;

    HRESULT hr =
        Global::device->GetDevice()->CreateComputePipelineState(&psoDesc, IID_PPV_ARGS(_pipelineState.GetAddressOf()));
    FAILED_CHECK_MESSAGE(hr, L"GenerateSSGIPass::InitShaderAndPSO Failed");
}
