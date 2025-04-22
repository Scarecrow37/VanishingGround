#include "pch.h"
#include "PBRTechnique.h"
#include "Shader.h"
#include "PBRPass.h"
#include "RenderScene.h"
#include "RenderTarget.h"

PBRTechnique::PBRTechnique() {}

PBRTechnique::~PBRTechnique() {}

void PBRTechnique::Initalize() 
{
    InitalizePBRLightPass();
}

void PBRTechnique::Execute(ComPtr<ID3D12GraphicsCommandList> commadList) 
{
    __super::Execute(commadList);
}

void PBRTechnique::InitalizePBRLightPass()
{
    std::shared_ptr<Shader> pbrShader = std::make_shared<Shader>();
    pbrShader->BeginBuild();
    pbrShader->LoadShader(L"../Shaders/vs_fr.hlsl", Shader::Type::VS);
    pbrShader->LoadShader(L"../Shaders/ps_pbr.hlsl", Shader::Type::PS);
    pbrShader->EndBuild();
    std::shared_ptr<PBRPass> pbrPass = std::make_shared<PBRPass>();
    pbrPass->SetClearValue(Color(0.f, 0.f, 0.f, 1.f));
    ComPtr<ID3D12PipelineState> pso;

    D3D12_GRAPHICS_PIPELINE_STATE_DESC psd{};
    psd.pRootSignature        = pbrShader->GetRootSignature().Get();
    psd.VS                    = pbrShader->GetShaderByteCode(Shader::Type::VS);
    psd.PS                    = pbrShader->GetShaderByteCode(Shader::Type::PS);
    psd.InputLayout           = pbrShader->GetInputLayout();
    psd.RasterizerState       = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    psd.BlendState            = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    psd.DepthStencilState     = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
    psd.SampleMask            = UINT_MAX;
    psd.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    psd.NumRenderTargets      = 1;
    psd.RTVFormats[0]         = DXGI_FORMAT_R32G32B32A32_FLOAT;
    psd.DSVFormat             = DXGI_FORMAT_D24_UNORM_S8_UINT;
    psd.SampleDesc            = {1, 0}; // AA 설정.
    UmDevice.GetDevice()->CreateGraphicsPipelineState(&psd, IID_PPV_ARGS(pso.GetAddressOf()));
    pbrPass->SetPipelineState(pso);
    pbrPass->SetShader(pbrShader);
    D3D12_VIEWPORT viewport{.TopLeftX = 0,
                            .TopLeftY = 0,
                            .Width    = (FLOAT)UmDevice.GetMode().Width,
                            .Height   = (FLOAT)UmDevice.GetMode().Height,
                            .MinDepth = 0.f,
                            .MaxDepth = 1.f};
    D3D12_RECT     scissor{.left   = 0,
                           .top    = 0,
                           .right  = (LONG)UmDevice.GetMode().Width,
                           .bottom = (LONG)UmDevice.GetMode().Height};
    pbrPass->Initialize(viewport, scissor);

    // notice : 강조!! 여기 코드 참고.
    RenderPass::DescriptorSet descriptorSet;
    UINT                      rtCount = 1;
    descriptorSet.CreateDescriptorSet(rtCount, _ownerScene->_depthStencilHandle, _ownerScene->_depthStencilBuffer,
                                      _ownerScene, true);
    pbrPass->SetOwnerScene(_ownerScene);
    pbrPass->SetDescriptors(descriptorSet);

    AddRenderPass(pbrPass);
}
