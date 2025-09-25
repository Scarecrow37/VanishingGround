#include "pch.h"
#include "SSGITechnique.h"
#include "RenderScene.h"

SSGITechnique::SSGITechnique() {}

SSGITechnique::~SSGITechnique() {}

void SSGITechnique::Initialize(ID3D12GraphicsCommandList* commandList)
{
    _motionVectorTex2D = std::make_shared<UnorderedAccessView>();
    _GIHalf2D[0]       = std::make_shared<UnorderedAccessView>();
    _GIHalf2D[1]       = std::make_shared<UnorderedAccessView>();

    auto res = Global::device->GetResolution();
    // motion vector
    auto desc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R32G32B32A32_FLOAT, res.cx, res.cy, 1, 0, 1, 0,
                                             D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);
    _motionVectorTex2D->InitializeAsTexture(desc, UnorderedAccessView::UAVSliceType::PER_MIP, true,
                                            D3D12_SRV_DIMENSION_TEXTURE2D);

    desc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R32G32B32A32_FLOAT, (res.cx / 2), (res.cy / 2), 1, 0, 1, 0,
                                        D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);
    _GIHalf2D[0]->InitializeAsTexture(desc, UnorderedAccessView::UAVSliceType::PER_MIP, true,
                                      D3D12_SRV_DIMENSION_TEXTURE2D);
    _GIHalf2D[1]->InitializeAsTexture(desc, UnorderedAccessView::UAVSliceType::PER_MIP, true,
                                      D3D12_SRV_DIMENSION_TEXTURE2D);
}

void SSGITechnique::Execute(ID3D12GraphicsCommandList* commandList)
{
    RenderTechnique::Execute(commandList);

    XMMATRIX view = _ownerScene->_camera->GetViewMatrix();
    XMMATRIX proj = _ownerScene->_camera->GetProjectionMatrix();

    _prevVP    = XMMatrixMultiply(view, proj);
    _currIndex = !_currIndex;
}
