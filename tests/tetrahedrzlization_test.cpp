//
// Created by RainSure on 24-12-14.
//
#include "service/cgal/recovery_edges_mesh/recovery_edges_mesh.h"
#include <gtest/gtest.h>

TEST(LayerBlockTest1, LayerBlockTest1) {
    std::vector<std::string> surfaces {
//        R"(../data/layerblock_boundary_processed.off)",
//        R"(../data/layerblock_horizon_100_processed.off)",
//        R"(../data/layerblock_horizon_101_processed.off)",
//        R"(../data/layerblock_horizon_102_processed.off)",
//        R"(../data/layerblock_horizon_103_processed.off)",
//        R"(../data/layerblock_horizon_104_processed.off)",
//        R"(../data/layerblock_fault_105_processed.off)",
//        R"(../data/layerblock_fault_106_processed.off)",
        R"(../data/layerblock_fault_107_processed.off)",
//        R"(../data/layerblock_fault_108_processed.off)",
//        R"(../data/layerblock_fault_109_processed.off)",
//        R"(../data/layerblock_fault_110_processed.off)"
    };
    std::vector<std::pair<int, int>> indices {
//            {0, 1},
//            {0, 1},
//            {0, 1},
            {0, 1},
//            {0, 1},
//            {0, 1},
//            {0, 1},
//            {0, 1},
//            {0, 1},
//            {0, 1},
//            {0, 1},
//            {0, 1}
    };
    std::string edges;
    std::string output = R"(..\data\cube_2_edges_test.mesh)";
    cdt::recovery_edges_from_surface_with_criteria(surfaces, indices, edges, output, 50.0, 20, 50.0, 10.0, 2.0, 50.0);
}

TEST(modelTest1, modelTest1) {
    std::vector<std::string> surfaces {
//        R"(../data/model_0.off)",
//        R"(../data/model_1.off)",
        R"(../data/model_2.off)",
//        R"(../data/model_3.off)",

    };
    std::vector<std::pair<int, int>> indices {
        {0, 1},
//        {0, 1},
//        {0, 1},
//        {0, 1},
    };
    std::string edges;
    std::string output = R"(..\data\model_edges_test.mesh)";
    cdt::recovery_edges_from_surface_with_criteria(surfaces, indices, edges, output, 1.0, 1.0, 1.0, 1.0, 2.0, 1.0);
}

TEST(PatchTest1, PatchTest1) {
    std::vector<std::string> surfaces {
        R"(../data/patch-01.off)",
        R"(../data/patch-13.off)",
        R"(../data/patch-20.off)",
        R"(../data/patch-21.off)",
        R"(../data/patch-23.off)",
        R"(../data/patch-30.off)",
    };

    std::vector<std::pair<int, int>> indices {
        {0, 1},
        {1, 3},
        {2, 0},
        {2, 1},
        {2, 3},
        {3, 0}
    };

    std::string edges;
    std::string output = R"(../data/patch_model_test.mesh)";
    cdt::recovery_edges_from_surface_with_criteria(surfaces, indices, edges, output, 1.0, 1.0, 1.0, 1.0, 2.0, 1.0);
}