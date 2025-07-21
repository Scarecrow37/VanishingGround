#pragma once
#include "RenderTechnique.h"

class ParticleRenderTechnique : public RenderTechnique
{
public:
    ParticleRenderTechnique();
    virtual ~ParticleRenderTechnique();

public:
    void Initialize(ID3D12GraphicsCommandList* commandList) override;
    void Execute(ID3D12GraphicsCommandList* commandList) override;

private:
    void InitializeSpriteParticlePass(ID3D12GraphicsCommandList* commandList);
    void InitializeParticleResolvePass(ID3D12GraphicsCommandList* commandList);

    void CreateWBOITResources();  // 리소스 생성
    void ReleaseWBOITResources(); // 필요 시 자원 해제

    SharedResource<UnorderedAccessView> _accumlateBuffer;
    SharedResource<UnorderedAccessView> _revealageBuffer;
};
