#pragma once

class DebugDrawCore
{
    enum Type { SPHERE, BOX, OBB, FRUSTUM, RING, RAY, SPOT_LIGHT };

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

    using DrawShape = std::variant<BoundingSphere, BoundingBox, BoundingOrientedBox, BoundingFrustum, DebugRing, DebugRay, DebugSpotLight>;
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
    CommandSet                                                  _commandSet;
    RenderTarget*                                               _renderTarget;
};