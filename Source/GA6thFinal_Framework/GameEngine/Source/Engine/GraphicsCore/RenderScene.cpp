#include "pch.h"
#include "RenderScene.h"
#include "FrameResource.h"
#include "Model.h"
#include "Quad.h"
#include "RenderPass.h"
#include "RenderTarget.h"
#include "RenderTechnique.h"
#include "ShaderBuilder.h"
#include "MeshRenderer.h"
#include "SkyBox.h"
#include "Animator.h"
#include "Light.h"

RenderScene::RenderScene(std::string_view name)
    : _frameQuad{std::make_unique<Quad>()}
    , _frameShader{std::make_unique<ShaderBuilder>()}
    , _skyBox{std::make_unique<SkyBox>()}
    , _name(name)
{
    _lightDatas.resize(MAX_LIGHT);
}

RenderScene::~RenderScene() {}

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
                          .Position   = cameraPos};

    auto& lights = UmLightCore.GetLights(_name.c_str());

    _numLight = {};
    for (auto& light : lights)
    {
        if (!light->_isActive)
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

    std::unordered_map<size_t, UINT>         materialPair;
    std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> handles;
    std::vector<MaterialData>                materialDatas;
    UINT                                     materialID = 0;

    _worldMatrixes.clear();
    _boneMatrixes.clear();
    for (auto& [isDestroy, component] : _renderQueue)
    {
        if (!component->IsActive())
            continue;

        const auto  type      = component->GetType();
        const auto& model     = component->GetModel();
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
            MaterialData materialData{};

            for (UINT j = 0; j < 4; j++)
            {
                if (nullptr == textures[i][j])
                    continue;

                auto iter = materialPair.find(textures[i][j]->GetHandle().ptr);
                if (iter == materialPair.end())
                {
                    materialPair.emplace(textures[i][j]->GetHandle().ptr, materialID);
                    materialData.ID[j] = materialID++;
                    handles.push_back(textures[i][j]->GetHandle());
                }
                else
                {
                    materialData.ID[j] = iter->second;
                }
            }

            materialDatas.push_back(materialData);
        }
    }

    UINT size = static_cast<UINT>(_worldMatrixes.size());
    ID3D12GraphicsCommandList* commandList = UmDevice.GetCommandList();

    _frameResources[_currentFrameIndex]->CopyStructuredBuffer(commandList, _worldMatrixes.data(), size * sizeof(ObjectData), FrameResource::Type::TRANSFORM);
    _frameResources[_currentFrameIndex]->CopyStructuredBuffer(commandList, _boneMatrixes.data(), size * sizeof(BoneMatrixes), FrameResource::Type::BONE_MATRIXES);
    _frameResources[_currentFrameIndex]->CopyStructuredBuffer(commandList, materialDatas.data(), size * sizeof(MaterialData), FrameResource::Type::MATERIAL);
    _frameResources[_currentFrameIndex]->CopyDescriptors(handles);
}

void RenderScene::RegisterOnRenderQueue(MeshRenderer* component)
{
    auto iter = std::find_if(_renderQueue.begin(), _renderQueue.end(), 
        [component](const auto& pair) { return !pair.first.get(); }); // isDestroy()가 false면 중복된 것

    if (iter != _renderQueue.end())
    {
        GRAPHICS_ASSERT(false, L"RenderScene::RegisterRenderQueue : Already registered component.");
        return;
    }

    _renderQueue.emplace_back(std::make_unique<bool>(false), component);
    component->_isDestroy = _renderQueue.back().first.get();
}

void RenderScene::Execute(ID3D12GraphicsCommandList* commandList)
{
    // 메쉬 최종 타겟 클리어
    ComPtr<ID3D12Resource>   rt = _meshLightingTarget->GetResource();

    CD3DX12_RESOURCE_BARRIER br = CD3DX12_RESOURCE_BARRIER::Transition(
        rt.Get(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET);
    commandList->ResourceBarrier(1, &br);

    auto  handle     = _meshLightingTarget->GetRTVHandle();
    float clearValue = _meshLightingTarget->clearValue;
    Color clearColor = {clearValue, clearValue, clearValue, 1.f};
    commandList->ClearRenderTargetView(handle, clearColor, 0, nullptr);

    for (auto& tech : _techniques)
    {
        tech->Execute(commandList);
    }
}

D3D12_CPU_DESCRIPTOR_HANDLE RenderScene::GetFinalImage()
{
    return _meshLightingTarget->GetSRVHandle();
}

void RenderScene::SetSkyBox(std::string_view path)
{
    _skyBox->SetTexture(path.data());
}

void RenderScene::ResetSkyBox()
{
    _skyBox->ResetResource();
}

void RenderScene::AddRenderTechnique(std::shared_ptr<RenderTechnique> technique)
{
    ID3D12GraphicsCommandList* commandList = UmDevice.GetCommandList();
    technique->SetOwnerScene(this);
    technique->Initialize(commandList);
    _techniques.push_back(technique);
}

// 250424
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void RenderScene::InitializeRenderScene()
{
    CreateCamera();
    CreateRenderTarget();
    CreateDepthStencil();
    CreateFrameQuadAndFrameShader();
    CreateFramePSO();
    CreateSrvDescriptorHeap();
    CreateFrameResource();
}

void RenderScene::CreateRenderTarget()
{
    // gbuffer 생성
    _gBuffer.resize(_gBufferCount);
    for (UINT i = 0; i <= GBuffer::WORLDPOSITION; ++i)
    {
        _gBuffer[i] = std::make_shared<RenderTarget>();
        _gBuffer[i]->Initialize(DXGI_FORMAT_R32G32B32A32_FLOAT, 0.247f);
        _gBuffer[i]->CreateShaderResourceView();
    }
  
    _gBuffer[GBuffer::DEPTH] = std::make_shared<RenderTarget>();
    _gBuffer[GBuffer::DEPTH]->Initialize(DXGI_FORMAT_R32_FLOAT, 1.f);
    _gBuffer[GBuffer::DEPTH]->CreateShaderResourceView();
    
    _gBuffer[GBuffer::CUSTOMDEPTH] = std::make_shared<RenderTarget>();
    _gBuffer[GBuffer::CUSTOMDEPTH]->Initialize(DXGI_FORMAT_R32_UINT, 1.f);
    _gBuffer[GBuffer::CUSTOMDEPTH]->CreateShaderResourceView();
    
    // 후처리용으로 돌려쓸 renderTarget 생성해주기
    _renderTargets.resize(_renderTargetPoolCount);
    for (UINT i = 0; i < _renderTargetPoolCount; ++i)
    {
        _renderTargets[i] = std::make_shared<RenderTarget>();
        _renderTargets[i]->Initialize(DXGI_FORMAT_R32G32B32A32_FLOAT, 0.247f);
        _renderTargets[i]->CreateShaderResourceView();
    }

    // 메쉬 음영처리가 된 타겟 하나 생성 -> 이 타겟을 가져와서 후처리를 진행해야함.
    _meshLightingTarget = std::make_shared<RenderTarget>();
    _meshLightingTarget->Initialize(DXGI_FORMAT_R32G32B32A32_FLOAT, 0.247f);
    _meshLightingTarget->CreateShaderResourceView();
}

void RenderScene::CreateDepthStencil() 
{
    UmViewManager.AddDescriptorHeap(ViewManager::Type::DEPTH_STENCIL, _depthStencilHandle);

    D3D12_RESOURCE_DESC depthDesc{.Dimension        = D3D12_RESOURCE_DIMENSION_TEXTURE2D,
                                  .Alignment        = 0,
                                  .Width            = UmDevice.GetMode().Width,
                                  .Height           = UmDevice.GetMode().Height,
                                  .DepthOrArraySize = 1,
                                  .MipLevels        = 1,
                                  .Format           = DXGI_FORMAT_R24G8_TYPELESS,
                                  .SampleDesc{.Count   = UmDevice.GetMSAAState() ? (UINT)4 : (UINT)1,
                                              .Quality = UmDevice.GetMSAAState() ? (UmDevice.GetMSAAQuality() - 1) : 0},
                                  .Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN,
                                  .Flags  = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL};

    D3D12_CLEAR_VALUE   optClear{.Format = UmDevice.GetDepthStencilFormat(), .DepthStencil{.Depth = 1.f, .Stencil = 0}};
    CD3DX12_HEAP_PROPERTIES property(D3D12_HEAP_TYPE_DEFAULT);

    HRESULT hr = S_OK;
    hr = UmDevice.GetDevice()->CreateCommittedResource(&property, D3D12_HEAP_FLAG_NONE, &depthDesc,
                                                       D3D12_RESOURCE_STATE_PRESENT, &optClear,
                                                       IID_PPV_ARGS(&_depthStencilBuffer));
    FAILED_CHECK_MESSAGE(hr, L"RenderScene::CreateDepthStencil UmDevice.GetDevice()->CreateCommittedResource Failed");

    D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{.Format        = UmDevice.GetDepthStencilFormat(),
                                          .ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D,
                                          .Flags         = D3D12_DSV_FLAG_NONE};

    UmDevice.GetDevice()->CreateDepthStencilView(_depthStencilBuffer.Get(), &dsvDesc, _depthStencilHandle);
}

void RenderScene::CreateFrameQuadAndFrameShader()
{
    // 화면 크기만한 quad만들기. NDC 좌표계로
    _frameQuad->Initialize(-1.f, 1.f, 2.f, 2.f, 0.f);
    _frameShader->BeginBuild();
    _frameShader->SetShader(L"../Shaders/vs_quad.hlsl", ShaderBuilder::Type::VS);
    _frameShader->SetShader(L"../Shaders/ps_quad_frame.hlsl", ShaderBuilder::Type::PS);
    _frameShader->EndBuild();
}

void RenderScene::CreateFramePSO()
{    
    D3D12_GRAPHICS_PIPELINE_STATE_DESC psodesc;
    ZeroMemory(&psodesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
    psodesc.RasterizerState       = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    psodesc.BlendState            = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    psodesc.DepthStencilState     = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
    psodesc.SampleMask            = UINT_MAX;
    psodesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    psodesc.InputLayout           = _frameShader->GetInputLayout();
    psodesc.NumRenderTargets      = 1;
    psodesc.RTVFormats[0]         = UmDevice.GetMode().Format;
    psodesc.DSVFormat             = DXGI_FORMAT_D24_UNORM_S8_UINT;
    psodesc.pRootSignature        = _frameShader->GetRootSignature();
    psodesc.SampleDesc            = {1, 0};
    psodesc.VS                    = _frameShader->GetShaderByteCode(ShaderBuilder::Type::VS);
    psodesc.PS                    = _frameShader->GetShaderByteCode(ShaderBuilder::Type::PS);

    ID3D12Device* device = UmDevice.GetDevice();
    HRESULT       hr     = device->CreateGraphicsPipelineState(&psodesc, IID_PPV_ARGS(&_framePSO));
    FAILED_CHECK_MESSAGE(hr, L"RenderScene::CreateFramePSO device->CreateGraphicsPipelineState Faild");
}

void RenderScene::CreateSrvDescriptorHeap() 
{
    D3D12_DESCRIPTOR_HEAP_DESC desc = {};
    desc.NumDescriptors             = 3;
    desc.Type                       = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    desc.Flags                      = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

    ID3D12Device* device = UmDevice.GetDevice();
    HRESULT hr = device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&_srvDescriptorHeap));
    FAILED_CHECK_MESSAGE(hr, L"RenderScene::CreateSrvDescriptorHeap device->CreateDescriptorHeap Failed");
}

void RenderScene::CreateFrameResource()
{
    _frameResources.resize(SWAPCHAIN_BUFFER_COUNT);
    for (UINT i = 0; i < SWAPCHAIN_BUFFER_COUNT; ++i)
    {
        _frameResources[i] = std::make_shared<FrameResource>();
        // 임시 텍스쳐 갯수가 달라질 수 있는거 아닌가요?
        _frameResources[i]->Initialize(1000, 4000);
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