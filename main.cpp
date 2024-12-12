//#include "examples/poissonRecon.h"
//#include "examples/compute_normal.h"
//#include "examples/compute_vertex_normal.h"
//#include "examples/export_mesh.h"
//#include "examples/extract_point_and_normal.h"
#include "examples/constrained_tetrahedralization.h"
// #include "service/cgal/multi_complex_mesh/multi_complex_mesh.h"
// #include "service/cgal/recovery_edges_mesh/recovery_edges_mesh.h"
#include "examples/read_berp.h"
int main()
{
    // int result = compute_normal(R"(D:\Code\Data\Tunnel\19-15121.xyz)", R"(D:\Code\Data\Tunnel\19-15121_normals.xyz)");
    // process_mesh(R"(D:\Code\Data\Tunnel\19-15121.xyz)", R"(D:\Code\Data\Tunnel\19-15121_normals.xyz)");
    // export_to_obj(R"(D:\Code\Data\Tunnel\54-15309.xyz)", R"(D:\Code\Data\Tunnel\54-15309.obj)");
    // extractPointsAndNormals(R"(D:\Code\Data\xh_model_data\Layer_hht4a.obj)", R"(D:\Code\Data\xh_model_data\Layer_hht4a.xyz)");
//    int result = solve(R"(D:\Code\Data\xh_model_data\Layer_hht4a.xyz)", R"(D:\Code\Data\xh_model_data\Layer_hht4a.off)");
//    return result;
    // return 0;

//     cdt::tetrahedralization_by_implicit_function(sphere_function);

//     cdt::convert_brep_to_poly(R"(D:\Code\cpp\PoissonReconCGAL\data\layerblock.brep)", R"(D:\Code\cpp\PoissonReconCGAL\data\layerblock)");
//     cdt::constrained_tetrahedralization_with_tetgen(R"(D:\Code\cpp\PoissonReconCGAL\data\layerblock)", R"(D:\Code\cpp\PoissonReconCGAL\data\layerblock)");
//     cdt::convertNodeEleToPLY(R"(D:\Code\cpp\PoissonReconCGAL\data\layerblock.node)", R"(D:\Code\cpp\PoissonReconCGAL\data\layerblock.ele)", R"(D:\Code\cpp\PoissonReconCGAL\data\layerblock.ply)");

//     cdt::tetrahedralization_by_surface_mesh(R"(../data/cube_2.off)", R"(../data/cube_2.mesh)", R"(../data/cube_2_refinement.mesh)");

//    std::vector<std::string> surfaces {
//        R"(D:\Code\cpp\PoissonReconCGAL\data\model_0.off)",
//        R"(D:\Code\cpp\PoissonReconCGAL\data\model_1.off)",
//        R"(D:\Code\cpp\PoissonReconCGAL\data\model_2.off)",
//        R"(D:\Code\cpp\PoissonReconCGAL\data\model_3.off)"
//    };
//    std::vector<std::pair<int, int>> indicies {
//            {0, 1},
//            {0, 1},
//            {0, 1},
//            {0, 1}
//    };
//    cdt::recovery_edges_from_surface(surfaces, indicies, R"(D:\Code\cpp\PoissonReconCGAL\data\polylines.txt)", R"(D:\Code\cpp\PoissonReconCGAL\data\model.mesh)");

    // std::vector<std::string> surfaces {
    //     R"(../data/cube_2.off)"
    // };
    // std::vector<std::pair<int, int>> indicies {
    //             {0, 1}
    // };
    // std::string edges = "";
    // cdt::recovery_edges_from_surface(surfaces, indicies, edges, R"(../data/cube_2.mesh)");

    std::vector<std::string> surfaces {
        R"(../data/patch-01.off)",
        R"(../data/patch-13.off)",
        R"(../data/patch-20.off)",
        R"(../data/patch-21.off)",
        R"(../data/patch-23.off)",
        R"(../data/patch-30.off)"
    };
    // std::vector<std::pair<int, int>> indicies {
    //                 {0, 1},
    //     {1, 3},
    //     {2, 0},
    //     {2, 1},
    //     {2, 3},
    //     {3, 0}
    // };
    // std::string edges = "";
    // cdt::recovery_edges_from_surface(surfaces, indicies, edges, R"(../data/patch_v2.mesh)");

    cdt::get_duplicate_vertices(surfaces, R"(../data/test.xyz)");

//     cdt::convert_brep_to_poly(R"(D:\Code\cpp\PoissonReconCGAL\data\layerblock.brep)", R"(D:\Code\cpp\PoissonReconCGAL\data\layerblock.poly)");
//     cdt::constrained_tetrahedralization_with_tetgen(R"(D:\Code\cpp\PoissonReconCGAL\data\cuboid)", R"(D:\Code\cpp\PoissonReconCGAL\data\cuboid)");
//     cdt::convertNodeEleToPLY(R"(D:\Code\cpp\PoissonReconCGAL\data\cuboid.node)", R"(D:\Code\cpp\PoissonReconCGAL\data\cuboid.ele)", R"(D:\Code\cpp\PoissonReconCGAL\data\cuboid.ply)");
//     cdt::tetrahedralization_by_surface_mesh(R"(D:\Code\cpp\PoissonReconCGAL\data\cone.off)", R"(D:\Code\cpp\PoissonReconCGAL\data\cone.mesh)", R"(D:\Code\cpp\PoissonReconCGAL\data\cone_refinement.mesh)");
}
