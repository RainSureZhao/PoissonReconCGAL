//
// Created by RainSure on 24-12-14.
//
#include <gtest/gtest.h>
#include "service/cgal/mesh_boolean/mesh_difference.h"

TEST(LayerBlockMeshDifferenceTest, LayerBlockMeshDifferenceTest) {
    std::string mesh1_input = R"(../data/layerblock_horizon_102_processed.off)";
    std::string mesh2_input = R"(../data/layerblock_fault_105_processed.off)";
    std::string output = R"(../data/layerblock_horizon_102_fault_105_difference.off)";
    std::string output_remeshed = R"(../data/layerblock_horizon_102_fault_105_difference_remeshed.off)";
    EXPECT_NO_THROW(mesh_difference(mesh1_input, mesh2_input, output, output_remeshed));
}

TEST(ExampleTest, ExampleTest) {
    std::string mesh1_input = R"(../data/blobby.off)";
    std::string mesh2_input = R"(../data/eight.off)";
    std::string output = R"(../data/blobby_eight_difference.off)";
    std::string output_remeshed = R"(../data/blobby_eight_difference_remeshed.off)";
    EXPECT_NO_THROW(mesh_difference(mesh1_input, mesh2_input, output, output_remeshed));
}