#include "pch.h"
#include "DebugDrawCore.h"
#include "RenderScene.h"
#include "RenderTarget.h"
#include "DepthStencilView.h"

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

void XM_CALLCONV DebugDrawCore::DrawGrid(std::string_view sceneName, FXMVECTOR xAxis, FXMVECTOR yAxis, FXMVECTOR origin, size_t xdivs, size_t ydivs, GXMVECTOR color)
{
    _drawDatas[sceneName].emplace_back(Type::GRID, color, Grid{xAxis, yAxis, origin, xdivs, ydivs});
}

void XM_CALLCONV DebugDrawCore::DrawDebugGrid(std::string_view sceneName, FXMVECTOR cameraPosition, float farZ, size_t linesPerSide, GXMVECTOR color)
{
    _drawDatas[sceneName].emplace_back(Type::DEBUG_GRID, color, DebugGrid{cameraPosition, linesPerSide, farZ});
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
    EffectPipelineStateDescription pd(&VertexPositionColor::InputLayout, CommonStates::Opaque, CommonStates::DepthDefault, CommonStates::CullNone, rtState, D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE);

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

        auto renderTarget = UmMultiRenderTargetManager.GetRenderTarget(renderScene->_meshRenderTargetName);
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
        
        _commandList->OMSetRenderTargets(1, &renderTarget->GetRTVHandle(), NULL, &renderScene->_depthStencilView->GetDSVHandle());
        _commandList->RSSetViewports(1, &viewPort);
        _commandList->RSSetScissorRects(1, &scissorRect);

        _basicEffect->Apply(_commandList.Get());
        _primitiveBatch->Begin(_commandList.Get());

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
            case Type::GRID:
            {
                const auto& grid = std::get<Grid>(data.Shape);
                ::DrawGrid(_primitiveBatch.get(), grid.AxisX, grid.AxisY, grid.Origin, grid.DivsX, grid.DivsY, data.Color);
                break;
            }
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
            case Type::DEBUG_GRID:
            {
                const auto& debugGrid = std::get<DebugGrid>(data.Shape);
                ::DrawDebugGrid(_primitiveBatch.get(), debugGrid.CameraPosition, debugGrid.FarZ, debugGrid.LinesPerSide, data.Color);
                break;
            }
            }
        }

        _primitiveBatch->End();
        _drawDatas[sceneName].clear();

        renderTarget->TransitionResource(_commandList.Get(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
        renderScene->_depthStencilView->TransitionResource(_commandList.Get(), D3D12_RESOURCE_STATE_PRESENT);
    }
    
    _commandList->Close();
    UmDevice.RegisterCommand(_commandList.Get(), CommandListType::POST_PROCESS_LIST);
}