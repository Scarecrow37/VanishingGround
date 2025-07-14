#pragma once
struct AccelerationStructureBuffers;
class MeshRenderer;
enum class AsBuildClass
{
    STATICBLAS,SKELETALBLAS
};
struct MeshInstanceDesc
{
    const class MeshRenderer* Renderer;
    UINT                      InstanceID;
    UINT                      HitGroupIndex=0;
    D3D12_RAYTRACING_INSTANCE_FLAGS Flags;
    AsBuildClass                    BuildClass;
};

class AccelerationStructureManager
{
public:
    void Initialize(UINT maxInstance);

    // 프레임 단위로 호출
    void BeginFrame();
    void SubmitInstance(const MeshRenderer* renderer);
    void EndFrame();

    // blas 필요없는 static mesh 제거용
    void RemoveUnUsedStaticMeshes(const std::vector<MeshRenderer*>& liveStatics);
    
    // getter
    const AccelerationStructureBuffers& GetTopLevel() const { return *_topLevelBuffers; }
    const DescriptorHandles&            GetTopLevelSRV() const { return _topLevelBuffersSRV; }

private:
    // 내부 BLAS 캐시
    struct BlasCache
    {
        UINT                                          refCount = 0;
        std::shared_ptr<AccelerationStructureBuffers> buf;
    };

    // key = BaseMesh* (모델 공유)
    std::unordered_map<const class Model*, BlasCache>       _staticBlasMap;
    std::vector<std::shared_ptr<AccelerationStructureBuffers>> _dynamicBlas; // 매-프레임 재빌드

    // TLAS
    std::shared_ptr<AccelerationStructureBuffers> _topLevelBuffers;
    DescriptorHandles                             _topLevelBuffersSRV;
    std::vector<MeshInstanceDesc>                 _pendingInstances;

    // 임시 GPU 인스턴스-desc 업로드 버퍼
    ComPtr<ID3D12Resource> _instanceUpload;

    UINT _nextInstanceID   = 0;
    UINT _maxInstanceCount = 0;

    // ---- 내부 helper ----
    void BuildOrUpdateStaticBLAS(ID3D12Device5* device, ID3D12GraphicsCommandList4* cmdList,
                                 const MeshRenderer* renderer,
                                 BlasCache& cache);
    void BuildDynamicBLAS(ID3D12Device5* device, ID3D12GraphicsCommandList4* cmdList, const MeshRenderer* renderer,
                          std::shared_ptr<AccelerationStructureBuffers>& outBuf);
    void BuildOrUpdateTLAS(ID3D12Device5* device, ID3D12GraphicsCommandList4* cmdList);
};