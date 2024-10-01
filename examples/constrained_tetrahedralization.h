//
// Created by RainSure on 24-10-1.
//

#ifndef POISSONRECONCGAL_CONSTRAINED_TETRAHEDRALIZATION_H
#define POISSONRECONCGAL_CONSTRAINED_TETRAHEDRALIZATION_H

#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include "tetgen.h"
/**
 * @brief Constrained tetrahedralization with Tetgen
 * @param input
 * @param output
 */

// 定义数据结构
struct Vertex {
    double x, y, z;
};

struct Edge {
    int start, end;
};

struct Face {
    std::vector<int> vertices;
};

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
    behavior.parse_commandline("pC");
    tetrahedralize(&behavior, &in, &out);

    // Step 3: 将结果保存到输出文件
    out.save_nodes((output + ".node").c_str());
    out.save_elements((output + ".ele").c_str());
    out.save_faces((output + ".face").c_str());
    char output_file[256];
    strcpy(output_file, (output + ".edge").c_str());
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

    // 读取 .brep 文件中的信息
    int numVertices, numEdges, numFaces;
    brep_in >> numVertices >> numEdges >> numFaces;

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
        int numFaceVertices;
        brep_in >> numFaceVertices;
        faces[i].vertices.resize(numFaceVertices);
        for (int j = 0; j < numFaceVertices; ++j) {
            brep_in >> faces[i].vertices[j];
        }
    }

    // 开始写入 .poly 文件
    poly_out << "# .poly file generated from .brep\n";

    // 写入顶点部分
    poly_out << numVertices << " 3 0 0\n";  // 顶点数、维度、属性数、边界标志数
    for (int i = 0; i < numVertices; ++i) {
        poly_out << i + 1 << " " << vertices[i].x << " " << vertices[i].y << " " << vertices[i].z << "\n";
    }

    // 写入边部分
    poly_out << numEdges << " 0\n";  // 边数、边界标志数
    for (int i = 0; i < numEdges; ++i) {
        poly_out << i + 1 << " " << edges[i].start << " " << edges[i].end << "\n";
    }

    // 写入面部分
    poly_out << numFaces << "\n";  // 面的数量
    for (int i = 0; i < numFaces; ++i) {
        poly_out << faces[i].vertices.size();  // 当前面的顶点数
        for (int j = 0; j < faces[i].vertices.size(); ++j) {
            poly_out << " " << faces[i].vertices[j];
        }
        poly_out << "\n";
    }

    // 保存完成，关闭文件
    poly_out.close();
    brep_in.close();

    std::cout << "Conversion complete: " << poly_file << " created from " << brep_file << std::endl;
}

#endif //POISSONRECONCGAL_CONSTRAINED_TETRAHEDRALIZATION_H
