#include "pch.h"
#include "Renderer.h"
#include "FrameResource.h"
#include "Model.h"
#include "Shader.h"
#include "UmScripts.h"

#define SeongU01
#ifdef SeongU01
#include "Box.h"
#include "Cylinder.h"
#include "GeoSphere.h"
#include "Grid.h"
#include "PBRTechnique.h"
#include "Quad.h"
#include "RenderScene.h"
#include "Sphere.h"
#endif

Renderer::Renderer()
    : _shader(std::make_unique<Shader>()), _frameResource(std::make_unique<FrameResource>()), _currnetState(0)
{
}

Renderer::~Renderer() {}

void Renderer::RegisterRenderQueue(MeshRenderer* component)
{
    auto iter =
        std::find_if(_components.begin(), _components.end(), [component](const auto& ptr) { return ptr == component; });

    if (iter != _components.end())
    {
        ASSERT(false, L"Renderer::RegisterRenderQueue : Already registered component.");
        return;
    }

    _components.push_back(component);
    // test code
    for (auto& meshRenerComps : _components)
    {
        _renderScenes["TEST PBR"]->RegisterOnRenderQueue(meshRenerComps);
    }
}

void Renderer::Initialize()
{
    std::shared_ptr<RenderScene> testRenderScene = std::make_shared<RenderScene>();
    testRenderScene->InitializeRenderScene(1);
    std::shared_ptr<PBRTechnique> pbrTech = std::make_shared<PBRTechnique>();
    testRenderScene->AddRenderTechnique("PBR", pbrTech);

    _renderScenes["TEST PBR"] = testRenderScene;
}

void Renderer::Update()
{
    // 비활성된 컴포넌트 제거
    auto first = std::remove_if(_components.begin(), _components.end(),
                                [](const auto& ptr) { return (!ptr->Enable || !ptr->gameObject->ActiveInHierarchy); });
    _components.erase(first, _components.end());

    UmMainCamera.Update();

    UmDevice.ResetCommands();
    // UpdateFrameResource();
    UmDevice.ClearBackBuffer(D3D12_CLEAR_FLAG_DEPTH, {0.5f, 0.5f, 0.5f, 1.f});
    for (auto& renderScene : _renderScenes)
    {
        renderScene.second->UpdateRenderScene();
    }
}

void Renderer::Render()
{
    ComPtr<ID3D12GraphicsCommandList> commandList = UmDevice.GetCommandList();

    for (auto& renderScene : _renderScenes)
    {
        renderScene.second->Excute(commandList);
    }
}

void Renderer::Flip()
{
    UmDevice.Flip();
}

HRESULT Renderer::CreatePipelineState()
{
    HRESULT hr = S_OK;

    D3D12_RASTERIZER_DESC rd = {};
    rd.FillMode              = D3D12_FILL_MODE_SOLID;
    rd.CullMode              = D3D12_CULL_MODE_BACK;
    // rd.FrontCounterClockwise = false;
    rd.DepthClipEnable = TRUE;

    D3D12_BLEND_DESC bd                      = {};
    bd.AlphaToCoverageEnable                 = FALSE;
    bd.IndependentBlendEnable                = FALSE;
    bd.RenderTarget[0].BlendEnable           = TRUE; // 알파 블렌딩 활성화
    bd.RenderTarget[0].SrcBlend              = D3D12_BLEND_SRC_ALPHA;
    bd.RenderTarget[0].DestBlend             = D3D12_BLEND_INV_SRC_ALPHA;
    bd.RenderTarget[0].BlendOp               = D3D12_BLEND_OP_ADD;
    bd.RenderTarget[0].SrcBlendAlpha         = D3D12_BLEND_ONE;
    bd.RenderTarget[0].DestBlendAlpha        = D3D12_BLEND_ZERO;
    bd.RenderTarget[0].BlendOpAlpha          = D3D12_BLEND_OP_ADD;
    bd.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

    D3D12_DEPTH_STENCIL_DESC dsd = {};
    dsd.DepthEnable              = TRUE;
    dsd.DepthWriteMask           = D3D12_DEPTH_WRITE_MASK_ALL;
    dsd.DepthFunc                = D3D12_COMPARISON_FUNC_LESS;
    dsd.StencilEnable            = FALSE;

    D3D12_GRAPHICS_PIPELINE_STATE_DESC psd = {};
    psd.pRootSignature                     = _shader->GetRootSignature().Get();
    psd.VS                                 = _shader->GetShaderByteCode(Shader::Type::VS);
    psd.PS                                 = _shader->GetShaderByteCode(Shader::Type::PS);
    // psd.GS = gs;
    // psd.StreamOutput		= {};
    psd.BlendState        = bd;
    psd.SampleMask        = UINT_MAX;
    psd.RasterizerState   = rd;
    psd.DepthStencilState = dsd;
    psd.InputLayout       = _shader->GetInputLayout();
    // psd.IBStripCutValue	= 0;
    psd.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    psd.NumRenderTargets      = 1;
    psd.RTVFormats[0]         = UmDevice.GetMode().Format;
    psd.DSVFormat             = DXGI_FORMAT_D24_UNORM_S8_UINT; // 임시 포맷
    // psd.DSVFormat = 0;						   //DS 포멧. (미지정시 스왑체인에 등록된 DS 버퍼의 포멧 값 사용)
    psd.SampleDesc = {1, 0}; // AA 설정.
    // psd.NodeMask 			= 0;
    // psd.CachedPSO 		= nullptr;
    // psd.Flags 			= D3D12_PIPELINE_STATE_FLAG_NONE;

    // 첫번째 렌더링 상태 객체 : "Solid" (기본상태)
    ComPtr<ID3D12Device> device = UmDevice.GetDevice();

    hr = device->CreateGraphicsPipelineState(&psd, IID_PPV_ARGS(_pipelineState[0].GetAddressOf()));
    FAILED_CHECK_BREAK(hr);

    psd.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
    hr = device->CreateGraphicsPipelineState(&psd, IID_PPV_ARGS(_pipelineState[1].GetAddressOf()));
    FAILED_CHECK_BREAK(hr);

    return hr;
}

void Renderer::UpdateFrameResource()
{
    CameraData cameraData{
        .View       = XMMatrixTranspose(UmMainCamera.GetViewMatrix()),
        .Projection = XMMatrixTranspose(UmMainCamera.GetProjectionMatrix()),
    };

    UmDevice.UpdateBuffer(_cameraBuffer, &cameraData, sizeof(CameraData));

    std::unordered_map<size_t, UINT>         materialPair;
    std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> handles;
    std::vector<XMMATRIX>                    worldMatrixes;
    std::vector<MaterialData>                materialDatas;
    UINT                                     materialID = 0;

    for (auto& component : _components)
    {
        auto& model    = component->GetModel();
        auto& meshes   = model->GetMeshes();
        auto& textures = model->GetTextures();

        XMMATRIX world = XMMatrixTranspose(component->transform->GetWorldMatrix());
        UINT     size  = (UINT)meshes.size();

        for (UINT i = 0; i < size; i++)
        {
            worldMatrixes.emplace_back(world);
            MaterialData materialData{};

            for (UINT j = 0; j < 4; j++)
            {
                if (textures.empty() || nullptr == textures[i][j])
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

    UINT size = static_cast<UINT>(worldMatrixes.size());
    _frameResource->CopyStructuredBuffer(UmDevice.GetCommandList().Get(), worldMatrixes.data(),
                                         size * sizeof(ObjectData), FrameResource::Type::TRANSFORM);
    _frameResource->CopyStructuredBuffer(UmDevice.GetCommandList().Get(), materialDatas.data(),
                                         size * sizeof(MaterialData), FrameResource::Type::MATERIAL);
    _frameResource->CopyDescriptors(handles);
}