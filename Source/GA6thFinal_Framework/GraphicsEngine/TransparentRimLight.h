#pragma once
#include "CustomMaterial.h"

class TransparentRimLight : public CustomMaterial
{
public:
    TransparentRimLight();
    virtual ~TransparentRimLight();

public:
    UINT GetRootParameterIndex(MeshType meshType, std::string_view name) const override;

public:
    void Initialize() override;
    void SetMaterial(ID3D12GraphicsCommandList* commandList, MeshRenderer* renderer) override;

private:
    ComPtr<ID3D12PipelineState>                            _pipelineState[2];
    FX<GE::VS::STATIC_FORWARD_FR, GE::PS::TRANSPARENT_LIM_RIGHT>   _fxStatic;
    FX<GE::VS::SKELETAL_FORWARD_FR, GE::PS::TRANSPARENT_LIM_RIGHT> _fxSkeletal;
};