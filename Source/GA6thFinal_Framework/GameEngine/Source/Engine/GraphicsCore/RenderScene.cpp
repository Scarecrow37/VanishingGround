#include "pch.h"
#include "RenderScene.h"
#include "Animator.h"
#include "DepthStencilView.h"
#include "FrameResource.h"
#include "Light.h"
#include "MeshRenderer.h"
#include "Model.h"
#include "RenderTechnique.h"
#include "SkyBox.h"
#include "SpriteRenderer.h"
#include "FontRenderer.h"
#include "AccelerationStructureManager.h"

RenderScene::RenderScene(std::string_view name) : _skyBox{std::make_unique<SkyBox>()}, _name(name)
{
    _lightDatas.resize(MAX_LIGHT);
}

RenderScene::~RenderScene() {}

D3D12_GPU_DESCRIPTOR_HANDLE RenderScene::GetFinalImage()
{
    RenderTarget* finalTarget = UmMultiRenderTargetManager.GetRenderTarget(_finalTargetName);
    return finalTarget->GetSRVHandle();
}

void RenderScene::SetSkyBox(std::wstring_view path)
{
    _skyBox->SetTexture(path.data());
}

void RenderScene::InitializeRenderScene()
{
    CreateCamera();
    CreateRenderTarget();
    CreateDepthStencil();
    CreateFrameResource();

    _frameQuad = std::make_unique<Quad>();
    _frameQuad->Initialize(-1.f, 1.f, 2.f, 2.f, 0.f);

    DXGI_MODE_DESC mode = UmDevice.GetMode();
    mode.Format         = DXGI_FORMAT_R32G32B32A32_FLOAT;

    _accumulationBuffer = MakeSharedResource<UnorderedAccessView>();
    _accumulationBuffer->Initialize(mode);
    _accumulationBuffer->TransitionResource(UmDevice.GetCommandList(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

    UmDXResourceManager.AddResource(_accumulationBuffer);


    if (UmRenderer._isRaytracing)
    { 
        _accelerationStructureManager = std::make_unique<AccelerationStructureManager>();
        _accelerationStructureManager->Initialize(10000);
    }
}

void RenderScene::RegisterOnRenderQueue(MeshRenderer* component)
{
    if (nullptr == component)
        return;

    auto iter = std::find_if(_meshRenderQueue.begin(), _meshRenderQueue.end(), [](const auto& pair) { return !pair.first.get(); });

    if (iter != _meshRenderQueue.end())
    {
        GRAPHICS_ASSERT(false, L"RenderScene::RegisterRenderQueue : Already registered component.");
        return;
    }

    _meshRenderQueue.emplace_back(std::make_unique<bool>(false), component);
    component->_isDestroyeds.push_back(_meshRenderQueue.back().first.get());
}

void RenderScene::RegisterOnRenderQueue(SpriteRenderer* component)
{
    if (nullptr == component)
        return;

    auto iter = std::find_if(_uiRenderQueue.begin(), _uiRenderQueue.end(), [](const auto& pair) { return !pair.first.get(); });

    if (iter != _uiRenderQueue.end())
    {
        GRAPHICS_ASSERT(false, L"RenderScene::RegisterRenderQueue : Already registered component.");
        return;
    }

    _uiRenderQueue.emplace_back(std::make_unique<bool>(false), component);
    component->_isDestroyeds.push_back(_uiRenderQueue.back().first.get());
}

void RenderScene::RegisterOnRenderQueue(FontRenderer* component)
{
    if (nullptr == component)
        return;

    auto iter = std::find_if(_fontRenderQueue.begin(), _fontRenderQueue.end(), [](const auto& pair) { return !pair.first.get(); });

    if (iter != _fontRenderQueue.end())
    {
        GRAPHICS_ASSERT(false, L"RenderScene::RegisterRenderQueue : Already registered component.");
        return;
    }

    _fontRenderQueue.emplace_back(std::make_unique<bool>(false), component);
    component->_isDestroyeds.push_back(_fontRenderQueue.back().first.get());
}

void RenderScene::AddRenderTechnique(std::unique_ptr<RenderTechnique> technique)
{
    ID3D12GraphicsCommandList* commandList = UmDevice.GetCommandList();
    technique->SetOwnerScene(this);
    technique->Initialize(commandList);
    _techniques.push_back(std::move(technique));
}

void RenderScene::UpdateRenderScene()
{
    UpdateGlobal();
    UpdateObject();
    UpdateUI();
    UpdateFont();

    ID3D12GraphicsCommandList* commandList = UmDevice.GetCommandList();

    _frameResources[_currentFrameIndex]->CopyStructuredBuffer(commandList, FrameResourceType::TRANSFORM, _worldMatrices.data(), (UINT)_worldMatrices.size());
    _frameResources[_currentFrameIndex]->CopyStructuredBuffer(commandList, FrameResourceType::BONE_MATRICES, _boneMatrices.data(), (UINT)_boneMatrices.size());
    _frameResources[_currentFrameIndex]->CopyStructuredBuffer(commandList, FrameResourceType::MATERIAL, _materialIDs.data(), (UINT)_materialIDs.size());
    _frameResources[_currentFrameIndex]->CopyStructuredBuffer(commandList, FrameResourceType::UI_TRANSFORM, _uiMatrices.data(), (UINT)_uiMatrices.size());
    _frameResources[_currentFrameIndex]->CopyStructuredBuffer(commandList, FrameResourceType::UI_MATERIAL, _uiMaterials.data(), (UINT)_uiMaterials.size());
    _frameResources[_currentFrameIndex]->CopyStructuredBuffer(commandList, FrameResourceType::STATIC_MESH_INSTANCE_ID,
                                                              _staticMeshInstanceIDs.data(),
                                                              (UINT)_staticMeshInstanceIDs.size());
    _frameResources[_currentFrameIndex]->CopyStructuredBuffer(commandList, FrameResourceType::SKELETAL_MESH_INSTANCE_ID,
                                                              _skeletalMeshInstanceIDs.data(),
                                                              (UINT)_skeletalMeshInstanceIDs.size());
}

void RenderScene::Execute(ID3D12GraphicsCommandList* commandList)
{
    auto& graphics       = Global::engineCore->Graphics;
    auto  descriptorHeap = graphics.ViewManager.GetShaderResourceHeap();
    commandList->SetDescriptorHeaps(1, &descriptorHeap);

    _accumulationBuffer->TransitionResource(commandList, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
    _accumulationBuffer->ClearUnorderedAccessView(commandList);

    auto meshRenderTarget = graphics.MultiRenderTargetManager.GetRenderTarget(_meshRenderTargetName);
    meshRenderTarget->TransitionResource(commandList, D3D12_RESOURCE_STATE_RENDER_TARGET);
    meshRenderTarget->ClearRenderTarget(commandList);

    const auto& gBuffers = graphics.MultiRenderTargetManager.GetRenderTargetGroup("GBuffer");
    for (auto& buffer : gBuffers)
    {
        buffer->TransitionResource(commandList, D3D12_RESOURCE_STATE_RENDER_TARGET);
        buffer->ClearRenderTarget(commandList);
    }

    _depthStencilView->TransitionResource(commandList, D3D12_RESOURCE_STATE_DEPTH_WRITE);
    _depthStencilView->ClearDepthStencilView(commandList);

    for (auto& tech : _techniques)
    {
        tech->Execute(commandList);
    }

    _depthStencilView->TransitionResource(commandList, D3D12_RESOURCE_STATE_PRESENT);

}

void RenderScene::ResetSkyBox()
{
    _skyBox->ResetResource();
}

void RenderScene::UpdateGlobal()
{
    _currentFrameIndex = UmDevice.GetCurrentBackBufferIndex();
    _camera->Update();

    CameraData cameraData{.View              = XMMatrixTranspose(_camera->GetViewMatrix()),
                          .Projection        = XMMatrixTranspose(_camera->GetProjectionMatrix()),
                          .ViewInverse       = XMMatrixTranspose(_camera->GetWorldMatrix()),
                          .ProejctionInverse = XMMatrixTranspose(_camera->GetProjectionInverseMatrix()),
                          .Position          = Vector4(_camera->GetPosition())};

    CameraData RaycameraData{.View           = _camera->GetViewMatrix(),
                          .Projection        = _camera->GetProjectionMatrix(),
                          .ViewInverse       = _camera->GetWorldMatrix(),
                          .ProejctionInverse = _camera->GetProjectionInverseMatrix(),
                          .Position          = Vector4(_camera->GetPosition())};
       

    auto& lights = UmLightCore.GetLights(_name.c_str());

    _numLight = {};
    for (auto& [isDestroy, light] : lights)
    {
        if (nullptr == light->_isActive || !(*light->_isActive))
            continue;

        switch (light->_type)
        {
        case Light::Type::DIRECTIONAL:
            _lightDatas[_numLight.Directional++] = light->_data;
            break;
        case Light::Type::POINT:
            _lightDatas[MAX_DIRECTIONAL_LIGHT + _numLight.Point++] = light->_data;
            break;
        case Light::Type::SPOT:
            _lightDatas[MAX_DIRECTIONAL_LIGHT + MAX_POINT_LIGHT + _numLight.Spot++] = light->_data;
            break;
        }
    }

    _cameraBuffer->UpdateBuffer(&cameraData);
    _RaycameraBuffer->UpdateBuffer(&RaycameraData);
    _lightBuffer->UpdateBuffer(_lightDatas.data());
}

void RenderScene::UpdateObject()
{
    auto first = std::remove_if(_meshRenderQueue.begin(), _meshRenderQueue.end(), [](const auto& pair) { return *pair.first; });
    _meshRenderQueue.erase(first, _meshRenderQueue.end());    

    _worldMatrices.clear();
    _boneMatrices.clear();
    _materialIDs.clear();
    _staticMeshInstanceIDs.clear();
    _skeletalMeshInstanceIDs.clear();
    UINT instanceID = 0;
    for (auto& [isDestroy, component] : _meshRenderQueue)
    {
        if (!component->IsActive())
            continue;

        const auto& model = component->GetModel();
        if (!model)
            continue;

        const auto  type      = component->GetType();
        const auto& meshes    = model->GetMeshes();
        const auto& materials = model->GetMaterials();
        const auto& textures  = model->GetTextures();

        XMMATRIX     world = XMMatrixTranspose(component->GetWorldMatrix());
        BoneMatrices boneMatrices{};

        if (MeshRenderType::SKELETAL == type)
        {
            auto animator = component->GetAnimator();
            if (animator) memcpy(&boneMatrices, animator->GetAnimationTransform(), sizeof(BoneMatrices));
        }

        UINT size = (UINT)meshes.size();
        for (UINT i = 0; i < size; i++)
        {
            _worldMatrices.push_back(world);
            _boneMatrices.push_back(boneMatrices);

            MaterialID materialID{};
            for (UINT j = 0; j < 4; j++)
            {
                materialID.ID[j] = textures[i][j]->GetID();
            }
            _materialIDs.push_back(materialID);
            if (MeshRenderType::STATIC == type)
            {
                _staticMeshInstanceIDs.push_back(instanceID);
            }
            else if (MeshRenderType::SKELETAL == type)
            {
                _skeletalMeshInstanceIDs.push_back(instanceID);
            }
            instanceID++;
        }
    }
    ClassifyMesh();
}

void RenderScene::ClassifyMesh()
{
    _staticMesh.clear();
    _skeletalMesh.clear();
    for (auto& [isDestroy,component] : _meshRenderQueue)
    {
        if (!component->IsActive())
            continue;
        const auto& model = component->GetModel();
        if (!model)
            continue;

        switch (component->GetType())
        {
        case MeshRenderType::STATIC:
            _staticMesh.push_back(component);
            break;
        case MeshRenderType::SKELETAL:
            _skeletalMesh.push_back(component);
            break;
        default:
            break;
        }
    }
}

void RenderScene::UpdateUI()
{    
    auto first = std::remove_if(_uiRenderQueue.begin(), _uiRenderQueue.end(), [](const auto& pair) { return *pair.first; });
    _uiRenderQueue.erase(first, _uiRenderQueue.end());

    _uiMatrices.clear();
    _uiMaterials.clear();
    for (auto& [isDestroy, component] : _uiRenderQueue)
    {
        if (!component->IsActive())
            continue;

        auto texture = component->GetTexture();
        if (nullptr == texture)
            continue;

        auto     size = component->GetSize();
        XMMATRIX world = component->GetWorldMatrix();
        XMMATRIX scale = XMMatrixIdentity();
        
        switch (component->GetType())
        {
        case SpriteType::MODE_2D:
            scale = XMMatrixScaling((float)size.cx, (float)size.cy, 1.f);
            break;        
        case SpriteType::MODE_3D:
        {
            XMVECTOR s, r, t;
            XMMatrixDecompose(&s, &r, &t, world);

            XMVECTOR combine = XMQuaternionMultiply(r, _camera->GetRotation());
            world = XMMatrixScalingFromVector(s) * XMMatrixRotationQuaternion(combine) * XMMatrixTranslationFromVector(t);
            [[fallthrough]];
        }
        case SpriteType::MODE_25D:
        {
            float ratio = (float)size.cx / (float)size.cy;
            scale       = XMMatrixScaling(ratio, 1.f, 1.f);
            break;
        }
        }
        
        world = XMMatrixTranspose(scale * world);
        _uiMatrices.push_back(world);

        UIMaterial material{.ID = texture->GetID(), .Alpha = 1.f};
        _uiMaterials.push_back(material);
    }
}

void RenderScene::UpdateFont()
{
    auto first = std::remove_if(_fontRenderQueue.begin(), _fontRenderQueue.end(), [](const auto& pair) { return *pair.first; });
    _fontRenderQueue.erase(first, _fontRenderQueue.end());
}

void RenderScene::CreateRenderTarget()
{
    auto                          mode                     = UmDevice.GetMode();
    auto                          commandList              = UmDevice.GetCommandList();
    auto&                         multiRenderTargetManager = UmMultiRenderTargetManager;
    SharedResource<RenderTarget>  renderTarget;
    mode.Format           = DXGI_FORMAT_R32G32B32A32_FLOAT;

    _meshRenderTargetName = _name + "_MeshRenderTarget";
    renderTarget          = MakeSharedResource<RenderTarget>();
    renderTarget->Initialize(mode, 0.247f);
    renderTarget->TransitionResource(commandList, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

    multiRenderTargetManager.AddRenderTarget(_meshRenderTargetName, renderTarget);

    _finalTargetName = _name + "_FinalTarget";
    renderTarget     = MakeSharedResource<RenderTarget>();
    renderTarget->Initialize(mode, 0.247f);
    renderTarget->TransitionResource(commandList, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

    multiRenderTargetManager.AddRenderTarget(_finalTargetName, renderTarget);
}

void RenderScene::CreateDepthStencil()
{
    _depthStencilView = MakeSharedResource<DepthStencilView>();

    auto mode   = UmDevice.GetMode();
    mode.Format = DXGI_FORMAT_R24G8_TYPELESS;
    _depthStencilView->Initialize(mode);

    UmDXResourceManager.AddResource(_depthStencilView);
}

void RenderScene::CreateFrameResource()
{
    _frameResources.resize(SWAPCHAIN_BUFFER_COUNT);

    constexpr UINT MAX_OBJECTS = 1000;
    for (UINT i = 0; i < SWAPCHAIN_BUFFER_COUNT; ++i)
    {
        _frameResources[i] = std::make_unique<FrameResource>();

        // Object Transform
        _frameResources[i]->AddFrameResource(sizeof(XMMATRIX), MAX_OBJECTS);

        // Object BoneTransform
        _frameResources[i]->AddFrameResource(sizeof(XMMATRIX) * MAX_BONE_MATRIX, MAX_OBJECTS);

        // Material
        _frameResources[i]->AddFrameResource(sizeof(MaterialID), MAX_OBJECTS);

        // UI Transform
        _frameResources[i]->AddFrameResource(sizeof(XMMATRIX), MAX_OBJECTS);

        // UI Material
        _frameResources[i]->AddFrameResource(sizeof(XMMATRIX), MAX_OBJECTS);

        // Vertex Buffer ID
        _frameResources[i]->AddFrameResource(sizeof(VertexBufferID), MAX_OBJECTS);

        // Index Buffer ID
        _frameResources[i]->AddFrameResource(sizeof(IndexBufferID), MAX_OBJECTS);

        // Static Mesh Instance ID
        _frameResources[i]->AddFrameResource(sizeof(StaticMeshInstanceID), MAX_OBJECTS);
        
        // Skeletal Mesh Instance ID
        _frameResources[i]->AddFrameResource(sizeof(SkeletalMeshInstanceID), MAX_OBJECTS);
    }

    _cameraBuffer = std::make_unique<ConstantBufferView>();
    UINT alignedSize =(sizeof(CameraData) + 255) & ~255;
    _cameraBuffer->Initialize(alignedSize);

    _RaycameraBuffer = std::make_unique<ConstantBufferView>();
    _RaycameraBuffer->Initialize(alignedSize);

    _lightBuffer = std::make_unique<ConstantBufferView>();
    _lightBuffer->Initialize(sizeof(LightData) * MAX_LIGHT);
}

void RenderScene::CreateCamera()
{
    _camera = std::make_shared<Camera>();

    Vector3 position = Vector3::Zero;
    Vector3 diretion = Vector3::Forward;
    Matrix  rotation = Matrix::Identity;

    _camera->SetRotation(rotation.ToEuler());
    _camera->SetPosition(position);
    _camera->Update();
}
