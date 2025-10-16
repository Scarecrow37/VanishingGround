#include "pch.h"
#include "SSGITechnique.h"
#include "RenderScene.h"

// render pass
#include "CalculateMotionVectorPass.h"
#include "GenerateSSGIPass.h"
#include "GITemporalPass.h"
#include "BilateralUpsamplePass.h"
#include "SSGICompositePass.h"

SSGITechnique::SSGITechnique() {}

SSGITechnique::~SSGITechnique() {}

void SSGITechnique::Initialize(ID3D12GraphicsCommandList* commandList)
{
    _motionVectorTex2D = std::make_shared<UnorderedAccessView>();
    _GIHalf2D[0]       = std::make_shared<UnorderedAccessView>();
    _GIHalf2D[1]       = std::make_shared<UnorderedAccessView>();
    _GITemporalHalf       = std::make_shared<UnorderedAccessView>();
    _constantBuffer    = std::make_shared<ConstantBufferView>();
    UINT size          = (sizeof(SSGIData) + 255) & ~255;
    _constantBuffer->Initialize(size);
    auto res = Global::device->GetResolution();
    // motion vector
    auto desc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R32G32_FLOAT, res.cx, res.cy, 1, 0, 1, 0,
                                             D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);
    _motionVectorTex2D->InitializeAsTexture(desc, UnorderedAccessView::UAVSliceType::PER_MIP, true,
                                            D3D12_SRV_DIMENSION_TEXTURE2D);

    desc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R32G32B32A32_FLOAT, (res.cx / 2), (res.cy / 2), 1, 0, 1, 0,
                                        D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);
    _GIHalf2D[0]->InitializeAsTexture(desc, UnorderedAccessView::UAVSliceType::PER_MIP, true,
                                      D3D12_SRV_DIMENSION_TEXTURE2D);
    _GIHalf2D[1]->InitializeAsTexture(desc, UnorderedAccessView::UAVSliceType::PER_MIP, true,
                                      D3D12_SRV_DIMENSION_TEXTURE2D);
    _GITemporalHalf->InitializeAsTexture(desc, UnorderedAccessView::UAVSliceType::PER_MIP, true,
                                         D3D12_SRV_DIMENSION_TEXTURE2D);

    _finalGITex = MakeSharedResource<RenderTarget>();
    desc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R32G32B32A32_FLOAT, res.cx, res.cy, 1,
                                             MAX_MIPMAP_LEVEL, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);

    _finalGITex->Initialize(desc, 0.247f);
    SSGIProperty property;
    property.Radius         = 0.01f;
    property.Thickness      = 0.05f;
    property.NumSample      = 16;
    property.Intensity      = 10.f;
    property.TemporalWeight = 0.85f;
    property.DepthSigma     = 2.f;
    property.NormalSigma    = 128.f;
    Global::renderPassDatas->AddRenderPassProperty("SSGIData", property);

    std::unique_ptr<RenderPass> pass;
    pass = std::make_unique<CalculateMotionVectorPass>();
    pass->Initialize(_ownerScene, this, commandList);
    AddRenderPass(std::move(pass));
    pass = std::make_unique<GenerateSSGIPass>();
    pass->Initialize(_ownerScene, this, commandList);
    AddRenderPass(std::move(pass));
    pass = std::make_unique<GITemporalPass>();
    pass->Initialize(_ownerScene, this, commandList);
    AddRenderPass(std::move(pass));
    pass = std::make_unique<BilateralUpsamplePass>();
    pass->Initialize(_ownerScene, this, commandList);
    AddRenderPass(std::move(pass));
    pass = std::make_unique<SSGICompositePass>();
    pass->Initialize(_ownerScene, this, commandList);
    AddRenderPass(std::move(pass));
}

void SSGITechnique::Execute(ID3D12GraphicsCommandList* commandList)
{
    RenderTechnique::Execute(commandList);
    UpdateConstantBuffer();
    _currIndex = !_currIndex;
}

void SSGITechnique::UpdateConstantBuffer()
{
    const auto& ssgiProperty = std::any_cast<const SSGIProperty&>(
        Global::renderPassDatas->GetRenderPassProperty("SSGIData"));

    XMMATRIX view        = _ownerScene->_camera->GetViewMatrix();
    XMMATRIX proj        = _ownerScene->_camera->GetProjectionMatrix();
    XMMATRIX viewProj    = XMMatrixMultiply(view, proj);
    XMMATRIX invViewProj = XMMatrixInverse(nullptr, viewProj);

    auto res = Global::device->GetResolution();

    // 상수 버퍼 관련 update
    SSGIData data;
    data.PreViewProj           = XMMatrixTranspose(_prevVP);
    data.InverseViewProjection = XMMatrixTranspose(invViewProj);
    data.ScreenSize            = Vector2((float)res.cx, (float)res.cy);
    data.Radius                = ssgiProperty.Radius;
    data.Thickness             = ssgiProperty.Thickness;
    data.NumSample             = ssgiProperty.NumSample;
    data.Intensity             = ssgiProperty.Intensity;
    data.TemporalWeight        = ssgiProperty.TemporalWeight;
    data.DepthSigma            = ssgiProperty.DepthSigma;
    data.NormalSigma           = ssgiProperty.NormalSigma;

    _constantBuffer->UpdateBuffer(&data);

    _prevVP    = viewProj;
}
