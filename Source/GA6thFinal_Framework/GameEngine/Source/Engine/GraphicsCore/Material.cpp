#include "pch.h"
#include "Material.h"
#include "Shader.h"

Material::Material() {}

Material::~Material() {}

void Material::Create()
{
    std::unique_ptr<Shader> shader = std::make_unique<Shader>();

    shader->BeginBuild();
    shader->LoadShader(L"../Shaders/vs_fr.hlsl", Shader::Type::VS);
    shader->LoadShader(L"../Shaders/ps_fr.hlsl", Shader::Type::PS);
    shader->EndBuild();

    CD3DX12_RASTERIZER_DESC    rasterizer{D3D12_DEFAULT};
    CD3DX12_BLEND_DESC         blend{D3D12_DEFAULT};
    CD3DX12_DEPTH_STENCIL_DESC depthstencil{D3D12_DEFAULT};

    D3D12_GRAPHICS_PIPELINE_STATE_DESC psd = {};
    psd.pRootSignature                     = shader->GetRootSignature().Get();
    psd.VS                                 = shader->GetShaderByteCode(Shader::Type::VS);
    psd.PS                                 = shader->GetShaderByteCode(Shader::Type::PS);

    psd.BlendState        = blend;
    psd.SampleMask        = UINT_MAX;
    psd.RasterizerState   = rasterizer;
    psd.DepthStencilState = depthstencil;
    psd.InputLayout       = shader->GetInputLayout();

    psd.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    psd.NumRenderTargets      = 1;
    psd.RTVFormats[0]         = UmDevice.GetMode().Format;
    psd.DSVFormat             = DXGI_FORMAT_D24_UNORM_S8_UINT; // 임시 포맷
    psd.SampleDesc            = {1, 0};


    ComPtr<ID3D12Device> device = UmDevice.GetDevice();

    HRESULT hr = device->CreateGraphicsPipelineState(&psd, IID_PPV_ARGS(_pipeLineState.GetAddressOf()));
    FAILED_CHECK_BREAK(hr);
}
