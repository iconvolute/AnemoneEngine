#pragma once
#include <cstddef>
#include <cstdint>
#include <vector>

namespace Anemone::Geometry
{
    struct FaceIndex
    {
        size_t Inner;
    };

    struct VertexIndex
    {
        size_t Inner;
    };

    struct WedgeIndex
    {
        size_t Inner;
    };

    struct CornerIndex
    {
        size_t Inner;
    };

    /// - all faces are triangles
    /// - mesh face has N corners (N=3 for triangles).
    /// - wedge all attributes of each corner of face
    ///   - wedge_id = face_id * corners + corner_id
    /// - vertex - vertex position shared by wedges
    class GEOMETRY_API Mesh final
    {
    public:
        static constexpr size_t MaxTextureCoords = 8;
        static constexpr size_t FacePrimitiveCornersCount = 3;

        // FaceMaterialIndex[face_index] = material_index.
        std::vector<int32_t> FaceMaterialIndices;

        // FaceSmoothingMasks[face_index] = smooth_mask.
        std::vector<uint32_t> FaceSmoothingMasks;

        // VertexPositions[vertex_index] = Float3(position)
        std::vector<Float3> VertexPositions;

        // WedgeIndices[wedge_index] = vertex_index
        std::vector<uint32_t> WedgeIndices;

        // WedgeTangentX[wedge_index] = tangent_x
        std::vector<Float3> WedgeTangentX;

        // WedgeTangentY[wedge_index] = tangent_y
        std::vector<Float3> WedgeTangentY;

        // WedgeTangentZ[wedge_index] = tangent_z
        std::vector<Float3> WedgeTangentZ;

        // WedgeTextureCoords[uv_layer][wedge_index] = texcoords.
        std::vector<Float2> WedgeTextureCoords[MaxTextureCoords];

        // WedgeColors[wedge_index] = color.
        std::vector<Float4> WedgeColors;

    public:
        constexpr size_t GetVerticesCount() const noexcept
        {
            return static_cast<uint32_t>(this->VertexPositions.size());
        }

        constexpr size_t GetWedgesCount() const
        {
            return this->WedgeIndices.size();
        }

        constexpr size_t GetFacesCount() const
        {
            return this->WedgeIndices.size() / FacePrimitiveCornersCount;
        }

        static constexpr WedgeIndex ComputeWedgeIndex(FaceIndex face, CornerIndex corner)
        {
            return WedgeIndex{face.Inner * FacePrimitiveCornersCount + corner.Inner};
        }

        static constexpr FaceIndex ComputeFaceIndex(CornerIndex corner_index)
        {
            return FaceIndex{corner_index.Inner / FacePrimitiveCornersCount};
        }
    };
}

namespace Anemone::Geometry::EditableMesh
{
    struct VertexId final
    {
        uint32_t Inner;

        static constexpr VertexId Invalid() { return {~static_cast<uint32_t>(0)}; }
    };

    struct VertexInstanceId final
    {
        uint32_t Inner;

        static constexpr VertexInstanceId Invalid() { return {~static_cast<uint32_t>(0)}; }
    };

    struct EdgeId final
    {
        uint32_t Inner;

        static constexpr EdgeId Invalid() { return {~static_cast<uint32_t>(0)}; }
    };

    struct PolygonGroupId final
    {
        uint32_t Inner;

        static constexpr PolygonGroupId Invalid() { return {~static_cast<uint32_t>(0)}; }
    };

    struct PolygonId final
    {
        uint32_t Inner;

        static constexpr PolygonId Invalid() { return {~static_cast<uint32_t>(0)}; }
    };

    struct MeshVertex
    {
        std::vector<VertexInstanceId> Instances;
        std::vector<EdgeId> ConnectedEdges;
    };

    struct MeshVertexInstance
    {
        VertexId Vertex;
        std::vector<PolygonId> ConnectedPolygons;
    };

    struct MeshEdge
    {
        VertexId Vertices[2];
        std::vector<PolygonId> ConnectedPolygons;
    };

    struct MeshPolygonContour
    {
        std::vector<VertexInstanceId> Instances;
    };

    struct MeshTriangle
    {
        VertexInstanceId Vertex1;
        VertexInstanceId Vertex2;
        VertexInstanceId Vertex3;
    };

    struct MeshPolygon
    {
        MeshPolygonContour Perimeter;
        std::vector<MeshPolygonContour> Holes;
        std::vector<MeshTriangle> Triangles;
        PolygonGroupId GroupId;
    };

    struct MeshPolygonGroup
    {
        std::vector<PolygonId> Polygons;
    };

    struct MeshDescriptor
    {
        std::vector<MeshVertex> Vertices;
        std::vector<MeshVertexInstance> VertexInstancesCollection;
        std::vector<MeshEdge> EdgesCollection;
        std::vector<MeshPolygon> PolygonsCollection;
        std::vector<MeshPolygonGroup> PolygonGroupsCollection;
    };
}

namespace Anemone::Geometry::v2
{
    struct EdgeId final
    {
        uint32_t Inner;

        static constexpr EdgeId Invalid() { return {~static_cast<uint32_t>(0)}; }

        [[nodiscard]] friend constexpr auto operator<=>(EdgeId const& left, EdgeId const& right) = default;
    };


    struct VertexId final
    {
        uint32_t Inner;

        static constexpr VertexId Invalid() { return {~static_cast<uint32_t>(0)}; }

        [[nodiscard]] friend constexpr auto operator<=>(VertexId const& left, VertexId const& right) = default;
    };

    // Half-edge representation.
    struct Edge final
    {
        // Index of vertex at the start of the edge.
        VertexId Vertex;

        // Index of the next edge. Forms a circular list of all edges going out of the vertex.
        EdgeId NextOutgoing;

        // Index of the edge going in the opposite direction. `Next->Vertex == this->Vertex`. Forms full edge.
        EdgeId Twin;

        // Index of the next edge in the polygon.
        EdgeId Next;
    };

    struct Triangle final
    {
        VertexId Vertices[3];
    };

    struct Mesh final
    {
        std::vector<Edge> Edges{};

        Triangle GetTriangle(EdgeId edge) const
        {
            EdgeId const& e1 = this->Edges[edge.Inner];
            EdgeId const& e2 = this->Edges[e1.Next.Inner];

            VertexId v0 = edge.Vertex;
            VertexId v1 = e1.Vertex;
            VertexId v2 = e2.Vertex;

            return Triangle{
                .Vertices = {v0, v1, v2},
            };
        }
    };
}

namespace Anemone::Geometry
{
    template <typename Vertex>
    Vertex Interpolate(Vertex const& v0, Vertex const& v1, float t) = delete;

    template <typename Vertex, typename Index>
    void Subdivide(
        std::vector<Vertex>& outVertices,
        std::vector<Index>& outIndices,
        std::span<Vertex const> inVertices,
        std::span<Index const> inIndices)
    {
        size_t const trianglesCount = inIndices.size() / 3;

        // Subdivide each triangle into 4 smaller triangles.
        //
        //                v1
        //                *
        //               /|
        //              / |
        //             /  |
        //            /   |
        //           /    |
        //       s0 *-----* s1
        //         /|    /|
        //        / |   / |
        //       /  |  /  |
        //      /   | /   |
        //     /    |/    |
        // v0 *-----*-----* v2
        //          s2

        for (size_t i = 0; i < trianglesCount; ++i)
        {
            Vertex const& v0 = inVertices[inIndices[i * 3 + 0]];
            Vertex const& v1 = inVertices[inIndices[i * 3 + 1]];
            Vertex const& v2 = inVertices[inIndices[i * 3 + 2]];

            Vertex const s0 = Interpolate(v0, v1, 0.5f);
            Vertex const s1 = Interpolate(v1, v2, 0.5f);
            Vertex const s2 = Interpolate(v0, v2, 0.5f);

            outVertices.emplace_back(v0);
            outVertices.emplace_back(v1);
            outVertices.emplace_back(v2);

            outVertices.emplace_back(s0);
            outVertices.emplace_back(s1);
            outVertices.emplace_back(s2);

            // triangle: v0, s0, s2
            outIndices.emplace_back(i * 6 + 0);
            outIndices.emplace_back(i * 6 + 3);
            outIndices.emplace_back(i * 6 + 5);

            // triangle: s0, v1, s1
            outIndices.emplace_back(i * 6 + 3);
            outIndices.emplace_back(i * 6 + 1);
            outIndices.emplace_back(i * 6 + 4);

            // triangle: s2, s1, v2
            outIndices.emplace_back(i * 6 + 5);
            outIndices.emplace_back(i * 6 + 4);
            outIndices.emplace_back(i * 6 + 2);

            // triangle: s0, s1, s2
            outIndices.emplace_back(i * 6 + 3);
            outIndices.emplace_back(i * 6 + 4);
            outIndices.emplace_back(i * 6 + 5);
        }
    }
}
