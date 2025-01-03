//
// Created by ByteDance on 12/26/24.
//
#include <gtest/gtest.h>
#include "service/weighted_points/weighted_points_process.h"

TEST(WeightPointTest, Test1) {
    service::weighted_points::Refine_weighted_points(
        R"(../data/weighted_points_corners.xyz)",
        R"(../data/weighted_points_edges.xyz)",
        R"(../data/refine_balls.xyz)",
        R"(../data/weighted_points_layerblock_output.xyz)"
    );
    service::weighted_points::Convert_weighted_points_to_mesh_separately(R"(../data/weighted_points_layerblock_output.xyz)",
        R"(../data/weighted_points_layerblock/weight_point)");
}

TEST(WeightPointTest, Test2) {
    service::weighted_points::Refine_weighted_points(
        R"(../data/weighted_points_corners_layerblock.xyz)",
        R"(../data/weighted_points_edges_layerblock.xyz)",
        R"(../data/refine_balls.xyz)",
        R"(../data/weighted_points_layerblock_output.xyz)"
    );
    service::weighted_points::Convert_weighted_points_to_mesh(R"(../data/weighted_points_layerblock_output.xyz)",
        R"(../data/weighted_points_layerblock/weight_points_without_refine_ball_v2.off)");
}

TEST(WeightPointTest, Test3) {
    service::weighted_points::Refine_weighted_points(
        R"(../data/weighted_points_corners.xyz)",
        R"(../data/weighted_points_edges.xyz)",
        R"(../data/refine_balls.xyz)",
        R"(../data/weighted_points_cube_output.xyz)"
    );
    service::weighted_points::Convert_weighted_points_to_mesh(R"(../data/weighted_points_cube_output.xyz)",
        R"(../data/weighted_points_cube/weight_points_v2.off)");
}