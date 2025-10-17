#include "../include/directxtk12/DebugDraw.h"
#include <vector>
#include <algorithm>

using namespace DirectX;

namespace
{
    inline void XM_CALLCONV DrawCube(PrimitiveBatch<VertexPositionColor>* batch,
        CXMMATRIX matWorld,
        FXMVECTOR color)
    {
        static const XMVECTORF32 s_verts[8] =
        {
            { { { -1.f, -1.f, -1.f, 0.f } } },
            { { {  1.f, -1.f, -1.f, 0.f } } },
            { { {  1.f, -1.f,  1.f, 0.f } } },
            { { { -1.f, -1.f,  1.f, 0.f } } },
            { { { -1.f,  1.f, -1.f, 0.f } } },
            { { {  1.f,  1.f, -1.f, 0.f } } },
            { { {  1.f,  1.f,  1.f, 0.f } } },
            { { { -1.f,  1.f,  1.f, 0.f } } }
        };

        static const WORD s_indices[] =
        {
            0, 1,
            1, 2,
            2, 3,
            3, 0,
            4, 5,
            5, 6,
            6, 7,
            7, 4,
            0, 4,
            1, 5,
            2, 6,
            3, 7
        };

        VertexPositionColor verts[8];
        for (size_t i = 0; i < 8; ++i)
        {
            XMVECTOR v = XMVector3Transform(s_verts[i], matWorld);
            XMStoreFloat3(&verts[i].position, v);
            XMStoreFloat4(&verts[i].color, color);
        }

        batch->DrawIndexed(D3D_PRIMITIVE_TOPOLOGY_LINELIST, s_indices, static_cast<UINT>(std::size(s_indices)), verts, 8);
    }
}

void XM_CALLCONV Draw(PrimitiveBatch<VertexPositionColor>* batch,
    const BoundingSphere& sphere,
    FXMVECTOR color)
{
    XMVECTOR origin = XMLoadFloat3(&sphere.Center);

    const float radius = sphere.Radius;

    XMVECTOR xaxis = g_XMIdentityR0 * radius;
    XMVECTOR yaxis = g_XMIdentityR1 * radius;
    XMVECTOR zaxis = g_XMIdentityR2 * radius;

    DrawRing(batch, origin, xaxis, zaxis, color);
    DrawRing(batch, origin, xaxis, yaxis, color);
    DrawRing(batch, origin, yaxis, zaxis, color);
}

void XM_CALLCONV Draw(PrimitiveBatch<VertexPositionColor>* batch,
    const BoundingBox& box,
    FXMVECTOR color)
{
    XMMATRIX matWorld = XMMatrixScaling(box.Extents.x, box.Extents.y, box.Extents.z);
    XMVECTOR position = XMLoadFloat3(&box.Center);
    matWorld.r[3] = XMVectorSelect(matWorld.r[3], position, g_XMSelect1110);

    DrawCube(batch, matWorld, color);
}

void XM_CALLCONV Draw(PrimitiveBatch<VertexPositionColor>* batch,
    const BoundingOrientedBox& obb,
    FXMVECTOR color)
{
    XMMATRIX matWorld = XMMatrixRotationQuaternion(XMLoadFloat4(&obb.Orientation));
    XMMATRIX matScale = XMMatrixScaling(obb.Extents.x, obb.Extents.y, obb.Extents.z);
    matWorld = XMMatrixMultiply(matScale, matWorld);
    XMVECTOR position = XMLoadFloat3(&obb.Center);
    matWorld.r[3] = XMVectorSelect(matWorld.r[3], position, g_XMSelect1110);

    DrawCube(batch, matWorld, color);
}

void XM_CALLCONV Draw(PrimitiveBatch<VertexPositionColor>* batch,
    const BoundingFrustum& frustum,
    FXMVECTOR color)
{
    XMFLOAT3 corners[BoundingFrustum::CORNER_COUNT];
    frustum.GetCorners(corners);

    VertexPositionColor verts[24] = {};
    verts[0].position = corners[0];
    verts[1].position = corners[1];
    verts[2].position = corners[1];
    verts[3].position = corners[2];
    verts[4].position = corners[2];
    verts[5].position = corners[3];
    verts[6].position = corners[3];
    verts[7].position = corners[0];

    verts[8].position = corners[0];
    verts[9].position = corners[4];
    verts[10].position = corners[1];
    verts[11].position = corners[5];
    verts[12].position = corners[2];
    verts[13].position = corners[6];
    verts[14].position = corners[3];
    verts[15].position = corners[7];

    verts[16].position = corners[4];
    verts[17].position = corners[5];
    verts[18].position = corners[5];
    verts[19].position = corners[6];
    verts[20].position = corners[6];
    verts[21].position = corners[7];
    verts[22].position = corners[7];
    verts[23].position = corners[4];

    for (size_t j = 0; j < std::size(verts); ++j)
    {
        XMStoreFloat4(&verts[j].color, color);
    }

    batch->Draw(D3D_PRIMITIVE_TOPOLOGY_LINELIST, verts, static_cast<UINT>(std::size(verts)));
}

void XM_CALLCONV DrawGrid(PrimitiveBatch<VertexPositionColor>* batch,
    FXMVECTOR xAxis,
    FXMVECTOR yAxis,
    FXMVECTOR origin,
    size_t xdivs,
    size_t ydivs,
    GXMVECTOR color)
{
    xdivs = std::max<size_t>(1, xdivs);
    ydivs = std::max<size_t>(1, ydivs);

    for (size_t i = 0; i <= xdivs; ++i)
    {
        float percent = float(i) / float(xdivs);
        percent = (percent * 2.f) - 1.f;
        XMVECTOR scale = XMVectorScale(xAxis, percent);
        scale = XMVectorAdd(scale, origin);

        VertexPositionColor v1(XMVectorSubtract(scale, yAxis), color);
        VertexPositionColor v2(XMVectorAdd(scale, yAxis), color);
        batch->DrawLine(v1, v2);
    }

    for (size_t i = 0; i <= ydivs; i++)
    {
        FLOAT percent = float(i) / float(ydivs);
        percent = (percent * 2.f) - 1.f;
        XMVECTOR scale = XMVectorScale(yAxis, percent);
        scale = XMVectorAdd(scale, origin);

        VertexPositionColor v1(XMVectorSubtract(scale, xAxis), color);
        VertexPositionColor v2(XMVectorAdd(scale, xAxis), color);
        batch->DrawLine(v1, v2);
    }
}

void XM_CALLCONV DrawRing(PrimitiveBatch<VertexPositionColor>* batch,
    FXMVECTOR origin,
    FXMVECTOR majorAxis,
    FXMVECTOR minorAxis,
    GXMVECTOR color)
{
    static const size_t c_ringSegments = 32;

    VertexPositionColor verts[c_ringSegments + 1];

    FLOAT fAngleDelta = XM_2PI / float(c_ringSegments);
    // Instead of calling cos/sin for each segment we calculate
    // the sign of the angle delta and then incrementally calculate sin
    // and cosine from then on.
    XMVECTOR cosDelta = XMVectorReplicate(cosf(fAngleDelta));
    XMVECTOR sinDelta = XMVectorReplicate(sinf(fAngleDelta));
    XMVECTOR incrementalSin = XMVectorZero();
    static const XMVECTORF32 s_initialCos =
    {
        1.f, 1.f, 1.f, 1.f
    };
    XMVECTOR incrementalCos = s_initialCos.v;
    for (size_t i = 0; i < c_ringSegments; i++)
    {
        XMVECTOR pos = XMVectorMultiplyAdd(majorAxis, incrementalCos, origin);
        pos = XMVectorMultiplyAdd(minorAxis, incrementalSin, pos);
        XMStoreFloat3(&verts[i].position, pos);
        XMStoreFloat4(&verts[i].color, color);
        // Standard formula to rotate a vector.
        XMVECTOR newCos = incrementalCos * cosDelta - incrementalSin * sinDelta;
        XMVECTOR newSin = incrementalCos * sinDelta + incrementalSin * cosDelta;
        incrementalCos = newCos;
        incrementalSin = newSin;
    }
    verts[c_ringSegments] = verts[0];

    batch->Draw(D3D_PRIMITIVE_TOPOLOGY_LINESTRIP, verts, c_ringSegments + 1);
}

void XM_CALLCONV DrawRay(PrimitiveBatch<VertexPositionColor>* batch,
    FXMVECTOR origin,
    FXMVECTOR direction,
    bool normalize,
    FXMVECTOR color)
{
    VertexPositionColor verts[3];
    XMStoreFloat3(&verts[0].position, origin);

    XMVECTOR normDirection = XMVector3Normalize(direction);
    XMVECTOR rayDirection = (normalize) ? normDirection : direction;

    XMVECTOR perpVector = XMVector3Cross(normDirection, g_XMIdentityR1);

    if (XMVector3Equal(XMVector3LengthSq(perpVector), g_XMZero))
    {
        perpVector = XMVector3Cross(normDirection, g_XMIdentityR2);
    }
    perpVector = XMVector3Normalize(perpVector);

    XMStoreFloat3(&verts[1].position, XMVectorAdd(rayDirection, origin));
    perpVector = XMVectorScale(perpVector, 0.0625f);
    normDirection = XMVectorScale(normDirection, -0.25f);
    rayDirection = XMVectorAdd(perpVector, rayDirection);
    rayDirection = XMVectorAdd(normDirection, rayDirection);
    XMStoreFloat3(&verts[2].position, XMVectorAdd(rayDirection, origin));

    XMStoreFloat4(&verts[0].color, color);
    XMStoreFloat4(&verts[1].color, color);
    XMStoreFloat4(&verts[2].color, color);

    batch->Draw(D3D_PRIMITIVE_TOPOLOGY_LINESTRIP, verts, 2);
}

void XM_CALLCONV DrawTriangle(PrimitiveBatch<VertexPositionColor>* batch,
    FXMVECTOR pointA,
    FXMVECTOR pointB,
    FXMVECTOR pointC,
    GXMVECTOR color)
{
    VertexPositionColor verts[4];
    XMStoreFloat3(&verts[0].position, pointA);
    XMStoreFloat3(&verts[1].position, pointB);
    XMStoreFloat3(&verts[2].position, pointC);
    XMStoreFloat3(&verts[3].position, pointA);

    XMStoreFloat4(&verts[0].color, color);
    XMStoreFloat4(&verts[1].color, color);
    XMStoreFloat4(&verts[2].color, color);

    XMStoreFloat4(&verts[3].color, color);

    batch->Draw(D3D_PRIMITIVE_TOPOLOGY_LINESTRIP, verts, 4);
}

void XM_CALLCONV DrawQuad(PrimitiveBatch<VertexPositionColor>* batch,
    FXMVECTOR pointA,
    FXMVECTOR pointB,
    FXMVECTOR pointC,
    GXMVECTOR pointD,
    HXMVECTOR color)
{
    VertexPositionColor vertexes[5];
    XMStoreFloat3(&vertexes[0].position, pointA);
    XMStoreFloat3(&vertexes[1].position, pointB);
    XMStoreFloat3(&vertexes[2].position, pointC);
    XMStoreFloat3(&vertexes[3].position, pointD);
    XMStoreFloat3(&vertexes[4].position, pointA);

    XMStoreFloat4(&vertexes[0].color, color);
    XMStoreFloat4(&vertexes[1].color, color);
    XMStoreFloat4(&vertexes[2].color, color);
    XMStoreFloat4(&vertexes[3].color, color);
    XMStoreFloat4(&vertexes[4].color, color);

    batch->Draw(D3D_PRIMITIVE_TOPOLOGY_LINESTRIP, vertexes, 5);
}

void XM_CALLCONV DrawLine(PrimitiveBatch<VertexPositionColor>* batch, FXMVECTOR pointA, FXMVECTOR pointB, FXMVECTOR color)
{
    VertexPositionColor vertexes[2];
    XMStoreFloat3(&vertexes[0].position, pointA);
    XMStoreFloat3(&vertexes[1].position, pointB);

    XMStoreFloat4(&vertexes[0].color, color);
    XMStoreFloat4(&vertexes[1].color, color);

    batch->Draw(D3D_PRIMITIVE_TOPOLOGY_LINESTRIP, vertexes, 2);
}

void XM_CALLCONV DrawSpotLight(DirectX::PrimitiveBatch<DirectX::VertexPositionColor>* batch,
                               DirectX::FXMVECTOR position, DirectX::FXMVECTOR direction, float range,
                               float innerCone, float outerCone, size_t numSegments, DirectX::GXMVECTOR color)
{
    XMVECTOR dir = XMVector3Normalize(direction);

    // 축 계산 (기준 평면)
    XMVECTOR up = XMVectorSet(0, 1, 0, 0);
    if (fabsf(XMVectorGetX(XMVector3Dot(up, dir))) > 0.99f)
        up = XMVectorSet(1, 0, 0, 0);
    XMVECTOR right = XMVector3Normalize(XMVector3Cross(up, dir));
    XMVECTOR front = XMVector3Normalize(XMVector3Cross(dir, right));

    XMVECTOR coneTip = position;

    // Outer 원뿔
    float    outerRadius = tanf(outerCone) * range;
    XMVECTOR outerBase   = XMVectorAdd(coneTip, XMVectorScale(dir, range));

    // Inner 원뿔
    float    innerRadius = tanf(innerCone) * range;
    XMVECTOR innerBase   = XMVectorAdd(coneTip, XMVectorScale(dir, range * 0.95f)); // 살짝 안쪽

    std::vector<VertexPositionColor> outerRing;
    std::vector<VertexPositionColor> innerRing;

    // 감쇠 시각화: 색상 밝기 다르게
    FXMVECTOR outerColor = XMVectorScale(color, 0.4f); // 바깥 원뿔 연하게
    FXMVECTOR innerColor = color;                      // 중심 원뿔 진하게

    for (size_t i = 0; i <= numSegments; ++i)
    {
        float angle = XM_2PI * float(i) / float(numSegments);
        float x = cosf(angle), y = sinf(angle);

        XMVECTOR offset = XMVectorAdd(XMVectorScale(right, x), XMVectorScale(front, y));

        // Outer
        XMVECTOR outerPoint = XMVectorAdd(outerBase, XMVectorScale(offset, outerRadius));
        if (0 == i % 6)
        {
            VertexPositionColor line1[]{{coneTip, outerColor}, {outerPoint, outerColor}};
            batch->Draw(D3D_PRIMITIVE_TOPOLOGY_LINELIST, line1, 2);
        }
        outerRing.emplace_back(outerPoint, outerColor);

        // Inner
        XMVECTOR innerPoint = XMVectorAdd(innerBase, XMVectorScale(offset, innerRadius));
        if (0 == i % 6)
        {
            VertexPositionColor line2[]{{coneTip, innerColor}, {innerPoint, innerColor}};
            batch->Draw(D3D_PRIMITIVE_TOPOLOGY_LINELIST, line2, 2);
        }

        innerRing.emplace_back(innerPoint, innerColor);
    }

    // 바깥 원둘레 라인
    for (size_t i = 0; i < outerRing.size() - 1; ++i)
    {
        VertexPositionColor v[] = {outerRing[i], outerRing[i + 1]};
        batch->Draw(D3D_PRIMITIVE_TOPOLOGY_LINELIST, v, 2);
    }

    // 안쪽 원둘레 라인
    for (size_t i = 0; i < innerRing.size() - 1; ++i)
    {
        VertexPositionColor v[] = {innerRing[i], innerRing[i + 1]};
        batch->Draw(D3D_PRIMITIVE_TOPOLOGY_LINELIST, v, 2);
    }

    // 빛 방향 화살표 (방향 강조용)
    XMVECTOR            arrowEnd    = XMVectorAdd(coneTip, XMVectorScale(dir, range * 1.1f));
    VertexPositionColor arrowLine[] = {{coneTip, XMVectorSet(1, 1, 0, 1)}, // 노란색 화살표
                                       {arrowEnd, XMVectorSet(1, 1, 0, 1)}};
    batch->Draw(D3D_PRIMITIVE_TOPOLOGY_LINELIST, arrowLine, 2);
}

void XM_CALLCONV DrawDebugGrid(DirectX::PrimitiveBatch<DirectX::VertexPositionColor>* batch, DirectX::FXMVECTOR cameraPosition, float farZ, size_t linesPerSide, DirectX::GXMVECTOR color)
{
    float height = fabsf(XMVectorGetY(cameraPosition));
    height       = max(height, 10.f);

    float mainSpacing = powf(10.f, roundf(log10f(height)));
    float subSpacing  = mainSpacing * 0.1f;

    /*XMVECTOR gridOrigin = XMVectorSet(roundf(XMVectorGetX(cameraPosition) / mainSpacing) * mainSpacing, 0.f,
                                      roundf(XMVectorGetZ(cameraPosition) / mainSpacing) * mainSpacing, 1.f);*/

    float gridSnapX = fmodf(XMVectorGetX(cameraPosition), mainSpacing);
    float gridSnapZ = fmodf(XMVectorGetZ(cameraPosition), mainSpacing);

    XMVECTOR gridOrigin = XMVectorSet(XMVectorGetX(cameraPosition) - gridSnapX, 0.f, XMVectorGetZ(cameraPosition) - gridSnapZ, 1.f);

    auto drawLines = [&](float spacing, int count, XMVECTOR baseColor, bool isMajor) {
        for (int i = -count; i <= count; ++i)
        {
            float offset = float(i) * spacing;

            if (!isMajor && fmodf(offset, mainSpacing) == 0.f)
                continue;

            XMVECTOR fromX = gridOrigin + XMVectorSet(offset, 0, -farZ, 0);
            XMVECTOR toX   = gridOrigin + XMVectorSet(offset, 0, +farZ, 0);
            XMVECTOR fromZ = gridOrigin + XMVectorSet(-farZ, 0, offset, 0);
            XMVECTOR toZ   = gridOrigin + XMVectorSet(+farZ, 0, offset, 0);

            // 중간점과 카메라 거리 계산 (감쇄)
            XMVECTOR midX = XMVectorLerp(fromX, toX, 0.5f);
            XMVECTOR midZ = XMVectorLerp(fromZ, toZ, 0.5f);

            float distX = XMVectorGetX(XMVector3Length(midX - cameraPosition));
            float distZ = XMVectorGetX(XMVector3Length(midZ - cameraPosition));
            
            float fadeX = 1.0f - std::clamp((distX - 5.f) / 20.f, 0.0f, 1.0f);
            float fadeZ = 1.0f - std::clamp((distZ - 5.f) / 20.f, 0.0f, 1.0f);            

            XMVECTOR fadedColorX = XMVectorSetW(baseColor, 0.f);
            XMVECTOR fadedColorZ = XMVectorSetW(baseColor, 0.f);

            // 중심축 제외
            if (fromX.m128_f32[0] != 0.f)
            {
                VertexPositionColor vx[] = {{fromX, fadedColorX}, {toX, fadedColorX}};
                batch->Draw(D3D_PRIMITIVE_TOPOLOGY_LINELIST, vx, 2);
            }
            if (fromZ.m128_f32[2] != 0.f)
            {
                VertexPositionColor vz[] = {{fromZ, fadedColorZ}, {toZ, fadedColorZ}};
                batch->Draw(D3D_PRIMITIVE_TOPOLOGY_LINELIST, vz, 2);
            }
        }
    };

    // 서브 라인    
    drawLines(subSpacing, 1 == subSpacing ? int(linesPerSide) * 100 : int(linesPerSide) * 10, XMVectorSet(0.3f, 0.3f, 0.3f, 1.f), false);

    // 메인 라인
    drawLines(mainSpacing, int(linesPerSide), XMVectorSet(1.f, 1.f, 1.f, 1.f), true);
    
    VertexPositionColor xAxis[] = {{XMVectorSet(-farZ, 0, 0, 1), Colors::Red},
                                   {XMVectorSet(farZ, 0, 0, 1), Colors::Red}};
    VertexPositionColor zAxis[] = {{XMVectorSet(0, 0, -farZ, 1), Colors::Blue},
                                   {XMVectorSet(0, 0, farZ, 1), Colors::Blue}};

    batch->Draw(D3D_PRIMITIVE_TOPOLOGY_LINELIST, xAxis, 2);
    batch->Draw(D3D_PRIMITIVE_TOPOLOGY_LINELIST, zAxis, 2);
}

void XM_CALLCONV DrawCircle(DirectX::PrimitiveBatch<DirectX::VertexPositionColor>* batch, DirectX::FXMVECTOR origin, float radius, DirectX::FXMVECTOR color)
{
    static const size_t c_circleSegments = 32;
    VertexPositionColor verts[c_circleSegments + 1];
    FLOAT fAngleDelta = XM_2PI / float(c_circleSegments);
    // Instead of calling cos/sin for each segment we calculate
    // the sign of the angle delta and then incrementally calculate sin
    // and cosine from then on.
    XMVECTOR cosDelta = XMVectorReplicate(cosf(fAngleDelta));
    XMVECTOR sinDelta = XMVectorReplicate(sinf(fAngleDelta));
    XMVECTOR incrementalSin = XMVectorZero();
    static const XMVECTORF32 s_initialCos =
    {
        1.f, 1.f, 1.f, 1.f
    };
    XMVECTOR incrementalCos = s_initialCos.v;
    for (size_t i = 0; i < c_circleSegments; i++)
    {
        XMVECTOR pos = XMVectorMultiplyAdd(g_XMIdentityR0, XMVectorScale(incrementalCos, radius), origin);
        pos = XMVectorMultiplyAdd(g_XMIdentityR1, XMVectorScale(incrementalSin, radius), pos);
        XMStoreFloat3(&verts[i].position, pos);
        XMStoreFloat4(&verts[i].color, color);
        // Standard formula to rotate a vector.
        XMVECTOR newCos = incrementalCos * cosDelta - incrementalSin * sinDelta;
        XMVECTOR newSin = incrementalCos * sinDelta + incrementalSin * cosDelta;
        incrementalCos = newCos;
        incrementalSin = newSin;
    }
    verts[c_circleSegments] = verts[0];
    batch->Draw(D3D_PRIMITIVE_TOPOLOGY_LINESTRIP, verts, c_circleSegments + 1);
}