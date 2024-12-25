//
// Created by ByteDance on 12/25/24.
//

#ifndef WEIGHTED_POINTS_PROCESS_H
#define WEIGHTED_POINTS_PROCESS_H

#include <vector>
#include <map>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
namespace service::weighted_points {
    typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
    typedef K::Point_3 Point_3;

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
     * @brief 加权点转成mesh
     * @param weighted_points_input
     * @param output
     */
    void Convert_weighted_points_to_mesh(const std::string& weighted_points_input, const std::string& output) {


    }
}

#endif //WEIGHTED_POINTS_PROCESS_H
