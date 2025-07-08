#pragma once
#include "RenderPass.h"

class UIPassBase : public RenderPass
{
public:
    UIPassBase(const std::vector<UINT>& instanceIDs);
    virtual ~UIPassBase();

public:
    void Initialize(RenderScene* ownerScene) override;

protected:
    void UpdateBuffer(ID3D12GraphicsCommandList* commandList);
    void SetResource(UINT rootParameterIndex, ID3D12GraphicsCommandList* commandList);

protected:
    const std::vector<UINT>&          _instanceIDs;
    std::unique_ptr<StructuredBuffer> _instanceIDBuffer;
    std::shared_ptr<Model>            _quadModel;
    BaseMesh*                         _halfQuad;
};