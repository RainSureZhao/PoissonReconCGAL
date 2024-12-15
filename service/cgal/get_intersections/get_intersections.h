//
// Created by RainSure on 24-12-15.
//

#ifndef POISSONRECONCGAL_GET_INTERSECTIONS_H
#define POISSONRECONCGAL_GET_INTERSECTIONS_H

#include <CGAL/Surface_mesh.h>
#include <CGAL/Polygon_mesh_processing/intersection.h>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Polyhedron_3.h>
#include <CGAL/boost/graph/IO/OFF.h>
#include <iostream>
#include <fstream>
#include <vector>

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef K::Point_3 Point_3;
typedef CGAL::Surface_mesh<Point_3> Surface_mesh;
typedef CGAL::Polyhedron_3<K> Polyhedron;
typedef typename boost::graph_traits<Surface_mesh>::face_descriptor    face_descriptor;
typedef Surface_mesh::Vertex_index vertex_descriptor;

/**
 * 获取两个网格的交集
 * @param mesh1
 * @param mesh2
 * @return 返回一个 pair 的 vector，pair 中的两个元素分别是两个网格的面
 */
std::vector<std::pair<face_descriptor, face_descriptor>> get_intersection(const Surface_mesh& mesh1, const Surface_mesh& mesh2) {

    std::vector<std::pair<face_descriptor, face_descriptor>> intersected_tris;
    CGAL::Polygon_mesh_processing::internal::compute_face_face_intersection(
            mesh1, mesh2,
            std::back_inserter(intersected_tris),
            CGAL::parameters::default_values(),
            CGAL::parameters::default_values());

    return intersected_tris;
}

/**
 * 将交集保存为 OFF 文件
 * @param mesh1
 * @param mesh2
 * @param intersected_tris
 * @param output_file1
 * @param output_file2
 */
void export_intersection_to_off(const Surface_mesh& mesh1, const Surface_mesh& mesh2,
                                const std::vector<std::pair<face_descriptor, face_descriptor>>& intersected_tris,
                                const std::string& output_file1, const std::string& output_file2) {
    // 创建两个新的 Surface_mesh 用于保存相交部分
    Surface_mesh intersection_mesh1;
    Surface_mesh intersection_mesh2;

    // 创建一个顶点映射，避免重复顶点
    std::map<Point_3, vertex_descriptor> point_to_vertex1, point_to_vertex2;

    // 遍历交集信息
    for (const auto& intersected_pair : intersected_tris) {
        face_descriptor face1 = intersected_pair.first;
        face_descriptor face2 = intersected_pair.second;

        // 获取 face1 的顶点
        std::vector<Point_3> vertices1;
        for (auto v : mesh1.vertices_around_face(mesh1.halfedge(face1))) {
            vertices1.push_back(mesh1.point(v));
        }

        // 获取 face2 的顶点
        std::vector<Point_3> vertices2;
        for (auto v : mesh2.vertices_around_face(mesh2.halfedge(face2))) {
            vertices2.push_back(mesh2.point(v));
        }
        std::cout << "vertices1 size: " << vertices1.size() << std::endl;
        std::cout << "vertices2 size: " << vertices2.size() << std::endl;

        // 将顶点添加到新的网格中（避免重复）
        std::vector<vertex_descriptor> new_vertices1, new_vertices2;

        auto add_vertex = [&](const Point_3& pt, std::map<Point_3, vertex_descriptor>& point_to_vertex, Surface_mesh& intersection_mesh) -> vertex_descriptor {
            // 查找是否已经存在相同的顶点
            auto it = point_to_vertex.find(pt);
            if (it != point_to_vertex.end()) {
                return it->second;
            } else {
                vertex_descriptor v = intersection_mesh.add_vertex(pt);
                point_to_vertex[pt] = v;
                return v;
            }
        };

        // 添加 face1 的顶点到 mesh1 的交集
        for (const auto& pt : vertices1) {
            new_vertices1.push_back(add_vertex(pt, point_to_vertex1, intersection_mesh1));
        }

        // 添加 face2 的顶点到 mesh2 的交集
        for (const auto& pt : vertices2) {
            new_vertices2.push_back(add_vertex(pt, point_to_vertex2, intersection_mesh2));
        }

        // 创建相交面：根据新顶点添加面
        intersection_mesh1.add_face(new_vertices1);
        intersection_mesh2.add_face(new_vertices2);
    }

    // 将交集的网格保存为 OFF 文件
    std::ofstream out1(output_file1);
    std::ofstream out2(output_file2);

    if (out1) {
        CGAL::IO::write_OFF(out1, intersection_mesh1);
        std::cout << "Intersection for mesh1 saved to " << output_file1 << std::endl;
    } else {
        std::cerr << "Error: unable to write to " << output_file1 << std::endl;
    }

    if (out2) {
        CGAL::IO::write_OFF(out2, intersection_mesh2);
        std::cout << "Intersection for mesh2 saved to " << output_file2 << std::endl;
    } else {
        std::cerr << "Error: unable to write to " << output_file2 << std::endl;
    }
    out1.close();
    out2.close();
}

#endif //POISSONRECONCGAL_GET_INTERSECTIONS_H
