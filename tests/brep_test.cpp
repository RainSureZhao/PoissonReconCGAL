//
// Created by ByteDance on 12/20/24.
//
#include <gtest/gtest.h>
#include <iostream>
#include "utils/read_boundary_representation.h"
#include "service/brep_process/brep_process.h"
#include "service/cgal/polyhedron/make_polyhedron.h"
#include "service/cgal/tetrahedron/tetrahedrzlization.h"
#include "service/pcl/point_cloud/view_point_cloud.h"
#include "service/vtk/point_cloud/view_point_cloud.h"
#include "utils/converter.h"

TEST(BrepTest, Test1) {
    auto brep = utils::ReadBoundaryRepresentation(R"(../data/layerblock.brep)");
    std::cout <<  brep.vertices.size() << std::endl;
    for(auto& marker : brep.horizonMarkers) {
        std::cout << "horizon marker: " << marker << std::endl;
    }
    for(auto& marker : brep.faultMarkers) {
        std::cout << "fault marker: " << marker << std::endl;
    }
    for(auto& marker : brep.boundaryMarkers) {
        std::cout << "boundary marker: " << marker << std::endl;
    }
}

TEST(BrepTest, Test2) {
    auto brep = utils::ReadBoundaryRepresentation(R"(../data/layerblock.brep)");
    auto psc = service::GetPSC(brep);
    std::cout << "psc vertices: " << psc.vertices.size() << "\n";
    std::cout << "psc feature edges: " << psc.featureEdges.size() << "\n";
    for(const auto& featureEdge : psc.featureEdges) {
        std::cout << "featureEdge: " << featureEdge.size() << "\n";
    }
    std::cout << "psc patches: " << psc.patches.size() << "\n";
}

TEST(BrepTest, Test3) {
    auto brep = utils::ReadBoundaryRepresentation(R"(../data/layerblock.brep)");
    auto psc = service::GetPSC(brep);
    auto patches = service::cgal::polyhedron::ConvertPSCToPolyhedrons(psc);
    std::vector<std::pair<int, int>> incidientDomains;
    incidientDomains.reserve(patches.size());
    for(size_t i = 0; i < patches.size(); i ++) {
        incidientDomains.emplace_back(0, 1);
    }
    auto featureCurves = service::cgal::polyhedron::GetFeatureCurves(psc.featureEdges, psc.vertices);
    std::vector<std::vector<service::cgal::polyhedron::Point>> newFeatureCurves;
    for(auto& featureCurve : featureCurves) {
        auto featureCurveSplit = service::cgal::polyhedron::ProcessFeatureCurve(featureCurve);
        newFeatureCurves.insert(newFeatureCurves.end(), featureCurveSplit.begin(), featureCurveSplit.end());
    }
    const std::string output = R"(../data/layerblock_test.mesh)";
    service::cgal::tetrahedron::Tetrahedralization(patches, newFeatureCurves, incidientDomains, output, 10.0, 25, 10.0, 1.0, 10.0, 10.0);
}

TEST(BrepTest, Test4) {
    auto brep = utils::ReadBoundaryRepresentation(R"(../data/layerblock.brep)");
    auto psc = service::GetPSC(brep);
    // utils::OutputPSCPatch(psc);
    auto featureCurves = service::cgal::polyhedron::GetFeatureCurves(psc.featureEdges, psc.vertices);
    std::vector<std::vector<service::cgal::polyhedron::Point>> newFeatureCurves;
    for(auto& featureCurve : featureCurves) {
        auto featureCurveSplit = service::cgal::polyhedron::ProcessFeatureCurve(featureCurve);
        newFeatureCurves.insert(newFeatureCurves.end(), featureCurveSplit.begin(), featureCurveSplit.end());
    }
    utils::OutputFeatureCurves(newFeatureCurves);
}

TEST(BrepTest, Test5) {
    auto brep = utils::ReadBoundaryRepresentation(R"(../data/layerblock.brep)");
    auto psc = service::GetPSC(brep);
    // utils::OutputPSCPatch(psc);
    auto featureCurves = service::cgal::polyhedron::GetFeatureCurves(psc.featureEdges, psc.vertices);
    std::vector<std::vector<service::cgal::polyhedron::Point>> newFeatureCurves;
    for(auto& featureCurve : featureCurves) {
        auto featureCurveSplit = service::cgal::polyhedron::ProcessFeatureCurve(featureCurve);
        newFeatureCurves.insert(newFeatureCurves.end(), featureCurveSplit.begin(), featureCurveSplit.end());
    }
    std::vector<std::vector<service::pcls::point_cloud::Point3D>> point_clouds;
    for(auto& featureCurve : newFeatureCurves) {
        std::vector<service::pcls::point_cloud::Point3D> point_cloud;
        point_cloud.reserve(featureCurve.size());
        for(auto& point : featureCurve) {
            point_cloud.emplace_back(point.x(), point.y(), point.z());
        }
        point_clouds.emplace_back(point_cloud);
    }
    service::pcls::point_cloud::View_point_clouds(point_clouds);
}

TEST(BrepTest, Test6) {
    auto brep = utils::ReadBoundaryRepresentation(R"(../data/layerblock.brep)");
    auto psc = service::GetPSC(brep);

    auto featureCurves = service::cgal::polyhedron::GetFeatureCurves(psc.featureEdges, psc.vertices);
    std::vector<std::vector<service::cgal::polyhedron::Point>> newFeatureCurves;
    for(auto& featureCurve : featureCurves) {
        auto featureCurveSplit = service::cgal::polyhedron::ProcessFeatureCurve(featureCurve);
        newFeatureCurves.insert(newFeatureCurves.end(), featureCurveSplit.begin(), featureCurveSplit.end());
    }
    std::vector<std::vector<service::vtks::point_cloud::Point3D>> point_clouds;
    for(auto& featureCurve : newFeatureCurves) {
        std::vector<service::vtks::point_cloud::Point3D> point_cloud;
        point_cloud.reserve(featureCurve.size());
        for(auto& point : featureCurve) {
            point_cloud.emplace_back(point.x(), point.y(), point.z());
        }
        point_clouds.emplace_back(point_cloud);
    }
    service::vtks::point_cloud::View_point_clouds(point_clouds);
}

TEST(BrepTest, Test7) {
    service::vtks::point_cloud::Test_vtk_PointCloud();
}