//
// Created by RainSure on 24-12-15.
//
#include <gtest/gtest.h>
#include "service/cgal/get_intersections/get_intersections.h"
#include <fstream>

TEST(GetIntersectionTest, GetIntersectionTest) {
    std::string surface_input = R"(../data/layerblock_horizon_102_processed.off)";
    std::string fault_input = R"(../data/layerblock_fault_105_processed.off)";
    Surface_mesh surface_mesh;
    Surface_mesh fault_mesh;
    std::ifstream surface_in(surface_input);
    std::ifstream fault_in(fault_input);
    if(!surface_in || !(surface_in >> surface_mesh)) {
        std::cerr << "Error: cannot read surface mesh from " << surface_input << std::endl;
        return;
    }
    if(!fault_in || !(fault_in >> fault_mesh)) {
        std::cerr << "Error: cannot read fault mesh from " << fault_input << std::endl;
        return;
    }
    auto result = get_intersection(surface_mesh, fault_mesh);
    std::cout << "Intersection size: " << result.size() << std::endl;
    std::string intersection_output1 = R"(../data/layerblock_horizon_102_fault_105_intersection_102.off)";
    std::string intersection_output2 = R"(../data/layerblock_horizon_102_fault_105_intersection_105.off)";
    export_intersection_to_off(surface_mesh, fault_mesh, result, intersection_output1, intersection_output2);
}