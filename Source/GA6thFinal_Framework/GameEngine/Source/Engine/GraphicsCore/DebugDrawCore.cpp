#include "pch.h"
#include "DebugDrawCore.h"

DebugDrawCore::DebugDrawCore() = default;

DebugDrawCore::~DebugDrawCore() = default;

void XM_CALLCONV DebugDrawCore::Draw(std::string_view sceneName, const BoundingSphere& sphere, FXMVECTOR color)
{
    _drawDatas[sceneName].emplace_back(ShapeType::SPHERE, color, sphere);
}

void XM_CALLCONV DebugDrawCore::Draw(std::string_view sceneName, const BoundingBox& box, FXMVECTOR color)
{
    _drawDatas[sceneName].emplace_back(ShapeType::BOX, color, box);
}

void XM_CALLCONV DebugDrawCore::Draw(std::string_view sceneName, const BoundingOrientedBox& obb, FXMVECTOR color)
{
    _drawDatas[sceneName].emplace_back(ShapeType::OBB, color, obb);
}

void XM_CALLCONV DebugDrawCore::Draw(std::string_view sceneName, const BoundingFrustum& frustum, FXMVECTOR color)
{
    _drawDatas[sceneName].emplace_back(ShapeType::FRUSTUM, color, frustum);
}

void XM_CALLCONV DebugDrawCore::DrawRing(std::string_view sceneName, FXMVECTOR origin, FXMVECTOR majorAxis,
                                         FXMVECTOR minorAxis, GXMVECTOR color)
{
    _drawDatas[sceneName].emplace_back(ShapeType::RING, color, DebugRing{origin, majorAxis, minorAxis});
}

void XM_CALLCONV DebugDrawCore::DrawRay(std::string_view sceneName, FXMVECTOR origin, FXMVECTOR direction,
                                        bool normalize, FXMVECTOR color)
{
    _drawDatas[sceneName].emplace_back(ShapeType::RAY, color, DebugRay{origin, direction, normalize});
}

void XM_CALLCONV DebugDrawCore::DrawSpotLight(std::string_view sceneName, FXMVECTOR position, FXMVECTOR direction,
                                              float range, float innerCone, float outerCone, FXMVECTOR color)
{
    _drawDatas[sceneName].emplace_back(ShapeType::SPOT_LIGHT, color,
                                       DebugSpotLight{position, direction, range, innerCone, outerCone});
}

void DebugDrawCore::Draw(const std::string_view sceneName, const DebugQuad& quad, FXMVECTOR color)
{
    _drawDatas[sceneName].emplace_back(ShapeType::QUAD, color, quad);
}

void DebugDrawCore::Initialize()
{
    RenderTargetState              rtState(DXGI_FORMAT_R32G32B32A32_FLOAT, DXGI_FORMAT_D24_UNORM_S8_UINT);
    EffectPipelineStateDescription pd(&VertexPositionColor::InputLayout, CommonStates::AlphaBlend,
                                      CommonStates::DepthRead, CommonStates::CullNone, rtState,
                                      D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE);

    _primitiveBatch = std::make_unique<PrimitiveBatch<VertexPositionColor>>(UmDevice.GetDevice());
    try
    {
        _basicEffect = std::make_unique<BasicEffect>(UmDevice.GetDevice(), EffectFlags::VertexColor, pd);
    }
    catch (const std::logic_error& e)
    {
        OutputDebugStringA(e.what()); // 또는 브레이크
        MessageBoxA(nullptr, e.what(), "BasicEffect 생성 실패", MB_OK);
    }

    UmDevice.CreateCommandList(_commandAllocator, _commandList, CommandType::DIRECT);
}

void DebugDrawCore::Render()
{
    _commandAllocator->Reset();
    _commandList->Reset(_commandAllocator.Get(), nullptr);

    for (const auto& [sceneName, datas] : _drawDatas)
    {
        auto  renderScene = UmRenderer.GetRenderScene(sceneName);
        auto& camera      = renderScene->_camera;

        _basicEffect->SetView(camera->GetViewMatrix());
        _basicEffect->SetProjection(camera->GetProjectionMatrix());

        auto renderTarget = UmMultiRenderTargetManager.GetRenderTarget(renderScene->_finalTargetName);
        renderTarget->TransitionResource(_commandList.Get(), D3D12_RESOURCE_STATE_RENDER_TARGET);
        renderScene->_depthStencilView->TransitionResource(_commandList.Get(), D3D12_RESOURCE_STATE_DEPTH_WRITE);

        auto&          mode = renderTarget->GetMode();
        D3D12_VIEWPORT viewPort{.TopLeftX = 0.f,
                                .TopLeftY = 0.f,
                                .Width    = static_cast<FLOAT>(mode.Width),
                                .Height   = static_cast<FLOAT>(mode.Height),
                                .MinDepth = 0.f,
                                .MaxDepth = 1.f};
        D3D12_RECT     scissorRect{
                .left = 0, .top = 0, .right = static_cast<LONG>(mode.Width), .bottom = static_cast<LONG>(mode.Height)};

        _commandList->OMSetRenderTargets(1, &renderTarget->GetRTVHandle(), NULL,
                                         &renderScene->_depthStencilView->GetDSVHandle());
        _commandList->RSSetViewports(1, &viewPort);
        _commandList->RSSetScissorRects(1, &scissorRect);

        _basicEffect->Apply(_commandList.Get());
        _primitiveBatch->Begin(_commandList.Get());

        for (const auto& data : datas)
        {
            XMVECTOR color = XMLoadFloat4(&data.Color);
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
            case ShapeType::QUAD: {
                const auto& quad   = std::get<DebugQuad>(data.Shape);
                XMVECTOR    pointA = XMLoadFloat3(&quad.PointA);
                XMVECTOR    pointB = XMLoadFloat3(&quad.PointB);
                XMVECTOR    pointC = XMLoadFloat3(&quad.PointC);
                XMVECTOR    pointD = XMLoadFloat3(&quad.PointD);
                ::DrawQuad(_primitiveBatch.get(), pointA, pointB, pointC, pointD, color);
                break;
            }
            }
        }

        _primitiveBatch->End();
        _drawDatas[sceneName].clear();

        renderTarget->TransitionResource(_commandList.Get(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
        renderScene->_depthStencilView->TransitionResource(_commandList.Get(), D3D12_RESOURCE_STATE_PRESENT);
    }
    _drawDatas.clear();

    _commandList->Close();
    UmDevice.RegisterCommand(_commandList.Get(), CommandListType::DEBUG_RENDER_LIST);
}