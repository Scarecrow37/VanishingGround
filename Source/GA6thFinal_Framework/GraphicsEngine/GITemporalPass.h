#pragma once
#include "RenderPass.h"
class GITemporalPass : public RenderPass
{
public:
    GITemporalPass() = default;
    virtual ~GITemporalPass();

public:
    void Initialize(RenderScene* ownerScene, RenderTechnique* ownerTechnique,
                    ID3D12GraphicsCommandList* commandList) override;
    void Begin(ID3D12GraphicsCommandList* commandList) override;
    void Draw(ID3D12GraphicsCommandList* commandList) override;
    void AddRenderPassDatas(std::string_view sceneName) override;

private:
    void InitShaderAnsPSO();

private:
    std::unique_ptr<class ShaderBuilder> _shader;
    class SSGITechnique*                 _ssgiTech;
};
