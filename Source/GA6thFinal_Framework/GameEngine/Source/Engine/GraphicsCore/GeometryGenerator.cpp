#include "pch.h"
#include "GeometryGenerator.h"

GeometryGenerator::MeshData GeometryGenerator::CreateBox(float width,
    float height,
    float depth,
    uint32 numSubdivisions)
{

    MeshData meshData;

    //
    // Create the vertices.
    //

    Vertex v[24];

    float w2 = 0.5f * width;
    float h2 = 0.5f * height;
    float d2 = 0.5f * depth;

    // Fill in the front face vertex data.
    v[0] =
        Vertex(-w2, -h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
    v[1] =
        Vertex(-w2, +h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
    v[2] =
        Vertex(+w2, +h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
    v[3] =
        Vertex(+w2, -h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f);

    // Fill in the back face vertex data.
    v[4] =
        Vertex(-w2, -h2, +d2, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f);
    v[5] =
        Vertex(+w2, -h2, +d2, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
    v[6] =
        Vertex(+w2, +h2, +d2, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
    v[7] =
        Vertex(-w2, +h2, +d2, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f);

    // Fill in the top face vertex data.
    v[8] =
        Vertex(-w2, +h2, -d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
    v[9] =
        Vertex(-w2, +h2, +d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
    v[10] =
        Vertex(+w2, +h2, +d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
    v[11] =
        Vertex(+w2, +h2, -d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f);

    // Fill in the bottom face vertex data.
    v[12] =
        Vertex(-w2, -h2, -d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f);
    v[13] =
        Vertex(+w2, -h2, -d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
    v[14] =
        Vertex(+w2, -h2, +d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
    v[15] =
        Vertex(-w2, -h2, +d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f);

    // Fill in the left face vertex data.
    v[16] =
        Vertex(-w2, -h2, +d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
    v[17] =
        Vertex(-w2, +h2, +d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
    v[18] =
        Vertex(-w2, +h2, -d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
    v[19] =
        Vertex(-w2, -h2, -d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f);

    // Fill in the right face vertex data.
    v[20] =
        Vertex(+w2, -h2, -d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f);
    v[21] =
        Vertex(+w2, +h2, -d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
    v[22] =
        Vertex(+w2, +h2, +d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f);
    v[23] =
        Vertex(+w2, -h2, +d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);

    meshData.vertices.assign(&v[0], &v[24]);
    
    for (int i = 0; i < 24; i++)
    {
        v[i].biTangent = XMVector3Cross(v[i].normal, v[i].tangent);
    }
    //
    // Create the indices.
    //

    uint32 i[36];

    i[0] = 0;
    i[1] = 1;
    i[2] = 2;
    i[3] = 0;
    i[4] = 2;
    i[5] = 3;

    i[6] = 4;
    i[7] = 5;
    i[8] = 6;
    i[9] = 4;
    i[10] = 6;
    i[11] = 7;

    i[12] = 8;
    i[13] = 9;
    i[14] = 10;
    i[15] = 8;
    i[16] = 10;
    i[17] = 11;

    i[18] = 12;
    i[19] = 13;
    i[20] = 14;
    i[21] = 12;
    i[22] = 14;
    i[23] = 15;

    i[24] = 16;
    i[25] = 17;
    i[26] = 18;
    i[27] = 16;
    i[28] = 18;
    i[29] = 19;

    i[30] = 20;
    i[31] = 21;
    i[32] = 22;
    i[33] = 20;
    i[34] = 22;
    i[35] = 23;

    meshData.indices32.assign(&i[0], &i[36]);

    numSubdivisions = std::min<uint32>(numSubdivisions, 6u);

    for (uint32 i = 0; i < numSubdivisions; ++i)
        Subdivide(meshData);

    return meshData;
}

GeometryGenerator::MeshData GeometryGenerator::CreateSphere(float radius,
    uint32 sliceCount,
    uint32 stackCount)
{
    MeshData meshData;

    Vertex topVertex(0.0f, +radius, 0.0f, 0.0f, +1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f);
    topVertex.position.w = 1.0f;
    Vertex bottomVertex(0.0f, -radius, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f);
    bottomVertex.position.w = 1.0f;
    meshData.vertices.push_back(topVertex);

    float phiStep = XM_PI / stackCount;
    float thetaStep = 2.0f * XM_PI / sliceCount;

    for (uint32 i = 1; i <= stackCount - 1; ++i)
    {
        float phi = i * phiStep;

        for (uint32 j = 0; j <= sliceCount; ++j)
        {
            float theta = j * thetaStep;

            Vertex v;

            v.position.x = radius * sinf(phi) * cosf(theta);
            v.position.y = radius * cosf(phi);
            v.position.z = radius * sinf(phi) * sinf(theta);
            v.position.w = 1.f;
            v.tangent.x = -radius * sinf(phi) * sinf(theta);
            v.tangent.y = 0.0f;
            v.tangent.z = +radius * sinf(phi) * cosf(theta);
            
            XMVECTOR T = XMLoadFloat3(&v.tangent);
            XMStoreFloat3(&v.tangent, XMVector3Normalize(T));

            XMVECTOR p = XMLoadFloat4(&v.position);
            XMStoreFloat3(&v.normal, XMVector3Normalize(p));

            v.texcoord.x = theta / XM_2PI;
            v.texcoord.y = phi / XM_PI;

            v.biTangent = XMVector3Cross(v.normal, v.tangent);

            meshData.vertices.push_back(v);
        }
    }

    meshData.vertices.push_back(bottomVertex);

    for (uint32 i = 1; i <= sliceCount; ++i)
    {
        meshData.indices32.push_back(0);
        meshData.indices32.push_back(i + 1);
        meshData.indices32.push_back(i);
    }

    uint32 baseIndex = 1;
    uint32 ringVertexCount = sliceCount + 1;
    for (uint32 i = 0; i < stackCount - 2; ++i)
    {
        for (uint32 j = 0; j < sliceCount; ++j)
        {
            meshData.indices32.push_back(baseIndex + i * ringVertexCount + j);
            meshData.indices32.push_back(baseIndex + i * ringVertexCount + j +
                1);
            meshData.indices32.push_back(baseIndex + (i + 1) * ringVertexCount +
                j);

            meshData.indices32.push_back(baseIndex + (i + 1) * ringVertexCount +
                j);
            meshData.indices32.push_back(baseIndex + i * ringVertexCount + j +
                1);
            meshData.indices32.push_back(baseIndex + (i + 1) * ringVertexCount +
                j + 1);
        }
    }

    uint32 southPoleIndex = (uint32)meshData.vertices.size() - 1;

    baseIndex = southPoleIndex - ringVertexCount;

    for (uint32 i = 0; i < sliceCount; ++i)
    {
        meshData.indices32.push_back(southPoleIndex);
        meshData.indices32.push_back(baseIndex + i);
        meshData.indices32.push_back(baseIndex + i + 1);
    }

    return meshData;
}

GeometryGenerator::MeshData GeometryGenerator::CreateGeosphere(
    float radius, uint32 numSubdivisions)
{
    MeshData meshData;

    numSubdivisions = std::min<uint32>(numSubdivisions, 6u);

    const float X = 0.525731f;
    const float Z = 0.850651f;

    XMFLOAT4 pos[12] = {
        XMFLOAT4(-X, 0.0f, Z, 1.f),  XMFLOAT4(X, 0.0f, Z, 1.f),   XMFLOAT4(-X, 0.0f, -Z, 1.f),
        XMFLOAT4(X, 0.0f, -Z, 1.f),  XMFLOAT4(0.0f, Z, X, 1.f),   XMFLOAT4(0.0f, Z, -X, 1.f),
        XMFLOAT4(0.0f, -Z, X, 1.f),  XMFLOAT4(0.0f, -Z, -X, 1.f), XMFLOAT4(Z, X, 0.0f, 1.f),
        XMFLOAT4(-Z, X, 0.0f, 1.f),  XMFLOAT4(Z, -X, 0.0f, 1.f),  XMFLOAT4(-Z, -X, 0.0f, 1.f)
    };

    uint32 k[60] = {
        1,  4,  0,  4,  9,  0,  4,  5,  9,  8,  5,  4,  1,  8,  4,
        1, 10,  8, 10,  3,  8,  8,  3,  5,  3,  2,  5,  3,  7,  2,
        3, 10,  7, 10,  6,  7,  6, 11,  7,  6,  0, 11,  6,  1,  0,
        10, 1,  6, 11,  0,  9,  2, 11,  9,  5,  2,  9, 11,  2,  7
    };

    meshData.vertices.resize(12);
    meshData.indices32.assign(&k[0], &k[60]);

    for (uint32 i = 0; i < 12; ++i)
        meshData.vertices[i].position = pos[i];

    for (uint32 i = 0; i < numSubdivisions; ++i)
        Subdivide(meshData);

    // Project vertices onto sphere and scale.
    for (uint32 i = 0; i < meshData.vertices.size(); ++i)
    {
        // Normalize position vector (excluding W)
        XMVECTOR n = XMVector3Normalize(XMLoadFloat4(&meshData.vertices[i].position));

        // Scale by radius
        XMVECTOR p = radius * n;

        // Store position back (with W = 1.0)
        XMFLOAT4 newPos;
        XMStoreFloat4(&newPos, p);
        newPos.w = 1.0f;
        meshData.vertices[i].position = newPos;

        // Store normal (XMFLOAT3)
        XMStoreFloat3(&meshData.vertices[i].normal, n);

        // For texcoords
        float x = meshData.vertices[i].position.x;
        float y = meshData.vertices[i].position.y;
        float z = meshData.vertices[i].position.z;

        float theta = atan2f(z, x);
        if (theta < 0.0f)
            theta += XM_2PI;

        float phi = acosf(y / radius);

        meshData.vertices[i].texcoord.x = theta / XM_2PI;
        meshData.vertices[i].texcoord.y = phi / XM_PI;

        // Partial derivative of position with respect to theta (tangent vector)
        meshData.vertices[i].tangent.x = -radius * sinf(phi) * sinf(theta);
        meshData.vertices[i].tangent.y = 0.0f;
        meshData.vertices[i].tangent.z = +radius * sinf(phi) * cosf(theta);
        meshData.vertices[i].biTangent = XMVector3Cross(meshData.vertices[i].normal, meshData.vertices[i].tangent);
        XMVECTOR T = XMLoadFloat3(&meshData.vertices[i].tangent);
        XMStoreFloat3(&meshData.vertices[i].tangent, XMVector3Normalize(T));
    }

    return meshData;
}


GeometryGenerator::MeshData GeometryGenerator::CreateCylinder(
    float bottomRadius, float topRadius, float height, uint32 sliceCount,
    uint32 stackcount)
{
    MeshData meshData;

    float stackHeight = height / stackcount;

    float radiusStep = (topRadius - bottomRadius) / stackcount;

    uint32 ringCount = stackcount + 1;

    for (uint32 i = 0; i < ringCount; ++i)
    {
        float y = -0.5f * height + i * stackHeight;
        float r = bottomRadius + i * radiusStep;

        float dTheta = 2.0f * XM_PI / sliceCount;
        for (uint32 j = 0; j <= sliceCount; ++j)
        {
            Vertex vertex;

            float c = cosf(j * dTheta);
            float s = sinf(j * dTheta);

            vertex.position = XMFLOAT4(r * c, y, r * s,1.f);

            vertex.texcoord.x = (float)j / sliceCount;
            vertex.texcoord.y = 1.0f - (float)i / stackcount;

            vertex.tangent = XMFLOAT3(-s, 0.0f, c);

            float dr = bottomRadius - topRadius;
            XMFLOAT3 bitangent(dr * c, -height, dr * s);

            XMVECTOR T = XMLoadFloat3(&vertex.tangent);
            XMVECTOR B = XMLoadFloat3(&bitangent);
            XMVECTOR N = XMVector3Normalize(XMVector3Cross(T, B));
            XMStoreFloat3(&vertex.normal, N);
            XMStoreFloat3(&vertex.biTangent, B);
            meshData.vertices.push_back(vertex);
        }
    }

    uint32 ringVertexCount = sliceCount + 1;

    for (uint32 i = 0; i < stackcount; ++i)
    {
        for (uint32 j = 0; j < sliceCount; ++j)
        {
            meshData.indices32.push_back(i * ringVertexCount + j);
            meshData.indices32.push_back((i + 1) * ringVertexCount + j);
            meshData.indices32.push_back((i + 1) * ringVertexCount + j + 1);

            meshData.indices32.push_back(i * ringVertexCount + j);
            meshData.indices32.push_back((i + 1) * ringVertexCount + j + 1);
            meshData.indices32.push_back(i * ringVertexCount + j + 1);
        }
    }

    BuildCylinderTopCap(bottomRadius, topRadius, height, sliceCount, stackcount,
        meshData);
    BuildCylinderBottomCap(bottomRadius, topRadius, height, sliceCount,
        stackcount, meshData);

    return meshData;
}
GeometryGenerator::MeshData GeometryGenerator::CreateGrid(float width, float depth, uint32 m, uint32 n)
{
    MeshData meshData;

    uint32 vertexCount = (m + 1) * (n + 1);
    uint32 faceCount = m * n * 2;

    float halfWidth = 0.5f * width;
    float halfDepth = 0.5f * depth;

    float dx = width / n;
    float dz = depth / m;

    meshData.vertices.resize(vertexCount);
    for (uint32 i = 0; i <= m; ++i)
    {
        float z = halfDepth - i * dz;
        for (uint32 j = 0; j <= n; ++j)
        {
            float x = -halfWidth + j * dx;

            uint32 index = i * (n + 1) + j;

            meshData.vertices[index].position = XMFLOAT4(x, 0.0f, z, 1.f);
            meshData.vertices[index].normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
            meshData.vertices[index].tangent = XMFLOAT3(1.0f, 0.0f, 0.0f);
            meshData.vertices[index].biTangent = XMFLOAT3(0.0f, 0.0f, 1.0f);
            meshData.vertices[index].texcoord.x = static_cast<float>(j);
            meshData.vertices[index].texcoord.y = static_cast<float>(i);
        }
    }

    meshData.indices32.resize(faceCount * 3);
    uint32 k = 0;
    for (uint32 i = 0; i < m; ++i)
    {
        for (uint32 j = 0; j < n; ++j)
        {
            uint32 i0 = i * (n + 1) + j;
            uint32 i1 = i * (n + 1) + (j + 1);
            uint32 i2 = (i + 1) * (n + 1) + j;
            uint32 i3 = (i + 1) * (n + 1) + (j + 1);

            meshData.indices32[k++] = i0;
            meshData.indices32[k++] = i1;
            meshData.indices32[k++] = i2;

            meshData.indices32[k++] = i2;
            meshData.indices32[k++] = i1;
            meshData.indices32[k++] = i3;
        }
    }

    return meshData;
}


GeometryGenerator::MeshData GeometryGenerator::CreateQuad(float x, float y,
    float w, float h,
    float depth)
{
    MeshData meshData;

    meshData.vertices.resize(4);
    meshData.indices32.resize(6);

    // position coordinates specified in NDC space.
    meshData.vertices[0] = Vertex(x, y - h, depth, 0.0f, 0.0f, -1.0f, 1.0f,
        0.0f, 0.0f, 0.0f, 1.0f);

    meshData.vertices[1] =
        Vertex(x, y, depth, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);

    meshData.vertices[2] = Vertex(x + w, y, depth, 0.0f, 0.0f, -1.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 0.0f);

    meshData.vertices[3] = Vertex(x + w, y - h, depth, 0.0f, 0.0f, -1.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 1.0f);

    for (size_t i = 0; i < 4; ++i)
    {
        meshData.vertices[i].biTangent = XMVector3Cross(meshData.vertices[i].normal, meshData.vertices[i].tangent);
    }

    meshData.indices32[0] = 0;
    meshData.indices32[1] = 1;
    meshData.indices32[2] = 2;

    meshData.indices32[3] = 0;
    meshData.indices32[4] = 2;
    meshData.indices32[5] = 3;

    return meshData;
}

void GeometryGenerator::Subdivide(MeshData& meshData)
{
    MeshData inputCopy = meshData;
    meshData.vertices.resize(0);
    meshData.indices32.resize(0);

    uint32 numtris = static_cast<uint32>(inputCopy.indices32.size()) / 3;
    uint32 numTris = (uint32)inputCopy.indices32.size() / 3;
    for (uint32 i = 0; i < numTris; ++i)
    {
        Vertex v0 = inputCopy.vertices[inputCopy.indices32[i * 3 + 0]];
        Vertex v1 = inputCopy.vertices[inputCopy.indices32[i * 3 + 1]];
        Vertex v2 = inputCopy.vertices[inputCopy.indices32[i * 3 + 2]];

        Vertex m0 = MidPoint(v0, v1);
        Vertex m1 = MidPoint(v1, v2);
        Vertex m2 = MidPoint(v0, v2);

        meshData.vertices.push_back(v0); // 0
        meshData.vertices.push_back(v1); // 1
        meshData.vertices.push_back(v2); // 2
        meshData.vertices.push_back(m0); // 3
        meshData.vertices.push_back(m1); // 4
        meshData.vertices.push_back(m2); // 5

        meshData.indices32.push_back(i * 6 + 0);
        meshData.indices32.push_back(i * 6 + 3);
        meshData.indices32.push_back(i * 6 + 5);

        meshData.indices32.push_back(i * 6 + 3);
        meshData.indices32.push_back(i * 6 + 4);
        meshData.indices32.push_back(i * 6 + 5);

        meshData.indices32.push_back(i * 6 + 5);
        meshData.indices32.push_back(i * 6 + 4);
        meshData.indices32.push_back(i * 6 + 2);

        meshData.indices32.push_back(i * 6 + 3);
        meshData.indices32.push_back(i * 6 + 1);
        meshData.indices32.push_back(i * 6 + 4);
    }
}

GeometryGenerator::Vertex GeometryGenerator::MidPoint(const Vertex& v0, const Vertex& v1)
{
    XMVECTOR p0 = XMLoadFloat4(&v0.position);
    XMVECTOR p1 = XMLoadFloat4(&v1.position);

    XMVECTOR n0 = XMLoadFloat3(&v0.normal);
    XMVECTOR n1 = XMLoadFloat3(&v1.normal);

    XMVECTOR tan0 = XMLoadFloat3(&v0.tangent);
    XMVECTOR tan1 = XMLoadFloat3(&v1.tangent);

    XMVECTOR biTan0 = XMLoadFloat3(&v0.biTangent);
    XMVECTOR biTan1 = XMLoadFloat3(&v1.biTangent);

    XMVECTOR tex0 = XMLoadFloat2(&v0.texcoord);
    XMVECTOR tex1 = XMLoadFloat2(&v1.texcoord);

    XMVECTOR pos = 0.5f * (p0 + p1);
    XMVECTOR normal = XMVector3Normalize(0.5f * (n0 + n1));
    XMVECTOR tangent = XMVector3Normalize(0.5f * (tan0 + tan1));
    XMVECTOR tex = 0.5f * (tex0 + tex1);

    Vertex v;
    XMStoreFloat4(&v.position, pos); 
    XMStoreFloat3(&v.normal, normal);
    XMStoreFloat3(&v.tangent, tangent);
    XMStoreFloat2(&v.texcoord, tex);

    return v;
}


void GeometryGenerator::BuildCylinderTopCap(float bottomRadius, float topRadius,
    float height, uint32 sliceCount,
    uint32 stackCount,
    MeshData& meshData)
{
    uint32 baseindex = static_cast<uint32>(meshData.vertices.size());
    float y = 0.5f * height;
    float dTheta = 2.f * XM_PI / sliceCount;
    for (uint32 i = 0; i <= sliceCount; ++i)
    {
        float x = topRadius * cosf(i * dTheta);
        float z = topRadius * sinf(i * dTheta);

        float u = x / height + 0.5f;
        float v = z / height + 0.5f;
        Vertex vertex = Vertex(x, y, z, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, u, v);
        vertex.biTangent = XMVector3Cross(vertex.normal, vertex.tangent);
        meshData.vertices.push_back(vertex);
    }

    meshData.vertices.push_back(
        Vertex(0.0f, y, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.5f, 0.5f));
    uint32 centerIndex = (uint32)meshData.vertices.size() - 1;

    for (uint32 i = 0; i < sliceCount; ++i)
    {
        meshData.indices32.push_back(centerIndex);
        meshData.indices32.push_back(baseindex + i + 1);
        meshData.indices32.push_back(baseindex + i);
    }
}

void GeometryGenerator::BuildCylinderBottomCap(float bottomRadius,
    float topRadius, float height,
    uint32 sliceCount,
    uint32 stackCount,
    MeshData& meshData)
{
    uint32 baseIndex = (uint32)meshData.vertices.size();
    float y = -0.5f * height;
    float dTheta = 2.0f * XM_PI / sliceCount;
    for (uint32 i = 0; i <= sliceCount; ++i)
    {
        float x = bottomRadius * cosf(i * dTheta);
        float z = bottomRadius * sinf(i * dTheta);
        float u = x / height + 0.5f;
        float v = z / height + 0.5f;

        Vertex vertex = Vertex(x, y, z, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, u, v);
        vertex.biTangent = XMVector3Cross(vertex.normal, vertex.tangent);
        meshData.vertices.push_back(vertex);
    }

    meshData.vertices.push_back(
        Vertex(0.0f, y, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.5f, 0.5f));

    uint32 centerIndex = (uint32)meshData.vertices.size() - 1;

    for (uint32 i = 0; i < sliceCount; ++i)
    {
        meshData.indices32.push_back(centerIndex);
        meshData.indices32.push_back(baseIndex + i);
        meshData.indices32.push_back(baseIndex + i + 1);
    }
}

