#include "pch.h"
#include "M_Default.h"

void M_Default::Initialize()
{    
    D3D12_RASTERIZER_DESC rd = {};
    rd.FillMode              = D3D12_FILL_MODE_SOLID;
    rd.CullMode              = D3D12_CULL_MODE_BACK;
    // rd.FrontCounterClockwise = false;
    rd.DepthClipEnable = TRUE;

    D3D12_BLEND_DESC bd                      = {};
    bd.RenderTarget[0].BlendEnable           = FALSE;
    bd.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

    D3D12_DEPTH_STENCIL_DESC dsd = {};
    dsd.DepthEnable              = TRUE;
    dsd.DepthWriteMask           = D3D12_DEPTH_WRITE_MASK_ALL;
    dsd.DepthFunc                = D3D12_COMPARISON_FUNC_LESS;
    dsd.StencilEnable            = FALSE;

    D3D12_GRAPHICS_PIPELINE_STATE_DESC psd = {};
    psd.pRootSignature                     = _shader->GetRootSignature().Get();
    psd.VS                                 = _shader->GetShaderByteCode(Shader::Type::VS);
    psd.PS                                 = _shader->GetShaderByteCode(Shader::Type::PS);
    // psd.GS = gs;
    // psd.StreamOutput		= {};
    psd.BlendState        = bd;
    psd.SampleMask        = UINT_MAX;
    psd.RasterizerState   = rd;
    psd.DepthStencilState = dsd;
    psd.InputLayout       = _shader->GetInputLayout();
    // psd.IBStripCutValue	= 0;
    psd.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    psd.NumRenderTargets      = 1;
    psd.RTVFormats[0]         = UmDevice.GetMode().Format;
    psd.DSVFormat             = DXGI_FORMAT_D24_UNORM_S8_UINT; // 임시 포맷
    // psd.DSVFormat = 0;						   //DS 포멧. (미지정시 스왑체인에 등록된 DS 버퍼의 포멧 값 사용)
    psd.SampleDesc = {1, 0}; // AA 설정.
    // psd.NodeMask 			= 0;
    // psd.CachedPSO 		= nullptr;
    // psd.Flags 			= D3D12_PIPELINE_STATE_FLAG_NONE;

    // 첫번째 렌더링 상태 객체 : "Solid" (기본상태)
    ComPtr<ID3D12Device> device = UmDevice.GetDevice();

    hr = device->CreateGraphicsPipelineState(&psd, IID_PPV_ARGS(_pipelineState[0].GetAddressOf()));
    FAILED_CHECK_BREAK(hr);

    psd.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
    hr = device->CreateGraphicsPipelineState(&psd, IID_PPV_ARGS(_pipelineState[1].GetAddressOf()));
    FAILED_CHECK_BREAK(hr);

    return hr;
}

void M_Default::Render()
{
}
