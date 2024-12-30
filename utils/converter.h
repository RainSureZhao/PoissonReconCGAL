//
// Created by ByteDance on 12/21/24.
//

#ifndef CONVERTER_H
#define CONVERTER_H

#include "model/geometry_element.h"
#include <fstream>
#include <vector>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Polyhedral_complex_mesh_domain_3.h>

namespace utils {
    /**
     * @brief 将patch转成off格式，方便可视化
     * @param patch
     * @param vertices
     * @param output
     */
    void OutputPatchToOff(const model::Patch& patch, const std::vector<model::Vertex>& vertices, const std::string& output) {
        typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
        typedef K::Point_3 Point;

        std::map<Point, int> pointToIndex;
        int idx = 0;
        std::vector<Point> points;
        for(const auto& f : patch.faces) {
            for(auto v : f.vertices) {
                assert(v < vertices.size());
                Point p = Point(vertices[v].x, vertices[v].y, vertices[v].z);
                if(pointToIndex.find(p) == pointToIndex.end()) {
                    pointToIndex[p] = idx++;
                    points.push_back(p);
                }
            }
        }
        std::ofstream out(output);
        out << "OFF" << std::endl;
        out << points.size() << " " << patch.faces.size() << " 0" << std::endl;
        for(const auto& p : points) {
            out << p.x() << " " << p.y() << " " << p.z() << std::endl;
        }
        for(const auto& f : patch.faces) {
            out << "3 ";
            for(auto v : f.vertices) {
                out << pointToIndex[Point(vertices[v].x, vertices[v].y, vertices[v].z)] << " ";
            }
            out << std::endl;
        }
        out.close();
    }

    /**
     * @brief 将psc转成patch，方便可视化
     * @param psc
     */
    void OutputPSCPatch(const model::PSC& psc) {
#pragma omp parallel for
        for(int i = 0; i < psc.patches.size(); i ++) {
            const auto& patch = psc.patches[i];
            OutputPatchToOff(patch, psc.vertices, "../data/patches/" + std::to_string(i) + ".off");
        }
    }

    typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
    typedef K::Point_3 Point;
    void OutputFeatureCurves(const std::vector<std::vector<Point>>& featureCurves) {
#pragma omp parallel for
        for(int i = 0; i < featureCurves.size(); i ++) {
            std::ofstream out("../data/newFeatureCurves/" + std::to_string(i) + ".xyz");
            for(const auto& p : featureCurves[i]) {
                out << p.x() << " " << p.y() << " " << p.z() << std::endl;
            }
            out.close();
        }
    }
}
#endif //CONVERTER_H
