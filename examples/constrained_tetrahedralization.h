//
// Created by RainSure on 24-10-1.
//

#ifndef POISSONRECONCGAL_CONSTRAINED_TETRAHEDRALIZATION_H
#define POISSONRECONCGAL_CONSTRAINED_TETRAHEDRALIZATION_H

#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <cassert>
#include <algorithm>
#include <array>
#include <set>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/Exporter.hpp>
#include "tetgen.h"
#include "utils/read_polylines.h"


#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>

#include <CGAL/Mesh_triangulation_3.h>
#include <CGAL/Mesh_complex_3_in_triangulation_3.h>
#include <CGAL/Mesh_criteria_3.h>

#include <CGAL/Implicit_to_labeling_function_wrapper.h>
#include <CGAL/Labeled_mesh_domain_3.h>
#include <CGAL/make_mesh_3.h>
#include "utils/implicit_functions.h"

// IO
#include <CGAL/IO/File_medit.h>

namespace params = CGAL::parameters;

#ifdef CGAL_CONCURRENT_MESH_3
typedef CGAL::Parallel_tag Concurrency_tag;
#else
typedef CGAL::Sequential_tag Concurrency_tag;
#endif

// Domain
typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef K::FT FT;
typedef K::Point_3 Point;
typedef FT_to_point_function_wrapper<double, K::Point_3> Function;
typedef CGAL::Implicit_multi_domain_to_labeling_function_wrapper<Function>
        Function_wrapper;
typedef Function_wrapper::Function_vector Function_vector;
typedef CGAL::Labeled_mesh_domain_3<K> Mesh_domain;

// Triangulation
typedef CGAL::Mesh_triangulation_3<Mesh_domain,CGAL::Default,Concurrency_tag>::type Tr;
typedef CGAL::Mesh_complex_3_in_triangulation_3<Tr> C3t3;

// Mesh Criteria
typedef CGAL::Mesh_criteria_3<Tr> Mesh_criteria;
typedef Mesh_criteria::Facet_criteria    Facet_criteria;
typedef Mesh_criteria::Cell_criteria     Cell_criteria;


namespace cdt {
    // 定义数据结构
    struct Vertex {
        double x, y, z;
    };

    struct Edge {
        int start, end;
    };

    struct Face {
        std::vector<int> vertices; // 三个顶点，以counterclockwise顺序给出
        std::vector<int> edges; // 三条边
        int marker;  // 面对应的表面标记
        int leftVolumeNumber, rightVolumeNumber;  // 面左右两侧的体编号 ‐1 if empty (for boundary faces)
    };

    struct Zone {
        int zoneNumber;
        std::vector<int> volumes;
    };

    struct Tetrahedron {
        int vertices[4];
    };

    typedef std::tuple<int, int, int> Triangle; // 用三个顶点索引定义一个三角面

    // 计算两个顶点之间的向量
    Vertex subtract(const Vertex& a, const Vertex& b) {
        return {a.x - b.x, a.y - b.y, a.z - b.z};
    }

    // 计算叉乘，得到法向量
    Vertex crossProduct(const Vertex& a, const Vertex& b) {
        return {
                a.y * b.z - a.z * b.y,
                a.z * b.x - a.x * b.z,
                a.x * b.y - a.y * b.x
        };
    }

    // 归一化向量
    void normalize(Vertex& v) {
        double length = std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
        v.x /= length;
        v.y /= length;
        v.z /= length;
    }

/**
 * @brief Constrained tetrahedralization with Tetgen
 * @param input
 * @param output
 */

    void constrained_tetrahedralization_with_tetgen(const std::string& input, const std::string& output)
    {
        tetgenio in, out;

        // Step 1: 读取输入的 .poly 文件
        // TetGen 的 load_poly 应该接受 const char*，但为了安全，可以使用 char[]
        char input_file[256];
        strcpy(input_file, input.c_str());

        if (!in.load_poly(input_file)) {
            std::cerr << "Error: Failed to load input file: " << input << std::endl;
            return;
        }

        // Step 2: 执行受约束四面体剖分
        // "p" 表示从 .poly 文件生成，"C" 表示使用受约束 Delaunay 剖分
        tetgenbehavior behavior;
//        strncpy(behavior.outfilename, output.c_str(), sizeof(behavior.outfilename) - 1);
//        // 确保字符串以 null 字符结束
//        behavior.outfilename[sizeof(behavior.outfilename) - 1] = '\0';

        const char* s_const = "pkC";
        char* s = const_cast<char*>(s_const);
        behavior.parse_commandline(s);
        strncpy(behavior.outfilename, output.c_str(), sizeof(behavior.outfilename) - 1);
        // 确保字符串以 null 字符结束
        behavior.outfilename[sizeof(behavior.outfilename) - 1] = '\0';
        tetrahedralize(&behavior, &in, &out);

        // Step 3: 将结果保存到输出文件
        out.save_nodes((output).c_str());
        out.save_elements((output).c_str());
        out.save_faces((output).c_str());
        char output_file[256];
        strcpy(output_file, (output).c_str());
        out.save_edges(output_file);

        std::cout << "Tetrahedralization complete. Results saved to: " << output << std::endl;
    }

    /**
     * @brief 从隐函数中生成四面体网格
     * @param function 隐函数
     * @param bounding_sphere 包围球
     * @param output 输出文件，xxx.mesh
     */
    void tetrahedralization_by_implicit_function(const Function& function, const K::Sphere_3& bounding_sphere,
                                                 const std::string& output) {
        /// [Domain creation] (Warning: Sphere_3 constructor uses squared radius !)
        Mesh_domain domain = Mesh_domain::create_implicit_mesh_domain(function, bounding_sphere);

        /// [Domain creation]

        // Mesh criteria
        Mesh_criteria criteria(params::facet_angle(30).facet_size(0.1).facet_distance(0.025).
                cell_radius_edge_ratio(2).cell_size(0.1));

        // Mesh generation
        C3t3 c3t3 = CGAL::make_mesh_3<C3t3>(domain, criteria);

        // Output
        std::ofstream medit_file(output);
        CGAL::IO::write_MEDIT(medit_file, c3t3);
        medit_file.close();
    }

    /**
     * @brief 从多个隐函数中生成四面体网格
     * @param functions 隐函数集合
     * @param bounding_sphere 包围球
     * @param output 输出文件，xxx.mesh
     */
    void tetrahedralization_by_implicit_functions(const std::vector<Function>& functions, const K::Sphere_3& bounding_sphere,
                                                  const std::string& output) {
        Function_vector v;
        for(const auto& f : functions) {
            v.push_back(f);
        }
        // Domain (Warning: Sphere_3 constructor uses square radius !)
        Mesh_domain domain(Function_wrapper(v), bounding_sphere,
                           params::relative_error_bound(1e-6));

        // Set mesh criteria
        Facet_criteria facet_criteria(30, 0.2, 0.02); // angle, size, approximation
        Cell_criteria cell_criteria(2., 0.4); // radius-edge ratio, size
        Mesh_criteria criteria(facet_criteria, cell_criteria);

        // Mesh generation
        C3t3 c3t3 = CGAL::make_mesh_3<C3t3>(domain, criteria, params::no_exude().no_perturb());

        // MeshRefinement Perturbation (maximum cpu time: 10s, targeted dihedral angle: default)
        CGAL::perturb_mesh_3(c3t3, domain, params::time_limit(10));

        // remove sliver Exudation
        CGAL::exude_mesh_3(c3t3, params::time_limit(12));

        // Output
        std::ofstream medit_file(output);
        CGAL::IO::write_MEDIT(medit_file, c3t3);
    }

    /**
     * @brief 从隐函数和符号字符串中生成四面体网格
     * @param functions 隐函数集合
     * @param signs 符号字符串
     * @param bounding_sphere 包围球
     * @param output 输出文件，xxx.mesh
     */
    void tetrahedralization_by_implicit_sign_functions(const std::vector<Function>& functions, const std::string& signs, const K::Sphere_3& bounding_sphere,
                                                       const std::string& output) {
        Function_vector v;
        for(const auto& f : functions) {
            v.push_back(f);
        }
        std::vector<std::string> vps;
        vps.push_back(signs);
        Mesh_domain domain(Function_wrapper(v, vps), bounding_sphere, params::relative_error_bound(1e-6));

        // Set mesh criteria
        Facet_criteria facet_criteria(30, 0.2, 0.02); // angle, size, approximation
        Cell_criteria cell_criteria(2., 0.4); // radius-edge ratio, size
        Mesh_criteria criteria(facet_criteria, cell_criteria);

        // Mesh generation
        C3t3 c3t3 = CGAL::make_mesh_3<C3t3>(domain, criteria, params::no_exude().no_perturb());

        // Perturbation (maximum cpu time: 10s, targeted dihedral angle: default)
        CGAL::perturb_mesh_3(c3t3, domain, params::time_limit(10));

        // Exudation
        CGAL::exude_mesh_3(c3t3, params::time_limit(12));

        // Output
        std::ofstream medit_file(output);
        CGAL::IO::write_MEDIT(medit_file, c3t3);
        medit_file.close();
    }

    void tetrahedralization_by_hybrid_domain(const std::string& output) {

    }

    // 函数：将 .brep 文件转换为 .poly 文件
    void convert_brep_to_poly(const std::string& brep_file, const std::string& poly_file) {
        std::ifstream brep_in(brep_file);
        std::ofstream poly_out(poly_file);

        if (!brep_in.is_open()) {
            std::cerr << "Error opening .brep file: " << brep_file << std::endl;
            return;
        }

        if (!poly_out.is_open()) {
            std::cerr << "Error opening .poly file: " << poly_file << std::endl;
            return;
        }

        std::string line;

        // 读取操作系统代码，模型信息，唯一标识
        int os_code;
        std::string model_tag, unique_id;
        std::getline(brep_in, line);  // 读取操作系统代码
        std::istringstream(line) >> os_code;
        std::getline(brep_in, model_tag);  // 读取模型标签
        std::getline(brep_in, unique_id);  // 读取唯一标识

        // 读取计数和几何信息
        int numVertices, numEdges, numFaces, numVolumes, numZones;
        int numMarkers;
        int numHorizonMarkers;
        int numFaultMarkers;
        int numBoundarySurfaceMarkers;
        std::getline(brep_in, line);
        std::istringstream(line) >> numVertices >> numEdges >> numFaces >> numVolumes;
        std::getline(brep_in, line);
        // 读入marker的数量
        std::istringstream(line) >> numMarkers;
        // horizon的marker
        brep_in >> numHorizonMarkers;
        std::vector<int> horizonMarkers(numHorizonMarkers);
        for(int i = 0; i < numHorizonMarkers; i ++) {
            int marker;
            brep_in >> marker;
            horizonMarkers[i] = marker;
        }
        // fault的marker
        brep_in >> numFaultMarkers;
        std::vector<int> faultMarkers(numFaultMarkers);
        for(int i = 0; i < numFaultMarkers; i ++) {
            int marker;
            brep_in >> marker;
            faultMarkers[i] = marker;
        }
        // boundary surface的marker
        brep_in >> numBoundarySurfaceMarkers;
        std::vector<int> boundarySurfaceMarkers(numBoundarySurfaceMarkers);
        for(int i = 0; i < numBoundarySurfaceMarkers; i ++) {
            int marker;
            brep_in >> marker;
            boundarySurfaceMarkers[i] = marker;
        }

        // 读取顶点
        std::vector<Vertex> vertices(numVertices);
        for (int i = 0; i < numVertices; ++i) {
            brep_in >> vertices[i].x >> vertices[i].y >> vertices[i].z;
        }

        // 读取边
        std::vector<Edge> edges(numEdges);
        for (int i = 0; i < numEdges; ++i) {
            brep_in >> edges[i].start >> edges[i].end;
        }

        // 读取面
        std::vector<Face> faces(numFaces);
        for (int i = 0; i < numFaces; ++i) {
            faces[i].vertices.resize(3);
            for (int j = 0; j < 3; ++j) {
                brep_in >> faces[i].vertices[j];
            }
            faces[i].edges.resize(3);
            for (int j = 0; j < 3; ++j) {
                brep_in >> faces[i].edges[j];
            }
            brep_in >> faces[i].marker;  // 读取面的标记
            brep_in >> faces[i].leftVolumeNumber; // 读取面左侧的体编号
            brep_in >> faces[i].rightVolumeNumber; // 读取面右侧的体编号
        }

        // 读取区域信息
        brep_in >> numZones;  // 读取区域数量
        std::vector<Zone> zones(numZones);
        for (int i = 0; i < numZones; ++i) {
            int numVolumesInZone;
            brep_in >> zones[i].zoneNumber >> numVolumesInZone;
            zones[i].volumes.resize(numVolumesInZone);
            for (int j = 0; j < numVolumesInZone; ++j) {
                brep_in >> zones[i].volumes[j];
            }
        }

        // 写入 .poly 文件
        poly_out << "# .poly file generated from .brep\n";
        poly_out << "# Part 1 - node list" << "\n";
        poly_out << "# node count, 3 dim, no attribute, no boundary marker" << "\n";

        // 写入顶点部分
        poly_out << numVertices << " 3 0 0\n";  // 顶点数、维度、属性数、边界标志数
        for (int i = 0; i < numVertices; ++i) {
            poly_out << i + 1 << " " << vertices[i].x << " " << vertices[i].y << " " << vertices[i].z << "\n";
        }


        poly_out << "# Part 2 - facet list" << "\n";
        // 写入面部分
        poly_out << numFaces << " " <<  1 << "\n";  // 面的数量
        for (int i = 0; i < numFaces; ++i) {
            poly_out << 1 << "\n";
            poly_out << faces[i].vertices.size();  // 当前面的顶点数
            for (int vertex : faces[i].vertices) {
                poly_out << " " << vertex + 1;
            }
            if(faces[i].marker != 0) {
                poly_out << " " << faces[i].marker;
            }
            poly_out << "\n";
        }

        poly_out << "# Part 3 - the hole list." << "\n";
        poly_out << "# There is no hole in bar." << "\n";
        poly_out << "0" << "\n";
        poly_out << "# Part 4 - the region list" << "\n";
        poly_out << "0" << "\n";
        // 写入区域部分
//        poly_out << numZones << "\n";  // 区域数量
//        for (const auto& zone : zones) {
//            poly_out << zone.zoneNumber << " " << zone.volumes.size();
//            for (int volume : zone.volumes) {
//                poly_out << " " << volume;
//            }
//            poly_out << "\n";
//        }

        // 保存并关闭文件
        poly_out.close();
        brep_in.close();

        std::cout << "Conversion complete: " << poly_file << " created from " << brep_file << std::endl;
    }

    bool check_ply(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error opening file: " << filename << std::endl;
            return false;
        }

        std::string line;
        int vertex_count = 0;
        int face_count = 0;
        bool in_vertex_section = false;
        bool in_face_section = false;



        // 读取顶点数量和面数量
        while (std::getline(file, line)) {
            if (line.find("element vertex") != std::string::npos) {
                sscanf(line.c_str(), "element vertex %d", &vertex_count);
            } else if (line.find("element face") != std::string::npos) {
                sscanf(line.c_str(), "element face %d", &face_count);
                break;  // 找到面定义后跳出循环，因为我们需要从这里开始检查
            }
        }

        // 现在跳过到面数据开始的部分
        while (std::getline(file, line) && line != "end_header") {}

        // 跳过顶点数据
        for (int i = 0; i < vertex_count && std::getline(file, line); ++i) {
            // 仅读取行，不处理内容，因为此处目的是跳过顶点数据
        }

        // 检查每个面的顶点索引
        int line_count = 0;
        while (std::getline(file, line) && line_count < face_count) {
            int num_vertices, v1, v2, v3;
            if (sscanf(line.c_str(), "3 %d %d %d", &v1, &v2, &v3) == 3) {
                if (v1 < 0 || v1 >= vertex_count || v2 < 0 || v2 >= vertex_count || v3 < 0 || v3 >= vertex_count) {
                    std::cerr << "Error: Invalid vertex index in face at line " << line_count + 1 << std::endl;
                    std::cerr << line << std::endl;
                    return false;
                }
            } else {
                std::cerr << "Error: Face line format incorrect at line " << line_count + 1 << std::endl;
                std::cerr << line << std::endl;
                return false;
            }
            line_count++;
        }

        std::cout << "PLY file check passed." << std::endl;
        return true;
    }

    // 函数：将 .node 和 .ele 文件转换为 .ply 文件，便于可视化
    void convertNodeEleToPLY(const std::string& nodeFilename, const std::string& eleFilename, const std::string& plyFilename) {
        std::ifstream nodeFile(nodeFilename);
        std::ifstream eleFile(eleFilename);
        std::ofstream plyFile(plyFilename);

        if (!nodeFile.is_open() || !eleFile.is_open() || !plyFile.is_open()) {
            std::cerr << "Error opening files!" << std::endl;
            return;
        }

        // Read vertices
        int numVertices, numDims, numAttribs, numBoundaryMarkers;
        nodeFile >> numVertices >> numDims >> numAttribs >> numBoundaryMarkers;

        std::vector<Vertex> vertices(numVertices);
        for (int i = 0; i < numVertices; ++i) {
            int x;
            nodeFile >> x >> vertices[i].x >> vertices[i].y >> vertices[i].z;
            // Skip additional attributes or boundary markers if present
            for (int j = 0; j < numAttribs + numBoundaryMarkers; ++j) {
                float dummy;
                nodeFile >> dummy;
            }
        }

        // Read tetrahedrons
        int numTetrahedrons, dim, attributes;
        eleFile >> numTetrahedrons >> dim >> attributes;  // Assuming nodesPerTetrahedron is 4
        std::vector<Tetrahedron> tetrahedrons(numTetrahedrons);
        for (int i = 0; i < numTetrahedrons; ++i) {
            int id;
            eleFile >> id;
            for (int j = 0; j < dim; ++j) {
                eleFile >> tetrahedrons[i].vertices[j];
            }
        }

        // 用于按顺序比较面，确保可以正确地使用set去重
        struct CompareTriangle {
            bool operator()(const Triangle& a, const Triangle& b) const {
                std::array<int, 3> sorted_a{std::get<0>(a), std::get<1>(a), std::get<2>(a)};
                std::array<int, 3> sorted_b{std::get<0>(b), std::get<1>(b), std::get<2>(b)};
                std::sort(sorted_a.begin(), sorted_a.end());
                std::sort(sorted_b.begin(), sorted_b.end());
                return sorted_a < sorted_b;
            }
        };

        std::set<Triangle, CompareTriangle> unique_faces;

        // 提取所有四面体的外表面
        for (const auto& tet : tetrahedrons) {
            int idx[4] = {tet.vertices[0] - 1, tet.vertices[1] - 1, tet.vertices[2] - 1, tet.vertices[3] - 1};

            // 对于四面体的每个面，将顶点索引排序并添加到集合中以去重
            for (int i = 0; i < 4; ++i) {
                Triangle face(idx[i], idx[(i+1)%4], idx[(i+2)%4]);
                if (unique_faces.find(face) == unique_faces.end()) {
                    unique_faces.insert(face);
                }
            }
        }

        // Write to PLY file
        plyFile << "ply\n";
        plyFile << "format ascii 1.0\n";
        plyFile << "element vertex " << vertices.size() << "\n";
        plyFile << "property float x\n";
        plyFile << "property float y\n";
        plyFile << "property float z\n";
        plyFile << "element face " << unique_faces.size() << "\n";
        plyFile << "property list uchar int vertex_index\n";
        plyFile << "end_header\n";

        // 输出顶点
        for (const auto& v : vertices) {
            plyFile << v.x << " " << v.y << " " << v.z << "\n";
        }

        // 输出面
        for (const auto& f : unique_faces) {
            plyFile << "3 " << std::get<0>(f) << " " << std::get<1>(f) << " " << std::get<2>(f) << "\n";
        }

        plyFile.close();
        nodeFile.close();
        eleFile.close();

        check_ply(plyFilename);
    }

    /**
     * @brief 保存网格到文件
     * @param vertices
     * @param faces
     * @param filename
     * @param formatId
     */
    void saveMeshToFile(const std::vector<Vertex>& vertices, const std::vector<Triangle>& faces, const std::string& filename, const std::string& formatId) {
        Assimp::Exporter exporter;
        aiScene scene;
        scene.mRootNode = new aiNode();
        scene.mMaterials = new aiMaterial*[1];
        scene.mMaterials[0] = new aiMaterial();
        scene.mNumMaterials = 1;
        scene.mMeshes = new aiMesh*[1];
        scene.mMeshes[0] = new aiMesh();
        scene.mNumMeshes = 1;
        scene.mRootNode->mMeshes = new unsigned int[1];
        scene.mRootNode->mMeshes[0] = 0;
        scene.mRootNode->mNumMeshes = 1;

        aiMesh* mesh = scene.mMeshes[0];
        mesh->mMaterialIndex = 0;
        mesh->mVertices = new aiVector3D[vertices.size()];
        mesh->mNumVertices = vertices.size();
        mesh->mFaces = new aiFace[faces.size()];
        mesh->mNumFaces = faces.size();
        mesh->mNormals = new aiVector3D[vertices.size()]; // 分配法向量数组

        for (size_t i = 0; i < vertices.size(); ++i) {
            mesh->mVertices[i] = aiVector3D(vertices[i].x, vertices[i].y, vertices[i].z);
        }

        for (size_t i = 0; i < faces.size(); ++i) {
            aiFace& face = mesh->mFaces[i];
            face.mIndices = new unsigned int[3];
            face.mNumIndices = 3;
            face.mIndices[0] = std::get<0>(faces[i]);
            face.mIndices[1] = std::get<1>(faces[i]);
            face.mIndices[2] = std::get<2>(faces[i]);

            Vertex v1 = vertices[face.mIndices[0]];
            Vertex v2 = vertices[face.mIndices[1]];
            Vertex v3 = vertices[face.mIndices[2]];
            Vertex edge1 = subtract(v2, v1);
            Vertex edge2 = subtract(v3, v1);
            Vertex normal = crossProduct(edge1, edge2);
            normalize(normal);

            // 将计算出的法向量分配给面的三个顶点
            mesh->mNormals[face.mIndices[0]] = aiVector3D(normal.x, normal.y, normal.z);
            mesh->mNormals[face.mIndices[1]] = aiVector3D(normal.x, normal.y, normal.z);
            mesh->mNormals[face.mIndices[2]] = aiVector3D(normal.x, normal.y, normal.z);
        }

        aiReturn result = exporter.Export(&scene, formatId, filename);
        if (result != aiReturn_SUCCESS) {
            std::cerr << "Assimp error while exporting: " << exporter.GetErrorString() << std::endl;
        }

        // Cleanup
        delete[] scene.mRootNode->mMeshes;
        delete scene.mRootNode;
        for (unsigned int i = 0; i < scene.mNumMeshes; ++i) {
            delete[] scene.mMeshes[i]->mVertices;
            delete[] scene.mMeshes[i]->mNormals;
            for (unsigned int j = 0; j < scene.mMeshes[i]->mNumFaces; ++j) {
                delete[] scene.mMeshes[i]->mFaces[j].mIndices;
            }
            delete[] scene.mMeshes[i]->mFaces;
            delete scene.mMeshes[i];
        }
        delete[] scene.mMeshes;
        delete[] scene.mMaterials[0];
        delete[] scene.mMaterials;
    }
}



#endif //POISSONRECONCGAL_CONSTRAINED_TETRAHEDRALIZATION_H
