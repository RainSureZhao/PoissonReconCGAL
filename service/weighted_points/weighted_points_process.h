//
// Created by ByteDance on 12/25/24.
//

#ifndef WEIGHTED_POINTS_PROCESS_H
#define WEIGHTED_POINTS_PROCESS_H

#include <vector>
#include <map>
#include <fstream>
#include <array>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Polyhedron_3.h>
#include <CGAL/make_mesh_3.h>
#include <CGAL/Surface_mesh.h>
#include <CGAL/Subdivision_method_3/subdivision_methods_3.h>
#include "CGAL/boost/graph/generators.h"
#include "CGAL/boost/graph/Graph_with_descriptor_with_graph.h"

namespace service::weighted_points {
    typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
    typedef K::Point_3 Point_3;
    typedef CGAL::Polyhedron_3<K> Polyhedron;
    typedef CGAL::Surface_mesh<Point_3>                          Mesh3;

    std::vector<std::pair<Point_3, double>> getWeightedPoints(const std::string& input) {
        std::vector<std::pair<Point_3, double>> weighted_points;
        std::ifstream inputFile(input);
        if(!inputFile.is_open()) {
            std::cerr << "Error opening file " << input << std::endl;
            return weighted_points;
        }
        std::string line;
        while(std::getline(inputFile, line)) {
            std::istringstream iss(line);
            double x, y, z;
            if(double weight; iss >> x >> y >> z >> weight) {
                weighted_points.emplace_back(Point_3(x, y, z), weight);
            }
        }
        inputFile.close();
        return weighted_points;
    }


    /**
     * @brief 加权点处理
     * @param weighted_points_corners_input
     * @param weighted_points_edges_input
     * @param refine_weighted_points_input
     * @param output
     */
    void Refine_weighted_points(const std::string& weighted_points_corners_input, const std::string& weighted_points_edges_input,
                                const std::string& refine_weighted_points_input, const std::string& output) {
        auto weighted_points_corners = getWeightedPoints(weighted_points_corners_input);
        auto weighted_points_edges = getWeightedPoints(weighted_points_edges_input);
        auto refine_weighted_points = getWeightedPoints(refine_weighted_points_input);
        std::map<Point_3, double> weighted_point_map;
        for(const auto& [p, w] : weighted_points_corners) {
            weighted_point_map[p] = w;
        }
        for(const auto& [p, w] : weighted_points_edges) {
            weighted_point_map[p] = w;
        }
        for(const auto& [p, w] : refine_weighted_points) {
            weighted_point_map[p] = w;
        }
        std::ofstream outputFile(output);
        if(!outputFile.is_open()) {
            std::cerr << "Error opening file " << output << std::endl;
            return;
        }
        for(const auto& [p, w] : weighted_point_map) {
            outputFile << p.x() << " " << p.y() << " " << p.z() << " " << w << std::endl;
        }
        outputFile.close();
    }

    /**
     *
     * @param center
     * @param radius
     * @param iterations
     * @return
     */
    Mesh3 GetSphere(const Point_3& center, double radius, unsigned int iterations = 2) {
        Mesh3 mesh;
        CGAL::make_icosahedron<Mesh3, Point_3>(mesh, center, radius);
        CGAL::Subdivision_method_3::Loop_subdivision(
          mesh, CGAL::parameters::number_of_iterations(iterations)
        );
        return mesh;
    }

    /**
     * @brief
     * @param spheres
     * @param output
     */
    void write_spheres_to_off(const std::vector<Mesh3>& spheres, const std::string& output) {
        std::ofstream out(output);
        std::map<Point_3, int> pointMap;
        int idx = 0;
        std::vector<Point_3> points;
        std::vector<std::array<Point_3, 3>> triangles;
        for(const auto& sphere : spheres) {
            for(const auto& v : vertices(sphere)) {
                if(Point_3 point = {sphere.point(v).x(), sphere.point(v).y(), sphere.point(v).z()}; !pointMap.contains(point)) {
                    pointMap[point] = idx ++;
                    points.push_back(point);
                }
            }
            for(const auto& f : faces(sphere)) {
                std::vector<Point_3> triangle;
                for(auto v : CGAL::vertices_around_face(halfedge(f, sphere), sphere)){
                    Point_3 point = {sphere.point(v).x(), sphere.point(v).y(), sphere.point(v).z()};
                    triangle.push_back(point);
                }
                assert(triangle.size() == 3);
                triangles.push_back({triangle[0], triangle[1], triangle[2]});
            }
        }
        out << "OFF" << std::endl;
        out << points.size() << ' ' << triangles.size() << " 0" << std::endl;
        for(const auto& p : points) {
            out << p.x() << ' ' << p.y() << ' ' << p.z() << std::endl;
        }
        for(const auto& triangle : triangles) {
            assert(pointMap.contains(triangle[0]));
            assert(pointMap.contains(triangle[1]));
            assert(pointMap.contains(triangle[2]));
            out << 3 << ' ' << pointMap[triangle[0]] << ' ' << pointMap[triangle[1]] << ' ' << pointMap[triangle[2]] << std::endl;
        }
        out.close();
    }

    /**
     * @brief 加权点转成mesh
     * @param weighted_points_input
     * @param output
     */
    void Convert_weighted_points_to_mesh(const std::string& weighted_points_input, const std::string& output) {
        auto weightedPoints = getWeightedPoints(weighted_points_input);
        std::vector<Mesh3> spheres(weightedPoints.size());
#pragma omp parallel for
        for(int i = 0; i < weightedPoints.size(); i ++) {
            spheres[i] = GetSphere(weightedPoints[i].first, weightedPoints[i].second);
        }
        write_spheres_to_off(spheres, output);
    }

    /**
     * @brief 加权点转成mesh
     * @param weighted_points_input
     * @param output
     */
    void Convert_weighted_points_to_mesh_separately(const std::string& weighted_points_input, const std::string& output) {
        auto weightedPoints = getWeightedPoints(weighted_points_input);
        std::vector<Mesh3> spheres(weightedPoints.size());
#pragma omp parallel for
        for(int i = 0; i < weightedPoints.size(); i ++) {
            spheres[i] = GetSphere(weightedPoints[i].first, weightedPoints[i].second);
            write_spheres_to_off({spheres[i]}, output + std::to_string(i) + ".off");
        }
    }


}

#endif //WEIGHTED_POINTS_PROCESS_H
