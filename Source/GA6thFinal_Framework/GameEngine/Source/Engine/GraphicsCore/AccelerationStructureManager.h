#pragma once
struct AccelerationStructureBuffers;
class MeshRenderer;
//class AccelerationStructureManager
//{
//public:
//    AccelerationStructureManager()  = default;
//    ~AccelerationStructureManager() = default;
//    
//public:
//    void Initialize(ID3D12Device5* device);
//    void AddBottomLevelAS(ID3D12Device5* pDevice, ID3D12GraphicsCommandList4* cmdList, const MeshRenderer* mesh);
//    void BuildTopLevelAS(ID3D12Device5* pDevice, ID3D12GraphicsCommandList4* cmdList,std::vector<Matrix> worldTransform);
//
//    std::vector<std::shared_ptr<AccelerationStructureBuffers>> _blasList;
//    std::shared_ptr<AccelerationStructureBuffers>              _topLevelBuffers;
//    UINT                                                       _maxInstanceCount = 10000;
//};
#pragma once
struct AccelerationStructureBuffers;
class MeshRenderer;
enum class AsBuildClass
{
    StaticBLAS,SkleltalBLAS
};
struct MeshInstanceDesc
{
    const class MeshRenderer* Renderer;
    UINT                      InstanceID;
    UINT                      HitGroupIndex;
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
    void RemoveUnUsedStaticMeshes(const std::vector<const MeshRenderer*>& liveStatics);
    
    // getter
    const AccelerationStructureBuffers& GetTopLevel() const { return *_topLevelBuffers; }

private:
    // 내부 BLAS 캐시
    struct BlasCache
    {
        std::shared_ptr<AccelerationStructureBuffers> buf;
        UINT                                          refCount = 0; // static mesh reference
    };

    // key = BaseMesh* (모델 공유)
    std::unordered_map<const class BaseMesh*, BlasCache>       _staticBlasMap;
    std::vector<std::shared_ptr<AccelerationStructureBuffers>> _dynamicBlas; // 매-프레임 재빌드

    // TLAS
    std::shared_ptr<AccelerationStructureBuffers> _topLevelBuffers;
    std::vector<MeshInstanceDesc>                 _pendingInstances;

    // 임시 GPU 인스턴스-desc 업로드 버퍼
    Microsoft::WRL::ComPtr<ID3D12Resource> _instanceUpload;

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