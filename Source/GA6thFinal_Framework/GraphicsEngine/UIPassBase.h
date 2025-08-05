#pragma once
#include "RenderPass.h"

class UIPassBase : public RenderPass
{
public:
    UIPassBase(const std::vector<UINT>& instanceIDs);
    virtual ~UIPassBase();

public:
    void Initialize(RenderScene* ownerScene, RenderTechnique* ownerTechnique, ID3D12GraphicsCommandList* commandList) override;
    void Begin(ID3D12GraphicsCommandList* commandList) override;
    void Draw(ID3D12GraphicsCommandList* commandList) override;
    void End(ID3D12GraphicsCommandList* commandList) override;

protected:
    void UpdateBuffer(ID3D12GraphicsCommandList* commandList);

protected:
    CameraData                          _cameraData;
    std::shared_ptr<Model>              _quadModel;
    std::unique_ptr<ConstantBufferView> _cameraBuffer;
    const std::vector<UINT>&            _instanceIDs;
    std::unique_ptr<StructuredBuffer>   _instanceIDBuffer;
    BaseMesh*                           _halfQuad;
};