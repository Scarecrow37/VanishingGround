#pragma once
#include "RenderTechnique.h"

class UITechnique_OIT : public RenderTechnique
{
    struct OITNode
    {
        float        color[4];
        float        depth;
        unsigned int next;
    };

public:
    UITechnique_OIT();
    virtual ~UITechnique_OIT();

public:
    DepthStencilView*    GetDepthStencilView() const { return _depthStencilView.Get(); }
    ConstantBufferView*  GetUIMaterialDataBuffer() const { return _uiMaterialDataBuffer.get(); }
    UnorderedAccessView* GetHeadBuffer() const { return _headBuffer.Get(); }
    UnorderedAccessView* GetNodesBuffer() const { return _nodesBuffer.Get(); }
    UnorderedAccessView* GetAtomicCounterBuffer() const { return _atomicCounterBuffer.Get(); }

public:
    void Initialize(ID3D12GraphicsCommandList* commandList) override;
    void Execute(ID3D12GraphicsCommandList* commandList) override;

private:
    std::vector<UINT>                   _renderDatas[SPRITE_TYPE_END];
    std::vector<UIMaterialData>         _uiMaterialDatas;
    SharedResource<DepthStencilView>    _depthStencilView;
    SharedResource<UnorderedAccessView> _headBuffer;
    SharedResource<UnorderedAccessView> _nodesBuffer;
    SharedResource<UnorderedAccessView> _atomicCounterBuffer;
    
    std::unique_ptr<ConstantBufferView> _uiMaterialDataBuffer;
};