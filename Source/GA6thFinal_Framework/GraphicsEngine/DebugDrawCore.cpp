#include "pch.h"
#include "DebugDrawCore.h"

DebugDrawCore::DebugDrawCore() {}

DebugDrawCore::~DebugDrawCore() {}

void XM_CALLCONV DebugDrawCore::Draw(std::string_view sceneName, const BoundingSphere& sphere, FXMVECTOR color)
{
    _drawDatas[sceneName].emplace_back(Type::SPHERE, color, sphere);
}

void XM_CALLCONV DebugDrawCore::Draw(std::string_view sceneName, const BoundingBox& box, FXMVECTOR color)
{
    _drawDatas[sceneName].emplace_back(Type::BOX, color, box);
}

void XM_CALLCONV DebugDrawCore::Draw(std::string_view sceneName, const BoundingOrientedBox& obb, FXMVECTOR color)
{    
    _drawDatas[sceneName].emplace_back(Type::OBB, color, obb);
}

void XM_CALLCONV DebugDrawCore::Draw(std::string_view sceneName, const BoundingFrustum& frustum, FXMVECTOR color)
{    
    _drawDatas[sceneName].emplace_back(Type::FRUSTUM, color, frustum);
}

void XM_CALLCONV DebugDrawCore::DrawRing(std::string_view sceneName, FXMVECTOR origin, FXMVECTOR majorAxis, FXMVECTOR minorAxis, GXMVECTOR color)
{
    _drawDatas[sceneName].emplace_back(Type::RING, color, DebugRing{origin, majorAxis, minorAxis});
}

void XM_CALLCONV DebugDrawCore::DrawRay(std::string_view sceneName, FXMVECTOR origin, FXMVECTOR direction, bool normalize, FXMVECTOR color)
{
    _drawDatas[sceneName].emplace_back(Type::RAY, color, DebugRay{origin, direction, normalize});
}

void XM_CALLCONV DebugDrawCore::DrawSpotLight(std::string_view sceneName, FXMVECTOR position, FXMVECTOR direction, float range, float innerCone, float outerCone, FXMVECTOR color)
{
    _drawDatas[sceneName].emplace_back(Type::SPOT_LIGHT, color, DebugSpotLight{position, direction, range, innerCone, outerCone});
}

void DebugDrawCore::Initialize()
{
    RenderTargetState              rtState(DXGI_FORMAT_R32G32B32A32_FLOAT, DXGI_FORMAT_D24_UNORM_S8_UINT);
    EffectPipelineStateDescription pd(&VertexPositionColor::InputLayout, CommonStates::AlphaBlend, CommonStates::DepthRead, CommonStates::CullNone, rtState, D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE);

    _primitiveBatch = std::make_unique<PrimitiveBatch<VertexPositionColor>>(Global::device->GetDevice());
    try
    {
        _basicEffect = std::make_unique<BasicEffect>(Global::device->GetDevice(), EffectFlags::VertexColor, pd);
    }
    catch (const std::logic_error& e)
    {
        OutputDebugStringA(e.what()); // 또는 브레이크
        MessageBoxA(nullptr, e.what(), "BasicEffect 생성 실패", MB_OK);
    }

    Global::commandController->AddCommandSet(CommandType::DIRECT, L"DebugDrawCore", _commandSet);
}

void DebugDrawCore::Render()
{
    for (const auto& [sceneName, datas] : _drawDatas)
    {
        auto  renderScene = Global::renderer->GetRenderScene(sceneName);
        auto& camera      = renderScene->_camera;

        _basicEffect->SetView(camera->GetViewMatrix());
        _basicEffect->SetProjection(camera->GetProjectionMatrix());

        auto renderTarget = Global::multiRenderTargetManager->GetRenderTarget(renderScene->_finalTargetName);
        renderTarget->TransitionResource(_commandSet, D3D12_RESOURCE_STATE_RENDER_TARGET);
        renderScene->_depthStencilView->TransitionResource(_commandSet, D3D12_RESOURCE_STATE_DEPTH_WRITE);

        auto&          resolution = renderTarget->GetResolution();
        D3D12_VIEWPORT viewPort{.TopLeftX = 0.f,
                                .TopLeftY = 0.f,
                                .Width    = static_cast<FLOAT>(resolution.Width),
                                .Height   = static_cast<FLOAT>(resolution.Height),
                                .MinDepth = 0.f,
                                .MaxDepth = 1.f};
        D3D12_RECT     scissorRect{
                .left = 0, .top = 0, .right = static_cast<LONG>(resolution.Width), .bottom = static_cast<LONG>(resolution.Height)};

        _commandSet->OMSetRenderTargets(1, &renderTarget->GetRTVHandle(), NULL, &renderScene->_depthStencilView->GetDSVHandle());
        _commandSet->RSSetViewports(1, &viewPort);
        _commandSet->RSSetScissorRects(1, &scissorRect);

        _basicEffect->Apply(_commandSet);
        _primitiveBatch->Begin(_commandSet);

        for (const auto& data : datas)
        {
            switch (data.Type)
            {
            case Type::SPHERE:
                ::Draw(_primitiveBatch.get(), std::get<BoundingSphere>(data.Shape), data.Color);
                break;
            case Type::BOX:
                ::Draw(_primitiveBatch.get(), std::get<BoundingBox>(data.Shape), data.Color);
                break;
            case Type::OBB:
                ::Draw(_primitiveBatch.get(), std::get<BoundingOrientedBox>(data.Shape), data.Color);
                break;
            case Type::FRUSTUM:
                ::Draw(_primitiveBatch.get(), std::get<BoundingFrustum>(data.Shape), data.Color);
                break;
            case Type::RING:
            {
                const auto& ring = std::get<DebugRing>(data.Shape);
                ::DrawRing(_primitiveBatch.get(), ring.Origin, ring.MajorAxis, ring.MinorAxis, data.Color);
                break;
            }
            case Type::RAY:
            {
                const auto& ray = std::get<DebugRay>(data.Shape);
                ::DrawRay(_primitiveBatch.get(), ray.Origin, ray.Direction, ray.Normalize, data.Color);
                break;
            }
            case Type::SPOT_LIGHT:
            {
                const auto& spotLight = std::get<DebugSpotLight>(data.Shape);
                ::DrawSpotLight(_primitiveBatch.get(), spotLight.Position, spotLight.Direction, spotLight.Range, spotLight.InnerCone, spotLight.OuterCone, 24, data.Color);
                break;
            }
            }
        }

        _primitiveBatch->End();
        _drawDatas[sceneName].clear();

        renderTarget->TransitionResource(_commandSet, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
        renderScene->_depthStencilView->TransitionResource(_commandSet, D3D12_RESOURCE_STATE_PRESENT);
    }
    _drawDatas.clear();

    _commandSet.ExecuteCommand(CommandQueueType::GRAPHICS_QUEUE);
}