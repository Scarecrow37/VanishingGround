#pragma once
#include "RenderPass.h"

class DXRDrawPass : public RenderPass
{
private:
    // shader table 최적화
    struct ShaderTableCache
    {
        bool   NeedUpdate        = true;
        UINT64 TlasSRV           = 0;
        UINT64 EnvMapSRV         = 0;
        UINT64 IrradianceMapSRV  = 0;
        UINT64 PreFilteredMapSRV = 0;
        UINT64 BRDFLUTSRV        = 0;

        UINT64 VertexBufferSRV  = 0;
        UINT64 IndexBufferSRV   = 0;
        UINT64 TextureHeapStart = 0;

        // shader identifier
        std::array<uint8_t, 32> RayGenID{};
        std::array<uint8_t, 32> MissID{};
        std::array<uint8_t, 32> HitGroupID{};
        std::array<uint8_t, 32> ShadowMissID{};
    };

public:
    DXRDrawPass() = default;
    virtual ~DXRDrawPass();

public:
    void Initialize(RenderScene* ownerScene, RenderTechnique* ownerTechnique, ID3D12GraphicsCommandList* commandList) override;
    void Begin(ID3D12GraphicsCommandList* commandList) override;
    void Draw(ID3D12GraphicsCommandList* commandList) override;
    void End(ID3D12GraphicsCommandList* commandList) override;

private:
    void CreateStateObject();
    void CreateShaderTable();
    void CreateShaderResource();
    void UpdateFrameResource(ID3D12GraphicsCommandList* commandList);

    void WriteCommand(ID3D12GraphicsCommandList* cmdList);

private:
    bool                               _init = false;
    ComPtr<ID3D12StateObject> _pso;
    ComPtr<ID3D12RootSignature> _globalRootsignature;
    ComPtr<ID3D12Resource>      _shaderTable;
    uint32_t                    _shaderTableEntrySize = 0;
    SharedResource<UnorderedAccessView>_outputResourceUAV;

    std::vector<VertexBufferID> _vertexBufferIDs;
    std::vector<IndexBufferID>  _indexBufferIDs;
    std::vector<MeshInstanceID>       _meshInstanceIDs;
    ComPtr<ID3D12GraphicsCommandList> _commandList;
    ComPtr<ID3D12CommandAllocator>    _commandAllocator;

    std::vector<InstanceData>         _instanceDatas;
    std::unique_ptr<StructuredBuffer> _instanceDatasBuffer;

    // 최적화
    ShaderTableCache _cache;
};
