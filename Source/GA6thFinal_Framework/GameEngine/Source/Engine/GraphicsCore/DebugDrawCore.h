#pragma once

class DebugDrawCore
{
    enum Type { SPHERE, BOX, OBB, FRUSTUM, GRID, DEBUG_GRID, RING, RAY, SPOT_LIGHT };

    struct Grid
    {
        XMVECTOR AxisX;
        XMVECTOR AxisY;
        XMVECTOR Origin;
        size_t   DivsX;
        size_t   DivsY;
    };

    struct DebugGrid
    {
        XMVECTOR CameraPosition;
        size_t   LinesPerSide;
        float    FarZ;
    };

    struct DebugRing
    {
        XMVECTOR Origin;
        XMVECTOR MajorAxis;
        XMVECTOR MinorAxis;
    };

    struct DebugRay
    {
        XMVECTOR Origin;
        XMVECTOR Direction;
        bool     Normalize;
    };

    struct DebugSpotLight
    {
        XMVECTOR Position;
        XMVECTOR Direction;
        float    Range;
        float    InnerCone;
        float    OuterCone;
    };

    using DrawShape = std::variant<BoundingSphere, BoundingBox, BoundingOrientedBox, BoundingFrustum, Grid, DebugRing, DebugRay, DebugSpotLight, DebugGrid>;
    struct DrawData
    {
        Type      Type;
        XMVECTOR  Color;
        DrawShape Shape;
    };

public:
    DebugDrawCore();
    ~DebugDrawCore();

public:
    void XM_CALLCONV Draw(std::string_view sceneName, const BoundingSphere& sphere, FXMVECTOR color = DirectX::Colors::White);
    void XM_CALLCONV Draw(std::string_view sceneName, const BoundingBox& box, FXMVECTOR color = DirectX::Colors::White);
    void XM_CALLCONV Draw(std::string_view sceneName, const BoundingOrientedBox& obb, FXMVECTOR color = DirectX::Colors::White);
    void XM_CALLCONV Draw(std::string_view sceneName, const BoundingFrustum& frustum, FXMVECTOR color = DirectX::Colors::White);
    void XM_CALLCONV DrawGrid(std::string_view sceneName, FXMVECTOR xAxis, FXMVECTOR yAxis, FXMVECTOR origin, size_t xdivs, size_t ydivs, GXMVECTOR color = DirectX::Colors::White);
    void XM_CALLCONV DrawDebugGrid(std::string_view sceneName, FXMVECTOR cameraPosition, float farZ, size_t linesPerSide = 40, GXMVECTOR color = DirectX::Colors::White);
    void XM_CALLCONV DrawRing(std::string_view sceneName, FXMVECTOR origin, FXMVECTOR majorAxis, FXMVECTOR minorAxis, GXMVECTOR color = DirectX::Colors::White);
    void XM_CALLCONV DrawRay(std::string_view sceneName, FXMVECTOR origin, FXMVECTOR direction, bool normalize = true, FXMVECTOR color = DirectX::Colors::White);
    void XM_CALLCONV DrawSpotLight(std::string_view sceneName, FXMVECTOR position, FXMVECTOR direction, float range, float innerCone, float outerCone, FXMVECTOR color = DirectX::Colors::White);

public:
    void Initialize();
    void Render();

private:
    std::unordered_map<std::string_view, std::vector<DrawData>> _drawDatas;
    std::unique_ptr<PrimitiveBatch<VertexPositionColor>>        _primitiveBatch;
    std::unique_ptr<BasicEffect>                                _basicEffect;
    ComPtr<ID3D12GraphicsCommandList>                           _commandList;
    ComPtr<ID3D12CommandAllocator>                              _commandAllocator;
};