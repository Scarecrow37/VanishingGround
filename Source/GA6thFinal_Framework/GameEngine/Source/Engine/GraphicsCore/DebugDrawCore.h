#pragma once

class DebugDrawCore
{
    enum ShapeType
    {
        SPHERE,
        BOX,
        OBB,
        FRUSTUM,
        RING,
        RAY,
        SPOT_LIGHT,
        QUAD
    };

    struct DebugRing
    {
        XMFLOAT3 Origin;
        XMFLOAT3 MajorAxis;
        XMFLOAT3 MinorAxis;
        DebugRing(FXMVECTOR origin, FXMVECTOR majorAxis, FXMVECTOR minorAxis)
        {
            XMStoreFloat3(&Origin, origin);
            XMStoreFloat3(&MajorAxis, majorAxis);
            XMStoreFloat3(&MinorAxis, minorAxis);
        }
    };

    struct DebugRay
    {
        XMFLOAT3 Origin;
        XMFLOAT3 Direction;
        bool     Normalize;
        DebugRay(FXMVECTOR origin, FXMVECTOR direction, const bool normalize) : Normalize(normalize)
        {
            XMStoreFloat3(&Origin, origin);
            XMStoreFloat3(&Direction, direction);
        }
    };

    struct DebugSpotLight
    {
        XMFLOAT3 Position;
        XMFLOAT3 Direction;
        float    Range;
        float    InnerCone;
        float    OuterCone;
        DebugSpotLight(FXMVECTOR position, FXMVECTOR direction, const float range, const float innerCone,
                       const float outerCone)
            : Range(range), InnerCone(innerCone), OuterCone(outerCone)
        {
            XMStoreFloat3(&Position, position);
            XMStoreFloat3(&Direction, direction);
        }
    };

    struct DebugQuad
    {
        XMFLOAT3 PointA;
        XMFLOAT3 PointB;
        XMFLOAT3 PointC;
        XMFLOAT3 PointD;
        DebugQuad(const POINT point, const SIZE size)
            : PointA(static_cast<float>(point.x), static_cast<float>(point.y), 0),
              PointB(static_cast<float>(point.x + size.cx), static_cast<float>(point.y), 0),
              PointC(static_cast<float>(point.x + size.cx), static_cast<float>(point.y + size.cy), 0),
              PointD(static_cast<float>(point.x), static_cast<float>(point.y + size.cy), 0)
        {
        }
        DebugQuad(FXMVECTOR pointA, FXMVECTOR pointB, FXMVECTOR pointC, GXMVECTOR pointD)
        {
            XMStoreFloat3(&PointA, pointA);
            XMStoreFloat3(&PointB, pointB);
            XMStoreFloat3(&PointC, pointC);
            XMStoreFloat3(&PointD, pointD);
        }
    };

    using DrawShape = std::variant<BoundingSphere, BoundingBox, BoundingOrientedBox, BoundingFrustum, DebugRing,
                                   DebugRay, DebugSpotLight, DebugQuad>;
    struct DrawData
    {
        ShapeType Type;
        XMFLOAT4  Color;
        DrawShape Shape;
        DrawData(const ShapeType type, FXMVECTOR color, const DrawShape& shape) : Type(type), Shape(shape)
        {
            XMStoreFloat4(&Color, color);
        }
    };

public:
    DebugDrawCore();
    ~DebugDrawCore();

public:
    void XM_CALLCONV Draw(std::string_view sceneName, const BoundingSphere& sphere,
                          FXMVECTOR color = DirectX::Colors::White);
    void XM_CALLCONV Draw(std::string_view sceneName, const BoundingBox& box, FXMVECTOR color = DirectX::Colors::White);
    void XM_CALLCONV Draw(std::string_view sceneName, const BoundingOrientedBox& obb,
                          FXMVECTOR color = DirectX::Colors::White);
    void XM_CALLCONV Draw(std::string_view sceneName, const BoundingFrustum& frustum,
                          FXMVECTOR color = DirectX::Colors::White);
    void XM_CALLCONV DrawRing(std::string_view sceneName, FXMVECTOR origin, FXMVECTOR majorAxis, FXMVECTOR minorAxis,
                              GXMVECTOR color = DirectX::Colors::White);
    void XM_CALLCONV DrawRay(std::string_view sceneName, FXMVECTOR origin, FXMVECTOR direction, bool normalize = true,
                             FXMVECTOR color = DirectX::Colors::White);
    void XM_CALLCONV DrawSpotLight(std::string_view sceneName, FXMVECTOR position, FXMVECTOR direction, float range,
                                   float innerCone, float outerCone, FXMVECTOR color = DirectX::Colors::White);
    void XM_CALLCONV Draw(std::string_view sceneName, const DebugQuad& quad, FXMVECTOR color = DirectX::Colors::White);

public:
    void Initialize();
    void Render();

private:
    std::unordered_map<std::string_view, std::vector<DrawData>> _drawDatas;
    std::unique_ptr<PrimitiveBatch<VertexPositionColor>>        _primitiveBatch;
    std::unique_ptr<BasicEffect>                                _basicEffect;
    ComPtr<ID3D12GraphicsCommandList>                           _commandList;
    ComPtr<ID3D12CommandAllocator>                              _commandAllocator;
    RenderTarget*                                               _renderTarget;
};