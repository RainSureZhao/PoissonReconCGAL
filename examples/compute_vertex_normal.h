//
// Created by RainSure on 24-8-23.
//

#ifndef POISSONRECONCGAL_COMPUTE_VERTEX_NORMAL_H
#define POISSONRECONCGAL_COMPUTE_VERTEX_NORMAL_H

#include <iostream>
#include <vector>
#include <map>
#include <cmath>
#include <fstream>
#include <sstream>
#include <string>
#include "geometry/vector3.h"


typedef std::vector<Vector3> Vertices;
typedef std::map<Vector3, std::pair<Vector3, double>> NormalMapWithArea;

// 计算单个三角形的面法向量及面积
std::pair<Vector3, double> compute_face_normal_and_area(const Vector3& p1, const Vector3& p2, const Vector3& p3) {
    Vector3 u = p2 - p1;
    Vector3 v = p3 - p1;
    Vector3 normal = u.cross(v);
    double area = 0.5 * normal.magnitude();
    return {normal.normalize(), area};
}

// 计算所有顶点的法向量
void compute_vertex_normals(const Vertices& vertices, NormalMapWithArea& normals) {
    for (size_t i = 0; i < vertices.size(); i += 3) {
        if (i + 2 >= vertices.size()) break; // 防止数组越界

        Vector3 p1 = vertices[i];
        Vector3 p2 = vertices[i + 1];
        Vector3 p3 = vertices[i + 2];

        auto [face_normal, area] = compute_face_normal_and_area(p1, p2, p3);

        normals[p1].first = normals[p1].first + face_normal * area;
        normals[p1].second += area;

        normals[p2].first = normals[p2].first + face_normal * area;
        normals[p2].second += area;

        normals[p3].first = normals[p3].first + face_normal * area;
        normals[p3].second += area;
    }

    // 归一化
    for (auto& pair : normals) {
        if (pair.second.second != 0) {
            pair.second.first = (pair.second.first / pair.second.second).normalize();
        }
    }
}

void process_mesh(const std::string& input_path, const std::string& output_path) {
    std::ifstream input_file(input_path);
    std::ofstream output_file(output_path);

    Vertices vertices;
    Vector3 point;
    double x, y, z;

    // 读取点
    std::string line;
    while (getline(input_file, line)) {
        std::istringstream iss(line);
        if (iss >> x >> y >> z) {
            vertices.emplace_back(x, y, z);
        }
    }

    // 计算法向量
    NormalMapWithArea normals;
    compute_vertex_normals(vertices, normals);

    // 写入点和法向量
    for (const auto& pair : normals) {
        output_file << pair.first.x << " " << pair.first.y << " " << pair.first.z << " "
                    << pair.second.first.x << " " << pair.second.first.y << " " << pair.second.first.z << std::endl;
    }
}

#endif //POISSONRECONCGAL_COMPUTE_VERTEX_NORMAL_H
