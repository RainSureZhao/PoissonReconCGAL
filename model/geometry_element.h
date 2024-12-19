//
// Created by RainSure on 24-12-19.
//

#ifndef POISSONRECONCGAL_GEOMETRY_ELEMENT_H
#define POISSONRECONCGAL_GEOMETRY_ELEMENT_H

#include <vector>
#include <unordered_set>
#include <string>

namespace model {
    using Marker = unsigned long long;
    using VolumnLabel = unsigned long long;

    struct Vertex {
        double x, y, z;

        // 重载等于运算符，用于在unordered_map中比较键
        bool operator==(const Vertex& other) const {
            return x == other.x && y == other.y && z == other.z;
        }

        // 重载小于运算符，用于在std::map中比较键
        bool operator<(const Vertex& other) const {
            if (x != other.x) return x < other.x;
            if (y != other.y) return y < other.y;
            return z < other.z;
        }
    };

    struct Edge {
        int start, end;
    };

    struct Face {
        std::vector<int> vertices; // 三个顶点，以counterclockwise顺序给出
        std::vector<int> edges; // 三条边
        std::vector<Vertex> verticesPositions;
        Marker marker;  // 面对应的表面标记
        VolumnLabel leftVolumeNumber, rightVolumeNumber;  // 面左右两侧的体编号 ‐1 if empty (for boundary faces)
        [[nodiscard]] bool isLegal() const {
            if(vertices.size() != 3 or verticesPositions.size() != 3) return false;
            if(vertices[0] == vertices[1] || vertices[1] == vertices[2] || vertices[0] == vertices[2]) return false;
            return true;
        }
    };

    struct Zone {
        int zoneNumber;
        std::vector<int> volumes;
    };

    struct BoundaryRepresentation {
        int osCode;
        std::string modelTag, uniqueId;
        size_t numVertices, numEdges, numFaces, numVolumes, numZones;
        size_t numMarkers, numHorizonMarkers, numFaultMarkers, numBoundarySurfaceMarkers;

        std::vector<Vertex> vertices;
        std::vector<Edge> edges;
        std::vector<Face> faces;
        std::vector<Face> boundaryFaces;
        std::vector<Face> faultFaces;
        std::vector<Face> horizonFaces;
        std::vector<Zone> zones;
        std::unordered_set<Marker> horizonMarkers;
        std::unordered_set<Marker> faultMarkers;
        std::unordered_set<Marker> boundaryMarkers;

        std::unordered_set<VolumnLabel> volumeLabels;
    };

    struct Patch {
        std::vector<Vertex> vertices;
        std::vector<Edge> edges;
        std::vector<Face> faces;
        Marker marker;
    };

    struct PSC {
        std::vector<Vertex> vertices;
        std::vector<Patch> patches;
        std::vector<Edge> featureEdges;
        std::vector<int> featureVertices;
    };
}

#endif //POISSONRECONCGAL_GEOMETRY_ELEMENT_H
