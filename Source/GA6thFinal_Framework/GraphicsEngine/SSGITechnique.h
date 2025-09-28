#pragma once
#include "RenderTechnique.h"

class SSGITechnique : public RenderTechnique
{
public:
    SSGITechnique();
    virtual ~SSGITechnique();

public:
    void Initialize(ID3D12GraphicsCommandList* commandList) override;
    void Execute(ID3D12GraphicsCommandList* commandList) override;

public:
    ConstantBufferView* GetConstantBufferView() { return _constantBuffer.get(); }

public:
    std::shared_ptr<UnorderedAccessView> _motionVectorTex2D;
    // prev, half 돌려쓸 예정
    std::shared_ptr<UnorderedAccessView> _GIHalf2D[2];
    std::shared_ptr<UnorderedAccessView> _GITemporalHalf;
    SharedResource<RenderTarget>         _finalGITex;
    bool                                 _currIndex = false;

private:
    void UpdateConstantBuffer();

private:
    std::shared_ptr<ConstantBufferView> _constantBuffer;

    XMMATRIX _prevVP{XMMatrixIdentity()};
};
