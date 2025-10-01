#include "pch.h"
#include "DebugDrawCore.h"
#include <d3d11on12.h>

DebugDrawCore::DebugDrawCore() = default;

DebugDrawCore::~DebugDrawCore() = default;

void XM_CALLCONV DebugDrawCore::Draw(std::string_view sceneName, const BoundingSphere& sphere, FXMVECTOR color)
{
    _drawDatas[sceneName].second.emplace_back(ShapeType::SPHERE, color, sphere);
}

void XM_CALLCONV DebugDrawCore::Draw(std::string_view sceneName, const BoundingBox& box, FXMVECTOR color)
{
    _drawDatas[sceneName].second.emplace_back(ShapeType::BOX, color, box);
}

void XM_CALLCONV DebugDrawCore::Draw(std::string_view sceneName, const BoundingOrientedBox& obb, FXMVECTOR color)
{
    _drawDatas[sceneName].second.emplace_back(ShapeType::OBB, color, obb);
}

void XM_CALLCONV DebugDrawCore::Draw(std::string_view sceneName, const BoundingFrustum& frustum, FXMVECTOR color)
{
    _drawDatas[sceneName].second.emplace_back(ShapeType::FRUSTUM, color, frustum);
}

void XM_CALLCONV DebugDrawCore::DrawRing(std::string_view sceneName, FXMVECTOR origin, FXMVECTOR majorAxis,
                                         FXMVECTOR minorAxis, GXMVECTOR color)
{
    _drawDatas[sceneName].second.emplace_back(ShapeType::RING, color, DebugRing{origin, majorAxis, minorAxis});
}

void XM_CALLCONV DebugDrawCore::DrawRay(std::string_view sceneName, FXMVECTOR origin, FXMVECTOR direction,
                                        bool normalize, FXMVECTOR color)
{
    _drawDatas[sceneName].second.emplace_back(ShapeType::RAY, color, DebugRay{origin, direction, normalize});
}

void XM_CALLCONV DebugDrawCore::DrawSpotLight(std::string_view sceneName, FXMVECTOR position, FXMVECTOR direction,
                                              float range, float innerCone, float outerCone, FXMVECTOR color)
{
    _drawDatas[sceneName].second.emplace_back(ShapeType::SPOT_LIGHT, color,
                                              DebugSpotLight{position, direction, range, innerCone, outerCone});
}

void DebugDrawCore::DrawQuad(std::string_view sceneName, FXMVECTOR pointA, FXMVECTOR pointB, FXMVECTOR pointC,
    GXMVECTOR pointD, HXMVECTOR color)
{
    _drawDatas[sceneName].first.emplace_back(ShapeType::QUAD, color, DebugQuad{pointA, pointB, pointC, pointD});
}

void DebugDrawCore::DrawLine(std::string_view sceneName, FXMVECTOR pointA, FXMVECTOR pointB, FXMVECTOR color)
{
    _drawDatas[sceneName].first.emplace_back(ShapeType::LINE, color, DebugLine{pointA, pointB});
}

void DebugDrawCore::DrawCircle(std::string_view sceneName, FXMVECTOR origin, float radius, FXMVECTOR color)
{
    _drawDatas[sceneName].first.emplace_back(ShapeType::CIRCLE, color, DebugCircle{origin, radius});
}

void DebugDrawCore::Initialize()
{
    RenderTargetState              rtState(DXGI_FORMAT_R32G32B32A32_FLOAT, DXGI_FORMAT_D32_FLOAT);
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
    //D3D11On12CreateDevice()

    for (const auto& [sceneName, datas] : _drawDatas)
    {
        auto  renderScene = Global::renderer->GetRenderScene(sceneName);
        auto& camera      = renderScene->_camera;

        auto renderTarget = Global::multiRenderTargetManager->GetRenderTarget(renderScene->_finalTargetName);
        renderTarget->TransitionResource(_commandSet, D3D12_RESOURCE_STATE_RENDER_TARGET);
        renderScene->_depthStencilView->TransitionResource(_commandSet, D3D12_RESOURCE_STATE_DEPTH_READ);

        auto&          resolution = renderTarget->GetResolution();
        D3D12_VIEWPORT viewPort{.TopLeftX = 0.f,
                                .TopLeftY = 0.f,
                                .Width    = static_cast<FLOAT>(resolution.cx),
                                .Height   = static_cast<FLOAT>(resolution.cy),
                                .MinDepth = 0.f,
                                .MaxDepth = 1.f};
        D3D12_RECT     scissorRect{.left = 0, .top = 0, .right = resolution.cx, .bottom = resolution.cy};

        _commandSet->OMSetRenderTargets(1, &renderTarget->GetRTVHandle(), NULL, &renderScene->_depthStencilView->GetDSVHandle());
        _commandSet->RSSetViewports(1, &viewPort);
        _commandSet->RSSetScissorRects(1, &scissorRect);

        _basicEffect->SetView(XMMatrixLookAtLH({0.f, 0.f, -1.f}, {0.f, 0.f, 1.f}, {0.f, 1.f, 0.f}));
        _basicEffect->SetProjection(XMMatrixOrthographicOffCenterLH(0.f, (float)resolution.cx, (float)resolution.cy, 0.f, 0.1f, 1000.f));
        _basicEffect->Apply(_commandSet);
        _primitiveBatch->Begin(_commandSet);

        Draw2D(datas.first);

        _primitiveBatch->End();

        _basicEffect->SetView(camera->GetViewMatrix());
        _basicEffect->SetProjection(camera->GetProjectionMatrix());

        _basicEffect->Apply(_commandSet);
        _primitiveBatch->Begin(_commandSet);

        Draw3D(datas.second);

        _primitiveBatch->End();

        _drawDatas[sceneName].first.clear();
        _drawDatas[sceneName].second.clear();

        renderTarget->TransitionResource(_commandSet, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
        renderScene->_depthStencilView->TransitionResource(_commandSet, D3D12_RESOURCE_STATE_PRESENT);
    }
    _drawDatas.clear();

    _commandSet.ExecuteCommand(GRAPHICS_QUEUE);
}

void DebugDrawCore::Draw2D(const std::vector<DrawData>& drawData) const
{
    for (const auto& data : drawData)
    {
        const XMVECTOR color = XMLoadFloat4(&data.Color);
        switch (data.Type)
        {
        case ShapeType::QUAD: {
            const auto&    quad   = std::get<DebugQuad>(data.Shape);
            const XMVECTOR pointA = XMLoadFloat3(&quad.PointA);
            const XMVECTOR pointB = XMLoadFloat3(&quad.PointB);
            const XMVECTOR pointC = XMLoadFloat3(&quad.PointC);
            const XMVECTOR pointD = XMLoadFloat3(&quad.PointD);
            ::DrawQuad(_primitiveBatch.get(), pointA, pointB, pointC, pointD, color);
            break;
        }
        case ShapeType::LINE: {
            const auto&    line   = std::get<DebugLine>(data.Shape);
            const XMVECTOR pointA = XMLoadFloat3(&line.PointA);
            const XMVECTOR pointB = XMLoadFloat3(&line.PointB);
            ::DrawLine(_primitiveBatch.get(), pointA, pointB, color);
            break;
        }
        case ShapeType::CIRCLE: {
            const auto&    circle = std::get<DebugCircle>(data.Shape);
            const XMVECTOR origin = XMLoadFloat3(&circle.Origin);
            ::DrawCircle(_primitiveBatch.get(), origin, circle.Radius, color);
            break;
        }
        default:
            break;
        }
    }
}

void DebugDrawCore::Draw3D(const std::vector<DrawData>& drawData) const
{
    for (const auto& data : drawData)
    {
        const XMVECTOR color = XMLoadFloat4(&data.Color);
        switch (data.Type)
        {
        case ShapeType::SPHERE:
            ::Draw(_primitiveBatch.get(), std::get<BoundingSphere>(data.Shape), color);
            break;
        case ShapeType::BOX:
            ::Draw(_primitiveBatch.get(), std::get<BoundingBox>(data.Shape), color);
            break;
        case ShapeType::OBB:
            ::Draw(_primitiveBatch.get(), std::get<BoundingOrientedBox>(data.Shape), color);
            break;
        case ShapeType::FRUSTUM:
            ::Draw(_primitiveBatch.get(), std::get<BoundingFrustum>(data.Shape), color);
            break;
        case ShapeType::RING: {
            const auto&    ring      = std::get<DebugRing>(data.Shape);
            const XMVECTOR origin    = XMLoadFloat3(&ring.Origin);
            const XMVECTOR majorAxis = XMLoadFloat3(&ring.MajorAxis);
            const XMVECTOR minorAxis = XMLoadFloat3(&ring.MinorAxis);
            ::DrawRing(_primitiveBatch.get(), origin, majorAxis, minorAxis, color);
            break;
        }
        case ShapeType::RAY: {
            const auto&    ray       = std::get<DebugRay>(data.Shape);
            const XMVECTOR origin    = XMLoadFloat3(&ray.Origin);
            const XMVECTOR direction = XMLoadFloat3(&ray.Direction);
            ::DrawRay(_primitiveBatch.get(), origin, direction, ray.Normalize, color);
            break;
        }
        case ShapeType::SPOT_LIGHT: {
            const auto&    spotLight = std::get<DebugSpotLight>(data.Shape);
            const XMVECTOR position  = XMLoadFloat3(&spotLight.Position);
            const XMVECTOR direction = XMLoadFloat3(&spotLight.Direction);
            ::DrawSpotLight(_primitiveBatch.get(), position, direction, spotLight.Range, spotLight.InnerCone,
                            spotLight.OuterCone, 24, color);
            break;
        }
        default:
            break;
        }
    }
}