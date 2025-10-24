#pragma once
#include "RenderTechnique.h"

class UITechnique : public RenderTechnique
{
    enum UIType
    {
        MODE_3D,
        MODE_25D,
        MODE_2D,
        MODE_TEXT,
        TYPE_END
    };
    struct OITNode
    {
        float        color[4];
        float        depth;
        unsigned int next;
    };

public:
    UITechnique();
    virtual ~UITechnique();

public:
    DepthStencilView*    GetDepthStencilView() const { return _depthStencilView.Get(); }
    ConstantBufferView*  GetUIMaterialDataBuffer() const { return _uiMaterialDataBuffer.get(); }
    UnorderedAccessView* GetHeadBuffer() const { return _headBuffer.Get(); }
    UnorderedAccessView* GetNodesBuffer() const { return _nodesBuffer.Get(); }
    UnorderedAccessView* GetAtomicCounterBuffer() const { return _atomicCounterBuffer.Get(); }
    ConstantBufferView*  GetCameraBuffer(int index) const { return _cameraBuffers[index].get(); }

public:
    void Initialize(ID3D12GraphicsCommandList* commandList) override;
    void Execute(ID3D12GraphicsCommandList* commandList) override;

private:
    void CreateCameraBuffer();

private:
    std::vector<UINT>                   _instanceIDs[TYPE_END];    
    std::vector<UIMaterialData>         _uiMaterialDatas;
    std::unique_ptr<ConstantBufferView> _uiMaterialDataBuffer;
    SharedResource<DepthStencilView>    _depthStencilView;

    // OIT Buffers
    SharedResource<UnorderedAccessView> _headBuffer;
    SharedResource<UnorderedAccessView> _nodesBuffer;
    SharedResource<UnorderedAccessView> _atomicCounterBuffer;    

    CameraData                          _cameraData[3];
    std::shared_ptr<ConstantBufferView> _cameraBuffers[3];
};