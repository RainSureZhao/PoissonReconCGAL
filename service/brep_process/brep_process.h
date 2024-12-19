//
// Created by ByteDance on 12/19/24.
//

#ifndef BREP_PROCESS_H
#define BREP_PROCESS_H

#include "model/geometry_element.h"
#include <vector>
#include <unordered_map>
#include <CGAL/Simple_cartesian.h>
#include <CGAL/Surface_mesh.h>

#include <CGAL/boost/graph/generators.h>
#include <CGAL/Polygon_mesh_processing/connected_components.h>
#include <CGAL/boost/graph/Face_filtered_graph.h>
#include <CGAL/property_map.h>

#include <boost/property_map/function_property_map.hpp>
namespace service {

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
    }


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
     * @brief 获取连通块
     * @param faces
     * @param vertices
     * @return
     */
    std::vector<std::vector<model::Face>> GetComponents(const std::vector<model::Face>& faces, const std::vector<model::Vertex>& vertices) {
        Mesh surfaceMesh = GetMesh(faces, vertices);

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
            components[i] = GetFacesFromSurfaceMesh(meshes[i]);
        }
        return components;
    }

    /**
     *
     * @param mesh
     * @return
     */
    std::vector<model::Face> GetFacesFromSurfaceMesh(const Mesh& mesh) {
        std::vector<model::Face> faces;

    }
}

#endif //BREP_PROCESS_H
