#pragma once

class GeometryGenerator
{
public:
    using uint16 = uint16_t;
    using uint32 = uint32_t;
    struct Vertex
    {
        Vertex() {}

        Vertex(const XMFLOAT4 pos, const XMFLOAT3 norm, const XMFLOAT3 tan,
            const XMFLOAT2 uv)
            : position{ pos }, normal{ norm }, tangent{ tan }, texcoord{ uv }
        {
        }

        Vertex(float px, float py, float pz, float nx, float ny, float nz,
            float tx, float ty, float tz, float u, float v)
            : position{ px, py, pz, 1.0f }, normal{ nx, ny, nz }, tangent{ tx, ty, tz },
            texcoord{ u, v }
        {
        }

        Vector4 position;
        Vector3 normal;
        Vector3 tangent;
        Vector3 biTangent = {0.f,0.f,0.f};
        Vector2 texcoord;
    };

    class MeshData
    {
    public:
        std::vector<Vertex>  vertices;
        std::vector<uint32>  indices32;
        std::vector<uint16>& GetIndices16()
        {
            if (_indices16.empty())
            {
                _indices16.resize(indices32.size());
                for (size_t i = 0; i < indices32.size(); ++i)
                {
                    _indices16[i] = static_cast<uint16>(indices32[i]);
                }
            }
            return _indices16;
        }

    private:
        std::vector<uint16> _indices16;
    };

    /**
     * @brief Generates a 3D box (cuboid) mesh.
     *
     * @param width : The width of the box along the X-axis.
     * @param height : The height of the box along the Y-axis.
     * @param depth : The depth of the box along the Z-axis.
     * @param numSubdivisions : The number of times to subdivide the box for
     * smoother geometry.
     * @return The generated box mesh data (MeshData).
     */
    MeshData CreateBox(float width, float height, float depth,
        uint32 numSubdivisions);

    /**
     * @brief Generates a spherical mesh.
     * @param radius : The radius of the sphere.
     * @param sliceCount : The number of vertical slices (longitude divisions).
     * @param stackCount : The number of horizontal stacks (latitude divisions).
     * @return The generated sphere mesh data (MeshData).
     */
    MeshData CreateSphere(float radius, uint32 sliceCount, uint32 stackCount);

    /**
     * @brief Generates a geosphere mesh based on the given radius and
     * subdivision level.
     *
     * A geosphere is typically created by refining an initial octahedron or
     * icosahedron, where triangles are recursively subdivided, and the vertices
     * are normalized to form a more spherical shape.
     *
     * @param radius : The radius of the geosphere.
     * @param numSubdivisions : The number of recursive subdivisions to refine
     * the mesh. Higher values create a smoother sphere but increase vertex
     * count.
     * @return A MeshData object containing the generated geosphere's vertex and
     * index data.
     */
    MeshData CreateGeosphere(float radius, uint32 numSubdivisions);

    /**
     * @brief Generates a cylinder mesh
     * @param bottomRadius : the radius of the bottom base of the cylinder
     * @param topRadius : the radius of the top base of the cylinder
     * @param height : total height of the cylinder
     * @param sliceCount : sliceCount The number of vertical slices around the
     * circumference.
     * @param stackcount : stackCount The number of horizontal divisions along
     * the height.
     * @return Cylinder mesh data(MeshData)
     */
    MeshData CreateCylinder(float bottomRadius, float topRadius, float height,
        uint32 sliceCount, uint32 stackcount);

    /**
     * @brief Generates a grid mesh with the specified width, depth, and
     * subdivisions.
     *
     * @param width The total width of the grid.
     * @param depth The total depth of the grid.
     * @param m The number of horizontal subdivisions (rows).
     * @param n The number of vertical subdivisions (columns).
     * @return A MeshData object representing the generated grid.
     */
    MeshData CreateGrid(float width, float depth, uint32 m, uint32 n);

    /**
     * @brief Creates a quad (rectangle) mesh with the specified position, size,
     * and depth.
     *
     * @param x The x-coordinate of the quad's center.
     * @param y The y-coordinate of the quad's center.
     * @param w The width of the quad.
     * @param h The height of the quad.
     * @param depth The depth (z-coordinate) of the quad.
     * @return A MeshData object representing the generated quad.
     */
    MeshData CreateQuad(float x, float y, float w, float h, float depth);


private:
    void   Subdivide(MeshData& meshData);
    Vertex MidPoint(const Vertex& v0, const Vertex& v1);
    void BuildCylinderTopCap(float bottomRadius, float topRadius, float height,
        uint32 sliceCount, uint32 stackCount,
        MeshData& meshData);
    void BuildCylinderBottomCap(float bottomRadius, float topRadius,
        float height, uint32 sliceCount,
        uint32 stackCount, MeshData& meshData);
};