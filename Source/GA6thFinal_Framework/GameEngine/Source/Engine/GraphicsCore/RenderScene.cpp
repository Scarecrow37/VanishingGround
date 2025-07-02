#include "pch.h"
#include "RenderScene.h"
#include "Animator.h"
#include "FrameResource.h"
#include "Light.h"
#include "MeshRenderer.h"
#include "Model.h"
#include "Quad.h"
#include "RenderTarget.h"
#include "RenderTechnique.h"
#include "SkyBox.h"
#include "UnorderedAccessView.h"
#include "DepthStencilView.h"

RenderScene::RenderScene(std::string_view name)
    : _skyBox{std::make_unique<SkyBox>()}
    , _name(name)
{
    _lightDatas.resize(MAX_LIGHT);
}

RenderScene::~RenderScene() {}

D3D12_GPU_DESCRIPTOR_HANDLE RenderScene::GetFinalImage()
{
    RenderTarget* finalTarget = UmMultiRenderTargetManager.GetRenderTarget(_finalTargetName);
    return finalTarget->GetSRVHandle();
}

void RenderScene::SetSkyBox(std::string_view path)
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
}

void RenderScene::RegisterOnRenderQueue(MeshRenderer* component)
{
    auto iter =
        std::find_if(_renderQueue.begin(), _renderQueue.end(), [](const auto& pair) { return !pair.first.get(); });

    if (iter != _renderQueue.end())
    {
        GRAPHICS_ASSERT(false, L"RenderScene::RegisterRenderQueue : Already registered component.");
        return;
    }

    _renderQueue.emplace_back(std::make_unique<bool>(false), component);
    component->_isDestroyeds.push_back(_renderQueue.back().first.get());
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
    // 카메라 업데이트
    _camera->Update();

    // 비활성된 컴포넌트 제거
    auto first = std::remove_if(_renderQueue.begin(), _renderQueue.end(), [](const auto& pair) { return *pair.first; });
    _renderQueue.erase(first, _renderQueue.end());

    _currentFrameIndex   = UmDevice.GetCurrentBackBufferIndex();
    Vector4    cameraPos = Vector4(_camera->GetWorldMatrix().Translation());
    CameraData cameraData{.View       = XMMatrixTranspose(_camera->GetViewMatrix()),
                          .Projection = XMMatrixTranspose(_camera->GetProjectionMatrix()),
                          .ViewInverse       = XMMatrixTranspose(_camera->GetWorldMatrix()),
                          .ProejctionInverse = XMMatrixTranspose(_camera->GetProjectionInverseMatrix()),
                          .Position   = cameraPos};

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

    UmDevice.UpdateBuffer(_cameraBuffer, &cameraData, sizeof(CameraData));
    UmDevice.UpdateBuffer(_lightBuffer, _lightDatas.data(), sizeof(LightData) * MAX_LIGHT);

    _worldMatrixes.clear();
    _boneMatrixes.clear();
    _materialIDs.clear();
    for (auto& [isDestroy, component] : _renderQueue)
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
        BoneMatrixes boneMatrixes{};

        if (MeshRenderType::SKELETAL == type)
        {
            auto animator = component->GetAnimator();
            if (animator) memcpy(&boneMatrixes, animator->GetAnimationTransform(), sizeof(BoneMatrixes));
        }

        UINT size = (UINT)meshes.size();

        for (UINT i = 0; i < size; i++)
        {
            _worldMatrixes.push_back(world);
            _boneMatrixes.push_back(boneMatrixes);

            MaterialID materialID{};
            for (UINT j = 0; j < 4; j++)
            {
                materialID.ID[j] = textures[i][j]->GetID();
            }
            _materialIDs.push_back(materialID);
        }
    }

    UINT                       size        = static_cast<UINT>(_worldMatrixes.size());
    ID3D12GraphicsCommandList* commandList = UmDevice.GetCommandList();

    _frameResources[_currentFrameIndex]->CopyStructuredBuffer(commandList, _worldMatrixes.data(), size * sizeof(XMMATRIX), FrameResource::Type::TRANSFORM);
    _frameResources[_currentFrameIndex]->CopyStructuredBuffer(commandList, _boneMatrixes.data(), size * sizeof(BoneMatrixes), FrameResource::Type::BONE_MATRIXES);
    _frameResources[_currentFrameIndex]->CopyStructuredBuffer(commandList, _materialIDs.data(), size * sizeof(MaterialID), FrameResource::Type::MATERIAL);
}

void RenderScene::Execute(ID3D12GraphicsCommandList* commandList)
{
    auto descriptorHeap = UmViewManager.GetShaderResourceHeap();
    commandList->SetDescriptorHeaps(1, &descriptorHeap);

    _accumulationBuffer->TransitionResource(commandList, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
    _accumulationBuffer->ClearUnorderedAccessView(commandList);

    auto meshRenderTarget = UmMultiRenderTargetManager.GetRenderTarget(_meshRenderTargetName);
    meshRenderTarget->TransitionResource(commandList, D3D12_RESOURCE_STATE_RENDER_TARGET);
    meshRenderTarget->ClearRenderTarget(commandList);

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

    auto mode = UmDevice.GetMode();
    mode.Format = DXGI_FORMAT_R24G8_TYPELESS;
    _depthStencilView->Initialize(mode);

    UmDXResourceManager.AddResource(_depthStencilView);
}

void RenderScene::CreateFrameResource()
{
    _frameResources.resize(SWAPCHAIN_BUFFER_COUNT);
    for (UINT i = 0; i < SWAPCHAIN_BUFFER_COUNT; ++i)
    {
        _frameResources[i] = std::make_unique<FrameResource>();
        _frameResources[i]->Initialize(1000);
    }
    // 임시 : 메인 카메라를 통해 Camera ConstantBuffer 만들기.
    CameraData cameraData{.View       = _camera->GetViewMatrix(),
                          .Projection = _camera->GetProjectionMatrix(),
                          .Position   = {0.f, 0.f, -5.f, 1.f}};

    UmDevice.CreateConstantBuffer(&cameraData, sizeof(CameraData), _cameraBuffer);
    UmDevice.CreateConstantBuffer(nullptr, sizeof(LightData) * MAX_LIGHT, _lightBuffer);
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