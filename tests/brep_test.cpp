//
// Created by ByteDance on 12/20/24.
//
#include <gtest/gtest.h>
#include <iostream>
#include "utils/read_boundary_representation.h"
#include "service/brep_process/brep_process.h"
#include "service/cgal/polyhedron/make_polyhedron.h"
#include "service/cgal/tetrahedron/tetrahedrzlization.h"
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
    const auto featureCurves = service::cgal::polyhedron::GetFeatureCurves(psc.featureEdges, psc.vertices);
    const std::string output = R"(../data/layerblock_test.mesh)";
    service::cgal::tetrahedron::Tetrahedralization(patches, featureCurves, incidientDomains, output, 100.0, 25, 100.0, 10.0, 100.0, 100.0);
}

TEST(BrepTest, Test4) {
    auto brep = utils::ReadBoundaryRepresentation(R"(../data/layerblock.brep)");
    auto psc = service::GetPSC(brep);
    // utils::OutputPSCPatch(psc);
    auto featureCurves = service::cgal::polyhedron::GetFeatureCurves(psc.featureEdges, psc.vertices);
    utils::OutputFeatureCurves(featureCurves);
}