#pragma once

class RenderPass;
class RenderTechnique;
class FrameResource;
class Camera;
class MeshRenderer;
class SkyBox;
class SpriteRenderer;
class TextRenderer;
class SDFTextRenderer;
class AccelerationStructureManager;
class UIRenderer;
class RenderScene
{
public:
    RenderScene(std::string_view name);
    ~RenderScene();

public:
    std::shared_ptr<Camera>      GetCamera() const { return _camera; }
    D3D12_GPU_DESCRIPTOR_HANDLE  GetFinalImage();
    SkyBox*                      GetSkyBox() { return _skyBox.get(); };
    SharedResource<RenderTarget> GetSharedRenderTarget() const;
    const bool                   IsDirtyFlag() const { return _isDirtyFlag; }

public:
    void SetCamera(std::shared_ptr<Camera> camera) { _camera = camera; }
    void SetEnvironmentSkyBox(std::wstring_view path);
    void SetIBLSkyBox(std::wstring_view path);

public:
    void InitializeRenderScene();
    void RegisterOnRenderQueue(MeshRenderer* component);
    void RegisterOnRenderQueue(SpriteRenderer* component);
    void RegisterOnRenderQueue(TextRenderer* component);
    void RegisterOnRenderQueue(SDFTextRenderer* component);
    void AddRenderTechnique(std::unique_ptr<RenderTechnique> technique);
    void AddRenderPassDatas();

public:
    void UpdateRenderScene(const float deltaTime);
    void Execute();

public:
    void ResetEnvironmentSkyBox();
    void ResetIBLSkyBox();
    void ClearRenderQueue();
    void UpdateRenderQueue();

public:
    template <typename T>
    T* GetRenderTechnique()
    {
        for (auto& pass : _techniques)
        {
            T* pointer = dynamic_cast<T*>(pass.get());
            if (pointer)
            {
                return pointer;
            }
        }
        return nullptr;
    }

private:
    void UpdateGlobal();
    void UpdateObject();
    void UpdateUI();

 private:
    void CreateRenderTarget();
    void CreateDepthStencil();
    void CreateFrameResource();
    void CreateCamera();

public:
    std::unique_ptr<AccelerationStructureManager> _accelerationStructureManager;

    std::string _name;
    std::string _meshRenderTargetName;
    std::string _finalTargetName;

    std::vector<std::unique_ptr<RenderTechnique>> _techniques;
    std::vector<MeshRenderer*>                    _meshRenderQueue;
    std::vector<UIRenderer*>                      _uiRenderQueue;
    std::vector<UIRenderer*>                      _activeUIs;
    std::vector<MeshInfo>                         _activeMeshes[MESH_TYPE_END];

    CommandSet _commandSet;

    // Frame Resource
    std::vector<std::unique_ptr<FrameResource>> _frameResources;
    std::vector<LightData>                      _lightDatas;
    std::vector<MatrixData>                     _matrices;
    std::vector<BoneMatrices>                   _boneMatrices;
    std::vector<Matrix>                         _uiMatrices;
    std::vector<UIMaterial>                     _uiMaterials;
    std::vector<MeshInstanceID>                 _staticMeshInstanceIDs;
    std::vector<MeshInstanceID>                 _skeletalMeshInstanceIDs;
    std::shared_ptr<Camera>                     _camera;
    NumLight                                    _numLight;

    std::unique_ptr<Quad>               _frameQuad;
    std::unique_ptr<SkyBox>             _skyBox;
    SharedResource<UnorderedAccessView> _accumulationBuffer;
    SharedResource<DepthStencilView>    _depthStencilView;

    // Buffers
    std::unique_ptr<ConstantBufferView> _cameraBuffer;
    std::unique_ptr<ConstantBufferView> _RaycameraBuffer;
    std::unique_ptr<ConstantBufferView> _lightBuffer;
    ComPtr<ID3D12PipelineState>         _framePSO;

    UINT _currentFrameIndex = 0;

private:
    std::vector<SharedResource<RenderTarget>> _sharedRenderTarget;
    bool                                      _isDirtyFlag;
    size_t                                    _prevSize;
};