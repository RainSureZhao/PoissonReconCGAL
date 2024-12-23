//
// Created by RainSure on 24-12-21.
//

#ifndef POISSONRECONCGAL_MAKE_POLYHEDRON_H
#define POISSONRECONCGAL_MAKE_POLYHEDRON_H

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Polyhedral_complex_mesh_domain_3.h>
#include "model/geometry_element.h"
#include <vector>
#include <map>
#include <random>
#include <fstream>

namespace service::cgal::polyhedron {
    typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
    typedef K::Point_3 Point;
    typedef CGAL::Mesh_polyhedron_3<K>::type Polyhedron;

    /**
     * @brief 将Patch转换为Polyhedron
     * @param patch
     * @param vertices
     * @return
     */
    Polyhedron ConvertPatchToPolyhedron(const model::Patch& patch, const std::vector<model::Vertex>& vertices) {
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
        std::random_device rd;
        // 输出到off文件，文件名随机生成，后续会删除掉，../data/xxx.off，xxx要足够随机，因为这个函数会并发调用，不能够有冲突
        std::string off = "../data/" + std::to_string(rd()) + ".off";
        std::ofstream out(off);
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

        std::ifstream fin(off);
        Polyhedron polyhedron;
        if(!(fin >> polyhedron)) {
            std::cerr << "Error reading " << off << " as a polyhedron!\n";
            throw std::runtime_error("Cannot read the polyhedron from the file");
        }
        fin.close();
        // delete这个文件
        std::remove(off.c_str());
        return polyhedron;
    }

    /**
     * @brief 将PSC转换为Polyhedrons
     * @param psc
     * @return
     */
    std::vector<Polyhedron> ConvertPSCToPolyhedrons(const model::PSC& psc) {
        std::vector<Polyhedron> patches(psc.patches.size());
#pragma omp parallel for
        for(int i = 0; i < psc.patches.size(); i ++) {
            patches[i] = ConvertPatchToPolyhedron(psc.patches[i], psc.vertices);
        }
        return patches;
    }

    /**
     * @brief GetFeatureCurves
     * @param featureEdges
     * @param vertices
     * @return
     */
    std::vector<std::vector<Point>> GetFeatureCurves(const std::vector<std::vector<model::Edge>>& featureEdges, const std::vector<model::Vertex>& vertices) {
        std::vector<std::vector<Point>> featureCurves;
        for(const auto& edges : featureEdges) {
            std::vector<Point> curve;
            for(const auto& edge : edges) {
                Point p1 = Point(vertices[edge.start].x, vertices[edge.start].y, vertices[edge.start].z);
                if(curve.empty() or curve.back() != p1) {
                    curve.push_back(p1);
                }
                Point p2 = Point(vertices[edge.end].x, vertices[edge.end].y, vertices[edge.end].z);
                curve.push_back(p2);
            }
            featureCurves.push_back(curve);
        }
        return featureCurves;
    }
}

#endif //POISSONRECONCGAL_MAKE_POLYHEDRON_H
