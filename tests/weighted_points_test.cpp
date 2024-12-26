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
        R"(../data/weighted_points_output.xyz)"
    );
    service::weighted_points::Convert_weighted_points_to_mesh_separately(R"(../data/weighted_points_output.xyz)",
        R"(../data/weighted_points/weight_point)");
}