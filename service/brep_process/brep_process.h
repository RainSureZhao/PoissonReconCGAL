//
// Created by ByteDance on 12/19/24.
//

#ifndef BREP_PROCESS_H
#define BREP_PROCESS_H

#include "model/geometry_element.h"
#include <vector>
#include <queue>
#include <unordered_map>
#include <boost/graph/graph_concepts.hpp>
#include <CGAL/Simple_cartesian.h>
#include <CGAL/Surface_mesh.h>

#include <CGAL/boost/graph/generators.h>
#include <CGAL/Polygon_mesh_processing/connected_components.h>
#include <CGAL/boost/graph/Face_filtered_graph.h>
#include <CGAL/property_map.h>

#include <boost/property_map/function_property_map.hpp>
#include <CGAL/boost/graph/alpha_expansion_graphcut.h>

namespace service {
    namespace PMP = CGAL::Polygon_mesh_processing;

    typedef CGAL::Simple_cartesian<double>     Kernel;
    typedef Kernel::Point_3                    Point;
    typedef Kernel::Compare_dihedral_angle_3   Compare_dihedral_angle_3;
    typedef CGAL::Surface_mesh<Point>          Mesh;
    typedef boost::graph_traits<Mesh>::face_descriptor                      face_descriptor;
    typedef boost::graph_traits<Mesh>::vertex_descriptor vertex_descriptor;
    typedef Kernel::FT                                                      FT;
    typedef CGAL::Face_filtered_graph<Mesh>                            Filtered_graph;

    /**
     * @brief 将三角网格生成CGAL::Surface_mesh
     * @param faces
     * @param vertices
     * @return
     */
    Mesh GetMesh(const std::vector<model::Face>& faces, const std::vector<model::Vertex>& vertices) {
        Mesh mesh;

        // 将顶点添加到新的网格中（避免重复）
        std::map<Point, vertex_descriptor> point_to_vertex;

        auto add_vertex = [&](const Point& pt, std::map<Point, vertex_descriptor>& point_to_vertex, Mesh& mesh) -> vertex_descriptor {
            // 查找是否已经存在相同的顶点
            auto it = point_to_vertex.find(pt);
            if (it != point_to_vertex.end()) {
                return it->second;
            }
            vertex_descriptor v = mesh.add_vertex(pt);
            point_to_vertex[pt] = v;
            return v;
        };
        for(const auto& face : faces) {
            std::vector<vertex_descriptor> new_vertices;
            for(const auto v : face.vertices) {
                Point point = {vertices[v].x, vertices[v].y, vertices[v].z};
                new_vertices.push_back(add_vertex(point, point_to_vertex, mesh));
            }
            mesh.add_face(new_vertices);
        }
        return mesh;
    }

    /**
     * @brief 将CGAL::SurfaceMesh转成Faces
     * @param mesh
     * @return
     */
    std::vector<model::Face> GetFacesFromSurfaceMesh(const Mesh& mesh, const model::BoundaryRepresentation& brep) {
        std::vector<model::Face> faces;
        std::map<Point, size_t> point_to_index;
        const auto& vertices = brep.vertices;
        for(size_t i = 0; i < vertices.size(); i ++) {
            Point point = {vertices[i].x, vertices[i].y, vertices[i].z};
            point_to_index[point] = i;
        }
        std::map<std::pair<int, int>, size_t> edge_to_index;
        const auto& edges = brep.edges;
        for(size_t i = 0; i < edges.size(); i ++) {
            const auto& edge = edges[i];
            std::pair<int, int> edge_pair = {edge.start, edge.end};
            edge_to_index[edge_pair] = i;
        }
        for(const auto f : mesh.faces()) {
            std::vector<Point> face_vertices;
            for (const auto v : mesh.vertices_around_face(mesh.halfedge(f))) {
                face_vertices.push_back(mesh.point(v));
            }
            model::Face face;
            face.vertices.clear();
            face.vertices.reserve(face_vertices.size());
            for(const auto & face_vertice : face_vertices) {
                face.vertices.push_back(point_to_index[face_vertice]);
            }
            for(size_t i = 0; i < face.vertices.size(); i ++) {
                size_t next_index = (i + 1) % face.vertices.size();
                size_t edge_index = edge_to_index[{face.vertices[i], face.vertices[next_index]}];
                face.edges.push_back(edge_index);
            }
            faces.push_back(face);
        }
        return faces;
    }

    /**
     * @brief 获取连通块
     * @param faces
     * @param vertices
     * @return
     */
    std::vector<std::vector<model::Face>> GetComponents(const std::vector<model::Face>& faces, const model::BoundaryRepresentation& brep) {
        Mesh surfaceMesh = GetMesh(faces, brep.vertices);

        Mesh::Property_map<face_descriptor, std::size_t> fccmap = surfaceMesh.add_property_map<face_descriptor,
            std::size_t>("f:CC").first;
        std::size_t num = PMP::connected_components(surfaceMesh, fccmap);
        std::vector<Mesh> meshes(num);
        for(size_t i = 0; i < num; ++i) {
            Filtered_graph ffg(surfaceMesh, i, fccmap);
            CGAL::copy_face_graph(ffg, meshes[i]);
        }
        std::vector<std::vector<model::Face>> components(num);
        for(size_t i = 0; i < num; ++i) {
            components[i] = GetFacesFromSurfaceMesh(meshes[i], brep);
        }
        return components;
    }

    std::vector<int> GetFeaturePoints(const  std::unordered_map<model::Marker, std::vector<std::vector<model::Face>>>& markerFaceComponents,
        const model::BoundaryRepresentation& brep) {
        return {};
    }

    /**
     * @brief 获取特征边
     * @param markerFaceComponents
     * @param brep
     * @return
     */
    std::vector<std::vector<model::Edge>> GetFeatureEdges(const  std::unordered_map<model::Marker, std::vector<std::vector<model::Face>>>& markerFaceComponents,
                                                                          const model::BoundaryRepresentation& brep) {
        std::vector<std::vector<model::Edge>> featureEdges;
        std::map<std::pair<int, int>, std::map<model::Marker, int>> edgeCount;
        for(const auto& [marker, faceComponent] : markerFaceComponents) {
            for(const auto& component : faceComponent) {
                for(const auto& face : component) {
                    for(const auto& edge : face.edges) {
                        const auto& edge_vertice = brep.edges[edge];
                        std::pair<int, int> edge_pair = {edge_vertice.start, edge_vertice.end};
                        if(edge_pair.first > edge_pair.second) {
                            std::swap(edge_pair.first, edge_pair.second);
                        }
                        if(edgeCount.find(edge_pair) == edgeCount.end()) {
                            edgeCount[edge_pair] = {};
                        }
                        edgeCount[edge_pair][marker] += 1;
                    }
                }
            }
        }
        std::vector<model::Edge> allFeatureEdges;
        std::map<std::vector<model::Marker>, std::vector<model::Edge>> markerEdges;
        for(const auto& [edge, count] : edgeCount) {
            if(count.size() > 1) {
                std::vector<model::Marker> markers;
                for(const auto& [marker, _] : count) {
                    markers.push_back(marker);
                }
                std::ranges::sort(markers);
                markerEdges[markers].emplace_back(edge.first, edge.second);
            }
        }
        for(const auto& [markers, edges] : markerEdges) {
            std::unordered_map<model::Marker, std::vector<model::Marker>> adj;
            for(const auto& edge : edges) {
                adj[edge.start].push_back(edge.end);
                adj[edge.end].push_back(edge.start);
            }
            std::vector<model::Edge> newEdges;
            std::unordered_set<model::Marker> st;
            auto bfs([&](model::Marker u) {
                std::queue<model::Marker> q;
                q.push(u);
                while(!q.empty()) {
                    auto t = q.front();
                    q.pop();
                    for(const auto& v : adj[t]) {
                        if(st.contains(v)) {
                            continue;
                        }
                        q.push(v);
                        newEdges.emplace_back(v, t);
                        st.insert(v);
                    }
                }
            });
            for(const auto&[u, v] : edges) {
                if(!st.contains(u)) {
                    bfs(u);
                }
                if(!st.contains(v)) {
                    bfs(v);
                }
            }
            featureEdges.push_back(newEdges);
        }
        return featureEdges;
    }

    /**
     * @brief 获取PSC
     * @param brep
     * @return
     */
    model::PSC GetPSC(const model::BoundaryRepresentation& brep) {
        model::PSC psc;
        psc.vertices = brep.vertices;
        // 1. 先把各个marker的Face放在一起
        std::unordered_map<model::Marker, std::vector<model::Face>> markerFaces;
        for(const auto& face : brep.faces) {
            markerFaces[face.marker].push_back(face);
        }
        // 2. 各个Marker的集合可能会有很多个连通块，分出来
        std::unordered_map<model::Marker, std::vector<std::vector<model::Face>>> markerFaceComponents;
        for(const auto&[marker, faces] : markerFaces) {
            markerFaceComponents[marker] = GetComponents(faces, brep);
        }
        // 3. 获取特征点，这里的特征点是被多个面共用的点
        const auto featurePoints = GetFeaturePoints(markerFaceComponents, brep);
        psc.featureVertices = featurePoints;
        // 4. 获取特征边，这里的特征边是被多个面共用的边
        const auto featureEdges = GetFeatureEdges(markerFaceComponents, brep);
        psc.featureEdges = featureEdges;
        // 5. 获取各个patch
        for(const auto& [marker, faceComponents] : markerFaceComponents) {
            for(const auto& component : faceComponents) {
                model::Patch patch;
                patch.marker = marker;
                patch.faces = component;
                // Todo: 后续判断这里是否会使用，如果不用就下掉了
                // patch.vertices = brep.vertices;
                psc.patches.push_back(patch);
            }
        }
        return psc;
    }
}

#endif //BREP_PROCESS_H
