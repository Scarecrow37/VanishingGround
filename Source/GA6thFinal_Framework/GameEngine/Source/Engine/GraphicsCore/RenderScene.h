#pragma once

class RenderPass;
class RenderTechnique;
class FrameResource;
class Camera;
class MeshRenderer;
class SkyBox;
class SpriteRenderer;
class FontRenderer;
class RenderScene
{
public:
    RenderScene(std::string_view name);
    ~RenderScene();

public:
    std::shared_ptr<Camera>     GetCamera() const { return _camera; }
    D3D12_GPU_DESCRIPTOR_HANDLE GetFinalImage();
    SkyBox*                     GetSkyBox() { return _skyBox.get(); };

public:
    void SetCamera(std::shared_ptr<Camera> camera) { _camera = camera; }
    void SetSkyBox(std::string_view path);

public:
    void InitializeRenderScene();
    void RegisterOnRenderQueue(MeshRenderer* component);
    void RegisterOnRenderQueue(SpriteRenderer* component);
    void RegisterOnRenderQueue(FontRenderer* component);
    void AddRenderTechnique(std::unique_ptr<RenderTechnique> technique);

public:
    void UpdateRenderScene();
    void ClassifyMesh();
    void RenderOnBackBuffer(ID3D12GraphicsCommandList* commandList);
    void RenderOnEditor(ID3D12GraphicsCommandList* commandList);
    void Execute(ID3D12GraphicsCommandList* commandList);

public:
    void ResetSkyBox();

private:
    void UpdateGlobal();
    void UpdateObject();
    void UpdateUI();
    void UpdateFont();

 private:
    void CreateRenderTarget();
    void CreateDepthStencil();
    void CreateFrameResource();
    void CreateCamera();

public:
    std::string _name;
    std::string _meshRenderTargetName;
    std::string _finalTargetName;

    std::vector<std::unique_ptr<RenderTechnique>>                  _techniques;
    std::vector<std::pair<std::unique_ptr<bool>, MeshRenderer*>>   _meshRenderQueue;
    std::vector<std::pair<std::unique_ptr<bool>, SpriteRenderer*>> _uiRenderQueue;
    std::vector<std::pair<std::unique_ptr<bool>, FontRenderer*>>   _fontRenderQueue;

    
    // mesh 분리
    std::vector<MeshRenderer*> _staticMesh;
    std::vector<MeshRenderer*> _skeletalMesh;

    // Frame Resource
    std::vector<std::unique_ptr<FrameResource>> _frameResources;
    std::vector<LightData>                      _lightDatas;
    std::vector<XMMATRIX>                       _worldMatrices;
    std::vector<BoneMatrices>                   _boneMatrices;
    std::vector<MaterialID>                     _materialIDs;
    std::vector<XMMATRIX>                       _uiMatrices;
    std::vector<UIMaterial>                     _uiMaterials;
    std::shared_ptr<Camera>                     _camera;
    NumLight                                    _numLight;

    std::unique_ptr<Quad>               _frameQuad;
    std::unique_ptr<SkyBox>             _skyBox;
    SharedResource<UnorderedAccessView> _accumulationBuffer;
    SharedResource<DepthStencilView>    _depthStencilView;

    // Buffers
    std::unique_ptr<ConstantBufferView> _cameraBuffer;
    std::unique_ptr<ConstantBufferView> _lightBuffer;
    ComPtr<ID3D12PipelineState>         _framePSO;

    UINT _currentFrameIndex = 0;
};