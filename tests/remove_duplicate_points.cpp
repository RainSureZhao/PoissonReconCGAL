//
// Created by RainSure on 24-12-13.
//
#include "examples/constrained_tetrahedralization.h"
#include <gtest/gtest.h>

TEST(RemoveDuplicatePointsTest, RemoveDuplicatePointsForLayerBlock) {
    std::vector<std::string> inputs {
            R"(../data/layerblock_boundary.off)",
            R"(../data/layerblock_horizon_100.off)",
            R"(../data/layerblock_horizon_101.off)",
            R"(../data/layerblock_horizon_102.off)",
            R"(../data/layerblock_horizon_103.off)",
            R"(../data/layerblock_horizon_104.off)",
            R"(../data/layerblock_fault_105.off)",
            R"(../data/layerblock_fault_106.off)",
            R"(../data/layerblock_fault_107.off)",
            R"(../data/layerblock_fault_108.off)",
            R"(../data/layerblock_fault_109.off)",
            R"(../data/layerblock_fault_110.off)"
    };
    std::vector<std::string> outputs {
            R"(../data/layerblock_boundary_processed.off)",
            R"(../data/layerblock_horizon_100_processed.off)",
            R"(../data/layerblock_horizon_101_processed.off)",
            R"(../data/layerblock_horizon_102_processed.off)",
            R"(../data/layerblock_horizon_103_processed.off)",
            R"(../data/layerblock_horizon_104_processed.off)",
            R"(../data/layerblock_fault_105_processed.off)",
            R"(../data/layerblock_fault_106_processed.off)",
            R"(../data/layerblock_fault_107_processed.off)",
            R"(../data/layerblock_fault_108_processed.off)",
            R"(../data/layerblock_fault_109_processed.off)",
            R"(../data/layerblock_fault_110_processed.off)"
    };
    EXPECT_NO_THROW(cdt::multi_remove_duplicate_vertices(inputs, outputs));
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}