#pragma once
#include "RenderPass.h"
class ParticleRibbonPass : public RenderPass
{
public:
    ParticleRibbonPass();
    virtual ~ParticleRibbonPass();

public:
    void SetAccumulationBuffers(SharedResource<UnorderedAccessView> color, SharedResource<UnorderedAccessView> alpha);

public:
    void Initialize(RenderScene* ownerScene, RenderTechnique* ownerTechnique, ID3D12GraphicsCommandList* commandList);
    void Begin(ID3D12GraphicsCommandList* commandList) override;
    void Draw(ID3D12GraphicsCommandList* commandList) override;
    void End(ID3D12GraphicsCommandList* commandList) override;

private:
    void InitializeShaderAndPSO();

private:
    static constexpr int MAX_SEGMENTS = 100;
    static constexpr int MAX_RIBBON_INDEX = 100000;

    SharedResource<UnorderedAccessView> _accumulateBuffer;
    SharedResource<UnorderedAccessView> _revealageBuffer;

    std::vector<UINT>                  _albedoTextureIDs;
    std::unique_ptr<StructuredBuffer> _textureIDBuffer;

    std::vector<std::vector<UINT>>                 _ribbonIndices;
    std::vector<std::unique_ptr<StructuredBuffer>> _ribbonIndexBuffer;

    FX<GE::VS::PARTICLE_RIBBON, GE::PS::PARTICLE_QUAD> _fx;

};
