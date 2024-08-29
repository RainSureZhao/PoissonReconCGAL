#include "examples/poissonRecon.h"
#include "examples/compute_normal.h"
#include "examples/compute_vertex_normal.h"
#include "examples/export_mesh.h"
#include "examples/extract_point_and_normal.h"
int main()
{
    // int result = compute_normal(R"(D:\Code\Data\Tunnel\19-15121.xyz)", R"(D:\Code\Data\Tunnel\19-15121_normals.xyz)");
    // process_mesh(R"(D:\Code\Data\Tunnel\19-15121.xyz)", R"(D:\Code\Data\Tunnel\19-15121_normals.xyz)");
    // export_to_obj(R"(D:\Code\Data\Tunnel\54-15309.xyz)", R"(D:\Code\Data\Tunnel\54-15309.obj)");
    extractPointsAndNormals(R"(D:\Code\Data\xh_model_data\Layer_hht4a.obj)", R"(D:\Code\Data\xh_model_data\Layer_hht4a.xyz)");
    int result = solve(R"(D:\Code\Data\xh_model_data\Layer_hht4a.xyz)", R"(D:\Code\Data\xh_model_data\Layer_hht4a.off)");
    return result;
    // return 0;
}
