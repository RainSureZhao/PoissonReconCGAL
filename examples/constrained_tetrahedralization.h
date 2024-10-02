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
#include "tetgen.h"

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
        const char* s_const = "pC";
        char* s = const_cast<char*>(s_const);
        behavior.parse_commandline(s);
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
        poly_out << numFaces << " " <<  0 << "\n";  // 面的数量
        for (int i = 0; i < numFaces; ++i) {
            poly_out << 1 << "\n";
            poly_out << faces[i].vertices.size();  // 当前面的顶点数
            for (int vertex : faces[i].vertices) {
                poly_out << " " << vertex + 1;
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



        typedef std::tuple<int, int, int> Triangle; // 用三个顶点索引定义一个三角面

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


}



#endif //POISSONRECONCGAL_CONSTRAINED_TETRAHEDRALIZATION_H
