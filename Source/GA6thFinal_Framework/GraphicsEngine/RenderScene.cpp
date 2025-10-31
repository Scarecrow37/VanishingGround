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
#include "TextRenderer.h"
#include "SDFTextRenderer.h"
#include "AccelerationStructureManager.h"

RenderScene::RenderScene(std::string_view name)
    : _name(name)
    , _isDirtyFlag(true)
    , _prevSize(0)
{
    _lightDatas.resize(MAX_LIGHT);
}

RenderScene::~RenderScene() = default;

D3D12_GPU_DESCRIPTOR_HANDLE RenderScene::GetFinalImage()
{
    RenderTarget* finalTarget = Global::multiRenderTargetManager->GetRenderTarget(_finalTargetName);
    return finalTarget->GetSRVHandle();
}

SharedResource<RenderTarget> RenderScene::GetSharedRenderTarget() const
{
    return _sharedRenderTarget[_currentFrameIndex];
}

void RenderScene::SetEnvironmentSkyBox(std::wstring_view path)
{
    _skyBox->SetEnvironmentTexture(path);
}

void RenderScene::SetIBLSkyBox(std::wstring_view path)
{
    _skyBox->SetIBLTexture(path);
}

void RenderScene::InitializeRenderScene()
{
    Global::commandController->AddCommandSet(CommandType::DIRECT, std::filesystem::path(_name).c_str(), _commandSet);

    CreateCamera();
    CreateRenderTarget();
    CreateDepthStencil();
    CreateFrameResource();

    _skyBox = std::make_unique<SkyBox>();
    _skyBox->Initialize();

    _frameQuad = std::make_unique<Quad>();
    _frameQuad->Initialize(-1.f, 1.f, 2.f, 2.f, 0.f);

    DXGI_MODE_DESC mode = Global::device->GetMode();
    mode.Format         = DXGI_FORMAT_R32G32B32A32_FLOAT;

    _accumulationBuffer = MakeSharedResource<UnorderedAccessView>();

    auto desc = CD3DX12_RESOURCE_DESC::Tex2D(mode.Format, mode.Width, mode.Height, 1, 1, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);
    _accumulationBuffer->InitializeAsTexture(desc, UnorderedAccessView::UAVSliceType::PER_MIP, true);
    _accumulationBuffer->TransitionResource(_commandSet, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

    Global::dxResourceManager->AddResource(_accumulationBuffer);

    if (Global::isRayTracing)
    { 
        _accelerationStructureManager = std::make_unique<AccelerationStructureManager>();
        _accelerationStructureManager->Initialize(10000, this);
    }
}

void RenderScene::RegisterOnRenderQueue(MeshRenderer* component)
{
    if (nullptr == component)
        return;

    auto iter = std::find_if(_meshRenderQueue.begin(), _meshRenderQueue.end(), [component](const auto& renderer) { return component->GetID() == renderer->GetID(); });

    if (iter != _meshRenderQueue.end())
    {
        GRAPHICS_ASSERT(false, L"RenderScene::RegisterRenderQueue : Already registered component.");
        return;
    }

    _meshRenderQueue.emplace_back(component);
}

void RenderScene::RegisterOnRenderQueue(SpriteRenderer* component)
{
    if (nullptr == component)
        return;

    auto iter = std::find_if(_uiRenderQueue.begin(), _uiRenderQueue.end(), [component](const auto& renderer) { return component->GetID() == renderer->GetID(); });

    if (iter != _uiRenderQueue.end())
    {
        GRAPHICS_ASSERT(false, L"RenderScene::RegisterRenderQueue : Already registered component.");
        return;
    }

    _uiRenderQueue.emplace_back(component);
}

void RenderScene::RegisterOnRenderQueue(TextRenderer* component)
{
    /*if (nullptr == component)
        return;

    auto iter = std::find_if(_textRenderQueue.begin(), _textRenderQueue.end(), [component](const auto& renderer) { return component->GetID() == renderer->GetID(); });

    if (iter != _textRenderQueue.end())
    {
        GRAPHICS_ASSERT(false, L"RenderScene::RegisterRenderQueue : Already registered component.");
        return;
    }

    _textRenderQueue.emplace_back(component);*/
}

void RenderScene::RegisterOnRenderQueue(SDFTextRenderer* component)
{
    if (nullptr == component)
        return;

    auto iter = std::find_if(_uiRenderQueue.begin(), _uiRenderQueue.end(), [component](const auto& renderer) { return component->GetID() == renderer->GetID(); });

    if (iter != _uiRenderQueue.end())
    {
        GRAPHICS_ASSERT(false, L"RenderScene::RegisterRenderQueue : Already registered component.");
        return;
    }

    _uiRenderQueue.emplace_back(component);
}

void RenderScene::AddRenderTechnique(std::unique_ptr<RenderTechnique> technique)
{
    technique->SetOwnerScene(this);
    technique->Initialize(_commandSet);
    _techniques.push_back(std::move(technique));
}

void RenderScene::AddRenderPassDatas()
{
    for (auto& technique : _techniques)
    {
         technique->AddRenderPassDatas(_name);
    }
}

void RenderScene::UpdateRenderScene(const float deltaTime)
{
    UpdateGlobal();
    UpdateObject();
    UpdateUI();

    if (Global::isRayTracing)
    {
        _accelerationStructureManager->RemoveUnUsedStaticMeshes(_activeMeshes[STATIC_MESH], _activeMeshes[SKELETAL_MESH]);
    }

    _frameResources[_currentFrameIndex]->CopyStructuredBuffer(_commandSet, FrameResourceType::TRANSFORM, _matrices.data(), (UINT)_matrices.size());
    _frameResources[_currentFrameIndex]->CopyStructuredBuffer(_commandSet, FrameResourceType::BONE_MATRICES, _boneMatrices.data(), (UINT)_boneMatrices.size());
    _frameResources[_currentFrameIndex]->CopyStructuredBuffer(_commandSet, FrameResourceType::UI_TRANSFORM, _uiMatrices.data(), (UINT)_uiMatrices.size());
    _frameResources[_currentFrameIndex]->CopyStructuredBuffer(_commandSet, FrameResourceType::UI_MATERIAL, _uiMaterials.data(), (UINT)_uiMaterials.size());
    
    for (auto& technique : _techniques)
    {
        technique->Update(_commandSet, deltaTime);
    }
}

void RenderScene::Execute()
{
    auto  descriptorHeap = Global::viewManager->GetShaderResourceHeap();
    _commandSet->SetDescriptorHeaps(1, &descriptorHeap);

    _accumulationBuffer->TransitionResource(_commandSet, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
    _accumulationBuffer->ClearUnorderedAccessView(_commandSet, Vector4(0.f, 0.f, 0.f, 1.f));

    auto meshRenderTarget = Global::multiRenderTargetManager->GetRenderTarget(_meshRenderTargetName);
    meshRenderTarget->TransitionResource(_commandSet, D3D12_RESOURCE_STATE_RENDER_TARGET);
    meshRenderTarget->ClearRenderTarget(_commandSet);

    const auto& gBuffers = Global::multiRenderTargetManager->GetRenderTargetGroup("G-Buffer");
    for (auto& buffer : gBuffers)
    {
        buffer->TransitionResource(_commandSet, D3D12_RESOURCE_STATE_RENDER_TARGET);
        buffer->ClearRenderTarget(_commandSet);
    }

    _depthStencilView->TransitionResource(_commandSet, D3D12_RESOURCE_STATE_DEPTH_WRITE);
    _depthStencilView->ClearDepthStencilView(_commandSet);

    for (auto& tech : _techniques)
    {
        tech->Execute(_commandSet);
    }

    _depthStencilView->TransitionResource(_commandSet, D3D12_RESOURCE_STATE_PRESENT);

    _commandSet->Close();
    Global::commandController->ExecuteCommand(CommandQueueType::GRAPHICS_QUEUE, _commandSet);

    _isDirtyFlag = false;
}

void RenderScene::ResetEnvironmentSkyBox()
{
    _skyBox->ResetEnvironmentResource();
}

void RenderScene::ResetIBLSkyBox()
{
    _skyBox->ResetIBLResource();
}

void RenderScene::ClearRenderQueue()
{
    _meshRenderQueue.clear();
    _uiRenderQueue.clear();
    /*_textRenderQueue.clear();
    _sdfTextRenderQueue.clear();*/
}

void RenderScene::UpdateRenderQueue()
{
    auto iter_mesh = std::remove_if(_meshRenderQueue.begin(), _meshRenderQueue.end(), [](const auto& renderer) { return !renderer->IsAlive(); });
    _meshRenderQueue.erase(iter_mesh, _meshRenderQueue.end());

    auto iter_ui = std::remove_if(_uiRenderQueue.begin(), _uiRenderQueue.end(), [](const auto& renderer) { return !renderer->IsAlive(); });
    _uiRenderQueue.erase(iter_ui, _uiRenderQueue.end());
}

void RenderScene::UpdateGlobal()
{
    _currentFrameIndex = Global::device->GetCurrentBackBufferIndex();
    _camera->Update();

    CameraData cameraData{.View              = XMMatrixTranspose(_camera->GetViewMatrix()),
                          .Projection        = XMMatrixTranspose(_camera->GetProjectionMatrix()),
                          .ViewInverse       = XMMatrixTranspose(_camera->GetWorldMatrix()),
                          .ProjectionInverse = XMMatrixTranspose(_camera->GetProjectionInverseMatrix()),
                          .Position          = Vector4(_camera->GetPosition())};

    CameraData RaycameraData{.View           = _camera->GetViewMatrix(),
                          .Projection        = _camera->GetProjectionMatrix(),
                          .ViewInverse       = _camera->GetWorldMatrix(),
                          .ProjectionInverse = _camera->GetProjectionInverseMatrix(),
                          .Position          = Vector4(_camera->GetPosition())};

    auto& lights = Global::lightCore->GetLights(_name.c_str());

    _numLight = {};
    for (auto& light : lights)
    {
        if (!light->IsActive())
            continue;

        switch (light->GetType())
        {
        case Light::Type::DIRECTIONAL:
            if (_numLight.Directional >= MAX_DIRECTIONAL_LIGHT)
                continue;
            _lightDatas[_numLight.Directional++] = light->GetLightData();
            break;
        case Light::Type::POINT:
            if (_numLight.Point >= MAX_POINT_LIGHT)
                continue;
            _lightDatas[MAX_DIRECTIONAL_LIGHT + _numLight.Point++] = light->GetLightData();
            break;
        case Light::Type::SPOT:
            if (_numLight.Spot >= MAX_SPOT_LIGHT)
                continue;
            _lightDatas[MAX_DIRECTIONAL_LIGHT + MAX_POINT_LIGHT + _numLight.Spot++] = light->GetLightData();
            break;
        case Light::Type::SHADOWPOINT:
            if (_numLight.ShadowPoint >= MAX_SHADOW_POINT_LIGHT)
                continue;
            _lightDatas[MAX_DIRECTIONAL_LIGHT + MAX_POINT_LIGHT + MAX_SPOT_LIGHT + _numLight.ShadowPoint++] = light->GetLightData();
            break;
        }
    }

    _cameraBuffer->UpdateBuffer(&cameraData);
    _RaycameraBuffer->UpdateBuffer(&RaycameraData);
    _lightBuffer->UpdateBuffer(_lightDatas.data());
}

void RenderScene::UpdateObject()
{        
    int   mainLight    = 0;
    float maxIntensity = 0.0f;

    for (int i = 0; i < MAX_DIRECTIONAL_LIGHT; i++)
    {
        if (maxIntensity < _lightDatas[i].Intensity)
        {
            maxIntensity = _lightDatas[i].Intensity;
            mainLight    = i;
        }
    }

    Vector3 lightDirection = (_lightDatas[mainLight].float3_1);
    lightDirection.Normalize();   

    _activeMeshes[STATIC_MESH].clear();
    _activeMeshes[SKELETAL_MESH].clear();
    _matrices.clear();
    _boneMatrices.clear();
    _staticMeshInstanceIDs.clear();
    _skeletalMeshInstanceIDs.clear();

    UINT index = 0;
    for (auto& component : _meshRenderQueue)
    {
        if (!component->IsActive())
        {
            continue;
        }

        const auto& model = component->GetModel();

        if (nullptr == model)
        {
            continue;
        }

        const auto  type           = component->GetType();
        const auto& customDepths   = component->GetCustomDepths();
        auto&       materials      = component->GetMaterials();
        auto&       modelMaterials = model->GetMaterials();
        const auto& meshes         = model->GetMeshes();
        const auto& textures       = model->GetTextures();

        BoneMatrices boneMatrices;
        MatrixData   matrixData          = {.World = component->GetWorldMatrix()};
        float        determinant         = XMMatrixDeterminant(matrixData.World).m128_f32[0];
        matrixData.InverseTransposeWorld = XMMatrixInverse(nullptr, matrixData.World);
        matrixData.World                 = XMMatrixTranspose(matrixData.World);

        if (SKELETAL_MESH == type)
        {
            Animator* animator = static_cast<Animator*>(component->GetAnimator());
            if (animator) memcpy(&boneMatrices, animator->GetAnimationTransform(), sizeof(BoneMatrices));
        }

        _matrices.push_back(matrixData);
        _boneMatrices.push_back(boneMatrices);

        auto& skinnedBuffers = component->GetDXRSkeletalMeshes();
        UINT size = (UINT)meshes.size();
                
        for (UINT i = 0; i < size; i++)
        {
            InstanceData instanceData{};

            if (modelMaterials[i].IsTwoSided)
            {
                materials[i].CullMode = Material::CullModeType::CULL_NONE;
            }
            else
            {
                materials[i].CullMode = determinant < 0.f ? Material::CullModeType::CULL_FRONT : Material::CullModeType::CULL_BACK;
            }

            for (UINT j = 0; j < 4; j++)
            {
                instanceData.MaterialID[j] = textures[i][j]->GetID();
            }

            instanceData.CustomDepth = customDepths[i];
            instanceData.MatrixID    = index;
            instanceData.Alpha       = materials[i].Alpha;

            DXRSkeletalMesh* skinnedMesh = nullptr;
            if (Global::isRayTracing)
            {
                skinnedMesh = SKELETAL_MESH == type ? skinnedBuffers[i].get() : nullptr;
            }

            _activeMeshes[type].emplace_back(instanceData, 
                                             materials[i], 
                                             meshes[i].get(),
                                             component,
                                             skinnedMesh, 
                                             0.f);
        }

        if (SKELETAL_MESH == type)
        {
            _skeletalMeshInstanceIDs.push_back(index);
        }
        else if (STATIC_MESH == type)
        {
            _staticMeshInstanceIDs.push_back(index);
        }
        index++;
    }

    for (auto& activeMesh : _activeMeshes)
    {
        std::stable_sort(activeMesh.begin(), activeMesh.end(),
                         [](const auto& a, const auto& b) { return a.Mesh > b.Mesh; });
    }
}

void RenderScene::UpdateUI()
{            
    std::vector<std::tuple<Matrix, UIMaterial, UIRenderer*>> uiDatas;
    uiDatas.reserve(_uiRenderQueue.size());    
    for (auto& component : _uiRenderQueue)
    {
        if (!component->IsActive())
            continue;

        if (UIRenderer::Type::SPRITE == component->GetType())
        {
            SpriteRenderer* spriteComponent = static_cast<SpriteRenderer*>(component);

            auto texture = spriteComponent->GetTexture();
            if (nullptr == texture)
                continue;

            auto     size        = spriteComponent->GetSize();
            XMMATRIX world       = spriteComponent->GetWorldMatrix();
            XMMATRIX scale       = XMMatrixIdentity();
            XMMATRIX translation = XMMatrixIdentity();

            switch (spriteComponent->GetUIType())
            {
                case UIType::MODE_2D:
                    scale       = XMMatrixScaling((float)size.cx, (float)-size.cy, 1.f);
                    translation = XMMatrixTranslation(size.cx * 0.5f, size.cy * 0.5f, 0.f);
                    break;

                case UIType::MODE_3D: {
                    XMVECTOR s, r, t;
                    XMMatrixDecompose(&s, &r, &t, world);

                    XMVECTOR combine = XMQuaternionMultiply(r, _camera->GetRotation());
                    world            = XMMatrixScalingFromVector(s) * XMMatrixRotationQuaternion(combine) * XMMatrixTranslationFromVector(t);
                    [[fallthrough]];
                }

                case UIType::MODE_25D: {
                    float ratio = (float)size.cx / (float)size.cy;
                    scale       = XMMatrixScaling(ratio, 1.f, 1.f);
                    break;
                }
            }

            UIMaterial uiMaterial{.ID          = texture->GetID(),
                                  .Alpha       = spriteComponent->GetAlpha(),
                                  .NumColmn    = spriteComponent->GetNumColumn(),
                                  .NumRow      = spriteComponent->GetNumRow(),
                                  .ColumnIndex = spriteComponent->GetColumnIndex(),
                                  .RowIndex    = spriteComponent->GetRowIndex()};

            uiDatas.emplace_back(scale * world * translation, uiMaterial, component);
        }
        else if (UIRenderer::Type::TEXT == component->GetType())
        {
            SDFTextRenderer* textComponent = static_cast<SDFTextRenderer*>(component);
            if (textComponent->IsActive())
            {
                const float fontSize = textComponent->GetFontSize();
                XMMATRIX    scale    = XMMatrixScaling(fontSize, fontSize, 1.f);
                XMMATRIX    rotation = XMMatrixRotationQuaternion(Quaternion::CreateFromYawPitchRoll(textComponent->GetRotation()));
                XMMATRIX translation = XMMatrixTranslationFromVector(textComponent->GetPosition());
                uiDatas.emplace_back(scale * rotation * translation, UIMaterial{}, component);
            }
        }
    }
    
    std::stable_sort(uiDatas.begin(), uiDatas.end(), [](const auto& a, const auto& b) {
        const auto& [lMatrix, lMaterial, lRenderer] = a;
        const auto& [rMatrix, rMaterial, rRenderer] = b;
        return lMatrix.Translation().z > rMatrix.Translation().z;
    });

    _uiMatrices.clear();
    _uiMaterials.clear();
    _activeUIs.clear();
    for (auto& [matrix, material, component] : uiDatas)
    {
        _uiMatrices.emplace_back(XMMatrixTranspose(matrix));
        _uiMaterials.emplace_back(material);
        _activeUIs.push_back(component);
    }
}

void RenderScene::CreateRenderTarget()
{
    auto                         resolution = Global::device->GetResolution();
    SharedResource<RenderTarget> renderTarget;
    auto desc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R32G32B32A32_FLOAT, resolution.cx, resolution.cy, 1, 1, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);

    _meshRenderTargetName = _name + "_MeshRenderTarget";
    renderTarget          = MakeSharedResource<RenderTarget>();
    renderTarget->Initialize(desc, 0.247f);
    renderTarget->TransitionResource(_commandSet, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

    Global::multiRenderTargetManager->AddRenderTarget(_meshRenderTargetName, renderTarget);

    _finalTargetName = _name + "_FinalTarget";
    renderTarget     = MakeSharedResource<RenderTarget>();
    renderTarget->Initialize(desc, 0.247f);
    renderTarget->TransitionResource(_commandSet, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

    Global::multiRenderTargetManager->AddRenderTarget(_finalTargetName, renderTarget);

    _sharedRenderTarget.resize(SWAPCHAIN_BUFFER_COUNT);
    for (auto& target : _sharedRenderTarget)
    {
        target = MakeSharedResource<RenderTarget>();
        target->Initialize(desc, 1.f);
        target->TransitionResource(_commandSet, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    }
}

void RenderScene::CreateDepthStencil()
{
    _depthStencilView = MakeSharedResource<DepthStencilView>();

    auto mode = Global::device->GetMode();
    mode.Format = DXGI_FORMAT_D32_FLOAT;
    auto desc   = CD3DX12_RESOURCE_DESC::Tex2D(mode.Format, mode.Width, mode.Height, 1, 1, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);
    _depthStencilView->Initialize(desc);

    Global::dxResourceManager->AddResource(_depthStencilView);
}

void RenderScene::CreateFrameResource()
{
    _frameResources.resize(SWAPCHAIN_BUFFER_COUNT);
    
    for (UINT i = 0; i < SWAPCHAIN_BUFFER_COUNT; ++i)
    {
        _frameResources[i] = std::make_unique<FrameResource>();

        // Object Transform
        _frameResources[i]->AddFrameResource(sizeof(MatrixData), MAX_OBJECTS);

        // Object BoneTransform
        _frameResources[i]->AddFrameResource(sizeof(BoneMatrices), MAX_OBJECTS);

        // UI Transform
        _frameResources[i]->AddFrameResource(sizeof(Matrix), MAX_OBJECTS);

        // UI Material
        _frameResources[i]->AddFrameResource(sizeof(UIMaterial), MAX_OBJECTS);

        // Vertex Buffer ID
        _frameResources[i]->AddFrameResource(sizeof(VertexBufferID), MAX_OBJECTS);

        // Index Buffer ID
        _frameResources[i]->AddFrameResource(sizeof(IndexBufferID), MAX_OBJECTS);

        //  Mesh Instance ID
        _frameResources[i]->AddFrameResource(sizeof(MeshInstanceID), MAX_OBJECTS);        
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